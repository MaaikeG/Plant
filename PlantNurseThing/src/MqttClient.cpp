#include "MqttClient.h"

bool shouldSaveConfig;

MqttClient::MqttClient(WiFiClient* _wiFiClient, WiFiManager* _wiFiManager)
    : testfeed(adafruitClient, "test", MQTT_QOS_1) {
  wiFiClient = _wiFiClient;
  wiFiManager = _wiFiManager;
}

void saveConfigCallback() {
  Serial.println("Should save config");
  // set flag because saving to SPIFF takes too long
  shouldSaveConfig = true;
}

void MqttClient::addParameters() {
  // clean FS, for testing
  // SPIFFS.format();

  Serial.println("mounting FS...");
  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      // file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
          Serial.println("\nparsed json");

          strcpy(mqttServer, json["mqttServer"]);
          mqttPort = json["mqtt_port"];
          strcpy(mqttClientId, json["mqttClientId"]);
          strcpy(mqttUsername, json["mqttUsername"]);
          strcpy(mqttPassword, json["mqttPassword"]);
        } else {
          Serial.println("failed to load json config");
        }
        configFile.close();
      }
    }
  } else {
    Serial.println("failed to mount FS");
  }
  // end read

  mqttServerParameter = new WiFiManagerParameter("server", "server address", mqttServer, MQTT_SERVER_LENGTH);
  mqtPortParameter = new WiFiManagerParameter("port", "port", String(mqttPort).c_str(), MQTT_PORT_LENGTH);
  mqttClientIdParameter = new WiFiManagerParameter("mqttClientId", "client id", mqttClientId, MQTT_CLIENT_ID_LENGTH);
  mqttUsernameParameter = new WiFiManagerParameter("username", "username", mqttUsername, MQTT_USERNAME_LENGTH);
  mqttPasswordParameter = new WiFiManagerParameter("password", "password", mqttPassword, MQTT_PASSWORD_LENGTH);

  wiFiManager->setSaveConfigCallback(saveConfigCallback);

  wiFiManager->addParameter(mqttServerParameter);
  wiFiManager->addParameter(mqtPortParameter);
  wiFiManager->addParameter(mqttClientIdParameter);
  wiFiManager->addParameter(mqttUsernameParameter);
  wiFiManager->addParameter(mqttPasswordParameter);
}

void MqttClient::saveParameters() {
  strcpy(mqttServer, mqttServerParameter->getValue());
  mqttPort = String(mqtPortParameter->getValue()).toInt();
  strcpy(mqttClientId, mqttClientIdParameter->getValue());
  strcpy(mqttUsername, mqttUsernameParameter->getValue());
  strcpy(mqttPassword, mqttPasswordParameter->getValue());

  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["mqttServer"] = mqttServer;
    json["mqtt_port"] = mqttPort;
    json["mqttUsername"] = mqttUsername;
    json["mqttPassword"] = mqttPassword;
    json["mqttClientId"] = mqttClientId;

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }

    json.printTo(Serial);
    Serial.println();
    json.printTo(configFile);
    configFile.close();
  }
  // end save
}

void testCallback(char* data, uint16_t len) {
  Serial.print("test: ");
  Serial.println(data);
}

void MqttClient::begin() {
  adafruitClient = new Adafruit_MQTT_Client(wiFiClient, &mqttServer[0],
                                            mqttPort, &mqttClientId[0],
                                            &mqttUsername[0], &mqttPassword[0]);

  testfeed.setCallback(testCallback);

  // Setup MQTT subscription for time feed.
  adafruitClient->subscribe(&testfeed);
}

void MqttClient::update(uint16_t timeLeft) {
  MqttConnect();
  adafruitClient->processPackets(timeLeft);
  if (millis() - lastPingMillis > PING_FREQUENCY) {
    lastPingMillis = millis();
    if (!adafruitClient->ping()) {
      adafruitClient->disconnect();
    }
  }
}

void MqttClient::MqttConnect() {
  int8_t ret;

  // Stop if already connected.
  if (adafruitClient->connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = adafruitClient->connect()) !=
         0) {  // connect will return 0 for connected
    Serial.println(adafruitClient->connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 10 seconds...");
    adafruitClient->disconnect();
    delay(10000);  // wait 10 seconds
    retries--;
    if (retries == 0) {
      wiFiManager->resetSettings();
      delay(50);  // make sure the settings are really reset or something
      // basically die and wait for WDT to reset me
      while (1)
        ;
    }
  }
  Serial.println("MQTT Connected!");
}
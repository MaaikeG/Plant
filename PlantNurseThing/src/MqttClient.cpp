#include "MqttClient.h"

bool shouldSaveConfig;

MqttClient::MqttClient(WiFiClient* _wiFiClient, WiFiManager* _wiFiManager)
    : testfeed(adafruitClient, "test", MQTT_QOS_1),
      mqttServer_parameter("server", "server address", mqttServer, 40),
      mqtt_port_parameter("port", "port", String(mqttPort).c_str(), 5),
      mqttClientId_parameter("mqttClientId", "client id", mqttClientId,
                               20),
      mqttUsername_parameter("username", "username", mqttUsername, 20),
      mqttPassword_parameter("password", "password", mqttPassword, 20) {
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

  wiFiManager->setSaveConfigCallback(saveConfigCallback);

  wiFiManager->addParameter(&mqttServer_parameter);
  wiFiManager->addParameter(&mqtt_port_parameter);
  wiFiManager->addParameter(&mqttClientId_parameter);
  wiFiManager->addParameter(&mqttUsername_parameter);
  wiFiManager->addParameter(&mqttPassword_parameter);
}

void MqttClient::saveParameters() {
  strcpy(mqttServer, mqttServer_parameter.getValue());
  mqttPort = String(mqtt_port_parameter.getValue()).toInt();
  strcpy(mqttClientId, mqttClientId_parameter.getValue());
  strcpy(mqttUsername, mqttUsername_parameter.getValue());
  strcpy(mqttPassword, mqttPassword_parameter.getValue());

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
  Serial.println(mqttServer_parameter.getValue());
  Serial.println(mqttPort);
  Serial.println(mqttClientId_parameter.getValue());
  Serial.println(mqttUsername_parameter.getValue());
  Serial.println(mqttPassword_parameter.getValue());
  // char buf[24];
  // String(mqttServer_parameter.getValue()).toCharArray(&buf[0], 24);
  // strcpy(buf, mqttServer_parameter.getValue());
  adafruitClient =
      new Adafruit_MQTT_Client(wiFiClient, &mqttServer[0],
                               //"m23.cloudmqtt.com"
                               // mqttServer_parameter.getValue(),
                               // 28205,
                               mqttPort, &mqttClientId[0],
                               //"NodeMcu Colin",
                               // mqttClientId_parameter.getValue(),
                               &mqttUsername[0],
                               //"ovzlnxsu",
                               // mqttUsername_parameter.getValue(),
                               &mqttPassword[0]
                               //"PmPxdzp_9BHq"
                               // mqttPassword_parameter.getValue()
      );

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
      delay(50); // make sure the settings are really reset or something
      // basically die and wait for WDT to reset me
      while (1)
        ;
    }
  }
  Serial.println("MQTT Connected!");
}
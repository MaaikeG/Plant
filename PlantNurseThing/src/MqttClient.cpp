#include "MqttClient.h"

bool shouldSaveConfig;

MqttClient::MqttClient(WiFiClient* _wifiClient, WiFiManager* _wifiManager)
    : testfeed(adafruitClient, "test", MQTT_QOS_1),
      mqtt_server_parameter("server", "server address", mqtt_server, 40),
      mqtt_port_parameter("port", "port", String(mqttPort).c_str(), 5),
      mqtt_client_id_parameter("mqtt_client_id", "client id", mqtt_client_id,
                               20),
      mqtt_username_parameter("username", "username", mqtt_username, 20),
      mqtt_password_parameter("password", "password", mqtt_password, 20) {
  wifiClient = _wifiClient;
  wifiManager = _wifiManager;
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

          strcpy(mqtt_server, json["mqtt_server"]);
          mqttPort = json["mqtt_port"];
          strcpy(mqtt_client_id, json["mqtt_client_id"]);
          strcpy(mqtt_username, json["mqtt_username"]);
          strcpy(mqtt_password, json["mqtt_password"]);
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

  wifiManager->setSaveConfigCallback(saveConfigCallback);

  wifiManager->addParameter(&mqtt_server_parameter);
  wifiManager->addParameter(&mqtt_port_parameter);
  wifiManager->addParameter(&mqtt_client_id_parameter);
  wifiManager->addParameter(&mqtt_username_parameter);
  wifiManager->addParameter(&mqtt_password_parameter);
}

void MqttClient::saveParameters() {
  strcpy(mqtt_server, mqtt_server_parameter.getValue());
  mqttPort = String(mqtt_port_parameter.getValue()).toInt();
  strcpy(mqtt_client_id, mqtt_client_id_parameter.getValue());
  strcpy(mqtt_username, mqtt_username_parameter.getValue());
  strcpy(mqtt_password, mqtt_password_parameter.getValue());

  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["mqtt_server"] = mqtt_server;
    json["mqtt_port"] = mqttPort;
    json["mqtt_username"] = mqtt_username;
    json["mqtt_password"] = mqtt_password;
    json["mqtt_client_id"] = mqtt_client_id;

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
  Serial.println(mqtt_server_parameter.getValue());
  Serial.println(mqttPort);
  Serial.println(mqtt_client_id_parameter.getValue());
  Serial.println(mqtt_username_parameter.getValue());
  Serial.println(mqtt_password_parameter.getValue());
  // char buf[24];
  // String(mqtt_server_parameter.getValue()).toCharArray(&buf[0], 24);
  // strcpy(buf, mqtt_server_parameter.getValue());
  adafruitClient =
      new Adafruit_MQTT_Client(wifiClient, &mqtt_server[0],
                               //"m23.cloudmqtt.com"
                               // mqtt_server_parameter.getValue(),
                               // 28205,
                               mqttPort, &mqtt_client_id[0],
                               //"NodeMcu Colin",
                               // mqtt_client_id_parameter.getValue(),
                               &mqtt_username[0],
                               //"ovzlnxsu",
                               // mqtt_username_parameter.getValue(),
                               &mqtt_password[0]
                               //"PmPxdzp_9BHq"
                               // mqtt_password_parameter.getValue()
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
      wifiManager->resetSettings();
      delay(50); // make sure the settings are really reset or something
      // basically die and wait for WDT to reset me
      while (1)
        ;
    }
  }
  Serial.println("MQTT Connected!");
}
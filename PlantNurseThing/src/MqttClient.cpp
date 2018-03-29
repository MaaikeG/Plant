#include "MqttClient.h"

bool shouldSaveConfig;

MqttClient::MqttClient(WiFiClient& _wiFiClient, WiFiManager* _wiFiManager)
    : wiFiClient(_wiFiClient), pubSubClient(_wiFiClient) {
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

  mqttServerParameter = new WiFiManagerParameter(
      "server", "server address", mqttServer, MQTT_SERVER_LENGTH);
  mqtPortParameter = new WiFiManagerParameter(
      "port", "port", String(mqttPort).c_str(), MQTT_PORT_LENGTH);
  mqttClientIdParameter = new WiFiManagerParameter(
      "mqttClientId", "client id", mqttClientId, MQTT_CLIENT_ID_LENGTH);
  mqttUsernameParameter = new WiFiManagerParameter(
      "username", "username", mqttUsername, MQTT_USERNAME_LENGTH);
  mqttPasswordParameter = new WiFiManagerParameter(
      "password", "password", mqttPassword, MQTT_PASSWORD_LENGTH);

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

void MqttClient::begin() {
  pubSubClient.setServer(mqttServer, mqttPort);
}

void MqttClient::update() {
  // Loop until we're reconnected
  while (!pubSubClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (pubSubClient.connect(mqttClientId, mqttUsername, mqttPassword)) {
      Serial.println("connected");
      pubSubClient.subscribe("test", 1);
    } else {
      Serial.print("failed, rc=");
      Serial.print(pubSubClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
  pubSubClient.loop();
}
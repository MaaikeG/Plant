#ifndef _MQTT_CLIENT_H
#define _MQTT_CLIENT_H

// clang-format off
#if defined(ARDUINO) && ARDUINO >= 100
  #include "arduino.h"
#else
  #include "WProgram.h"
#endif
// clang-format on

#include <ESP8266WiFi.h>
#include <FS.h>
#include <ArduinoJson.h>
#include "WiFiManager.h"
#include <PubSubClient.h>

#define MQTT_SERVER_LENGTH 40
#define MQTT_PORT_LENGTH 5
#define MQTT_CLIENT_ID_LENGTH 20
#define MQTT_USERNAME_LENGTH 20
#define MQTT_PASSWORD_LENGTH 20

#define PING_FREQUENCY 25000

class MqttClient {
 private:
  WiFiClient &wiFiClient;
  WiFiManager* wiFiManager;
  unsigned long lastPingMillis;
  char mqttServer[MQTT_SERVER_LENGTH];
  int mqttPort = 8080;
  char mqttClientId[MQTT_CLIENT_ID_LENGTH];
  char mqttUsername[MQTT_USERNAME_LENGTH];
  char mqttPassword[MQTT_PASSWORD_LENGTH];
  WiFiManagerParameter* mqttServerParameter;
  WiFiManagerParameter* mqtPortParameter;
  WiFiManagerParameter* mqttClientIdParameter;
  WiFiManagerParameter* mqttUsernameParameter;
  WiFiManagerParameter* mqttPasswordParameter;

 public:
  MqttClient(WiFiClient& _wiFiClient, WiFiManager* _wiFiManager);
  PubSubClient pubSubClient;
  void addParameters();
  void saveParameters();
  void begin();
  void update();
};

#endif
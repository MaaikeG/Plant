#ifndef _MQTT_CLIENT_h
#define _MQTT_CLIENT_h

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
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define MQTT_SERVER_LENGTH 40
#define MQTT_CLIENT_ID_LENGTH 20
#define MQTT_USERNAME_LENGTH 20
#define MQTT_PASSWORD_LENGTH 20

#define PING_FREQUENCY 25000

class MqttClient {
 private:
  WiFiClient* wiFiClient;
  WiFiManager* wiFiManager;
  void MqttConnect();
  Adafruit_MQTT_Client* adafruitClient;
  Adafruit_MQTT_Subscribe testfeed;
  unsigned long lastPingMillis;
  char mqttServer[MQTT_SERVER_LENGTH];
  int mqttPort = 8080;
  char mqttUsername[MQTT_USERNAME_LENGTH];
  char mqttClientId[MQTT_CLIENT_ID_LENGTH];
  char mqttPassword[MQTT_PASSWORD_LENGTH];
  WiFiManagerParameter mqttServer_parameter;
  WiFiManagerParameter mqtt_port_parameter;
  WiFiManagerParameter mqttClientId_parameter;
  WiFiManagerParameter mqttUsername_parameter;
  WiFiManagerParameter mqttPassword_parameter;

 public:
  MqttClient(WiFiClient* _wiFiClient, WiFiManager* _wiFiManager);
  void addParameters();
  void saveParameters();
  void begin();
  void update(uint16_t timeLeft);
};

#endif
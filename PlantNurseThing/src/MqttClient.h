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
  WiFiClient* wifiClient;
  WiFiManager* wifiManager;
  void MqttConnect();
  Adafruit_MQTT_Client* adafruitClient;
  Adafruit_MQTT_Subscribe testfeed;
  unsigned long lastPingMillis;
  char mqtt_server[MQTT_SERVER_LENGTH];
  int mqttPort = 8080;
  char mqtt_username[MQTT_USERNAME_LENGTH];
  char mqtt_client_id[MQTT_CLIENT_ID_LENGTH];
  char mqtt_password[MQTT_PASSWORD_LENGTH];
  WiFiManagerParameter mqtt_server_parameter;
  WiFiManagerParameter mqtt_port_parameter;
  WiFiManagerParameter mqtt_client_id_parameter;
  WiFiManagerParameter mqtt_username_parameter;
  WiFiManagerParameter mqtt_password_parameter;

 public:
  MqttClient(WiFiClient* _wifiClient, WiFiManager* _wifiManager);
  void addParameters();
  void saveParameters();
  void begin();
  void update(uint16_t timeLeft);
};

#endif
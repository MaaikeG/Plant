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
#include <Ticker.h>
#include "SensorsController.h"
#include <list>
#include <utility>
#include <stdlib.h>

#define MQTT_SERVER_LENGTH 40
#define MQTT_PORT_LENGTH 6
#define MQTT_CLIENT_ID_LENGTH 20
#define MQTT_USERNAME_LENGTH 20
#define MQTT_PASSWORD_LENGTH 20

#define TOPIC_PREFIX "plantNurse/"
#define TEST_TOPIC TOPIC_PREFIX "test"
#define TEMPERATURE_TOPIC TOPIC_PREFIX "temperature"
#define HUMIDITY_TOPIC TOPIC_PREFIX "humidity"
#define PRESSURE_TOPIC TOPIC_PREFIX "pressure"
#define SOIL_MOISTURE_TOPIC TOPIC_PREFIX "soilMoisture"
#define LIGHT_TOPIC TOPIC_PREFIX "light"
#define WATERING_TOPIC TOPIC_PREFIX "waterNow"
#define MODE_TOGGLE_TOPIC TOPIC_PREFIX "mode"
#define STATUS_TOPIC TOPIC_PREFIX "status"
#define UPDATE_SENSOR_TOPIC TOPIC_PREFIX "updateSensorsNow"
#define WATER_RESERVOIR_EMPTYNESS_TOPIC TOPIC_PREFIX "waterReservoirEmptyness"

class MqttClient : public PubSubClient {
 private:
  WiFiClient& wiFiClient;
  WiFiManager& wiFiManager;
  unsigned long lastPingMillis;
  WiFiManagerParameter* mqttServerParameter;
  WiFiManagerParameter* mqttPortParameter;
  WiFiManagerParameter* mqttClientIdParameter;
  WiFiManagerParameter* mqttUsernameParameter;
  WiFiManagerParameter* mqttPasswordParameter;
  Ticker reconnectTicker;
  bool tickerAttached;
  char mqttServer[MQTT_SERVER_LENGTH];
  int mqttPort = 8080;
  char mqttClientId[MQTT_CLIENT_ID_LENGTH];
  char mqttUsername[MQTT_USERNAME_LENGTH];
  char mqttPassword[MQTT_PASSWORD_LENGTH];
  std::list<std::pair<const char*, uint8_t>> subscriptions;


 public:
  MqttClient(WiFiClient& _wiFiClient, WiFiManager& _wiFiManager);
  void addParameters();
  void saveParameters();
  void begin();
  void badParametersReset();
  void update();
  void connect();
  void publishSensorValues(SensorsController& sensorsController);
  boolean subscribe(const char* topic);
  boolean subscribe(const char* topic, uint8_t qos);
};

#endif
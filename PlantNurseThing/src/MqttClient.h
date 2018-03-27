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
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "Credentials.h"

#define PING_FREQUENCY 25000

class MqttClient {
 private:
  void MqttConnect();
  Adafruit_MQTT_Client adafruitClient;
  Adafruit_MQTT_Subscribe testfeed;
  unsigned long lastPingMillis;

 public:
  MqttClient(Client *client);
  void begin();
  void update();
};

#endif
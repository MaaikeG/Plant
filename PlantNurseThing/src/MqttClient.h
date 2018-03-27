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

class MqttClient {
 private:
  void MqttConnect();
  Adafruit_MQTT_Client adafruitClient;
  Adafruit_MQTT_Subscribe testfeed;

 public:
  MqttClient(Client *client);
  void begin();
  void update();
};

#endif
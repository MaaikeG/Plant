#ifndef _MANAGED_WIFI_CLIENT_H
#define _MANAGED_WIFI_CLIENT_H

// clang-format off
#if defined(ARDUINO) && ARDUINO >= 100
  #include "arduino.h"
#else
  #include "WProgram.h"
#endif
// clang-format on

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <SSD1306.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "WiFiManager.h"
#include <SSD1306.h>

#define AP_SSID "Planet Nurse Access Point"
#define AP_PASSWORD "plantNurse"

typedef void (*ConfigModeCallback)(WiFiManager*);

void configModeCallback(WiFiManager* wiFiManager, SSD1306* oled);

class ManagedWiFiClient {
 private:
  ConfigModeCallback configModeCallback;

 public:
  WiFiClientSecure client;
  ManagedWiFiClient(ConfigModeCallback _configModeCallback);
  void begin(WiFiManager* wiFiManager);
};

#endif
#include "ManagedWifiClient.h"

ManagedWifiClient::ManagedWifiClient(ConfigModeCallback _configModeCallback) {
  configModeCallback = _configModeCallback;
}

void ManagedWifiClient::begin() {
  WiFiManager wifiManager;
  wifiManager.setAPCallback(configModeCallback);
  if (!wifiManager.autoConnect(APSsid, APPassword)) {
    Serial.println("failed to connect and hit timeout");
    // reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }

  Serial.println("WiFi connected");
}
#include "ManagedWiFiClient.h"

ManagedWiFiClient::ManagedWiFiClient(ConfigModeCallback _configModeCallback) {
  configModeCallback = _configModeCallback;
}

void ManagedWiFiClient::begin(WiFiManager* wiFiManager) {
  wiFiManager->setAPCallback(configModeCallback);
  if (!wiFiManager->autoConnect(APSsid, APPassword)) {
    Serial.println("failed to connect and hit timeout");
    // reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }

  Serial.println("WiFi connected");
}
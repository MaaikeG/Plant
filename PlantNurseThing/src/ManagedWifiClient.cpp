#include "ManagedWiFiClient.h"

ManagedWiFiClient::ManagedWiFiClient(ConfigModeCallback _configModeCallback) {
  configModeCallback = _configModeCallback;
}

void ManagedWiFiClient::begin(WiFiManager* wiFiManager) {
  wiFiManager->setAPCallback(configModeCallback);
  if (!wiFiManager->autoConnect(AP_SSID, AP_PASSWORD)) {
    Serial.println("failed to connect and hit timeout");
    // reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }

  Serial.println("WiFi connected");
}

void configModeCallback(WiFiManager* myWiFiManager, SSD1306* oled) {
  oled->clear();
  oled->drawString(0, 0, "Entered config mode");

  String ipLabel = "ip: ";
  oled->drawString(0, 10, ipLabel);
  oled->drawString(oled->getStringWidth(ipLabel), 10,
                   WiFi.softAPIP().toString());

  String SsidLabel = "SSID: ";
  int SsidLabelWidth = oled->getStringWidth(SsidLabel);
  oled->drawString(0, 20, SsidLabel);
  oled->drawStringMaxWidth(SsidLabelWidth, 20,
                           oled->getWidth() - SsidLabelWidth,
                           myWiFiManager->getConfigPortalSSID());

  String passwordLabel = "password: ";
  oled->drawString(0, 44, "password: ");
  oled->drawString(oled->getStringWidth(passwordLabel), 44, AP_PASSWORD);
  oled->display();
}
#include "MqttClient.h"

MqttClient::MqttClient(Client *client)
    : adafruitClient(client, MQTT_SERVER, MQTT_PORT, MQTT_CLIENT_ID,
                     MQTT_USERNAME, MQTT_PASSWORD),
      testfeed(&adafruitClient, "test", MQTT_QOS_1) {}

void MqttClient::MqttConnect() {
  int8_t ret;

  // Stop if already connected.
  if (adafruitClient.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = adafruitClient.connect()) !=
         0) {  // connect will return 0 for connected
    Serial.println(adafruitClient.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 10 seconds...");
    adafruitClient.disconnect();
    delay(10000);  // wait 10 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1)
        ;
    }
  }
  Serial.println("MQTT Connected!");
}

void testCallback(char* data, uint16_t len) {
  Serial.print("test: ");
  Serial.println(data);
}

void MqttClient::begin(){
  testfeed.setCallback(testCallback); 
   
  // Setup MQTT subscription for time feed. 
  adafruitClient.subscribe(&testfeed); 
}

void MqttClient::update(uint16_t timeLeft) {
  MqttConnect();
  adafruitClient.processPackets(timeLeft);
  if (millis() - lastPingMillis > PING_FREQUENCY) {
    lastPingMillis = millis();
    if (!adafruitClient.ping()) {
      adafruitClient.disconnect();
    }
  }
}
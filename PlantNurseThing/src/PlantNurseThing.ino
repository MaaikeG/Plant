#include <Wire.h>
#include <SSD1306.h>
#include <Ticker.h>
#include "DebouncedButton.h"
#include "SensorsController.h"
#include "WateringController.h"
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include "WiFiManager.h"
#include <ESP8266WiFi.h>
#include "Credentials.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define I2C_SDA D5
#define I2C_SCL D6

#define waterFrequency 5000 //temp
#define wateringDuration 1000

#define APSsid "Planet Nurse Access Point"
#define APPassword "plantNurse"

SensorsController sensorsController(A0,D2);
SSD1306 oled(0x3c, I2C_SDA, I2C_SCL);
WateringController wateringController(D1, &oled); 
Ticker printSensorValuesTicker;
bool printSensorValuesNextIteration = true;

bool isInManualMode;
DebouncedButton manualModeToggleButton(D3);
bool manualModeToggled;

WiFiClientSecure client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_PORT, MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD);
Adafruit_MQTT_Subscribe testfeed = Adafruit_MQTT_Subscribe(&mqtt, "test");

void setup()   {
  Serial.begin(9600);
  Wire.begin(D5, D6);

  oled.init();
  oled.flipScreenVertically();

  printSensorValuesTicker.attach_ms(2000, [](){
    // set a flag because actually running it takes too long, because at the moment we water and the same time, because we don't have a better way to activate that yet
    printSensorValuesNextIteration = true;
  });

  pinMode(LED_BUILTIN, OUTPUT);
  setManualMode(false);
  manualModeToggleButton.begin();

  WiFiManager wifiManager;
  wifiManager.setAPCallback(configModeCallback);
  if(!wifiManager.autoConnect(APSsid, APPassword)) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }

  Serial.println("WiFi connected");

  testfeed.setCallback(testCallback);
  
  // Setup MQTT subscription for time feed.
  mqtt.subscribe(&testfeed);
}

void loop() {
  if(manualModeToggleButton.read() == LOW){
    if(!manualModeToggled){
      setManualMode(!isInManualMode);
      manualModeToggled = true;
    }
  }else{
    manualModeToggled = false;
  }
  
  if(!isInManualMode){
    wateringController.update();
  }

  if(printSensorValuesNextIteration && !wateringController.isWatering){
    printSensorValuesNextIteration = false;
    sensorsController.updateSensorValues();
    printSensorValues();
  }

  MQTT_connect();
  mqtt.processPackets(10000);
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }

  oled.display();
}

void setManualMode(bool value){
  isInManualMode = value;
  digitalWrite(LED_BUILTIN, value);
}

void printSensorValues() {
  oled.clear();
  char res[24];
  char buff[5];
  dtostrf(sensorsController.getTemperature(), 2, 1, buff);
  sprintf(res, "Temperature: %s C", buff);
  oled.drawString(0, 0, res);

  dtostrf(sensorsController.getHumidity(), 2, 1, buff);
  sprintf(res, "Humidity: %s%%", buff);
  oled.drawString(0, 10, res);

  dtostrf(sensorsController.getPressure(), 4, 1, buff);
  sprintf(res, "Pressure: %s hPa", buff);
  oled.drawString(0, 20, res);

  dtostrf(sensorsController.getSoilMoisture(), 2, 1, buff);
  sprintf(res, "Soil moisture: %s%%", buff);
  oled.drawString(0, 30, res);

  dtostrf(sensorsController.getLightIntensity(), 2, 1, buff);
  sprintf(res, "Light: %s%%", buff);
  oled.drawString(0, 40, res);
  oled.display();
}

void configModeCallback (WiFiManager *myWiFiManager) {
  oled.clear();
  oled.drawString(0, 0, "Entered config mode");
  
  String ipLabel = "ip: ";
  oled.drawString(0, 10, ipLabel);
  oled.drawString(oled.getStringWidth(ipLabel), 10, WiFi.softAPIP().toString());
  
  String SsidLabel = "SSID: ";
  int SsidLabelWidth = oled.getStringWidth(SsidLabel);
  oled.drawString(0, 20, SsidLabel);
  oled.drawStringMaxWidth(SsidLabelWidth, 20, oled.getWidth() - SsidLabelWidth, myWiFiManager->getConfigPortalSSID());
  
  String passwordLabel = "password: ";
  oled.drawString(0, 44, "password: ");
  oled.drawString(oled.getStringWidth(passwordLabel), 44, APPassword);
  oled.display();
}

void testCallback(char *data, uint16_t len) {
  Serial.print("test: ");
  Serial.println(data);
}

void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 10 seconds...");
       mqtt.disconnect();
       delay(10000);  // wait 10 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}

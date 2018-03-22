#include "WateringController.h"
#include "Amux.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <SSD1306.h>
#include <Ticker.h>
#include "DebouncedButton.h"

#define I2C_SDA D5
#define I2C_SCL D6

Amux amux(A0, D2);
Adafruit_BME280 bme280;
SSD1306 oled(0x3c, I2C_SDA, I2C_SCL);
WateringController wateringController(D1, &oled); 
Ticker printSensorValuesTicker;
bool printSensorValuesNextIteration = true;

bool isInManualMode;
DebouncedButton manualModeToggleButton(D3);
bool manualModeToggled;

void setup()   {
  Serial.begin(9600);
  Wire.begin(D5, D6);
  bme280.begin();
  oled.init();
  oled.flipScreenVertically();
  printSensorValuesTicker.attach_ms(2000, [](){
    // set a flag because actually running it takes too long, because at the moment we water and the same time, because we don't have a better way to activate that yet
    printSensorValuesNextIteration = true;
  });
  pinMode(LED_BUILTIN, OUTPUT);
  setManualMode(false);
  manualModeToggleButton.begin();
}

void loop() {
  if(!isInManualMode){
    wateringController.update();
  }

  if(printSensorValuesNextIteration && !wateringController.isWatering){
    printSensorValuesNextIteration = false;
    printSensorValues();
  }

  if(manualModeToggleButton.read() == LOW){
    if(!manualModeToggled){
      setManualMode(!isInManualMode);
      manualModeToggled = true;
    }
  }else{
    manualModeToggled = false;
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
  dtostrf(bme280.readTemperature(), 2, 1, buff);
  sprintf(res, "Temperature: %s C", buff);
  oled.drawString(0, 0, res);
 
  dtostrf(bme280.readHumidity(), 2, 1, buff);
  sprintf(res, "Humidity: %s%%", buff);
  oled.drawString(0, 10, res);
 
  dtostrf(bme280.readPressure(), 4, 1, buff);
  sprintf(res, "Pressure: %s hPa", buff);
  oled.drawString(0, 20, res);
 
  dtostrf(percentifyAnalogInput(amux.getSoilMoisture()), 2, 1, buff);
  sprintf(res, "Soil moisture: %s%%", buff);
  oled.drawString(0, 30, res);
 
  dtostrf(percentifyAnalogInput(amux.getLightIntensity()), 2, 1, buff);
  sprintf(res, "Light: %s%%", buff);
  oled.drawString(0, 40, res);
}

uint8_t percentifyAnalogInput(uint16_t value) {
  return (value / 1023.0) * 100;
}
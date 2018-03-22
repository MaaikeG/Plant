#include "ServoRegulator.h"
#include <Wire.h>
#include <SSD1306.h>
#include <Ticker.h>
#include "DebouncedButton.h"
#include "SensorsController.h"

#define I2C_SDA D5
#define I2C_SCL D6

#define waterFrequency 5000 //temp
#define wateringDuration 1000

SensorsController sensorsController(Amux(A0,D2));
SSD1306 oled(0x3c, I2C_SDA, I2C_SCL);
ServoRegulator servoRegulator(D1); 
Ticker printSensorValuesTicker;
bool printSensorValuesNextIteration = true;

bool isInManualMode;
DebouncedButton manualModeToggleButton(D3);
bool manualModeToggled;

unsigned long lastWatering;

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
}

void loop() {
  if(!isInManualMode && millis() - lastWatering > waterFrequency /* replace by real condition later*/){
    servoRegulator.startWatering();
    lastWatering = millis();
  }else if(!isInManualMode && millis() - lastWatering > wateringDuration && servoRegulator.isWatering){
    servoRegulator.stopWatering();
  }else if(printSensorValuesNextIteration){
    printSensorValuesNextIteration = false;
    sensorsController.updateSensorValues();
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
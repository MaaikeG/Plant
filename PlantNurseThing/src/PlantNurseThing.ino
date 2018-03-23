#include <Wire.h>
#include <SSD1306.h>
#include <Ticker.h>
#include "DebouncedButton.h"
#include "SensorsController.h"
#include "WateringController.h"

#define I2C_SDA D5
#define I2C_SCL D6

SensorsController sensorsController(A0,D2);
SSD1306 oled(0x3c, I2C_SDA, I2C_SCL);
WateringController wateringController(D1, &oled); 
Ticker updateSensorValuesTicker;
bool updateSensorValuesNextIteration = true;

enum mode {
  Manual,
  Automatic
};

bool currentMode;

DebouncedButton modeToggleButton(D3);
bool modeToggled;

void setup()   {
  Serial.begin(9600);
  Wire.begin(D5, D6);
  oled.init();
  oled.flipScreenVertically();
  updateSensorValuesTicker.attach_ms(2000, [](){
    // set a flag because actually running it takes too long, because at the moment we water and the same time, because we don't have a better way to activate that yet
    updateSensorValuesNextIteration = true;
  });
  pinMode(LED_BUILTIN, OUTPUT);
  setMode(false);
  modeToggleButton.begin();
}

void loop() {
  if(currentMode == Automatic){
    wateringController.update(sensorsController.getSoilMoisture());
  }

  if(updateSensorValuesNextIteration && !wateringController.isWatering){
    updateSensorValuesNextIteration = false;
    sensorsController.updateSensorValues();
    printSensorValues();
  }

  if(modeToggleButton.read() == LOW){
    if(!modeToggled){
      setMode(!currentMode);
      modeToggled = true;
    }
  }else{
    modeToggled = false;
  }
  oled.display();
}

void setMode(bool mode){
  currentMode = mode;
  digitalWrite(LED_BUILTIN, mode);
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
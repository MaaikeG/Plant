#include <Wire.h>
#include <SSD1306.h>
#include <Ticker.h>
#include "DebouncedButton.h"
#include "SensorsController.h"
#include "WateringController.h"
#include "ScreenCarousel.h"

#define I2C_SDA D5
#define I2C_SCL D6

#define waterFrequency 5000 //temp
#define wateringDuration 1000

SSD1306 oled(0x3c, I2C_SDA, I2C_SCL);
SensorsController sensorsController(Amux(A0,D2));
ScreenCarousel screenCarousel(&oled, sensorsController);
WateringController wateringController(D1, &oled); 
Ticker printSensorValuesTicker;
bool printSensorValuesNextIteration = true;

bool isInManualMode;
DebouncedButton manualModeToggleButton(D3);
bool manualModeToggled;

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
  if(!isInManualMode){
    wateringController.update();
  }

  if(printSensorValuesNextIteration && !wateringController.isWatering){
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


}
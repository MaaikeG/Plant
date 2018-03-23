#include <Wire.h>
#include <SSD1306.h>
#include <Ticker.h>
#include "DebouncedButton.h"
#include "WateringController.h"
#include "ScreenCarousel.h"

#define I2C_SDA D5
#define I2C_SCL D6

#define waterFrequency 5000 //temp
#define wateringDuration 1000

SSD1306 oled(0x3c, I2C_SDA, I2C_SCL);
SensorsController sensorsController(Amux(A0,D2));
ScreenCarousel screenCarousel(&oled, &sensorsController);
WateringController wateringController(D1, &oled); 
Ticker printSensorValuesTicker;
bool printSensorValuesNextIteration = true;

bool isInManualMode;
DebouncedButton manualModeToggleButton(D3);
bool manualModeToggled;

FrameCallback frames[] = {
  [](OLEDDisplay *display, OLEDDisplayUiState* state, short x, short y){
    screenCarousel.drawFrame1(display, state, x, y);
  }, [](OLEDDisplay *display, OLEDDisplayUiState* state, short x, short y){
    screenCarousel.drawFrame2(display, state, x, y);
  }
};

void setup()   {
  Serial.begin(9600);
  Wire.begin(D5, D6);
  oled.init();
  printSensorValuesTicker.attach_ms(2000, [](){
    // set a flag because actually running it takes too long, because at the moment we water and the same time, because we don't have a better way to activate that yet
    printSensorValuesNextIteration = true;
  });
  pinMode(LED_BUILTIN, OUTPUT);
  setManualMode(false);
  manualModeToggleButton.begin();
  screenCarousel.begin(frames, 2);
  oled.flipScreenVertically();
}

void loop() {
  if(!isInManualMode){
    wateringController.update();
  }

  if(!wateringController.isWatering){
    int remainingTimeBudget = screenCarousel.update();
    if(remainingTimeBudget > 0 && printSensorValuesNextIteration){
      printSensorValuesNextIteration = false;
      sensorsController.updateSensorValues();
    }
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
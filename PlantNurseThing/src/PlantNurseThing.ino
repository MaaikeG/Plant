#include <Wire.h>
#include <SSD1306.h>
#include <Ticker.h>
#include "DebouncedButton.h"
#include "WateringController.h"
#include "ScreenCarousel.h"

#define I2C_SDA D5
#define I2C_SCL D6

SensorsController sensorsController(A0,D2);
SSD1306 oled(0x3c, I2C_SDA, I2C_SCL);
WateringController wateringController(D1, &oled); 
ScreenCarousel screenCarousel(&oled, &sensorsController, &wateringController);
Ticker updateSensorValuesTicker;
bool updateSensorValuesNextIteration = true;

enum Mode {
  Manual,
  Automatic
};

FrameCallback frames[] = {
  [](OLEDDisplay *display, OLEDDisplayUiState* state, short x, short y){
    screenCarousel.drawFrame1(display, state, x, y);
  }, [](OLEDDisplay *display, OLEDDisplayUiState* state, short x, short y){
    screenCarousel.drawFrame2(display, state, x, y);
  }, [](OLEDDisplay *display, OLEDDisplayUiState* state, short x, short y){
    screenCarousel.drawFrame3(display, state, x, y); 
  }
};

Mode currentMode; 
 
DebouncedButton modeToggleButton(D3); 
bool modeToggled = true; 

void setup()   {
  Serial.begin(9600);
  Wire.begin(D5, D6);
  oled.init();
  updateSensorValuesTicker.attach_ms(2000, [](){
    // set a flag because actually running it takes too long, because at the moment we water and the same time, because we don't have a better way to activate that yet
    updateSensorValuesNextIteration = true;
  });
  pinMode(LED_BUILTIN, OUTPUT);
  setMode(Manual);
  modeToggleButton.begin();
  screenCarousel.begin(frames, 3);
  oled.flipScreenVertically();
}

void loop() {
  if(currentMode == Automatic){
    wateringController.update(sensorsController.getSoilMoisture());
  }

  if(!wateringController.isWatering){
    int remainingTimeBudget = screenCarousel.update();
    if(remainingTimeBudget > 0 && updateSensorValuesNextIteration){
      updateSensorValuesNextIteration = false;
      sensorsController.updateSensorValues();
    }
  }

  if(modeToggleButton.read() == LOW){
    if(!modeToggled){
      setMode(currentMode == Automatic ? Manual : Automatic);
      modeToggled = true;
    }
  }else{
    modeToggled = false;
  }
}

void setMode(Mode mode){
  currentMode = mode;
  digitalWrite(LED_BUILTIN, mode == Automatic ? LOW : HIGH);
}
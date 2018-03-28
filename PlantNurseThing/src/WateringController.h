#ifndef _WATERINGCONTROLLER_h
#define _WATERINGCONTROLLER_h

#include <SSD1306.h>
#include <Servo.h>

// clang-format off
#if defined(ARDUINO) && ARDUINO >= 100
  #include "arduino.h"
#else
  #include "WProgram.h"
#endif
// clang-format on

// Variables are lower for testing. Should be increased for real-life use to
// commented values.
#define wateringDuration 3000     // 5000
#define soilMoistureThreshold 20  // if soil is this dry we need to water.
#define reservoirEmptyTimeCheck \
  3000  // 60000 // is one minute is not enough for soil moisture to increase,
        // reservoir must be empty.
#define RESERVOIR_EMPTY_LED D7

class WateringController {
 private:
  Servo servo;
  uint8_t servoPin;
  SSD1306* oled;
  bool reservoirEmptyCheckDone = true;
  unsigned long wateringStart;

 public:
  WateringController(uint8_t _servoPin, SSD1306* _oled);
  void startWatering();
  void stopWatering();
  bool shouldWater(uint8_t soilMoisture);
  void update(uint8_t soilMoisture);
  void checkReservoirEmpty(uint8_t soilMoisture);
  bool isWatering;
  bool reservoirEmpty;  // use this to blink a red led?
  unsigned long lastWatering;
};

#endif

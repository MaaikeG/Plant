#ifndef _WATERING_CONTROLLER_h
#define _WATERING_CONTROLLER_h

#include <SSD1306.h>
#include <Servo.h>
#include "TimedTicker.h"

// clang-format off
#if defined(ARDUINO) && ARDUINO >= 100
  #include "arduino.h"
#else
  #include "WProgram.h"
#endif
// clang-format on

// Variables are lower for testing. Should be increased for real-life use to
// commented values.
#define WATERING_DURATION 3000
#define SOIL_MOISTURE_THRESHOLD 20  // if soil is this dry we need to water.
#define RESERVOIR_EMPTY_CHECK_TIME 5 * 60 * 1000  // reservoir must be empty after this period.
#define WATER_FLOW_ANGLE 90
#define NO_WATER_FLOW_ANGLE 0
#define STOP_WATERING_DURATION 1000
#define STOP_WATERING_UPDATE_PERIOD 50

class WateringController {
 private:
  Servo servo;
  uint8_t servoPin;
  uint8_t reservoirEmptyLedPin;
  SSD1306& oled;
  unsigned long wateringStart;
  void (*onReservoirEmpty)();
  void (*onReservoirFilled)();
  TimedTicker slowStopTicker;
  bool shouldWater();
  Ticker checkReservoirEmptyTicker;

 public:
  WateringController(uint8_t _servoPin, uint8_t _reservoirEmptyLedPin,
                     SSD1306& _oled, void (*_onReservoirEmpty)(),
                     void (*_onReservoirFilled)(),
                     uint8_t (*_getSoilMoisture)());
  void startWatering();
  void stopWatering();
  void begin();
  void update();
  bool isWatering;
  bool reservoirEmpty;
  unsigned long lastWatering;
  void setAngle(uint16_t newAngle);
  uint8_t (*getSoilMoisture)();
  bool reservoirEmptyCheckDone = true;
  void reservoirEmptied();
  void reservoirFilled();
};

#endif

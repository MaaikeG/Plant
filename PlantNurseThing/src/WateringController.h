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
#define wateringDuration 1500
#define soilMoistureThreshold 20  // if soil is this dry we need to water.
#define reservoirEmptyTimeCheck \
  3000  // 60000 // is one minute is not enough for soil moisture to increase,
        // reservoir must be empty.
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
  bool reservoirEmptyCheckDone = true;
  unsigned long wateringStart;
  void (*onReservoirEmpty)();
  void (*onReservoirFilled)();
	TimedTicker slowStopTicker;

 public:
  WateringController(uint8_t _servoPin, uint8_t _reservoirEmptyLedPin,
                     SSD1306& _oled, void (*_onReservoirEmpty)(),
                     void (*_onReservoirFilled)());
  void startWatering();
  void stopWatering();
  bool shouldWater(uint8_t soilMoisture);
  void begin();
  void update(uint8_t soilMoisture);
  void checkReservoirEmpty(uint8_t soilMoisture);
  bool isWatering;
  bool reservoirEmpty;  // use this to blink a red led?
  unsigned long lastWatering;
	uint16_t angle;
	void setAngle(uint16_t newAngle);
};

#endif

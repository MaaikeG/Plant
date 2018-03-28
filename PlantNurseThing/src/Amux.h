#ifndef _AMUX_H
#define _AMUX_H

// clang-format off
#if defined(ARDUINO) && ARDUINO >= 100
  #include "arduino.h"
#else
  #include "WProgram.h"
#endif
// clang-format on

class Amux {
 private:
  uint8_t inputPin;
  uint8_t selectorPin;

 public:
  Amux(uint8_t _inputPin, uint8_t _selectorPin);
  uint16_t getSoilMoisture();
  uint16_t getLightIntensity();
};

#endif

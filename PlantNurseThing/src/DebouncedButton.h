#ifndef _DEBOUNCED_BUTTON_H
#define _DEBOUNCED_BUTTON_H

// clang-format off
#if defined(ARDUINO) && ARDUINO >= 100
  #include "arduino.h"
#else
  #include "WProgram.h"
#endif
// clang-format on

#define DEBOUNCE_DELAY 50

class DebouncedButton {
 private:
  uint8_t buttonPin;
  bool lastButtonState;
  bool buttonState;
  unsigned long lastDebounceTime;

 public:
  DebouncedButton(uint8_t _buttonPin);
  bool read();
  void begin();
};

#endif
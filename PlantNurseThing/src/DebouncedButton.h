#define debounceDelay 50

#ifndef _Toggle_h
#define _Toggle_h

// clang-format off
#if defined(ARDUINO) && ARDUINO >= 100
  #include "arduino.h"
#else
  #include "WProgram.h"
#endif
// clang-format on

class DebouncedButton
{
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
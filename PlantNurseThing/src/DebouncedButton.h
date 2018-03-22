#define debounceDelay 50

#ifndef _Toggle_h
#define _Toggle_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

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
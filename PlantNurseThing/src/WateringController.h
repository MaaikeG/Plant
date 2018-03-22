#ifndef _WATERINGCONTROLLER_h
#define _WATERINGCONTROLLER_h

#include <Servo.h>
#include <SSD1306.h>

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define waterFrequency 5000 //temp
#define wateringDuration 1000

class WateringController
{
private:
	Servo servo;
	uint8_t servoPin;
	SSD1306* oled;

public:
	WateringController(uint8_t _servoPin, SSD1306* _oled);
	void startWatering();
	void stopWatering();
	bool isWatering;
	void update();
	unsigned long lastWatering;
};

#endif


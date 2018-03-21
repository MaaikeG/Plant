// ServoRegulator.h

#ifndef _SERVOREGULATOR_h
#define _SERVOREGULATOR_h
#define SERVO D3

#include <Servo.h>

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class ServoRegulator
{
private:
	Servo servo;
	uint8_t servoPin;

public:
	ServoRegulator(uint8_t _servoPin);
	void water();
};

#endif


#include "ServoRegulator.h"

ServoRegulator::ServoRegulator(uint8_t _servoPin) {
	servo.attach(_servoPin);
}

void ServoRegulator::startWatering() {
	// todo: update oled
	servo.write(90);
	isWatering = true;
}

void ServoRegulator::stopWatering() {
	// todo: update oled
	servo.write(0);
	isWatering = false;
}


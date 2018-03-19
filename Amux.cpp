#include "Amux.h"

Amux::Amux(uint8_t _inputPin, uint8_t _selectorPin)
{
	inputPin = _inputPin;
	selectorPin = _selectorPin;
}

uint16_t Amux::getSoilMoisture()
{
	digitalWrite(selectorPin, HIGH);
	// TODO: make clockwatch!
	delay(100);
	uint16_t value = analogRead(inputPin);
	// set back to low to avoid corrosion
	digitalWrite(selectorPin, LOW);
	return value;
}

uint16_t Amux::getLightIntensity()
{
	//Todo: only wait 100 ms from last switch
	delay(100); // wait shortly for sensor value to stabilize
	return analogRead(inputPin);
}

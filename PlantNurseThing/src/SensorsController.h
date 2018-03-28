#ifndef _SensorsController_h
#define _SensorsController_h

#include "Amux.h"
#include <Adafruit_BME280.h>
#include <Wire.h>

// clang-format off
#if defined(ARDUINO) && ARDUINO >= 100
  #include "arduino.h"
#else
  #include "WProgram.h"
#endif
// clang-format on

class SensorsController
{
 private:
	Amux amux;
	Adafruit_BME280 Bme280;
    float temperature;
	float humidity;
	float pressure;
    uint8_t soilMoisture;
    uint8_t lightIntensity;
	uint8_t percentifyAnalogInput(uint16_t value);

 public:
	SensorsController(uint8_t amuxInputPin, uint8_t amuxSelectorPin);
	void updateSensorValues();

	float getTemperature();
	float getHumidity();
	float getPressure();
    uint8_t getSoilMoisture();
    uint8_t getLightIntensity();
};

#endif

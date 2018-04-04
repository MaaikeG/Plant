#include "SensorsController.h"

SensorsController::SensorsController(uint8_t amuxInputPin,
                                     uint8_t amuxSelectorPin)
    : amux(amuxInputPin, amuxSelectorPin) {
}

void SensorsController::begin() {
  // Wire.begin(D5, D6); assume it's already been called by setup()
  Bme280.begin();
  updateSensorValues();
}

void SensorsController::updateSensorValues() {
  temperature = Bme280.readTemperature();
  humidity = Bme280.readHumidity();
  pressure = Bme280.readPressure();
  lightIntensity = percentifyAnalogInput(amux.getLightIntensity());
  soilMoisture = percentifyAnalogInput(amux.getSoilMoisture());
}

uint8_t SensorsController::percentifyAnalogInput(uint16_t value) {
  return (value / 1023.0) * 100;
}

float SensorsController::getTemperature() {
  return SensorsController::temperature;
}
float SensorsController::getHumidity() { return SensorsController::humidity; }
float SensorsController::getPressure() { return SensorsController::pressure; }
uint8_t SensorsController::getSoilMoisture() {
  return SensorsController::soilMoisture;
}
uint8_t SensorsController::getLightIntensity() {
  return SensorsController::lightIntensity;
}
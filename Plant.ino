#include "Amux.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

Amux amux(A0, D2);
Adafruit_BME280 Bme280;

void setup()   {                
  Serial.begin(9600);
  Wire.begin(D5, D6);
  setupOLED();
  Bme280.begin();
  printSensorValues();
}

void loop() { 
  delay(2000);
  printSensorValues();
}

void printSensorValues() {
  clearDisplay();
  char res[24];
  char buff[5];
  dtostrf(Bme280.readTemperature(), 2, 1, buff);
  sprintf(res, "Temperature: %s C", buff);
  writeText(res, 0, 0);
  
  dtostrf(Bme280.readHumidity(), 2, 1, buff);
  sprintf(res, "Humidity: %s%%", buff);
  writeText(res, 0, 8);
  
  dtostrf(Bme280.readPressure(), 4, 1, buff);
  sprintf(res, "Pressure: %s hPa", buff);
  writeText(res, 0, 16);

  dtostrf(percentifyAnalogInput(amux.getSoilMoisture()), 2, 1, buff);
  sprintf(res, "Soil moisture: %s%%", buff);
  writeText(res, 0, 32);

  dtostrf(percentifyAnalogInput(amux.getLightIntensity()), 2, 1, buff);
  sprintf(res, "Light: %s%%", buff);
  writeText(res, 0, 38);
}

uint8_t percentifyAnalogInput(uint16_t value) {
	return (value / 1023.0) * 100;
}
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define BME_SCK D5
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

Adafruit_BME280 bme; // I2C

void setupBME() {
    bool status;
   
    status = bme.begin();  
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        while (1);
    }
}

float getTemperature() {
  return bme.readTemperature();
}

float getHumidity() {
  return bme.readHumidity();
}

float getPressure() {
  return bme.readPressure() / 100.0F;
}

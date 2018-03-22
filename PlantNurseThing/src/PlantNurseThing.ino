#include "ServoRegulator.h"
#include "Amux.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <SSD1306.h>

#define I2C_SDA D5
#define I2C_SCL D6

Amux amux(A0, D2);
Adafruit_BME280 Bme280;
SSD1306 Oled(0x3c, I2C_SDA, I2C_SCL);
ServoRegulator servoRegulator(D3);

void setup()   {
  Serial.begin(9600);
  Wire.begin(D5, D6);
  Bme280.begin();
  Oled.init();
  Oled.flipScreenVertically();
}

void loop() {
  printSensorValues();
  servoRegulator.water();
  delay(2000);
}

void printSensorValues() {
  Oled.clear();
  char res[24];
  char buff[5];
  dtostrf(Bme280.readTemperature(), 2, 1, buff);
  sprintf(res, "Temperature: %s C", buff);
  Oled.drawString(0, 0, res);

  dtostrf(Bme280.readHumidity(), 2, 1, buff);
  sprintf(res, "Humidity: %s%%", buff);
  Oled.drawString(0, 10, res);

  dtostrf(Bme280.readPressure(), 4, 1, buff);
  sprintf(res, "Pressure: %s hPa", buff);
  Oled.drawString(0, 20, res);

  dtostrf(percentifyAnalogInput(amux.getSoilMoisture()), 2, 1, buff);
  sprintf(res, "Soil moisture: %s%%", buff);
  Oled.drawString(0, 30, res);

  dtostrf(percentifyAnalogInput(amux.getLightIntensity()), 2, 1, buff);
  sprintf(res, "Light: %s%%", buff);
  Oled.drawString(0, 40, res);
  Oled.display();
}

uint8_t percentifyAnalogInput(uint16_t value) {
  return (value / 1023.0) * 100;
}
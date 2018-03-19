#include <Wire.h>

void setup()   {                
  Serial.begin(9600);
  Wire.begin(D5, D6);
  setupOLED();
  setupBME();
  setupAMUX();
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
  dtostrf(getTemperature(), 2, 1, buff);
  sprintf(res, "Temperature: %s C", buff);
  writeText(res, 0, 0);
  
  dtostrf(getHumidity(), 2, 1, buff);
  sprintf(res, "Humidity: %s%%", buff);
  writeText(res, 0, 8);
  
  dtostrf(getPressure(), 4, 1, buff);
  sprintf(res, "Pressure: %s hPa", buff);
  writeText(res, 0, 16);

  dtostrf(getSoilMoisture(), 2, 1, buff);
  sprintf(res, "Soil moisture: %s%%", buff);
  writeText(res, 0, 32);

  dtostrf(getLightIntensity(), 2, 1, buff);
  sprintf(res, "Light: %s%%", buff);
  writeText(res, 0, 38);
}


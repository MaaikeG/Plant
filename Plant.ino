#include <Wire.h>

void setup()   {                
  Serial.begin(9600);
  Wire.begin(D5, D6);
  setupOLED();
  setupBME();
  PrintSensorValues();
}

void loop() { 
}

void PrintSensorValues() {
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
}


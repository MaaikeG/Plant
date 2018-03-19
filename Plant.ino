#include <Wire.h>

void setup()   {                
  Serial.begin(9600);
  Wire.begin(D5, D6);
  setupOLED();
  setupBME();
  printValues();
}

void loop() { 
}

#define AMUX_SEL D2
#define AMUX_OUT A0

void setupAMUX() {
  pinMode(AMUX_OUT, INPUT);
  pinMode(AMUX_SEL, OUTPUT);
}

float getSoilMoisture() {
  int val;
  digitalWrite(AMUX_SEL, HIGH);
  delay(100); // TODO: make clockwatch!
  val = analogRead(AMUX_OUT);
  // set back to low to avoid corrosion
  digitalWrite(AMUX_SEL, LOW);
  return (val / 1023.0) *100;
}

float getLightIntensity() {
  digitalWrite(AMUX_SEL, LOW);
  delay(100); // wait shortly for sensor value to stabilize (make clockwatch!)
  int val = analogRead(AMUX_OUT);
  delay(100); // wait shortly for sensor value to stabilize (make clockwatch!)
  return (val /1023.0) *100;
}


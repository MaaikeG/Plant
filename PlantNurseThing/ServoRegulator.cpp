// 
// 
// 

#include "ServoRegulator.h"

ServoRegulator::ServoRegulator(uint8_t _servoPin) {
	servo.attach(_servoPin);
}

void ServoRegulator::water() {
	int pos;

	for (pos = 0; pos <= 90; pos += 1) // goes from 0 degrees to 180 degrees 
	{                                  // in steps of 1 degree 
		servo.write(pos);              // tell servo to go to position in variable 'pos' 
		delay(15);                       // waits 15ms for the servo to reach the position 
	}
	for (pos = 90; pos >= 0; pos -= 1)     // goes from 180 degrees to 0 degrees 
	{
		servo.write(pos);              // tell servo to go to position in variable 'pos' 
		delay(15);                       // waits 15ms for the servo to reach the position 
	}
}


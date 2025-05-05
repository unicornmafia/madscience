
#include <ESP32Servo.h>

Servo myservo;  // create servo object to control a servo
// 16 servo objects can be created on the ESP32

#define SERVO_PIN_X 1

int pos = 0;    // variable to store the servo position

int servoPin = 1;

void setup() {
  // define pin, and set min and max.  1000=0, 2000=180 
	myservo.attach(SERVO_PIN_X, 1000, 2000); 
}

void loop() {

	for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
		// in steps of 1 degree
		myservo.write(pos);    // tell servo to go to position in variable 'pos'
		delay(15);             // waits 15ms for the servo to reach the position
	}
	for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
		myservo.write(pos);    // tell servo to go to position in variable 'pos'
		delay(15);             // waits 15ms for the servo to reach the position
	}
}


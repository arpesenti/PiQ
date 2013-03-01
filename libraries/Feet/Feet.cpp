#include "Feet.h"

Feet::Feet() {
	areOpen = false;
}

void Feet::init() {
	leftServo.attach(LEFT_SERVO_PIN);
	rightServo.attach(RIGHT_SERVO_PIN);
}

// void Feet::close(int speed) {
// 	if (areOpen)
// 	{
// 		// leftServo.attach(LEFT_SERVO_PIN);
// 		// rightServo.attach(RIGHT_SERVO_PIN);
// 		delay(100);
// 		write(&leftServo, LEFT_CLOSED_ANGLE, speed);
// 		write(&rightServo, RIGHT_CLOSED_ANGLE, speed);
// 		areOpen = false;
// 		// leftServo.detach();
// 		// rightServo.detach();
// 	}
// }

// void Feet::open(int speed) {
// 	if (!areOpen)
// 	{
// 		// leftServo.attach(LEFT_SERVO_PIN);
// 		// rightServo.attach(RIGHT_SERVO_PIN);
// 		delay(100);
// 		write(&leftServo, LEFT_CLOSED_ANGLE - OPEN_ANGLE, speed);
// 		write(&rightServo, RIGHT_CLOSED_ANGLE + OPEN_ANGLE, speed);
// 		areOpen = true;
// 		// leftServo.detach();
// 		// rightServo.detach();
// 	}
// }

void Feet::open() {
	for (int i =0; i< 10; i++) {
	leftServo.write(LEFT_CLOSED_ANGLE - OPEN_ANGLE);
	delay(15);
	SoftwareServo::refresh();
	rightServo.write(RIGHT_CLOSED_ANGLE + OPEN_ANGLE);
	delay(15);
	SoftwareServo::refresh(); 
}
}

void Feet::partialOpen() {
	for (int i =0; i< 10; i++) {
	leftServo.write(LEFT_CLOSED_ANGLE - OPEN_ANGLE);
	delay(15);
	SoftwareServo::refresh();
	rightServo.write(RIGHT_CLOSED_ANGLE + OPEN_ANGLE);
	delay(15);
	SoftwareServo::refresh(); 
}
}

void Feet::close() {
	for (int i =0; i< 10; i++) {
	leftServo.write(LEFT_CLOSED_ANGLE);
	delay(15);
	 SoftwareServo::refresh();
	rightServo.write(RIGHT_CLOSED_ANGLE);
	delay(15);
	 SoftwareServo::refresh();
	}
}

// move servo to angle in degree with speed in percentage
// void Feet::write(Servo *servo, int angle, int speed) {
// 	int stepDelay;
// 	if (speed > 100)
// 	{
// 		stepDelay = 0;
// 	} else if (speed <= 0) {
// 		stepDelay = 100;
// 	} else {
// 		stepDelay = 100 - speed;
// 	}
// 	int startingAngle = servo->read();
// 	int step = (angle - startingAngle)/NUMBER_OF_STEPS; 
// 	int currentAngle = startingAngle;
// 	for (int i = 0; i<NUMBER_OF_STEPS; i++) {
// 		servo->write(currentAngle);
// 		currentAngle += step;
// 		delay(stepDelay);
// 	}
// 	servo->write(angle); // if the step is not a divisor of the angle
// }

// void Feet::trick() {
// 	// TODO experiment a nice trick
// 	randomSeed(analogRead(0));
// 	int n = random(3,10);
// 	if (areOpen) close();
// 	for (int i=0; i<n; i++) {
// 		open(random(50,100));
// 		delay(random(100,200));
// 		close(random(50,100));
// 		delay(random(100,200));
// 	}
// }

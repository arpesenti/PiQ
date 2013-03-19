#include "Motion.h"

Motion::Motion() {
	minSpeedLeft = MIN_SPEED_LEFT;
	minSpeedRight = MIN_SPEED_RIGHT;
	availableSpeed = 100 - (minSpeedRight > minSpeedLeft ? minSpeedRight: minSpeedLeft);
}

void Motion::init(){
	motorBegin(MOTOR_LEFT);
	motorBegin(MOTOR_RIGHT);
}


// N.B. speed is a value between 0 and 100 (representing a percentage)

void Motion::moveForward(int speed) {
	if (speed >= 100) speed = 99;
	int speedLeft = minSpeedLeft + speed/100.0 * availableSpeed;
	int speedRight = minSpeedRight + speed/100.0 * availableSpeed;
   
	motorForward(MOTOR_LEFT, speedLeft);
	motorForward(MOTOR_RIGHT, speedRight);
	
}

void Motion::moveBackward(int speed) {
	if (speed >= 100) speed = 99;
	int speedLeft = minSpeedLeft + speed/100.0 * availableSpeed;
	int speedRight = minSpeedRight + speed/100.0 * availableSpeed;
   
	motorReverse(MOTOR_LEFT, speedLeft);
	motorReverse(MOTOR_RIGHT, speedRight);

}

void Motion::stop() {
	motorStop(MOTOR_LEFT);
	motorStop(MOTOR_RIGHT);
}

void Motion::rotateLeft(int speed) {
	if (speed >= 100) speed = 99;
	int speedLeft = minSpeedLeft + speed/100.0 * availableSpeed;
	int speedRight = minSpeedRight + speed/100.0 * availableSpeed;

 // 	motorReverse(MOTOR_LEFT, speedLeft + ACCELERATION_SPEED);
	// motorForward(MOTOR_RIGHT, speedRight + ACCELERATION_SPEED);
	// delay(ACCELERATION_TIME);

	motorReverse(MOTOR_LEFT, speedLeft);
	motorForward(MOTOR_RIGHT, speedRight);
}

void Motion::rotateRight(int speed) {
	if (speed >= 100) speed = 99;
	int speedLeft = minSpeedLeft + speed/100.0 * availableSpeed;
	int speedRight = minSpeedRight + speed/100.0 * availableSpeed;
   
	// motorForward(MOTOR_LEFT, speedLeft + ACCELERATION_SPEED);
	// motorReverse(MOTOR_RIGHT, speedRight + ACCELERATION_SPEED);
	// delay(ACCELERATION_TIME);

	motorForward(MOTOR_LEFT, speedLeft);
	motorReverse(MOTOR_RIGHT, speedRight);
}

void Motion::setMinSpeed(int left, int right) {
	minSpeedLeft = left;
	minSpeedRight = right;
	availableSpeed = 100 - (minSpeedRight > minSpeedLeft ? minSpeedRight: minSpeedLeft);
}

// positive drift -> rotate left
void Motion::moveForwardWithDrift(int speed, int drift) {
	// set a lower minimum speed
	int speedLeft   =  minSpeedLeft - 30 + constrain(speed/100.0 * (availableSpeed + 30) + (drift / DAMPING), 0, availableSpeed); 
  	int speedRight  = minSpeedRight - 30 + constrain(speed/100.0 * (availableSpeed + 30) - (drift / DAMPING), 0, availableSpeed);

  	Serial.print("Speed left: ");
  	Serial.print(speedLeft);
  	Serial.print("  Speed right: ");
  	Serial.println(speedRight);

 	motorForward(MOTOR_LEFT, speedLeft);
  	motorForward(MOTOR_RIGHT, speedRight); 
}

// TODO - calibrate
void Motion::penguinWalking() {
	float T = 5; // period
  
  for (float t = 0.0; t <= 70.0; t += 0.2){   
  	motorForward(MOTOR_LEFT, minSpeedLeft + availableSpeed * cos(2*3.14/T*t));
  	motorForward(MOTOR_RIGHT, minSpeedRight +  availableSpeed * sin(2*3.14/T*t));
		// //position.update();
		// delay(10);
		// //position.update();
		// delay(10);
		// position.update();     
		delay(20);
		if (fmod(2*PI+2*3.14/T*t, 2*PI)<0.05 && abs(fmod(2*PI+2*3.14/T*t, 2*PI)-PI/2)<0.05 ) {
			stop();
			delay(50);
		}
  }
	
	stop();
	// position.update();
	// delay(10);
	// position.update();
}

#include "Motion.h"

Motion::Motion() {
	
}

void Motion::init(){
	motorBegin(MOTOR_LEFT);
	motorBegin(MOTOR_RIGHT);
}


// N.B. speed is a value between 0 and 100 (representing a percentage)

void Motion::moveForward(int speed) {
	int speedLeft = MIN_SPEED_LEFT + speed/100.0 * AVAILABLE_SPEED;
	int speedRight = MIN_SPEED_RIGHT + speed/100.0 * AVAILABLE_SPEED;
   
	motorForward(MOTOR_LEFT, speedLeft);
	motorForward(MOTOR_RIGHT, speedRight);
	
}

void Motion::moveBackward(int speed) {
	int speedLeft = MIN_SPEED_LEFT + speed/100.0 * AVAILABLE_SPEED;
	int speedRight = MIN_SPEED_RIGHT + speed/100.0 * AVAILABLE_SPEED;
   
	motorReverse(MOTOR_LEFT, speedLeft);
	motorReverse(MOTOR_RIGHT, speedRight);

}

void Motion::stop() {
	motorStop(MOTOR_LEFT);
	motorStop(MOTOR_RIGHT);
}

void Motion::rotateLeft(int speed) {
	int speedLeft = MIN_SPEED_LEFT + speed/100.0 * AVAILABLE_SPEED;
	int speedRight = MIN_SPEED_RIGHT + speed/100.0 * AVAILABLE_SPEED;
   
	motorReverse(MOTOR_LEFT, speedLeft);
	motorForward(MOTOR_RIGHT, speedRight);
}

void Motion::rotateRight(int speed) {
	int speedLeft = MIN_SPEED_LEFT + speed/100.0 * AVAILABLE_SPEED;
	int speedRight = MIN_SPEED_RIGHT + speed/100.0 * AVAILABLE_SPEED;
   
	motorForward(MOTOR_LEFT, speedLeft);
	motorReverse(MOTOR_RIGHT, speedRight);
}

// TODO - calibrate
void Motion::penguinWalking(Position& position) {
	float T = 5; // period
  
  for (float t = 0.0; t <= 70.0; t += 0.1){    
  	motorForward(MOTOR_LEFT, MIN_SPEED_LEFT + AVAILABLE_SPEED * cos(2*3.14/T*t));
  	motorForward(MOTOR_RIGHT, MIN_SPEED_RIGHT +  AVAILABLE_SPEED * sin(2*3.14/T*t));
		position.update();
		delay(10);
		position.update();
		delay(10);
		position.update();       
  }
	
	stop();
	position.update();
	delay(10);
	position.update();
}

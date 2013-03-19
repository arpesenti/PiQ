#ifndef Motion_h
#define Motion_h

#include <AFMotor.h>
#include <RobotMotor.h>
#include <Arduino.h>
//#include <Position.h>
#include <Pins.h>
#include <PS2.h>
#include <HMC5883L.h>


// TODO: adjust these 2 values in order to have the robot that goes straightforward at minimum speed
#define MIN_SPEED_LEFT 50
#define MIN_SPEED_RIGHT 50
#define DAMPING 5 //1 is most sensitive, range 1 to 1023

#define ACCELERATION_SPEED 20
#define ACCELERATION_TIME 30

class Motion {
public:
	Motion();
	void init();
	void moveForward(int speed);
	void moveForwardWithDrift(int speed, int drift);
	void moveBackward(int speed);
	void stop();
	void rotateLeft(int speed);
	void rotateRight(int speed);
	void setMinSpeed(int left, int right);
	void penguinWalking();
private:
	int minSpeedLeft;
	int minSpeedRight;
	int availableSpeed;
};

#endif

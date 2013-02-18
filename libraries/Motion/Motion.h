#ifndef Motion_h
#define Motion_h

#include <AFMotor.h>
#include <RobotMotor.h>
#include <Arduino.h>
#include <Position.h>
#include <Pins.h>
#include <PS2.h>
#include <HMC5883L.h>


// TODO: adjust these 2 values in order to have the robot that goes straightforward at minimum speed
#define MIN_SPEED_LEFT 50
#define MIN_SPEED_RIGHT 50


// IF MIN_SPEED_LEFT < MIN_SPEED_RIGHT
#define AVAILABLE_SPEED (100 - MIN_SPEED_RIGHT)


class Motion {
public:
	Motion();
	void moveForward(int speed);
	void moveBackward(int speed);
	void stop();
	void rotateLeft(int speed);
	void rotateRight(int speed);
	void penguinWalking(Position& position);
	
};

#endif

#ifndef Feet_h
#define Feet_h

#include <Arduino.h>
#include <SoftwareServo.h>
#include <Pins.h>

#define OPEN_ANGLE 20
#define LEFT_CLOSED_ANGLE 170
#define RIGHT_CLOSED_ANGLE 10
#define PARTIAL_OPEN_ANGLE 20
#define NUMBER_OF_STEPS 5

class Feet {
public:
	Feet();
	void init();
	void close();
	void partialOpen();
	void open();
	void close(int speed);
	void open(int speed);
	void trick();
	
private:
	//void write(Servo *servo, int angle, int speed);
	bool areOpen;
	SoftwareServo leftServo;
	SoftwareServo rightServo;
};

#endif
#ifndef Feet_h
#define Feet_h

#include <Arduino.h>
#include <Servo.h>

#define LEFT_SERVO_PIN 9
#define RIGHT_SERVO_PIN 10
#define OPEN_ANGLE 45
#define LEFT_CLOSED_ANGLE 180
#define RIGHT_CLOSED_ANGLE 0
#define NUMBER_OF_STEPS 5

class Feet {
public:
	Feet();
	void init();
	void close();
	void open();
	void close(int speed);
	void open(int speed);
	void trick();
	
private:
	void write(Servo *servo, int angle, int speed);
	bool areOpen;
	Servo leftServo;
	Servo rightServo;
};

#endif
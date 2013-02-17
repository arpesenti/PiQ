#ifndef Robot_h
#define Robot_h

#include <Arduino.h>
#include <Position.h>
#include <Motion.h>
#include <ProximitySensor.h>
#include <LineSensor.h>
#include <Feet.h>
#include <Remote.h>

class Robot {
public:
	Robot();
	void init();
	bool scanForEgg();
	bool reachEgg();
	bool catchEgg();
	bool positioningTowardHome();
	int tryToApproach();
	bool searchLine();
	bool followLineToHome();
	bool deposit();
	bool newInit();
	bool escapeFromPanic();
private:
	Position position;
	Motion motion;
	ProximitySensor topSensor;
	ProximitySensor bottomNearSensor;
	ProximitySensor bottomFarSensor;
	LineSensor lineSensor;
	Feet feet;
	Remote remote;
};

#endif
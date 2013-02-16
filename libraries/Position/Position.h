#ifndef Position_h
#define Position_h

#include <Arduino.h>
#include <PS2.h>
#include <HMC5883L.h>

#define CLOCK_PIN 6
#define DATA_PIN 5

class Position {
public:
	Position();
	int getX();
	int getY();
	double getOrientation();
	void reset();
	void update();
	bool init();
private:
	double updateAngle();
	void mouseInit();
	int x;
	int y;
	double relativeAngle;
	double northAngle;
	PS2 mouse;
	HMC5883L compass;
};
#endif
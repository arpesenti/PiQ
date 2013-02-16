#ifndef Position2_h
#define Position2_h

#include <Arduino.h>
#include <PS2.h>
#include <HMC5883L.h>

#define SCALE 1.3
#define DECLINATION 0.03199

class Position2 {
public:
	Position2();
	int getX();
	int getY();
	void update();
	double getOrientation();
	void reset();
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
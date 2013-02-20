#ifndef Position_h
#define Position_h

#include <Arduino.h>
#include <PS2.h>
#include <HMC5883L.h>
#include <Pins.h>
#include <Motion.h>
#include <EEPROM.h>

#define COMPASS_SCALE 1.3
#define COMPASS_DECLINATION 0.03199
#define NUMBER_OF_SAMPLES 180

class Position {
public:
	Position();
	int getX();
	int getY();
	double getOrientation();
	double getRawOrientation();
	void reset();
	void update();
	bool init();
	void calibrate(Motion &motion, bool newCalibration);
private:
	float updateAngle();
	void mouseInit();
	void compassMaxMin(int xRaw, int yRaw);
	void calcScaleFactor_Offset();
	void storeCal();
	void readCal();
	int x;
	int y;
	double relativeAngle;
	double northAngle;
	int xScaleFactor;
	int yScaleFactor;
	int compassXOffset;
	int compassYOffset;
	PS2 mouse;
	HMC5883L compass;
};
#endif
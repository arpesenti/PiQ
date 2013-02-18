#ifndef Line_h
#define Line_h

#include <Arduino.h>
#include <ADJDS311.h>
#include <Pins.h>
#include <Wire.h> // check if it's necessary

#define C_LOW 0
#define C_MEDIUM 1
#define C_HIGH 2

// threshold to determine the intensity of each RGB component
#define C_LOW_THRESHOLD 500
#define C_HIGH_THRESHOLD 600

class LineSensor {
public:
	LineSensor();
	void init();
	char color();
	int leftReflectance();
	int rightReflectance();
	int centerReflectance();
private:
	ADJDS311 colorSensor;
	int level(int component_intensity);
	
};

#endif

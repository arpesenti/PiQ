#ifndef Line_h
#define Line_h

#include <Arduino.h>
#include <ADJDS311.h>
#include <Pins.h>
#include <Wire.h> // check if it's necessary

#define LOW 0
#define MEDIUM 1
#define HIGH 2

// threshold to determine the intensity of each RGB component
#define LOW_THRESHOLD 100
#define HIGH_THRESHOLD 155

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

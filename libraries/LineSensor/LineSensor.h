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
#define C_LOW_THRESHOLD 600
#define C_HIGH_THRESHOLD 602

#define WINDOW_WIDTH 3
#define ALPHA_FILTER 0.8

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
	int level(double component_intensity);
	int window[WINDOW_WIDTH];
	int previousValueLeft;
	int previousValueRight;
	int previousValueCenter;
	int windowColorR[WINDOW_WIDTH];
	int windowColorG[WINDOW_WIDTH];
	int windowColorB[WINDOW_WIDTH];
	int previousValueColorR;
	int previousValueColorG;
	int previousValueColorB;


	int median(int x[]);
	int exponentialFilter(int value, int previous);
};

#endif

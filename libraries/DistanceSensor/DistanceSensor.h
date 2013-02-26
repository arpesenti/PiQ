#ifndef Distance_h
#define Distance_h

#include <Arduino.h>
#include <Pins.h> 

#define MAX_DIMENSION_SAMPLES 11
#define HISTORY_WIDTH 5
#define ALPHA_FILTER 0.8

struct sample{
	int distance;
	int value;
};

class DistanceSensor {
public:
	DistanceSensor();
	void initProximity();
	void initHighDistanceTop();
	void initHighDistanceBottom();
	double distance();
	void switchOn();
	void switchOff();
private:
	int pin;
	int pin5V;
	sample table[MAX_DIMENSION_SAMPLES]; 
	int num_samples;
	double previousValue;
	double history[HISTORY_WIDTH];
	double median(double x[]);
	double interpolate(double readValue);
	double exponentialFilter(double value);
};
#endif

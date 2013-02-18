#ifndef Distance_h
#define Distance_h

#include <Arduino.h>
#include <Pins.h> 

#define MAX_DIMENSION_SAMPLES 11

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
private:
	int pin;
	sample table[MAX_DIMENSION_SAMPLES]; 
	int num_samples;

};
#endif

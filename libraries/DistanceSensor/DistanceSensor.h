#ifndef Distance_h
#define Distance_h

#include <Arduino.h>
#include <Pins.h> 

class DistanceSensor {
public:
	DistanceSensor();
	void initProximity();
	void initHighDistanceTop();
	void initHighDistanceBottom();
	double distance();
private:
	int pin;

};
#endif

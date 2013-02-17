#include "DistanceSensor.h"

DistanceSensor::DistanceSensor() {}

void DistanceSensor::initProximity(){
	pin = PROXIMITY_PIN;
}

void DistanceSensor::initHighDistanceTop(){
	pin = HIGHDISTANCE_TOP_PIN;
}

void DistanceSensor::initHighDistanceBottom(){
	pin = HIGHDISTANCE_BOTTOM_PIN;
}

double DistanceSensor::distance() {
	return analogRead(pin);
}

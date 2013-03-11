#include "DistanceSensor.h"

DistanceSensor::DistanceSensor() {
	for(int i = 0; i < MAX_DIMENSION_SAMPLES; i++){
		table[i].distance = -1;
		table[i].value = -1;
	}
	for(int i = 0; i < HISTORY_WIDTH; i++) {
		history[i] = 0;
	}		
	previousValue = 0;
	previousTime = 0;
}

void DistanceSensor::initProximity(){
	pin = PROXIMITY_PIN;
	int distances[] = {3, 5, 7, 10, 16, 20, 50};
	int values[] = {600, 460, 345, 260, 180, 145, 100};
  num_samples = 7;
  for(int i = 0; i < num_samples; i++){
		table[i].distance = distances[i];
		table[i].value = values[i];
	}		 
}

void DistanceSensor::initHighDistanceTop(){
	pin = HIGHDISTANCE_TOP_PIN;
	int distances[] = {15, 20, 25, 30, 40, 50, 60, 70, 80, 100, 120, 150};
	int values[] = {540, 500, 440, 380, 300, 250, 205, 180, 160, 100, 80, 50};
  num_samples = 10;
  for(int i = 0; i < num_samples; i++){
		table[i].distance = distances[i];
		table[i].value = values[i];
	}		
}

void DistanceSensor::initHighDistanceBottom(){
	pin = HIGHDISTANCE_BOTTOM_PIN;
	int distances[] = {15, 20, 25, 30, 40, 50, 60, 70, 80, 100, 120, 150};
	int values[] = {540, 490, 435, 375, 295, 245, 200, 175, 160, 115, 100, 50};
  num_samples = 10;
  for(int i = 0; i < num_samples; i++){
		table[i].distance = distances[i];
		table[i].value = values[i];
	}	
}

double DistanceSensor::distance() {
	for(int i = 0; i < HISTORY_WIDTH; i++) {
		history[i] = analogRead(pin);
	}
	if (millis() - previousTime > 500) {
		previousValue = 0;
	}
	double value = exponentialFilter(median(history));
	previousTime = millis();
	return interpolate(value);
	//return value;	
}

double DistanceSensor::interpolate(double readValue) {
	int i = 0;
	while (i < num_samples && table[i].value > readValue){
		i = i + 1;
	}
	if (i == 0){
	  return table[0].distance;
	} else if (i == num_samples){
		return table[num_samples - 1].distance;
	}else{
		// linear interpolation
		double m = ((double)(table[i].value - table[i-1].value)) /  (table[i].distance - table[i-1].distance);
		return   (readValue - table[i-1].value) / m + table[i-1].distance;
	}
}

double DistanceSensor::median(double x[]) {
    double temp;
    int i, j;
    // the following two loops sort the array x in ascending order
    for(i=0; i<HISTORY_WIDTH-1; i++) {
        for(j=i+1; j<HISTORY_WIDTH; j++) {
            if(x[j] < x[i]) {
                // swap elements
                temp = x[i];
                x[i] = x[j];
                x[j] = temp;
            }
        }
    }
 
    if(HISTORY_WIDTH%2==0) {
        // if there is an even number of elements, return mean of the two elements in the middle
        return((x[HISTORY_WIDTH/2] + x[HISTORY_WIDTH/2 - 1]) / 2.0);
    } else {
        // else return the element in the middle
        return x[HISTORY_WIDTH/2];
    }
}

double DistanceSensor::exponentialFilter(double value) {
	previousValue = ALPHA_FILTER * value + (1 - ALPHA_FILTER) * previousValue;
	return previousValue;
}

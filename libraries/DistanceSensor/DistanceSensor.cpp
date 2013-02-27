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
}

void DistanceSensor::initProximity(){
	pin = PROXIMITY_PIN;
	int distances[] = {3, 5, 10 , 15, 20, 25, 30, 35};
	int values[] = {621, 325, 221, 178, 153, 136, 126, 113};
  num_samples = 8;
  for(int i = 0; i < num_samples; i++){
		table[i].distance = distances[i];
		table[i].value = values[i];
	}		 
}

void DistanceSensor::initHighDistanceTop(){
	pin = HIGHDISTANCE_TOP_PIN;
	int distances[] = {15, 20, 25, 30, 40, 60, 80, 100, 120, 150};
	int values[] = {540, 515, 470, 400, 295, 210, 140, 100, 80, 50};
  num_samples = 10;
  for(int i = 0; i < num_samples; i++){
		table[i].distance = distances[i];
		table[i].value = values[i];
	}		
}

void DistanceSensor::initHighDistanceBottom(){
	pin = HIGHDISTANCE_BOTTOM_PIN;
	int distances[] = {15, 20, 25, 30, 40, 60, 80, 100, 120, 150};
	int values[] = {527, 500, 455, 390, 280, 200, 150, 115, 100, 50};
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
	double value = exponentialFilter(median(history));
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

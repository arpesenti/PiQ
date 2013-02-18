#include "DistanceSensor.h"

DistanceSensor::DistanceSensor() {
	for(int i = 0; i < MAX_DIMENSION_SAMPLES; i++){
		table[i].distance = -1;
		table[i].value = -1;
	}		

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
	int distances[] = {10, 20, 30, 40, 50, 75, 100, 125, 150};
	int values[] = {585, 480, 360, 290, 230, 170, 135, 95, 80};
  num_samples = 9;
  for(int i = 0; i < num_samples; i++){
		table[i].distance = distances[i];
		table[i].value = values[i];
	}		
}

void DistanceSensor::initHighDistanceBottom(){
	pin = HIGHDISTANCE_BOTTOM_PIN;
	int distances[] = {10, 20, 30, 40, 50, 75, 100, 125, 150, 175, 200};
	int values[] = {550, 510, 400, 340, 270, 180, 120, 90, 60, 50, 40};
  num_samples = 11;
  for(int i = 0; i < num_samples; i++){
		table[i].distance = distances[i];
		table[i].value = values[i];
	}	
}

double DistanceSensor::distance() {
	double readValue = analogRead(pin);
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

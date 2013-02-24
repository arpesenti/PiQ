#include "LineSensor.h"

LineSensor::LineSensor(): colorSensor(COLOR_SENSOR_LED_PIN) {}

void LineSensor::init(){
	colorSensor.init();
  	colorSensor.ledOn(); //turn LED on
	//Calibrate white 
	//Need to hold white card in front (1-3mm) of it to calibrate from
	colorSensor.calibrate();
	Serial.println("");
	Serial.println("end of calibration of Color sensor"); 
	Serial.println("");

	//filter initialization
	previousValueLeft = 0;
	previousValueRight = 0;
	previousValueColorR = 0;
	previousValueColorG = 0;
	previousValueColorB = 0;
	for (int i = 0; i < WINDOW_WIDTH; i++) {
		window[i] = 0;
		windowColorR[i] = 0;
		windowColorG[i] = 0;
		windowColorB[i] = 0;
	}
}

char LineSensor::color() {
	for (int i = 0; i< WINDOW_WIDTH; i++) {
		RGBC color = colorSensor.read();
		windowColorR[i] = color.red;
		windowColorG[i] = color.green;
		windowColorB[i] = color.blue;
	}

	int redValue = exponentialFilter(median(windowColorR), previousValueColorR);
	int greenValue = exponentialFilter(median(windowColorG), previousValueColorG);
	int blueValue = exponentialFilter(median(windowColorB), previousValueColorB);

	previousValueColorR = redValue;
	previousValueColorG = greenValue;
	previousValueColorB = blueValue;

	int Rlevel = level(redValue);
	int Glevel = level(greenValue);
	int Blevel = level(blueValue);

	if (Rlevel == C_HIGH && Glevel == C_HIGH && Blevel == C_HIGH) // white
		return 'w';
	if (Rlevel == C_LOW && Glevel == C_LOW && Blevel == C_LOW) // black
		return 'k';
	if (Rlevel == C_LOW && Glevel == C_LOW && Blevel == C_HIGH) // blue
		return 'b';

	// not strictly necessary for our purpose, just for testing the sensor
	
	// CYAN -> HIGH: BLU GREEN
	//  YELLOW -> HIGH: RED GREEN
	//  MAGENTA -> HIGH: BLU RED
	if (Rlevel == C_HIGH && Glevel == C_LOW && Blevel == C_LOW) // red
		return 'r';
	if (Rlevel == C_LOW && Glevel == C_HIGH && Blevel == C_LOW) // green
		return 'g';
	if (Rlevel == C_HIGH && Glevel == C_HIGH && Blevel == C_LOW) // yellow
		return 'y';
	if (Rlevel == C_HIGH && Glevel == C_LOW && Blevel == C_HIGH) // magenta
		return 'm';
	if (Rlevel == C_LOW && Glevel == C_HIGH && Blevel == C_HIGH) // cyan
		return 'c';
	
	return 'n'; // not a known color
}

int LineSensor::leftReflectance() {
	for (int i = 0; i< WINDOW_WIDTH; i++) {
		window[i] = analogRead(LEFT_REFLECTANCE_PIN);
	}
	previousValueLeft = exponentialFilter(median(window), previousValueLeft);
	return previousValueLeft;
}

int LineSensor::rightReflectance() {
	for (int i = 0; i< WINDOW_WIDTH; i++) {
		window[i] = analogRead(RIGHT_REFLECTANCE_PIN);
	}
	previousValueRight = exponentialFilter(median(window), previousValueRight);
	return previousValueRight;
}

//  Lower value is an indication of greater reflection
int LineSensor::centerReflectance() {
	for (int i = 0; i< WINDOW_WIDTH; i++) {
		RGBC color = colorSensor.read();
		window[i] = (1023 - color.clear);
	}
	previousValueCenter = exponentialFilter(median(window), previousValueCenter);
	return previousValueCenter;
}

int LineSensor::level(int component_intensity){
	if (component_intensity < C_LOW_THRESHOLD)
		return C_LOW;
	if (component_intensity > C_HIGH_THRESHOLD)
		return C_HIGH;
	return C_MEDIUM;
}

int LineSensor::median(int x[]) {
    int temp;
    int i, j;
    // the following two loops sort the array x in ascending order
    for(i=0; i<WINDOW_WIDTH-1; i++) {
        for(j=i+1; j<WINDOW_WIDTH; j++) {
            if(x[j] < x[i]) {
                // swap elements
                temp = x[i];
                x[i] = x[j];
                x[j] = temp;
            }
        }
    }
 
    if(WINDOW_WIDTH%2==0) {
        // if there is an even number of elements, return mean of the two elements in the middle
        return((x[WINDOW_WIDTH/2] + x[WINDOW_WIDTH/2 - 1]) / 2.0);
    } else {
        // else return the element in the middle
        return x[WINDOW_WIDTH/2];
    }
}

int LineSensor::exponentialFilter(int value, int previous) {
	double x = ALPHA_FILTER * value + (1 - ALPHA_FILTER) * previous;
	return (int)x;
}


#include "LineSensor.h"

LineSensor::LineSensor(): colorSensor(COLOR_SENSOR_LED_PIN) {}

void LineSensor::init(){
	colorSensor.init();
  	colorSensor.ledOn(); //turn LED on
	//Calibrate white 
	//Need to hold white card in front (1-3mm) of it to calibrate from
	colorSensor.calibrate();
	Serial.println("end of calibration of Color sensor"); 
}

char LineSensor::color() {
	RGBC color = colorSensor.read();
	int Rlevel = level(color.red);
	int Glevel = level(color.green);
	int Blevel = level(color.blue);

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
	if (Rlevel == C_HIGH && Glevel == C_LOW && Blevel == HIGH) // magenta
		return 'm';
	if (Rlevel == C_LOW && Glevel == C_HIGH && Blevel == C_HIGH) // cyan
		return 'c';
	
	return 'n'; // not a known color
}

int LineSensor::leftReflectance() {
	return analogRead(LEFT_REFLECTANCE_PIN);
}

int LineSensor::rightReflectance() {
	return analogRead(RIGHT_REFLECTANCE_PIN);
}

//  Lower value is an indication of greater reflection
int LineSensor::centerReflectance() {
	RGBC color = colorSensor.read();
	return (1023 - color.clear);
}

int LineSensor::level(int component_intensity){
	if (component_intensity < LOW_THRESHOLD)
		return C_LOW;
	if (component_intensity > HIGH_THRESHOLD)
		return C_HIGH;
	return C_MEDIUM;
}

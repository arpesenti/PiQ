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

	if (Rlevel == HIGH && Glevel == HIGH && Blevel == HIGH) // white
		return 'w';
	if (Rlevel == LOW && Glevel == LOW && Blevel == LOW) // black
		return 'k';
	if (Rlevel == LOW && Glevel == LOW && Blevel == HIGH) // blue
		return 'b';

	// not strictly necessary for our purpose, just for testing the sensor
	
	// CYAN -> HIGH: BLU GREEN
	//  YELLOW -> HIGH: RED GREEN
	//  MAGENTA -> HIGH: BLU RED
	if (Rlevel == HIGH && Glevel == LOW && Blevel == LOW) // red
		return 'r';
	if (Rlevel == LOW && Glevel == HIGH && Blevel == LOW) // green
		return 'g';
	if (Rlevel == HIGH && Glevel == HIGH && Blevel == LOW) // yellow
		return 'y';
	if (Rlevel == HIGH && Glevel == LOW && Blevel == HIGH) // magenta
		return 'm';
	if (Rlevel == LOW && Glevel == HIGH && Blevel == HIGH) // cyan
		return 'c';
	
	return 'n'; // not a known color
}

int LineSensor::leftReflectance() {
	return analogRead(LEFT_REFLECTANCE_PIN);
}

int LineSensor::rightReflectance() {
	return analogRead(RIGHT_REFLECTANCE_PIN);
}

int LineSensor::centerReflectance() {
	RGBC color = colorSensor.read();
	return color.clear;
}

int LineSensor::level(int component_intensity){
	if (component_intensity < LOW_THRESHOLD)
		return LOW;
	if (component_intensity > HIGH_THRESHOLD)
		return HIGH;
	return MEDIUM;
}

#include "Position.h"

#define SCALE 1.3
#define DECLINATION 0.03199

Position::Position(): mouse(CLOCK_PIN, DATA_PIN), compass() {
	Serial.println("Position constructor");
	x = 0;
	y = 0;
}

bool Position::init() {
	// inizitialize mouse
	mouseInit();
	// initialize compass
	int error = compass.SetScale(SCALE);
	if (error != 0) {
		Serial.println(compass.GetErrorText(error));
		return false;
	}
	error = 0;
	error = compass.SetMeasurementMode(Measurement_Continuous);
	if (error != 0) {
		Serial.println(compass.GetErrorText(error));
		return false;
	}
	MagnetometerScaled scaled = compass.ReadScaledAxis();
	relativeAngle = atan2(scaled.YAxis, scaled.XAxis) + DECLINATION; 
	northAngle = fmod(PI/2 - relativeAngle, 2*PI); // the robot is placed initially at 90 degree
	return true;
}

// Position::Position(int clockPinMouse, int dataPinMouse): mouse(clockPinMouse, dataPinMouse), compass() {
// 	Serial.println("Position constructor");
// 	x = 0;
// 	y = 0;
// 	// inizitialize mouse
// 	mouseInit();
// 	// initialize compass
// 	int error = compass.SetScale(SCALE);
// 	if (error != 0) {
// 		Serial.println(compass.GetErrorText(error));
// 	}
// 	error = 0;
// 	error = compass.SetMeasurementMode(Measurement_Continuous);
// 	if (error != 0) {
// 		Serial.println(compass.GetErrorText(error));
// 	}
// 	MagnetometerScaled scaled = compass.ReadScaledAxis();
// 	relativeAngle = atan2(scaled.YAxis, scaled.XAxis) + DECLINATION; 
// 	northAngle = fmod(PI/2 - relativeAngle, 2*PI); // the robot is placed initially at 90 degree
// }

int Position::getX() {
	return x;
}

int Position::getY() {
	return y;
}

double Position::getOrientation() {
	return fmod(northAngle + relativeAngle, 2*PI);
}

void Position::update() {
	mouse.write(0xeb); // ask data
	mouse.read(); // ignore acknowledgement 
	mouse.read(); // mstat
	char dx = mouse.read();
	char dy = mouse.read();
	double alpha = (relativeAngle + northAngle) - PI/2;
	x = x + cos(alpha)*dx - sin(alpha)*dy;
	y = y + sin(alpha)*dx + cos(alpha)*dy;
	relativeAngle = updateAngle();
}

void Position::reset() {
	// PRECONDITION: robot must have exactly at his back the home
	x = 0;
	y = 0;
}

double Position::updateAngle() {
	MagnetometerScaled scaled = compass.ReadScaledAxis();
	float heading = atan2(scaled.YAxis, scaled.XAxis);
	heading += DECLINATION;
	if (heading < 0) heading += 2 * PI;
	return heading;
}

void Position::mouseInit() {
  mouse.write(0xff);  // reset
  mouse.read();  // ack byte
  mouse.read();  // blank */
  mouse.read();  // blank */
  mouse.write(0xf0);  // remote mode
  mouse.read();  // ack
  delayMicroseconds(100);
}
#include "Position2.h"

Position2::Position2(): mouse(6,5), compass() {
	x = 0;
	y = 0;
}

int Position2::getX() {
	return x;
}

int Position2::getY() {
	return y;
}

double Position2::getOrientation() {
	return fmod(northAngle + relativeAngle, 2*PI);
}

void Position2::update() {
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

void Position2::reset() {
	// PRECONDITION: robot must have exactly at his back the home
	x = 0;
	y = 0;
}

double Position2::updateAngle() {
	MagnetometerScaled scaled = compass.ReadScaledAxis();
	float heading = atan2(scaled.YAxis, scaled.XAxis);
	heading += DECLINATION;
	if (heading < 0) heading += 2 * PI;
	return heading;
}

void Position2::mouseInit() {
  mouse.write(0xff);  // reset
  mouse.read();  // ack byte
  mouse.read();  // blank */
  mouse.read();  // blank */
  mouse.write(0xf0);  // remote mode
  mouse.read();  // ack
  delayMicroseconds(100);
}
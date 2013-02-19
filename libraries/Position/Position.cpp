#include "Position.h"

int xMax;
int yMax;
int xMin;
int yMin;

Position::Position(): mouse(MOUSE_CLOCK_PIN, MOUSE_DATA_PIN), compass() {
	x = 0;
	y = 0;
}

bool Position::init() {
	// inizitialize mouse
	mouseInit();
	// initialize compass
  	int error = compass.SetScale(COMPASS_SCALE);
	if (error == 1) {
		Serial.println(compass.GetErrorText(error));
		return false;
	}
	error = 0;
	error = compass.SetMeasurementMode(Measurement_Continuous);
	if (error == 1) {
		Serial.println(compass.GetErrorText(error));
		return false;
	}
	readCal(); // TODO: check if calibration is already in EEPROM. If not initialize xMax,.. to 0
	relativeAngle = updateAngle();
	northAngle = fmod(2*PI + PI/2 - relativeAngle, 2*PI);
  	Serial.println(northAngle);
  	Serial.println(relativeAngle);
	Serial.println("end of initialization of Position"); 
	Serial.println("");
	return true;
}


int Position::getX() {
	return x;
}

int Position::getY() {
	return y;
}

double Position::getOrientation() {
	return fmod(2*PI + northAngle + relativeAngle, 2*PI);
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

	// clear mouse buffer
	mouse.write(0xeb); // ask data
	mouse.read(); // ignore acknowledgement 
	mouse.read(); // mstat
	char dx = mouse.read();
	char dy = mouse.read();

	x = 0;
	y = 0;
}

float Position::updateAngle() {
	MagnetometerScaled scaled = compass.ReadScaledAxis();
	//compensate for compass location, using stored calibration data
  	scaled.XAxis = (xScaleFactor * scaled.XAxis) + compassXOffset;
  	scaled.YAxis = (yScaleFactor * scaled.YAxis) + compassYOffset;
  
   	// Calculate heading when the magnetometer is level, then correct for signs of axis.
  	float heading = atan2(scaled.YAxis, scaled.XAxis);

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

void Position::calibrate(Motion &motion) {
	int speed = 20;
	motion.rotateLeft(speed);
	for (int index = 0; index < 16; index++)
      {
          for(int x=0;x<100;x++)  //continualy read the raw axis data while waiting for operator to slowly rotate compass to next position
          {
            MagnetometerScaled scaled = compass.ReadScaledAxis();
            compassMaxMin(scaled.XAxis,scaled.YAxis);  //look for max and min values
            delay(20);
          }
          
      } 
    motion.stop();
    calcScaleFactor_Offset();   
    storeCal();    //save the calculated x & y offset and scale factor
}

void Position::compassMaxMin(int xRaw, int yRaw)
{
  if(xRaw>500 || xRaw<-500 || yRaw>500 || yRaw<-500)
    return; //prevent extraneous high readings from messing with calibration
    
  if(xRaw > xMax)
    xMax = xRaw;

  if(xRaw < xMin)
    xMin = xRaw;
  
  if(yRaw > yMax)
    yMax = yRaw;
  
  if(yRaw < yMin)
    yMin = yRaw;
   
}

void Position::calcScaleFactor_Offset()
{
  xScaleFactor = (yMax - yMin)/(xMax - xMin);
  if(xScaleFactor < 1)
    xScaleFactor = 1;

  yScaleFactor = (xMax - xMin)/(yMax - yMin);
  if(yScaleFactor < 1)
    yScaleFactor = 1;

  compassXOffset = ((xMax - xMin)/2 - xMax) * xScaleFactor;
  compassYOffset = ((yMax - yMin)/2 - yMax) * yScaleFactor;
   
}

void Position::storeCal(){ 
  //write x & y scale factors and offsets to eeprom  
    EEPROM.write(0,lowByte(compassXOffset));    
    EEPROM.write(1,highByte(compassXOffset));
    
    EEPROM.write(2,lowByte(compassYOffset));    
    EEPROM.write(3,highByte(compassYOffset));
    
    EEPROM.write(4,lowByte(xScaleFactor));    
    EEPROM.write(5,highByte(xScaleFactor));
    
    EEPROM.write(6,lowByte(yScaleFactor));    
    EEPROM.write(7,highByte(yScaleFactor));
     
}

void Position::readCal(){
  
  compassXOffset = (EEPROM.read(1) * 256) + EEPROM.read(0);
  compassYOffset = (EEPROM.read(3) * 256) + EEPROM.read(2);
  xScaleFactor = (EEPROM.read(5) * 256) + EEPROM.read(4);
  yScaleFactor = (EEPROM.read(7) * 256) + EEPROM.read(6);
  
}
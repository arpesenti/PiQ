
/*
HMC5883L_Example.pde - Example sketch for integration with an HMC5883L triple axis magnetomerwe.
Copyright (C) 2011 Love Electronics (loveelectronics.co.uk)

Modified to compensate for nearby ferrous materials that may distort the earth's magnetic field.
Using this appnote from Honeywell for the principle and math
http://www.ssec.honeywell.com/position-sensors/datasheets/sae.pdf
Modified by: Richard Heiliger

This program is free software: you can redistribute it and/or modify
it under the terms of the version 3 GNU General Public License as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

// Reference the I2C Library
#include <Wire.h>
// Reference the HMC5883L Compass Library
#include <HMC5883L.h>
#include <EEPROM.h>

// Store our compass as a variable.
HMC5883L compass;
// Record any errors that may occur in the compass.
int error = 0;
MagnetometerRaw magraw;
MagnetometerScaled scaled;
int xScaleFactor;
int yScaleFactor;
int compassXOffset;
int compassYOffset;
float headingDegrees;
int xMax;
int yMax;
int xMin;
int yMin;

// Out setup routine, here we will configure the microcontroller and compass.
void setup()
{
  // Initialize the serial port.
  Serial.begin(9600);

  Serial.println("Starting the I2C interface.");
  Wire.begin(); // Start the I2C interface.

  Serial.println("Constructing new HMC5883L");
  compass = HMC5883L(); // Construct a new HMC5883 compass.
    
  Serial.println("Setting scale to +/- 1.3 Ga");
  error = compass.SetScale(1.3); // Set the scale of the compass.
  if(error != 0) // If there is an error, print it out.
    Serial.println(compass.GetErrorText(error));
  
  Serial.println("Setting measurement mode to continous.");
  error = compass.SetMeasurementMode(Measurement_Continuous); // Set the measurement mode to Continuous
  if(error != 0) // If there is an error, print it out.
    Serial.println(compass.GetErrorText(error));

//run the compensation    
getFerrousCompensation();   
    
}

// Our main program loop.
void loop()
{
    // Retrived the scaled values from the compass (scaled to the configured scale).
  magraw = compass.ReadRawAxis();
  scaled = compass.ReadScaledAxis();
  
  //compensate for compass location, using stored calibration data
  scaled.XAxis = (xScaleFactor * scaled.XAxis) + compassXOffset;
  scaled.YAxis = (yScaleFactor * scaled.YAxis) + compassYOffset;
  
   // Calculate heading when the magnetometer is level, then correct for signs of axis.
  float heading = atan2(scaled.YAxis, scaled.XAxis);
  
  // Once you have your heading, you must then add your 'Declination Angle', which is the 'Error' of the magnetic field in your location.
  // Find yours here: http://www.magnetic-declination.com/
  // Mine is: 2ï¿½ 37' W, which is 2.617 Degrees, or (which we need) 0.0456752665 radians, I will use 0.0457
  // If you cannot find your Declination, comment out these two lines, your compass will be slightly off.
  
  float declinationAngle = 0.0457;  
  heading += declinationAngle;
  
  // Correct for when signs are reversed.
  if(heading < 0)
    heading += 2*PI;
    
  // Check for wrap due to addition of declination.
  if(heading > 2*PI)
    heading -= 2*PI;
   
  // Convert radians to degrees for readability.
  headingDegrees = heading * 180/M_PI; 
  
  // Output the data via the serial port.
  Output(magraw, scaled, heading, headingDegrees);
  // Normally we would delay the application by 66ms to allow the loop
  // to run at 15Hz (default bandwidth for the HMC5883L).
  // However since we have a long serial out (104ms at 9600) we will let
  // it run at its natural speed.
  // delay(66);
}

// Output the data down the serial port.
void Output(MagnetometerRaw raw, MagnetometerScaled scaled, float heading, float headingDegrees)
{
   Serial.print("Raw:\t");
   Serial.print(raw.XAxis);
   Serial.print("   ");   
   Serial.print(raw.YAxis);
   Serial.print("   ");   
   Serial.print(raw.ZAxis);
   Serial.print("   \tScaled:\t");
   
   Serial.print(scaled.XAxis);
   Serial.print("   ");   
   Serial.print(scaled.YAxis);
   Serial.print("   ");   
   Serial.print(scaled.ZAxis);

   Serial.print("   \tHeading:\t");
   Serial.print(heading);
   Serial.print(" Radians   \t");
   Serial.print(headingDegrees);
   Serial.println(" Degrees   \t");
}


//*********************************************
//output of data, to check raw mag values, scaled mag values, x&y max/min , x&y offsets & scale factor
void calibrationOutput()
{
  //raw magnetometer x&y values
  Serial.print("RawMag x=");
  Serial.print(magraw.XAxis);
  Serial.print(",y=");
  Serial.print(magraw.YAxis);
  Serial.print(" ");
  //mag values scled with gause gain setting  
  Serial.print("ScaledMag x=");
  Serial.print(scaled.XAxis);
  Serial.print(",y=");
  Serial.print(scaled.YAxis);
  Serial.print(" ");  
  //current max min value as calibration proceeds
  Serial.print(" xMax=");
  Serial.print(xMax);
  Serial.print(",yMax=");
  Serial.print(yMax);
  Serial.print(", xMin=");
  Serial.print(xMin);
  Serial.print(", yMin=");
  Serial.println(yMin);

}

//********************************
//calibration data after completed
void calibrationComplete()
{
  //min/max values found
  Serial.print("MaxMin xMax=");
  Serial.print(xMax);
  Serial.print(",yMax=");
  Serial.print(yMax);
  Serial.print(", xMin=");
  Serial.print(xMin);
  Serial.print(", yMin=");
  Serial.print(yMin);
  //calculated offset and scale factors
  Serial.print("xScale=");
  Serial.print(xScaleFactor);
  Serial.print(",yScale=");
  Serial.print(yScaleFactor);
  Serial.print(", XOffset=");
  Serial.print(compassXOffset);
  Serial.print(", YOffset=");
  Serial.print(compassYOffset);  

  

}

/**********************************************************
* Calibrate
*  Performs a chip calibration
*
* @return bool - The calibration result
**********************************************************/
void getFerrousCompensation(void) {
     
      for (int index = 0; index < 16; index++)
      {
          
          Serial.println("SLOWLY ROTATE COMPASS THRU AT LEAST 360 DEGREES");
          Serial.println("COMPENSATION FUNCTION WILL END IN 20 SECONDS");
                 
          //delay(5000);  //wait a couple seconds for operator to get arrow ponted north
          //delayMillis = millis() + 2000;
          for(int x=0;x<100;x++)  //continualy read the raw axis data while waiting for operator to slowly rotate compass to next position
          {
            magraw = compass.ReadRawAxis();
            scaled = compass.ReadScaledAxis();
            compassMaxMin(scaled.XAxis,scaled.YAxis);  //look for max and min values
            calibrationOutput();  //output current data as calibration proceeds
            delay(20);
          }
          
      } 
    calcScaleFactor_Offset();
    calibrationComplete();  //output the resulting values from calibration    
    storeCal();    //save the calculated x & y offset and scale factor
    Serial.println("OFFSET AND SCALE FACTOR STORED IN EEPROM");   
    Serial.println("COMPENSATION FUNCTION COMPLETED");
    
}

//used calibrating hmc5883l compass
//*******************************************************
void compassMaxMin(int xRaw, int yRaw)
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

//*******************************************************************
//calculate the x&y scale factor and offsets for hmc5883l
//compensates for objects around sensor distorting magnetometer values
void calcScaleFactor_Offset()
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

/*************************************************************************/  
//save calibration data to eeprom
void storeCal(){ 
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

/*************************************************************************/  
//read calibration data from eeprom
void readCal(){
  
  compassXOffset = (EEPROM.read(1) * 256) + EEPROM.read(0);
  compassYOffset = (EEPROM.read(3) * 256) + EEPROM.read(2);
  xScaleFactor = (EEPROM.read(5) * 256) + EEPROM.read(4);
  yScaleFactor = (EEPROM.read(7) * 256) + EEPROM.read(6);
  
}
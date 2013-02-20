#include <AFMotor.h>  
#include <RobotMotor.h>  
#include <LineSensor.h>
#include <Pins.h>
#include <Motion.h>
#include <ADJDS311.h>
#include <Wire.h>

Motion motion;
LineSensor lineSensor;
int speed = 20; // speed in percent when moving along a straight line

void setup()
{
  Serial.begin(9600); 
  motion.init();
  lineSensor.init();
  Serial.println("Ready");  
}

void loop()
{
  int drift = lineSense();
  Serial.println("");
  Serial.print("Drift: ");
  Serial.print(drift);
  motion.moveForwardWithDrift(speed, drift);
  delay(300);
}

//returns drift - 0 if over line, minus value if left, plus if right
int lineSense()
{
  int leftVal = lineSensor.leftReflectance();
  int centerVal = lineSensor.centerReflectance();
  int rightVal = lineSensor.rightReflectance();
  Serial.println("");
  Serial.print("Reflectance: ");
  Serial.print(leftVal);
  Serial.print(", ");
  Serial.print(centerVal);
  Serial.print(", ");
  Serial.print(rightVal);
  Serial.print(", Color: ");
  Serial.print(lineSensor.color());
  int leftSense = centerVal - leftVal;
  int rightSense = rightVal - centerVal;
  int drift = rightVal - leftVal ;
  return drift;
}


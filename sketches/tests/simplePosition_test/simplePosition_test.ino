#include <Position.h>
#include <PS2.h>
#include <HMC5883L.h>
#include <Wire.h>
#include <Pins.h>
#include <EEPROM.h>
#include <Position.h>
#include <Motion.h>
#include <AFMotor.h>
#include <RobotMotor.h>

Position position;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  position.init();
}

void loop() {
  position.update();
  Serial.print("X: ");
  Serial.print(position.getX());
   position.update();
  Serial.print("   Y: ");
  Serial.println(position.getY());
   position.update();
  //Serial.print("   orientation: ");
  //Serial.print(position.getOrientation()); 
  // position.update();
  //Serial.print("     ");
  //Serial.println(sqrt(square(position.getX()) + square(position.getY())));
   //position.update();
  //delay(200);
}
  
  

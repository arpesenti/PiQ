#include <Motion.h>
#include <Position.h>

#include <AFMotor.h>
#include <RobotMotor.h>
#include <Pins.h>
#include <PS2.h>
#include <HMC5883L.h>
#include <Wire.h>

Motion motion;

void setup(){
	motion.init();
  Serial.begin(9600);
}

void loop(){
  motion.moveForward(50);
  delay(1000);
  motion.stop();
  motion.moveBackward(100);
  delay(1000);
  motion.stop();
  motion.rotateLeft(10);
  delay(1000);
  motion.stop();
  motion.rotateRight(20);
  delay(1000);
  motion.stop();
  delay(1000);
}

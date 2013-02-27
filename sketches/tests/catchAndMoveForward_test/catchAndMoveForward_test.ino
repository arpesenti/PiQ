#include <Robot.h>
#include <Position.h>
#include <Motion.h>
#include <DistanceSensor.h>
#include <LineSensor.h>
#include <Feet.h>
#include <Remote.h>
#include <Pins.h>
#include <PS2.h>
#include <HMC5883L.h>
#include <Servo.h>
#include <IRremote.h>
#include <ADJDS311.h>
#include <Wire.h>
#include <AFMotor.h>
#include <RobotMotor.h>
#include <EEPROM.h>


Feet feet;
Motion motion;

void setup() {
  Serial.begin(9600);
  feet.init();
  motion.init();
  feet.open();
  delay(1000);
  feet.close();
}

void loop() {
  motion.moveForward(50);
  delay(10000);
  motion.stop();
  //feet.open(80);
  //delay(1000);
  //feet.close(80);
  //delay(1000);
  //feet.trick();
  //delay(5000);
}
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
#include <SoftwareServo.h>
#include <IRremote.h>
#include <ADJDS311.h>
#include <Wire.h>
#include <AFMotor.h>
#include <RobotMotor.h>
#include <EEPROM.h>
#include <Eyes.h>

// Robot
Robot robot;
int state;
bool result;
int cruiseSpeed;
int rotationalCruiseSpeed;

DistanceSensor prox;
DistanceSensor highTop;
DistanceSensor highBottom;

void setup(){
  Serial.begin(9600);
  robot.init();
  prox.initProximity();
  highTop.initHighDistanceTop();
  highBottom.initHighDistanceBottom();
  Serial.println("end initialization");
}

void loop(){
  Serial.println("");
  Serial.print("Proximity  ");
  Serial.print(prox.distance());
  Serial.print("      Top  ");
  Serial.print(highTop.distance());
  Serial.print("    Bottom ");
  Serial.println(highBottom.distance());
  //delay(500);
}

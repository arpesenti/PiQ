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

// Robot
Robot robot;
int state;
bool result;
int cruiseSpeed;
int rotationalCruiseSpeed;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  robot.init();
  Serial.println("Robot init finished ----------");
  delay(3000);
  
  result = robot.scanForEgg();
  if (result == true) {
    Serial.println("Egg found! -------------");
    Serial.println("Trying to reach it! -------------");
    delay(3000);
    result = robot.reachEgg();
    if (result == true)
      Serial.println("Egg reached! -------------");
    else
      Serial.println("Not reached -------------");
  } else {
    Serial.println("Not found --------------"); 
  }
  if (state == PANIC)
    Serial.println("------ PANIC -------");
}

void loop() {
  
}

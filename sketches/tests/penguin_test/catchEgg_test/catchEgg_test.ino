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

// Robot
Robot robot;
int state;
bool result;
int cruiseSpeed;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  robot.init();
  Serial.println("Robot init finished ----------");
  delay(3000);
  
  // PRECONDINDITION: egg in front
  result = robot.catchEgg();
  if (result == true)
    Serial.println("Egg catched! -------------");
    robot
  else
    Serial.println("Not catched --------------"); 
  if (state == PANIC)
    Serial.println("------ PANIC -------");
}

void loop() {
  
}

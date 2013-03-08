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
  delay(5000);
  
  // PRECONDITION: in front of the home
  robot.catchEgg();
  delay(3000);
  result = robot.deposit();
  if (result == true) {
    Serial.println("Egg deposited! -------------");
    result = robot.newInit();
    if (result == true) 
      Serial.println("New init done! -------------");
    else 
      Serial.println("Failed --------------");
  } else {
    Serial.println("Failed --------------"); 
  }
  if (state == PANIC)
    Serial.println("------ PANIC -------");
}

void loop() {
  
}

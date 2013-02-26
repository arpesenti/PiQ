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
  result = robot.positioningTowardHome();
  if (result == true) {
    Serial.println("Positioned toward home! -------------");
    delay(3000);
    int res = robot.tryToApproach();
    if (res == APPROACH_FAILED) {
      Serial.println("Approach failed! -------------");
    } else if (res == APPROACH_FOUND_LINE) {
      Serial.println("Found line! -------------");
      Serial.println("Starting following the line! -------------");
      delay(3000);
      result = robot.followLineToHome();
      if (result == true) {
        Serial.println("Arrived home! -------------");
      } else {
        Serial.println("Line following failed! -------------");
      }
    } else if (res == APPROACH_NOT_FOUND_LINE) {
      Serial.println("Line not found! -------------");
      Serial.println("Searching line! -------------");
      delay(3000);
      result = robot.searchLine();
      if (result == true) {
        Serial.println("Line found! -------------");
        Serial.println("Starting following the line! -------------");
        delay(3000);
        result = robot.followLineToHome();
        if (result == true) {
          Serial.println("Arrived home! -------------");
        } else {
          Serial.println("Line following failed! -------------");
        }
      } else {
        Serial.println("Line not found! -------------");
      }
    }
  } else {
    Serial.println("Not found --------------"); 
  }
  if (state == PANIC)
    Serial.println("------ PANIC -------");
}

void loop() {
  
}


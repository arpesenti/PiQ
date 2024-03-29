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
  delay(2000);
  robot.catchEgg();
  // PRECONDITION: robot nearby the home
  result = robot.searchLine();
  if (result == true) {
    Serial.println("Line found! -------------");
    delay(3000);
    result = robot.followLineToHome();
    if (result == true) {
      Serial.println("Arrived home! -------------");
    } else {
      Serial.println("Failed folowing line! -------------");
    }
  } else {
    Serial.println("Line Not found --------------"); 
  }
  if (state == PANIC) {
    Serial.println("------ PANIC -------");
  }
}

void loop() {
  
}

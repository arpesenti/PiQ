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
int result;
int cruiseSpeed;
int rotationalCruiseSpeed;

void setup() {
        Serial.begin(9600);
        Wire.begin();
	robot.init();
        robot.start();
        Serial.println("init");
        delay(3000);
        
}

void loop(){
  Serial.println("start search");
  robot.searchEggSimple();
  delay(1000);

  Serial.println("come back home simple");
  Serial.println(robot.comeBackHomeSimple());
  delay(1000);
}

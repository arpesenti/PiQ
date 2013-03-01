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


Robot robot;
int state;
int cruiseSpeed;
int rotationalCruiseSpeed;

void setup() {
        Serial.begin(9600);
        Wire.begin();
	robot.init();
        robot.recalibrate();
        delay(3000);
        robot.rotateLeft(3*PI/4);
         delay(3000);
 }

void loop() {
  
}

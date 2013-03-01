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
#include <SoftwareServo.h>
Robot robot;
int state;
int cruiseSpeed;
int rotationalCruiseSpeed;

void setup() {
        Serial.begin(9600);
        Wire.begin();
	robot.init();
        //robot.recalibrate();
        delay(3000);
        robot.start();
        delay(1000);
        robot.catchEgg();
        delay(1000);
        Serial.println("init");
        robot.rotateRight(3*PI/4);
        delay(1000);
        robot.changePosition();
        delay(1000);
        robot.rotateLeft(3*PI/4);
        delay(1000);
        robot.changePosition();
        delay(1000);
        robot.positioningTowardHome();
        Serial.print("Positioned");
        delay(1000);
        while ( robot.tryToApproach() == APPROACH_FAILED){
          robot.positioningTowardHome();
          delay(1000);
          Serial.println("reposition");
        }
        Serial.println("Arrived home hopefully");
}

void loop() {
  
}

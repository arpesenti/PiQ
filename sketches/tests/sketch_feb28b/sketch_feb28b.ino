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
        delay(2000);
        
        // test real start
        robot.start();
        Serial.println("end of start");
        delay(2000);
        
        // test rotate
        Serial.print("rotation to free direction   :");
        Serial.println(robot.rotateToFreeDirection());
        delay(500);
        
        // test scan
        while(robot.scanForEgg())
          robot.changePosition();
         
         Serial.println (" egg found - exit from cycles of scan/change");
          delay(1000);
          
         robot.reachEgg();
        
}

void loop(){
  
}

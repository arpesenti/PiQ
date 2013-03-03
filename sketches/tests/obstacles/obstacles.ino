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
int result;
int cruiseSpeed;
int rotationalCruiseSpeed;

Motion motion;
DistanceSensor highTop;
DistanceSensor prox;
DistanceSensor highBottom;

double highDistanceTop = 0;
double proxDistance = 15;

void setup() {
        Serial.begin(9600);
        Wire.begin();
        prox.initProximity();
        highTop.initHighDistanceTop();
        highBottom.initHighDistanceBottom();
        highDistanceTop =  highTop.distance();
        proxDistance = prox.distance();
        Serial.print("Proximity  ");
        Serial.print(proxDistance);
        Serial.print("      Top  ");
        Serial.println(highDistanceTop);
        Serial.println("start");
  
        
}

void loop(){
  motion.moveForward(10);
  //Serial.println("");
  //Serial.print("Proximity  ");
  //Serial.print(prox.distance());
  highDistanceTop =  highTop.distance();
  proxDistance = prox.distance();
  Serial.print("Proximity  ");
  Serial.print(proxDistance);
  Serial.print("      Top  ");
  Serial.println(highDistanceTop);
  if(highDistanceTop < 30 || proxDistance < 7){
    motion.stop();
    Serial.println("stopped");
    delay(500);
    
  }
 
  //Serial.print("    Bottom ");
  //Serial.println(highBottom.distance());
  
}

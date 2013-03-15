#include <Motion.h>
#include <Position.h>

#include <AFMotor.h>
#include <RobotMotor.h>
#include <Pins.h>
#include <PS2.h>
#include <HMC5883L.h>
#include <Wire.h>

Motion motion;

void setup(){
    motion.init();
    Serial.begin(9600);
    delay(500);
    motion.moveForward(50);
    delay(500);
}

void loop(){
  
}

#include <Motion.h>
#include <Position.h>

#include <AFMotor.h>
#include <RobotMotor.h>
#include <Pins.h>
#include <PS2.h>
#include <HMC5883L.h>
#include <Wire.h>
#include <EEPROM.h>

Motion motion;

void setup(){
    motion.init();
    Serial.begin(9600);
    delay(10000);
    motion.moveForward(30);
    delay(5000);
    motion.stop();
    //motion.penguinWalking();
    delay(500);
}

void loop(){
  
}

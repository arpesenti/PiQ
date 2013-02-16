/****************************************************
 * HelloRobot.ino: Initial Robot test sketch
 * Michael Margolis 4 July 2012
 *****************************************************/
// include motor libraries
#include <AFMotor.h>
// adafruit motor shield library
#include <RobotMotor.h> // 2wd or 4wd motor library
// Setup runs at startup and is used configure pins and init system variables
void setup()
{
  Serial.begin(9600);
  motorBegin(MOTOR_LEFT);
  motorBegin(MOTOR_RIGHT);
 
}


void loop()
{
  for (int speed = 30; speed <= 100; speed += 10){
    
    motorForward(MOTOR_LEFT, speed);
    motorForward(MOTOR_RIGHT, speed);
    delay(1000);
    motorStop(MOTOR_LEFT);
    motorStop(MOTOR_RIGHT);
    delay(2000);
  }
      
}




/****************************************************
 * HelloRobot.ino: Initial Robot test sketch
 * Michael Margolis 4 July 2012
 *****************************************************/
// include motor libraries
#include <AFMotor.h>
// adafruit motor shield library
#include <RobotMotor.h> // 2wd or 4wd motor library

#include <IRremote.h> // use the library
int receiver = 5; // pin 1 of IR receiver to Arduino digital pin 11
IRrecv irrecv(receiver); // create instance of 'irrecv'
decode_results results;




float t = 0.0;

int speed = 30;
int MIN = 70;
// Setup runs at startup and is used configure pins and init system variables
void setup()
{
  
  motorBegin(MOTOR_LEFT);
  motorBegin(MOTOR_RIGHT);
  
  irrecv.enableIRIn();
 
}


void loop()
{
  float T = 5;
  
  
  delay(3000);
  for (t = 0.0; t <= 70.0; t += 0.1){
    
    motorForward(MOTOR_LEFT, MIN + speed *cos(2*3.14/T*t));
    motorForward(MOTOR_RIGHT, MIN + speed *sin(2*3.14/T*t));
    delay(30);
    if (irrecv.decode(&results)) // have we received an IR signal?
    {
    if(results.value == 284131965)
     {motorStop(MOTOR_LEFT);
      motorStop(MOTOR_RIGHT);
      delay(1000);
     }
    irrecv.resume();// receive the next value
    }
   
  }
    motorStop(MOTOR_LEFT);
    motorStop(MOTOR_RIGHT);  
}




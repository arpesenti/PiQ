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

// States
#define WAIT 0
#define EXPLORE 1
#define EXPLORE_SCAN 2
#define EXPLORE_CHANGE_POSITION 21
#define EXPLORE_REACHEGG 3
#define CATCH 4
#define COMEBACK 5
#define COMEBACK_POSITIONING 6
#define COMEBACK_APPROACHING 7
#define COMEBACK_LINEFOLLOWING 8
#define COMEBACK_LINESEARCHING 9
#define DEPOSIT 10
#define PANIC 11

// Approach results
// TODO move this constants in robot
#define APPROACH_FOUND_LINE 1
#define APPROACH_NOT_FOUND_LINE 2
#define APPROACH_FAILED 3

// Robot
Robot robot;
int state;
int result;

void setup() {
        Serial.begin(9600);
        Wire.begin();
	state = EXPLORE_REACHEGG;
	robot.init();
        robot.start();
        delay(3000);
        Serial.println("init");
}

void loop() {
  switch(state){
    case WAIT: 
      break;
      //update strategy
      
    case EXPLORE_SCAN:
      if(robot.scanForEgg())
        changeStateTo(EXPLORE_REACHEGG);
      else
	changeStateTo(EXPLORE_CHANGE_POSITION);
      break;
      
    case EXPLORE_REACHEGG:
      if(robot.reachEgg())
        changeStateTo(CATCH);
      else
        changeStateTo(EXPLORE_SCAN);
      break;
      
    case CATCH:
      if(robot.catchEgg())
        changeStateTo(COMEBACK_POSITIONING);
      else
        changeStateTo(EXPLORE_SCAN);
      break;
      
    case COMEBACK_POSITIONING:
      if(robot.positioningTowardHome()) 
        changeStateTo(COMEBACK_APPROACHING);
      else 
        // TODO: can not positioning toward home, what to do?  
      break;
      
    case COMEBACK_APPROACHING:
      result = robot.tryToApproach();
      if (result == APPROACH_FOUND_LINE)
        changeStateTo(COMEBACK_LINEFOLLOWING);
      else if (result == APPROACH_NOT_FOUND_LINE)
        changeStateTo(COMEBACK_LINESEARCHING);
      else if (result == APPROACH_FAILED)
        Serial.println("Approach failed");
        //TODO what to do when failed?
      break;
      
    case COMEBACK_LINEFOLLOWING:
      if (robot.followLineToHome())
        changeStateTo(DEPOSIT);
      else
        changeStateTo(COMEBACK_LINESEARCHING);
      break;
      
    case COMEBACK_LINESEARCHING:
      if (robot.searchLine())
        changeStateTo(COMEBACK_LINEFOLLOWING);
      else
        //TODO what to do if it do not find the line?
        Serial.println("Line not found");
      break;
      
    case DEPOSIT:
      if (robot.deposit()) {
        robot.newInit();
        changeStateTo(EXPLORE_SCAN);
      } else 
        //TODO what to do if can't deposit the egg?
        Serial.println("Deposit failed");
      break;    
      
    case PANIC:
      Serial.println("Entered panic state");
      break;
  } 
}


void changeStateTo(int nextState){
	if (state != PANIC)
		state = nextState;
}

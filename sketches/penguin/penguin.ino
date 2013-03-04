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

void setup() {
        Serial.begin(9600);
        Wire.begin();
	state = WAIT;
	robot.init();
        pinMode(13, OUTPUT);
        digitalWrite(13, LOW);
        Serial.println("init");
}

void loop() {
  delay(1000);
  switch(state){
    case WAIT: 
      Serial.println("----- WAIT -----");
      robot.escapeFromWait();
      delay(1000);
      robot.changePosition();
      digitalWrite(13, HIGH);
      break;
      
    case EXPLORE_SCAN:
      Serial.println("----- EXPLORE_SCAN -----");
      if(robot.scanForEgg())
        changeStateTo(EXPLORE_REACHEGG);
      else
	changeStateTo(EXPLORE_CHANGE_POSITION);
      break;
    case EXPLORE_CHANGE_POSITION:
      Serial.println("----- CHANGE_POSITION ------");
      if (robot.changePosition())
        changeStateTo(EXPLORE_SCAN);
      break;  
    case EXPLORE_REACHEGG:
      Serial.println("----- EXPLORE_REACHEGG -----");
      if(robot.reachEgg())
        changeStateTo(CATCH);
      else
        changeStateTo(EXPLORE_SCAN);
      break;
      
    case CATCH:
      Serial.println("----- CATCH -----");
      if(robot.catchEgg())
        changeStateTo(COMEBACK_POSITIONING);
      else
        changeStateTo(EXPLORE_SCAN);
      break;
      
    case COMEBACK_POSITIONING:
      Serial.println("----- COMEBACK_POSITIONING -----");
      if(robot.positioningTowardHome()) 
        changeStateTo(COMEBACK_APPROACHING);
      else 
        // TODO: can not positioning toward home, what to do?  
      break;
      
    case COMEBACK_APPROACHING:
      Serial.println("----- COMEBACK_APPROACHING-----");
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
      Serial.println("----- COMEBACK_LINEFOLLOWING -----");
      if (robot.followLineToHome())
        changeStateTo(DEPOSIT);
      else
        changeStateTo(COMEBACK_LINESEARCHING);
      break;
      
    case COMEBACK_LINESEARCHING:
      Serial.println("----- COMEBACK_LINESEARCHING -----");
      if (robot.searchLine())
        changeStateTo(COMEBACK_LINEFOLLOWING);
      else
        //TODO what to do if it do not find the line?
        Serial.println("Line not found");
      break;
      
    case DEPOSIT:
      Serial.println("----- DEPOSIT -----");
      if (robot.deposit()) {
        robot.newInit();
        changeStateTo(EXPLORE_SCAN);
      } else 
        //TODO what to do if can't deposit the egg?
        Serial.println("Deposit failed");
      break;    
      
    case PANIC:
      Serial.println("----- PANIC -----");
      Serial.println("Entered panic state");
      robot.escapeFromPanic();
      break;
  } 
}


void changeStateTo(int nextState){
	if (state != PANIC)
		state = nextState;
}

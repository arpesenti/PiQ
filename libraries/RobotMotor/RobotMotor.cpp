/*******************************************************
    RobotMotor.cpp // Adafruit version for 2WD and 4WD chassis
    low level motor driver for use with adafruit motor shield 

    Motor constants used are defined AFMotor.h

    Copyright Michael Margolis May 8 2012
********************************************************/

#include <Arduino.h>  
#include <AFMotor.h>  // adafruit motor shield library
#include "RobotMotor.h"


const int differential = 0; // % faster left motor turns compared to right  

// tables hold time in ms to rotate robot 360 degrees at various speeds 
// this enables conversion of rotation angle into timed motor movement 
// The speeds are percent of max speed
// Note: low cost motors do not have enough torque at low speeds so
// the robot will not move below this value 
// Interpolation is used to get a time for any speed from MIN_SPEED to 100%

// constants for 2 wheeled robot chassis
#if defined CHASSIS_2WD
const int MIN_SPEED = 40; // first table entry is 40% speed
const int SPEED_TABLE_INTERVAL = 10; // each table entry is 10% faster speed
const int NBR_SPEEDS =  1 + (100 - MIN_SPEED)/ SPEED_TABLE_INTERVAL;
 
int speedTable[NBR_SPEEDS]  =  {40,     50,   60,   70,   80,   90,  100}; // speeds  
int rotationTime[NBR_SPEEDS] = {5500, 3300, 2400, 2000, 1750, 1550, 1150}; // time 

AF_DCMotor motors[] = {
    // AF_DCMotor(1, MOTOR12_1KHZ), // left is Motor #1 
    // AF_DCMotor(2, MOTOR12_1KHZ)  // right is Motor #2
    // AF_DCMotor(1, MOTOR34_1KHZ), // left is Motor #1 
    // AF_DCMotor(2, MOTOR34_1KHZ)  // right is Motor #2
    AF_DCMotor(1), // left is Motor #1 
    AF_DCMotor(2)  // right is Motor #2
	};
// constants for 4 wheeled robot	
#elif defined CHASSIS_4WD
const int MIN_SPEED = 60; // first table entry is 60% speed
const int SPEED_TABLE_INTERVAL = 10; // each table entry is 10% faster speed
const int NBR_SPEEDS =  1 + (100 - MIN_SPEED)/ SPEED_TABLE_INTERVAL;
 
int speedTable[NBR_SPEEDS]  =  {60,   70,   80,   90,  100}; // speeds  
int rotationTime[NBR_SPEEDS] = {5500, 3300, 2400, 2000, 1750}; // time

AF_DCMotor motors[] = {
    AF_DCMotor(4, MOTOR34_1KHZ), // left front is Motor #4 
    AF_DCMotor(3, MOTOR34_1KHZ), // right front is Motor #3
    AF_DCMotor(1, MOTOR12_1KHZ), // left rear is Motor #1
    AF_DCMotor(2, MOTOR12_1KHZ)  // right rear is Motor #2
};

#else
#error "expected definition: CHASSIS_2WD or CHASSIS_4WD not found" 
#endif
	
int  motorSpeed[2]  = {0,0}; // left and right motor speeds stored here (0-100%)

void motorBegin(int motor)
{
  motorStop(motor);  // stop the front motor
#if defined CHASSIS_4WD   
  motorStop(motor+2); // stop the rear motor
#endif   
}

// speed range is 0 to 100 percent
void motorSetSpeed(int motor, int speed)
{
  if( motor == MOTOR_LEFT && speed > differential)
    speed -= differential;
  motorSpeed[motor] = speed;           // save the value
  int pwm = map(speed, 0,100, 0,255);  // scale to PWM range
   
  motors[motor].setSpeed(pwm) ;
#if defined CHASSIS_4WD   
  motors[motor+2].setSpeed(pwm) ;
#endif  
}

void motorReverse(int motor, int speed)
{
  motorSetSpeed(motor, speed);
  motors[motor].run(FORWARD); 
#if defined CHASSIS_4WD  
  motors[motor+2].run(FORWARD);   
#endif  
}

void motorForward(int motor, int speed)
{
  motorSetSpeed(motor, speed);
  motors[motor].run(BACKWARD); 
#if defined CHASSIS_4WD    
  motors[motor+2].run(BACKWARD);   
#endif  
}

void motorStop(int motor)
{
  // todo set speed to 0 ???
  motors[motor].run(RELEASE);      // stopped
#if defined CHASSIS_4WD    
  motors[motor+2].run(RELEASE);   
#endif  
}

void motorBrake(int motor)
{
  motors[motor].run(BRAKE);      // stopped
#if defined CHASSIS_4WD   
  motors[motor+2].run(BRAKE); 
#endif
}

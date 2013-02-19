#include <Position.h>
#include <PS2.h>
#include <HMC5883L.h>
#include <Wire.h>
#include <AFMotor.h>
#include <RobotMotor.h>
#include <Pins.h>

Position position;
int speed;
int stepTime = 2000;
int rotationalTime = 500;

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  Serial.println("Init setup");
  bool succesful = position.init();
  if (!succesful) {
    Serial.println("Error in initializing position");
  }
  motorBegin(MOTOR_LEFT);
  motorBegin(MOTOR_RIGHT);
  output();
  randomSeed(analogRead(0));
  Serial.println("End of setup");
}

void loop()
{
 for (int count = 0; count < 1; count += 1)
 {
     speed = random(50,100);
     Serial.println("Start motor with speed");
     Serial.println(speed);
     motorForward(MOTOR_LEFT, speed);
     motorForward(MOTOR_RIGHT, speed);
     double distance = 0.0;
     int x = position.getX();
     int y = position.getY();
     while (distance < 3000) {
      distance = sqrt(square((x-position.getX())) +  square((y-position.getY())));
      position.update();
      output();
     }
     motorStop(MOTOR_LEFT);
     motorStop(MOTOR_RIGHT);
     position.update();
     delay(50);
     position.update();
     delay(1000);
     
 }
 Serial.println("******************************************");
 output();
 Serial.println("******************************************");
 
 // return home
 int x = position.getX();
 int y = position.getY();
 Serial.println(atan2(y, x));
 double angleToFollow = fmod(atan2(y, x) - PI, 2*PI); 
 Serial.println("Angle to follow");
 Serial.println(angleToFollow);
 rotateToAngle(angleToFollow);
 double distance = sqrt(square(position.getX())+square(position.getY()));
 while (abs(distance) > 100) {
     motorForward(MOTOR_LEFT, 60);
     motorForward(MOTOR_RIGHT, 60);
     position.update();
     distance = sqrt(square(position.getX())+square(position.getY()));
 }
 motorStop(MOTOR_LEFT);
 motorStop(MOTOR_RIGHT);
}

void rotateToAngle(double angle) {
 double currentAngle = position.getOrientation();
 while (abs(currentAngle- angle) > PI/15) {
   motorForward(MOTOR_LEFT, 70);
   motorReverse(MOTOR_RIGHT, 70);
   position.update();
   delay(10);
   motorForward(MOTOR_LEFT, 55);
   motorReverse(MOTOR_RIGHT, 55);
   currentAngle = position.getOrientation();
   Serial.println("Current angle");
   Serial.println(currentAngle);
   delay(10);
 }  
 motorStop(MOTOR_LEFT);
 motorStop(MOTOR_RIGHT);
}

void output()
{
  Serial.println(" ");
  Serial.println(position.getX());
  Serial.println(position.getY());
  Serial.println(position.getOrientation());
  
}


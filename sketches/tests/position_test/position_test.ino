#include <Position.h>
#include <PS2.h>
#include <HMC5883L.h>
#include <Wire.h>
#include <AFMotor.h>
#include <RobotMotor.h>

Position position;
int speed;
int stepTime = 2000;
int rotationalTime = 500;

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  Serial.println("Init setup");
  position.init();
  motorBegin(MOTOR_LEFT);
  motorBegin(MOTOR_RIGHT);
  output();
  randomSeed(analogRead(0));
  Serial.println("End of setup");
}

void loop()
{
 for (int count = 0; count < 5; count += 1)
 {
     speed = random(50,100);
     Serial.println("Start motor with speed");
     Serial.println(speed);
     motorForward(MOTOR_LEFT, speed);
     motorForward(MOTOR_RIGHT, speed);
     for(int time = 0; time < stepTime; time += 20)
     { 
       delay(20);
       position.update();
       output();
     }
     motorStop(MOTOR_LEFT);
     motorStop(MOTOR_RIGHT);
     position.update();
     delay(50);
     position.update();
     delay(1000);
     
     // rotate
     motorForward(MOTOR_LEFT, speed);
     motorReverse(MOTOR_RIGHT, speed);
     for(int time = 0; time < rotationalTime; time += 10)
     {
       delay(10);
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
 output();
 
 // return home
 int x = position.getX();
 int y = position.getY();
 double angleToFollow = fmod(atan2(y, x) - PI, 2*PI); 
 rotateToAngle(angleToFollow);
 double distance = sqrt(square(position.getX())+square(position.getY()));
 while (abs(distance) > 3) {
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
 while (abs(currentAngle- angle) > PI/30) {
   motorForward(MOTOR_LEFT, 60);
   motorReverse(MOTOR_RIGHT, 60);
   position.update();
   currentAngle = position.getOrientation();
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


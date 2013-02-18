#include <Wire.h>

#include <Position.h>

#include <PS2.h>

#include <HMC5883L.h>

#include <AFMotor.h>
#include <RobotMotor.h>


#include <LineSensor.h>
#include <ADJDS311.h>
#include <Pins.h>


Position position;
LineSensor lineSensor;
int speed;
int stepTime = 2000;
int rotationalTime = 500;

void setup()
{
  Serial.begin(9600);
  Serial.println("start setup");
  lineSensor.init();
  Serial.println(position.init());
  motorBegin(MOTOR_LEFT);
  motorBegin(MOTOR_RIGHT);
  output();
  randomSeed(analogRead(2));
  
}

void loop()
{
 for (int count = 0; count < 5; count += 1)
 {
     Serial.print("color seen: ");
     
     Serial.println(lineSensor.color());
     delay(2000);
     speed = random(50,100);
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
  
}


void output()
{
  Serial.println(" ");
  Serial.println(position.getX());
  Serial.println(position.getY());
  Serial.println(position.getOrientation());
  
}

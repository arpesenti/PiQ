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


Position position;
Motion motion;

Robot robot;

void setup(){
  Serial.begin(9600);
  Wire.begin();  
	robot.init();
  motion.init();
  position.calibrate(motion, false);
  position.init();
}

void loop(){
  
  for ( int i = 1; i <= 15; i++){
    double angle = position.getOrientation();
    double deltaAngle = PI/i;
    Serial.print("iteration: ");
    Serial.println(i);
    Serial.print("start angle: ");
    Serial.print(angle * RAD_TO_DEG);
    Serial.print("   ||    ");
    Serial.print("delta angle: ");
    Serial.print(deltaAngle * RAD_TO_DEG);
    double toAngle = angle + deltaAngle;
		Serial.print("   ||    ");
    Serial.print("arriving angle: ");
    Serial.println(toAngle * RAD_TO_DEG);
    // in this case, angles are always positive
    toAngle = fmod(2*PI + toAngle, 2*PI);
  	robot.rotateRight(deltaAngle);
		delay(1000);
		angle = position.getOrientation();
		Serial.print("actual arriving angle: ");
    Serial.println(angle * RAD_TO_DEG);
		delay(2000);
		Serial.println("rotate left of the same delta : ");		
		robot.rotateLeft(deltaAngle);
		delay(1000);
		angle = position.getOrientation();
		Serial.print("actual arriving angle: ");
    Serial.println(angle * RAD_TO_DEG);
		delay(2000);
		
    //rotateToAngle(toAngle); 
  
  }
  
}

void rotateToAngle(double toAngle){
  
  double tol = 0.1; // 5.7 degrees
  motion.rotateRight(5); // 20% speed
  while(abs(position.getOrientation() - toAngle) > tol){
   position.update(); 
   Serial.println(position.getOrientation() * RAD_TO_DEG);
  }
  motion.stop();
  delay(2000);
  Serial.print("Arrived to angle : ");
  Serial.println(position.getOrientation() * RAD_TO_DEG);
  
}

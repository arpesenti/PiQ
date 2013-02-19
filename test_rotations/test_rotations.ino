#include <Position.h>
#include <Motion.h>
#include <Wire.h>
#include <Pins.h>
#include <PS2.h>
#include <HMC5883L.h>
#include <AFMotor.h>
#include <RobotMotor.h>
#include <EEPROM.h>

Position position;
Motion motion;

void setup(){
  position.init();
  motion.init();
  position.calibrate(motion);
  Serial.begin(9600);
  Wire.begin();  
}

void loop(){
  
  for ( int i = 1; i <= 6; i++){
    double angle = position.getOrientation();
    double deltaAngle = PI/i;
    Serial.print("iteration: ");
    Serial.println(i);
    Serial.print("start angle: ");
    Serial.print(angle * RAD_TO_DEG);
    Serial.print("   ||    ");
    Serial.print("delta angle: ");
    Serial.println(deltaAngle * RAD_TO_DEG);
    double toAngle = angle + deltaAngle;
    // in this case, angles are always positive
    toAngle = fmod(toAngle, 2*PI);
    if (toAngle < 0)
     toAngle += 2*PI;
    rotateToAngle(toAngle); 
  
  }
  
}

void rotateToAngle(double toAngle){
  
  double tol = 0.1; // 5.7 degrees
  motion.rotateRight(20); // 20% speed
  while(abs(position.getOrientation() - toAngle) > tol){
   position.update(); 
   Serial.println(position.getOrientation() * RAD_TO_DEG);
  }
  motion.stop();
  delay(2000);
  Serial.print("Arrived to angle : ");
  Serial.println(position.getOrientation() * RAD_TO_DEG);
  
}

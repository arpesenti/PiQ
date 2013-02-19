#include <Wire.h>
#include <Position.h>
#include <PS2.h>
#include <HMC5883L.h>
#include <Pins.h>

Position position;

void setup(){
  Serial.begin(9600);
  Wire.begin();
  position.init();
}

// Our main program loop.
void loop(){
  position.update();
  Serial.println(position.getOrientation()*180/PI);
  delay(100);
}

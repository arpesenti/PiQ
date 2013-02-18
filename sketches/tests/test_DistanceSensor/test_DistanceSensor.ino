#include <DistanceSensor.h>
#include <Pins.h>


DistanceSensor prox;
DistanceSensor highTop;
DistanceSensor highBottom;

void setup(){
  Serial.begin(9600);
  prox.initProximity();
  highTop.initHighDistanceTop();
  highBottom.initHighDistanceBottom();
  Serial.println("end initialization");
}

void loop(){
  Serial.println(prox.distance());
  //Serial.println(highTop.distance());
  //Serial.println(highBottom.distance());
  delay(200);
}

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
  highTop.switchOff();
  highBottom.switchOff();
}

void loop(){
  Serial.println("");
  Serial.print("Proximity  ");
  Serial.println(prox.distance());
  Serial.print("Top        ");
  Serial.println(highTop.distance());
  Serial.print("Bottom     ");
  Serial.println(highBottom.distance());
  delay(500);
}

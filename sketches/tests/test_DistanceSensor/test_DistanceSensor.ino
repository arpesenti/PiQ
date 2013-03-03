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
  Serial.println("");
  Serial.print("Proximity  ");
  Serial.print(prox.distance());
  Serial.print("      Top  ");
  Serial.print(highTop.distance());
  Serial.print("    Bottom ");
  Serial.println(highBottom.distance());
  delay(500);
}

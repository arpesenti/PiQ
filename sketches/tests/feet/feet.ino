#include <Feet.h>
#include <Servo.h>
#include <Pins.h>

Feet feet;

void setup() {
  feet.init();
}

void loop() {
  feet.open();
  delay(1000);
  feet.close();
  delay(10000);
  //feet.open(80);
  //delay(1000);
  //feet.close(80);
  //delay(1000);
  //feet.trick();
  //delay(5000);
}

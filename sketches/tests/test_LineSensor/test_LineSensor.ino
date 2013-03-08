#include <Wire.h>
#include <ADJDS311.h>
#include <Pins.h>
#include <LineSensor.h>
#include <EEPROM.h>

LineSensor lineSensor;

void setup(){
  Serial.begin(9600);
  lineSensor.init(false);
}

void loop(){
  Serial.println(lineSensor.leftReflectance());
  Serial.println(lineSensor.rightReflectance());
  Serial.println(lineSensor.centerReflectance());
  Serial.println(lineSensor.color());
  Serial.println("");
  delay(500);
  
}

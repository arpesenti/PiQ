#include <ADJDS311.h>
#include <Wire.h>

// Connect to SDA to A4, SCL to A5 

#define SENSORLED_PIN  2 //LED on the ADJDS-311 connected to digital pin

ADJDS311 colorSensor(SENSORLED_PIN);

void setup(){
  Serial.begin(9600);
  
  colorSensor.init();
  colorSensor.ledOn(); //turn LED on
  
  //Calibrate white 
  //Need to hold white card in front (1-3mm) of it to calibrate from
  colorSensor.calibrate(); 
}

void loop(){
  RGBC color = colorSensor.read(); //read the color
  
  Serial.print(color.red);
  Serial.print(" | ");
  Serial.print(color.green);
  Serial.print(" | ");
  Serial.print(color.blue);
  Serial.print(" | ");
  Serial.println(color.clear);
  
  delay(200); //just here to slow down the serial output
 
}

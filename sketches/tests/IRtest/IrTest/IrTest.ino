

#include <IRremote.h> // use the library
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
#include <SoftwareServo.h>
#include <IRremote.h>
#include <ADJDS311.h>
#include <Wire.h>
#include <AFMotor.h>
#include <RobotMotor.h>
#include <EEPROM.h>
#include <Eyes.h>

// Robot
Robot robot;
int state;
int result;
int cruiseSpeed;
int rotationalCruiseSpeed;

int receiver = 44; // pin 1 of IR receiver to Arduino digital pin 11
IRrecv irrecv(receiver); // create instance of 'irrecv'
decode_results results;

void setup()
{
  Serial.begin(9600); // for serial monitor output
  irrecv.enableIRIn(); // Start the receiver
  Wire.begin();
        randomSeed(analogRead(0));
	state = WAIT;
	robot.init();
        Serial.println("init");
}
void loop()
{
  if (irrecv.decode(&results)) // have we received an IR signal?
  {
    Serial.println(results.value, HEX); // display it on serial monitor in hexadecimal
    Serial.println(results.value  & 255 , HEX);
    if(results.value & 255 == 0x088)
      Serial.println("uno");
    irrecv.resume();// receive the next value
  }

}

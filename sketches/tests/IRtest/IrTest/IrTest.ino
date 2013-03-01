#include <IRremote.h> // use the library
int receiver = 44; // pin 1 of IR receiver to Arduino digital pin 11
IRrecv irrecv(receiver); // create instance of 'irrecv'
decode_results results;

void setup()
{
  Serial.begin(9600); // for serial monitor output
  irrecv.enableIRIn(); // Start the receiver
}
void loop()
{
  if (irrecv.decode(&results)) // have we received an IR signal?
  {
    Serial.println(results.value, HEX); // display it on serial monitor in hexadecimal
    if(results.value == 284131965)
      Serial.println("uno");
    irrecv.resume();// receive the next value
  }

}

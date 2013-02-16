#include <ps2.h>
#include <IRremote.h> // use the library
int receiver = 5; // pin 1 of IR receiver to Arduino digital pin 11
IRrecv irrecv(receiver); // create instance of 'irrecv'
decode_results results;
/*
 * an arduino sketch to interface with a ps/2 mouse.
 * Also uses serial protocol to talk back to the host
 * and report what it finds.
 */

/*
 * Pin 5 is the mouse data pin, pin 6 is the clock pin
 * Feel free to use whatever pins are convenient.
 * First pin is the clock (leftmost), second data (center)
 */
PS2 mouse(2, 6);

/*
 * initialize the mouse. Reset it, and place it into remote
 * mode, so we can get the encoder data on demand.
 */
void mouse_init()
{
  mouse.write(0xff);  // reset
  mouse.read();  // ack byte
  mouse.read();  // blank */
  mouse.read();  // blank */
  mouse.write(0xf0);  // remote mode
  mouse.read();  // ack
  delayMicroseconds(100);
}

void setup()
{
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  mouse_init();
}

/*
 * get a reading from the mouse and report it back to the
 * host via the serial line.
 */
void loop()
{
  char mstat;
  char mx;
  char my;

  /* get a reading from the mouse */
  mouse.write(0xeb);  // give me data!
  mouse.read();      // ignore ack
  mstat = mouse.read();
  mx = mouse.read();
  my = mouse.read();
  if (mx>0 || my>0) {
    digitalWrite(13,HIGH);
    delay(1000);
    digitalWrite(13,LOW);
  }
  if (irrecv.decode(&results)) // have we received an IR signal?
  {
    Serial.println(results.value); // display it on serial monitor in hexadecimal
    if(results.value == 284131965) {
      digitalWrite(13,HIGH);
      delay(1000);
      digitalWrite(13,LOW);
    }
    irrecv.resume();// receive the next value
  }
  /* send the data back up */
  Serial.print(mstat, BIN);
  Serial.print("\tX=");
  Serial.print(mx, DEC);
  Serial.print("\tY=");
  Serial.print(my, DEC);
  Serial.println();
  //delay(20);  /* twiddle */
}

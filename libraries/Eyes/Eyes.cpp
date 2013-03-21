#include "Eyes.h"

Eyes::Eyes() {

}

void Eyes::init(){
	pinMode(LEFT_EYE_PIN, OUTPUT);
	pinMode(RIGHT_EYE_PIN, OUTPUT);
	//on();
}
void Eyes::on() {
	eyesOn = true;
	digitalWrite(LEFT_EYE_PIN, HIGH);
	digitalWrite(RIGHT_EYE_PIN, HIGH);
}
void Eyes::off() {
	eyesOn = false;
	digitalWrite(LEFT_EYE_PIN, LOW);
	digitalWrite(RIGHT_EYE_PIN, LOW);
}
void Eyes::blink(int intervallMilliSeconds) {
	for (int i = 0; i<intervallMilliSeconds/50; i++) {
		on();
		delay(50);
		off();
	}
	if (eyesOn) on();
}

void Eyes::commandBlink(int currentState){
	digitalWrite(PANIC_LED, HIGH);
  delay(50);
	digitalWrite(PANIC_LED, LOW);
	delay(100);
	digitalWrite(PANIC_LED, HIGH);
	delay(100);
	digitalWrite(PANIC_LED, LOW);
	delay(100);
	if(currentState == PANIC_STATE)
		digitalWrite(PANIC_LED, HIGH);
}

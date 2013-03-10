#include "Eyes.h"

Eyes::Eyes() {

}

void Eyes::init(){
	// TODO initialize led pin
}
void Eyes::on() {

}
void Eyes::off() {

}
void Eyes::blink(int intervallMilliSeconds) {
	
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

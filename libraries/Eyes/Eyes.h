#ifndef Eyes_h
#define Eyes_h


#include <Arduino.h>
#include <Pins.h>


#define PANIC_STATE 11 // PANIC STATE - MUST BE CONSISTENT WITH PANIC IN robot.h

class Eyes {
public:
	Eyes();
	void init();
	void on();
	void off();
	void blink(int intervallMilliSeconds);
	void commandBlink(int currentState); // blink when receiving command from remote
	
private:
	int ledPin;
	bool eyesOn;
};

#endif

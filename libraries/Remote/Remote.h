#ifndef Remote_h
#define Remote_h

#include <Arduino.h>
#include <IRremote.h>
#include <Pins.h>

#define WAIT_STRATEGY_BUTTON 0 //TODO remote code
#define WAIT_STRATEGY 0
#define NORMAL_STRATEGY_BUTTON 1 //TODO remote code
#define NORMAL_STRATEGY 1
#define PANIC_STRATEGY_BUTTON 2 //TODO remote code
#define PANIC_STRATEGY 2

class Remote {
public:
	Remote();
	void init();
	char strategy();
	void update(); // to be called every loop
private:
	IRrecv receiver;
	decode_results results;
	char currentStrategy;
};

#endif
#ifndef Remote_h
#define Remote_h

#include <Arduino.h>
#include <IRremote.h>
#include <Pins.h>

#define WAIT_STRATEGY_BUTTON 284151855 // button off
#define WAIT_STRATEGY '0'
#define NORMAL_STRATEGY_BUTTON 284131965 // button 1
#define NORMAL_STRATEGY '1'
#define PANIC_STRATEGY_BUTTON 284136045 // button 2
#define PANIC_STRATEGY '2'



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

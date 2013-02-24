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
#define REMOTE_ROTATELEFT_BUTTON 3 //TODO
#define REMOTE_ROTATELEFT '3'
#define REMOTE_ROTATERIGHT_BUTTON 4 //TODO
#define REMOTE_ROTATERIGHT '4'
#define REMOTE_MOVEFORWARD_BUTTON 5 //TODO
#define REMOTE_MOVEFORWARD '5'
#define REMOTE_MOVEBACKWARD_BUTTON 6 //TODO
#define REMOTE_MOVEBACKWARD '6'
#define REMOTE_STOP_BUTTON 7 //TODO
#define REMOTE_STOP '7'
#define REMOTE_OPENFEET_BUTTON 8 //TODO
#define REMOTE_OPENFEET '8'
#define REMOTE_CLOSEFEET_BUTTON 9 //TODO
#define REMOTE_CLOSEFEET '9'
#define REMOTE_INCREASE_SPEED_BUTTON 10 // TODO
#define REMOTE_INCREASE_SPEED 'a'
#define REMOTE_DECREASE_SPEED_BUTTON 11 // TODO
#define REMOTE_DECREASE_SPEED 'b'

#define REMOTE_NO_STRATEGY 'n'

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

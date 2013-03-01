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
#define REMOTE_ROTATELEFT_BUTTON 10EF40BF // left (vol -)
#define REMOTE_ROTATELEFT '3'
#define REMOTE_ROTATERIGHT_BUTTON 10EF7887 // right (vol +)
#define REMOTE_ROTATERIGHT '4'
#define REMOTE_MOVEFORWARD_BUTTON 10EFD827 // up (ch +)
#define REMOTE_MOVEFORWARD '5'
#define REMOTE_MOVEBACKWARD_BUTTON 10EF9867 // down (ch -)
#define REMOTE_MOVEBACKWARD '6'
#define REMOTE_STOP_BUTTON 10EF58A7 // ok
#define REMOTE_STOP '7'
#define REMOTE_OPENFEET_BUTTON 10EF807F // Pg Up
#define REMOTE_OPENFEET '8'
#define REMOTE_CLOSEFEET_BUTTON 10EFB847 // Pg Dn
#define REMOTE_CLOSEFEET '9'
#define REMOTE_INCREASE_SPEED_BUTTON 10EF30CF // Green button
#define REMOTE_INCREASE_SPEED 'a'
#define REMOTE_DECREASE_SPEED_BUTTON 10EF10EF // Red button
#define REMOTE_DECREASE_SPEED 'b'
#define SEARCH_LINE_STRATEGY_BUTTON 10EF28D7 // blu button
#define SEARCH_LINE_STRATEGY 'C'


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

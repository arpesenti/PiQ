#ifndef Remote_h
#define Remote_h

#include <Arduino.h>
#include <IRremote.h>
#include <Pins.h>

/*#define NORMAL_STRATEGY_BUTTON 0x10EF827D // button 1 - put robot in exploreScan
#define NORMAL_STRATEGY '1'
#define PANIC_STRATEGY_BUTTON 0x10EFD02F // power button
#define PANIC_STRATEGY '2'
#define REMOTE_ROTATELEFT_BUTTON 0x10EF40BF // left (vol -)
#define REMOTE_ROTATELEFT '3'
#define REMOTE_ROTATERIGHT_BUTTON 0x10EF7887 // right (vol +)
#define REMOTE_ROTATERIGHT '4'
#define REMOTE_MOVEFORWARD_BUTTON 0x10EFD827 // up (ch +)
#define REMOTE_MOVEFORWARD '5'
#define REMOTE_MOVEBACKWARD_BUTTON 0x10EF9867 // down (ch -)
#define REMOTE_MOVEBACKWARD '6'
#define REMOTE_STOP_BUTTON 0x10EF58A7 // ok
#define REMOTE_STOP '7'
#define REMOTE_OPENFEET_BUTTON 0x10EF807F // Pg Up
#define REMOTE_OPENFEET '8'
#define REMOTE_CLOSEFEET_BUTTON 0x10EFB847 // Pg Dn
#define REMOTE_CLOSEFEET '9'
#define REMOTE_INCREASE_SPEED_BUTTON 0x10EF30CF // Green button
#define REMOTE_INCREASE_SPEED 'a'
#define REMOTE_DECREASE_SPEED_BUTTON 0x10EF10EF // Red button
#define REMOTE_DECREASE_SPEED 'b'
#define SEARCH_LINE_STRATEGY_BUTTON 0x10EF28D7 // blu button - put robot in searchLine
#define SEARCH_LINE_STRATEGY 'C'
#define COMEBACK_STRATEGY_BUTTON 0x10EF08F7 // yellow button - put robot in positioningTowardHome
#define COMEBACK_STRATEGY 'd'
*/

#define NORMAL_STRATEGY_BUTTON 0x88 // button 1 - put robot in exploreScan
#define NORMAL_STRATEGY '1'
#define PANIC_STRATEGY_BUTTON 0x8C // power button
#define PANIC_STRATEGY '2'
#define REMOTE_ROTATELEFT_BUTTON 0x96 // left (vol -)
#define REMOTE_ROTATELEFT '3'
#define REMOTE_ROTATERIGHT_BUTTON 0xAF // right (vol +)
#define REMOTE_ROTATERIGHT '4'
#define REMOTE_MOVEFORWARD_BUTTON 0x92 // up (ch +)
#define REMOTE_MOVEFORWARD '5'
#define REMOTE_MOVEBACKWARD_BUTTON 0xAE // down (ch -)
#define REMOTE_MOVEBACKWARD '6'
#define REMOTE_STOP_BUTTON 0x91 // ok
#define REMOTE_STOP '7'
#define REMOTE_OPENFEET_BUTTON 0x90 // TV RADIO
#define REMOTE_OPENFEET '8'
#define REMOTE_CLOSEFEET_BUTTON 0x8E // Menu exit
#define REMOTE_CLOSEFEET '9'
#define REMOTE_INCREASE_SPEED_BUTTON 0xAA // Green button
#define REMOTE_INCREASE_SPEED 'a'
#define REMOTE_DECREASE_SPEED_BUTTON 0x8D // Red button
#define REMOTE_DECREASE_SPEED 'b'
#define SEARCH_LINE_STRATEGY_BUTTON 0xAB // blu button - put robot in searchLine
#define SEARCH_LINE_STRATEGY 'C'
#define COMEBACK_STRATEGY_BUTTON 0x95 // yellow button - put robot in positioningTowardHome
#define COMEBACK_STRATEGY 'd'


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

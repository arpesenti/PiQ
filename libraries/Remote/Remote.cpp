#include "Remote.h"

Remote::Remote(): receiver(IR_PIN) {
	currentStrategy = REMOTE_NO_STRATEGY;
}

void Remote::init() {
	receiver.enableIRIn();
}

void Remote::update() {
	if (receiver.decode(&results)) { // have we received an IR signal?
		if (results.value == NORMAL_STRATEGY_BUTTON)
		{
			currentStrategy = NORMAL_STRATEGY;
		} else if (results.value == PANIC_STRATEGY_BUTTON) {
			currentStrategy = PANIC_STRATEGY;
		} else if (results.value == REMOTE_ROTATELEFT_BUTTON) {
			currentStrategy = REMOTE_ROTATELEFT;
		} else if (results.value == REMOTE_ROTATERIGHT_BUTTON) {
			currentStrategy = REMOTE_ROTATERIGHT;
		} else if (results.value == REMOTE_MOVEFORWARD_BUTTON) {
			currentStrategy = REMOTE_MOVEFORWARD;
		} else if (results.value == REMOTE_MOVEBACKWARD_BUTTON) {
			currentStrategy = REMOTE_MOVEBACKWARD;
		} else if (results.value == REMOTE_STOP_BUTTON) {
			currentStrategy = REMOTE_STOP;
		} else if (results.value == REMOTE_OPENFEET_BUTTON) {
			currentStrategy = REMOTE_OPENFEET;
		} else if (results.value == REMOTE_CLOSEFEET_BUTTON) {
			currentStrategy = REMOTE_CLOSEFEET;
		} else if (results.value == REMOTE_INCREASE_SPEED_BUTTON) {
			currentStrategy = REMOTE_INCREASE_SPEED;
		} else if (results.value == REMOTE_DECREASE_SPEED_BUTTON) {
			currentStrategy = REMOTE_DECREASE_SPEED;
		} else if (results.value == SEARCH_LINE_STRATEGY_BUTTON) {
			currentStrategy = SEARCH_LINE_STRATEGY;
		} 

    	receiver.resume();
	} 
}

char Remote::strategy() {
	char strategy = currentStrategy;
	currentStrategy = REMOTE_NO_STRATEGY; // when read clear the current strategy
	return strategy;
}

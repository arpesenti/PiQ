#include "Remote.h"

Remote::Remote(): receiver(IR_PIN) {
	currentStrategy = WAIT_STRATEGY;
}

void Remote::init() {
	receiver.enableIRIn();
}

void Remote::update() {
	if (receiver.decode(&results)) { // have we received an IR signal?
		unsigned long value = results.value;
		if (value == NORMAL_STRATEGY_BUTTON)
		{
			currentStrategy = NORMAL_STRATEGY;
		} else if (value == WAIT_STRATEGY_BUTTON) {
			currentStrategy = WAIT_STRATEGY;
		} else if (value == PANIC_STRATEGY_BUTTON) {
			currentStrategy = PANIC_STRATEGY;
		} else if (value == REMOTE_ROTATELEFT_BUTTON) {
			currentStrategy = REMOTE_ROTATELEFT;
		} else if (value == REMOTE_ROTATERIGHT_BUTTON) {
			currentStrategy = REMOTE_ROTATERIGHT;
		} else if (value == REMOTE_MOVEFORWARD_BUTTON) {
			currentStrategy = REMOTE_MOVEFORWARD;
		} else if (value == REMOTE_MOVEBACKWARD_BUTTON) {
			currentStrategy = REMOTE_MOVEBACKWARD;
		} else if (value == REMOTE_STOP_BUTTON) {
			currentStrategy = REMOTE_STOP;
		} else if (value == REMOTE_OPENFEET_BUTTON) {
			currentStrategy = REMOTE_OPENFEET_BUTTON;
		} else if (value == REMOTE_CLOSEFEET_BUTTON) {
			currentStrategy = REMOTE_CLOSEFEET;
		}

    	receiver.resume();
	} 
}

char Remote::strategy() {
	return currentStrategy;
}

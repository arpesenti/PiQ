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
		}

    receiver.resume();
	} 
}

char Remote::strategy() {
	return currentStrategy;
}

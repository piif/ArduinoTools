#define NOT_IN_MAIN
#include "events.h"

void Events::begin() {
	// malloc eventHandler[]
	// malloc eventQueue[]
}

void Events::fire(Events::eventType type) {
	// add event type to list (or to a bit field ?)
}

void Events::waitNext() {
	// compute next time
	// wait in a interruptible manner
	// -> http://www.instructables.com/id/Arduino-Timer-Interrupts/
	// write a delayIdle() method with prefered timer as argument ?
	// find fired events -> call callbacks
}

Events::eventHandler *Events::registerEvent(Events::eventType type, Events::eventCallback callback, void *data) {
	return 0;
}
Events::eventHandler *Events::registerTimeout(long ms, Events::eventCallback callback, void *data) {
	return 0;
}
Events::eventHandler *Events::registerInterval(long ms, Events::eventCallback callback, void *data) {
	return 0;
}
bool Events::unregisterEvent(Events::eventHandler *handler) {
	return false;
}

#define NOT_IN_MAIN
#include "events.h"
#include "delayIdle.h"

#define MAX_QUEUE_SIZE 5
#define HANDLER_ALLOC_BLOCK_SIZE 5

struct _eventHandler {
	Events::eventType type;
	int id; // = rank in handler map
	Events::eventCallback callback;
	void *data;
	union {
		struct {
			long delay; // number of ms for timeout / interval events
			unsigned long next;  // date of next tick for timeout / interval events
			int count;  // date of next tick for timeout / interval events
		} timerSpec;
		struct {
			byte pin;
			byte interrupt;
			byte mode;
		} inputSpec;
	};
};

void Events::begin() {
	eventHandlerMax = 0;
	handlers = 0;
	queueSize = 0;
	eventQueueTypes = (byte *)malloc(MAX_QUEUE_SIZE);
	eventQueueDetails = (short *)malloc(MAX_QUEUE_SIZE);
}

void Events::fire(Events::eventType type) {
	fire(type, -1);
}
void Events::fire(Events::eventType type, short detail) {
	if (queueSize >= MAX_QUEUE_SIZE) {
		// full => ignore
		return;
	}
	eventQueueTypes[queueSize] = type;
	eventQueueDetails[queueSize] = detail;
	queueSize++;
}

void Events::waitNext() {
	// foreach fired events, call callbacks
	for(int q = 0; q < queueSize; q++) {
		for(int h = 0; h < eventHandlerMax; h++) {
			if (eventQueueTypes[q] == handlers[h].type) {
				// this handler may be for this event
				eventHandler *hdl = &(handlers[h]);

				if (hdl->type == event_timer) {
					// is it expired ? => handle
					if (hdl->timerSpec.next <= micros()) {
						hdl->callback(-1, hdl->data);
						if (hdl->timerSpec.count == 1) {
							unregisterEvent(hdl);
						} else {
							if (hdl->timerSpec.count > 0) {
								hdl->timerSpec.count--;
							}
							do {
								hdl->timerSpec.next += hdl->timerSpec.delay;
							} while(hdl->timerSpec.next < micros());
						}
					}
				} else if (hdl->type == event_input) {
					// is it for this interrupt ?
					if (hdl->inputSpec.interrupt == eventQueueDetails[q]) {
						hdl->callback(eventQueueDetails[q], hdl->data);
					}
				} else if (hdl->type >= 10) {
					hdl->callback(eventQueueDetails[q], hdl->data);
				}
			}
		}
	}

	// compute next time
	unsigned long next = 100000L; // by default, wait 0.1s
	for(int h = 0; h < eventHandlerMax; h++) {
		eventHandler *hdl = &(handlers[h]);
		if (hdl->type == event_timer) {
			if (next > hdl->timerSpec.next) {
				next = hdl->timerSpec.next;
			}
		}
	}

	// wait in a interruptible manner
	delayIdle(next - micros());
}

Events::eventHandler *Events::registerEvent(Events::eventType type, Events::eventCallback callback, void *data) {
	int i = findNewHandler();
	if (i == -1) {
		return 0;
	}
	Events::eventHandler *result = &(handlers[i]);
	result->type = type;
	result->id = i;
	result->callback = callback;
	result->data = data;
	return result;
}

Events::eventHandler *Events::registerTimeout(long ms, Events::eventCallback callback, void *data) {
	return registerInterval(ms, 1, callback, data);
}
Events::eventHandler *Events::registerInterval(long ms, Events::eventCallback callback, void *data) {
	return registerInterval(ms, -1, callback, data);
}
Events::eventHandler *Events::registerInterval(long ms, int count, Events::eventCallback callback, void *data) {
	Events::eventHandler *result = registerEvent(event_timer, callback, data);
	if (result == 0) {
		return 0;
	}

	result->timerSpec.delay = ms;
	result->timerSpec.next = micros() + ms * 1000;
	result->timerSpec.count = count;

	// TODO : update global delay if empty or if "next" is before

	return result;
}

Events::eventHandler *Events::registerInput(byte input, byte mode, Events::eventCallback callback, void *data) {
	Events::eventHandler *result = registerEvent(event_input, callback, data);
	if (result == 0) {
		return 0;
	}

	result->inputSpec.interrupt = INTERRUPT_FOR_PIN(input);
	if (result->inputSpec.interrupt == -1) {
		return 0;
	}
	result->inputSpec.pin = input;
	result->inputSpec.mode = mode;

	bool foundOther = false;
	for(int i = 0; i < eventHandlerMax; i++) {
		if (i != result->id && handlers[i].type == event_input
				&& handlers[i].inputSpec.pin == result->inputSpec.pin) {
			foundOther = true;
			break;
		}
	}
	if (!foundOther) {
		enableInputInterrupt(result->inputSpec.interrupt, mode);
	}

	return result;
}

bool Events::unregisterEvent(Events::eventHandler *handler) {
	if (handler->type == event_input) {
		bool foundOther = false;
		for(int i = 0; i < eventHandlerMax; i++) {
			if (i != handler->id && handlers[i].type == event_input
					&& handlers[i].inputSpec.pin == handler->inputSpec.pin) {
				foundOther = true;
				break;
			}
		}
		if (!foundOther) {
			disableInputInterrupt(handler->inputSpec.interrupt);
		}
	}

	handler->type = event_free;
	return true;
}

int Events::findNewHandler() {
	for(int i = 0; i < eventHandlerMax; i++) {
		if (handlers[i].type == event_free) {
			return i;
		}
	}
	// no free handlers, grow list
	eventHandlerMax += HANDLER_ALLOC_BLOCK_SIZE;
	handlers = (eventHandler *)realloc(handlers, eventHandlerMax * sizeof(eventHandler));
	if (handlers == 0) {
		// can't realloc => abort.
		return -1;
	}

	// initialize last HANDLER_ALLOC_BLOCK_SIZE blocks as free
	for(int i = eventHandlerMax - HANDLER_ALLOC_BLOCK_SIZE; i < eventHandlerMax; i++) {
		handlers[i].type = event_free;
	}
	// and return first of them.
	return eventHandlerMax - HANDLER_ALLOC_BLOCK_SIZE;
}
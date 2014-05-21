#define NOT_IN_MAIN
#define USE_INTERRUPT_INPUT_HANDLER
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
		byte subType; // may be input pin, serial number ...
		// for timeout / interval events
		struct {
			unsigned long delay; // number of ms
			unsigned long next;  // date of next tick
			unsigned int count;  // remaining tick number ( <0 if infinite)
		} timerSpec;
		// for input events
		struct {
			byte pin;
			byte interrupt;
			byte mode;
		} inputSpec;
		// for analog comparator events
		struct {
			byte mode;
		} analogCompSpec;
	};
};

// fire only one analog comp event per event loop
volatile bool analogCompAlready;

Events::Events() {
	eventHandlerMax = 0;
	handlers = 0;
	queueSize = 0;
	eventQueueTypes = 0;
	eventQueueDetails = 0;
	eventLoop = 0;

	defaultTimer = DEFAULT_EVENTS_TIMER;
	defaultTimeout = DEFAULT_EVENTS_TIMEOUT;
}

void Events::begin(byte defaultTimer, unsigned long defaultTimeout) {
	eventQueueTypes = (byte *)malloc(MAX_QUEUE_SIZE);
	eventQueueDetails = (short *)malloc(MAX_QUEUE_SIZE);
	this->defaultTimer = defaultTimer;
	this->defaultTimeout = defaultTimeout;
}

void Events::begin() {
	begin(DEFAULT_EVENTS_TIMER, DEFAULT_EVENTS_TIMEOUT);
}

bool Events::fire(Events::eventType type) {
	return fire(type, -1);
}
bool Events::fire(Events::eventType type, short detail) {
//	delayCancel(defaultTimer);
	byte qs = queueSize;
	if (qs >= MAX_QUEUE_SIZE) {
		// full => ignore
		return false;
	}
	eventQueueTypes[qs] = type;
	eventQueueDetails[qs] = detail;
	queueSize++;

	return true;
}

void Events::waitNext() {
	waitNext(DEFAULT_SLEEP_MODE);
}

void Events::waitNext(word sleepMode) {
	if (queueSize == MAX_QUEUE_SIZE) {
		LOG("Queue was full");
	}

	bool found = false;
	unsigned long next = 0xffffffffL;

	// loop on event queue THEN on timers
	// but at end of event queue, we may have new items in queue => must loop
	while (queueSize > 0) {
		// for each fired events, call callback
		for (int q = 0; q < queueSize; q++) {
			for (int h = 0; h < eventHandlerMax; h++) {
				if (eventQueueTypes[q] == handlers[h].type) {
					// this handler may be for this event
					eventHandler *hdl = &(handlers[h]);

					if (hdl->type == event_input) {
						// is it for this interrupt ?
						if (hdl->inputSpec.interrupt == eventQueueDetails[q]) {
							hdl->callback(eventQueueDetails[q], hdl->data);
						}
					} else if (hdl->type == event_analogcomp) {
						bool state = analogCompValue() ? RISING : FALLING;
						if (hdl->analogCompSpec.mode == CHANGE || hdl->analogCompSpec.mode == state) {
							hdl->callback(state, hdl->data);
						}
					} else if (hdl->type >= 10) {
						hdl->callback(eventQueueDetails[q], hdl->data);
					}
				}
			}
		}
		queueSize = 0;

		// look at expired timer events + compute next time
		// TODO : must avoid to use millis/micros
		// => must compute our own time ellapsed since preceding call, using timer values
		unsigned long now = (millis() * 1000) + (micros() % 1000);
		next = 0xffffffffL;

		for(int h = 0; h < eventHandlerMax; h++) {
			eventHandler *hdl = &(handlers[h]);

			if (hdl->type == event_timer) {
				// is it expired ? => handle
				if (hdl->timerSpec.next <= now + TIMER_EVENT_PRECISION) {
					hdl->callback(-1, hdl->data);
					if (hdl->timerSpec.count == 1) {
						unregisterEvent(hdl);
					} else {
						if (hdl->timerSpec.count > 0) {
							hdl->timerSpec.count--;
						}
						do {
							hdl->timerSpec.next += hdl->timerSpec.delay * 1000;
						} while(hdl->timerSpec.next < now + TIMER_EVENT_PRECISION);
					}
				}

				if (next > hdl->timerSpec.next) {
					next = hdl->timerSpec.next;
					found = true;
				}
			}
		}
	}

	if (!found) {
		if (defaultTimeout == 0) {
			sleepNow(sleepMode);
		} else {
			// wait in a interruptible manner, until default timeout
			delayIdleWith(defaultTimeout, defaultTimer, sleepMode, true);
		}
	} else {
		// wait in a interruptible manner
		// TODO : remove time ellapsed since "now" calculation, but risks to be too late
		// round value too TIMER_EVENT_PRECISION multiple
		long delay = next - now, rem = delay % TIMER_EVENT_PRECISION;
		if (rem > TIMER_EVENT_PRECISION / 2) {
			delay -= rem + TIMER_EVENT_PRECISION;
		} else {
			delay -= rem;
		}
		delayIdleWith(delay, defaultTimer, sleepMode, true);
	}

	analogCompAlready = false;
	eventLoop++;
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

Events::eventHandler *Events::registerTimeout(unsigned long ms, Events::eventCallback callback, void *data) {
	return registerInterval(ms, 1, callback, data);
}
Events::eventHandler *Events::registerInterval(unsigned long ms, Events::eventCallback callback, void *data) {
	return registerInterval(ms, -1, callback, data);
}
Events::eventHandler *Events::registerInterval(unsigned long ms, int count, Events::eventCallback callback, void *data) {
	Events::eventHandler *result = registerEvent(event_timer, callback, data);
	if (result == 0) {
		return 0;
	}

	result->timerSpec.delay = ms;
	result->timerSpec.next = 0; //(millis() + ms) * 1000 + micros();
	result->timerSpec.count = count;

	// TODO : update global delay if empty or if "next" is before

	return result;
}

void inputHandler(int interrupt) {
	Scheduler.fire(Events::event_input, interrupt);
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

	if (!findOther(event_input, result->inputSpec.pin, result->id)) {
		setInputHandler(result->inputSpec.interrupt, inputHandler);
		enableInputInterrupt(result->inputSpec.interrupt, mode);
	}

	return result;
}

void analogCompHandler(int interrupt) {
	if (analogCompAlready) return;
	analogCompAlready = true;
	Scheduler.fire(Events::event_analogcomp, 0); // event loop handler will get analog comp value
	// to avoid bouncing values
}

Events::eventHandler *Events::registerAnalogComp(byte mode, Events::eventCallback callback, void *data) {
	Events::eventHandler *result = registerEvent(event_analogcomp, callback, data);
	if (result == 0) {
		return 0;
	}

	result->analogCompSpec.mode = mode;

	if (!findOther(event_analogcomp, 0xff, result->id)) {
		setAnalogCompHandler(analogCompHandler);
		enableAnalogCompInterrupt(CHANGE);
	}

	return result;
}

bool Events::unregisterEvent(Events::eventHandler *handler) {
	if (handler->type == event_input) {
		if (!findOther(event_input, handler->inputSpec.pin, handler->id)) {
			disableInputInterrupt(handler->inputSpec.interrupt);
			setInputHandler(handler->inputSpec.interrupt, 0);
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

bool Events::findOther(byte type, byte subType, byte excepted) {
	for(int i = 0; i < eventHandlerMax; i++) {
		if (i != excepted
				&& handlers[i].type == type
				&& (subType == 0xff || handlers[i].subType == subType)) {
			return true;
		}
	}
	return false;
}

Events Scheduler;

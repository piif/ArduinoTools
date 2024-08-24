#define NOT_IN_MAIN
#include "events.h"
#include "delayIdle.h"

#ifdef WITHOUT_MILLIS_FUNCTIONS
#include "myMillis.h"
#endif

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
	maxQueueSize = 0;
	eventQueueTypes = 0;
	eventQueueDetails = 0;
	eventLoop = 0;

	defaultTimer = DEFAULT_EVENTS_TIMER;
	defaultTimeout = DEFAULT_EVENTS_TIMEOUT;
}

void Events::begin(byte defaultTimer, unsigned long defaultTimeout) {
	eventQueueTypes = (byte *)malloc(MAX_QUEUE_SIZE * sizeof(byte));
	eventQueueDetails = (int *)malloc(MAX_QUEUE_SIZE * sizeof(int));
	this->defaultTimer = defaultTimer;
	this->defaultTimeout = defaultTimeout;
}

void Events::begin() {
	begin(DEFAULT_EVENTS_TIMER, DEFAULT_EVENTS_TIMEOUT);
}

bool Events::fire(Events::eventType type) {
	return fire(type, -1);
}
bool Events::fire(Events::eventType type, int detail) {
//	delayCancel(defaultTimer);
	byte qs = queueSize++;
	if (maxQueueSize < queueSize) {
		maxQueueSize = queueSize;
	}
	if (qs >= MAX_QUEUE_SIZE) {
		// full => ignore
		return false;
	}
	eventQueueTypes[qs] = type;
	eventQueueDetails[qs] = detail;

	return true;
}

void Events::waitNext() {
	waitNext(DEFAULT_SLEEP_MODE);
}

void Events::waitNext(word sleepMode) {
	if (queueSize >= MAX_QUEUE_SIZE) {
		LOG("Queue was full");
		queueSize = MAX_QUEUE_SIZE;
	}

	bool found = false;
	unsigned long now = 0, next = 0xffffffffL;

	// loop on event queue THEN on timers
	// but at end of event queue, we may have new items in queue => must loop
	do {
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
		// if a handler fired an event in queue, loop will continue on it
		// => here, we can empty it safely
		queueSize = 0;

		// look at expired timer events + compute next time
		// TODO : must avoid to use millis/micros
		// => must compute our own time elapsed since preceding call, using timer values
#ifdef WITHOUT_MILLIS_FUNCTIONS
		now = myMillis() * 1000;
#else
		now = (millis() * 1000) + (micros() % 1000);
#endif
		next = 0xffffffffL;

		for (int h = 0; h < eventHandlerMax; h++) {
			eventHandler *hdl = &(handlers[h]);

			if (hdl->type == event_timer) {
				// is it expired ? => handle
				if (hdl->timerSpec.next <= now + TIMER_EVENT_PRECISION) {
					hdl->callback(hdl->timerSpec.count - 1, hdl->data);
					if (hdl->timerSpec.count == 1) {
						// last timer iteration => remove it
						unregisterEvent(hdl);
						hdl->timerSpec.next  = 0xffffffffL;
					} else {
						if (hdl->timerSpec.count > 0) {
							// remaining iterations => just update counter
							hdl->timerSpec.count--;
						}
						// infinite iterations, or remaining ones => compute next time
						do {
							hdl->timerSpec.next += hdl->timerSpec.delay * 1000;
							// loop over iterations in case we missed some
						} while(hdl->timerSpec.next < now + TIMER_EVENT_PRECISION);
					}
				}

				if (next > hdl->timerSpec.next) {
					next = hdl->timerSpec.next;
					found = true;
				}
			}
		}
		// here, timer handler having queued new events must be handled => loop
	} while (queueSize > 0);

	if (!found) {
		if (defaultTimeout == 0) {
			sleepNow(sleepMode);
		} else {
			// wait in a interruptible manner, until default timeout
			delayIdleWith(defaultTimeout, defaultTimer, sleepMode, true);
		}
	} else {
		// wait in a interruptible manner
		// TODO : remove time elapsed since "now" calculation, but risks to be too late
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

void Events::dump(Stream &s) {
	s.print("* max queueSize was ");s.println(maxQueueSize);
	maxQueueSize = 0;

	for (int q = 0; q < queueSize && q < MAX_QUEUE_SIZE; q++) {
		s.print("* q["); s.print(q); s.print("] = ");
		s.print(eventQueueTypes[q]); s.print(" / ");; s.println(eventQueueDetails[q]);
	}
	for (int h = 0; h < eventHandlerMax; h++) {
		eventHandler *hdl = &(handlers[h]);
		s.print("* h["); s.print(h); s.print(" ("); s.print((int)hdl); s.print(")] = ");
		if (hdl->type == event_free) {
			s.println("-");
		} else {
			s.print(hdl->type); s.print(" / "); s.print(hdl->id); s.print(" / "); s.println((int)hdl->callback);
		}
		switch(hdl->type) {
		case event_input:
			s.print("*  input: pin ");
			s.print(hdl->inputSpec.pin); s.print(" / int "); s.print(hdl->inputSpec.interrupt); s.print(" / mode "); s.println(hdl->inputSpec.mode);
			break;
		case event_timer:
			s.print("*  timer: count ");
			s.print(hdl->timerSpec.count); s.print(" / delay "); s.print(hdl->timerSpec.delay); s.print(" / next "); s.println(hdl->timerSpec.next);
			break;
		case event_analogcomp:
			s.print("*  analog: mode ");
			s.println(hdl->analogCompSpec.mode);
			break;
		case event_serial:
			s.println("*  serial: Not Implemented ");
			break;
		case event_twi:
			s.println("*  twi: Not Implemented ");
			break;
		case event_free:
			break;
		}
	}
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
#ifdef WITHOUT_MILLIS_FUNCTIONS
	result->timerSpec.next = (myMillis() + ms) * 1000;
#else
	result->timerSpec.next = (millis() + ms) * 1000 + (micros() % 1000);
#endif
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

#ifndef EVENTS_EVENTS_H
#define EVENTS_EVENTS_H 1

// CALLER MUST DECIDE WHICH KIND OF EVENTS HE WANTS, BY #DEFINING
// ONE OR SEVERAL OF
// - USE_INTERRUPT_INPUT_HANDLER
// - USE_INTERRUPT_TIMER_HANDLER
// - USE_INTERRUPT_SERIAL_HANDLER
// - USE_INTERRUPT_TWI_HANDLER
// BEFORE THIS #INCLUSION
#include <Arduino.h>

#include <ArduinoTools.h>

class Events {
	typedef enum { free=0, timeout=1, interval=2 } eventType;
	// TODO : pin input interrupt, serial
	// > 10 for user defined

	typedef void (*eventCallback)(void *data);

	typedef struct _eventHandler {
		Events::eventType type;
		int id; // = rank in handler map
		eventCallback callback;
		void *data;
		// specific to events => replace by a union if other types need other fields
		long delay; // number of ms for timeout / interval events
		long next;  // date of next tick for timeout / interval events
	} eventHandler;

	void begin();

	eventHandler *registerEvent(eventType type, eventCallback callback, void *data);
	eventHandler *registerTimeout(long ms, eventCallback callback, void *data);
	eventHandler *registerInterval(long ms, eventCallback callback, void *data);
	bool unregisterEvent(eventHandler *handler);

	void fire(eventType type);

	// to call repeatedly
	void waitNext();

private:
	// eventHandler[]
	// eventQueue[]

};

#endif

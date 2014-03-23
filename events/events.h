#ifndef EVENTS_EVENTS_H
#define EVENTS_EVENTS_H 1
#include <Arduino.h>


// CALLER MUST DECIDE WHICH KIND OF EVENTS HE WANTS, BY #DEFINING
// ONE OR SEVERAL OF
// - USE_INTERRUPT_INPUT_HANDLER
// - USE_INTERRUPT_SERIAL_HANDLER
// - USE_INTERRUPT_TWI_HANDLER
// BEFORE THIS #INCLUSION
// TIMER must be on
#define USE_INTERRUPT_TIMER_HANDLER
#include <ArduinoTools.h>

class Events {
public:
	typedef enum {
		event_free = 0,
		event_timer = 1,
		event_input = 2
	} eventType;
	// TODO : pin input interrupt, serial
	// > 10 for user defined

	typedef void (*eventCallback)(byte detail, void *data);

	typedef struct _eventHandler eventHandler;

	void begin();

	/**
	 * Register a user event handler, with given type (shoud be >= 10)
	 * Returns a pointer to an eventHandler structure
	 */
	eventHandler *registerEvent(eventType type, eventCallback callback, void *data);

	/**
	 * Fire a timer event after ms milliseconds, which will be handled
	 * by callback, with data as second argument
	 * Callback first argument is unused
	 * Returns a pointer to an eventHandler structure
	 */
	eventHandler *registerTimeout(unsigned long ms, eventCallback callback, void *data);

	/**
	 * Fire a timer event every ms milliseconds, which will be handled
	 * by callback, with data as second argument
	 * Callback first argument is unused
	 * Returns a pointer to an eventHandler structure
	 */
	eventHandler *registerInterval(unsigned long ms, eventCallback callback, void *data);

	/**
	 * Fire a timer event, count times, every ms milliseconds.
	 * Event will be handled by callback, with data as second argument
	 * Callback first argument is unused
	 * Returns a pointer to an eventHandler structure
	 */
	eventHandler *registerInterval(unsigned long ms, int count, eventCallback callback, void *data);

	/**
	 * Fire a "input" event each time hardware event "mode" occurs on pin "input"
	 * Event will be handled by callback, with data as second argument
	 * Callback first argument is input number
	 * Returns a pointer to an eventHandler structure
	 */
	eventHandler *registerInput(byte input, byte mode, eventCallback callback, void *data);

	/**
	 * unregisters given handler
	 */
	bool unregisterEvent(eventHandler *handler);

	/**
	 * fire a new event with given type and detail
	 */
	void fire(eventType type, short detail);
	void fire(eventType type);

	/**
	 * to call repeatedly, to handle queued events
	 */
	void waitNext();
	void waitNext(word sleepMode);

private:
	byte eventHandlerMax;
	eventHandler *handlers;
	volatile byte queueSize;
	byte *eventQueueTypes;
	short *eventQueueDetails;

	int findNewHandler();
};

extern Events Scheduler;

#endif

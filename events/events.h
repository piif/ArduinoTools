#ifndef EVENTS_EVENTS_H
#define EVENTS_EVENTS_H
#include <Arduino.h>

// CALLER MUST DECIDE WHICH KIND OF EVENTS HE WANTS, BY #DEFINING
// ONE OR SEVERAL OF
// - USE_INTERRUPT_TIMER_HANDLER_x (must match defaultTimer value)
// - USE_INTERRUPT_INPUT_HANDLER_x
// - USE_INTERRUPT_ANALAGCOMP_HANDLER
// - USE_INTERRUPT_SERIAL_HANDLER_x
// - USE_INTERRUPT_TWI_HANDLER_x
// BEFORE THIS #INCLUSION
// if TIMER0 is used, it must override default one (used for timing functions)
// => option -Wl,--allow-multiple-definition is needed or linking fails.

// timer used for events "wait" loops
#define DEFAULT_EVENTS_TIMER 2
#define DEFAULT_EVENTS_TIMEOUT 100000L

// timer events are handled with a precision of 1024µs
// 1024 is one of the timer prescales => will need less timer loops
#define TIMER_EVENT_PRECISION 1024

#include <ArduinoTools.h>

class Events {
public:
	typedef enum {
		event_free = 0,
		event_input = 1,
		event_timer = 2,
		event_analogcomp = 3,
		event_serial = 4, // TODO
		event_twi = 5 // TODO
	} eventType;
	// > 10 for user defined

	typedef void (*eventCallback)(byte detail, void *data);

	typedef struct _eventHandler eventHandler;

	/**
	 * defaultTimer : Timer to use for delays. May be modified before any use (before or just after begin() call)
	 * defaults to DEFAULT_EVENTS_TIMER
	 * /!\ matching USE_INTERRUPT_TIMER_x must be defined by caller, excepted if timer methods
	 * are not used and defaultTimeout is set to 0.
	 *
	 * Timeout for waitNext() calls when no timer is active.
	 * 0 means to wait forever, until any interrupt occurs.
	 * Defaults to 100000L = 0.1s
	 */
	void begin(byte defaultTimer, unsigned long defaultTimeout);
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
	 * mode may be LOW, HIGH CHANGE or RAISING
	 * Callback first argument is input number
	 * Returns a pointer to an eventHandler structure
	 */
	eventHandler *registerInput(byte input, byte mode, eventCallback callback, void *data);

	/**
	 * Fire a "analogcomp" event each time event "mode" occurs on analog comparator
	 * Event will be handled by callback, with data as second argument
	 * mode may be CHANGE, FALLING or RISING
	 * Returns a pointer to an eventHandler structure
	 */
	eventHandler *registerAnalogComp(byte mode, eventCallback callback, void *data);

	/**
	 * unregisters given handler
	 */
	bool unregisterEvent(eventHandler *handler);

	/**
	 * Fire a new event with given type and detail.
	 * Returns false if queue is full
	 */
	bool fire(eventType type, short detail);
	bool fire(eventType type);

	/**
	 * to call repeatedly, to handle queued events
	 */
	void waitNext();
	void waitNext(word sleepMode);

	Events();

private:
	byte defaultTimer;
	unsigned long defaultTimeout;

	byte eventHandlerMax;
	eventHandler *handlers;
	volatile byte queueSize;
	byte *eventQueueTypes;
	short *eventQueueDetails;

	int findNewHandler();
	bool findOther(byte type, byte subtype, byte excepted);

//	void inputHandler(int interrupt);
};

extern Events Scheduler;

#endif

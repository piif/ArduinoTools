/**
 * make arduino idle during a given delay, by use of internal timers
 * User can decide which timer to use (timer DEFAULT_DELAY_TIMER by default)
 */

#ifndef EVENTS_DELAYIDLE_H
#define EVENTS_DELAYIDLE_H 1

#include <Arduino.h>

#define USE_INTERRUPT_TIMER_HANDLER
#include <ArduinoTools.h>

extern int innerloops;
/**
 * default timer to use
 */
#define DEFAULT_DELAY_TIMER 1

/**
 * default sleep mode to use
 */
#define DEFAULT_SLEEP_MODE SLEEP_MODE_IDLE

/**
 * This methods returns after "microseconds" µs, but may be cancelled by
 * delayCancel() calls.
 * Returns true if delay was achieved, false if it was interrupted
 */
bool delayIdle(long microseconds);

/**
 * Same method as delayIdle(), but specify a sleep mode to use
 * Be careful to choose a mode which doesn't disable the timer !
 */
bool delayIdle(long microseconds, word sleep_mode);

/**
 * Same method as delayIdle(), but specify a timer to use
 */
bool delayIdleWith(long microseconds, byte timer);

/**
 * Same method as delayIdleWith(), but specify a sleep mode to use
 * Be careful to choose a mode which doesn't disable the timer !
 */
bool delayIdleWith(long microseconds, byte timer, word sleep_mode);

/**
 * An interrupt handler may call this method to cancel a running delayIdle()
 */
void delayCancel();

/**
 * Force to sleep mode, until "something" wake CPU on
 * The waited event must be enabled, thus choose sleep mode carefully
 */
void sleepNow(word sleep_mode);

/**
 * Like sleepMode, with default sleep mode
 */
void sleepNow();

extern void delayIdleISR();

#endif

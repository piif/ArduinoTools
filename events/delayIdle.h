/**
 * make arduino idle during a given delay, by use of internal timers
 * User can decide which timer to use (timer DEFAULT_DELAY_TIMER by default)
 */

#ifndef EVENTS_DELAYIDLE_H
#define EVENTS_DELAYIDLE_H 1

#define USE_INTERRUPT_TIMER_HANDLER
#include "LowLevelTools.h"

// for sleep modes debug
extern int innerloops;

/**
 * default timer to use
 */
#define DEFAULT_DELAY_TIMER 1

// TODO : add an option to handle delay with Watchdog Timer instead => no timer used, but less precision

/**
 * This methods returns after "microseconds" µs, but may be cancelled by
 * delayCancel() calls.
 * Returns true if delay was achieved, false if it was interrupted
 */
bool delayIdle(unsigned long microseconds);
bool delayOrInterruptIdle(unsigned long microseconds);

/**
 * Same method as delayIdle(), but specify a sleep mode to use
 * Be careful to choose a mode which doesn't disable the timer !
 */
bool delayIdle(unsigned long microseconds, word sleep_mode);
bool delayOrInterruptIdle(unsigned long microseconds, word sleep_mode);

/**
 * Same method as delayIdle(), but specify a timer to use
 */
bool delayIdleWith(unsigned long microseconds, byte timer);
bool delayOrInterruptIdleWith(unsigned long microseconds, byte timer);

/**
 * Same method as delayIdleWith(), but specify a sleep mode to use
 * Be careful to choose a mode which doesn't disable the timer !
 */
bool delayIdleWith(unsigned long microseconds, byte timer, word sleep_mode);
bool delayOrInterruptIdleWith(unsigned long microseconds, byte timer, word sleep_mode);
bool delayIdleWith(unsigned long microseconds, byte timer, word sleep_mode, bool cancelOnInterrupt);

/**
 * An interrupt handler may call this method to cancel a running delayIdle()
 */
void delayCancel(byte timer);

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

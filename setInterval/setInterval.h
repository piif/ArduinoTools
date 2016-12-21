/**
 * simple library to repeat calls to a function every given delay, like js setInterval function
 * this version doesn't use timers nor interrupts
 */

#ifndef SET_INTERVAL_H
#define SET_INTERVAL_H

#ifdef __cplusplus
//extern "C" {
#endif

/**
 * constant to return on error
 */
#define SET_INTERVAL_ERROR ((setIntervalTimer)-2)
#define SET_INTERVAL_FREE 0
#define SET_INTERVAL_PAUSED -1

/**
 * max number of simultaneous timers
 */
#define MAX_INTERVAL 10

/**
 * handle returned by setInterval
 */
typedef int setIntervalTimer;

/**
 * prototype for callback functions
 * void *userData : pointer passed to setInterval call
 * long delta : called function with ms late
 * int missed : number of call missed
 */
typedef void (setIntervalFunction)(void *, long, int);

/**
 * define a timer which calls "callback" function every "ms" milliseconds
 */
setIntervalTimer setInterval(long ms, setIntervalFunction *callback, void *userData);

/**
 * change delay for this timer
 */
setIntervalTimer changeInterval(setIntervalTimer timer, long ms);
setIntervalTimer changeInterval(setIntervalTimer timer, long ms, setIntervalFunction *callback, void *userData);

/**
 * function to call from main loop() to handle timers
 */
void setIntervalStep();

#ifdef __cplusplus
//}
#endif

#endif

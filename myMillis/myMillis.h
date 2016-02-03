#include <Arduino.h>

#define CLOCK_TICKS_PER_MILLI (F_CPU / 1000)

// number of milliscond since last initTimerMillis()
extern volatile unsigned long timer0_millis;

// clock cycle counter, updated after each timer0 overflow
extern volatile unsigned long clock_ticks;

// how many clock ticks there are for one timer0 overflow
extern volatile unsigned long clock_ticks_per_ovf;

// Initialize timer0 with given specifications, and deduce
// how to count millis
// wgm : kind of pwm to use
// top : OCR0A value
// prescale : one of authorized values for timer0 prescaler (1, 8, 64, 256, 1024)
// returns -1 if bad argument
short initTimerMillis(byte wgm, byte top, byte prescale);

// return current millis counter
unsigned long myMillis() ;

// blocks for m millis
void myDelay(unsigned long m);

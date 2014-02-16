#ifndef PWM_PWM_H
#define PWM_PWM_H 1

#include "Arduino.h"
#include "avr/io.h"

/**
 * Timer 0 :
 * - used by delay & delayMicrosecond
 * - output on pins 6 et 5
 * - 8 bits
 * - prescaler 1, 8, 64, 256, or 1024
 *
 * Timer 1 :
 * - output on pins 9 et 10
 * - 16 bits
 * - same prescaler as timer 0
 * - Input Capture Unit (?)
 *
 * Timer 2 :
 * - output on pins 11 et 3
 * - 8 bits
 * - prescaler 1, 8, 32, 64, 128, 256, 1024
 * - ability to use external clock (?)
 */

#define FREQUENCY_TO_PERIOD(f) (F_CPU/(f))

// Compare Output Mode values :
// TODO : made from doc for Timer1, verify for timers 0 and 2
#define COMPARE_OUTPUT_MODE_NONE     0
#define COMPARE_OUTPUT_MODE_TOGGLE   1
#define COMPARE_OUTPUT_MODE_NORMAL   2
#define COMPARE_OUTPUT_MODE_INVERTED 3

// TODO : constants for WGM values : to be documented !
#define WGM_0_FAST_MAX   3
#define WGM_0_FAST_OCRA  7
#define WGM_0_PHASE_MAX  1
#define WGM_0_PHASE_OCRA 5

#define WGM_1_FAST_MAX   0
#define WGM_1_FAST_ICR   14
#define WGM_1_FAST_OCRA  15
#define WGM_1_PHASE_ICR  8
#define WGM_1_PHASE_OCRA 9

#define WGM_2_FAST_MAX   3
#define WGM_2_FAST_OCRA  7
#define WGM_2_PHASE_MAX  1
#define WGM_2_PHASE_OCRA 5

// Arduino output associated with PWM outs
#define PWM_0_A 6
#define PWM_0_B 5
#define PWM_1_A 9
#define PWM_1_B 10
#define PWM_2_A 11
#define PWM_2_B 3

// clock prescalers
#define PWM0_PRESCALER_1    1
#define PWM0_PRESCALER_8    2
#define PWM0_PRESCALER_64   3
#define PWM0_PRESCALER_256  4
#define PWM0_PRESCALER_1024 5

#define PWM1_PRESCALER_1    1
#define PWM1_PRESCALER_8    2
#define PWM1_PRESCALER_64   3
#define PWM1_PRESCALER_256  4
#define PWM1_PRESCALER_1024 5

#define PWM2_PRESCALER_1    1
#define PWM2_PRESCALER_8    2
#define PWM2_PRESCALER_32   3
#define PWM2_PRESCALER_64   4
#define PWM2_PRESCALER_128  5
#define PWM2_PRESCALER_256  6
#define PWM2_PRESCALER_1024 7

// version "bas niveau"
extern void setPWM(
		byte pwm, unsigned int icr,
		byte com_a, unsigned int ocr_a,
		byte com_b, unsigned int ocr_b,
		byte wgm, byte cs);

extern void setPWMmode(
		byte pwm,
		byte com_a, byte com_b);

// Limité au mode Fast non-inverted, mais en utilisant ICR, et en
// calculant ICR+CS pour une période donnée
// TODO !
extern void setPWMsimple(
		byte pwm, unsigned long period,
		unsigned int dutyA, unsigned int dutyB);

#endif

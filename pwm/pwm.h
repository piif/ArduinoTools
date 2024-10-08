#ifndef PWM_PWM_H
#define PWM_PWM_H 1

#include <Arduino.h>

// TODO : add ATtiny defines

/**
 * On Arduino UNO :
 * Timer 0 :
 * - used by delay & delayMicrosecond
 * - output on pins 6 and 5
 * - 8 bits
 * - prescaler 1, 8, 64, 256, or 1024
 *
 * Timer 1 :
 * - output on pins 9 and 10
 * - 16 bits
 * - same prescaler as timer 0
 * - Input Capture Unit (?)
 *
 * Timer 2 :
 * - output on pins 11 and 3
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
#define WGM_NORMAL 0

#define WGM_0_FAST_MAX   3
#define WGM_0_FAST_OCRA  7
#define WGM_0_PHASE_MAX  1
#define WGM_0_PHASE_OCRA 5

#if defined __AVR_ATtinyX5__
#define SET_PWM_1_NONE  0
#define SET_PWM_1_A     1
#define SET_PWM_1_B     2
#define SET_PWM_1_AB    3

// to be ORed with SET_PWM_1_*
#define WGM_1_FAST_MAX   0
#define WGM_1_FAST_OCR1C   4

// not PWM
#define COMPARE_OUTPUT_MODE_CLEAR 2
#define COMPARE_OUTPUT_MODE_SET   3
// PWM mode : OC1x / !OC1x
#define COMPARE_OUTPUT_MODE_1_NONE_NONE  0
#define COMPARE_OUTPUT_MODE_1_CLEAR_SET  1
#define COMPARE_OUTPUT_MODE_1_CLEAR_NONE 2
#define COMPARE_OUTPUT_MODE_1_SET_NONE   3

#else
#define WGM_1_FAST_MAX   0
#define WGM_1_FAST_ICR   14
#define WGM_1_FAST_OCRA  15
#define WGM_1_PHASE_ICR  8
#define WGM_1_PHASE_OCRA 9
#endif

#define WGM_2_FAST_MAX   3
#define WGM_2_FAST_OCRA  7
#define WGM_2_PHASE_MAX  1
#define WGM_2_PHASE_OCRA 5

// TODO : verify values for each target + add 4&5 for mega
#define WGM_3_FAST_MAX   0
#define WGM_3_FAST_ICR   14
#define WGM_3_FAST_OCRA  15
#define WGM_3_PHASE_ICR  8
#define WGM_3_PHASE_OCRA 9

// Arduino output associated with PWM outs
#if defined __AVR_ATmega2560__

/** MEGA */
#define PWM_0_A 13
#define PWM_0_B 4
#define PWM_1_A 11
#define PWM_1_B 12
#define PWM_1_C 13
#define PWM_2_A 10
#define PWM_2_B 9
#define PWM_3_A 5
#define PWM_3_B 2
#define PWM_3_C 3
#define PWM_4_A 6
#define PWM_4_B 7
#define PWM_4_C 8
#define PWM_5_A 46
#define PWM_5_B 45
#define PWM_5_C 44

#elif defined __AVR_ATmega328P__

/** UNO */
#define PWM_0_A 6
#define PWM_0_B 5
#define PWM_1_A 9
#define PWM_1_B 10
#define PWM_2_A 11
#define PWM_2_B 3

#elif defined __AVR_ATmega32U4__

/** Micro / Leonardo / Yun */
#define PWM_0_A 11
#define PWM_0_B 3
#define PWM_1_A 9
#define PWM_1_B 10
#define PWM_1_C 11
#define PWM_3_A 5
#define PWM_4_A 13
#define PWM_4_B 10
#define PWM_4_C 6

#elif defined __AVR_ATtinyX5__

/** ATtiny *5 */
#define PWM_0_A 0
#define PWM_NOT_1_A 0
#define PWM_0_B 1
#define PWM_1_A 1
#define PWM_1_B 4
#define PWM_NOT_1_B 3

#else
#error Unknown arduino model
#endif

// clock prescalers
// TODO verify values for 2560 and 32u4

#define PWMx_PRESCALER_OFF  0

// true for timer 0, 1, 3, 4, 5 excepted timer4 on 32u4
#define PWMx_PRESCALER_1    1
#define PWMx_PRESCALER_8    2
#define PWMx_PRESCALER_64   3
#define PWMx_PRESCALER_256  4
#define PWMx_PRESCALER_1024 5

// for compatibility
#define PWM0_PRESCALER_1    PWMx_PRESCALER_1
#define PWM0_PRESCALER_8    PWMx_PRESCALER_8
#define PWM0_PRESCALER_64   PWMx_PRESCALER_64
#define PWM0_PRESCALER_256  PWMx_PRESCALER_256
#define PWM0_PRESCALER_1024 PWMx_PRESCALER_1024

#if defined __AVR_ATtinyX5__
#define PWM1_PRESCALER_1      1
#define PWM1_PRESCALER_2      2
#define PWM1_PRESCALER_4      3
#define PWM1_PRESCALER_8      4
#define PWM1_PRESCALER_16     5
#define PWM1_PRESCALER_32     6
#define PWM1_PRESCALER_64     7
#define PWM1_PRESCALER_128    8
#define PWM1_PRESCALER_256    9
#define PWM1_PRESCALER_512   10
#define PWM1_PRESCALER_1024  11
#define PWM1_PRESCALER_2048  12
#define PWM1_PRESCALER_4096  13
#define PWM1_PRESCALER_8192  14
#define PWM1_PRESCALER_16384 15
#else
#define PWM1_PRESCALER_1    PWMx_PRESCALER_1
#define PWM1_PRESCALER_8    PWMx_PRESCALER_8
#define PWM1_PRESCALER_64   PWMx_PRESCALER_64
#define PWM1_PRESCALER_256  PWMx_PRESCALER_256
#define PWM1_PRESCALER_1024 PWMx_PRESCALER_1024
#endif

#define PWM2_PRESCALER_1    1
#define PWM2_PRESCALER_8    2
#define PWM2_PRESCALER_32   3
#define PWM2_PRESCALER_64   4
#define PWM2_PRESCALER_128  5
#define PWM2_PRESCALER_256  6
#define PWM2_PRESCALER_1024 7

// on 32u4 timer4 have different prescaler and registers
#if defined __AVR_ATmega32U4__
#define PWM4_PRESCALER_1      1
#define PWM4_PRESCALER_2      2
#define PWM4_PRESCALER_4      3
#define PWM4_PRESCALER_8      4
#define PWM4_PRESCALER_16     5
#define PWM4_PRESCALER_32     6
#define PWM4_PRESCALER_64     7
#define PWM4_PRESCALER_128    8
#define PWM4_PRESCALER_256    9
#define PWM4_PRESCALER_512   10
#define PWM4_PRESCALER_1024  11
#define PWM4_PRESCALER_2048  12
#define PWM4_PRESCALER_4096  13
#define PWM4_PRESCALER_8192  14
#define PWM4_PRESCALER_16384 15
#endif

// return prescale constant for given timer and divider
// returns 0 if this prescale value doesn't exists for this timer
byte getPrescale(byte timer, int divider);

// compute nearest prescale and top value for given timer to achieve wanted frequency
// outputs rounded frequency
void computePWM(byte timer, unsigned long &frequency,
		word &prescale, word &top);

// "low level" version
extern bool setPWM(
		byte pwm, unsigned int icr,
		byte com_a, unsigned int ocr_a,
		byte com_b, unsigned int ocr_b,
		byte wgm, byte cs);

#ifndef __AVR_ATtinyX5__
// for those with OCRnC output
extern bool setPWM3(
		byte pwm, unsigned int icr,
		byte com_a, unsigned int ocr_a,
		byte com_b, unsigned int ocr_b,
		byte com_c, unsigned int ocr_c,
		byte wgm, byte cs);
#endif

extern void setPWMmode(
		byte pwm,
		byte com_a, byte com_b);

// Limited to Fast non-inverted mode, but with ICR, and with
// computing of ICR+CS for a given period
// TODO !
extern void setPWMsimple(
		byte pwm, unsigned long period,
		unsigned int dutyA, unsigned int dutyB);

#endif

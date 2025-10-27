#ifndef PWM_PWM_H
#define PWM_PWM_H 1

#include <Arduino.h>

#define TICKS_PER_MICRO (F_CPU / 1000000UL)
#define FREQUENCY_TO_PERIOD(f) (F_CPU/(f))
#define PERIOD_TO_FREQUENCY(f) (F_CPU/(f))

// Compare Output Mode values :
#define COMPARE_OUTPUT_MODE_NONE     0
#define COMPARE_OUTPUT_MODE_TOGGLE   1
#define COMPARE_OUTPUT_MODE_NORMAL   2
#define COMPARE_OUTPUT_MODE_INVERTED 3

// TODO : constants for WGM values : to be documented !

// WGM for "normal" 8 bits timers (see exceptions below)
#define WGM_NORMAL        0x00
#define WGM_PHASE_CORRECT 0x01
#define WGM_CTC           0x02
#define WGM_FAST          0x03

// to be ORed with WGM_*
#define WGM_TOP_FF        0x00
#define WGM_TOP_OCRA      0x04

// Specific for all 16 bits timers
#define WGM_16b_Normal                             0x00
#define WGM_16b_PWM_PHASE_CORRECT_0FF              0x01
#define WGM_16b_PWM_PHASE_CORRECT_1FF              0x02
#define WGM_16b_PWM_PHASE_CORRECT_3FF              0x03
#define WGM_16b_CTC_OCRxA                          0x04
#define WGM_16b_FAST_0FF                           0x05
#define WGM_16b_FAST_1FF                           0x06
#define WGM_16b_FAST_3FF                           0x07
#define WGM_16b_PWM_PHASE_FREQUENCY_CORRECT_ICRx   0x08
#define WGM_16b_PWM_PHASE_FREQUENCY_CORRECT_OCRxA  0x09
#define WGM_16b_PWM_PHASE_CORRECT_ICRx             0x0A
#define WGM_16b_PWM_PHASE_CORRECT_OCRxA            0x0B
#define WGM_16b_CTC_ICRx                           0x0C
#define WGM_16b_FAST_ICRx                          0x0E
#define WGM_16b_FAST_OCRxA                         0x0F

// specific timers
#if defined __AVR_ATmega32U4__
    #define WGM_4_FAST
    #define WGM_4_PHASE_FREQUENCY_CORRECT
    #define WGM_4_PWM6_SINGLE_SLOPE
    #define WGM_4_PWM6_DUAL_SLOPE
#endif

#if defined __AVR_ATtinyX5__
    #define SET_PWM_1_NONE  0
    #define SET_PWM_1_A     1
    #define SET_PWM_1_B     2
    #define SET_PWM_1_AB    3

    // to be ORed with SET_PWM_1_*
    #define WGM_1_FAST_MAX     0
    #define WGM_1_FAST_OCR1C   4

    // not PWM
    #define COMPARE_OUTPUT_MODE_CLEAR 2
    #define COMPARE_OUTPUT_MODE_SET   3
    // PWM mode : OC1x / !OC1x
    #define COMPARE_OUTPUT_MODE_1_NONE_NONE  0
    #define COMPARE_OUTPUT_MODE_1_CLEAR_SET  1
    #define COMPARE_OUTPUT_MODE_1_CLEAR_NONE 2
    #define COMPARE_OUTPUT_MODE_1_SET_NONE   3
#endif

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

    /** Micro / Leonardo / Yun / Fio */
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
    #define PWM_0_B 1
    #define PWM_1_A 1 
    #define PWM_NOT_1_A 0
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

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__)
    #define PWM2_PRESCALER_1    1
    #define PWM2_PRESCALER_8    2
    #define PWM2_PRESCALER_32   3
    #define PWM2_PRESCALER_64   4
    #define PWM2_PRESCALER_128  5
    #define PWM2_PRESCALER_256  6
    #define PWM2_PRESCALER_1024 7
#endif

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
void computePWMforFrequency(byte timer, unsigned long &frequency,
        byte &prescale, word &top);
// alias of previous function (for compatibility)
void computePWM(byte timer, unsigned long &frequency,
        byte &prescale, word &top);

// compute nearest prescale and top value for given timer to achieve wanted period in µs
void computePWMforPeriod(byte timer, unsigned long &period,
        byte &prescale, word &top);

// "low level" version
extern bool setPWM(
        byte timer, word icr,
        byte com_a, word ocr_a,
        byte com_b, word ocr_b,
        byte wgm, byte cs);

#ifndef __AVR_ATtinyX5__
// for those with OCRnC output
extern bool setPWM3(
        byte timer, word icr,
        byte com_a, word ocr_a,
        byte com_b, word ocr_b,
        byte com_c, word ocr_c,
        byte wgm, byte cs);
#endif

extern void setPWMmode(
        byte timer,
        byte com_a, byte com_b);

#endif // PWM_PWM_H

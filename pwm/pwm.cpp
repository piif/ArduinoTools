#include "pwm.h"

#include <avr/io.h>
#include <LowLevelTools.h>

word getMaxPrescale(byte timer) {
#if defined __AVR_ATmega32U4__
    if (timer == 4) {
        return 16384;
    }
#endif
#if defined __AVR_ATtinyX5__
    if (timer == 1) {
        return 16384;
    }
#endif
    return 1024;
}

byte getPrescale(byte timer, int divider) {
#if defined __AVR_ATmega32U4__
    if (timer == 4) {
        switch (divider) {
            case     1: return PWM4_PRESCALER_1;
            case     2: return PWM4_PRESCALER_2;
            case     4: return PWM4_PRESCALER_4;
            case     8: return PWM4_PRESCALER_8;
            case    16: return PWM4_PRESCALER_16;
            case    32: return PWM4_PRESCALER_32;
            case    64: return PWM4_PRESCALER_64;
            case   128: return PWM4_PRESCALER_128;
            case   256: return PWM4_PRESCALER_256;
            case   512: return PWM4_PRESCALER_512;
            case  1024: return PWM4_PRESCALER_1024;
            case  2048: return PWM4_PRESCALER_2048;
            case  4096: return PWM4_PRESCALER_4096;
            case  8192: return PWM4_PRESCALER_8192;
            case 16384: return PWM4_PRESCALER_16384;
            default   : return 0;
        }
    }
#endif
#if defined __AVR_ATtinyX5__
    if (timer == 1) {
        switch (divider) {
            case     1: return PWM1_PRESCALER_1;
            case     2: return PWM1_PRESCALER_2;
            case     4: return PWM1_PRESCALER_4;
            case     8: return PWM1_PRESCALER_8;
            case    16: return PWM1_PRESCALER_16;
            case    32: return PWM1_PRESCALER_32;
            case    64: return PWM1_PRESCALER_64;
            case   128: return PWM1_PRESCALER_128;
            case   256: return PWM1_PRESCALER_256;
            case   512: return PWM1_PRESCALER_512;
            case  1024: return PWM1_PRESCALER_1024;
            case  2048: return PWM1_PRESCALER_2048;
            case  4096: return PWM1_PRESCALER_4096;
            case  8192: return PWM1_PRESCALER_8192;
            case 16384: return PWM1_PRESCALER_16384;
            default   : return 0;
        }
    }
#endif
#if defined __AVR_ATmega328P__ || defined __AVR_ATmega2560__
    if (timer == 2) {
        switch (divider) {
            case    1: return PWM2_PRESCALER_1;
            case    8: return PWM2_PRESCALER_8;
            case   32: return PWM2_PRESCALER_32;
            case   64: return PWM2_PRESCALER_64;
            case  128: return PWM2_PRESCALER_128;
            case  256: return PWM2_PRESCALER_256;
            case 1024: return PWM2_PRESCALER_1024;
            default   : return 0;
        }
    }
#endif
    switch (divider) {
        case    1: return PWMx_PRESCALER_1;
        case    8: return PWMx_PRESCALER_8;
        case   64: return PWMx_PRESCALER_64;
        case  256: return PWMx_PRESCALER_256;
        case 1024: return PWMx_PRESCALER_1024;
        default   : return 0;
    }
}

unsigned long computePWMforTicks(byte timer, unsigned long ticks, byte &prescale, word &top);

// compute nearest prescale and top value for given timer to achieve wanted frequency
// outputs rounded frequency
void computePWMforFrequency(byte timer, unsigned long &frequency, byte &prescale, word &top) {
    unsigned long ticks = FREQUENCY_TO_PERIOD(frequency);
    unsigned long resultTicks = computePWMforTicks(timer, ticks, prescale, top);
    frequency = PERIOD_TO_FREQUENCY(resultTicks);
}

// alias of previous function (for compatibility)
void computePWM(byte timer, unsigned long &frequency, byte &prescale, word &top) {
    computePWMforFrequency(timer, frequency, prescale, top);
}

// compute nearest prescale and top value for given timer to achieve wanted period in µs
void computePWMforPeriod(byte timer, unsigned long &period, byte &prescale, word &top) {
    unsigned long ticks = period * TICKS_PER_MICRO;
    unsigned long resultTicks = computePWMforTicks(timer, ticks, prescale, top);
    period = resultTicks / TICKS_PER_MICRO;
}

// compute nearest prescale and top value for given timer to achieve wanted number of ticks
// return number of ticks applied
unsigned long computePWMforTicks(byte timer, unsigned long ticks, byte &prescale, word &top) {
    word timerMax =
#if defined __AVR_ATmega32U4__
        (timer == 1 || timer == 3) ? 65535 : ((timer == 4) ? 1023 : 255);
#elif defined __AVR_ATtinyX5__
        255;
#elif defined __AVR_ATmega2560__
        (timer == 0 || timer == 2) ? 255 : 65535;
#else
        (timer == 1) ? 65535 : 255;
#endif

    if (ticks <= timerMax) {
        prescale = getPrescale(timer, 1);
        top = ticks;
        return ticks;
    }
    // compute nearest prescale
    word maxDivider = getMaxPrescale(timer);
    prescale = 0;
    word divider = 1;
    do {
        ticks >>= 1;
        divider <<= 1;
        prescale = getPrescale(timer, divider);
    } while (divider < maxDivider && (prescale == 0 || ticks > timerMax));
    if (prescale == 0 || ticks > timerMax) {
        // not found, return max period possible
        prescale = getPrescale(timer, maxDivider);
        top = timerMax;
        return (unsigned long)timerMax * maxDivider;
    } else {
        top = ticks;
        return (unsigned long)top * divider;
    }
}

bool setPWM3(
        byte timer, word icr,
        byte com_a, word ocr_a,
        byte com_b, word ocr_b,
        byte com_c, word ocr_c,
        byte wgm, byte cs) {

    byte oldSREG = SREG;
    cli();

    switch(timer) {
    case 0:
        // TCCR0A : COM0A1 COM0A0 COM0B1 COM0B0 – – WGM01 WGM00
        // TCCR0B : FOC0A FOC0B – - WGM02 CS12 CS11 CS10
        TCCR0A = (com_a << COM0A0) | (com_b << COM0B0) | (wgm & 0x3);
        TCCR0B = (TCCR0B & 0xf0) | ((wgm >> 2) << WGM02) | (cs & 0x07);
        OCR0A = (byte)ocr_a;
        OCR0B = (byte)ocr_b;
#if defined(ICR0)
        ICR0 = icr;
#endif
        break;

    case 1:
#if defined __AVR_ATtinyX5__
        // TCCR1 : CTC1 PWM1A COM1A1 COM1A0 CS13 CS12 CS11 CS10
        // GTTCR : TSM PWM1B COM1B1 COM1B0 FOC1B FOC1A PSR1 PSR0
        TCCR1 = (wgm >> 1) << PWM1A | (com_a << COM1A0) | cs;
        GTTCR = (wgm & 1) << PWM1B | (com_b << COM1B0) | (GTTCR & 0x0f);
#else
        // TCCR1A : COM1A1 COM1A0 COM1B1 COM1B0 [COM1C1 COM1C0] WGM11 WGM10
        // TCCR1B : ICNC1 ICES1 – WGM13 WGM12 CS12 CS11 CS10
#if defined(COM1C0)
        TCCR1A = (com_a << COM1A0) | (com_b << COM1B0) | (com_c << COM1C0) | (wgm & 0x3);
#else
        TCCR1A = (com_a << COM1A0) | (com_b << COM1B0) | (wgm & 0x3);
#endif
        TCCR1B = (TCCR1B & 0xe0) | ((wgm >> 2) << WGM12) | cs;
#endif
        OCR1A = ocr_a;
        OCR1B = ocr_b;
#if defined(OCR1C)
        OCR1C = ocr_c;
#endif
#if defined(ICR1)
        ICR1 = icr;
#endif
        break;

#ifdef TCCR2A
    case 2:
        // TCCR2A : COM2A1 COM2A0 COM2B1 COM2B0 – – WGM21 WGM20
        // TCCR2B : FOC2A FOC2B – - WG22 CS22 CS21 CS20
        TCCR2A = (com_a << COM2A0) | (com_b << COM2B0) | (wgm & 0x3);
        TCCR2B = (TCCR2B & 0xf0) | ((wgm >> 2) << WGM22) | (cs & 0x07);
        OCR2A = (byte)ocr_a;
        OCR2B = (byte)ocr_b;
#if defined(ICR2)
        ICR2 = icr;
#endif
        break;
#endif

#ifdef TCCR3A
    case 3:
#if defined(COM3C0)
        TCCR3A = (com_a << COM3A0) | (com_b << COM3B0) | (com_c << COM3C0) | (wgm & 0x3);
#else
        TCCR3A = (com_a << COM3A0) | (com_b << COM3B0) | (wgm & 0x3);
#endif
        TCCR3B = (TCCR3B & 0xf0) | ((wgm >> 2) << WGM32) | (cs & 0x07);
        OCR3A = (byte)ocr_a;
        OCR3B = (byte)ocr_b;
#if defined(OCR3C)
        OCR3C = ocr_c;
#endif
#if defined(ICR3)
        ICR3 = icr;
#endif
        break;
#endif

#ifdef TCCR4A
    case 4:
#if defined __AVR_ATmega32U4__
        // TCCR4A : COM4A1 COM4A0 COM4B1 COM4B0 FOC4A FOC4B PWM4A PWM4B
        // TCCR4B : PWM4X PSR4 DTPS41 DTPS40 CS43 CS42 CS41 CS40
        TCCR4A = (com_a << COM4A0) | (com_b << COM4B0) | (com_c << COM4D0) | (wgm & 0x3);
        TCCR4B = (TCCR4B & 0xf0) | ((wgm >> 2) << WGM42) | (cs & 0x07);
#else
        // TCCR4A : COM4A1 COM4A0 COM4B1 COM4B0 COM4C1 COM4C0 WGM41 WGM40
        // TCCR4B : ICNC4 ICES4 – WGM43 WGM42 CS42 CS41 CS40
        TCCR4A = (com_a << COM4A0) | (com_b << COM4B0)  | (com_c << COM4C0)| (wgm & 0x3);
        TCCR4B = (TCCR4B & 0xe0) | ((wgm >> 2) << WGM43) | (cs & 0x07);
#endif
        OCR4A = (byte)ocr_a;
        OCR4B = (byte)ocr_b;
#if defined(OCR4C)
        OCR4C = ocr_c;
#endif
#if defined(ICR4)
        ICR4 = icr;
#endif
        break;
#endif

#ifdef TCCR5A
    case 5:
        TCCR5A = (com_a << COM5A0) | (com_b << COM5B0) | (wgm & 0x3);
        TCCR5B = (TCCR5B & 0xf0) | ((wgm >> 2) << WGM52) | (cs & 0x07);
        OCR5A = (byte)ocr_a;
        OCR5B = (byte)ocr_b;
#if defined(OCR5C)
        OCR5C = ocr_c;
#endif
#if defined(ICR5)
        ICR5 = icr;
#endif
        break;
#endif

    default:
        return false;
    }

    SREG = oldSREG;
    return true;
}

bool setPWM(byte timer, word icr,
        byte com_a, word ocr_a,
        byte com_b, word ocr_b,
        byte wgm, byte cs) {
    return setPWM3(timer,  icr,
                   com_a, ocr_a,
                   com_b, ocr_b,
                   0, 0,
                   wgm, cs);
}

void setPWMmode(
        byte timer,
        byte com_a, byte com_b) {

#if defined __AVR_ATtinyX5__
    if (timer == 1) {
        TCCR1_bits->_COM1A = com_a;
        TCCR1_bits->_PWM1A = (timer != 0);
    } else {
#endif
    byte oldSREG;
    TCCR_REG TCCR;

    switch(timer) {
    case 0:
        TCCR.registers.TCCRnA = TCCR0A;
        TCCR.registers.TCCRnB = TCCR0B;
        break;
#ifdef TCCR1A
    case 1:
        TCCR.registers.TCCRnA = TCCR1A;
        TCCR.registers.TCCRnB = TCCR1B;
        break;
#endif
#ifdef TCCR2A
    case 2:
        TCCR.registers.TCCRnA = TCCR2A;
        TCCR.registers.TCCRnB = TCCR2B;
        break;
#endif
#ifdef TCCR3A
    case 3:
        TCCR.registers.TCCRnA = TCCR3A;
        TCCR.registers.TCCRnB = TCCR3B;
        break;
#endif
#ifdef TCCR4A
    case 4:
        TCCR.registers.TCCRnA = TCCR4A;
        TCCR.registers.TCCRnB = TCCR4B;
        break;
#endif
#ifdef TCCR5A
    case 5:
        TCCR.registers.TCCRnA = TCCR5A;
        TCCR.registers.TCCRnB = TCCR5B;
        break;
#endif
    default:
        // unsupported => ignore
        // TODO : return -1 ?
        return;
    }

    TCCR.fields.COMnA = com_a; 
    TCCR.fields.COMnB = com_b;

    oldSREG = SREG;
    cli();
    switch(timer) {
    case 0:
        TCCR0A = TCCR.registers.TCCRnA;
        TCCR0B = TCCR.registers.TCCRnB;
        break;
#ifdef TCCR1A
    case 1:
        TCCR1A = TCCR.registers.TCCRnA;
        TCCR1B = TCCR.registers.TCCRnB;
        break;
#endif
#ifdef TCCR2A
    case 2:
        TCCR2A = TCCR.registers.TCCRnA;
        TCCR2B = TCCR.registers.TCCRnB;
        break;
#endif
#ifdef TCCR3A
    case 3:
        TCCR3A = TCCR.registers.TCCRnA;
        TCCR3B = TCCR.registers.TCCRnB;
        break;
#endif
#ifdef TCCR4A
    case 4:
        TCCR4A = TCCR.registers.TCCRnA;
        TCCR4B = TCCR.registers.TCCRnB;
        break;
#endif
#ifdef TCCR5A
    case 5:
        TCCR5A = TCCR.registers.TCCRnA;
        TCCR5B = TCCR.registers.TCCRnB;
        break;
#endif
    }
    SREG = oldSREG;
#if defined __AVR_ATtinyX5__
    }
#endif
}

void setPWMsimple(
        byte timer, unsigned long period,
        unsigned int dutyA, unsigned int dutyB) {
    // todo ...
}

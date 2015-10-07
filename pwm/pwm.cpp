#include "pwm.h"

#include <avr/io.h>
#include <ArduinoTools.h>

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
		}
	} else
#endif
	if (timer == 2) {
		switch (divider) {
		case    1: return PWM2_PRESCALER_1;
		case    8: return PWM2_PRESCALER_8;
		case   32: return PWM2_PRESCALER_32;
		case   64: return PWM2_PRESCALER_64;
		case  128: return PWM2_PRESCALER_128;
		case  256: return PWM2_PRESCALER_256;
		case 1024: return PWM2_PRESCALER_1024;
		}
	} else {
		switch (divider) {
		case 1   : return PWMx_PRESCALER_1;
		case 8   : return PWMx_PRESCALER_8;
		case 64  : return PWMx_PRESCALER_64;
		case 256 : return PWMx_PRESCALER_256;
		case 1024: return PWMx_PRESCALER_1024;
		}
	}
	return 0;
}

// compute nearest prescale and top value for given timer to achieve wanted frequency
// outputs rounded frequency
void computePWM(byte timer, unsigned long &frequency, word &prescale, word &top) {
	// compute ticks number
	unsigned long ticks = F_CPU / frequency;

#if defined __AVR_ATmega32U4__
	word timerMax = (timer == 1 || timer == 3) ? 65536 : 256;
#else
	word timerMax = (timer == 1) ? 65535 : 255;
#endif

	if (ticks <= timerMax) {
		prescale = getPrescale(timer, 1);
		top = ticks;
		return;
	}
	// compute nearest prescale
	prescale = 0;
	word p = 1;
	while (prescale == 0 || ticks > timerMax) {
		ticks >>= 1;
		p <<= 1;
		prescale = getPrescale(timer, p);
	}
	top = ticks;
	frequency = (p * ticks) / F_CPU;
}

/**
 * TCCR0A : COM0A1 COM0A0 COM0B1 COM0B0 – – WGM01 WGM00
 * TCCR0B : FOC0A FOC0B – – WGM02 CS02 CS01 CS00
 *
 * TCCR1A : COM1A1 COM1A0 COM1B1 COM1B0 – – WGM11 WGM10
 * TCCR1B : ICNC1 ICES1 – WGM13 WGM12 CS12 CS11 CS10
 *
 * TCCR2A : COM2A1 COM2A0 COM2B1 COM2B0 – – WGM21 WGM20
 * TCCR2B : FOC2A FOC2B – – WGM22 CS22 CS21 CS20
 *
 * FOC = Force Output Compare
 * WGM = Waveform Generation Mode
 * CS = clock select (prescale)
 * COM = Compare Output Mode
 *
**/

bool setPWM(byte pwm, unsigned int icr,
		byte com_a, unsigned int ocr_a,
		byte com_b, unsigned int ocr_b,
		byte wgm, byte cs) {

	byte oldSREG;
	TCCR_REG TCCR;
#if defined __AVR_ATmega32U4__
	TCCR4_REG TCCR4;
#endif

	switch(pwm) {
	case 0:
		READ_TCCR_REG(TCCR, 0);
		break;
	case 1:
		READ_TCCR_REG(TCCR, 1);
		break;
#ifdef TCCR2A
	case 2:
		READ_TCCR_REG(TCCR, 2);
		break;
#endif
#ifdef TCCR3A
	case 3:
		READ_TCCR_REG(TCCR, 3);
		break;
#endif
#ifdef TCCR4A
	case 4:
#if defined __AVR_ATmega32U4__
		READ_TCCR4_REG(TCCR4);
#else
		READ_TCCR_REG(TCCR, 4);
#endif
		break;
#endif
#ifdef TCCR5A
	case 5:
		READ_TCCR_REG(TCCR, 5);
		break;
#endif
	}

#if defined __AVR_ATmega32U4__
	if (pwm == 4) {
		TCCR4.fields.COMnA = com_a;
		TCCR4.fields.COMnB = com_b;
		SET_TCCR4_WGM(TCCR4, wgm);
		TCCR4.fields.CSn = cs;
	} else {
#endif
		TCCR.fields.COMnA = com_a;
		TCCR.fields.COMnB = com_b;
		SET_TCCR_WGM(TCCR, wgm);
		TCCR.fields.CSn = cs;
#if defined __AVR_ATmega32U4__
	}
#endif
	oldSREG = SREG;
	cli();
	switch(pwm) {
	case 0:
		WRITE_TCCR_REG(TCCR, 0);
		OCR0A = (byte)ocr_a;
		OCR0B = (byte)ocr_b;
		break;
	case 1:
		WRITE_TCCR_REG(TCCR, 1);
		// set OCR1x AFTER TCCR1x or high byte is forced to 0 !?!?
		ICR1 = icr;
		OCR1A = ocr_a;
		OCR1B = ocr_b;
		break;
#ifdef TCCR2A
	case 2:
		WRITE_TCCR_REG(TCCR, 2);
		OCR2A = (byte)ocr_a;
		OCR2B = (byte)ocr_b;
		break;
#endif
#ifdef TCCR3A
	case 3:
		WRITE_TCCR_REG(TCCR, 3);
		OCR3A = (byte)ocr_a;
		OCR3B = (byte)ocr_b;
		break;
#endif
#ifdef TCCR4A
	case 4:
#if defined __AVR_ATmega32U4__
		WRITE_TCCR4_REG(TCCR4);
#else
		WRITE_TCCR_REG(TCCR, 4);
#endif
		OCR4A = (byte)ocr_a;
		OCR4B = (byte)ocr_b;
		break;
#endif
#ifdef TCCR5A
	case 5:
		WRITE_TCCR_REG(TCCR, 5);
		OCR5A = (byte)ocr_a;
		OCR5B = (byte)ocr_b;
		break;
#endif
	default:
		return false;
	}
	SREG = oldSREG;
	return true;
}

// TODO : handle 32u4 Timer4 case
bool setPWM3(
		byte pwm, unsigned int icr,
		byte com_a, unsigned int ocr_a,
		byte com_b, unsigned int ocr_b,
		byte com_c, unsigned int ocr_c,
		byte wgm, byte cs) {

	byte oldSREG;
	TCCR_REG TCCR;

	switch(pwm) {
	case 0:
		READ_TCCR_REG(TCCR, 0);
		break;
	case 1:
		READ_TCCR_REG(TCCR, 1);
		break;
#ifdef TCCR2A
	case 2:
		READ_TCCR_REG(TCCR, 2);
		break;
#endif
#ifdef TCCR3A
	case 3:
		READ_TCCR_REG(TCCR, 3);
		break;
#endif
#ifdef TCCR4A
	case 4:
		READ_TCCR_REG(TCCR, 4);
		break;
#endif
#ifdef TCCR5A
	case 5:
		READ_TCCR_REG(TCCR, 5);
		break;
#endif
	}

	TCCR.fields.COMnA = com_a;
	TCCR.fields.COMnB = com_b;
	TCCR.fields.COMnC = com_c;
	SET_TCCR_WGM(TCCR, wgm);
	TCCR.fields.CSn = cs;

	oldSREG = SREG;
	cli();
	switch(pwm) {
#ifdef OCR1C
	case 1:
		WRITE_TCCR_REG(TCCR, 1);
		// set OCR1x AFTER TCCR1x or high byte is forced to 0 !?!?
		ICR1 = icr;
		OCR1A = ocr_a;
		OCR1B = ocr_b;
		OCR1C = ocr_c;
		break;
#endif
#ifdef OCR3C
	case 3:
		WRITE_TCCR_REG(TCCR, 3);
		OCR3A = (byte)ocr_a;
		OCR3B = (byte)ocr_b;
		OCR3C = (byte)ocr_c;
		break;
#endif
#ifdef OCR4C
	case 4:
		WRITE_TCCR_REG(TCCR, 4);
		OCR4A = (byte)ocr_a;
		OCR4B = (byte)ocr_b;
		OCR4C = (byte)ocr_c;
		break;
#endif
#ifdef OCR5C
	case 5:
		WRITE_TCCR_REG(TCCR, 5);
		OCR5A = (byte)ocr_a;
		OCR5B = (byte)ocr_b;
		OCR5C = (byte)ocr_c;
		break;
#endif
	default:
		return false;
	}
	SREG = oldSREG;
	return true;
}

void setPWMmode(
		byte pwm,
		byte com_a, byte com_b) {

	byte oldSREG;
	TCCR_REG TCCR;

	switch(pwm) {
	case 0:
		TCCR.registers.TCCRnA = TCCR0A;
		TCCR.registers.TCCRnB = TCCR0B;
		break;
	case 1:
		TCCR.registers.TCCRnA = TCCR1A;
		TCCR.registers.TCCRnB = TCCR1B;
		break;
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
	switch(pwm) {
	case 0:
		TCCR0A = TCCR.registers.TCCRnA;
		TCCR0B = TCCR.registers.TCCRnB;
		break;
	case 1:
		TCCR1A = TCCR.registers.TCCRnA;
		TCCR1B = TCCR.registers.TCCRnB;
		break;
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
}

void setPWMsimple(
		byte pwm, unsigned long period,
		unsigned int dutyA, unsigned int dutyB) {
	// todo ...
}

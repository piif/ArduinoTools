#include "pwm.h"

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


typedef union _tccr {
	unsigned int all;

	struct _registers {
		byte TCCRnB:8;
		byte TCCRnA:8;
	} registers;

	struct _fields {
		// ordered from low bits to high bits
		byte CSn:3;
		byte WGMnH:2;
		byte unused2:3;
		byte WGMnL:2;
		byte unused1:2;
		byte COMnB:2;
		byte COMnA:2;
	} fields;
} TCCR_REG;

void setPWM(byte pwm, unsigned int icr,
		byte com_a, unsigned int ocr_a,
		byte com_b, unsigned int ocr_b,
		byte wgm, byte cs) {

	byte oldSREG;
	TCCR_REG TCCR;

	switch(pwm) {
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
	default:
		TCCR.registers.TCCRnA = TCCR0A;
		TCCR.registers.TCCRnB = TCCR0B;
		break;
	}

	TCCR.fields.COMnA = com_a;
	TCCR.fields.COMnB = com_b;
	TCCR.fields.WGMnL = wgm & 0x3;
	TCCR.fields.WGMnH = wgm >> 2;
	TCCR.fields.CSn = cs;

	oldSREG = SREG;
	cli();
	switch(pwm) {
	case 1:
		TCCR1A = TCCR.registers.TCCRnA;
		TCCR1B = TCCR.registers.TCCRnB;
		// set OCR1x AFTER TCCR1x or high byte is forced to 0 !?!?
		ICR1 = icr;
		OCR1A = ocr_a;
		OCR1B = ocr_b;
		break;
#ifdef TCCR2A
	case 2:
		TCCR2A = TCCR.registers.TCCRnA;
		TCCR2B = TCCR.registers.TCCRnB;
		OCR2A = (byte)ocr_a;
		OCR2B = (byte)ocr_b;
		break;
#endif
	default:
		TCCR0A = TCCR.registers.TCCRnA;
		TCCR0B = TCCR.registers.TCCRnB;
		OCR0A = (byte)ocr_a;
		OCR0B = (byte)ocr_b;
		break;
	}
	SREG = oldSREG;
}

void setPWMmode(
		byte pwm,
		byte com_a, byte com_b) {

	byte oldSREG;
	TCCR_REG TCCR;

	switch(pwm) {
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
	default:
		TCCR.registers.TCCRnA = TCCR0A;
		TCCR.registers.TCCRnB = TCCR0B;
		break;
	}

	TCCR.fields.COMnA = com_a;
	TCCR.fields.COMnB = com_b;

	oldSREG = SREG;
	cli();
	switch(pwm) {
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
	default:
		TCCR0A = TCCR.registers.TCCRnA;
		TCCR0B = TCCR.registers.TCCRnB;
		break;
	}
	SREG = oldSREG;
}

void setPWMsimple(
		byte pwm, unsigned long period,
		unsigned int dutyA, unsigned int dutyB) {
	// todo ...
}

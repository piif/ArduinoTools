#include "pwm.h"

#include <avr/io.h>
#include <ArduinoTools.h>

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

void setPWM(byte pwm, unsigned int icr,
		byte com_a, unsigned int ocr_a,
		byte com_b, unsigned int ocr_b,
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
	SET_TCCR_WGM(TCCR, wgm);
	TCCR.fields.CSn = cs;

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
		WRITE_TCCR_REG(TCCR, 4);
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
	}
	SREG = oldSREG;
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

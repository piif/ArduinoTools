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
	case 2:
		TCCR.registers.TCCRnA = TCCR2A;
		TCCR.registers.TCCRnB = TCCR2B;
		break;
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
	case 2:
		TCCR2A = TCCR.registers.TCCRnA;
		TCCR2B = TCCR.registers.TCCRnB;
		OCR2A = (byte)ocr_a;
		OCR2B = (byte)ocr_b;
		break;
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
	case 2:
		TCCR.registers.TCCRnA = TCCR2A;
		TCCR.registers.TCCRnB = TCCR2B;
		break;
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
	case 2:
		TCCR2A = TCCR.registers.TCCRnA;
		TCCR2B = TCCR.registers.TCCRnB;
		break;
	default:
		TCCR0A = TCCR.registers.TCCRnA;
		TCCR0B = TCCR.registers.TCCRnB;
		break;
	}
	SREG = oldSREG;
}

// Limité au mode Fast non-inverted, mais en utilisant ICR, et en
// calculant ICR+CS pour une période donnée
void setPWMsimple(
		byte pwm, unsigned long period,
		unsigned int dutyA, unsigned int dutyB) {
	// todo ...
}


#ifdef OLD_VERSION

// specific to PWM 1 -> outputs 9 and 10

//	Serial.println("set bit " #set_reference ":" #set_position " from " #test_reference ":" #test_position);
#define SET_PWM_BIT(set_position, set_reference, test_position, test_reference) \
	if((test_reference) & (1<<(test_position))) { \
		set_##set_reference |= 1 << (set_position); \
	} else { \
		clr_##set_reference &= (byte)(~(1 << (set_position))); \
	}

void setPWM1(byte com1a, unsigned int ocr1a,
		byte com1b, unsigned int ocr1b,
		byte wgm, byte cs) {
	byte set_tccr1a = 0, clr_tccr1a = 0xff;
	byte set_tccr1b = 0, clr_tccr1b = 0xff;

	SET_PWM_BIT(COM1A0, tccr1a, 0, com1a)
	SET_PWM_BIT(COM1A1, tccr1a, 1, com1a)

	SET_PWM_BIT(COM1B0, tccr1a, 0, com1b)
	SET_PWM_BIT(COM1B1, tccr1a, 1, com1b)

	SET_PWM_BIT(WGM10, tccr1a, 0, wgm)
	SET_PWM_BIT(WGM11, tccr1a, 1, wgm)
	SET_PWM_BIT(WGM12, tccr1b, 2, wgm)
	SET_PWM_BIT(WGM13, tccr1b, 3, wgm)

	SET_PWM_BIT(CS10, tccr1b, 0, cs)
	SET_PWM_BIT(CS11, tccr1b, 1, cs)
	SET_PWM_BIT(CS12, tccr1b, 2, cs)

	TCCR1A = (TCCR1A | set_tccr1a) & clr_tccr1a;
	TCCR1B = (TCCR1B | set_tccr1b) & clr_tccr1b;

	// set OCR1x AFTER TCCR1x or high byte is forced to 0 !?!?
	OCR1A = ocr1a;
	OCR1B = ocr1b;

	Serial.print("TCCR1A : ");
	Serial.println(TCCR1A, BIN);

	Serial.print("TCCR1B : ");
	Serial.println(TCCR1B, BIN);

	Serial.print("OCR1A : ");
	Serial.println(OCR1A);

	Serial.print("OCR1B : ");
	Serial.println(OCR1B);
}
#endif

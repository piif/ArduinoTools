/**
 * some defines and methods to handle some low level features like
 * interrupts and timers
 */

#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>

/**
 * interrupt handler must be defined statically, because their table is
 * stored in flash memory
 * => to be able to add / remove handlers, we create an array and statically
 * define handlers pointing to them
 */

typedef void (*InterruptHandler)();

/**
 * Set given function as interrupt handler for given number, or remove existing one
 * if 0 is used as handler
 * (use macros defined is io.h)
 * Replace existing one if any
 * Returns old handler (0 is no handler)
 * TODO : add ability to chain several handlers ?
 */
InterruptHandler setInterruptHandler(short interruptNumber, InterruptHandler handler);

/**
 * structure to map TCCR* registers to word variables
 */
typedef union _tccr {
	word all;

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

#define READ_TCCR_REG(variable, num) \
	variable.registers.TCCRnA = TCCR ## num ## A;  \
	variable.registers.TCCRnB = TCCR ## num ## B

#define WRITE_TCCR_REG(variable, num) \
	TCCR ## num ## A = variable.registers.TCCRnA;  \
	TCCR ## num ## B = variable.registers.TCCRnB

#define SET_TCCR_WGM(variable, wgm) \
	variable.fields.WGMnL = (wgm) & 0x3;  \
	variable.fields.WGMnH = (wgm) >> 2


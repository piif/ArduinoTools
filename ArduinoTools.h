/**
 * some defines and methods to handle some low level features like
 * interrupts and timers
 */

#ifndef AVRTOOLS_H
#define AVRTOOLS_H 1

#include <Arduino.h>
#include <avr/io.h>

/**
 * interrupt handler must be defined statically, because their table is
 * stored in flash memory
 * => to be able to add / remove handlers, we create an array and statically
 * define handlers pointing to them
 */

typedef void (*InterruptHandler)();

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

/**
 * Set given function as interrupt handler for given interrupt type & number,
 * or remove existing one if 0 is used as handler
 * Replace existing one if any
 * Returns old handler (0 is no handler)
 */
#ifdef USE_INTERRUPT_INPUT_HANDLER
	extern const short _interrupts_first_input;
	#define setInputHandler(inputNumber, handler) \
		setInterruptHandler(_interrupts_first_input + inputNumber, handler)
#endif

#ifdef USE_INTERRUPT_TIMER_HANDLER
	extern const short _interrupts_first_timer;
	#define setTimerHandler(timerNumber, handler) \
		setInterruptHandler(_interrupts_first_timer + timerNumber, handler)
#endif

#ifdef USE_INTERRUPT_SERIAL_HANDLER
	extern const short _interrupts_first_serial;
	#define setSerialHandler(serialNumber, handler) \
		setInterruptHandler(_interrupts_first_serial + serialNumber, handler)
#endif

#ifdef USE_INTERRUPT_TWI_HANDLER
	extern const short _interrupts_first_twi;
	#define setTwiHandler(twiNumber, handler) \
		setInterruptHandler(_interrupts_first_twi + twiNumber, handler)
#endif

// underlying version
InterruptHandler setInterruptHandler(short interruptNumber, InterruptHandler handler);

#if defined(USE_INTERRUPT_INPUT_HANDLER) \
	|| defined(USE_INTERRUPT_TIMER_HANDLER) \
	|| defined(USE_INTERRUPT_SERIAL_HANDLER) \
	|| defined(USE_INTERRUPT_TWI_HANDLER)
	#include <ArduinoToolsInternals.h>
#endif

#endif

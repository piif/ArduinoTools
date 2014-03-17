/**
 * some defines and methods to handle some low level features like
 * interrupts and timers
 */

#ifndef AVRTOOLS_H
#define AVRTOOLS_H 1

#include <Arduino.h>
#include <avr/io.h>
#include <avr/sleep.h>

/**
 * default sleep mode to use
 */
#define DEFAULT_SLEEP_MODE SLEEP_MODE_IDLE

/**
 * The 5 different modes are:
 *     SLEEP_MODE_IDLE         -the least power savings
 *     SLEEP_MODE_ADC
 *     SLEEP_MODE_PWR_SAVE
 *     SLEEP_MODE_STANDBY
 *     SLEEP_MODE_PWR_DOWN     -the most power savings
 */
void sleepNow(word sleep_mode);
void sleepNow();

/**
 * interrupt handler must be defined statically, because their table is
 * stored in flash memory
 * => to be able to add / remove handlers, we create an array and statically
 * define handlers pointing to them
 */

typedef void (*InterruptHandler)(int data);

#if defined (__AVR_ATmega328P__)
	#define INTERRUPT_FOR_PIN(p) (((p) == 2) ? 0 : ((p) == 3) ? 1 : -1)
#elif defined (__AVR_ATmega2560__)
	#define INTERRUPT_FOR_PIN(p) (((p) == 2) ? 0 : \
			((p) == 3) ? 1 : \
			((p) == 21) ? 2 : \
			((p) == 20) ? 3 : \
			((p) == 19) ? 4 : \
			((p) == 18) ? 5 : -1)
#else
	#define INTERRUPT_FOR_PIN(p) (-1)
	#warning variant not implemented
#endif

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

#define TIMER_COMPARE_A OCIE0A
#define TIMER_COMPARE_B OCIE0B
#define TIMER_OVERFLOW  TOIE0

/**
 * Enable given interrupt by setting according registers
 * Returns true if required argument are implemented on current device
 */
// mode = LOW, CHANGE, RISING, FALLING
bool enableInputInterrupt(byte input, byte mode);
bool disableInputInterrupt(byte input);

// mode = TIMER_COMPARE_A, TIMER_COMPARE_B, TIMER_OVERFLOW
bool enableTimerInterrupt(byte timer, byte mode);
bool disableTimerInterrupt(byte timer, byte mode);

bool enableSerialInterrupt(byte serial);
bool disableSerialInterrupt(byte serial);

bool enableTwiInterrupt(byte twi);
bool disableTwiInterrupt(byte twi);

/**
 * Set given function as interrupt handler for given interrupt type & number,
 * or remove existing one if 0 is used as handler
 * Replace existing one if any
 * Returns old handler (0 is no handler)
 */
#ifdef USE_INTERRUPT_INPUT_HANDLER
	extern const short _interrupts_first_input;
	#define setInputHandler(inputNumber, handler) \
		setInterruptHandler(_interrupts_first_input + inputNumber, handler, inputNumber)
#endif

#ifdef USE_INTERRUPT_TIMER_HANDLER
	extern const short _interrupts_first_timer;
	#define setTimerHandler(timerNumber, handler) \
		setInterruptHandler(_interrupts_first_timer + timerNumber, handler, timerNumber)
#endif

#ifdef USE_INTERRUPT_SERIAL_HANDLER
	extern const short _interrupts_first_serial;
	#define setSerialHandler(serialNumber, handler) \
		setInterruptHandler(_interrupts_first_serial + serialNumber, handler, serialNumber)
#endif

#ifdef USE_INTERRUPT_TWI_HANDLER
	extern const short _interrupts_first_twi;
	#define setTwiHandler(twiNumber, handler) \
		setInterruptHandler(_interrupts_first_twi + twiNumber, handler, twiNumber)
#endif

// underlying function
InterruptHandler setInterruptHandler(short interruptNumber, InterruptHandler handler, int data);

#if defined(USE_INTERRUPT_INPUT_HANDLER) \
	|| defined(USE_INTERRUPT_TIMER_HANDLER) \
	|| defined(USE_INTERRUPT_SERIAL_HANDLER) \
	|| defined(USE_INTERRUPT_TWI_HANDLER)
	#include <ArduinoToolsInternals.h>
#endif

#endif

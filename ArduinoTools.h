/**
 * some defines and methods to handle some low level features like
 * interrupts and timers
 *
 * set*Handler functions are correctly implemented if some defines are set :
 * CALLER MUST DECIDE WHICH KIND OF HANDLERS HE WANTS, BY #DEFINING
 * ONE OR SEVERAL OF
 * - USE_INTERRUPT_TIMER_HANDLER_x (must match defaultTimer value)
 * - USE_INTERRUPT_INPUT_HANDLER_x
 * - USE_INTERRUPT_ANALAGCOMP_HANDLER
 * - USE_INTERRUPT_SERIAL_HANDLER_x
 * - USE_INTERRUPT_TWI_HANDLER_x
 * BEFORE THIS #INCLUSION
 *
 * If TIMER0 is used, it must override default one defined in wiring.c
 *  (used for millis(), delay() ..)
 * In this case, option -Wl,--allow-multiple-definition is needed or linking will fail.
 */

#ifndef AVRTOOLS_H
#define AVRTOOLS_H 1

#include <Arduino.h>
#include <avr/io.h>
#include <avr/sleep.h>

// for debug purpose
#ifdef ARDUINO_TOOLS_DEBUG
extern volatile int ISRnum, ISRcalled, ISRlast1, ISRlast2;
#endif

#ifdef ARDUINO_TOOLS_DEBUG
	#define LOG(str)  Serial.println(str); Serial.flush();
	#define LOGd(v) Serial.print(#v "\t: "); Serial.println(v); Serial.flush();
	#define LOGx(v) Serial.print(#v "\t: 0x"); Serial.println(v, HEX); Serial.flush();
	#define LOGb(v) Serial.print(#v "\t: 0b"); Serial.println(v, BIN); Serial.flush();
#else
	#define LOG(str)
	#define LOGd(v)
	#define LOGx(v)
	#define LOGb(v)
#endif

/**
 * How to know which kind of arduino we are compiling for :
 */
#if defined(__AVR_ATmega32U4__)
// it's a Leonardo, Micro or Yun
#elif defined(__AVR_ATmega328P__)
// it's a Uno
#elif defined(__AVR_ATmega2560__)
// it's a Mega
#else
// I don't know ...
#endif

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
#elif defined (__AVR_ATmega32U4__)
	#define INTERRUPT_FOR_PIN(p) (((p) == 0) ? 2 : \
			((p) == 1) ? 3 : \
			((p) == 2) ? 1 : \
			((p) == 3) ? 0 : \
			((p) == 7) ? 6 : -1)
#elif defined (__AVR_ATmega2560__)
	#define INTERRUPT_FOR_PIN(p) (((p) == 2) ? 0 : \
			((p) == 3) ? 1 : \
			((p) == 21) ? 2 : \
			((p) == 20) ? 3 : \
			((p) == 19) ? 4 : \
			((p) == 18) ? 5 : -1)
#elif defined (__AVR_ATmega32U4__)
	#define INTERRUPT_FOR_PIN(p) (((p) == 0) ? 2 : \
			((p) == 1) ? 3 : \
			((p) == 2) ? 1 : \
			((p) == 3) ? 0 : \
			((p) == 7) ? 6 : -1)
#else
	#define INTERRUPT_FOR_PIN(p) (-1)
	#warning variant not implemented
#endif

/**
 * bitWrite(dest, bit, value) is defined in arduino.h to fix value of bit "bit" into "dest"
 * bitWrite2,3,4 do the same with a 2,3,4 bits value, "bit" being lower one
 * Thus bitWrite4(x, 4, 0xA) with x==0 before implies x to became 0xA0
 */
#define bitWrite2(dest, bit, value) (dest) = (((dest) & ~(0x3UL << bit)) | (((value & 0x3UL)) << bit))
#define bitWrite3(dest, bit, value) (dest) = (((dest) & ~(0x7UL << bit)) | (((value & 0x7UL)) << bit))
#define bitWrite4(dest, bit, value) (dest) = (((dest) & ~(0xfUL << bit)) | (((value & 0xfUL)) << bit))


/**
 * structure to map TCCR* registers to word variables
 * usefull to change timer/pwm parameters
 */
typedef union _tccr {
	word all;

	struct _registers {
		byte TCCRnB:8;
		byte TCCRnA:8;
	} registers;

	struct _fields {
		// ordered from low bits to high bits
		byte CSn:3;   // Clock Select
		byte WGMnH:2; // Waveform Generation Mode High bits
		byte unused:3; // depends on timer
		byte WGMnL:2; // Waveform Generation Mode Low bits
		byte COMnC:2; // only for 1,3 on 32u4 + 1,3,4,5 on 2560
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

// TOIEx and OCIExx have same values for timers 0 to 5, excepted for leonardo family
// where timer4 is different, but *ableTimerInterrupt function handle this case
#define TIMER_OVERFLOW  TOIE0
#define TIMER_COMPARE_A OCIE0A
#define TIMER_COMPARE_B OCIE0B
#ifdef OCIE1C
#define TIMER_COMPARE_C OCIE1C
#endif
#ifdef OCIE4D
#define TIMER_COMPARE_D OCIE4D
#endif

/**
 * Enable given interrupt by setting according registers
 * Returns true if required argument are implemented on current device
 */
// mode = LOW, CHANGE, RISING, FALLING
bool enableInputInterrupt(byte input, byte mode);
// mode = TIMER_COMPARE_A, TIMER_COMPARE_B, TIMER_OVERFLOW
bool enableTimerInterrupt(byte timer, byte mode);
bool disableInputInterrupt(byte input);
// mode = CHANGE, RISING, FALLING
bool enableAnalogCompInterrupt(byte mode);
bool enableSerialInterrupt(byte serial);
bool enableTwiInterrupt(byte twi);

/**
 * Disable given interrupt by setting according registers
 * Returns true if required argument are implemented on current device
 */
bool disableTimerInterrupt(byte timer, byte mode);
// if param disableComparator is TRUE, comparator is disabled too
bool disableAnalogCompInterrupt(bool disableComparator);
bool disableAnalogCompInterrupt();
bool disableSerialInterrupt(byte serial);
bool disableTwiInterrupt(byte twi);

/**
 * Set given function as interrupt handler for given interrupt type & number,
 * or remove existing one if 0 is used as handler
 * Replace existing one if any
 * Returns old handler (0 if no handler)
 */
InterruptHandler setInputHandler(short inputNumber, InterruptHandler handler);
InterruptHandler setTimerHandler(short timerNumber, InterruptHandler handler);
InterruptHandler setAnalogCompHandler(InterruptHandler handler);
InterruptHandler setSerialHandler(short serialNumber, InterruptHandler handler);
InterruptHandler setTwiHandler(short twiNumber, InterruptHandler handler);

#if defined(__AVR_ATmega32U4__)
	// input only from ADC
	#define ANALOG_COMP_INPUT -1
	#define ANALOG_COMP_REFERENCE 7
#elif defined(__AVR_ATmega328P__)
	#define ANALOG_COMP_INPUT 7
	#define ANALOG_COMP_REFERENCE 6
#elif defined(__AVR_ATmega2560__)
	#define ANALOG_COMP_INPUT 5
	// ref only from internal
	#define ANALOG_COMP_REFERENCE -1
#else
#warning unknown hardware
#endif

#define analogCompValue() ((ACSR & (1 << ACO)) ? HIGH : LOW)

#define ANALOG_COMP_SOURCE_AIN1 255

/**
 * set analog comparator source to AIN1 pin (depends on hardware) or one of
 * ADC inputs (A0 to An, n <= 5 excepted for mega, <= 15)
 */
bool setAnalogCompSource(byte source);

#define ANALOG_COMP_REFERENCE_AIN0 0
#define ANALOG_COMP_REFERENCE_INTERNAL 1

/**
 * set analog comparator reference to AIN0 or internal reference
 */
bool setAnalogCompReference(byte ref);


/**
 * some defines to have information on number of interrupts kind are available on current target
 */
// INT0_vect & 1 are always defined
// INT2 to 7 depends on hardware
#if defined(INT7_vect)
	#define _INTERRUPTS_INPUT_TOTAL 8
#elif defined(INT6_vect)
	#define _INTERRUPTS_INPUT_TOTAL 7
#elif defined(INT5_vect)
	#define _INTERRUPTS_INPUT_TOTAL 6
#elif defined(INT4_vect)
	#define _INTERRUPTS_INPUT_TOTAL 5
#elif defined(INT3_vect)
	#define _INTERRUPTS_INPUT_TOTAL 4
#elif defined(INT2_vect)
	#define _INTERRUPTS_INPUT_TOTAL 3
#else
	#define _INTERRUPTS_INPUT_TOTAL 2
#endif

#if defined(TIMER5_OVF_vect)
	#define _INTERRUPTS_TIMER_TOTAL 6
#elif defined(TIMER4_OVF_vect)
	#define _INTERRUPTS_TIMER_TOTAL 5
#elif defined(TIMER3_OVF_vect)
	#define _INTERRUPTS_TIMER_TOTAL 4
#else
	#define _INTERRUPTS_TIMER_TOTAL 3
#endif

#define _INTERRUPTS_ANALOGCOMP_TOTAL 1
// not yet implemented.
#define _INTERRUPTS_SERIAL_TOTAL 0
#define _INTERRUPTS_TWI_TOTAL 0

// internal defines to help in selection of ISR() to define
#if defined(USE_INTERRUPT_INPUT_HANDLER_0) || \
	defined(USE_INTERRUPT_INPUT_HANDLER_1) || \
	defined(USE_INTERRUPT_INPUT_HANDLER_2) || \
	defined(USE_INTERRUPT_INPUT_HANDLER_3) || \
	defined(USE_INTERRUPT_INPUT_HANDLER_4) || \
	defined(USE_INTERRUPT_INPUT_HANDLER_5) || \
	defined(USE_INTERRUPT_INPUT_HANDLER_6) || \
	defined(USE_INTERRUPT_INPUT_HANDLER_7)

	#define USE_INTERRUPT_INPUT_HANDLER
#endif

#if defined(USE_INTERRUPT_TIMER_HANDLER_0) || \
	defined(USE_INTERRUPT_TIMER_HANDLER_1) || \
	defined(USE_INTERRUPT_TIMER_HANDLER_2) || \
	defined(USE_INTERRUPT_TIMER_HANDLER_3) || \
	defined(USE_INTERRUPT_TIMER_HANDLER_4) || \
	defined(USE_INTERRUPT_TIMER_HANDLER_5)

	#define USE_INTERRUPT_TIMER_HANDLER
#endif

#if defined(USE_INTERRUPT_SERIAL_HANDLER_0) || \
	defined(USE_INTERRUPT_SERIAL_HANDLER_1) || \
	defined(USE_INTERRUPT_SERIAL_HANDLER_2)

	#define USE_INTERRUPT_SERIAL_HANDLER
#endif

#if defined(USE_INTERRUPT_TWI_HANDLER_0) || \
	defined(USE_INTERRUPT_TWI_HANDLER_1)

	#define USE_INTERRUPT_TWI_HANDLER
#endif

#if !defined(NOT_IN_MAIN) && ( \
	defined(USE_INTERRUPT_INPUT_HANDLER) \
	|| defined(USE_INTERRUPT_TIMER_HANDLER) \
	|| defined(USE_INTERRUPT_ANALOGCOMP_HANDLER) \
	|| defined(USE_INTERRUPT_SERIAL_HANDLER) \
	|| defined(USE_INTERRUPT_TWI_HANDLER) )
	#include <ArduinoToolsInternals.h>
#endif

#endif

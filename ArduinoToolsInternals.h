#include <avr/interrupt.h>

extern InterruptHandler interruptHandler[];

#ifdef ARDUINO_TOOLS_DEBUG
	volatile int ISRnum = 0;
	volatile int ISRcalled = 0;
	volatile int ISRlast1 = -1;
	volatile int ISRlast2 = -1;
	#define _set_ISR(vect, n, data) ISR(vect) { \
		ISRnum++; \
		ISRlast2 = ISRlast1; \
		ISRlast1 = n; \
		if (interruptHandler[n]) { \
			(interruptHandler[n])(data); \
			ISRcalled++; \
		} \
	}
#else
	#define _set_ISR(vect, n, data) ISR(vect) { \
		if (interruptHandler[n]) (interruptHandler[n])(data); \
	}
#endif

// underlying function
InterruptHandler setInterruptHandler(short interruptNumber, InterruptHandler handler, int data);

#if defined(USE_INTERRUPT_INPUT_HANDLER)
	// enumerate input interrupts
	#define _INTERRUPTS_FIRST_INPUT 0

	#if defined(USE_INTERRUPT_INPUT_HANDLER_0)
		_set_ISR(INT0_vect, _INTERRUPTS_FIRST_INPUT+0, 0)
	#endif
	#if defined(USE_INTERRUPT_INPUT_HANDLER_1)
		_set_ISR(INT1_vect, _INTERRUPTS_FIRST_INPUT+1, 1)
	#endif
	#if defined(INT2_vect) && defined(USE_INTERRUPT_INPUT_HANDLER_2)
		_set_ISR(INT2_vect, _INTERRUPTS_FIRST_INPUT+2, 2)
	#endif
	#if defined(INT3_vect) && defined(USE_INTERRUPT_INPUT_HANDLER_3)
		_set_ISR(INT3_vect, _INTERRUPTS_FIRST_INPUT+3, 3)
	#endif
	#if defined(INT4_vect) && defined(USE_INTERRUPT_INPUT_HANDLER_4)
		_set_ISR(INT4_vect, _INTERRUPTS_FIRST_INPUT+4, 4)
	#endif
	#if defined(INT5_vect) && defined(USE_INTERRUPT_INPUT_HANDLER_5)
		_set_ISR(INT5_vect, _INTERRUPTS_FIRST_INPUT+5, 5)
	#endif
	#if defined(INT6_vect) && defined(USE_INTERRUPT_INPUT_HANDLER_6)
		_set_ISR(INT6_vect, _INTERRUPTS_FIRST_INPUT+6, 6)
	#endif
	#if defined(INT7_vect) && defined(USE_INTERRUPT_INPUT_HANDLER_7)
		_set_ISR(INT7_vect, _INTERRUPTS_FIRST_INPUT+7, 7)
	#endif

	#define __DELTA_1 _INTERRUPTS_FIRST_INPUT + _INTERRUPTS_INPUT_TOTAL

	InterruptHandler setInputHandler(short inputNumber, InterruptHandler handler) {
		return setInterruptHandler(_INTERRUPTS_FIRST_INPUT + inputNumber, handler, inputNumber);
	}
#else
	#define __DELTA_1 0

	InterruptHandler setInputHandler(short inputNumber, InterruptHandler handler) {
		// TODO : how to specify "unavailable"
		return 0;
	}
#endif
#if defined(USE_INTERRUPT_TIMER_HANDLER)
	#define _INTERRUPTS_FIRST_TIMER __DELTA_1

	#if defined(USE_INTERRUPT_TIMER_HANDLER_0)
		_set_ISR(TIMER0_OVF_vect, _INTERRUPTS_FIRST_TIMER+0, 0)
	#endif
	#if defined(USE_INTERRUPT_TIMER_HANDLER_1)
		_set_ISR(TIMER1_OVF_vect, _INTERRUPTS_FIRST_TIMER+1, 1)
	#endif
	#if defined(USE_INTERRUPT_TIMER_HANDLER_2)
		#if !defined(__AVR_ATmega32U4__)
		_set_ISR(TIMER2_OVF_vect, _INTERRUPTS_FIRST_TIMER+2, 2)
		#else
			#error Cannot use Timer2 on 32u4 processor
		#endif
	#endif
	#if defined(TIMER3_OVF_vect) && defined(USE_INTERRUPT_TIMER_HANDLER_2)
		#if defined(TCCR3A)
		_set_ISR(TIMER3_OVF_vect, _INTERRUPTS_FIRST_TIMER+3, 3)
		#else
			#error Cannot use Timer3
		#endif
	#endif
	#if defined(TIMER4_OVF_vect) && defined(USE_INTERRUPT_TIMER_HANDLER_4)
		#if defined(TCCR4A)
		_set_ISR(TIMER4_OVF_vect, _INTERRUPTS_FIRST_TIMER+4, 4)
		#else
			#error Cannot use Timer4
		#endif
	#endif
	#if defined(TIMER5_OVF_vect) && defined(USE_INTERRUPT_TIMER_HANDLER_5)
		#if defined(TCCR5A)
		_set_ISR(TIMER5_OVF_vect, _INTERRUPTS_FIRST_TIMER+5, 5)
		#else
			#error Cannot use Timer5
		#endif
	#endif

	#define __DELTA_2 _INTERRUPTS_FIRST_TIMER + _INTERRUPTS_TIMER_TOTAL

	InterruptHandler setTimerHandler(short timerNumber, InterruptHandler handler) {
		return setInterruptHandler(_INTERRUPTS_FIRST_TIMER + timerNumber, handler, timerNumber);
	}
#else
	#define __DELTA_2 __DELTA_1
	InterruptHandler setTimerHandler(short timerNumber, InterruptHandler handler) {
		// TODO : how to specify "unavailable"
		return 0;
	}
#endif


#if defined(USE_INTERRUPT_ANALOGCOMP_HANDLER)
	#define _INTERRUPTS_FIRST_ANALOGCOMP __DELTA_2

	#ifdef ANA_COMP_vect
		_set_ISR(ANA_COMP_vect, _INTERRUPTS_FIRST_ANALOGCOMP, 0)
	#else
		_set_ISR(ANALOG_COMP_vect, _INTERRUPTS_FIRST_ANALOGCOMP, 0)
	#endif

	#define __DELTA_3 _INTERRUPTS_FIRST_ANALOGCOMP + 1

	InterruptHandler setAnalogCompHandler(InterruptHandler handler) {
		return setInterruptHandler(_INTERRUPTS_FIRST_ANALOGCOMP, handler, 0);
	}
#else
	#define __DELTA_3 __DELTA_2

	InterruptHandler setAnalogCompHandler(InterruptHandler handler) {
		// TODO : how to specify "unavailable"
		return 0;
	}
#endif


#if defined(USE_INTERRUPT_SERIAL_HANDLER)

	#define _INTERRUPTS_FIRST_SERIAL __DELTA_3
	#error NOT YET IMPLEMENTED
	#define _INTERRUPTS_SERIAL_TOTAL 0

	#define __DELTA_4 _INTERRUPTS_FIRST_SERIAL + _INTERRUPTS_SERIAL_TOTAL

	InterruptHandler setSerialHandler(short serialNumber, InterruptHandler handler) {
		return setInterruptHandler(_INTERRUPTS_FIRST_SERIAL + serialNumber, handler, serialNumber);
	}
#else
	#define __DELTA_4 __DELTA_3

	InterruptHandler setSerialHandler(short serialNumber, InterruptHandler handler) {
		// TODO : how to specify "unavailable"
		return 0;
	}
#endif



#if defined(USE_INTERRUPT_TWI_HANDLER)

	#define _INTERRUPTS_FIRST_TWI __DELTA_4
	#error NOT YET IMPLEMENTED
	#define _INTERRUPTS_TWI_TOTAL 0

	#define __DELTA_5 _INTERRUPTS_FIRST_TWI + _INTERRUPTS_TWI_TOTAL

	InterruptHandler setTwiHandler(short twiNumber, InterruptHandler handler) {
		return setInterruptHandler(_INTERRUPTS_FIRST_TWI + twiNumber, handler, twiNumber);
	}
#else
	#define __DELTA_5 __DELTA_4

	InterruptHandler setTwiHandler(short twiNumber, InterruptHandler handler) {
		// TODO : how to specify "unavailable"
		return 0;
	}
#endif

#define _INTERRUPTS_TOTAL __DELTA_5

// now, we know how many handlers we must store
InterruptHandler interruptHandler[_INTERRUPTS_TOTAL] = { 0, };
int interruptData[_INTERRUPTS_TOTAL];


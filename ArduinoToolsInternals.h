#include <avr/interrupt.h>

extern InterruptHandler interruptHandler[];

#define _set_ISR(vect, n) ISR(vect) { \
	if (interruptHandler[n]) (interruptHandler[n])(); \
}

#ifdef USE_INTERRUPT_INPUT_HANDLER

	// enumerate input interrupts
	#define _INTERRUPTS_FIRST_INPUT 0
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

	_set_ISR(INT0_vect, _INTERRUPTS_FIRST_INPUT+0)
	_set_ISR(INT1_vect, _INTERRUPTS_FIRST_INPUT+1)
	#ifdef INT2_vect
		_set_ISR(INT2_vect, _INTERRUPTS_FIRST_INPUT+2)
	#endif
	#ifdef INT3_vect
		_set_ISR(INT3_vect, _INTERRUPTS_FIRST_INPUT+3)
	#endif
	#ifdef INT4_vect
		_set_ISR(INT4_vect, _INTERRUPTS_FIRST_INPUT+4)
	#endif
	#ifdef INT5_vect
		_set_ISR(INT5_vect, _INTERRUPTS_FIRST_INPUT+5)
	#endif
	#ifdef INT6_vect
		_set_ISR(INT6_vect, _INTERRUPTS_FIRST_INPUT+6)
	#endif
	#ifdef INT7_vect
		_set_ISR(INT7_vect, _INTERRUPTS_FIRST_INPUT+7)
	#endif

	const short _interrupts_first_input = _INTERRUPTS_FIRST_INPUT;

	#define __DELTA_1 _INTERRUPTS_FIRST_INPUT + _INTERRUPTS_INPUT_TOTAL
#else
	#define __DELTA_1 0
#endif

#ifdef USE_INTERRUPT_TIMER_HANDLER
	#define _INTERRUPTS_FIRST_TIMER __DELTA_1

	#if defined(TIMER5_OVF_vect)
		#define _INTERRUPTS_TIMER_TOTAL 6
	#elif defined(TIMER4_OVF_vect)
		#define _INTERRUPTS_TIMER_TOTAL 5
	#elif defined(TIMER3_OVF_vect)
		#define _INTERRUPTS_TIMER_TOTAL 4
	#else
		#define _INTERRUPTS_TIMER_TOTAL 3
	#endif

	_set_ISR(TIMER0_OVF_vect, _INTERRUPTS_FIRST_TIMER+0)
	_set_ISR(TIMER1_OVF_vect, _INTERRUPTS_FIRST_TIMER+1)
	_set_ISR(TIMER2_OVF_vect, _INTERRUPTS_FIRST_TIMER+2)
	#ifdef TIMER3_OVF_vect
		_set_ISR(TIMER3_OVF_vect, _INTERRUPTS_FIRST_TIMER+3)
	#endif
	#ifdef TIMER4_OVF_vect
		_set_ISR(TIMER4_OVF_vect, _INTERRUPTS_FIRST_TIMER+4)
	#endif
	#ifdef TIMER5_OVF_vect
		_set_ISR(TIMER5_OVF_vect, _INTERRUPTS_FIRST_TIMER+5)
	#endif

	const short _interrupts_first_timer = _INTERRUPTS_FIRST_TIMER;

	#define __DELTA_2 _INTERRUPTS_FIRST_TIMER + _INTERRUPTS_TIMER_TOTAL
#else
	#define __DELTA_2 __DELTA_1
#endif



#ifdef USE_INTERRUPT_SERIAL_HANDLER
	#define _INTERRUPTS_FIRST_SERIAL __DELTA_2
	#error NOT YET IMPLEMENTED
	#define _INTERRUPTS_SERIAL_TOTAL 0

	const short _interrupts_first_serial = _INTERRUPTS_FIRST_SERIAL;

	#define __DELTA_3 _INTERRUPTS_FIRST_SERIAL + _INTERRUPTS_SERIAL_TOTAL
#else
	#define __DELTA_3 __DELTA_2
#endif



#ifdef USE_INTERRUPT_TWI_HANDLER
	#define _INTERRUPTS_FIRST_TWI __DELTA_3
	#error NOT YET IMPLEMENTED
	#define _INTERRUPTS_TWI_TOTAL 0

	const short _interrupts_first_twi = _INTERRUPTS_FIRST_TWI;

	#define __DELTA_4 _INTERRUPTS_FIRST_TWI + _INTERRUPTS_TWI_TOTAL
#else
	#define __DELTA_4 __DELTA_3
#endif

#define _INTERRUPTS_TOTAL __DELTA_4

// now, we know how many handlers we must store
InterruptHandler interruptHandler[_INTERRUPTS_TOTAL] = { 0, };


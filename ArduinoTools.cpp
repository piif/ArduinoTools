#define NOT_IN_MAIN
#include "ArduinoTools.h"

#ifndef _VECTOR_SIZE
#define NB_INTERRUPT _VECTORS_SIZE / 4
#else
#define NB_INTERRUPT _VECTORS_SIZE / _VECTOR_SIZE
#endif

extern InterruptHandler interruptHandler[];
extern int interruptData[];

bool enableInputInterrupt(byte input, byte mode) {
	// some copy/paste from WInterrupts.cpp
	switch (input) {
	case 0:
	#if defined(EICRA) && defined(ISC00) && defined(EIMSK)
		EICRA = (EICRA & ~((1 << ISC00) | (1 << ISC01))) | (mode << ISC00);
		EIMSK |= (1 << INT0);
	#elif defined(MCUCR) && defined(ISC00) && defined(GICR)
		MCUCR = (MCUCR & ~((1 << ISC00) | (1 << ISC01))) | (mode << ISC00);
		GICR |= (1 << INT0);
	#elif defined(MCUCR) && defined(ISC00) && defined(GIMSK)
		MCUCR = (MCUCR & ~((1 << ISC00) | (1 << ISC01))) | (mode << ISC00);
		GIMSK |= (1 << INT0);
	#else
		#error attachInterrupt not finished for this CPU (case 0)
	#endif
	return true;

	case 1:
	#if defined(EICRA) && defined(ISC10) && defined(ISC11) && defined(EIMSK)
		EICRA = (EICRA & ~((1 << ISC10) | (1 << ISC11))) | (mode << ISC10);
		EIMSK |= (1 << INT1);
	#elif defined(MCUCR) && defined(ISC10) && defined(ISC11) && defined(GICR)
		MCUCR = (MCUCR & ~((1 << ISC10) | (1 << ISC11))) | (mode << ISC10);
		GICR |= (1 << INT1);
	#elif defined(MCUCR) && defined(ISC10) && defined(GIMSK) && defined(GIMSK)
		MCUCR = (MCUCR & ~((1 << ISC10) | (1 << ISC11))) | (mode << ISC10);
		GIMSK |= (1 << INT1);
	#else
		#warning attachInterrupt may need some more work for this cpu (case 1)
	#endif
	return true;

	case 2:
	#if defined(EICRA) && defined(ISC20) && defined(ISC21) && defined(EIMSK)
		EICRA = (EICRA & ~((1 << ISC20) | (1 << ISC21))) | (mode << ISC20);
		EIMSK |= (1 << INT2);
	#elif defined(MCUCR) && defined(ISC20) && defined(ISC21) && defined(GICR)
		MCUCR = (MCUCR & ~((1 << ISC20) | (1 << ISC21))) | (mode << ISC20);
		GICR |= (1 << INT2);
	#elif defined(MCUCR) && defined(ISC20) && defined(GIMSK) && defined(GIMSK)
		MCUCR = (MCUCR & ~((1 << ISC20) | (1 << ISC21))) | (mode << ISC20);
		GIMSK |= (1 << INT2);
	#endif
	return true;

#if defined(EICRA) && defined(EIMSK)
#if defined(ISC30) && defined(INT3)
	case 3:
		EICRA = (EICRA & ~((1<<ISC30) | (1<<ISC31))) | (mode << ISC30);
		EIMSK |= (1<<INT3);
	return true;
#endif
#if defined(ISC40) && defined(INT4)
	case 4:
		EICRB = (EICRB & ~((1 << ISC40) | (1 << ISC41))) | (mode << ISC40);
		EIMSK |= (1 << INT4);
	return true;
#endif
#if defined(ISC50) && defined(INT5)
	case 5:
		EICRB = (EICRB & ~((1 << ISC50) | (1 << ISC51))) | (mode << ISC50);
		EIMSK |= (1 << INT5);
	return true;
#endif
#if defined(ISC60) && defined(INT6)
	case 6:
		EICRB = (EICRB & ~((1 << ISC60) | (1 << ISC61))) | (mode << ISC60);
		EIMSK |= (1 << INT6);
	return true;
#endif
#if defined(ISC70) && defined(INT7)
	case 7:
		EICRB = (EICRB & ~((1 << ISC70) | (1 << ISC71))) | (mode << ISC70);
		EIMSK |= (1 << INT7);
	return true;
#endif
#endif
	}

	return false;
}
bool disableInputInterrupt(byte input) {
	// some copy/paste from WInterrupts.cpp
	switch (input) {
#if defined(INT0)
	case 0:
	#if defined(EIMSK)
		EIMSK &= ~(1 << INT0);
	#elif defined(GICR)
		GICR &= ~(1 << INT0); // atmega32
	#elif defined(GIMSK)
		GIMSK &= ~(1 << INT0);
	#else
	#error detachInterrupt not finished for this cpu
	#endif
	return true;
#endif

#if defined(INT1)
	case 1:
	#if defined(EIMSK)
		EIMSK &= ~(1 << INT1);
	#elif defined(GICR)
		GICR &= ~(1 << INT1); // atmega32
	#elif defined(GIMSK)
		GIMSK &= ~(1 << INT1);
	#else
	#warning detachInterrupt may need some more work for this cpu (case 1)
	#endif
	return true;
#endif

#if defined(EIMSK) && defined(INT2)
	case 2:
		EIMSK &= ~(1 << INT2);
	return true;
#endif
#if defined(EIMSK) && defined(INT3)
	case 3:
		EIMSK &= ~(1 << INT3);
	return true;
#endif
#if defined(EIMSK) && defined(INT4)
	case 4:
		EIMSK &= ~(1 << INT4);
	return true;
#endif
#if defined(EIMSK) && defined(INT5)
	case 5:
		EIMSK &= ~(1 << INT5);
	return true;
#endif
#if defined(EIMSK) && defined(INT6)
	case 6:
		EIMSK &= ~(1 << INT6);
	return true;
#endif
#if defined(EIMSK) && defined(INT7)
	case 7:
		EIMSK &= ~(1 << INT7);
	return true;
#endif

	}
	return false;
}

bool enableTimerInterrupt(byte timer, byte mode) {
#if defined(__AVR_ATmega32U4__)
	if (timer == 2 || timer > 4) {
		return false;
	}
	if (mode == TIMER_COMPARE_D && timer != 4) {
		return false;
	}
	if (mode == TIMER_COMPARE_C && timer != 1 &&  timer != 3) {
		return false;
	}
#if defined(__AVR_ATmega2560__)
	if (timer > 5) {
		return false;
	}
	if (mode == TIMER_COMPARE_C && timer != 1 &&  timer != 3 && timer != 4 && timer != 5) {
		return false;
	}
	if (mode == TIMER_COMPARE_D) {
		return false;
	}
#else
	if (timer > 2) {
		return false;
	}
	if (mode == TIMER_COMPARE_C || mode == TIMER_COMPARE_D) {
		return false;
	}
#endif
	switch(timer) {
	case 0:
		TIMSK0 |= (1<<mode);
		return true;
	case 1:
		TIMSK1 |= (1<<mode);
		return true;
#ifdef TIMSK2
	case 2:
		TIMSK2 |= (1<<mode);
		return true;
#endif
#ifdef TIMSK3
	case 3:
		TIMSK3 |= (1<<mode);
		return true;
#endif
#ifdef TIMSK4
	case 4:
		// TODO : if mega32u, mode bits are differents
#if defined(__AVR_ATmega32U4__)
		switch(mode) {
		case TIMER_COMPARE_A:
			TIMSK4 |= (1<<OCIE4A);
		break;
		case TIMER_COMPARE_B:
			TIMSK4 |= (1<<OCIE4B);
		break;
		case TIMER_COMPARE_C:
			return false;
		case TIMER_COMPARE_D:
			TIMSK4 |= (1<<OCIE4D);
		break;
		case TIMER_OVERFLOW:
			TIMSK4 |= (1<<TOIE4);
		break;
		}
#else
		TIMSK4 |= (1<<mode);
#endif
		return true;
#endif
#ifdef TIMSK5
	case 5:
		TIMSK5 |= (1<<mode);
		return true;
#endif
	}
	return false;
}
bool disableTimerInterrupt(byte timer, byte mode) {
	switch(timer) {
	case 0:
		TIMSK0 &= ~(1<<mode);
		return true;
	case 1:
		TIMSK1 &= ~(1<<mode);
		return true;
	case 2:
		TIMSK2 &= ~(1<<mode);
		return true;
#ifdef TCCR3A
	case 3:
		TIMSK3 &= ~(1<<mode);
		return true;
#endif
#ifdef TCCR4A
	case 4:
		TIMSK4 &= ~(1<<mode);
		return true;
#endif
#ifdef TCCR5A
	case 5:
		TIMSK5 &= ~(1<<mode);
		return true;
#endif
	}
	return false;
}


bool setAnalogCompSource(byte source) {
	if (source == ANALOG_COMP_SOURCE_AIN1) {
		// disable mux
		ADCSRB &= ~(1<<ACME);
	} else {
#ifdef __AVR_ATmega2560__
		// set mux
		ADMUX &= ~(0x7);
		ADMUX |= source & 0x7;
		ADCSRB &= ~(1<<MUX5);
		if (source & 0x8) {
			ADCSRB |= 1<<MUX5;
		}
#elif defined(__AVR_ATmega328P)
		if (source > 5) {
			return false;
		}
		ADMUX &= ~(0x7);
		ADMUX |= source & 0x7;
#elif defined(__AVR_ATmega32U4__)
		byte mux;
		switch(source) {
		case 0:
			mux = 7;
			break;
		case 1:
			mux = 6;
			break;
		case 2:
			mux = 5;
			break;
		case 3:
			mux = 4;
			break;
		case 4:
			mux = 1;
			break;
		case 5:
			mux = 0;
			break;
		default:
			return false;
		}
#endif
		// enable mux + disable adc
		ADCSRB |= 1<<ACME;
		ADCSRA &= ~(1<<ADEN);
	}
	return true;
}

bool setAnalogCompReference(byte ref) {
	if (ref == ANALOG_COMP_REFERENCE_INTERNAL) {
		ACSR &= ~(1<<ACBG);
	} else {
#ifdef __AVR_ATmega2560__
		// no AIN1 pin accessible on Mega
		return false;
#else
		ACSR |= 1<<ACBG;
#endif
	}
	return true;
}

bool enableAnalogCompInterrupt(byte mode) {
	byte modeBits = 0;

	// first clear interrupt flag to change mode without side effects, and other used ones
	// to just have to set usefull ones
	ACSR &= ~((1 << ACIE) || (1 << ACIS1) || (1 << ACIS0));
	// compute ACISx bits
	switch (mode) {
	case FALLING:
		modeBits |= 1 << ACIS1;
		break;
	case RISING:
		modeBits |= (1 << ACIS1) | (1 << ACIS0);
		break;
	// CHANGE => let to 0
	}
	ACSR |= modeBits;
	// finally, set AC Interrupt Enable and enable comparator
	ACSR &= ~(1 << ACD);
	ACSR |= (1 << ACIE);
	return true;
}
bool disableAnalogCompInterrupt() {
	return disableAnalogCompInterrupt(false);
}
bool disableAnalogCompInterrupt(bool disableComparator) {
	// clear AC Interrupt Enable and disable comparator
	ACSR &= ~(1 << ACIE);
	if (disableComparator) {
		ACSR |= 1 << ACD;
	}
	return true;
}

bool enableSerialInterrupt(byte serial) {
	return false;
}
bool disableSerialInterrupt(byte serial) {
	return false;
}

bool enableTwiInterrupt(byte twi) {
	return false;
}

bool disableTwiInterrupt(byte twi) {
	return false;
}

InterruptHandler setInterruptHandler(short interruptNumber, InterruptHandler handler, int data) {
	InterruptHandler before = interruptHandler[interruptNumber];
	interruptHandler[interruptNumber] = handler;
	interruptData[interruptNumber] = data;
	return before;
}

/**
 * copy/paste from http://www.arduino.cc/playground/Learning/ArduinoSleepCode
 */
void sleepNow(word sleep_mode) {
	// TODO : how to avoid race condition where "wake up interruption"
	// raised between start of this method and call to  sleep_mode() ?
    /*
     * The 5 different modes are:
     *     SLEEP_MODE_IDLE         -the least power savings
     *     SLEEP_MODE_ADC
     *     SLEEP_MODE_PWR_SAVE
     *     SLEEP_MODE_STANDBY
     *     SLEEP_MODE_PWR_DOWN     -the most power savings
     */
    set_sleep_mode(sleep_mode);   // sleep mode is set here
    sleep_enable(); // then, it's enabled

    sleep_mode(); // here, we really enter in sleep mode
    // Here an external event wake up the CPU
    sleep_disable(); // disable sleep mode
}

void sleepNow() {
	sleepNow(DEFAULT_SLEEP_MODE);
}

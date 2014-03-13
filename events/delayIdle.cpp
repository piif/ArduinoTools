
#define NOT_IN_MAIN
#include "delayIdle.h"

#include <avr/sleep.h>
#include <pwm/pwm.h>

// TODO : one variable per timer !!!
volatile long delayLoops = -1;
volatile bool cancelFlag = false;

// for sleep modes debug
int innerloops = 0;

void delayIdleISR() {
	delayLoops--;
}

bool delayIdleWith(long microseconds, byte timer, word sleep_mode) {
	bool result = true;

	InterruptHandler before = setTimerHandler(timer, delayIdleISR);

	// compute ticks number
	long ticks = F_CPU / 1000000L * microseconds;
	// compute better prescale
	byte prescale;
	if (timer == 2) {
		if (ticks % 1024 == 0) {
			ticks /= 1024;
			prescale = PWM2_PRESCALER_1024;
		} else if (ticks % 256 == 0) {
			ticks /= 256;
			prescale = PWM2_PRESCALER_256;
		} else if (ticks % 128 == 0) {
			ticks /= 128;
			prescale = PWM2_PRESCALER_128;
		} else if (ticks % 64 == 0) {
			ticks /= 64;
			prescale = PWM2_PRESCALER_64;
		} else if (ticks % 32 == 0) {
			ticks /= 32;
			prescale = PWM2_PRESCALER_32;
		} else if (ticks % 8 == 0) {
			ticks /= 8;
			prescale = PWM2_PRESCALER_8;
		} else {
			prescale = PWM2_PRESCALER_1;
		}
	} else {
		// all other timers have the same prescalers
		if (ticks % 1024 == 0) {
			ticks /= 1024;
			prescale = PWM1_PRESCALER_1024;
		} else if (ticks % 256 == 0) {
			ticks /= 256;
			prescale = PWM1_PRESCALER_256;
		} else if (ticks % 64 == 0) {
			ticks /= 64;
			prescale = PWM1_PRESCALER_64;
		} else if (ticks % 8 == 0) {
			ticks /= 8;
			prescale = PWM1_PRESCALER_8;
		} else {
			prescale = PWM1_PRESCALER_1;
		}
	}

	long timerSize = (timer == 1) ? 65536 : 256;
	delayLoops = ticks / timerSize;
	int remainder;
	if (delayLoops % timerSize == 0) {
		remainder = 0;
		delayLoops--;
	} else {
		remainder = timerSize - ticks % timerSize;
	}
//	Serial.print("delayIdle :");
//	Serial.println(ticks);
//	Serial.println(delayLoops);
//	Serial.println(remainder);
//	Serial.flush();

	// compute cycle total, loop number, prescale/mode
	setPWM(timer, 0, /* ICR ? */
		COMPARE_OUTPUT_MODE_NONE, 0,
		COMPARE_OUTPUT_MODE_NONE, 0,
		(timer == 1) ? WGM_1_FAST_MAX : WGM_0_FAST_MAX, prescale);


	// listen for counter overflow interrupt
	innerloops = 0;
	switch(timer) {
	case 0:
		TIMSK0 |= (1<<TOIE0);
		TCNT0 = remainder;
		break;
	case 1:
		TIMSK1 |= (1<<TOIE1);
		TCNT1 = remainder;
		break;
	case 2:
		TIMSK2 |= (1<<TOIE2);
		TCNT2 = remainder;
		break;
#ifdef TCCR3A
	case 3:
		TIMSK3 |= (1<<TOIE3);
		TCNT3 = remainder;
		break;
#endif
#ifdef TCCR4A
	case 4:
		TIMSK4 |= (1<<TOIE4);
		TCNT4 = remainder;
		break;
#endif
#ifdef TCCR5A
	case 5:
		TIMSK5 |= (1<<TOIE5);
		TCNT5 = remainder;
		break;
#endif
	}

	// wait for enough timer loops
	while(delayLoops > 0) {
		innerloops++;
		sleepNow(sleep_mode);
		if (cancelFlag) {
			cancelFlag = false;
			result = false;
			break;
		}
	}
	// stop to listen to interrupt
	switch(timer) {
	case 0:
		TIMSK0 &= ~(1<<TOIE0);
		TCNT0 = remainder;
		break;
	case 1:
		TIMSK1 &= ~(1<<TOIE1);
		TCNT1 = remainder;
		break;
	case 2:
		TIMSK2 &= ~(1<<TOIE2);
		TCNT2 = remainder;
		break;
#ifdef TCCR3A
	case 3:
		TIMSK3 &= ~(1<<TOIE3);
		TCNT3 = remainder;
		break;
#endif
#ifdef TCCR4A
	case 4:
		TIMSK4 &= ~(1<<TOIE4);
		TCNT4 = remainder;
		break;
#endif
#ifdef TCCR5A
	case 5:
		TIMSK5 &= ~(1<<TOIE5);
		TCNT5 = remainder;
		break;
#endif
	}

	setTimerHandler(timer, before);

	return result;
}

bool delayIdleWith(long microseconds, byte timer) {
	return delayIdleWith(microseconds, timer, DEFAULT_SLEEP_MODE);
}
bool delayIdle(long microseconds, word sleep_mode) {
	return delayIdleWith(microseconds, DEFAULT_DELAY_TIMER, sleep_mode);
}
bool delayIdle(long microseconds) {
	return delayIdleWith(microseconds, DEFAULT_DELAY_TIMER, DEFAULT_SLEEP_MODE);
}

void delayCancel() {
	cancelFlag = true;
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

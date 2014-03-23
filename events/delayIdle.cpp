
#include "delayIdle.h"

#include <pwm/pwm.h>

// TODO : one variable per timer !!!
volatile long delayLoops = -1;
volatile bool cancelFlag = false;

// for sleep modes debug
int innerloops = 0;

void delayIdleISR(int data) {
	delayLoops--;
}

bool delayIdleWith(unsigned long microseconds, byte timer, word sleep_mode) {
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
		TCNT0 = remainder;
		break;
	case 1:
		TCNT1 = remainder;
		break;
	case 2:
		TCNT2 = remainder;
		break;
#ifdef TCCR3A
	case 3:
		TCNT3 = remainder;
		break;
#endif
#ifdef TCCR4A
	case 4:
		TCNT4 = remainder;
		break;
#endif
#ifdef TCCR5A
	case 5:
		TCNT5 = remainder;
		break;
#endif
	}
	enableTimerInterrupt(timer, TIMER_OVERFLOW);

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
	disableTimerInterrupt(timer, TIMER_OVERFLOW);
	setTimerHandler(timer, before);

	return result;
}

bool delayIdleWith(unsigned long microseconds, byte timer) {
	return delayIdleWith(microseconds, timer, DEFAULT_SLEEP_MODE);
}
bool delayIdle(unsigned long microseconds, word sleep_mode) {
	return delayIdleWith(microseconds, DEFAULT_DELAY_TIMER, sleep_mode);
}
bool delayIdle(unsigned long microseconds) {
	return delayIdleWith(microseconds, DEFAULT_DELAY_TIMER, DEFAULT_SLEEP_MODE);
}

void delayCancel() {
	cancelFlag = true;
}

#define NOT_IN_MAIN
#include "delayIdle.h"

#include <pwm/pwm.h>

volatile long delayLoops[_INTERRUPTS_TIMER_TOTAL] = { -1, };
volatile bool cancelFlag[_INTERRUPTS_TIMER_TOTAL] = { false, };

// for sleep modes debug
int innerloops = 0;

void delayIdleISR(int data) {
	delayLoops[data]--;
}

bool delayIdleWith(unsigned long microseconds, byte timer, word sleep_mode, bool cancelOnInterrupt) {
	bool result = true;

	InterruptHandler before = setTimerHandler(timer, delayIdleISR);

	// compute ticks number
	long ticks = F_CPU / 1000000L * microseconds;
	// compute better prescale
	byte prescale;
	if (timer == 2) {
		if (ticks % 1024 == 0) { // TODO : does compiler generates & 1023 == 0 ?
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
#if defined __AVR_ATmega32U4__
	} else if (timer == 4) {
		if (ticks % 16384 == 0) {
			ticks /= 16384;
			prescale = PWM4_PRESCALER_16384;
		} else if (ticks % 8192 == 0) {
			ticks /= 8192;
			prescale = PWM4_PRESCALER_8192;
		} else if (ticks % 4096 == 0) {
			ticks /= 4096;
			prescale = PWM4_PRESCALER_4096;
		} else if (ticks % 2048 == 0) {
			ticks /= 2048;
			prescale = PWM4_PRESCALER_2048;
		} else if (ticks % 1024 == 0) {
			ticks /= 1024;
			prescale = PWM4_PRESCALER_1024;
		} else if (ticks % 512 == 0) {
			ticks /= 512;
			prescale = PWM4_PRESCALER_512;
		} else if (ticks % 256 == 0) {
			ticks /= 256;
			prescale = PWM4_PRESCALER_256;
		} else if (ticks % 128 == 0) {
			ticks /= 128;
			prescale = PWM4_PRESCALER_128;
		} else if (ticks % 64 == 0) {
			ticks /= 64;
			prescale = PWM4_PRESCALER_64;
		} else if (ticks % 32 == 0) {
			ticks /= 32;
			prescale = PWM4_PRESCALER_32;
		} else if (ticks % 16 == 0) {
			ticks /= 16;
			prescale = PWM4_PRESCALER_16;
		} else if (ticks % 8 == 0) {
			ticks /= 8;
			prescale = PWM4_PRESCALER_8;
		} else if (ticks % 4 == 0) {
			ticks /= 4;
			prescale = PWM4_PRESCALER_4;
		} else if (ticks % 2 == 0) {
			ticks /= 2;
			prescale = PWM4_PRESCALER_2;
		} else {
			prescale = PWM2_PRESCALER_1;
		}
#endif
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
	delayLoops[timer] = ticks / timerSize;
	int remainder;
	if (ticks % timerSize == 0) {
		remainder = 0;
	} else {
		remainder = timerSize - ticks % timerSize;
		delayLoops[timer]++;
	}
//	Serial.print("delayIdle : ");
//	Serial.print(prescale);
//	Serial.print(" / ");
//	Serial.println(ticks);
//	Serial.flush();
//	delay(400);

	// compute cycle total, loop number, prescale/mode
	setPWM(timer, 0, /* ICR ? */
		COMPARE_OUTPUT_MODE_NONE, 0,
		COMPARE_OUTPUT_MODE_NONE, 0,
		WGM_NORMAL, prescale);

	// listen for counter overflow interrupt
	innerloops = 0;
	switch(timer) {
	case 0:
		TCNT0 = remainder;
		break;
	case 1:
		TCNT1 = remainder;
		break;
#ifdef TCCR2A
	case 2:
		TCNT2 = remainder;
		break;
#endif
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

	long delayLoopsBefore;

	// wait for enough timer loops
	while(delayLoops[timer] > 0) {
		innerloops++;
		delayLoopsBefore = delayLoops[timer];
		sleepNow(sleep_mode);
		if ((cancelOnInterrupt && delayLoopsBefore == delayLoops[timer]) || cancelFlag[timer]) {
			cancelFlag[timer] = false;
			result = false;
			break;
		}
	}
	// stop to listen to interrupt
	disableTimerInterrupt(timer, TIMER_OVERFLOW);
	setTimerHandler(timer, before);

	return result;
}

bool delayIdle(unsigned long microseconds) {
	return delayIdleWith(microseconds, DEFAULT_DELAY_TIMER, DEFAULT_SLEEP_MODE, false);
}
bool delayOrInterruptIdle(unsigned long microseconds) {
	return delayIdleWith(microseconds, DEFAULT_DELAY_TIMER, DEFAULT_SLEEP_MODE, true);
}

bool delayIdleWith(unsigned long microseconds, byte timer) {
	return delayIdleWith(microseconds, timer, DEFAULT_SLEEP_MODE, false);
}
bool delayOrInterruptIdleWith(unsigned long microseconds, byte timer) {
	return delayIdleWith(microseconds, timer, DEFAULT_SLEEP_MODE, true);
}

bool delayIdle(unsigned long microseconds, word sleep_mode) {
	return delayIdleWith(microseconds, DEFAULT_DELAY_TIMER, sleep_mode, false);
}
bool delayOrInterruptIdle(unsigned long microseconds, word sleep_mode) {
	return delayIdleWith(microseconds, DEFAULT_DELAY_TIMER, sleep_mode, true);
}

bool delayIdleWith(unsigned long microseconds, byte timer, word sleep_mode) {
	return delayIdleWith(microseconds, DEFAULT_DELAY_TIMER, sleep_mode, false);
}
bool delayOrInterruptIdleWith(unsigned long microseconds, byte timer, word sleep_mode) {
	return delayIdleWith(microseconds, DEFAULT_DELAY_TIMER, sleep_mode, true);
}

void delayCancel(byte timer) {
	cancelFlag[timer] = true;
}

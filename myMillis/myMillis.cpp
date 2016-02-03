#include "myMillis/myMillis.h"
volatile unsigned long timer0_millis;
volatile unsigned long clock_ticks;
volatile unsigned long clock_ticks_per_ovf = 0;

ISR(TIMER0_OVF_vect) {
	// overflow => clock did clock_ticks_per_ovf cycles since last one
	clock_ticks += clock_ticks_per_ovf;

	// if enough clock ticks elapsed
	if (clock_ticks > CLOCK_TICKS_PER_MILLI) {
		// update millis counter
		timer0_millis += clock_ticks / CLOCK_TICKS_PER_MILLI;
		// and set clock counter to remaining ticks
		clock_ticks = clock_ticks % CLOCK_TICKS_PER_MILLI;
	}
}

short initTimerMillis(byte wgm, byte top, byte prescale) {
	byte timer0_prescale_value;
	byte timer0_prescale_index;
	word timer0_top;

	// stop timer0
	TCCR0B &= ~((1<<CS02) | (1<<CS01) | (1<<CS00));

	switch(wgm) {
	case 0:
	case 3:
		timer0_top = 0xFF;
		break;
	case 1:
		timer0_top = 0x1FF;
		break;
	case 2:
	case 7:
		timer0_top = top;
		break;
	case 5:
		timer0_top = top * 2;
		break;
	default:
		return -1;
	}

	switch(prescale) {
	case 1:
		timer0_prescale_value = 1;
		timer0_prescale_index = 1;
		break;
	case 8:
		timer0_prescale_value = 8;
		timer0_prescale_index = 2;
		break;
	case 64:
		timer0_prescale_value = 64;
		timer0_prescale_index = 3;
		break;
	case 256:
		timer0_prescale_value = 256;
		timer0_prescale_index = 4;
		break;
	case 1024:
		timer0_prescale_value = 1024;
		timer0_prescale_index = 5;
		break;
	default:
		return -1;
	}

	timer0_millis = 0;
	clock_ticks = 0;

	clock_ticks_per_ovf = timer0_top * timer0_prescale_value;

	// prepare timer0
	OCR0A = top;
	TCNT0 = 0;

	bitWrite(TCCR0A, WGM00, wgm & 1);
	bitWrite(TCCR0A, WGM01, wgm & 2);
	bitWrite(TCCR0B, WGM02, wgm & 4);

	// relaunch it
	TCCR0B |= timer0_prescale_index << CS00;

//	Serial.print("TCCR0A : ");Serial.println(TCCR0A, HEX);
//	Serial.print("TCCR0B : ");Serial.println(TCCR0B, HEX);
//	Serial.print("clock_ticks_per_ovf : "); Serial.println(clock_ticks_per_ovf);

	// and activate interrupt
#if defined(TIMSK0)
	bitSet(TIMSK0, TOIE0);
#elif defined(TIMSK)
	bitSet(TIMSK, TOIE0);
#else
#error unsupported architecture
#endif

	return 0;
}

unsigned long myMillis() {
	unsigned long m;
	uint8_t oldSREG = SREG;

	// disable interrupts while we read timer0_millis or we might get an
	// inconsistent value (e.g. in the middle of a write to timer0_millis)
	cli();
	m = timer0_millis;
	SREG = oldSREG;

	return m;
}

void myDelay(unsigned long m) {
	unsigned long start = myMillis();

	unsigned long target = start + m;
//	Serial.print("myDelay ");Serial.println(start);
//	Serial.print(" -> ");Serial.println(target);

	if (target < start) { // means will cycle soon => wait this cycle before
		while (myMillis() >= start) {
			// sleep idle ?
		}
	}
	while (myMillis() < target) {
		// sleep idle ?
	}
}

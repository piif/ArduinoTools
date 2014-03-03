#include "a574g.h"

// TODO : WORKS ONLY FOR ATmega328P, ADAPT FOR MEGA !!!

#include <Arduino.h>

#define A574_NB_DIGITS 4

#ifdef INSIDE
// display pluged with digits "inside" arduino
/* display selection pins (+) */
#define D0 2
#define D1 5
#define D2 6
#define D3 8

/* segment selection pins (-) */
#define St 3
#define SL 4
#define SR 7
#define Sm 9
#define Sl 13
#define Sr 10
#define Sb 12
#define Sp 11
#else
// display pluged with digits "outside" arduino
/* display selection pins (+) */
#define D0 13
#define D1 10
#define D2  9
#define D3  7

/* segment selection pins (-) */
#define St 12
#define SL 11
#define SR  8
#define Sm  6
#define Sl  2
#define Sr  5
#define Sb  3
#define Sp  4
#endif

/*
 mapping example INSIDE
    lbprm3R21Lt0
    11011010011000
1 = 11001000011100 = 12828
3 = 10000000110000 = 8240
5 = 10000011000000 = 8384
7 = 11001100010000 = 13072
 OUTSIDE
    0tL12R3mrpbl
    01100100111100
  1101110101111111
111101100101111111
*/

// all segments bits to 1 and display bits to 0
#define ALL_OFF ( (word) ( \
	1<<St | 1<<SL | 1<<SR | 1<<Sm |  \
	1<<Sl | 1<<Sr | 1<<Sb | 1<<Sp )  )

// all position bits to 0, other bits to 1 => can be and'ed with segment mask to force
// to display at a given position
#define POSITION_MASK ( (word) ~(1<<D0 | 1<<D1 | 1<<D2 | 1<<D3) )

// current segments to light on display
volatile byte _toDisplay[A574_NB_DIGITS];
// segments lighted on before a roll effect was launched
volatile byte _toDisplayBefore[A574_NB_DIGITS];

// current mask to send to display
volatile word toSend[A574_NB_DIGITS];

A574g::A574g() : Led7() {
	nbDigits = A574_NB_DIGITS;

	this->toDisplay = _toDisplay;
	this->toDisplayBefore = _toDisplayBefore;

	// call super() ?
	pinMode(D0, OUTPUT);
	pinMode(D1, OUTPUT);
	pinMode(D2, OUTPUT);
	pinMode(D3, OUTPUT);
	pinMode(St, OUTPUT);
	pinMode(SL, OUTPUT);
	pinMode(SR, OUTPUT);
	pinMode(Sm, OUTPUT);
	pinMode(Sl, OUTPUT);
	pinMode(Sr, OUTPUT);
	pinMode(Sb, OUTPUT);
	pinMode(Sp, OUTPUT);
}

word prepareForSegment(byte s) {
	// start with all segments off (HIGH), digit bit off (LOW),
	// other bits = 0
	int res = ALL_OFF;
	// then light on (LOW) segments ON in mapChars
	if (s & (1 << 7)) res ^= 1 << St;
	if (s & (1 << 6)) res ^= 1 << SL;
	if (s & (1 << 5)) res ^= 1 << SR;
	if (s & (1 << 4)) res ^= 1 << Sm;
	if (s & (1 << 3)) res ^= 1 << Sl;
	if (s & (1 << 2)) res ^= 1 << Sr;
	if (s & (1 << 1)) res ^= 1 << Sb;
	if (s & (1 << 0)) res ^= 1 << Sp;
	return res;
}

/**
 * modify given segment map to force it's display position
 */
word prepareForPos(word map, byte pos) {
	switch(pos) {
	case 0: return (map & POSITION_MASK) | (1<<D0);
	case 1: return (map & POSITION_MASK) | (1<<D1);
	case 2: return (map & POSITION_MASK) | (1<<D2);
	case 3: return (map & POSITION_MASK) | (1<<D3);
	}
	return POSITION_MASK;
}

void A574g::prepareDisplay(byte pos, byte segments) {
	toSend[pos] = prepareForPos((segments == 0) ? ALL_OFF : prepareForSegment(segments), pos);
}
void A574g::updateDisplay() {
	// nothing. job will be done by send()
}
volatile short digit = 0;

void doSend(word w) {
	PORTB = (PORTB & 0xC0) | (w >> 8);
	PORTD = (PORTD & 0x02) |  w;
}

void A574g::send() {
	Led7::send();
	doSend(toSend[digit]);
	digit = (digit + 1) % A574_NB_DIGITS;
}

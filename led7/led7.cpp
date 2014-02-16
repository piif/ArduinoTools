#include "led7.h"

// TODO : WORKS ONLY FOR ATmega328P, ADAPT FOR MEGA !!!

#include <Arduino.h>

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

const char *mapChars = LED7_CHARS;

// segment map for each of the characters listed in mapChars
byte mapSegments[] = {
	//tLRmlrbp
	0b11101110, // 0
	0b00100100, // 1
	0b10111010, // 2
	0b10110110, // 3
	0b01110100, // 4
	0b11010110, // 5
	0b11011110, // 6
	0b10100100, // 7
	0b11111110, // 8
	0b11110110, // 9
	0b00000001, // .
	0b00000000, // ' '
	0b11111100, // a
	0b01011110, // b
	0b11001010, // c
	0b00111110, // d
	0b11011010, // e
	0b11011000, // f
	0b11001110, // g
	0b01011100, // h
	0b00000100, // i
	0b00100110, // j
	0b01011010, // k
	0b01001010, // l
	0b11101100, // m
	0b00011100, // n
	0b00011110, // o
	0b11111000, // p
	0b11110100, // q
	0b00011000, // r
	0b11000110, // s
	0b01011000, // t
	0b00001110, // u
	0b01101110, // v
	0b01111110, // w
	0b01111100, // x
	0b01110110, // y
	0b10101010, // z
	0b10111000, // ?
	0b00010000, // -
	0b00000010, // _
	0b01000000, // '
};

// current segments to light on display
volatile byte toDisplay[LED7_NB_DIGITS];
// segments lighted on before a roll effect was launched
volatile byte toDisplayBefore[LED7_NB_DIGITS];

// current mask to send to display
volatile word toSend[LED7_NB_DIGITS];

void Led7::setup() {
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

byte getSegments(char c) {
	int i = 0;
	const char *ptr = mapChars;
	// no upper letters in the map => convert them to lower.
	if (c >= 'A' && c <= 'Z') {
		c = c - 'A' + 'a';
	}
	while (*ptr != '\0') {
		if (*ptr == c) {
			return mapSegments[i];
		}
		i++;
		ptr++;
	}
	return 0;
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


int Led7::rollSpeed = 500;

volatile short rollingStep = 0;
volatile int rollingCount;

/**
 * Prepare for rolling display.
 * Next calls to send() will handle rolling steps.
 */
void launchRoll() {
	rollingStep = 4;
	rollingCount = 0;
}

/**
 * Cancel running roll.
 * In fact, only next call to send() will update display
 */
void cancelRoll() {
	rollingStep = 0;
	rollingCount = 0;
}

/**
 * shift displayed chars on the left, and set last displayed char to "c"
 */
void Led7::shift(char c) {
	cancelRoll();
	for(short i = 0; i < LED7_NB_DIGITS - 1; i++) {
		toDisplayBefore[i] = toDisplay[i];
		toDisplay[i] = toDisplay[i + 1];
		toSend[i] = prepareForPos(toSend[i + 1], i);
	}
	toDisplay[LED7_NB_DIGITS - 1] = getSegments(c);
	toSend[LED7_NB_DIGITS - 1] = prepareForPos(prepareForSegment(toDisplay[LED7_NB_DIGITS - 1]), LED7_NB_DIGITS - 1);
}

void Led7::display(unsigned int value) {
	cancelRoll();
	for (short i = LED7_NB_DIGITS - 1; i >= 0; i--) {
		toDisplayBefore[i] = toDisplay[i];
		if (value == 0) {
			toDisplay[i] = (i == LED7_NB_DIGITS - 1) ? getSegments('0') : 0;
		} else {
			toDisplay[i] = getSegments('0' + (value % 10));
			value /= 10;
		}
		toSend[i] = prepareForPos(prepareForSegment(toDisplay[i]), i);
	}
}

void Led7::display(const char *value) {
	cancelRoll();
	short i;
	for (i = 0; value[i] != '\0' && i < LED7_NB_DIGITS; i++) {
		toDisplayBefore[i] = toDisplay[i];
		toDisplay[i] = getSegments(value[i]);
		toSend[i] = prepareForPos(prepareForSegment(toDisplay[i]), i);
	}
	while (i < LED7_NB_DIGITS) {
		toDisplayBefore[i] = toDisplay[i];
		toDisplay[i] = 0;
		toSend[i] = prepareForPos(ALL_OFF, i);
		i++;
	}
}

void Led7::display(const char *value, int len) {
	cancelRoll();
	short i;
	if (len > LED7_NB_DIGITS) {
		len = LED7_NB_DIGITS;
	}
	for (i = 0; i < len; i++) {
		toDisplayBefore[i] = toDisplay[i];
		toDisplay[i] = getSegments(value[i]);
		toSend[i] = prepareForPos(prepareForSegment(toDisplay[i]), i);
	}
	while (i < LED7_NB_DIGITS) {
		toDisplayBefore[i] = toDisplay[i];
		toDisplay[i] = 0;
		toSend[i] = prepareForPos(ALL_OFF, i);
		i++;
	}
}

void Led7::display(String value) {
	cancelRoll();
	for (unsigned short i = 0; i < LED7_NB_DIGITS; i++) {
		toDisplayBefore[i] = toDisplay[i];
		toDisplay[i] = (i >= value.length()) ? 0 : getSegments(value[i]);
		toSend[i] = prepareForPos(prepareForSegment(toDisplay[i]), i);
	}
}

void Led7::roll(unsigned int value) {
	display(value);
	launchRoll();
}

void Led7::roll(const char *value) {
	display(value);
	launchRoll();
}

void Led7::roll(const char *value, int len) {
	display(value, len);
	launchRoll();
}

void Led7::roll(String value) {
	display(value);
	launchRoll();
}

volatile short digit = 0;

void doSend(word w) {
	PORTB = (PORTB & 0xC0) | (w >> 8);
	PORTD = (PORTD & 0x02) |  w;
}

void Led7::send() {
	// look if toSend must be modified by a running roll effect
	if (rollingStep > 0) {
		if (rollingCount == 0) {
			rollingStep--;
			rollingCount = Led7::rollSpeed;
			for (short i = LED7_NB_DIGITS - 1; i >= 0; i--) {
				switch(rollingStep) {
				case 3:
					toSend[i] = prepareForPos(prepareForSegment(
							(toDisplayBefore[i] & 0xfe) << 3 ), i);
				break;
				case 2:
					toSend[i] = prepareForPos(prepareForSegment(
							((toDisplayBefore[i] & 0xfe) << 6 )
							| ((toDisplay[i] & 0x80) >> 6) ), i);
				break;
				case 1:
					toSend[i] = prepareForPos(prepareForSegment(
							(toDisplay[i] & 0xf0) >> 3 ), i);
				break;
				case 0:
					toSend[i] = prepareForPos(prepareForSegment(toDisplay[i]), i);
				break;
				}
			}
		} else {
			rollingCount--;
		}
	}
	doSend(toSend[digit]);
	digit = (digit + 1) % LED7_NB_DIGITS;
}

void Led7::log(byte value) {
	doSend(prepareForPos(prepareForSegment(getSegments('0' + (value % 10))), value / 10));
}

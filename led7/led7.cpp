
#include <Arduino.h>
#include "led7.h"

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

Led7::Led7() {
	rollSpeed = 100;
}

byte Led7::getSegments(char c) {
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
	for(short i = 0; i < nbDigits - 1; i++) {
		toDisplayBefore[i] = toDisplay[i];
		toDisplay[i] = toDisplay[i + 1];
		prepareDisplay(i, toDisplay[i]);
	}
	toDisplay[nbDigits - 1] = getSegments(c);
	prepareDisplay(nbDigits - 1, toDisplay[nbDigits - 1]);
	updateDisplay();
}

void Led7::display(unsigned int value) {
	cancelRoll();
	for (short i = nbDigits - 1; i >= 0; i--) {
		toDisplayBefore[i] = toDisplay[i];
		if (value == 0) {
			toDisplay[i] = (i == nbDigits - 1) ? getSegments('0') : 0;
		} else {
			toDisplay[i] = getSegments('0' + (value % 10));
			value /= 10;
		}
		prepareDisplay(i, toDisplay[i]);
	}
	updateDisplay();
}

void Led7::display(const char *value) {
	cancelRoll();
	short i;
	for (i = 0; value[i] != '\0' && i < nbDigits; i++) {
		toDisplayBefore[i] = toDisplay[i];
		toDisplay[i] = getSegments(value[i]);
		prepareDisplay(i, toDisplay[i]);
	}
	while (i < nbDigits) {
		toDisplayBefore[i] = toDisplay[i];
		toDisplay[i] = 0;
		prepareDisplay(i, 0);
		i++;
	}
	updateDisplay();
}

void Led7::display(const char *value, int len) {
	cancelRoll();
	short i;
	if (len > nbDigits) {
		len = nbDigits;
	}
	for (i = 0; i < len; i++) {
		toDisplayBefore[i] = toDisplay[i];
		toDisplay[i] = getSegments(value[i]);
		prepareDisplay(i, toDisplay[i]);
	}
	while (i < nbDigits) {
		toDisplayBefore[i] = toDisplay[i];
		toDisplay[i] = 0;
		prepareDisplay(i, 0);
		i++;
	}
	updateDisplay();
}

void Led7::display(String value) {
	cancelRoll();
	for (unsigned short i = 0; i < nbDigits; i++) {
		toDisplayBefore[i] = toDisplay[i];
		toDisplay[i] = (i >= value.length()) ? 0 : getSegments(value[i]);
		prepareDisplay(i, toDisplay[i]);
	}
	updateDisplay();
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

void Led7::send() {
	// look if toSend must be modified by a running roll effect
	if (rollingStep > 0) {
		if (rollingCount == 0) {
			rollingStep--;
			rollingCount = Led7::rollSpeed;
			for (short i = nbDigits - 1; i >= 0; i--) {
				switch(rollingStep) {
				case 3:
					prepareDisplay(i, (toDisplayBefore[i] & 0xfe) << 3 );
				break;
				case 2:
					prepareDisplay(i,
							((toDisplayBefore[i] & 0xfe) << 6 )
							| ((toDisplay[i] & 0x80) >> 6) );
				break;
				case 1:
					prepareDisplay(i, (toDisplay[i] & 0xf0) >> 3 );
				break;
				case 0:
					prepareDisplay(i, toDisplay[i]);
				break;
				}
			}
			updateDisplay();
		} else {
			rollingCount--;
		}
	}
}

void Led7::log(byte value) {
	prepareDisplay(value / 10, getSegments('0' + (value % 10)));
	updateDisplay();
}

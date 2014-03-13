#include "csg4m.h"
#include <Arduino.h>
#include <Wire/Wire.h>

#define CSG4M_NB_DIGITS 4

/* segment selection masks */
#define St (1 << 0)
#define SL (1 << 5)
#define SR (1 << 1)
#define Sm (1 << 6)
#define Sl (1 << 4)
#define Sr (1 << 2)
#define Sb (1 << 3)
#define Sp (1 << 7)

bool wireInitialized = false;

CSG4M::CSG4M(byte address) : Led7() {
	if (!wireInitialized) {
		Wire.begin();
		delay(50);
		wireInitialized = true;
	}

	this->address = address;
	nbDigits = CSG4M_NB_DIGITS;
	setPower(4);

	toDisplay = (byte *)malloc(CSG4M_NB_DIGITS);
	toDisplayBefore = (byte *)malloc(CSG4M_NB_DIGITS);
	toSend = (byte *)malloc(CSG4M_NB_DIGITS);
}

byte prepareForSegment(byte s) {
	int res = 0;
	if (s & (1 << 7)) res |= St;
	if (s & (1 << 6)) res |= SL;
	if (s & (1 << 5)) res |= SR;
	if (s & (1 << 4)) res |= Sm;
	if (s & (1 << 3)) res |= Sl;
	if (s & (1 << 2)) res |= Sr;
	if (s & (1 << 1)) res |= Sb;
	if (s & (1 << 0)) res |= Sp;
	return res;
}

void CSG4M::setPower(byte power) {
	byte data[2] = { 0x00, (byte)(((power & 0x7) << 4) | 0x7) };
	Wire.beginTransmission(address);
    Wire.write(data, 2);
    Wire.endTransmission();
}

void CSG4M::updateDisplay() {
	byte data[6] = { 0x01, toSend[0], toSend[1], toSend[2], toSend[3]};
	Wire.beginTransmission(address);
    Wire.write(data, 5);
    Wire.endTransmission();
}

void CSG4M::prepareDisplay(byte pos, byte segments) {
	toSend[pos] = prepareForSegment(segments);
}

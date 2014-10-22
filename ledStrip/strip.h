/**
 * Simplified version of Adafruit_NeoPixel lib
 * constraints :
 * - only for WS2812 @ 800KHz with GRB LEDs
 * - only one strip is handled
 * advantages :
 * - can set color by r, g and b / Color struct / H / H, L
 * todo :
 * - use some defines to handle other strips (just have to change order of
 *   fields in color struct)
 */

#include <Arduino.h>

typedef struct _color {
	// fields in the order needed to send raw data
	byte g;
	byte r;
	byte b;
} Color;

void stripInit(int len, byte pin);
void stripSetRGB(int position, byte r, byte g, byte b);
void stripSetColor(int position, Color c);
void stripSetH(int position, byte h);
void stripSetHL(int position, byte h, byte l);
Color stripGetColor(int position);
byte stripGetH(int position);

void stripAll(Color c);
void stripOff();

void stripUpdate();

void HtoRGB(byte h, Color *c);
void HLtoRGB(byte h, byte l, Color *c);
byte RGBtoH(Color c);

extern Color *strip;

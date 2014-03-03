/**
 * minimal library to handle 7 segments display
 * this class must be derived for device specific versions
 */

#ifndef LED_7_H
#define LED_7_H 1

#include <Arduino.h>

/**
 * list of all characters we can display
 */
#define LED7_CHARS "0123456789. abcdefghijklmnopqrstuvwxyz?-_'"

class Led7 {
public:
	/*
	 * number of digits handled by this display
	 */
	byte nbDigits;

	/**
	 * initialize pinout setup for display
	 */
	Led7();

	/**
	 * actually send current value of a digit to the device
	 * must be called repeatedly to display each digit.
	 * at least 25/s/digit thus 100/s
	 */
	void send();

	/**
	 * shift displayed chars on the left, and set last displayed char to "c"
	 */
	void shift(char c);

	/**
	 * displays 4 last digits of given positive integer value
	 */
	void display(unsigned int value);

	/**
	 * displays first 4 chars of given string
	 */
	void display(const char *value);
	void display(const char *value, int len);
	void display(String value);

	/**
	 * "roll" methods are like "display" methods, but
	 * change display in three steps, like if old value was pushed up by new value
	 */
	void roll(unsigned int value);
	void roll(const char *value);
	void roll(const char *value, int len);
	void roll(String value);

	/**
	 * number of calls to send() method to wait between each step of roll effect
	 */
	int rollSpeed;

	void log(byte value);

//protected:
	// current segments to light on display
	volatile byte *toDisplay;
	// segments lighted on before a roll effect was launched
	volatile byte *toDisplayBefore;

	byte getSegments(char c);
	virtual void prepareDisplay(byte pos, byte segments) = 0;
	virtual void updateDisplay() = 0;
};
#endif

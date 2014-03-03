/**
 * minimal library to handle a CSG-4M  7 segments 4 digits display, controlled by SAA1064
 * (http://www.nxp.com/documents/data_sheet/SAA1064_CNV.pdf)
 * inspiration from http://mbed.org/users/wim/notebook/saa1064-i2c-7-segment-led-driver/
 *
 * TODO : Wire conflicts with mstimer2 => how to call send() repeatedly without mstimer ?
 * => need a global scheduler instead ?
 *
 */

#ifndef LED_7_A574G__H
#define LED_7_A574G_H 1

#include "led7.h"

#define I2C_DEFAULT_ADDR 0x38

class CSG4M : public Led7 {
public:
	CSG4M(byte address = I2C_DEFAULT_ADDR);

	// I2C address of device
	byte address;

	// value between 0 and 7
	void setPower(byte power);

protected:
	void prepareDisplay(byte pos, byte segments);
	void updateDisplay();

private:
	// current mask to send to display
	volatile byte *toSend;
	volatile byte power;
};
#endif

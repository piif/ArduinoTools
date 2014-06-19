/**
 * minimal library to handle a A-574G 7 segments 4 digits display
 *  (http://www.paralight.us/uploads/pdf/A-574G.pdf)
 */

#ifndef LED_7_A574G__H
#define LED_7_A574G_H 1

#include "led7.h"

class A574g : public Led7 {
public:
	A574g();
	void send();

protected:
	void prepareDisplay(byte pos, byte segments);
	void updateDisplay();
	word prepareForSegment(byte s);
	word prepareForPos(word map, byte pos);
	void doSend(word w);
};
#endif

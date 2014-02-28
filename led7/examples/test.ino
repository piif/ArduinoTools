#include "led7/a574g.h"
#include <MsTimer2/MsTimer2.h>

A574g display;

void send() {
	display.send();
}

void setup(void) {
	Serial.begin(115200);

	display.display("toto");
	display.rollSpeed = 100;

	MsTimer2::set(1, send);
	MsTimer2::start();

	Serial.println("setup end");
}

void demoCount() {
	for(int value = 0; value < 100; value++) {
		display.display(value);
		delay(100);
	}
}

void demoRoll() {
	const char *texts[] = {
			"8888", "toto", "1234", "- -", "Pif.", "42..",
			0
	};
	for(const char **text = texts; *text; text++) {
		display.roll(*text);
		delay(2000);
	}
}

void demoShift() {
	const char *text = "1234567890  ";
	for(const char *c = text; *c; c++) {
		display.shift(*c);
		delay(500);
	}
}

void loop() {
	demoCount();
	demoRoll();
	demoShift();
}

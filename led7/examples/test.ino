#define TARGET_DISPLAY_CSG4M 1

#ifdef TARGET_DISPLAY_A574G
	#include "led7/a574g.h"
	A574g *display;
#elif TARGET_DISPLAY_CSG4M
	#include "led7/csg4m.h"
	CSG4M *display;
#else
	#error "Must define a TARGET_DISPLAY_*"
#endif

#include <MsTimer2/MsTimer2.h>

void send() {
	display->send();
}

void setup(void) {
	Serial.begin(115200);

	Serial.println("setup start");

#ifdef TARGET_DISPLAY_A574G
	display = new A574G();
#elif TARGET_DISPLAY_CSG4M
	display = new CSG4M();
#endif

	display->rollSpeed = 1000;
//	display->display("toto");

	MsTimer2::set(10, send);
	MsTimer2::start();

	Serial.println("setup end");
}

#if TARGET_DISPLAY_CSG4M
void demoPower() {
	for(int value = 0; value < 8; value++) {
		display->setPower(value);
		display->display(8880 + value);
		delay(1000);
	}
	display->setPower(4);
}
#endif

void demoCount() {
	for(int value = 0; value < 100; value++) {
		display->display(value);
		delay(100);
	}
}

void demoRoll() {
	const char *texts[] = {
			"8888", "toto", "1234", "- -", "Pif.", "42..",
			0
	};
	for(const char **text = texts; *text; text++) {
		display->roll(*text);
		delay(2000);
	}
}

void demoShift() {
	const char *text = "1234567890   ";
	for(const char *c = text; *c; c++) {
		display->shift(*c);
		delay(500);
	}
}


void loop() {
	demoCount();
#if TARGET_DISPLAY_CSG4M
	demoPower();
#endif
//	demoRoll();
	demoShift();
}

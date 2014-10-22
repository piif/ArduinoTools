#ifdef PIF_TOOL_CHAIN
	#include <Arduino.h>
	// other includes with full pathes
	// example : #include "led7/led7.h"
#else
	// other includes with short pathes
	// example : #include "led7.h"
#endif

#ifndef DEFAULT_BAUDRATE
	#define DEFAULT_BAUDRATE 115200
#endif

#define STRIP_PIN 8
#define STRIP_LEN 60
#include <ledStrip/strip.h>



void setup(void) {
	Serial.begin(DEFAULT_BAUDRATE);

	pinMode(13, OUTPUT);
	Serial.println("ready");
	stripInit(STRIP_LEN, STRIP_PIN);
	stripUpdate(); // Initialize all pixels to 'off'

//	for(int i = 0; i < STRIP_LEN; i++) {
//		stripSetRGB(i, 255, 255, 255);
//	}
//	Serial.println("white");Serial.flush();
//	stripUpdate();
//	delay(5000);
//	for(int i = 0; i < STRIP_LEN; i++) {
//		stripSetRGB(i, 0, 0, 0);
//	}
//	Serial.println("black");Serial.flush();
//	stripUpdate();
//	delay(3000);
//	for(int i = 0; i < STRIP_LEN; i++) {
//		stripSetRGB(i, 255, 255, 255);
//	}
//	Serial.println("white");Serial.flush();
//	stripUpdate();
//	delay(5000);

	Serial.println("loop");Serial.flush();
}

void effectOnOff(int step) {
	static bool flag = HIGH;
	static byte h = 0;
	static byte position = 0;
	if (flag) {
		stripSetH(position, h);
		h = (h + 3) % 256;
	} else {
		stripSetH(position, 0);
	}
    stripUpdate();
    position++;
	if (position == STRIP_LEN) {
		position = 0;
		flag = flag ? LOW : HIGH;
	}
	delay(5);

}

void effectScroll(int step) {
	byte first = stripGetH(0);
	for (int i = 1; i < STRIP_LEN; i++) {
		stripSetH(i - 1, stripGetH(i));
	}
	stripSetH(STRIP_LEN - 1, first);
    stripUpdate();

	delay(25);
}

void effectPeaks(int step) {
	static int peak = STRIP_LEN / 7;
	static byte h = 3;

	for (int i = peak - 9; i < peak + 9; i++) {
		if (i < 0 || i > STRIP_LEN) {
			continue;
		}
		byte level = 9 - abs(9 - (step % 19));
		byte dist = abs(peak - i);
		if (level == 0) {
			stripSetH(i, 0);
		} else {
			stripSetHL(i, h, (10 - dist) * level * 2);
		}
	}
	if (step % 19 == 18) {
		peak = (peak + 17) % STRIP_LEN;
		h = (h + 13) % 255 + 1;
	}
    stripUpdate();
	delay(25);
}

void effectShade(int step) {
	static byte h = 17;

	for(byte i = 0; i < STRIP_LEN; i++) {
		stripSetHL(i, h, (i+1) * 3);
	}

    stripUpdate();
	h = h * 4;
	delay(25);
}

void effectAll(int step) {
	static Color colors[] = {
		{g:0,r:255,b:0},
		{g:255,r:0,b:0},
		{g:0,r:0,b:255},
		{g:255,r:255,b:255}
	};
	stripAll(colors[step]);
	delay(500);
}

typedef void (*EffectCallback)(int);

typedef struct _effect {
	unsigned int nbSteps;
	EffectCallback func;
} Effect;

Effect effects[] = {
	{
		nbSteps: 4,
		func: effectAll
	},
	{
		nbSteps: 19 * 10,
		func: effectPeaks
//	}, {
//		nbSteps: 1,
//		func: effectShade
//	}, {
//		nbSteps: 5 * 60,
//		func: effectScroll
	}, {
		nbSteps: 9 * 60,
		func: effectOnOff
	}, {
		nbSteps: 5 * 60,
		func: effectScroll
	}
};

unsigned int effect = 0, step = 0;
bool off = false;

void loop() {
	if (Serial.available()) {
		int b = Serial.read();
		if (b == '0') {
			off = true;
			stripOff();
		} else if (b == '1') {
			off = false;
			step = effect = 0;
		}
	}
	if (!off) {
		if (step >= effects[effect].nbSteps) {
			step = 0;
			effect++;
			if (effect >= sizeof(effects)) {
				effect = 0;
			}
		}
		effects[effect].func(step);
	}
	step++;
}

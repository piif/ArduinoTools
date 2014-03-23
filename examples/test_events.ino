#include <Arduino.h>
#include <avr/sleep.h>

#define USE_INTERRUPT_INPUT_HANDLER_0
#include <events/events.h>

byte rollSteps[] = { 7, 6, 8, 11, 12, 10 };
void rollStep(byte detail, void *data) {
	static byte step = 0;
	digitalWrite(rollSteps[step], HIGH);
	step = (step + 1) % 6;
	digitalWrite(rollSteps[step], LOW);
}

void blink(byte detail, void *data) {
	static bool flag = HIGH;
	digitalWrite(9, flag);
	flag = !flag;
}

bool b3 = HIGH, b4 = HIGH, b5 = HIGH;

void button2(byte detail, void *data) {
	bool b = digitalRead(3);
	if (b != b3) {
		b3 = b;
		Serial.print("B3 ");
		Serial.println(b3);
	} else {
		bool b = digitalRead(4);
		if (b != b4) {
			b4 = b;
			Serial.print("B4 ");
			Serial.println(b4);
		} else {
			bool b = digitalRead(5);
			if (b != b5) {
				b5 = b;
				Serial.print("B5 ");
				Serial.println(b5);
			} else {
				Serial.println("?");
			}
		}
	}
	Serial.flush();
}
void button(byte detail, void *data) {
	Serial.print("button ...");
	Serial.flush();
	Scheduler.registerTimeout(1, button2, 0);
}

void setup() {
	Serial.begin(115200);
	Serial.println("test event");
	Serial.flush();

	for (int i = 6; i <= 12; i++) {
		pinMode(i, OUTPUT);
		digitalWrite(i, LOW);
	}
	pinMode(13, OUTPUT);
	digitalWrite(13, HIGH);

	Scheduler.begin();
	Events::eventHandler *eh;
	eh = Scheduler.registerInterval(300, 30, rollStep, 0);
	if (eh == 0) {
		Serial.println("timer 1 failed");
	}
	eh = Scheduler.registerInterval(150, -1, blink, 0);
	if (eh == 0) {
		Serial.println("timer 2 failed");
	}
	eh = Scheduler.registerInput(2, CHANGE, button, 0);
	if (eh == 0) {
		Serial.println("input failed");
	}
}

void loop() {
	Scheduler.waitNext(SLEEP_MODE_IDLE);
// TODO : WHY DOESN'T IT WORK WITH PWR_SAVE !!!
//	Scheduler.waitNext(SLEEP_MODE_PWR_SAVE);
}

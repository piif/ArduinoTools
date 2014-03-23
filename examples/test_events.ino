#include <Arduino.h>
#include <avr/sleep.h>

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
	Scheduler.registerInterval(300, 30, rollStep, 0);
	Scheduler.registerInterval(150, -1, blink, 0);
}

void loop() {
	Scheduler.waitNext(SLEEP_MODE_IDLE);
// TODO : WHY DOESN'T IT WORK WITH PWR_SAVE !!!
//	Scheduler.waitNext(SLEEP_MODE_PWR_SAVE);
}

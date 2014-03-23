#include <Arduino.h>
#include <avr/sleep.h>

#include <events/delayIdle.h>

void setup() {
	Serial.begin(115200);
	Serial.println("test event");

	for (int i = 6; i <= 12; i++) {
		pinMode(i, OUTPUT);
		digitalWrite(i, LOW);
	}
	pinMode(13, OUTPUT);
	digitalWrite(13, HIGH);
}

void loop() {
	Serial.println("delayIdle1");
	Serial.flush();

	delayIdleWith(400L * 1000L, 2, SLEEP_MODE_PWR_SAVE);
	Serial.println(innerloops);

	digitalWrite(6, HIGH);
	digitalWrite(7, HIGH);
	digitalWrite(8, HIGH);
	digitalWrite(10, LOW);
	digitalWrite(11, LOW);
	digitalWrite(12, LOW);

	Serial.println("delayIdle2");
	Serial.flush();

	delayIdleWith(2600L * 1000L, 2, SLEEP_MODE_PWR_SAVE);
	Serial.println(innerloops);

	digitalWrite(6, LOW);
	digitalWrite(7, LOW);
	digitalWrite(8, LOW);
	digitalWrite(10, HIGH);
	digitalWrite(11, HIGH);
	digitalWrite(12, HIGH);
}

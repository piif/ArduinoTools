#include <Arduino.h>

#define USE_INTERRUPT_INPUT_HANDLER_0
#include <ArduinoTools.h>

bool b0, b1, b2, b3;

volatile bool input2 = false;

void input2handler(int data) {
	input2=true;
}

void setup() {
	Serial.begin(115200);
	Serial.println("test buttons");
	Serial.flush();

	pinMode(2, INPUT);

	pinMode(3, INPUT_PULLUP);
	pinMode(4, INPUT_PULLUP);
	pinMode(5, INPUT_PULLUP);

	pinMode(6, OUTPUT);
	pinMode(7, OUTPUT);
	pinMode(8, OUTPUT);
	pinMode(13, OUTPUT);
	digitalWrite(13, HIGH);

	b0 = digitalRead(2);
	b1 = digitalRead(3);
	b2 = digitalRead(4);
	b3 = digitalRead(5);
	digitalWrite(6, b1);
	digitalWrite(7, b2);
	digitalWrite(8, b3);

	setInputHandler(INTERRUPT_FOR_PIN(2), input2handler);
	enableInputInterrupt(INTERRUPT_FOR_PIN(2), CHANGE);
}

void loop() {
	bool b;

	sleepNow(SLEEP_MODE_PWR_SAVE);
	if (input2) {
		input2 = false;
		delay(1);
		b = digitalRead(2);
		if (b != b0) {
			Serial.print("0 : ");
			b0 = b;
			Serial.print(b0);

			pinMode(2, OUTPUT);
			digitalWrite(2, LOW);

			b1 = digitalRead(3);
			b2 = digitalRead(4);
			b3 = digitalRead(5);

			digitalWrite(6, b1);
			digitalWrite(7, b2);
			digitalWrite(8, b3);

			Serial.print(" -> ");
			Serial.print(b1);
			Serial.print(b2);
			Serial.println(b3);
			Serial.flush();

			pinMode(2, INPUT);
		}
	}
}

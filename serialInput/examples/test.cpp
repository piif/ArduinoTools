
#include "Arduino.h"
#include "../serialInput.h"

byte a = 0;
int b = 0;

void c() {
	Serial.print("function c : a = ");
	Serial.print(a, DEC);
	Serial.print(", b = ");
	Serial.println(b);
}

InputItem list[] = {
	{ 'a', 'b', &a },
	{ 'b', 'i', &b },
	{ 'c', 'f', (void *)c }
};

int main(void) {
	registerInput(sizeof(list), list);
	for(;;) {
		handleInput();
	}
}

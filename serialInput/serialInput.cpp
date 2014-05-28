/**
 * minimal lib to react to input data from serial
 */

#include "serialInput.h"
#include "Arduino.h"

byte buffer[SERIAL_INPUT_MAX_LEN + 1];
int current = 0;

int nbItems = 0;
InputItem *items;

int registerInput(int _nbItems, InputItem *_items) {
	nbItems = _nbItems;
	items = _items;
	return 0;
}

int convChar(char c) {
	if (c >= '0' && c <= '9') return c - '0';
	if (c >= 'a' && c <= 'f') return c - 'a' + 10;
	if (c >= 'A' && c <= 'F') return c - 'A' + 10;
	return 0;
}

void parseInput(byte *str, const int len) {
	for (int i = 0; i < nbItems; i++) {
		if (items[i].prefix == str[0]) {
			// command found
			Serial.print("\nFound command '");
			Serial.print((char)str[0]);
			if (len > 1) {
				Serial.print("', '");
				Serial.write((str + 1), len - 1);
			}
			Serial.println("'.");

			int value = 0, b = 10, s = 1;
			for (int j = 1; j < len; j++) {
				if (j == 1 && (char)str[j] == 'x') {
					b = 16;
				} else if (j == 1 && (char)str[j] == '-') {
					s = -1;
				} else {
					value = value * b + convChar((char)str[j]);
				}
			}
			value = value * s;

			if (items[i].type == 'f' || items[i].type == 'I') {
				((destFuncInt)(items[i].destination))(value);
			} else if (items[i].type == 'B') {
				((destFuncByte)(items[i].destination))((byte)value);
			} else if (items[i].type == 'S') {
				str[len] = '\0';
				((destFuncString)(items[i].destination))((const char *)(str + 1));
			} else if (items[i].type == 'b') {
				*((byte *)(items[i].destination)) = value;
			} else if (items[i].type == 'i') {
				*((int *)(items[i].destination)) = value;
			} else if (items[i].type == 's') {
				str[len] = '\0';
				for(int j = 1; j <= len; j++) {
					((char *)(items[i].destination))[j - 1] = str[j];
				}
			}
			return;
		}
	}
	// unknown command.
	Serial.print("!! Unknown command '");
	Serial.print(str[0], DEC);
	Serial.println("'. ignored.");
}

void handleInput() {
	handleInput(true);
}

void handleInput(bool echo) {
	// read data if any
	// TODO : handle this by interruptions ?
	int avail = Serial.available();
	if (avail > SERIAL_INPUT_MAX_LEN - current) {
		avail = SERIAL_INPUT_MAX_LEN - current;
	}
	// TODO : consider readBytes will work since available was called just before ...
	while (avail) {
		buffer[current] = Serial.read();
		if (echo) {
			Serial.print((char)buffer[current]);
		}
		current++;
		avail--;
	}
	// handle data if available. may happen even if to new data read,
	//  if data are still in buffer after command parsing in preceding call
	if (current == 0) {
		return;
	}

	int i, j;
	// handle input
	for (i = 0; i < current; i++) {
		if (buffer[i] == '\n' || buffer[i] == '\r') {
			parseInput(buffer, i);
			// skip newlines
			do {
				i++;
			} while (i < current && (buffer[i] == '\n' || buffer[i] == '\r'));
			// move end of buffer at its start
			j = 0;
			while (i < current) {
				buffer[j] = buffer[i];
				i++;
				j++;
			}
			current = j;
			return;
		}
	}
	if (current == SERIAL_INPUT_MAX_LEN) {
		// buffer overflow => parse or ignore ?
		// for the moment, skip data.
		current = 0;
	}
}

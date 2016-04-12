/**
 * minimal lib to react to input data from serial
 */

#include "serialInput.h"
#include "Arduino.h"

static byte buffer[SERIAL_INPUT_MAX_LEN + 1];
static int current = 0;

static int nbItems = 0;
static InputItem *items;

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

void parseInput(byte *str, const int len, Stream &channel) {
	for (int i = 0; i < nbItems; i++) {
		if (items[i].prefix == str[0]) {
			// command found
			channel.print("\nFound command '");
			channel.print((char)str[0]);
			if (len > 1) {
				channel.print("', '");
				channel.write((str + 1), len - 1);
			}
			channel.println("'.");

			if (items[i].type == 'S') {
				str[len] = '\0';
				((destFuncString)(items[i].destination))((const char *)(str + 1), channel);
			} else if (items[i].type == 's') {
				str[len] = '\0';
				for(int j = 1; j <= len; j++) {
					((char *)(items[i].destination))[j - 1] = str[j];
				}
			} else {
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
					((destFuncInt)(items[i].destination))(value, channel);
				} else if (items[i].type == 'B') {
					((destFuncByte)(items[i].destination))((byte)value, channel);
				} else if (items[i].type == 'b') {
					*((byte *)(items[i].destination)) = value;
				} else if (items[i].type == 'i') {
					*((int *)(items[i].destination)) = value;
				}
			}
			return;
		}
	}
	// unknown command.
	channel.print("!! Unknown command '");
	channel.print(str[0], DEC);
	channel.println("', ignored.");
}

void handleInput() {
	handleInput(Serial, true);
}

void handleInput(Stream &channel) {
	handleInput(channel, true);
}

void handleInput(bool echo) {
	handleInput(Serial, echo);
}

void handleInput(Stream &channel, bool echo) {
	// read data if any
	// TODO : handle this by interruptions ?
	int avail = channel.available();
	if (avail > SERIAL_INPUT_MAX_LEN - current) {
		avail = SERIAL_INPUT_MAX_LEN - current;
	}
	// TODO : consider readBytes will work since available was called just before ...
	while (avail) {
		buffer[current] = channel.read();
		if (echo) {
			channel.print((char)buffer[current]);
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
			if (i > 0) { // skip empty lines
				parseInput(buffer, i, channel);
			}
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
		channel.print("\nLine to long, input aborted.\n");
		current = 0;
	}
}

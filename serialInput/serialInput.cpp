/**
 * minimal lib to react to input data from serial
 */

#include "serialInput.h"
#include "Arduino.h"

static byte buffer[SERIAL_INPUT_MAX_LEN + 1];
static int bufferStart = 0, bufferEnd = 0;

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

int parseInput(byte *str, const int len, Stream &channel) {
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
				return ((destFuncString)(items[i].destination))((const char *)(str + 1), channel);
			} else if (items[i].type == 's') {
				str[len] = '\0';
				for(int j = 1; j <= len; j++) {
					((char *)(items[i].destination))[j - 1] = str[j];
				}
				return len;
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
					return ((destFuncInt)(items[i].destination))(value, channel);
				} else if (items[i].type == 'B') {
					return ((destFuncByte)(items[i].destination))((byte)value, channel);
				} else if (items[i].type == 'b') {
					return *((byte *)(items[i].destination)) = value;
				} else if (items[i].type == 'i') {
					return *((int *)(items[i].destination)) = value;
				}
			}
		}
	}
	// unknown command.
	channel.print("!! Unknown command '");
	channel.print(str[0], DEC);
	channel.println("', ignored.");
	return -1;
}

int handleInput() {
	return handleInput(Serial, true);
}

int handleInput(Stream &channel) {
	return handleInput(channel, true);
}

int handleInput(bool echo) {
	return handleInput(Serial, echo);
}

int handleInput(Stream &channel, bool echo) {
	// skip newlines
	while (bufferStart < bufferEnd && (buffer[bufferStart] == '\n' || buffer[bufferStart] == '\r')) {
		bufferStart++;
	}
	if (bufferStart > 0) {
		// newlines skipped, or reentrant call => must shift data
		// shift remaining bytes in buffer
		int j = 0;
		while (bufferStart < bufferEnd) {
			buffer[j] = buffer[bufferStart];
			bufferStart++;
			j++;
		}
		bufferStart = 0;
		bufferEnd = j;
	}

	// read data if any
	// TODO : handle this by interruptions ?
	int avail = channel.available();
	if (avail > SERIAL_INPUT_MAX_LEN - bufferEnd) {
		avail = SERIAL_INPUT_MAX_LEN - bufferEnd;
	}
	// TODO : consider readBytes will work since available was called just before ...
	while (avail) {
		buffer[bufferEnd] = channel.read();
		if (echo) {
			channel.print((char)buffer[bufferEnd]);
		}
		bufferEnd++;
		avail--;
	}

	// handle data if available. may happen even if no new data read,
	// if data are still in buffer after command parsing in previous call
	if (bufferEnd == 0) {
		return -1;
	}

	int i;
	// handle input
	for (i = 0; i < bufferEnd; i++) {
		if (buffer[i] == '\n' || buffer[i] == '\r') {
			if (i > 0) { // skip empty lines
				bufferStart = i+1; // => next call to handleInput will read remaining buffer
				return parseInput(buffer, i, channel);
			}
		}
	}
	if (bufferEnd == SERIAL_INPUT_MAX_LEN) {
		// buffer overflow => parse or ignore ?
		// for the moment, skip data.
		channel.print("\nLine to long, input aborted.\n");
		bufferEnd = 0;
	}
	return -1;
}

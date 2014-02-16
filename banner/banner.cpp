
// include prototype
#include "banner.h"

// include police structure
#include POLICE

static char *currentMessage;
static int charIndex, byteIndex, messageLen;

static int findChar(char);

/**
 * initialize iterator on given message
 * return number of iterations needed
 */
int bannerSetMessage(char *message) {
	currentMessage = message;
	messageLen = 0;
	while(message[messageLen]) {
		message[messageLen] = findChar(message[messageLen]);
		messageLen++;
	}
	charIndex = byteIndex = 0;
	return (MAPS_NB_BYTES_PER_CHAR+1) * messageLen;
}

/**
 * return next byte to send to display message
 * loops at end of message
 */
int bannerNext() {
	if (byteIndex == MAPS_NB_BYTES_PER_CHAR) {
		// end of current char => return a "space" between chars
		byteIndex++;
		return 0;
	} else if (byteIndex > MAPS_NB_BYTES_PER_CHAR) {
		// after end of char => take next one, and continue nominal case
		charIndex++;
		if (charIndex >= messageLen) {
			charIndex = 0;
		}
		byteIndex = 0;
	}
	// nominal case => return current byte of current char, then ++ byte index
	return *(MAP_FOR(currentMessage[charIndex]) + byteIndex++);
}

/**
 * force to go to given step in banner scenario
 */
void bannerSeek(int newCharIndex, int newByteIndex) {
	charIndex = newCharIndex % messageLen;
	byteIndex = newByteIndex % (MAPS_NB_BYTES_PER_CHAR + 1);
}

static int findChar(char c) {
	int i = NB_CHARS;
	while(i) {
		i--;
		if (CHARS[i] == c) {
			return i;
		}
	}
	return -1;
}

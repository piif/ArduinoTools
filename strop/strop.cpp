#include "strop.h"

Stream& operator<<(Stream& stream, const __FlashStringHelper *v) {
    stream.print(v);
    return stream;
}
Stream& operator<<(Stream& stream, const String &v) {
    stream.print(v);
    return stream;
}
Stream& operator<<(Stream& stream, const char *v ) {
    stream.print(v);
    return stream;
}
Stream& operator<<(Stream& stream, char v) {
    stream.print(v);
    return stream;
}
Stream& operator<<(Stream& stream, unsigned char v) {
    stream.print(v);
    return stream;
}
Stream& operator<<(Stream& stream, int v) {
    stream.print(v);
    return stream;
}
Stream& operator<<(Stream& stream, unsigned int v) {
    stream.print(v);
    return stream;
}
Stream& operator<<(Stream& stream, long v) {
    stream.print(v);
    return stream;
}
Stream& operator<<(Stream& stream, unsigned long v) {
    stream.print(v);
    return stream;
}
Stream& operator<<(Stream& stream, double v) {
    stream.print(v);
    return stream;
}
Stream& operator<<(Stream& stream, const Printable &v) {
    stream.print(v);
    return stream;
}

static const char hexChars[]  = "0123456789abcdef";
/* static buffer for conversion operation results
 * /!\ it's static thus not reentrant
 */
static char buffer[17];

const char *toBaseN(word value, byte base, byte digitLen, byte size, byte limit) {
    char *p = buffer;
    base--;
    if (size > limit) {
        size = limit;
    }
    do {
        size--;
        *p++ = hexChars[(value >> (digitLen*size)) & base];
    } while(size);
    *p = '\0';
    return buffer;
}

const char *toHex(byte value, byte size = 2) {
    return toBaseN(value, 16, 4, size, 2);
}
const char *toHex(word value, byte size = 4) {
    return toBaseN(value, 16, 4, size, 4);
}
const char *toOct(byte value, byte size = 3) {
    return toBaseN(value, 8, 3, size, 3);
}
const char *toOct(word value, byte size = 6) {
    return toBaseN(value, 8, 3, size, 6);
}
const char *toBin(byte value, byte size = 8) {
    return toBaseN(value, 2, 1, size, 8);
}
const char *toBin(word value, byte size = 16) {
    return toBaseN(value, 2, 1, size, 16);
}

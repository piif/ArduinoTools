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

Stream& operator<<(Stream& os, BaseN b) {
    // number of bits per digit
    unsigned short digit_len = (b.base==2) ? 1 : 4;
    // bit mask for 1 digit
    unsigned long mask = b.base-1;
    unsigned short shift;
    if (b.len != 0) {
        shift = (b.len-1)*digit_len;
    } else {
        shift = (sizeof(mask) * __CHAR_BIT__)-digit_len;
    }
    mask <<= shift;
    while (shift > 0 && (mask & b.value) == 0) {
        if (b.len != 0) {
            os << b.pad;
        }
        shift -= digit_len;
        mask >>= digit_len;
    }
    while(mask) {
        unsigned short d = (mask & b.value) >> shift;
        os << (char)( d >= 10 ? (d-10+'a') : (d+'0') );
        shift -= digit_len;
        mask >>= digit_len;
    }
    return os;
}

BaseN bin(unsigned long value, unsigned short len = 0, char pad = ' ') {
    return BaseN(value, 2, len, pad);
}

BaseN hex(unsigned long value, unsigned short len = 0, char pad = ' ') {
    return BaseN(value, 16, len, pad);
}

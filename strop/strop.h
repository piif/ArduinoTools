/**
 * Stream Operators, similar to std::out ones in standard C++
 * Each operator as same prototype as those in arduino lib Print.h, thus whatever you can do with Serial.print
 * can be done with this lib, using such syntax :
 *   Serial << "a string " << intVariable << EOL;
 *
 * TODO: DEC, HEX, BIN arguments for integer are not implemented
 * 
 */

#ifndef STR_OP_H
#define STR_OP_H

#include "Arduino.h"

Stream& operator<<(Stream& stream, const __FlashStringHelper *v);
Stream& operator<<(Stream& stream, const String &v);
Stream& operator<<(Stream& stream, const char *v );
Stream& operator<<(Stream& stream, char v);
Stream& operator<<(Stream& stream, unsigned char v);
Stream& operator<<(Stream& stream, int v);
Stream& operator<<(Stream& stream, unsigned int v);
Stream& operator<<(Stream& stream, long v);
Stream& operator<<(Stream& stream, unsigned long v);
Stream& operator<<(Stream& stream, double v);
Stream& operator<<(Stream& stream, const Printable &v);

struct BaseN {
    unsigned long value;
    unsigned short base; // 2 or 16 supported only
    unsigned short len;
    char pad;
    explicit BaseN(unsigned long v, unsigned short _base, unsigned short _len = 0, char _pad = ' ') :
      value(v), base(_base), len(_len), pad(_pad) {}
};

struct BaseN bin(unsigned long value, unsigned short len = 0, char pad = ' ');
struct BaseN hex(unsigned long value, unsigned short len = 0, char pad = ' ');
Stream& operator<<(Stream& os, BaseN b);

#define EOL '\n'

#endif

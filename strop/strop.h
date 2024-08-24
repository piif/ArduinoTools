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

#define EOL '\n'

/*
  Example :
    byte bt = 123;
    Serial << "test 123:16 = " << toHex(bt, 1) << " truncated = " << toHex(bt) << EOL;
    Serial << "test 123:2  = " << toBin(bt, 4) << " truncated = " << toBin(bt) << EOL;
    word wd = 0b111101101;
    Serial << "test 0755   = " << toOct(wd, 4) << " = " << toBin(wd) << EOL;
  Output :
    test 123:16 = b truncated = 7b
    test 123:2  = 1011 truncated = 01111011
    test 0755   = 0755 = 0000000111101101
*/

const char *toHex(byte value, byte size = 2);
const char *toHex(word value, byte size = 4);
const char *toOct(byte value, byte size = 3);
const char *toOct(word value, byte size = 6);
const char *toBin(byte value, byte size = 8);
const char *toBin(word value, byte size =16);

#endif

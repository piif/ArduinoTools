/**
 * to test on PC : g++ -x c++ formatters.ino -o formatters
 * to test on arduino, use IDE
 * 
 * some conclusions :
 * With g++ on x86/amd architecture, sizeof are :
 *                     x86/amd  arduino
 *   sizeof short     |   16   |  16  |
 *   sizeof int       |   32   |  16  |
 *   sizeof long      |   64   |  32  |
 *   sizeof long long |   64   |  64  |
 * 
 * __builtin_clz is hard coded relative to 32 bits on x86 and 16 bits on arduino, even with a 64 bits argument
 */

#ifdef ARDUINO
    #include <Arduino.h>
    #include "strop.h"
    #define OUTPUT Serial
    #define STREAM Stream
#else
    #include <iostream>
    #include <iomanip>
    #define OUTPUT cout
    #define STREAM ostream
    #define F(str) str
    using namespace std;

struct BaseN {
    unsigned long value;
    unsigned short base; // 2 or 16 supported only
    unsigned short len;
    char pad;
    explicit BaseN(unsigned long v, unsigned short _base, unsigned short _len = 0, char _pad = ' ') :
      value(v), base(_base), len(_len), pad(_pad) {}
};

STREAM& operator<<(STREAM& os, BaseN b) {
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
#endif

#define bitlen(x) ((sizeof(x) * __CHAR_BIT__) - __builtin_clz(x) )

void tests() {
    unsigned short s = 42;
    unsigned int   i = 42;
    unsigned long  l = 0x12345678UL;
    unsigned long long ll = 0x123456789ABCDEF0UL;

    OUTPUT << F("sizeof unsigned short s = ") << (sizeof( s) * __CHAR_BIT__)
           << F(" , clz/len(") << s << F(") = ") << __builtin_clz( s) << '/' << bitlen( s) << '\n';
    OUTPUT << F("sizeof unsigned int   i = ") << (sizeof( i) * __CHAR_BIT__)
           << F(" , clz/len(") << i << F(") = ") << __builtin_clz( i) << '/' << bitlen( i) << '\n';
    OUTPUT << F("sizeof unsigned long  l = ") << (sizeof( l) * __CHAR_BIT__)
           << F(" , clz/len(") << l << F(") = ") << __builtin_clz( l) << '/' << bitlen( l) << '\n';
    OUTPUT << F("sizeof unsigned long long ll = ") << (sizeof(ll) * __CHAR_BIT__)
           << F(" , clz/len(") <<ll << F(") = ") << __builtin_clz(ll) << '/' << bitlen(ll) << '\n';

    OUTPUT << F("\ns = ") << s
        << " = '0x" << hex(s) << "' = '0b" << bin(s)
        << "'\n  = (3/' ') '0x" << hex(s, 3) << "' =(7/' ') '0b" << bin(s, 7)
        << "'\n  = (3/'0') '0x" << hex(s, 3, '0') << "' =(7/'0') '0b" << bin(s, 7, '0')
        << "'\n  = (1/' ') '0x" << hex(s, 1) << "' =(4/' ') '0b" << bin(s, 4)
        << "'\n  = (1/'0') '0x" << hex(s, 1, '0') << "' =(4/'0') '0b" << bin(s, 4, '0') << "'\n\n";

    s = 1UL << ((sizeof(s) * __CHAR_BIT__)-1);
    OUTPUT << "s = " << s
        << " = '0x" << hex(s) << "' = '0b" << bin(s)
        << "'\n  = (4/' ') '0x" << hex(s, 4) << "' =(4/' ') '0b" << bin(s, 4)
        << "'\n  = (4/'0') '0x" << hex(s, 4, '0') << "' =(4/'0') '0b" << bin(s, 4, '0') << "'\n\n";

    s = 0;
    OUTPUT << "s = " << s
        << " = '0x" << hex(s) << "' = '0b" << bin(s)
        << "'\n  = (4/' ') '0x" << hex(s, 4) << "' =(4/' ') '0b" << bin(s, 4)
        << "'\n  = (4/'0') '0x" << hex(s, 4, '0') << "' =(4/'0') '0b" << bin(s, 4, '0') << "'\n\n";

    OUTPUT << "i = " << i << " = '0x" << hex(i)
        << "' = '0b" << bin(i)
        << "'\n  = (4/' ') '0x" << hex(i, 4) << "' =(8/' ') '0b" << bin(i, 8)
        << "'\n  = (4/'0') '0x" << hex(i, 4, '0') << "' =(8/'0') '0b" << bin(i, 8, '0') << "'\n\n";

    OUTPUT << "l = " << l << " = '0x" << hex(l)
        << "' = '0b" << bin(l)
        << "'\n  =(30/' ') '0b" << bin(l, 30)
        << "'\n  =(30/'0') '0b" << bin(l, 30, '0')
        << "'\n  =(32/'0') '0b" << bin(l, 32, '0') << "'\n\n";

    l = 1UL << ((sizeof(l) * __CHAR_BIT__)-1);
    OUTPUT << "l = " << l << " = '0x" << hex(l)
        << "' = '0b" << bin(l)
        << "'\n  =(30/'.') '0b" << bin(l, 30, '.')
        << "'\n  =(32/'0') '0b" << bin(l, 32, '0') << \'\n\n";

    OUTPUT << "ll = " << ll << " = '0x" << hex(ll)
        << "' = '0b" << bin(ll)
        << "'\n  =(60/'.') '0b" << bin(ll, 60, '.')
        << "'\n  =(64/'0') '0b" << bin(ll, 64, '0') << "'\n\n";
}

#ifdef ARDUINO

void setup() {
    Serial.begin(115200);
    tests();
}
void loop() {}

#else // ARDUINO

int main() {
    tests();
}

#endif // ARDUINO

#ifndef HEX_TOOLS_H
#define HEX_TOOLS_H

#include <Arduino.h>

void toHex(byte b, char *h);
void toHex(byte b, Stream *s);

void toHex(word w, char *h);
void toHex(word w, Stream *s);

void hexDump(byte *data, int len, Stream *s);

#endif // HEX_TOOLS_H

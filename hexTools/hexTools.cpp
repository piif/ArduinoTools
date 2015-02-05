#include "hexTools.h"

static const char hexChars[]  = "0123456789abcdef";

void toHex(byte b, char *h) {
	h[0] = hexChars[b >> 4];
	h[1] = hexChars[b & 0x0f];
}
void toHex(byte b, Stream *s) {
	s->write(hexChars[b >> 4]);
	s->write(hexChars[b & 0x0f]);
}

void toHex(word w, char *h) {
	h[0] = hexChars[w >> 12];
	h[1] = hexChars[(w >> 8) & 0x0f];
	h[2] = hexChars[(w >> 4) & 0x0f];
	h[3] = hexChars[w & 0x0f];
}
void toHex(word w, Stream *s) {
	s->write(hexChars[w >> 12]);
	s->write(hexChars[(w >> 8) & 0x0f]);
	s->write(hexChars[(w >> 4) & 0x0f]);
	s->write(hexChars[w & 0x0f]);
}

static const char HexDumpModel[] =
		"....: "
		".. .. .. .. .. .. .. ..  .. .. .. .. .. .. .. .. "
		"                 \n";

void hexDump(byte *data, int len, Stream *s) {
	char line[sizeof(HexDumpModel)];

	word iData = 0, iHex = 6, iChar = 55;
	while(iData < (word)len) {
		if ((iData & 0xF) == 0) {
			if (iData != 0) {
				s->write(line, sizeof(HexDumpModel) - 1);
			}
			memcpy(line, HexDumpModel, sizeof(HexDumpModel));
			iData = 0, iHex = 6, iChar = 55;
			toHex(iData, line);
		}
		toHex(data[iData], line + iHex);
		iHex += 3;
		if ((iData & 0x7) == 7) {
			iHex++;
		}

		line[iChar] = (data[iData] > 32) ? (char)data[iData] : '.';
		iChar++;

		iData++;
	}
	if ((iData & 0xF) != 0) {
		s->write(line, sizeof(HexDumpModel) - 1);
	}
}

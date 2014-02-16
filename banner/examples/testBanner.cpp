/** programme de test de la génération des fichiers de polices */

#include <stdio.h>

#include "../banner.h"

// depend of police.h file, but is unknown there => use full byte instead
#define CHAR_HEIGHT 8

void printChar(char map) {
	int y;
	unsigned char msk;

	y = CHAR_HEIGHT;
	msk = 0x1 /*<< (CHAR_HEIGHT - 1)*/;
	for(;;) {
		putchar( (map & msk) ? '#' : ' ');
		y--;
		if(y == 0) break;
		//msk>>=1;
		msk<<=1;
	}
	putchar('\n');
}

int main(int argc, char **argv) {
	char s[200];
	int len;

	while(gets(s)) {
		len = bannerSetMessage(s);
		printf("%d step to display\n", len);
		while(len--) {
			printChar(bannerNext());
		}
	}

	return 0;
}

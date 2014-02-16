CC=gcc
CFLAGS=-Wall -I.. -g
# -I/usr/include/libpng12
#LDFLAGS=-lpng
LDFLAGS=/usr/lib/libpng12.a -lz

all : testBanner5x8

testBanner.o: testBanner.cpp ../banner.h

testBanner5x8: testBanner.o banner5x8.o
	$(CC) $(LDFLAGS) -o testBanner5x8 testBanner.o banner5x8.o

banner3x5.o: ../police_3x5.h ../banner.cpp
	$(CC) $(CFLAGS) -DPOLICE='"police_3x5.h"' -c ../banner.cpp -o banner3x5.o

banner5x8.o: ../police_5x8.h ../banner.cpp
	$(CC) $(CFLAGS) -DPOLICE='"police_5x8.h"' -c ../banner.cpp -o banner5x8.o

clean:
	rm -f testBanner5x8 *.o

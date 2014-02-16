CC=gcc
CFLAGS=-Wall -g
# -I/usr/include/libpng12
#LDFLAGS=-lpng
LDFLAGS=/usr/lib/libpng12.a -lz

all :
	make -C makePolice
	make -C examples

clean:
	make -C makePolice clean
	make -C examples clean

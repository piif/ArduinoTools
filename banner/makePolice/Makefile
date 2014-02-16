CC=gcc
CFLAGS=-Wall -g
# -I/usr/include/libpng12
LDFLAGS=-lpng
#LDFLAGS=/usr/lib/libpng12.a -lz

all : png2h ../police_3x5.h ../police_5x8.h ../police_5x8_rev.h

png2h : png2h.o
	$(CC) -o png2h png2h.o $(LDFLAGS)

../police_3x5.h: police.txt police_3x5.png png2h
	./png2h police.txt police_3x5.png 3 5 > ../police_3x5.h

../police_5x8.h: police.txt police_5x8.png png2h
	./png2h police.txt police_5x8.png 5 8 > ../police_5x8.h

../police_5x8_rev.h: police.txt police_5x8.png png2h
	./png2h -r police.txt police_5x8.png 5 8 > ../police_5x8_rev.h

clean:
	rm -f ../police_3x5.h ../police_5x8.h ../police_5x8_rev.h png2h *.o

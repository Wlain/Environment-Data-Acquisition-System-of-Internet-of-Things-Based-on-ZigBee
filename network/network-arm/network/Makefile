CC=arm-linux-gcc
icsslread:main.c cssl.o
	$(CC) -o network main.c cssl.o
cssl.o:cssl.c cssl.h
	$(CC) -Wall -D_GNU_SOURCE -g -O -c cssl.c
clean:
	rm -f *.o network

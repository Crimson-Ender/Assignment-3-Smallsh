CC=gcc --std=c99 -g

smallsh: shell.c shell.h process.o
	$(CC) shell.c process.o -o smallsh

process.o: process.c process.h
	$(CC) -c process.c

clean:
	rm -f *.o smallsh
CC=gcc --std=c99 -g

smallsh: shell.c shell.h
	$(CC) shell.c -o smallsh

clean:
	rm -f *.o smallsh
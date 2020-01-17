CC = gcc
CFLAGS = -llz4 -O2 -g -Wall -fsanitize=address

all: library demo.c fastsearch.c fastsearch.h config.h
	$(CC) -DMAKE_DB $(CFLAGS) fastsearch.c demo.c

searchonly: demo.c fastsearch.c fastsearch.h config.h
	$(CC) $(CFLAGS) fastsearch.c demo.c

library: fastsearch.c fastsearch.h config.h
	$(CC) $(CFLAGS) -c fastsearch.c

clean:
	rm -f *.o a.out
CC=gcc
CFLAGS= -g

all: server phil

server: server.c
	$(CC) -o $@ $(CFLAGS) $^
phil: philosopher.c
	$(CC) -o $@ $(CFLAGS) $^

clean:
	rm -f server phil


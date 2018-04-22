CC=gcc
CFLAGS=-std=c99
DBFLAGS=-g

make: client server

client: client.c
	$(CC) $(CFLAGS) -pthread -o client.o client.c

server: server.c
	$(CC) $(CFLAGS) -pthread -o server.o server.c

clean:
	rm client.o server.o

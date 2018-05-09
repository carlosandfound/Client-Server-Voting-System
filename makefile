CC=gcc
CFLAGS=-std=c99
DBFLAGS=-g

make: client server

client: client.c
	$(CC) $(CFLAGS) -pthread -o client client.c

server: server.c
	$(CC) $(CFLAGS) -pthread -o server server.c

clean:
	rm client server

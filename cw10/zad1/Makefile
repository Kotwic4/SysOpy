CFLAGS += -Wall
CC = gcc

all: server client

server: server.c common.h
	$(CC) $(CFLAGS) server.c -o server -lpthread

client: client.c common.h
	$(CC) $(CFLAGS) client.c -o client

clean:
	-rm -rf *.o server client

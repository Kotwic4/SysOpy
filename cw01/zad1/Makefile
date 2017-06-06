CFLAGS += -Wall -g
CC = gcc

all: libbook.a libbook.so

book.o: book.c book.h
	$(CC) $(CFLAGS) -c book.c

libbook.a: book.o
	ar rcs $@ $^

libbook.so: book.c book.h
	$(CC) -fPIC -shared book.c -o $@


clean:
	-rm -rf *.o *.a *.so

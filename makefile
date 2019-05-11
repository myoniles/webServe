CC=gcc
CFLAGS = -g -Wall -pthread
SOURCES=$(wildcard src/*.c)
OBJECTS=$(patsubst %.c, %.o, $(SOURCES))

all: webServe clean

webServe: webServe.o
	$(CC) $(CFLAGS) -o webServe webServe.o

webServe.o: ./src/webServe.c ./src/*.h
	$(CC) $(CFLAGS) -c ./src/webServe.c

clean:
	-rm webServe.o

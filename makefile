CC=gcc
CFLAGS = -g -Wall
SOURCES=$(wildcard src/*.c)
OBJECTS=$(patsubst %.c, %.o, $(SOURCES))

all: ./bin/webServe clean

./bin/webServe: webServe.o
	$(CC) $(CFLAGS) -o ./bin/webServe webServe.o

webServe.o: ./src/webServe.c ./src/*.h
	$(CC) $(CFLAGS) -c ./src/webServe.c

clean:
	-rm webServe.o

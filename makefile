CC=gcc
CFLAGS = -g -Wall
SOURCES=$(wildcard src/*.c)
OBJECTS=$(patsubst %.c, %.o, $(SOURCES))

all: ./bin/webServe clean

./bin/webServe: webServe.o
	$(CC) -o ./bin/webServe webServe.o

webServe.o: ./src/webServe.c ./src/*.h
	$(CC) -c ./src/webServe.c

clean:
	-rm webServe.o

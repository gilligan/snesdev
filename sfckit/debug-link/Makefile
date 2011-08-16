CC=g++
CFLAGS=-Wall

all: debuglib

debuglib: sfcdebug.o util.o
	g++ -dynamiclib -o libsfcdebug.dylib sfcdebug.o  util.o
	cp libsfcdebug.dylib /usr/local/lib

util.o: util.c util.h
	$(CC) $(CFLAGS) -c util.c -o util.o

libsfcdebug.o: sfcdebug.c linkregs.h
	$(CC) $(CFLAGS) -c sfcdebug.c -o sfcdebug.o
clean:
	rm -rf sfcdebug.o libsfcdebug.dylib

CC=g++
CFLAGS=-Wall

all: debuglib

debuglib: sfcdebug.o
	g++ -dynamiclib -o libsfcdebug.dylib sfcdebug.o 
	cp libsfcdebug.dylib /usr/local/lib

libsfcdebug.o: sfcdebug.c
	$(CC) $(CFLAGS) -c sfcdebug.c -o sfcdebug.o
clean:
	rm -rf sfdebug.o libsfcdebug.dylib

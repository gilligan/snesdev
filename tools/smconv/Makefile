CXX=g++
CFLAGS=-O2

all: smconv

SRC=$(wildcard *.cpp)
OBJ=$(SRC:.cpp=.o)

smconv: $(OBJ)
	$(CXX) -o$@ $(OBJ)

clean:
	rm -rf *.o smconv


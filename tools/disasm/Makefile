CC = gcc
CFLAGS = -g

SOURCE = comments.c dcb.c main.c misc.c symbol.c types.c
OBJS = comments.o dcb.o main.o misc.o symbol.o types.o
DEFINES = -DUNIX
LIBS = 

disasm: $(OBJS)
	$(CC) $(CFLAGS) -o disasm $(OBJS) $(DEFINES) $(LIBS)
clean:
	rm -f *.o disasm
tar:
	tar -cvf disasm.tar Makefile *.c *.h

comments.o: proto.h
dcb.o: proto.h
main.o: proto.h disasm.h
misc.o: 
symbol.o: proto.h
types.o: proto.h


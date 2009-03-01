CC=gcc
LD=gcc
CFLAGS=-Wall -g -O2 `libpng-config --cflags` `pkg-config glib-2.0 --cflags` --std=c99
LDFLAGS=`libpng-config --libs` `pkg-config glib-2.0 --libs`

OBJS= main.o read_image.o

pngtosnes: main.c ${OBJS}
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ ${OBJS}

clean:
	rm -f *.o pngtosnes

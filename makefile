TR=biditex
SRC=bidi.c  biditex.c  io.c
OBJ := $(patsubst %.c,%.o,$(SRC))


LIBS = `fribidi-config --libs`
CC = gcc
CFLAGS = -O2 -Wall -g `fribidi-config --cflags`


all: $(TR)

$(TR) : $(OBJ)
	$(CC) -o $@ $^ $(LIBS)

.c.o:
	$(CC) -c $(CFLAGS) $<

clean:
	rm -f $(OBJ) $(TR) .depend

depend:
	$(CC) -M $(SRC) > .depend

-include .depend



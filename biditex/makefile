TR=biditex
SRC=bidi.c  biditex.c  io.c util.c
OBJ := $(patsubst %.c,%.o,$(SRC))
INSTPATH=/usr/local/bin/


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

install: $(TR)
	cp $(TR) $(INSTPATH)

uninstall:
	rm "$(INSTPATH)$(TR)"

-include .depend

TR=biditex
SRC=bidi.c biditex.c dict.c io.c util.c ignore.c
OBJ := $(patsubst %.c,%.o,$(SRC))
MAN=biditex.1
INSTPATH=/usr/local
BINPATH=$(INSTPATH)/bin/
MANPATH=$(INSTPATH)/share/man/man1/

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
	cp $(TR) $(BINPATH)
	cp ../docs/man/$(MAN) $(MANPATH)

uninstall:
	rm -f "$(BINPATH)$(TR)"
	rm -f "$(MANPATH)$(MAN)"

-include .depend

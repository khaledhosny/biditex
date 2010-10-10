TR=biditex
SRC=bidi.c biditex.c dict.c io.c util.c ignore.c
OBJ := $(patsubst %.c,%.o,$(SRC))
MAN=biditex.1
INSTPATH=/usr/local
TEXMF=/usr/share/texmf
BINPATH=$(INSTPATH)/bin/
MANPATH=$(INSTPATH)/share/man/man1/

LIBS = $(FRIBIDI_LIBS) -lfribidi
CC = gcc
CFLAGS = $(FRIBIDI_CFLAGS) -O2 -Wall -g 


all: $(TR)

all_static: $(OBJ)
	$(CC) -o $(TR) $^ /usr/lib/libfribidi.a

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
	mkdir -p $(MANPATH)
	cp ../docs/man/$(MAN) $(MANPATH)
	mkdir -p $(TEXMF)/tex/latex/biditex
	cp biditex.sty $(TEXMF)/tex/latex/biditex/
	mktexlsr

uninstall:
	rm -f "$(BINPATH)$(TR)"
	rm -f "$(MANPATH)$(MAN)"
	rm -fr $(TEXMF)/tex/latex/biditex
	mktexlsr


-include .depend

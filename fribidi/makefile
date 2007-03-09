TR=libfribidi.a

SRC=fribidi_char_sets_cap_rtl.c fribidi_char_sets_cp1255.c fribidi_char_sets_cp1256.c fribidi_char_sets_isiri_3342.c fribidi_char_sets_iso8859_6.c fribidi_char_sets_iso8859_8.c fribidi_char_sets.c fribidi_char_sets_utf8.c fribidi_char_type.c fribidi_mem.c fribidi_mirroring.c fribidi.c fribidi_types.c fribidi_utils.c wcwidth.c

OBJ := $(patsubst %.c,%.o,$(SRC))


LIBS = 
CC = i586-mingw32msvc-gcc
AR=i586-mingw32msvc-ar
RANLIB=i586-mingw32msvc-ranlib
CFLAGS = -O2 -Wall -DHAS_FRIBIDI_TAB_CHAR_TYPE_9_I -DHAS_FRIBIDI_TAB_CHAR_TYPE_2_I


all:  $(TR)

$(TR): $(OBJ)
	$(AR) rvu $(TR) $(OBJ)
	$(RANLIB) $(TR)

.c.o:
	$(CC) -c $(CFLAGS) $<

clean:
	rm -f $(OBJ) $(TR)



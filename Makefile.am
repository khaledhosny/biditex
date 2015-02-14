NULL =

dist_man_MANS = \
  docs/man/biditex.1 \
  $(NULL)

bin_PROGRAMS = \
  biditex \
  $(NULL)

biditex_SOURCES = \
  src/bidi.c \
  src/biditex.c \
  src/dict.c \
  src/io.c \
  src/util.c \
  src/ignore.c \
  $(NULL)

AM_CPPFLAGS = \
  $(FRIBIDI_CFLAGS) \
  $(NULL)

LDADD = \
  $(FRIBIDI_LIBS) \
  $(NULL)

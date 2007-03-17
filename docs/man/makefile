all: README.TXT

README.TXT: biditex.1
	man ./biditex.1 | col | unix2dos > README.TXT || rm -f README.TXT

clean:
	rm -f README.TXT

INSTPATH=/usr/local

all:
	cd biditex ; make
	cd docs/biditex-doc ; make

clean:
	cd biditex ; make clean
	cd docs/biditex-doc ; make clean
	cd docs/man ; make clean

install:
	cp biditex/biditex $(INSTPATH)/bin
	cp docs/man/biditex.1 $(INSTPATH)/share/man/man1
	mkdir -p $(INSTPATH)/share/doc/biditex
	mkdir -p $(INSTPATH)/share/doc/biditex/example
	cp docs/biditex-doc/biditex-doc.pdf $(INSTPATH)/share/doc/biditex
	cp docs/copyright  $(INSTPATH)/share/doc/biditex
	cp docs/example/example.tex $(INSTPATH)/share/doc/biditex/example
	cp docs/example/makefile $(INSTPATH)/share/doc/biditex/example


uninstall:
	rm -f $(INSTPATH)/bin/biditex
	rm -f $(INSTPATH)/share/man/man1/biditex.1
	rm -f $(INSTPATH)/share/doc/biditex/example/example.tex
	rm -f $(INSTPATH)/share/doc/biditex/example/makefile
	rmdir $(INSTPATH)/share/doc/biditex/example
	rm -f $(INSTPATH)/share/doc/biditex/biditex-doc.pdf
	rm -f $(INSTPATH)/share/doc/biditex/copyright
	rmdir $(INSTPATH)/share/doc/biditex

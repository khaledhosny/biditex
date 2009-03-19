ifeq ($(INSTPATH),)
	INSTPATH=$(DESTDIR)/usr
endif

TEXMF=$(DESTDIR)/usr/share/texmf

all:
	cd biditex ; make
	cd docs/biditex-doc ; make

all_static:
	cd biditex ; make all_static
	cd docs/biditex-doc ; make
	

clean:
	cd biditex ; make clean
	cd docs/biditex-doc ; make clean
	cd docs/man ; make clean

instdeb:
	INSTPATH=/usr make installfiles

uninstdeb:
	INSTPATH=/usr make uninstallfiles

installfiles:
	mkdir -p $(INSTPATH)/bin
	mkdir -p $(INSTPATH)/share/doc/biditex/example
	mkdir -p $(INSTPATH)/share/man/man1
	cp -p biditex/biditex $(INSTPATH)/bin
	cp -p docs/man/biditex.1 $(INSTPATH)/share/man/man1
	mkdir -p $(INSTPATH)/share/doc/biditex
	mkdir -p $(INSTPATH)/share/doc/biditex/example
	cp -p docs/biditex-doc/biditex-doc.pdf $(INSTPATH)/share/doc/biditex
	cp -p docs/copyright  $(INSTPATH)/share/doc/biditex
	cp -p docs/example/example.tex $(INSTPATH)/share/doc/biditex/example
	cp -p docs/example/makefile $(INSTPATH)/share/doc/biditex/example
	mkdir -p $(TEXMF)/tex/latex/biditex
	cp -p biditex/biditex.sty $(TEXMF)/tex/latex/biditex/

install: installfiles
	mktexlsr

uninstallfiles:
	rm -f $(INSTPATH)/bin/biditex
	rm -f $(INSTPATH)/share/man/man1/biditex.1
	rm -fr $(INSTPATH)/share/doc/biditex
	rm -fr $(TEXMF)/tex/latex/biditex

uninstall: uninstallfiles
	mktexlsr


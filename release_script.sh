#!/bin/bash

if [ "$1" == "" ]; then
	echo usage version 
	echo    for example ./releasescript 0.0.5
	exit 1;
fi
VER="$1"

dpkg -P biditex
rm biditex_*.tar.gz
rm -f *w32.zip *.deb *.rpm
rm -f backup-*-pre-biditex.tgz

make clean
make -f makefile.win32 clean

make
checkinstall -D --pkgname=biditex --pkgversion=$VER \
	--pkglicense=GPL --pkggroup=text --nodoc  \
	--install=no \
	'--maintainer=artyomtnk@yahoo.com' \
	make instdeb

alien --scripts --to-rpm biditex_$VER-1_*.deb


#backup docs before clean
cp docs/biditex-doc/biditex-doc.pdf .

# make win32 release

make clean
make -f makefile.win32
make -f makefile.win32 zip

#  cleanup
rm biditex-doc.pdf
rm -f backup*.tgz
make -f makefile.win32 clean

svn export . biditex_$VER

tar -czvf biditex_$VER.tar.gz biditex_$VER

rm -fr biditex_$VER

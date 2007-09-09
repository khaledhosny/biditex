#!/bin/bash

if [ "$1" == "" ]; then
	echo usage version 
	echo    for example ./releasescript 0.0.5
	exit 1;
fi
VER="$1"

dpkg -P biditex
rm biditex_*.deb
rm biditex_*.rpm
rm biditex_*.tar.gz
rm biditex-w32.zip

make clean
make -f makefile.win32 clean

make
checkinstall --pkgname=biditex --pkgversion=$VER \
	--pkglicense=GPL --pkggroup=text --nodoc  \
	--install=no \
	"--maintainer=Artyom Tonkikh (artyomtnk@yahoo.com)" \
	make instdeb

alien --to-rpm biditex_$VER-1_i386.deb

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

tar -Xexcl.txt -czvvf biditex_$VER.tar.gz .

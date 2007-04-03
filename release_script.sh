#!/bin/bash

VER="0.0.5"

sudo dpkg -P biditex

make clean
make -f makefile.win32 clean

make
sudo checkinstall --pkgname=biditex --pkgversion=$VER \
	--pkglicense=GPL --pkggroup=text --nodoc  \
	"--maintainer=Artyom Tonkikh (artyomtnk@yahoo.com)"

sudo alien --to-rpm biditex_$VER-1_i386.deb

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

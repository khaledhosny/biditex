#!/bin/bash

if [ "$1" == "" ]; then
	echo usage version 
	echo    for example ./releasescript 0.0.5
	exit 1;
fi
VER="$1"

if [ "`dpkg -l biditex | awk '/biditex/{print $1}'`" == "ii" ] ; then
	echo First remove biditex package
	exit 1
fi

rm biditex_*.tar.gz
rm -f *win32.zip *.deb *.rpm
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

mv biditex-win32.zip biditex_$VER-win32.zip

svn export . biditex_$VER

tar -czvf biditex_$VER.tar.gz biditex_$VER

rm -fr biditex_$VER

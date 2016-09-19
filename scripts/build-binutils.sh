#!/bin/sh

ROOTDIR=$(pwd)/root

cd $ROOTDIR/sys/src

wget https://github.com/himanshugoel2797/Cardinal-binutils-2.26.1/archive/master.zip -O binutils.zip
unzip binutils.zip -d .

mkdir -p build-binutils
cd build-binutils

../Cardinal-binutils-2.26.1-master/configure --target=x86_64-elf-cardinal --prefix="$ROOTDIR/sys" --with-sysroot=$ROOTDIR --disable-nls --disable-werror
make -j2
make install
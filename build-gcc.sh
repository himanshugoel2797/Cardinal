#!/bin/sh

ROOTDIR=$(pwd)/root

cd $ROOTDIR/sys/src

#First build a bootstrap musl
wget https://github.com/himanshugoel2797/Cardinal-musl/archive/master.zip -O musl.zip
unzip musl.zip -d .

cd Cardinal-musl-master
./configure --disable-shared --prefix=$ROOTDIR/sys
make -j4 all
make install-header

#Next build and install binutils
cd ..

wget https://github.com/himanshugoel2797/Cardinal-binutils-2.26.1/archive/master.zip -O binutils.zip
unzip binutils.zip -d .

mkdir -p build-binutils
cd build-binutils

../Cardinal-binutils-2.26.1-master/configure --target=x86_64-elf-cardinal --prefix="$ROOTDIR/sys" --with-sysroot=$ROOTDIR --disable-nls --disable-werror
make -j2
make install

#Build a bootstrap gcc
cd ..

wget https://github.com/himanshugoel2797/Cardinal-gcc-5.4.0/archive/master.zip -O gcc.zip
unzip gcc.zip -d .

mkdir -p build-gcc
cd build-gcc

../Cardinal-gcc-5.4.0-master/configure --target=x86_64-elf-cardinal --prefix="$ROOTDIR/sys" --with-sysroot=$ROOTDIR --enable-languages=c,c++
make all-gcc
make all-target-libgcc
make install-gcc
make install-target-libgcc

#Go back and rebuild musl with the new compiler
cd ../Cardinal-musl-master
rm -rf *
./configure --disable-shared --prefix=$ROOTDIR/sys CC=$ROOTDIR/sys/bin/x86_64-elf-cardinal-gcc
make -j4 all
make install

#Now build libstdc++-v3 for C++ support
cd ../build-gcc
make all-target-libstdc++-v3
make install-target-libstdc++-v3
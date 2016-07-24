#!/bin/sh

SRC_DIR=$(pwd)
ROOTDIR=$(pwd)/root

mkdir root
cd root

mkdir sys
mkdir usr
mkdir home
mkdir dev
mkdir proc

cd sys
mkdir src
cd src
mkdir musl

wget https://github.com/himanshugoel2797/Cardinal-musl/archive/master.zip -O musl.zip
unzip musl.zip -d .

cd Cardinal-musl-master
./configure --disable-shared --prefix=$ROOTDIR/sys
make -j4 all
make install
cd ..

cd $ROOTDIR/sys/include
cp $SRC_DIR/libs/libCardinal/include/* .

cd ..
cd ..
cd ..

qemu-img create disk.img 128M
mke2fs -L "HDD0" -o "Cardinal" disk.img 
#Figure out how to mount and write the root to the disk image
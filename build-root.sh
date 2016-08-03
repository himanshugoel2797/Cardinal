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


wget https://github.com/himanshugoel2797/Cardinal_directory_server/archive/master.zip -O dir_server.zip
unzip dir_server.zip -d .

cd Cardinal_directory_server-master
make -j4 all
make install
cd ..

cd $ROOTDIR/sys/include
mkdir cardinal
cd cardinal
cp $SRC_DIR/libs/libCardinal/include/* .

cd ..
cd ..
cd ..

qemu-img create disk.img 128M
mke2fs -L "HDD0" -o "Cardinal" disk.img 
#Figure out how to mount and write the root to the disk image
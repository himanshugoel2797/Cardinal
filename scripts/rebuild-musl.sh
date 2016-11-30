#!/bin/sh

SRC_DIR=$(pwd)
ROOTDIR=$(pwd)/root

mkdir -p $ROOTDIR/sys/include/cardinal
cp $SRC_DIR/libs/libCardinal/include/* $ROOTDIR/sys/include/cardinal

cd $ROOTDIR/sys/src/Cardinal-musl-master

make clean
make -j4 all
make install
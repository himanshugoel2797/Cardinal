#!/bin/sh

SRC_DIR=$(pwd)
ROOTDIR=$(pwd)/root

cp $SRC_DIR/libs/libCardinal/include/* $ROOTDIR/sys/include/cardinal/*

cd $ROOTDIR/sys/src/Cardinal-musl-master

make -j4 all
make install
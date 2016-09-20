#!/bin/sh

SRC_DIR=$(pwd)
ROOTDIR=$(pwd)/root

SYS_BIN_DIR=$ROOTDIR/sys/bin

cd utils

make clean
make all
make install
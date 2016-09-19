#!/bin/sh

SRC_DIR=$(pwd)
ROOTDIR=$(pwd)/root

INITRD_INST_DIR=$SRC_DIR/initrd
SYS_SRC_DIR=$ROOTDIR/sys/src
INITRD_DIR=$SYS_SRC_DIR/initrd

cd $INITRD_DIR
for D in *; do [ -d "${D}" ] && (cd $D && git pull origin master && make clean && make all && cp *.elf $INITRD_INST_DIR && cd ..); done
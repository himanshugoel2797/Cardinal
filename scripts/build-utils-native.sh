#!/bin/sh

SRC_DIR=$(pwd)
ROOTDIR=$(pwd)/root

SYS_BIN_DIR=$ROOTDIR/sys/bin

cd utils

make clean
CC=x86_64-elf-cardinal-gcc make all

mv *.elf $SYS_BIN_DIR
cd $SYS_BIN_DIR
rm Makefile ##Makefile gets copied over for some reason, so delete the copy

for f in *.elf; do mv "$f" "${f%.elf}"; done
#!/bin/sh

SRC_DIR=$(pwd)
ROOTDIR=$(pwd)/root

cd $SRC_DIR/libs/libCardinal
	make uninstall 
	make clean
	make all
	make install
cd ..

cd $SRC_DIR/libs/libfileserver
	make uninstall
	make clean
	make all
	make install
cd ..

cd $SRC_DIR/libs/liblist
	make uninstall 
	make clean
	make all
	make install
cd ..

cd $SRC_DIR/libs/libdisplay
	make uninstall 
	make clean
	make all
	make install
cd ..

cd $SRC_DIR/libs/libpci
	make uninstall 
	make clean
	make all
	make install
cd ..
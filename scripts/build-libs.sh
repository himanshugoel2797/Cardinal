#!/bin/sh

SRC_DIR=$(pwd)
ROOTDIR=$(pwd)/root

mkdir -p $ROOTDIR/sys/include/cardinal
cp $SRC_DIR/libs/libCardinal/include/* $ROOTDIR/sys/include/cardinal

cd $SRC_DIR/libs/libfileserver
	make uninstall
	make clean
	make all
	make install
cd ..

cd $SRC_DIR/libs/libio
	make uninstall
	make clean
	make all
	make install
cd ..

cd $SRC_DIR/libs/libipc
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

cd $SRC_DIR/srv/mem_srv/lib
	make uninstall 
	make clean
	make all
	make install
cd ..

cd $SRC_DIR/srv/proc_srv/lib
	make uninstall 
	make clean
	make all
	make install
cd ..

cd $SRC_DIR/srv/namespace_dir/lib
	make uninstall 
	make clean
	make all
	make install
cd ..
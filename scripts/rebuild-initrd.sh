#!/bin/sh

SRC_DIR=$(pwd)
ROOTDIR=$(pwd)/root

INITRD_INST_DIR=$SRC_DIR/initrd

cd $INITRD_INST_DIR
rm -rf *.elf

cd $SRC_DIR/sys_init 
	make clean
	make all
	make install INSTALL_DIR=$INITRD_INST_DIR

cd $SRC_DIR/srv

cd namespace_man
	make clean
	make all
	make install
	mv *.elf $INITRD_INST_DIR 
cd ..

cd perm_man
	make clean
	make all
	make install
	mv *.elf $INITRD_INST_DIR
cd ..

cd root_srv
	make clean
	make all
	make install
	mv *.elf $INITRD_INST_DIR
cd ..

cd shmem
	make clean
	make all
	make install
	mv *.elf $INITRD_INST_DIR
cd ..
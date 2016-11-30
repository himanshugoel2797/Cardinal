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

cd $SRC_DIR/userboot 
	make clean
	make all
	make install INSTALL_DIR=$INITRD_INST_DIR

cd $SRC_DIR/srv

cd elf_srv
	make clean
	make all
	make install INSTALL_DIR=$INITRD_INST_DIR
cd ..

cd namespace_dir
	make clean
	make all
	make install INSTALL_DIR=$INITRD_INST_DIR
	mv *.elf $INITRD_INST_DIR 
cd ..

cd proc_srv
	make clean
	make all
	make install INSTALL_DIR=$INITRD_INST_DIR
	mv *.elf $INITRD_INST_DIR
cd ..

cd mem_srv
	make clean
	make all
	make install INSTALL_DIR=$INITRD_INST_DIR
	mv *.elf $INITRD_INST_DIR
cd ..
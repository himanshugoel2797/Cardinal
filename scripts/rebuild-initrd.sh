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

cd sibyl
	make clean
	make all
	make install INSTALL_DIR=$INITRD_INST_DIR
	mv *.elf $INITRD_INST_DIR
cd ..

cd $SRC_DIR/srv/initrd_srv
	make clean
	make all
	cp initrd_srv.elf $INITRD_INST_DIR
cd $SRC_DIR

cd $SRC_DIR/drivers/display/framebuffer
	make clean
	make all
	cp *.elf $INITRD_INST_DIR
cd $SRC_DIR

cd $SRC_DIR/drivers/display/vmware-svga
	make clean
	make all
	cp *.elf $INITRD_INST_DIR
cd $SRC_DIR

cd $SRC_DIR/drivers/display/ihd
	make -f targets/ilk clean
	make -f targets/hsw clean
	make -f targets/ilk all
	make -f targets/hsw all

	cp *.elf $INITRD_INST_DIR
cd $SRC_DIR

cd $SRC_DIR/drivers/pci_loader
	make clean
	make all
	cp *.elf $INITRD_INST_DIR
cd $SRC_DIR

cd $SRC_DIR/drivers/network/rtl8139
	make clean
	make all
	cp *.elf $INITRD_INST_DIR
cd $SRC_DIR

cd $SRC_DIR/drivers/network/rtl8168
	make clean
	make all
	cp *.elf $INITRD_INST_DIR
cd $SRC_DIR

cd $SRC_DIR/drivers/usb/uhci
	make clean
	make all
	cp *.elf $INITRD_INST_DIR
cd $SRC_DIR

cd $SRC_DIR/drivers/ahci
	make clean
	make all
	cp *.elf $INITRD_INST_DIR
cd $SRC_DIR

cd $SRC_DIR/drivers/audio/ihda
	make clean
	make all
	cp *.elf $INITRD_INST_DIR
cd $SRC_DIR

cp $SRC_DIR/drivers/pci_devices.txt $SRC_DIR/initrd/pci_devices.txt
cp wallpaper.png $SRC_DIR/initrd/wallpaper.png
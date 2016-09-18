#!/bin/sh

BASE_URL=git@github.com:himanshugoel2797

SRC_DIR=$(pwd)
ROOTDIR=$(pwd)/root

SYS_SRC_DIR=$ROOTDIR/sys/src
INITRD_DIR=$SYS_SRC_DIR/initrd

cd $SYS_SRC_DIR
mkdir -p initrd
cd $INITRD_DIR

#clone projects, then start the rebuild script
git clone $BASE_URL/Cardinal_directory_server.git
git clone $BASE_URL/Cardinal_elf_loader.git
git clone $BASE_URL/Cardinal_system_init.git
git clone $BASE_URL/Cardinal_shmem_server.git

sh $SRC_DIR/rebuild-initrd.sh
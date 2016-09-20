#!/bin/sh

SRC_DIR=$(pwd)
ROOTDIR=$(pwd)/root

#Build the sysroot
./built-root.sh

#Build the cross compiler
./build-gcc.sh

export PATH="$ROOTDIR/sys/bin":$PATH

./build-utils.sh

./build-servers.sh

./build-initrd.sh

./build-utils-native.sh

#Build the kernel
make all
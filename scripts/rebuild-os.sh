#!/bin/sh

SRC_DIR=$(pwd)
ROOTDIR=$(pwd)/root

./rebuild-musl.sh

./rebuild-utils.sh

./rebuild-servers.sh

./rebuild-initrd.sh

./build-utils-native.sh

#Build the kernel
make all
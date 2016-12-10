#!/bin/sh

SRC_DIR=$(pwd)
ROOTDIR=$(pwd)/root

rm -rf $SRC_DIR/libc_pack
mkdir $SRC_DIR/libc_pack

cd $ROOTDIR/sys/lib

cp libio.a $SRC_DIR/libc_pack
cp libipc.a $SRC_DIR/libc_pack
cp libmemserver.a $SRC_DIR/libc_pack
cp libnamespaceserver.a $SRC_DIR/libc_pack

cd $SRC_DIR/libc_pack

ar -x libio.a
ar -x libipc.a
ar -x libmemserver.a
ar -x libnamespaceserver.a

ar -qc libcardcore.a *.o

cp libcardcore.a $ROOTDIR/sys/lib

cd $SRC_DIR

rm -rf libc_pack
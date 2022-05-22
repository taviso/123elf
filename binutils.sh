#!/bin/bash

BINUTILS_XZ=binutils-2.34.tar.xz 
BINUTILS_DIR=`basename $BINUTILS_XZ .tar.xz`
BINUTILS_URL=https://ftp.gnu.org/gnu/binutils/$BINUTILS_XZ
ORIG_PWD=`pwd`

if [ ! -f "$BINUTILS_XZ" ]; then
   wget "$BINUTILS_URL"
fi

if [ ! -d "$BINUTILS_DIR" ]; then
   tar -xvf "$BINUTILS_XZ"
fi

if [ ! -x "$BINUTILS_DIR/binutils/objcopy" ]; then
   cd $BINUTILS_DIR
   ./configure --enable-targets=all
   make
fi
cd $ORIG_PWD

for b in $BINUTILS_DIR/binutils/{objcopy,objdump} $BINUTILS_DIR/ld/ld-new; do
   DEST_NAME=`basename $b | sed 's/-new//g'`

   if [ ! -x $DEST_NAME ]; then
      cp -v "$b" "$DEST_NAME"
   fi
done


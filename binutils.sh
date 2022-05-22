#!/bin/bash

BINUTILS_XZ=binutils-2.38.tar.xz 
BINUTILS_DIR=`basename $BINUTILS_XZ .tar.xz`
BINUTILS_URL=https://ftp.gnu.org/gnu/binutils/$BINUTILS_XZ
ORIG_PWD=`pwd`

# Help text.
if [ "$1" = "-h" ]; then
   echo "Usage: $0 [clean]"
   echo
   echo "Specify \"clean\" to remove binutils, otherwise it will be downloaded and built."
   exit
fi

# Optional cleanup if requested.
if [ "$1" = "clean" ]; then
   rm -fv objcopy objdump ld $BINUTILS_XZ
   rm -rfv $BINUTILS_DIR
   exit
fi

# Download binutils.
if [ ! -f "$BINUTILS_XZ" ]; then
   wget "$BINUTILS_URL"
fi

# Extract binutils.
if [ ! -d "$BINUTILS_DIR" ]; then
   tar -xvf "$BINUTILS_XZ"
fi

# Compile binutils.
if [ ! -x "$BINUTILS_DIR/binutils/objcopy" ]; then
   cd $BINUTILS_DIR
   ./configure --enable-targets=all
   make
fi
cd $ORIG_PWD

# Copy compiled binaries to current directory.
for b in $BINUTILS_DIR/binutils/{objcopy,objdump} $BINUTILS_DIR/ld/ld-new; do
   DEST_NAME=`basename $b | sed 's/-new//g'`

   if [ ! -x $DEST_NAME ]; then
      cp -v "$b" "$DEST_NAME"
   fi
done


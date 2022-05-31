#!/bin/sh

BINUTILS_DL="https://ftp.gnu.org/gnu/binutils"
BINUTILS_XZ="binutils-2.38.tar.xz"
BINUTILS_DIR="$(basename $BINUTILS_XZ .tar.xz)"
BINUTILS_URL="$BINUTILS_DL/$BINUTILS_XZ"
ORIG_DIR="$(pwd)"

# Dont continue on error.
set -e

# Help text.
if [ "$1" = '-h' ]; then
   echo >&2 "Usage: $0 [clean]"
   echo >&2
   echo >&2 'Specify "clean" to remove binutils, otherwise it will be downloaded and built.'
   exit 1
fi

# Optional cleanup if requested.
if [ "$1" = 'clean' ]; then
   rm -fv objcopy objdump ld "$BINUTILS_XZ"
   rm -rfv "$BINUTILS_DIR"
   exit
fi

# Download binutils.
if [ ! -f "$BINUTILS_XZ" ]; then
   wget "$BINUTILS_URL"
fi

# Extract binutils.
if [ ! -d "$BINUTILS_DIR" ]; then
   tar xf "$BINUTILS_XZ"
fi

# Compile binutils.
if [ ! -x "$BINUTILS_DIR/binutils/objcopy" ]; then
   cd "$BINUTILS_DIR"
   ./configure --enable-targets=all \
               --enable-ld=default \
               --disable-plugins \
               --disable-werror --disable-nls \
               --disable-gas --disable-libctf --disable-gprof \
               --with-system-zlib
   make all-ld -j$(nproc) MAKEINFO=true
fi

# Copy compiled binaries to working directory.
copy() {
   test ! -x "$2" && cp -v "$1" "$2"
}

cd "$ORIG_DIR"
copy "$BINUTILS_DIR/binutils/objcopy" objcopy
copy "$BINUTILS_DIR/binutils/objdump" objdump
copy "$BINUTILS_DIR/ld/ld-new" ld

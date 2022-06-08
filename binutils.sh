#!/bin/sh

BINUTILS_DL="https://ftp.gnu.org/gnu/binutils"
BINUTILS_XZ="binutils-2.38.tar.xz"
BINUTILS_DIR="$(basename $BINUTILS_XZ .tar.xz)"
BINUTILS_URL="$BINUTILS_DL/$BINUTILS_XZ"
ORIGIN_DIR="$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)"

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
    rm -fv "$ORIGIN_DIR/objcopy" "$ORIGIN_DIR/objdump" "$ORIGIN_DIR/ld" "$ORIGIN_DIR/as"
    rm -rfv "$BINUTILS_DIR"
    rm -fv "$ORIGIN_DIR/$BINUTILS_XZ"
    exit
fi

# Download binutils.
if [ ! -f "$BINUTILS_XZ" ]; then
    wget "$BINUTILS_URL"
fi

# Extract binutils.
if [ ! -d "$BINUTILS_DIR" ]; then
    xzcat "$BINUTILS_XZ" | tar xf -
fi

# Compile binutils.
if [ ! -x "$BINUTILS_DIR/binutils/objcopy" ]; then
    cd "$BINUTILS_DIR" && \
    ./configure --enable-targets=i386-pc-elf32 \
                --disable-libctf \
                --disable-plugins \
                --disable-gprof \
                --enable-compressed-debug-sections=none && \
    gmake all-ld all-gas MAKEINFO=true
    cd "$ORIGIN_DIR"
fi

# Copy compiled binaries to working directory.
copy() {
    test ! -x "$2" && cp -v "$1" "$2"
}

copy "$BINUTILS_DIR/binutils/objcopy" "$ORIGIN_DIR/objcopy"
copy "$BINUTILS_DIR/binutils/objdump" "$ORIGIN_DIR/objdump"
copy "$BINUTILS_DIR/ld/ld-new" "$ORIGIN_DIR/ld"
copy "$BINUTILS_DIR/gas/as-new" "$ORIGIN_DIR/as"

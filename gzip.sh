#!/bin/sh
#
# This script is only necessary if you have a *very* old gzip.
#
GZIP_DL="https://ftp.gnu.org/gnu/gzip"
GZIP_XZ="gzip-1.12.tar.xz"
GZIP_DIR="$(basename $GZIP_XZ .tar.xz)"
GZIP_URL="$GZIP_DL/$GZIP_XZ"
ORIG_DIR="$(pwd)"

# Dont continue on error.
set -e

# Help text.
if [ "$1" = '-h' ]; then
   echo >&2 "Usage: $0 [clean]"
   echo >&2
   echo >&2 'Specify "clean" to remove gzip, otherwise it will be downloaded and built.'
   exit 1
fi

# Optional cleanup if requested.
if [ "$1" = 'clean' ]; then
   rm -fv gzip gunzip zcat "$GZIP_XZ"
   rm -rfv "$GZIP_DIR"
   exit
fi

# Download gzip.
if [ ! -f "$GZIP_XZ" ]; then
   wget "$GZIP_URL"
fi

# Extract gzip.
if [ ! -d "$GZIP_DIR" ]; then
   tar xf "$GZIP_XZ"
fi

# Compile gzip.
if [ ! -x "$GZIP_DIR/gzip" ]; then
   cd "$GZIP_DIR"
   ./configure
   make -j$(nproc)
fi

# Copy compiled binaries to working directory.
copy() {
   test ! -x "$2" && cp -v "$1" "$2"
}

cd "$ORIG_DIR"
copy "$GZIP_DIR/gzip" gzip
copy "$GZIP_DIR/gunzip" gunzip
copy "$GZIP_DIR/zcat" zcat

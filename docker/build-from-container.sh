#!/bin/bash
## build-from-container.sh
## @author gdm85
##
## This is the script run from within the container to perform the necessary setup steps
## to build the '123' Linux-compatible binary.
##
#
set -e

## compile custom binutils if installed version does not support coff-i386
if ! make check; then
    BU_VER=2.38
    BU_ARC=binutils-${BU_VER}.tar.xz
    wget --continue "https://ftp.gnu.org/gnu/binutils/$BU_ARC"
    tar xf "$BU_ARC"
    rm "$BU_ARC"
    cd "binutils-${BU_VER}"
    ./configure --enable-targets=all
    make -j
    cp ld/ld-new ../ld
    cp binutils/objcopy binutils/objdump ..

    cd ..
    make check
    rm -rf "binutils-${BU_VER}"
fi

if ! test -f orig/123.o; then
    ## download IMG files if not already existing
    for I in 1 2 3 4 5; do
        wget --quiet --continue "https://archive.org/download/123-unix/123UNIX${I}.IMG"
    done

    ./extract.sh
fi

exec make

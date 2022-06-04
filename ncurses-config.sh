#!/bin/sh
nclibs="-lncurses"
if command -v ncurses6-config > /dev/null; then
    nclibs="$(ncurses6-config --libs-only-l)"
elif command -v pkg-config > /dev/null; then
    pkgcfglibs="$(pkg-config --libs ncurses)"
    if [ -n "$pkgcfglibs" ]; then
        nclibs="$pkgcfglibs"
    fi
fi
echo "$nclibs"

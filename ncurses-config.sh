#!/bin/sh
nclibs="-lncurses"
if command -v ncurses6-config > /dev/null; then
    nclibs="$(ncurses6-config --libs)"
elif command -v pkg-config > /dev/null; then
    pkgcfglibs="$(pkg-config --libs ncurses 2>/dev/null)"
    if [ "$?" -eq 0 -a -n "$pkgcfglibs" ]; then
        nclibs="$pkgcfglibs"
    fi
fi
echo "$nclibs"

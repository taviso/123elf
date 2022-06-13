#!/bin/sh

file_undefine=undefine.lst
file_redefine=redefine.lst
file_localize=localize.lst
file_globalize=globalize.lst

if [ "$#" -lt 1 ]; then
    echo >&2 "Usage: $0 <undefine|redefine|localize|globalize>"
    exit 1
fi

platform="$(uname -s)"

if [ "$platform" = "FreeBSD" ]; then
    file_undefine=undefine.freebsd
fi

if [ "$platform" = "NetBSD" ]; then
    file_undefine=undefine.netbsd
fi

if [ "$platform" = "OpenBSD" ]; then
    file_undefine=undefine.openbsd
fi

if [ "$1" = "undefine" ]; then
    echo "$file_undefine"
 elif [ "$1" = "redefine" ]; then
    echo "$file_redefine"
 elif [ "$1" = "localize" ]; then
    echo "$file_localize"
 elif [ "$1" = "globalize" ]; then
    echo "$file_globalize"
 else
    echo >&2 "Usage: $0 <undefine|redefine|localize|globalize>"
    exit 1
fi

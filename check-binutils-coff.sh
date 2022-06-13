#!/bin/sh

BASE="$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)"
PATH="${BASE}:$PATH"

BFD_TARGET_ERROR="Your version of binutils was compiled without coff-i386 target support.
You can try running ./binutils.sh to build a version that does support it."

objdump --info | grep -q '^coff-i386$' || (echo "$BFD_TARGET_ERROR"; false)

#!/bin/sh
#
# Extract the necessary files from Lotus 1-2-3 UNIX
#
BASE="$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)"
IMG="${BASE}"
ROOT="${BASE}/share"
ORIG="${BASE}/orig"
ETC="${ROOT}/lotus/etc"
LOTUS="${ROOT}/lotus/123.v10"

CPIO="$(command -v cpio)"
TAR="$(command -v tar)"

if [ "$1" = 'clean' ]; then
    rm -rf "${ROOT}" "${ORIG}" "${ETC}"
    exit
fi

# Use any tools placed in this directory first.
export PATH="${BASE}:$PATH"

if ! test -f "${IMG}/123UNIX1.IMG"   \
       -a -f "${IMG}/123UNIX2.IMG"   \
       -a -f "${IMG}/123UNIX3.IMG"   \
       -a -f "${IMG}/123UNIX4.IMG"   \
       -a -f "${IMG}/123UNIX5.IMG"; then
    echo >&2 'You need to download the original 1-2-3 UNIX IMG files.'
    echo >&2 'They are available here: https://archive.org/download/123-unix'
    exit 1
fi

mkdir "${ROOT}"
mkdir "${ORIG}"

cd "${ROOT}"
echo '==> Extracting 123UNIX1.IMG tar archive'
"${TAR}" xvf "${IMG}/123UNIX1.IMG"
echo '==> Extracting 123UNIX2.IMG cpio archive'
"${CPIO}" -idv < "${IMG}/123UNIX2.IMG"
echo '==> Extracting 123UNIX3.IMG cpio archive'
"${CPIO}" -idv < "${IMG}/123UNIX3.IMG"
echo '==> Seeking into 123UNIX4.IMG to extract cpio archive'
dd if="${IMG}/123UNIX4.IMG" skip=1 bs=550536 | "${CPIO}" -idv
echo '==> Extracting 123UNIX5.IMG cpio archive'
"${CPIO}" -idv < "${IMG}/123UNIX5.IMG"
cd - > /dev/null

echo '==> Reconstructing object file'

if ! cat "${LOTUS}"/sysV386/lib/123.o.z_1 "${LOTUS}"/sysV386/lib/123.o.z_2 | zcat > "${ORIG}/123.o"; then
    echo >&2 'Failed to decompress object files.'
    echo >&2 'If you see the message "code out of range", gzip is too old.'
    echo >&2 'You can try running ./gzip.sh to build a recent gzip that is known to work.'
    exit 1
fi

echo '==> Uncompressing .z files'
find "${ROOT}" -iname '*.z' -exec gunzip --force {} \;

echo '==> Uncompressing and copying remaining object files'
cp "${LOTUS}"/sysV386/lib/*.o "${ORIG}/"

echo '==> Copying the banner template over'
install -m 644 "${ROOT}/usr/tmp/lotus_install/123/banner" "${LOTUS}/ri/USA-English/123ban.ri"

echo '==> Copying default config file'
install -m 755 -d "${ETC}"
install -m 644 "l123set.cf" "${ETC}/l123set.cf"

echo '==> Copying man page'
install -m 755 -d "${ROOT}/man/man1"
install -m 644 "${ROOT}/lotus/man/man1/123.1" "${ROOT}/man/man1/123.1"

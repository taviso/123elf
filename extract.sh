#!/bin/sh
#
# Extract the necessary files from Lotus 1-2-3 UNIX
#
BASE="$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)"
IMG="${BASE}"
ROOT="${BASE}/root"
ORIG="${BASE}/orig"
LOTUS="${ROOT}/lotus/123.v10"

CPIO="$(command -v cpio)"
TAR="$(command -v tar)"

if [ "$1" = 'clean' ]; then
    rm -rfv "${ROOT}" "${ORIG}"
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
echo "==> Extracting 123UNIX1.IMG tar archive"
"${TAR}" xvf "${IMG}/123UNIX1.IMG"
echo "==> Extracting 123UNIX2.IMG cpio archive"
"${CPIO}" -idv < "${IMG}/123UNIX2.IMG"
echo "==> Extracting 123UNIX3.IMG cpio archive"
"${CPIO}" -idv < "${IMG}/123UNIX3.IMG"
echo "==> Seeking into 123UNIX4.IMG to extract cpio archive"
dd if="${IMG}/123UNIX4.IMG" skip=550536 bs=1 | "${CPIO}" -idv
echo "==> Extracting 123UNIX5.IMG cpio archive"
"${CPIO}" -idv < "${IMG}/123UNIX5.IMG"
cd - > /dev/null

echo "==> Uncompressing .z files"
find "${ROOT}" -iname '*.z' -exec gunzip --force {} \;

echo "==> Uncompressing and copying object files"
cat "${LOTUS}"/sysV386/lib/123.o.z_1 "${LOTUS}"/sysV386/lib/123.o.z_2 | zcat > "${ORIG}/123.o"
cp "${LOTUS}"/sysV386/lib/*.o "${ORIG}/"

echo "==> Installing better keymaps"
for t in xterm xterm-256color; do
    cp -v "${BASE}/xterm" "${LOTUS}/keymaps/$(echo "${t}" | cut -c1)/${t}"
done
if [ ! -f "${LOTUS}/keymaps/$(echo "${TERM}" | cut -c1)/${TERM}" ]; then
    cp -v "${BASE}/xterm" "${LOTUS}/keymaps/$(echo "${TERM}" | cut -c1)/${TERM}"
fi

echo "==> Copying the banner template over"
cp -v "${ROOT}/usr/tmp/lotus_install/123/banner" "${LOTUS}/ri/USA-English/123ban.ri"

echo "==> Copying default configuration file"
cp -v "${BASE}/l123set.cf" "${ROOT}/lotus/l123set.cf"

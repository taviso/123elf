#!/bin/sh
#
# Extract the necessary files from Lotus 1-2-3 UNIX
#

ROOT="root"
ORIG="orig"
LOTUS="${ROOT}/lotus/123.v10"

# Use any tools placed in this directory first.
export PATH=".:$PATH"

if ! test -f 123UNIX1.IMG   \
       -a -f 123UNIX2.IMG   \
       -a -f 123UNIX3.IMG   \
       -a -f 123UNIX4.IMG   \
       -a -f 123UNIX5.IMG; then
    echo >&2 'You need to download the original 1-2-3 UNIX IMG files.'
    echo >&2 'They are available here: https://archive.org/download/123-unix'
    exit 1
fi

mkdir "${ROOT}"
mkdir "${ORIG}"

echo "==> Extracting tar archive 123UNIX1.IMG"
tar -C "${ROOT}" -xf 123UNIX1.IMG

for img in 123UNIX2.IMG 123UNIX3.IMG 123UNIX4.IMG 123UNIX5.IMG; do
    echo "==> Extracting cpio archive $img"
    cpio -D "${ROOT}" -id < "$img"
done

echo "==> Uncompressing .z files"
find "${ROOT}" -iname '*.z' -exec gunzip {} \;

echo "==> Uncompressing and copying object files"
cat "${LOTUS}"/sysV386/lib/123.o.z_? | gzip -d > "${ORIG}/123.o"
cp "${LOTUS}"/sysV386/lib/*.o "${ORIG}/"

echo "==> Installing better keymaps"
for t in xterm xterm-256color; do
    cp -v xterm "${LOTUS}/keymaps/$(echo "${t}" | cut -c1)/${t}"
done
if [ ! -f "${LOTUS}/keymaps/$(echo "${TERM}" | cut -c1)/${TERM}" ]; then
    cp -v xterm "${LOTUS}/keymaps/$(echo "${TERM}" | cut -c1)/${TERM}"
fi

echo "==> Copying the banner template over"
cp -v "${ROOT}/usr/tmp/lotus_install/123/banner" "${LOTUS}/ri/USA-English/123ban.ri"

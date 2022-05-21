#!/bin/bash
#
# Extract the necessary files from Lotus 1-2-3 UNIX
#

LOTUS=root/lotus/123.v10

if ! test -f 123UNIX1.IMG   \
       -a -f 123UNIX2.IMG   \
       -a -f 123UNIX3.IMG   \
       -a -f 123UNIX4.IMG   \
       -a -f 123UNIX5.IMG; then
    printf "You need to download the original 1-2-3 UNIX IMG files.\n" 1>&2
    exit 1
fi

mkdir root
mkdir orig

tar -C root -xf 123UNIX1.IMG

for i in 123UNIX{2..5}.IMG; do
    cpio -D root -id < ${i}
done

find root -iname '*.z' -exec gunzip {} \;

cat ${LOTUS}/sysV386/lib/123.o.z_? | gzip -d > orig/123.o

cp ${LOTUS}/sysV386/lib/*.o orig/

# Make some links for keymaps
for i in xterm xterm-256color; do
    ln ${LOTUS}/keymaps/s/sysV386-xterm ${LOTUS}/keymaps/${i:0:1}/${i}
done

# Copy the banner template over
cp root/usr/tmp/lotus_install/123/banner ${LOTUS}/ri/USA-English/123ban.ri

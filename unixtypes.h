#ifndef __UNIXTYPES_H
#define __UNIXTYPES_H

#pragma pack(push, 1)

struct unixflock {
    uint16_t    l_type;
    uint16_t    l_whence;
    uint32_t    l_start;
    uint32_t    l_len;
    uint16_t    l_pid;
};

struct unixstat {
    uint16_t    st_dev;
    uint16_t    st_ino;
    uint16_t    st_mode;
    uint16_t    st_nlink;
    uint16_t    st_uid;
    uint16_t    st_gid;
    uint16_t    st_rdev;
    uint16_t    pad;
    uint32_t    st_size;
    uint32_t    st_uatime;
    uint32_t    st_umtime;
    uint32_t    st_uctime;
};

#define UNIX_S_IFBLK 0x6000
#define UNIX_S_IFREG 0x8000
#define UNIX_S_IFLNK 0xA000
#define UNIX_S_IFDIR 0x4000
#define UNIX_S_IFCHR 0x2000
#define UNIX_S_IFIFO 0x1000

struct unixdirent {
    uint16_t    d_ino;
    uint32_t    d_off;
    uint16_t    d_reclen;
    uint16_t    d_type;
    char        d_name[256];
};

#pragma pack(pop)
#endif

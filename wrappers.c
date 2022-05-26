#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <err.h>
#include <curses.h>
#include <termios.h>
#include <signal.h>
#include <errno.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/times.h>
#include <sys/ioctl.h>

#include "filemap.h"

// The Lotus view of errno.
extern int __unix_errno;

#pragma pack(push, 1)
struct unixtermios {
    uint16_t    c_iflag;
    uint16_t    c_oflag;
    uint16_t    c_cflag;
    uint16_t    c_lflag;
    char        c_line;
    uint8_t     c_cc[8];
};
#pragma pack(pop)

#define SI86FPHW 40
#define FP_387 3

#define UNIX_VMIN 4
#define UNIX_VTIME 5

static struct termios original;

void __attribute__((constructor)) init_terminal_settings()
{
    // Make a backup of the terminal state to restore to later.
    if (tcgetattr(STDIN_FILENO, &original) != 0) {
        err(EXIT_FAILURE, "Failed to query terminal attributes.");
    }
}

void __attribute__((destructor)) fini_terminal_settings()
{
    // Restore any terminal craziness that 123 left behind.
    if (tcsetattr(STDIN_FILENO, TCSANOW, &original) != 0) {
        warn("Failed to restore terminal attributes, sorry!");
    }
}

int __unix_ioctl(int fd, unsigned long request, struct unixtermios *argp)
{
    struct termios tio = {0};

    if (argp == NULL) {
        return -1;
    }

    switch (request) {
        case TCSETS:
            if (tcgetattr(fd, &tio) != 0) {
                err(EXIT_FAILURE, "Failed to translate ioctl() to tcgetattr()");
            }

            // Lotus wants raw mode?
            if ((argp->c_oflag & 0xE7FFu) == 0) {
                cfmakeraw(&tio);

                // Okay, but nobody likes ignbrk
                tio.c_iflag |= BRKINT | IGNBRK;
                tio.c_lflag |= ISIG;
            } else {
                // I think it's trying to disable raw mode?
            }

            // Translate timeouts.
            tio.c_cc[VTIME] = argp->c_cc[UNIX_VTIME];
            tio.c_cc[VMIN] = argp->c_cc[UNIX_VMIN];

            if (tcsetattr(fd, TCSANOW, &tio) != 0) {
                err(EXIT_FAILURE, "Failed to translate ioctl() to tcsetattr()");
            }
            return 0;
        case TCGETS:
            if (tcgetattr(fd, &tio) != 0) {
                err(EXIT_FAILURE, "Failed to translate ioctl() to tcgetattr()");
            }
            argp->c_cc[UNIX_VTIME] = tio.c_cc[VTIME];
            argp->c_cc[UNIX_VMIN]  = tio.c_cc[VMIN];
            return 0;
        default:
            fprintf(stderr, "ioctl: unknown request\n");
    }
    __unix_errno = errno;
    return -1;
}

struct unixflock {
    uint16_t    l_type;
    uint16_t    l_whence;
    uint32_t    l_start;
    uint32_t    l_len;
    uint32_t    l_sysid;
    uint16_t    l_pid;
    uint32_t    l_pad[4];
};

int __unix_fcntl(int fd, int cmd, void *arg)
{
    static int unix_cmd_table[32] = {
        [3] = F_GETFL,
        [4] = F_SETFL,
        [5] = F_GETLK,
        [6] = F_SETLK,
        [7] = F_SETLKW,
    };
    static int unix_lck_table[] = {
        [1] = F_RDLCK,
        [2] = F_WRLCK,
        [3] = F_UNLCK,
    };

    // Translate command from UNIX to Linux.
    cmd = unix_cmd_table[cmd];

    switch (cmd) {
        case F_GETFL: {
            int linuxflags = fcntl(fd, cmd);
            int unixflags  = 0;

            // Pass through errno.
            __unix_errno = errno;

            if (linuxflags == -1)
                return -1;

            // I think these are all the flags 123 uses.
            if (linuxflags & O_WRONLY)
                unixflags |= 1;
            if (linuxflags & O_RDWR)
                unixflags |= 2;
            if (linuxflags & O_NONBLOCK)
                unixflags |= 4;
            if (linuxflags & O_APPEND)
                unixflags |= 8;
            if (linuxflags & O_CREAT)
                unixflags |= 0x100;

            return unixflags;
        }
        case F_SETFL: {
            int unixflags  = cmd;
            int linuxflags = 0;

            // I think these are the only flags you can change.
            if (unixflags & 4)
                linuxflags |= O_NONBLOCK;
            if (linuxflags & 8)
                linuxflags |= O_APPEND;

            if (fcntl(fd, cmd, &linuxflags) == 0) {
                return 0;
            }

            __unix_errno = errno;

            return -1;
        }
        case F_SETLK: {
            struct unixflock *ufl = arg;
            struct flock lfl = {0};

            // Translate the lock structure over.
            lfl.l_type = unix_lck_table[ufl->l_type];
            lfl.l_start = ufl->l_start;
            lfl.l_len = ufl->l_len;
            lfl.l_whence = ufl->l_whence;

            if (fcntl(fd, cmd, &lfl) == 0) {
                return 0;
            }
            __unix_errno = errno;
            return -1;
        }
        default:
            err(EXIT_FAILURE, "fcntl: unknown cmd %u requested.\n", cmd);
    }
    return -1;
}

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
    uint32_t    st_atime;
    uint32_t    st_mtime;
    uint32_t    st_ctime;
};

#define UNIX_S_IFBLK 0x6000
#define UNIX_S_IFREG 0x8000
#define UNIX_S_IFLNK 0xA000
#define UNIX_S_IFDIR 0x4000

static int translate_linux_stat(struct stat *linuxstat, struct unixstat *unixstat)
{
    memset(unixstat, 0, sizeof *unixstat);
    unixstat->st_dev = linuxstat->st_dev;
    unixstat->st_ino = linuxstat->st_ino;
    unixstat->st_nlink = linuxstat->st_nlink;
    unixstat->st_mode = linuxstat->st_mode & 0x1FF;
    unixstat->st_uid = linuxstat->st_uid;
    unixstat->st_gid = linuxstat->st_gid;
    unixstat->st_rdev = linuxstat->st_rdev;
    unixstat->st_size = linuxstat->st_size;
    unixstat->st_atime =  linuxstat->st_atim.tv_sec;
    unixstat->st_mtime =  linuxstat->st_mtim.tv_sec;
    unixstat->st_ctime =  linuxstat->st_ctim.tv_sec;

    switch (linuxstat->st_mode & S_IFMT) {
        case S_IFREG: unixstat->st_mode |= UNIX_S_IFREG; break;
        case S_IFDIR: unixstat->st_mode |= UNIX_S_IFDIR; break;
        case S_IFLNK: unixstat->st_mode |= UNIX_S_IFLNK; break;
        case S_IFBLK: unixstat->st_mode |= UNIX_S_IFBLK; break;
        default:
            warnx("Failed to translate filetype %#x.", linuxstat->st_mode);
            return -1;
    }

    return 0;
}

int __unix_stat(const char *pathname, struct unixstat *statbuf)
{
    struct stat buf;

    // This routine can change filenames to make them more suitable for Linux.
    pathname = map_unix_pathname(pathname);

    if (stat(pathname, &buf) != 0) {
        __unix_errno = errno;
        return -1;
    }

    return translate_linux_stat(&buf, statbuf);
}

int __unix_fstat(int fd, struct unixstat *statbuf)
{
    struct stat buf;

    if (fstat(fd, &buf) != 0) {
        __unix_errno = errno;
        return -1;
    }

    return translate_linux_stat(&buf, statbuf);
}

int __unix_open(const char *pathname, int flags, mode_t mode)
{
    // This routine can change filenames to make them more suitable for Linux.
    pathname = map_unix_pathname(pathname);

    switch (flags) {
        case 0x000: return open(pathname, O_RDONLY);
        case 0x001: return open(pathname, O_WRONLY);
        case 0x102: return open(pathname, O_CREAT | O_RDWR, mode);
        case 0x101: return open(pathname, O_CREAT | O_WRONLY, mode);
        case 0x109: return open(pathname, O_CREAT | O_WRONLY | O_APPEND, mode);
        case 0x302: return open(pathname, O_CREAT | O_TRUNC | O_RDWR, mode);
        default:
            errx(EXIT_FAILURE, "open() was called with unrecognized flags %#x", flags);
    }
    return -1;
}

int __unix_uname(char *sysname)
{
    struct utsname name;
    if (uname(&name) != 0) {
        return -1;
    }
    strncpy(sysname, name.sysname, 48);
    return 0;
}

int __unix_times(void *buffer)
{
    struct tms buf;
    // Note: 123 only cares about the return code.
    return times(&buf);
}

int __unix_read(int fd, void *buf, size_t count)
{
    int result;

    // We can do any necessary keyboard translation here.
    if (fd == STDIN_FILENO && count == 1 && isatty(fd)) {
        char key;

        // Do the actual read.
        result = read(fd, &key, 1);

        // Just pass through any error or timeout.
        if (result != 1) {
            __unix_errno = errno;
            return result;
        }

        // Now we can apply any fixups.
        switch (key) {
            // Apparently UNIX does not handle DEL characters reliably,
            // something to do with the console driver.
            //
            // Lets map it to backspace instead.
            case 0x7f: key = '\b';
                       break;
        }

        // Copy the updated result over.
        memcpy(buf, &key, 1);

        // All done.
        return result;
    }

    result = read(fd, buf, count);

    __unix_errno = errno;

    return result;
}

int __unix_sysi86(int cmd, uint32_t *result)
{
    // This is used to check for x87 support, nothing else is supported.
    if (cmd != SI86FPHW)
        return -1;

    *result = FP_387;
    return 0;
}

int __unix_access(const char *pathname, int mode)
{
    // The mode definitions is compatible with Linux, but we might want to
    // adjust pathnames.
    if (access(map_unix_pathname(pathname), mode) != 0) {
        __unix_errno = errno;
        return -1;
    }

    return 0;
}

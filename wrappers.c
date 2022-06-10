#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <err.h>
#include <termios.h>
#include <signal.h>
#include <errno.h>
#include <dirent.h>

#include <sys/stat.h>
#include <sys/utsname.h>
#include <sys/times.h>
#include <sys/ioctl.h>

#include "unixterm.h"
#include "filemap.h"

// The Lotus view of errno.
extern int __unix_errno;

#define SI86FPHW 40
#define FP_387 3

#define TCGETS 0x5401
#define TCSETS 0x5402
#define TCSETSW 0x5403

static struct termios original;

void __attribute__((constructor)) init_terminal_settings()
{
    // Make a backup of the terminal state to restore to later.
    // Pass a heap pointer t to tcgetattr,
    // then copy result to data segment,
    // as a workaround for OpenBSD not allowing
    // ioctl(2) to write to the data segment at this point.
    int attr;
    struct termios *t;
    if (isatty(STDIN_FILENO)) {
        t = malloc(sizeof(struct termios));
        attr = tcgetattr(STDIN_FILENO, t);
        memmove(&original, t, sizeof(struct termios));
        free(t);
        if (attr != 0) {
            err(EXIT_FAILURE, "Failed to query terminal attributes.");
        }
    }
}

void __attribute__((destructor)) fini_terminal_settings()
{
    // Restore any terminal craziness that 123 left behind.
    if (isatty(STDIN_FILENO) && tcsetattr(STDIN_FILENO, TCSANOW, &original) != 0) {
        warn("Failed to restore terminal attributes, sorry!");
    }
}

int __unix_ioctl(int fd, unsigned long request, struct unixtermios *argp)
{
    int action;
    static bool rawmode;
    struct termios tio = {0};
    static struct termios restore = {0};

    if (argp == NULL) {
        return -1;
    }

    // Assume changes should be immediate by default.
    action = TCSANOW;

    // Translating termios is really difficult, but 1-2-3 only wants to use a
    // few features. It wasnts to enable and disable "raw" mode, and change
    // VTIME and VMIN.
    switch (request) {
        case TCSETSW:
            // Choose the right action.
            action = TCSADRAIN;

            // fallthrough
        case TCSETS:
            // Fetch current attributes.
            if (tcgetattr(fd, &tio) != 0) {
                err(EXIT_FAILURE, "Failed to translate ioctl() to tcgetattr()");
            }

            // Translate timeouts.
            tio.c_cc[VTIME] = argp->c_cc[UNIX_VTIME];
            tio.c_cc[VMIN] = argp->c_cc[UNIX_VMIN];

            // Install new attributes.
            if (tcsetattr(fd, action, &tio) != 0) {
                err(EXIT_FAILURE, "Failed to translate ioctl() to tcsetattr()");
            }

            return 0;
        case TCGETS:
            // Fetch real attributes.
            if (tcgetattr(fd, &tio) != 0) {
                err(EXIT_FAILURE, "Failed to translate ioctl() to tcgetattr()");
            }

            // Initialize output in case 123 didn't.
            memset(argp, 0, sizeof *argp);

            // Translate VTIME and VMIN.
            argp->c_cc[UNIX_VTIME] = tio.c_cc[VTIME];
            argp->c_cc[UNIX_VMIN]  = tio.c_cc[VMIN];

            return 0;
        case 0x7602:    // Unknown?
        case 0x7603:    // Unknown?
        case 0x4B01:    // Unknown?
        case 0x4544:    // Unknown, 'DE'?
            break;
        default:
            warnx("ioctl: unknown request %#lx", request);
    }
    __unix_errno = errno;
    return -1;
}

#pragma pack(push, 1)
struct unixflock {
    uint16_t    l_type;
    uint16_t    l_whence;
    uint32_t    l_start;
    uint32_t    l_len;
    uint16_t    l_pid;
};
#pragma pack(pop)

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
    static int linux_lck_table[] = {
        [F_RDLCK] = 1,
        [F_WRLCK] = 2,
        [F_UNLCK] = 3,
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
        case F_GETLK: {
            struct unixflock *ufl = arg;
            struct flock lfl = {0};

            // Check if there is a lock.
            if (fcntl(fd, cmd, &lfl) == -1) {
                __unix_errno = errno;
                return -1;
            }

            // Translate the lock over.
            ufl->l_start = lfl.l_start;
            ufl->l_len = lfl.l_len;
            ufl->l_whence = lfl.l_whence;
            ufl->l_type = linux_lck_table[lfl.l_type];
            ufl->l_pid = lfl.l_pid;
            return 0;
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
    uint32_t    st_uatime;
    uint32_t    st_umtime;
    uint32_t    st_uctime;
};

#define UNIX_S_IFBLK 0x6000
#define UNIX_S_IFREG 0x8000
#define UNIX_S_IFLNK 0xA000
#define UNIX_S_IFDIR 0x4000
#define UNIX_S_IFCHR 0x2000

static int translate_linux_stat(const struct stat *linuxstat, struct unixstat *unixstat)
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
    unixstat->st_uatime =  linuxstat->st_atim.tv_sec;
    unixstat->st_umtime =  linuxstat->st_mtim.tv_sec;
    unixstat->st_uctime =  linuxstat->st_ctim.tv_sec;

    // The MSB of st_dev is queried by file_is_local()
    unixstat->st_dev &= ~0x8000;

    switch (linuxstat->st_mode & S_IFMT) {
        case S_IFREG: unixstat->st_mode |= UNIX_S_IFREG; break;
        case S_IFDIR: unixstat->st_mode |= UNIX_S_IFDIR; break;
        case S_IFLNK: unixstat->st_mode |= UNIX_S_IFLNK; break;
        case S_IFBLK: unixstat->st_mode |= UNIX_S_IFBLK; break;
        case S_IFCHR: unixstat->st_mode |= UNIX_S_IFCHR; break;
        default:
            warnx("Failed to translate filetype %#x.", linuxstat->st_mode);
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

    // Reset errno
    __unix_errno = 0;

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

    result = read(fd, buf, count);

    if (result == -1) {
        // Lotus requests non-blocking io, but then doesn't
        // handle EAGAIN.
        if (errno == EAGAIN) {
            return 0;
        }
        __unix_errno = errno;
    }

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

#pragma pack(push, 1)
struct unixdirent {
    uint16_t    d_ino;
    uint32_t    d_off;
    uint16_t    d_reclen;
    uint16_t    d_type;
    char        d_name[256];
};
#pragma pack(pop)

struct unixdirent * __unix_readdir(DIR *dirp)
{

    struct dirent *lent;
    static struct unixdirent uent;

    // Reset errno so that end of stream and error can be distinguished.
    errno = 0;

    // We reuse the same static dirent, so reset it here.
    memset(&uent, 0, sizeof uent);

    // Fetch the real entry, and translate it to the UNIX format.
    if ((lent = readdir(dirp))) {
        uent.d_ino = lent->d_ino;
        uent.d_type = lent->d_type;
        strncpy(uent.d_name, lent->d_name, sizeof uent.d_name);
        return &uent;
    }

    // Error, or end of stream, pass through errno.
    __unix_errno = errno;
    return NULL;
}

void (* __unix_signal(int signum, void (*handler)))(int)
{
    static int unix_sig_table[] = {
        [7]  = -1,   // SIGEMT
        [10] = SIGBUS,
        [12] = SIGSYS,
        [16] = SIGUSR1,
        [17] = SIGUSR2,
        [18] = SIGCHLD,
        [19] = SIGCONT,
        [20] = SIGWINCH,
        [21] = SIGURG,
        [22] = SIGTTOU,
        [23] = SIGSTOP,
        [24] = SIGTSTP,
        [25] = SIGCONT,
        [26] = SIGTTIN,
        [27] = SIGTTOU,
        [28] = SIGVTALRM,
        [29] = SIGPROF,
        [30] = SIGXCPU,
        [31] = SIGXFSZ,
        [32] = -1, // SIGWAITING
        [33] = -1, // SIGLWP
        [24] = -1, // SIGAIO
    };

    // Translate the signal number, 0 means no translation necessary.
    if (unix_sig_table[signum] != 0) {
        signum = unix_sig_table[signum];
    }

    // If this is -1, no such signal exists on Linux.
    if (signum == -1) {
        return SIG_ERR;
    }

    return signal(signum, handler);
}

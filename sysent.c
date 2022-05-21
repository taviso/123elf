/*
 * Copyright (c) 2001 Christoph Hellwig.
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * SCO Unix 3.x personality switch
 */
#include <ibcs-us/ibcs/map.h>
#include <ibcs-us/ibcs/sysent.h>
#include <ibcs-us/per-cxenix/sysent.h>
#include <ibcs-us/ibcs/linux26-compat.h>
#include <ibcs-us/per-sco/signal.h>
#include <ibcs-us/per-sco/sysent.h>
#include <ibcs-us/per-svr4/signal.h>
#include <ibcs-us/per-svr4/socket.h>
#include <ibcs-us/per-svr4/sysent.h>

#include <ibcs-us/linux26-compat/asm/uaccess.h>
#include <ibcs-us/linux26-compat/linux/errno.h>
#include <ibcs-us/linux26-compat/linux/init.h>
#include <ibcs-us/linux26-compat/linux/kernel.h>
#include <ibcs-us/linux26-compat/linux/linkage.h>
#include <ibcs-us/linux26-compat/linux/module.h>
#include <ibcs-us/linux26-compat/linux/net.h>
#include <ibcs-us/linux26-compat/linux/personality.h>
#include <ibcs-us/linux26-compat/linux/signal.h>
#include <ibcs-us/linux26-compat/linux/socket.h>
#include <ibcs-us/linux26-compat/linux/types.h>

#define _K(a) (void *)__NR_##a

MODULE_DESCRIPTION("OpenServer/Xenix personality");
MODULE_AUTHOR("Christoph Hellwig, partially taken from iBCS");
MODULE_LICENSE("GPL");
MODULE_INFO(supported,"yes");
MODULE_INFO(bugreport,"agon04@users.sourceforge.net");


static u_char sco_err_table[] = {
/*   0 -   9 */   0,   1,   2,    3,   4,   5,   6,   7,   8,   9,
/*  10 -  19 */   10,  11,  12,  13,  14,  15,  16,  17,  18,  19,
/*  20 -  29 */   20,  21,  22,  23,  24,  25,  26,  27,  28,  29,
/*  30 -  39 */   30,  31,  32,  33,  34,  45,  78,  46,  89, 145,
/*  40 -  49 */  150,  90,  35,  36,  37,  38,  39,  40,  41,  42,
/*  50 -  59 */   43,  44,  50,  51,  52,  53,  54,  55,  56,  57,
/*  60 -  69 */   60,  61,  62,  63,  64,  65,  66,  67,  68,  69,
/*  70 -  79 */   70,  71,  74,  76,  77,  79,  80,  81,  82,  83,
/*  80 -  89 */   84,  85,  86,  87,  88, 152, 153,  22,  93,  94,
/*  90 -  99 */   95,  96, 118,  97,  98,  99, 100, 101, 102, 103,
/* 100 - 109 */  104, 105, 106, 107, 108,  63, 110, 111, 112, 113,
/* 110 - 119 */  114, 115, 116, 117,  92,  91, 151, 135, 137, 138,
/* 120 - 122 */  139, 140, 28
};

/*
 * Map Linux RESTART* values (512,513,514) to EINTR
 */
static u_char lnx_err_table[] = {
/* 512 - 514 */	EINTR, EINTR, EINTR
};

static struct map_segment sco_err_map[] = {
	{ 0,	0+sizeof(sco_err_table)-1,	sco_err_table },
	{ 512,	512+sizeof(lnx_err_table)-1,	lnx_err_table },
	{ .start = -1 }
};

/* XXX: move to signal.h */
#define SCO__SIGSTKFLT	SCO_SIGTERM /* Stack fault */

static unsigned long linux_to_sco_signals[NSIGNALS + 1] = {
/*  0 -  3 */	0,		SCO_SIGHUP,	SCO_SIGINT,	SCO_SIGQUIT,
/*  4 -  7 */	SCO_SIGILL,	SCO_SIGTRAP,	SCO_SIGABRT,	SCO_SIGBUS,
/*  8 - 11 */	SCO_SIGFPE,	SCO_SIGKILL,	SCO_SIGUSR1,	SCO_SIGSEGV,
/* 12 - 15 */	SCO_SIGUSR2,	SCO_SIGPIPE,	SCO_SIGALRM,	SCO_SIGTERM,
/* 16 - 19 */	SCO__SIGSTKFLT,	SCO_SIGCLD,	SCO_SIGCONT,	SCO_SIGSTOP,
/* 20 - 23 */	SCO_SIGTSTP,	SCO_SIGTTIN,	SCO_SIGTTOU,	SCO_SIGURG,
/* 24 - 27 */	SCO_SIGXCPU,	SCO_SIGXFSZ,	SCO_SIGVTALRM,	SCO_SIGPROF,
/* 28 - 31 */	SCO_SIGWINCH,	SCO_SIGPOLL,	SCO_SIGPWR,	SCO_SIGSYS,
/*      32 */	-1
};

static unsigned long linux_to_xenix_signals[NSIGNALS+1] = {
/*  0 -  3 */	0,		IBCS_SIGHUP,	IBCS_SIGINT,	IBCS_SIGQUIT,
/*  4 -  7 */	IBCS_SIGILL,	IBCS_SIGTRAP,	IBCS_SIGABRT,	-1,
/*  8 - 11 */	IBCS_SIGFPE,	IBCS_SIGKILL,	IBCS_SIGUSR1,	IBCS_SIGSEGV,
/* 12 - 15 */	IBCS_SIGUSR2,	IBCS_SIGPIPE,	IBCS_SIGALRM,	IBCS_SIGTERM,
/* 16 - 19 */	IBCS_SIGSEGV,	IBCS_SIGCHLD,	IBCS_SIGCONT,	IBCS_SIGSTOP,
/* 20 - 23 */	IBCS_SIGTSTP,	IBCS_SIGTTIN,	IBCS_SIGTTOU,	IBCS_SIGUSR1,
/* 24 - 27 */	IBCS_SIGGXCPU,	IBCS_SIGGXFSZ,	IBCS_SIGVTALRM,	IBCS_SIGPROF,
/* 28 - 31 */	IBCS_SIGWINCH,	20 /*SIGIO*/,	IBCS_SIGPWR,	-1,
/*      32 */	-1
};

static unsigned long sco_to_linux_signals[NSIGNALS+1] = {
/*  0 -  3 */	0,		SIGHUP,		SIGINT,		SIGQUIT,
/*  4 -  7 */	SIGILL,		SIGTRAP,	SIGIOT,		SIGABRT,
/*  8 - 11 */	SIGFPE,		SIGKILL,	SIGBUS,		SIGSEGV,
/* 12 - 15 */	SIGSYS,		SIGPIPE,	SIGALRM,	SIGTERM,
/* 16 - 19 */	SIGUSR1,	SIGUSR2,	SIGCHLD,	SIGPWR,
/* 20 - 23 */	SIGWINCH,	SIGURG,		SIGPOLL,	SIGSTOP,
/* 24 - 27 */	SIGTSTP,	SIGCONT,	SIGTTIN,	SIGTTOU,
/* 28 - 31 */	SIGVTALRM,	SIGPROF,	SIGXCPU,	SIGXFSZ,
/*      32 */	-1
};

static unsigned long xenix_to_linux_signals[NSIGNALS+1] = {
/*  0 -  3 */	0,		SIGHUP,		SIGINT,		SIGQUIT,
/*  4 -  7 */	SIGILL,		SIGTRAP,	SIGIOT,		SIGUNUSED,
/*  8 - 11 */	SIGFPE,		SIGKILL,	SIGUNUSED,	SIGSEGV,
/* 12 - 15 */	SIGUNUSED,	SIGPIPE,	SIGALRM,	SIGTERM,
/* 16 - 19 */	SIGUSR1,	SIGUSR2,	SIGCHLD,	SIGPWR,
/* 20 - 23 */	SIGPOLL,	-1,		-1,		-1,
/* 24 - 27 */	-1,		-1,		-1,		-1,
/* 28 - 31 */	-1,		-1,		-1,		-1,
/*      32 */	-1
};

static char sco_socktype[] = {
	SOCK_STREAM,
	SOCK_DGRAM,
	0,
	SOCK_RAW,
	SOCK_RDM,
	SOCK_SEQPACKET
};

static struct map_segment sco_socktype_map[] = {
	{ 1, 6, (u_char *)sco_socktype },
	{ .start = -1 }
};

static struct map_segment sco_sockopt_map[] =  {
	{ 0x0001, 0x0001, (u_char *)SO_DEBUG },
	{ 0x0002, 0x0002, (u_char *)__SO_ACCEPTCON },
	{ 0x0004, 0x0004, (u_char *)SO_REUSEADDR },
	{ 0x0008, 0x0008, (u_char *)SO_KEEPALIVE },
	{ 0x0010, 0x0010, (u_char *)SO_DONTROUTE },
	{ 0x0020, 0x0020, (u_char *)SO_BROADCAST },
	{ 0x0040, 0x0040, (u_char *)SO_USELOOPBACK },
	{ 0x0080, 0x0080, (u_char *)SO_LINGER },
	{ 0x0100, 0x0100, (u_char *)SO_OOBINLINE },
	{ 0x0200, 0x0200, (u_char *)SO_ORDREL },
	{ 0x0400, 0x0400, (u_char *)SO_IMASOCKET },
	{ 0x1001, 0x1001, (u_char *)SO_SNDBUF },
	{ 0x1002, 0x1002, (u_char *)SO_RCVBUF },
	{ 0x1003, 0x1003, (u_char *)SO_SNDLOWAT },
	{ 0x1004, 0x1004, (u_char *)SO_RCVLOWAT },
	{ 0x1005, 0x1005, (u_char *)SO_SNDTIMEO },
	{ 0x1006, 0x1006, (u_char *)SO_RCVTIMEO },
	{ 0x1007, 0x1007, (u_char *)SO_ERROR },
	{ 0x1008, 0x1008, (u_char *)SO_TYPE },
	{ 0x1009, 0x1009, (u_char *)SO_PROTOTYPE },
	{ .start = -1 }
};

/*
 * The first three entries (AF_UNSPEC, AF_UNIX and AF_INET)
 * are identity mapped. All others aren't available under
 * Linux, nor are Linux's AF_AX25 and AF_IPX available from
 * SCO as far as I know.
 */
static struct map_segment sco_af_map[] =  {
	{ 0, 2, NULL },
	{ .start = -1 }
};


static struct sysent sco_syscall_table[] = {
/*   0 */	{ abi_syscall,		Fast,	"syscall",	""	},
/*   1 */	{ _K(exit),		1,	"exit",		"d"	},
/*   2 */	{ abi_fork,		Spl,	"fork",		""	},
/*   3 */	{ abi_read,		3,	"read",		"dpd"	},
/*   4 */	{ _K(write),		3,	"write",	"dpd"	},
/*   5 */	{ svr4_open,		3,	"open",		"soo"	},
/*   6 */	{ _K(close),		1,	"close",	"d"	},
/*   7 */	{ abi_wait,		Spl,	"wait",		"xxx"	},
/*   8 */	{ _K(creat),		2,	"creat",	"so"	},
/*   9 */	{ _K(link),		2,	"link",		"ss"	},
/*  10 */	{ _K(unlink),		1,	"unlink",	"s"	},
/*  11 */	{ abi_exec,		Spl,	"exec",		"sxx"	},
/*  12 */	{ _K(chdir),		1,	"chdir",	"s"	},
/*  13 */	{ abi_time,		0,	"time",		""	},
/*  14 */	{ svr4_mknod,		3,	"mknod",	"soo"	},
/*  15 */	{ _K(chmod),		2,	"chmod",	"so"	},
/*  16 */	{ _K(chown),		3,	"chown",	"sdd"	},
/*  17 */	{ abi_brk,		1,	"brk/break",	"x"	},
/*  18 */	{ svr4_stat,		2,	"stat",		"sp"	},
/*  19 */	{ sco_lseek,		3,	"seek/lseek",	"ddd"	},
/*  20 */	{ abi_getpid,		Spl,	"getpid",	""	},
/*  21 */	{ 0,			Ukn,	"mount",	""	},
#ifdef CONFIG_65BIT
/*  22 */	{ 0,			Ukn,	"umount",	"s"	},
#else
/*  22 */	{ _K(umount),		1,	"umount",	"s"	},
#endif
/*  23 */	{ _K(setuid),		1,	"setuid",	"d"	},
/*  24 */	{ abi_getuid,		Spl,	"getuid",	""	},
/*  25 */	{ abi_stime,		1,	"stime",	"d"	},
/*  26 */	{ sco_ptrace,		4,	"ptrace",	"xdxx"	},
/*  27 */	{ _K(alarm),		1,	"alarm",	"d"	},
/*  28 */	{ svr4_fstat,		2,	"fstat",	"dp"	},
/*  29 */	{ _K(pause),		0,	"pause",	""	},
/*  30 */	{ abi_utime,		2,	"utime",	"sp"	},
/*  31 */	{ 0,			Ukn,	"stty",		""	},
/*  32 */	{ 0,			Ukn,	"gtty",		""	},
/*  33 */	{ _K(access),		2,	"access",	"so"	},
#ifdef CONFIG_65BIT
/*  34 */	{ 0,			Ukn,	"nice",		"d"	},
#else
/*  34 */	{ _K(nice),		1,	"nice",		"d"	},
#endif
/*  35 */	{ svr4_statfs,		4,	"statfs",	"spdd"	},
/*  36 */	{ _K(sync),		0,	"sync",		""	},
/*  37 */	{ abi_kill,		2,	"kill",		"dd"	},
/*  38 */	{ svr4_fstatfs,		4,	"fstatfs",	"dpdd"	},
/*  39 */	{ abi_procids,		Spl,	"procids",	"d"	},
/*  40 */	{ cxenix,		Fast,	"cxenix",	""	},
/*  41 */	{ _K(dup),		1,	"dup",		"d"	},
/*  42 */	{ abi_pipe,		Spl,	"pipe",		""	},
/*  43 */	{ _K(times),		1,	"times",	"p"	},
/*  44 */	{ 0	,		Ukn,	"prof",		""	},
/*  45 */	{ 0,			Ukn,	"lock/plock",	""	},
/*  46 */	{ _K(setgid),		1,	"setgid",	"d"	},
/*  47 */	{ abi_getgid,		Spl,	"getgid",	""	},
/*  48 */	{ abi_sigfunc,		Fast,	"sigfunc",	"xxx"	},
/*  49 */	{ svr4_msgsys,		Spl,	"msgsys",	"dxddd"	},
/*  50 */	{ sco_sysi86,		3,	"sysi86/sys3b",	"d"	},
/*  51 */	{ _K(acct),		1,	"acct/sysacct",	"x"	},
/*  52 */	{ svr4_shmsys,		Fast,	"shmsys",	"ddxo"	},
/*  53 */	{ svr4_semsys,		Spl,	"semsys",	"dddx"	},
/*  54 */	{ sco_ioctl,		Spl,	"ioctl",	"dxx"	},
/*  55 */	{ 0,			3,	"uadmin",	"xxx"	},
/*  56 */	{ 0,			Ukn,	"?",		""	},
/*  57 */	{ v7_utsname,		1,	"utsys",	"x"	},
/*  58 */	{ _K(fsync),		1,	"fsync",	"d"	},
/*  59 */	{ abi_exec,		Spl,	"execv",	"spp"	},
/*  60 */	{ _K(umask),		1,	"umask",	"o"	},
/*  61 */	{ _K(chroot),		1,	"chroot",	"s"	},
/*  62 */	{ sco_fcntl,		3,	"fcntl",	"dxx"	},
/*  63 */	{ svr4_ulimit,		2,	"ulimit",	"xx"	},
/*  64 */	{ 0,			Ukn,	"?",		""	},
/*  65 */	{ 0,			Ukn,	"?",		""	},
/*  66 */	{ 0,			Ukn,	"?",		""	},
/*  67 */	{ 0,			Ukn,	"?",		""	},
/*  68 */	{ 0,			Ukn,	"?",		""	},
/*  69 */	{ 0,			Ukn,	"?",		""	},
/*  70 */	{ 0,			Ukn,	"advfs",	""	},
/*  71 */	{ 0,			Ukn,	"unadvfs",	""	},
/*  72 */	{ 0,			Ukn,	"rmount",	""	},
/*  73 */	{ 0,			Ukn,	"rumount",	""	},
/*  74 */	{ 0,			Ukn,	"rfstart",	""	},
/*  75 */	{ 0,			Ukn,	"?",		""	},
/*  76 */	{ abi_debug,		Spl,	"rdebug",	""	},
/*  77 */	{ 0,			Ukn,	"rfstop",	""	},
/*  78 */	{ 0,			Ukn,	"rfsys",	""	},
/*  79 */	{ _K(rmdir),		1,	"rmdir",	"s"	},
/*  80 */	{ abi_mkdir,		2,	"mkdir",	"so"	},
/*  81   	{ _K(getdents),		3,	"getdents",	"dxd"	},
    81   	{ svr4_getdents,	3,	"getdents",	"dxd"	},
    81 */	{ sco_getdents,		3,	"getdents",	"dxd"	},
/*  82 */	{ 0,			Ukn,	"libattach",	""	},
/*  83 */	{ 0,			Ukn,	"libdetach",	""	},
/*  84 */	{ svr4_sysfs,		3,	"sysfs",	"dxx"	},
/*  85 */	{ svr4_getmsg,		Spl,	"getmsg",	"dxxx"	},
/*  86 */	{ svr4_putmsg,		Spl,	"putmsg",	"dxxd"	},
/*  87 */	{ _K(poll),		3,	"poll",		"xdd"	},
/*  88 */	{ 0,			Ukn,	"nosys88",	""	},
/*  89 */	{ sw_security,		6,	"security",	"dxxxxx"},
/*  90 */	{ _K(symlink),		2,	"symlink",	"ss"	},
/*  91 */	{ svr4_lstat,		2,	"lstat",	"sp"	},
/*  92 */	{ _K(readlink),		3,	"readlink",	"spd"	},

	/*
	 * Ok, this is where the good-old SCO calls end.
	 * Everything from here onwards is OSR5-specific.
	 *
	 * In fact OSR5 documents only the following calls:
	 *
	 *   o readv (121)
	 *   o writev (122)
	 *   o xstat (123)
	 *   o lxstat (124)
	 *   o fxstat (125)
	 *   o clocal (127)
	 *   o gettimeofday (171)
	 *   o settimeofday (172)
	 *
	 * Everything else is guesswork (from AVR4/iABI4), even though
	 * the OSR5 ELF libs use at least some of those... (e.g. mmap).
	 */

/*  93 */	{ 0,			Ukn,	"?",		""	},
/*  94 */	{ 0,			Ukn,	"?",		""	},
/*  95 */	{ abi_sigprocmask,	3,	"sigprocmask",	"dxx"	},
/*  96 */	{ abi_sigsuspend,	Spl,	"sigsuspend",	"x"	},
/*  97 */	{ 0,			2,	"sigaltstack",	"xx"	},
/*  98 */	{ abi_sigaction,	3,	"sigaction",	"dxx"	},
/*  99 */	{ svr4_sigpending,	2,	"sigpending",	"dp"	},
/* 100 */	{ svr4_context,		Spl,	"context",	""	},
/* 101 */	{ 0,			Ukn,	"evsys",	""	},
/* 102 */	{ 0,			Ukn,	"evtrapret",	""	},
/* 103 */	{ sco_statvfs,		2,	"statvfs",	"sp"	},
/* 104 */	{ sco_fstatvfs,		2,	"fstatvfs",	"dp"	},
/* 105 */	{ 0,			Ukn,	"unimp (sysisc)",""	},
/* 106 */	{ 0,			Ukn,	"nfssys",	""	},
/* 107 */	{ 0,			4,	"waitid",	"ddxd"	},
/* 108 */	{ 0,			3,	"sigsendsys",	"ddd"	},
/* 109 */	{ svr4_hrtsys,		Spl,	"hrtsys",	"xxx"	},
/* 110 */	{ 0,			3,	"acancel",	"dxd"	},
/* 111 */	{ 0,			Ukn,	"async",	""	},
/* 112 */	{ 0,			Ukn,	"priocntlsys",	""	},
/* 113 */	{ svr4_pathconf,	2,	"pathconf",	"sd"	},
/* 114 */	{ _K(mincore),		3,	"mincore",	"xdx"	},
/* 115 */	{ sco_mmap,		6,	"mmap",		"xxxxdx"},
/* 116 */	{ _K(mprotect),		3,	"mprotect",	"xdx"	},
/* 117 */	{ _K(munmap),		2,	"munmap",	"xd"	},
/* 118 */	{ svr4_fpathconf,	2,	"fpathconf",	"dd"	},
/* 119 */	{ abi_fork,		Spl,	"vfork",	""	},
/* 120 */	{ _K(fchdir),		1,	"fchdir",	"d"	},
/* 121 */	{ _K(readv),		3,	"readv",	"dxd"	},
/* 122 */	{ _K(writev),		3,	"writev",	"dxd"	},
/* 123 */	{ sco_xstat,		3,	"xstat",	"dsx"	},
/* 124 */	{ sco_lxstat,     	3,	"lxstat",	"dsx"	},
/* 125 */	{ sco_fxstat,		3,	"fxstat",	"ddx"	},
/* 126 */	{ svr4_xmknod,		4,	"xmknod",	"dsox"	},
/* 127 */	{ 0,			Ukn,	"syslocal",	"d"	},
/* 128 */	{ svr4_getrlimit,	2,	"setrlimit",	"dx"	},
/* 129 */	{ svr4_setrlimit,	2,	"getrlimit",	"dx"	},
/* 130 */	{ 0,			Ukn,	"?",		""	},
/* 131 */	{ 0,			Ukn,	"?",		""	},
/* 132 */	{ 0,			Ukn,	"?",		""	},
/* 133 */	{ 0,			Ukn,	"?",		""	},
/* 134 */	{ 0,			Ukn,	"?",		""	},
/* 135 */	{ 0,			Ukn,	"?",		""	},
/* 136 */	{ 0,			Ukn,	"?",		""	},
/* 137 */	{ 0,			Ukn,	"?",		""	},
/* 138 */	{ 0,			Ukn,	"?",		""	},
/* 139 */	{ 0,			Ukn,	"?",		""	},
/* 140 */	{ socksys_syscall,	1,	"socksys",	"x"	},
/* 141 */	{ 0,			Ukn,	"?",		""	},
/* 142 */	{ 0,			Ukn,	"?",		""	},
/* 143 */	{ 0,			Ukn,	"?",		""	},

	/*
	 * The next three are used when an OSR5 app is launched by
	 * a UnixWare libc.
	 *
	 * These aren't supported by the UW7 personality either, but
	 * adding the names here makes the traces look saner.
	 */

/* 144 */	{ 0,			2,	"uw7-secsys",	"dx"	},
/* 145 */	{ 0,			4,	"uw7-filepriv",	"sdxd"	},
/* 146 */	{ 0,			3,	"uw7-procpriv",	"dxd"	},

/* 147 */	{ 0,			Ukn,	"?",		""	},
/* 148 */	{ 0,			Ukn,	"?",		""	},
/* 149 */	{ 0,			Ukn,	"?",		""	},
/* 150 */	{ 0,			Ukn,	"?",		""	},
/* 151 */	{ 0,			Ukn,	"?",		""	},
/* 152 */	{ 0,			Ukn,	"?",		""	},
/* 153 */	{ 0,			Ukn,	"?",		""	},
/* 154 */	{ 0,			Ukn,	"?",		""	},
/* 155 */	{ 0,			Ukn,	"?",		""	},
/* 156 */	{ 0,			Ukn,	"?",		""	},
/* 157 */	{ 0,			Ukn,	"?",		""	},
/* 158 */	{ 0,			Ukn,	"?",		""	},
/* 159 */	{ 0,			Ukn,	"?",		""	},
/* 160 */	{ 0,			Ukn,	"?",		""	},
/* 161 */	{ 0,			Ukn,	"?",		""	},
/* 162 */	{ 0,			Ukn,	"?",		""	},
/* 163 */	{ 0,			Ukn,	"?",		""	},
/* 164 */	{ 0,			Ukn,	"?",		""	},
/* 165 */	{ 0,			Ukn,	"?",		""	},
/* 166 */	{ 0,			Ukn,	"?",		""	},
/* 167 */	{ 0,			Ukn,	"?",		""	},
/* 168 */	{ 0,			Ukn,	"?",		""	},
/* 169 */	{ 0,			Ukn,	"?",		""	},
/* 170 */	{ 0,			Ukn,	"?",		""	},
/* 171 */	{ abi_gettime,		2,	"gettimeofday",  "xx"	},
/* 172 */	{ abi_settime,		2,	"settimeofday",  "xx"	},
};

static asmlinkage void
sco_lcall7(int segment, struct pt_regs *regs)
{
	int sysno = _AX(regs) & 0xff;

	if (sysno >= ARRAY_SIZE(sco_syscall_table))
		set_error(regs, iABI_errors(-EINVAL));
	else
		lcall7_dispatch(regs, &sco_syscall_table[sysno], 1);
}


static struct exec_domain sco_exec_domain = {
	name:		"OpenServer",
	handler:	(handler_t)sco_lcall7,
	pers_low:	3 /* PER_SCOSVR3, PER_OSR5 */,
	pers_high:	3 /* PER_SCOSVR3, PER_OSR5 */,
	signal_map:	sco_to_linux_signals,
	signal_invmap:	linux_to_sco_signals,
	err_map:	sco_err_map,
	socktype_map:	sco_socktype_map,
	sockopt_map:	sco_sockopt_map,
	af_map:		sco_af_map,
	module:		THIS_MODULE
};

static struct exec_domain xenix_exec_domain = {
	name:		"Xenix",
	handler:	(handler_t)sco_lcall7,
	pers_low:	7 /* PER_XENIX */,
	pers_high:	7 /* PER_XENIX */,
	signal_map:	xenix_to_linux_signals,
	signal_invmap:	linux_to_xenix_signals,
	err_map:	sco_err_map,
	socktype_map:	sco_socktype_map,
	sockopt_map:	sco_sockopt_map,
	af_map:		sco_af_map,
	module:		THIS_MODULE
};


static int __init
sco_module_init(void)
{
        int		err;

        err = register_exec_domain(&sco_exec_domain);
        if (err) {
                return err;
        }

        err = register_exec_domain(&xenix_exec_domain);
        if (err) {
                unregister_exec_domain(&sco_exec_domain);
                return err;
        }

        return err;
}

static void __exit
sco_module_exit(void)
{
	unregister_exec_domain(&xenix_exec_domain);
	unregister_exec_domain(&sco_exec_domain);
}

module_init(sco_module_init);
module_exit(sco_module_exit);

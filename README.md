# Lotus 1-2-3 for Linux

This is a native port of Lotus 1-2-3 version 3 to Linux. This is possible
because the official Lotus 1-2-3 for UNIX port used a technique called [partial
linking](https://sourceware.org/binutils/docs-2.38/ld/Options.html#:~:text=This%20is%20often%20called%20partial%20linking.) to workaround a technical limitation in early UNIX systems. Surprisingly, that can be used to modify it to support new platforms.

There's an article documenting how this is possible
[here](https://lock.cmpxchg8b.com/linux123.html).

![Lotus 1-2-3 for Linux](https://lock.cmpxchg8b.com/img/123linux.png)

## Building

### Dependencies

First, you need a version of binutils that is compiled with `coff-i386` target
support. You can check like this:

```
$ objdump --info | grep coff-i386
```

> Note: Most distributions do not enable this for some reason.

Secondly, you need a copy of Lotus 1-2-3 for UNIX, you can download it
[here](https://archive.org/details/123-unix). Just place the raw disk images in 
the build directory and run `extract.sh`.

Finally, just run `make`.

#### Packages

The following packages are required

| Ubuntu              | Fedora              |
| ------------------- | ------------------- |
| build-essential     | glibc-devel.i686    |
| gcc-multilib        | libgcc.i686         |
| lib32ncurses-dev    | ncurses-static.i686 |

### Binutils

Unfortunately, most distributions do not enable `coff-i386` support in binutils.

It's very easy to enable it yourself, download [binutils](https://www.gnu.org/software/binutils/) and configure it with `--enable-targets=all`.

You need `objcopy` and `objdump` from the `binutils` directory, and `ld-new` from the `ld` directory. Copy those binaries into the 123elf build directory.

> Important: `ld-new` must be renamed `real-ld` or gcc won't use it (I don't know why).

The Makefile should automatically use the new binaries, and continue to build.

## Running

Copy the file `l123set.cf` to `~/.l123set`, and run `./123`.


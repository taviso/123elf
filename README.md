# Lotus 1-2-3 for Linux

This is a native port of Lotus 1-2-3 Release 3 to Linux. This is possible
because Lotus 1-2-3 for UNIX SystemV used a trick called [partial
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

Run the included `binutils.sh` to download and compile a version of binutils known to work for this.

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

> Note: binutils-2.38 is known to work, some earlier versions have been found to not work.

There's no need to install it, just build it with `make`.

When that's complete, copy `objcopy` and `objdump` from the `binutils` directory, and `ld-new` from the `ld` directory to `123elf` directory.

> Important: Remember to rename `ld-new` to `ld`.

The Makefile should automatically use the new binaries, and continue to build.

## Installing

Run `make install`
By default it will install to /usr/local
In case you want to change the base dir run `make prefix=path install`
In case you want to change the bindir you can append `bindir=path` (e.g. `make bindir=usr`)

It will install:
 - the shared files of the lotus installation to the shared folder 
 - The relinked binary
 - A shim to run Lotus on the system bin folder  based on the envrionemnt variable `LOTUS_HOME`

*Observerd limitation: when checking pathing it seems to access some file in the shared folder which make it appeared as if it stuck when it try to change it*


## Running

Copy the file `l123set.cf` to `~/.l123set`, and run `./123`.

### Getting Started

- There is a man page in `./root/lotus/man/man1/123.1`, and a full manual available online [here](https://archive.org/details/lotus-1-2-3-release-3.1-reference/Lotus%201-2-3%20Release%203.1%20-%20Tutorial).

Lotus 1-2-3 has context sensitive online help, you can press `F1` at most times to see some hints.

> Note: You use use the `/` key to open the 123 menu!

If you've used any spreadsheet before, you should be able to get started
quickly. Functions use `@` instead of `=`, but the common functions like
`@SUM`, `@AVG`, `@INDEX`, and even `@HLOOKUP` all work as you would expect.

## Bugs

- ~~The keyboard map seems to be incomplete~~ This seems to be working in xterm, please test other terminals!
- Graphs don't work yet (I'm working on it!).
- Printing doesn't work yet.
- File an issue if you notice something, there are probably lots of minor issues that can be fixed!

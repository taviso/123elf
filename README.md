# Lotus 1-2-3 for Linux

This is a native port of Lotus 1-2-3 Release 3 to Linux. This is possible
because Lotus 1-2-3 for UNIX SystemV used a trick called [partial
linking](https://sourceware.org/binutils/docs-2.38/ld/Options.html#:~:text=This%20is%20often%20called%20partial%20linking.) to workaround a technical limitation in early UNIX systems. Surprisingly, that can be used to modify it to support new platforms.

There's an article documenting how this is possible
[here](https://lock.cmpxchg8b.com/linux123.html).

![Lotus 1-2-3 for Linux](https://lock.cmpxchg8b.com/img/123linux.png)

## Building

### With Docker/Podman

If you have [Docker](https://www.docker.com/) or [Podman](https://podman.io/) installed you can run
```sh
./run-from-container.sh 20
```

And that script will automatically build Lotus 1-2-3 within a Ubuntu 20 container and then run it in another container.
Other Ubuntu versions are supported (launch `./run-from-container.sh` without arguments to see which).

You can also build the `123` binary only by using:

```sh
./build-from-host.sh 20
```

But the produced binary will not work if your host system libraries differ from the container.

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

| Ubuntu              | Fedora              | Debian (bookworm)   | Ubuntu (bionic)
| ------------------- | ------------------- | ------------------- | -------------------
| build-essential     | glibc-devel.i686    | build-essential     | build-essential
| gcc-multilib        | libgcc.i686         | gcc-multilib        | gcc-multilib
| lib32ncurses-dev    | ncurses-static.i686 | lib32ncurses-dev    | libncurses-dev:i386

### Binutils

Unfortunately, most distributions do not enable `coff-i386` support in binutils.

It's very easy to enable it yourself, download [binutils](https://www.gnu.org/software/binutils/) and configure it with `--enable-targets=all`.

> Note: binutils-2.38 is known to work, some earlier versions have been found to not work.

There's no need to install it, just build it with `make`.

When that's complete, copy `objcopy` and `objdump` from the `binutils` directory, and `ld-new` from the `ld` directory to `123elf` directory.

> Important: Remember to rename `ld-new` to `ld`.

The Makefile should automatically use the new binaries, and continue to build.

## Running

Just run `./123`.

### Getting Started

There is a quick start guide in the wiki [here](https://github.com/taviso/123elf/wiki/Getting-Started), and the full manual can be seen here [here](https://archive.org/details/lotus-1-2-3-release-3.1-reference/Lotus%201-2-3%20Release%203.1%20-%20Tutorial).

There is a man page in `./root/lotus/man/man1/123.1` that describes the command line options.

Lotus 1-2-3 has context sensitive online help, you can press <kbd>F1</kbd> at most times to see some hints.

> Note: You use the <kbd>/</kbd> key to open the 123 menu!

If you've used any spreadsheet before, you should be able to get started quickly. Functions use `@` instead of `=`, but the common functions like `@SUM`, `@AVG`, `@INDEX`, and even `@HLOOKUP` all work as you would expect.

## FAQ

- Q. How do I quit 123?

If the status indicator in the top right says `READY`, try `/Quit Yes`.

If it doesn't say `READY` (it might say `ERROR`, `HELP` `POINT`, `MENU` or
something else), try hitting `Esc` until it goes back to `READY`.

- Q. I get the error 'invalid compressed data--code out of range'.

You have a *very* old gzip with broken lzw/pack support. You can try running
the `gzip.sh` script to build a more recent gzip, then rerun the extract
script.

## Bugs

- ~~The keyboard map seems to be incomplete~~ This seems to be working in xterm, please test other terminals!
- ~~Graphs don't work yet~~ (Partially working!, see [#5](https://github.com/taviso/123elf/issues/5)).
- ~~Printing doesn't work yet~~ (Print to file works, print to lpr is being worked on, see [#50](https://github.com/taviso/123elf/issues/50)).
- File an issue if you notice something, there are probably lots of minor issues that can be fixed!

## Security

By default, 123 allows Autoexec macros in worksheets. Lotus macros are very
powerful, and can run shell commands, read and write arbitrary files, load
plugins and so on.

However, you can disable Autoexec macros via `/Worksheet Global Default Autoexec`.

> I am thinking of changing this default before we reach a first release, see [#27](https://github.com/taviso/123elf/issues/27).

If you disable Autoexec then in *theory* it's safe to open untrusted
worksheets -- ***but*** this software hasn't been maintained for over 30 years,
and may contain security bugs!

We can fix bugs with `coffsyrup`, by redirecting unsafe functions to new
safe versions, so we will make a best effort to fix vulnerabilities if you
[report](https://github.com/taviso/123elf/issues/new) them!



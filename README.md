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

| Ubuntu              | Fedora              | Debian (bookworm)
| ------------------- | ------------------- | -------------------
| build-essential     | glibc-devel.i686    | build-essential
| gcc-multilib        | libgcc.i686         | gcc-multilib
| lib32ncurses-dev    | ncurses-static.i686 | lib32ncurses-dev

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

- There is a man page in `./root/lotus/man/man1/123.1`, and a full manual available online [here](https://archive.org/details/lotus-1-2-3-release-3.1-reference/Lotus%201-2-3%20Release%203.1%20-%20Tutorial).

Lotus 1-2-3 has context sensitive online help, you can press `F1` at most times to see some hints.

> Note: You use use the `/` key to open the 123 menu!

If you've used any spreadsheet before, you should be able to get started
quickly. Functions use `@` instead of `=`, but the common functions like
`@SUM`, `@AVG`, `@INDEX`, and even `@HLOOKUP` all work as you would expect.

## FAQ

- Q. How do I quit 123?

If the status indicator in the top right says `READY`, try `/Quit Yes`.

If it doesn't say `READY` (it might say `ERROR`, `HELP` `POINT`, `MENU` or
something else), try hitting `Esc` until it goes back to `READY`.

## Bugs

- ~~The keyboard map seems to be incomplete~~ This seems to be working in xterm, please test other terminals!
- Graphs don't work yet (I'm working on it!).
- Printing doesn't work yet.
- File an issue if you notice something, there are probably lots of minor issues that can be fixed!

## Security

By default, 123 allows Autoexec macros in worksheets, so there is no security
and you should not open untrusted files. In 1-2-3, macros can contain shell
commands, read and write arbitrary files, load plugins and so on.

You can disable Autoexec macros via `/Worksheet Global Default Autoexec`.

> I am thinking of changing this default before we reach a first release!

If you do disable Autoexec then in *theory* it's safe to open untrusted
worksheets -- ***but*** this software hasn't been maintained for over 30 years,
and is likely full of critical security bugs!

I can patch bugs using `coffsyrup` to redirect unsafe functions to new
versions, so I will make a best effort to fix bugs if you report them! (Just
file an issue, no need for disclosure).


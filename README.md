# Lotus 1-2-3 for Linux

This is a native port of Lotus 1-2-3 version 3 to Linux. This is possible
because the official Lotus 1-2-3 for UNIX port used a technique called [partial
linking](https://sourceware.org/binutils/docs-2.38/ld/Options.html#:~:text=This%20is%20often%20called%20partial%20linking.), which can be modified to support new platforms.

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

#### Ubuntu

> Note: Ubuntu does not enable `coff-i386` support in it's binutils package, so
> you need to build your own copy, follow the instructions below.

The following packages are required:

    - build-essential
    - gcc-multilib
    - lib32ncurses-dev

### Binutils

If you want to build binutils just for this purpose, just pass
`--enable-targets=all` to configure.

If you don't want to override your system binutils, copy the new `objcopy`,
`objdump` and `ld-new` into the build directory.

> Important: `ld` must be called `real-ld` or gcc won't use it (I don't know why).

Now run `PATH=.:$PATH make`, and it should work.

## Running

Copy the file `l123set.cf` to `~/.l123set`, and run `./123`.


# Security Policy

##  Macros
By default, 1-2-3 allows [AutoExec macros](https://github.com/taviso/123elf/wiki/Getting-Started#autoexec-macros)
in worksheets. 1-2-3 macros are powerful, they can run shell commands with `{SYSTEM}`, read and write
arbitrary files with `{OPEN}`, and so on.

However, you can disable AutoExec macros via `/Worksheet Global Default Autoexec No Update`.

> I am thinking of changing this default before we reach a first release, see [#27](https://github.com/taviso/123elf/issues/27).

If you disable AutoExec then in *theory* it's safe to open untrusted
worksheets -- ***but*** this software hasn't been maintained for over 30 years,
and may contain security bugs!

## Sealed Files

There is a 1-2-3 feature called "Sealing", accessible via `/File Admin Seal`. In theory, this requires
a password to alter protected ranges (see `/Worksheet Global Prot`) or to view hidden ranges (see `/Range Format
Hidden`).

This is not a supported security feature, the password is stored in the file in plaintext.

## Reporting a Vulnerability

It is possible to fix bugs by redirecting unsafe functions to new safe versions, so we will
make a best effort to fix vulnerabilities if you [report](https://github.com/taviso/123elf/issues/new)
them!


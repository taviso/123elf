#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>

extern int __unix_main(int argc, char **argv, char **envp);
extern int setchrclass(const char *class);

int main(int argc, char **argv, char **envp)
{
    putenv("TERMINFO=/usr/share/terminfo/");
    putenv("LOTUS_OS_ENV=x");
    putenv("LOTUS_ESCAPE_TIMEOUT=10");
    setenv("TMPDIR", "/tmp", 0);

    setchrclass("ascii");
    return __unix_main(argc, argv, envp);
}

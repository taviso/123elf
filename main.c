#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>

#include "lotdefs.h"
#include "lottypes.h"
#include "lotfuncs.h"

extern int __unix_main(int argc, char **argv, char **envp);
extern int setchrclass(const char *class);

int main(int argc, char **argv, char **envp)
{
    putenv("TERMINFO=/usr/share/terminfo/");
    putenv("LOTUS_OS_ENV=x");

    // This controls how long lotus waits to see if an escape character is part
    // of a sequence or on it's own. Increase it if escape characters are not
    // being recognized properly, perhaps you're using 123 over a very slow
    // connection. If you set it too high, lotus might seem to pause when
    // pressing the Esc key.
    setenv("LOTUS_ESCAPE_TIMEOUT", "1", 0);

    setenv("TMPDIR", "/tmp", 0);

    setchrclass("ascii");
    return __unix_main(argc, argv, envp);
}

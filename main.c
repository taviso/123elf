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
    char **new_argv;

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

    // Enforce UNIX file-name mode
    new_argv = malloc((argc + 3) * sizeof(*new_argv));
    if (argc > 1) {
        memcpy(&new_argv[3], &argv[1], (argc - 1) * sizeof(*new_argv));
    }
    new_argv[0] = argv[0];
    new_argv[1] = "-f";
    new_argv[2] = "unix";
    argc += 2;
    new_argv[argc] = NULL;

    return __unix_main(argc, new_argv, envp);
}

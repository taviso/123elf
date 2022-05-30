#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <curses.h>
#include <getopt.h>
#include <unistd.h>
#include <err.h>

#include "lotdefs.h"
#include "lottypes.h"
#include "lotfuncs.h"

extern int __unix_main(int argc, char **argv, char **envp);
extern int setchrclass(const char *class);

static void hide_option_from_lotus(int *argc, char **argv) {
    // Now move optind back one position
    --optind;

    // We can't remove options like -xyz or -zparam.
    if (argv[optind][0] != '-' || argv[optind][2] != '\0') {
        errx(EXIT_FAILURE, "Options cannot be combined.");
    }

    // Copy remaining parameters down.
    memmove(&argv[optind],
            &argv[optind + 1],
            (--*argc - optind) * sizeof(*argv));
}

static void print_help()
{
    // This is an atexit() routine that is called after 1-2-3 prints
    // it's own help, so we can append any flags we support.
    printf("        -b                      to enable banner\n");
    printf("        -u                      to disable undo support\n");
}

int main(int argc, char **argv, char **envp)
{
    char dumpfile[64];
    int opt;

    // The location of terminfo definitions.
    setenv("TERMINFO", "/usr/share/terminfo/", 0);

    // This controls how long lotus waits to see if an escape character is part
    // of a sequence or on it's own. Increase it if escape characters are not
    // being recognized properly, perhaps you're using 123 over a very slow
    // connection. If you set it too high, lotus might seem to pause when
    // pressing the Esc key.
    setenv("LOTUS_ESCAPE_TIMEOUT", "1", 0);

    // This changes how some timeouts work, is this still necessary?
    setenv("LOTUS_OS_ENV", "xenix", 0);

    // If you send lotus a SIGUSR1 (e.g. kill -USR1 $(pidof 123)), it will save
    // a copy of the screen to the specified file. You can use this for automation
    // or grabbing some figures over ssh from a session you left running.
    snprintf(dumpfile, sizeof dumpfile, "123screen.%d.txt", getpid());
    setenv("LOTUS_SCREEN_DUMP", dumpfile, 0);

    setchrclass("ascii");

    // Disable the banner by default, it can be re-enabled via -b.
    banner_printed = true;

    // Enable undo by default, you can disable it via -u.
    reset_undo(1);

    while ((opt = getopt(argc, argv, "f:c:k:np:w:hbu")) != -1) {
        switch (opt) {
            case 'b': banner_printed = false;
                      hide_option_from_lotus(&argc, argv);
                      break;
            case 'u': undo_off_cmd();
                      hide_option_from_lotus(&argc, argv);
                      break;
            case '?':
            case 'h': atexit(print_help);
                      break;
        }
    }
    return __unix_main(argc, argv, envp);
}

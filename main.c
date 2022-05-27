#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <curses.h>
#include <getopt.h>
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
}

int main(int argc, char **argv, char **envp)
{
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

    setchrclass("ascii");

    // Enable emulation of CTRL+PgUp and CTRL+PgDn on physical DEC VT320 terminal
    // DEC's Shift+F20 -> Ctrl+PgUp (Go to next sheet)
    // DEC's Shift+F19 -> Ctrl+PgDn (Go to previous sheet)
    // DEC's Shift+F11 -> ESC
    // DEC's Shift+F6  -> ESC
    // docs: https://www.vt100.net/docs/vt510-rm/DECUDK.html
    char *term = getenv("TERM");
    if (term[0]=='v' && term[1]=='t') {
        setenv("TERM", "vt100", 1);
        printf("\033P1;1;1|34/1b5b353b357e\033\\"); // '|34/' = F20
        printf("\033P1;1;1|33/1b5b363b357e\033\\"); // '|33/' = F19
        printf("\033P1;1;1|17/1b\033\\"); // '|17/' = F6
        printf("\033P1;1;1|23/1b\033\\"); // '|23/' = F11
    }

    // Disable the banner by default, it can be re-enabled via -b.
    banner_printed = true;

    while ((opt = getopt(argc, argv, "f:c:k:np:w:hb")) != -1) {
        switch (opt) {
            case 'b': banner_printed = false;
                      hide_option_from_lotus(&argc, argv);
                      break;
            case '?':
            case 'h': atexit(print_help);
                      break;
        }
    }
    return __unix_main(argc, argv, envp);
}

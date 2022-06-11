#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <limits.h>
#include <alloca.h>
#include <err.h>

#include "lottypes.h"
#include "lotdefs.h"
#include "lotfuncs.h"

extern int __unix_main(int argc, char **argv, char **envp);
extern int setchrclass(const char *class);

static void canonicalize_auto_worksheet(int *argc, char **argv, const char *wkspath)
{
    static char autofile[PATH_MAX];

    if (realpath(wkspath, autofile) == NULL) {
        warn("Failed to canonicalize worksheet path.");
        return;
    }

    if (access(autofile, F_OK) != 0) {
        warn("The worksheet specified does not exist.");
        return;
    }

    // Okay, replace it with a canonical path!
    argv[(*argc)++] = "-w";
    argv[(*argc)++] = autofile;
}

// This is used to evaluate a macro on the commandline.
static const char *macro_text;

static const char *playback_macro_text(int16_t n)
{
    // n is set one 123 wants to advance to the next cell, so
    // we just remove the contents.
    if (n != 0) {
        macro_text = NULL;
    }

    return macro_text;
}

// This is called when 123 is ready to receive input, we can check
// if we have any jobs we want to do.
int ready_to_read(int fd)
{
    static struct MACXRTNS macro = {
        .get_mac_text = playback_macro_text,
    };

    // If we have a macro to evaluate, submit it here.
    if (macro_text && in_rdy_mode()) {
        macro_buff_run(&macro);
    }

    return 0;
}

// This is an atexit() routine that is called after 1-2-3 prints
// it's own help, so we can append any flags we support.
static void print_help()
{
    static int printed;

    // This can happen if multiple invalid options are passed.
    if (printed++)
        return;

    printf("        -b                      to enable banner\n");
    printf("        -u                      to disable undo support\n");
    printf("        -e macro                to evaluate a macro\n");
}

static const char *optstring = ":f:c:k:np:w:hbue:";

int main(int argc, char **argv, char **envp)
{
    char **lotargv;
    char dumpfile[64];
    int lotargc;
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

    // No need to close the printer driver, it is currently a noop.
    need_to_close = false;

    // We always need at least two entries, for argv[0] and a terminator.
    lotargc = 2;

    // We need to do a first pass through the options to see how many there
    // are.
    while ((opt = getopt(argc, argv, optstring)) != -1)
        lotargc++;

    // If there was a non-option parameter, we will inject a synthetic
    // parameter.
    if (argv[optind] != NULL)
        lotargc += 2;

    // Allocate the argument vector we're going to pass through to lotus.
    lotargv = alloca(lotargc * sizeof(*argv));

    // Now reset and copy the options over.
    lotargc = 0;

    // Reset optind to restart getopt();
    optind = 1;

    // The first argument is the same.
    lotargv[lotargc++] = argv[0];

    // This time we copy options over.
    while ((opt = getopt(argc, argv, optstring)) != -1) {
        // Here 'continue' means don't pass this option to Lotus and 'break'
        // means pass it through verbatim.
        switch (opt) {
            case 'b': banner_printed = false;
                      continue;
            case 'u': undo_off_cmd();
                      continue;
            case 'e': macro_text = optarg;
                      continue;
            case '?':
            case 'h': atexit(print_help);
                      break;
        }

        // If we reach here, we want to pass this option through to
        // Lotus verbatim.
        lotargv[lotargc] = alloca(3);
        lotargv[lotargc][0] = '-';
        lotargv[lotargc][1] = opt;
        lotargv[lotargc][2] = 0;
        lotargc++;

        // There may also be an argument.
        if (optarg)
            lotargv[lotargc++] = optarg;

    }

    // Lotus does not accept any non-options arguments. If you do provide
    // one, we will try to translate it into a worksheet name.
    if (argv[optind] != NULL) {
        canonicalize_auto_worksheet(&lotargc, lotargv, argv[optind]);
    }

    // Always add a NULL terminator
    lotargv[lotargc] = NULL;

    return __unix_main(lotargc, lotargv, envp);
}

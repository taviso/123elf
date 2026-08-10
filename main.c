#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <getopt.h>
#include <unistd.h>
#include <limits.h>
#include <alloca.h>
#include <err.h>

#include "lottypes.h"
#include "lotdefs.h"
#include "lotfuncs.h"
#include "atfuncs.h"

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

#define MAX_MACRO 64

// This is used to evaluate a macro on the commandline.
static int macro_cell_num = -1;
static int macro_cell_cnt;
static int macro_run_signal;
static const char *macro_cell_text[MAX_MACRO];

static const char *playback_macro_text(int16_t n)
{
    // n is set when 123 wants to advance to the next cell, so
    // we just remove the contents. Note that some macro commands
    // "advance" the macro cell automatically, like {OPEN}, {WRITE},
    // {IF}.
    macro_cell_num += n;

    if (macro_cell_num < MAX_MACRO) {
        return macro_cell_text[macro_cell_num];
    }
    return NULL;
}

// This is called when 123 is ready to receive input, we can check
// if we have any jobs we want to do.
int ready_to_read(int fd)
{
    static struct MACXRTNS macro = {
        .get_mac_text = playback_macro_text,
    };

    // Only check for macro jobs in READY mode.
    if (!in_rdy_mode())
        return 0;

    // If we have a macro to evaluate, submit it here.
    if (macro_cell_cnt && macro_cell_num == -1) {
        // Start at the first cell, this allows multiple cells to be
        // used on the commandline, -e a -e b -e c, and so on.
        macro_cell_num = 0;
        // Submit it.
        macro_buff_run(&macro);
    }

    // If a signal handler macro has been triggered, run it here.
    if (macro_run_signal) {
        // Mark this signal processed.
        macro_run_signal = 0;

        // Trigger a {REFRESH} to leave the idle loop.
        kfqueue_submit_kfun(6244);

        // Invoke the macro \1.
        macro_key_run(24);
    }

    return 0;
}

// This signal handler triggers the use of macro \1.
static void macro_signal(int n)
{
    macro_run_signal++;
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
    setenv("LOTUS_OS_ENV", "systemv", 0);

    // If you send lotus a SIGUSR1 (e.g. kill -USR1 $(pidof 123)), it will save
    // a copy of the screen to the specified file. You can use this for automation
    // or grabbing some figures over ssh from a session you left running.
    snprintf(dumpfile, sizeof dumpfile, "123screen.%d.txt", getpid());
    setenv("LOTUS_SCREEN_DUMP", dumpfile, 0);

    setchrclass("ascii");

    // Setup a signal handler for SIGUSR2
    signal(SIGUSR2, macro_signal);

    // Disable the banner by default, it can be re-enabled via -b.
    banner_printed = true;

    // Enable undo by default, you can disable it via -u.
    reset_undo(1);

    // No need to close the printer driver, it is currently a noop.
    need_to_close = false;

    // Configure our @functions here.
    init_at_funcs();

    // Count the maximum number of lotargv entries needed.
    // Each getopt iteration can produce up to 2 entries (flag + optarg).
    lotargc = 2;

    while ((opt = getopt(argc, argv, optstring)) != -1)
        lotargc += 2;

    if (argv[optind] != NULL)
        lotargc += 2;

    lotargv = alloca(lotargc * sizeof(*argv));

    lotargc = 0;
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
            case 'e': if (macro_cell_cnt < MAX_MACRO) {
                          macro_cell_text[macro_cell_cnt++] = optarg;
                      }
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

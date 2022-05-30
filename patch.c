#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <err.h>
#include <alloca.h>
#include <curses.h>
#include <termios.h>
#include <spawn.h>

#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/utsname.h>

#include "lotdefs.h"
#include "lottypes.h"
#include "lotfuncs.h"

int lic_init(void)
{
    fprintf(stderr, "🏴‍☠️  license granted 🏴‍☠️\n");
    return 2;
}

// Printing does not currently work, but this can be fixed in future.
int open_printer_drivers(void)
{
    return 1;
}
int close_printer_drivers(void)
{
    return 0;
}
int load_printer_drivers(void)
{
    return 0;
}
int read_print_config_dir(void)
{
    return 0;
}

// The original function uses a fixed size stack buffer which crashes if
// COLUMNS > 256. This rewrite uses a dynamic buffer.
int display_column_labels()
{
    uint8_t *dspinfo = vmr[3];
    uint16_t column_labels = 0;
    uint8_t *buf;

    // Is this right?
    buf = alloca(COLS);

    // I have no idea what this structure is.
    if (dspinfo[11]) {
        column_labels = get_column_labels(dspinfo[10],
                                          dspinfo[11],
                                          buf,
                                          displayed_window[30]);
    }

    get_column_labels(dspinfo[9],
                      displayed_window[13],
                      &buf[column_labels],
                      displayed_window[11]);
    x_disp_txt_set_pos(displayed_window[19],
                       displayed_window[18] - 1);
    return x_disp_txt_write(displayed_window[21], buf, 0);
}

extern char **environ;
extern int redraw();
int shell()
{
    pid_t pid;
    char cmd[48], *shell;
    char *argv[] = {"sh", "-c", cmd, NULL};
    struct termios origin;

    tcgetattr(STDIN_FILENO, &origin);

    puts("\033[2J\033[H"); /* clear the screen */
    printf("(Type 'exit' and press ENTER to return to 1-2-3)\n\n");
    puts("\033[2;1H"); /* move to the next line */

    shell = getenv("SHELL");
    if (!shell)
        shell = "/bin/sh";
    sprintf(cmd, "stty sane; tput cnorm; %s", shell);
    int status = posix_spawn(&pid, "/bin/sh", NULL, NULL, argv, environ);
    if (status == 0) {
        do {
          if (waitpid(pid, &status, 0) == -1) {
              /* should not happen */
              exit(1);
          }
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    /* restore the screen */
    tcsetattr(STDIN_FILENO, TCSANOW, &origin);
    redraw();
    return 0;
}

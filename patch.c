#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <curses.h>

#include "lotdefs.h"
#include "lottypes.h"
#include "lotfuncs.h"

int lic_init(void)
{
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
    buf = malloc(COLS);

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

// This routine normally installs the original termios state but I would
// rather handle that with ncurses, so we do nothing.
void kbd_term()
{
    return;
}

// Lotus uses SysV "raw" mode with a bunch of ioctls, but rather than trying
// to translate termios structures, we can translate the intent by using
// ncurses instead.
int set_raw_mode()
{
    return 0;
}

int unset_raw_mode()
{
    return 0;
}

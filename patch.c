#include <stdio.h>
#include <stdint.h>
#include <alloca.h>
#include <curses.h>
#include <string.h>

#include "lottypes.h"
#include "lotdefs.h"
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
    char *buf;

    // Is this right?
    buf = alloca(COLS);

    // I have no idea what this structure is.
    if (dspinfo[11]) {
        column_labels = get_column_labels(dspinfo[10],
                                          dspinfo[11],
                                          buf,
                                          displayed_window->regiona.width);
    }

    get_column_labels(dspinfo[9],
                      displayed_window->field_1A,
                      &buf[column_labels],
                      displayed_window->regionb.width);
    x_disp_txt_set_pos(displayed_window->ypos,
                       displayed_window->xpos - 1);
    return x_disp_txt_write(displayed_window->columns, buf, 0);
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

uint32_t fmt_cpy(uint32_t *dst, uint32_t *src, uint16_t src_len)
{
    static const uint32_t dst_max = 256;
    uint32_t *dptr, *dst_end = dst + dst_max;

    for (dptr = dst; src_len >= 4 && dptr < dst_end; dptr++) {
        *dptr = *src++;
        src_len -= 4;
        if (*dptr > 0x80000000) {
            *dptr &= 0x7fffffff;
            if (src_len != 0) {
                uint8_t *sptr = (uint8_t *)src;
                uint8_t reps = *sptr;
                if (dptr + reps + 1 >= dst_end) {
                    // prevent overflow
                    break;
                }
                memdup(dptr, 4, reps*4 + 4);
                src_len -= 1;
                dptr += reps;
                src = (uint32_t *)(sptr+1);
            }
        }
    }
    return dptr - dst;
}

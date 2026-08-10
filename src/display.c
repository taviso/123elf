#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "lottypes.h"
#include "lotdefs.h"
#include "lotfuncs.h"
#include "loterrs.h"
#include "kfun.h"

static inline bool is_cell_valid(struct CELLCOORD cell)
{
    return !(cell.row == 0xFFFF
          && cell.col == 0xFF
          && cell.sheet == 0xFF);
}

static inline bool compare_cell(struct CELLCOORD a, struct CELLCOORD b)
{
    return a.row == b.row
       &&  a.col == b.col
       &&  a.sheet == b.sheet;
}

int mr_step_wait()
{
    char stpstatus[256];
    uint32_t key  = input_key();
    uint16_t wkey = key;
    char *p;

    // Setup a status indicator in p.
    p = stpstatus;

    // Figure out the current location of the macro.
    if (is_cell_valid(mac_cell)) {
        dspcref(0xff, mac_cell, 0, &p, 1);
    } else if (mac_xrtns.get_mac_name) {
        mac_xrtns.get_mac_name(&p);
    } else if (is_cell_valid(mac_lastgoodcell)) {
        dspcref(0xff, mac_lastgoodcell, 0, &p, 1);
    }

    // Terminate the string.
    *p++ = 0;

    // If possible, give a snippet of the macro.
    if (cmdhandle) {
        char *cmdname = access_resource(cmdhandle);
        size_t prefix_len = strlen(stpstatus);
        size_t remain = sizeof(stpstatus) - prefix_len;

        if (mac_str) {
            snprintf(stpstatus + prefix_len, remain,
                     ": {%s} (%.16s%s)",
                     cmdname, mac_str,
                     strlen(mac_str) > 16 ? "..." : "");
        } else {
            snprintf(stpstatus + prefix_len, remain,
                     ": {%s}", cmdname);
        }
    }

    // Display it in the indicator panel.
    if (strlen(stpstatus)) {
        int16_t szneeded;
        x_disp_txt_fit(strlen(stpstatus),
                       stpstatus,
                       32,
                       &szneeded);

        stpstatus[szneeded] = '\0';
        x_disp_txt_set_pos(0, num_text_rows - 1);
        x_disp_txt_set_bg(32 * hpu_per_col, 1, 0);
        x_disp_txt_zone(strlen(stpstatus), stpstatus, 0, 0, 32 * hpu_per_col);
    }

    // Check if a key has been pressed.
    if (key & 0xFFFF0000)
        return true;

    if (wkey == 0)
        return false;

    // Check for Ctrl+C.
    if (wkey != KFUN_BREAK)
        return true;

    if (!mac_nobreak())
        return true;

    return false;
}

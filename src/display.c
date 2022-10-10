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

int mr_step_wait()
{
    static char stpstatus[256];
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
        strcat(stpstatus, ": {");
        strcat(stpstatus, cmdname);
        strcat(stpstatus, "}");
    }

    if (mac_str) {
        strcat(stpstatus, " (");
        strncat(stpstatus, mac_str, 16);

        // If the string is very long, truncate it.
        if (strlen(mac_str) > 16)
            strcat(stpstatus, "...");

        strcat(stpstatus, ")");
    }

    // Display it.
    if (strlen(stpstatus)) {
        set_error_string(stpstatus, 0, 0);
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

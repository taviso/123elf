#include <curses.h>

#include "lottypes.h"
#include "lotdefs.h"
#include "ttydraw.h"

// The canvas used for drawing ascii-art graphics.
extern caca_canvas_t *cv;

void exprt_scan_linx(int x, int y, int width, int attr)
{
    caca_set_attr(cv, attr);
    caca_draw_thin_line(cv, x, y, x + width, y);
    refresh();
    return;
}

void exprt_fill_rect(int x, int y, int width, int height, int attr)
{
    caca_set_attr(cv, attr);
    caca_fill_box(cv, x, y, width, height, ' ');
    refresh();
    return;
}

void exprt_thin_diag_line(int x1, int y1, int x2, int y2, int attr)
{
    caca_set_attr(cv, attr);
    caca_draw_thin_line(cv, x1, y1, x2, y2);
    refresh();
    return;
}

void exprt_thin_vert_line(int x, int y, int height, int attr)
{
    caca_set_attr(cv, attr);
    caca_draw_thin_line(cv, x, y, x, y + height);
    refresh();
    return;
}

// This is called for cross hatching, pattern fills.
void exprt_shade_rect(struct POINT origin,
                      struct POINT dim,
                      PATT *fillpat,
                      uint16_t fillcolor)
{
    caca_set_attr(cv, fillcolor);
    caca_fill_box(cv, origin.ptx, origin.pty, dim.ptx, dim.pty, fillpat->pattptr[0]);
    refresh();
    return;
}

void exprt_fill_scan_list()
{
    return;
}


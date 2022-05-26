#ifndef __DRAW_H
#define __DRAW_H

// The canvas used for drawing ascii-art graphics.
extern caca_canvas_t *cv;

void exprt_scan_linx(int x, int y, int width, int attr);
void exprt_fill_rect(int x, int y, int width, int height, int attr);
void exprt_thin_diag_line(int x1, int y1, int x2, int y2, int attr);
void exprt_thin_vert_line(int x, int y, int height, int attr);
void exprt_shade_rect(int a, int b, int c, int d, void *e, int f);
void exprt_fill_scan_list();

#endif

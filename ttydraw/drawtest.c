#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include "config.h"

#include "ttydraw.h"

int main(int argc, char *argv[])
{
    caca_canvas_t *cv;
    initscr();
    cv = caca_create_canvas(100, 30);
    caca_draw_thin_line(cv, 10, 10, 80, 25);
    caca_draw_cp437_box(cv, 20, 5, 20, 10);
    caca_fill_ellipse(cv, 80, 8, 5, 5, '#');
    getch();
    endwin();
    return 0;
}

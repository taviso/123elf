#include <stddef.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include "config.h"

#include "ttydraw.h"
#include "ttyint.h"

int caca_put_char(caca_canvas_t *cv, int x, int y, uint32_t ch)
{
    mvaddch(y, x, ch | COLOR_PAIR(cv->curattr));
    return 1;
}

int caca_put_str(caca_canvas_t *cv, int x, int y, char const *s)
{
    int len;
    if (y < 0 || y >= (int)cv->height || x >= (int)cv->width) {
        return strlen(s);
    }

    for (len = 0; *s; len++) {
        caca_put_char(cv, x + len, y, *s++);
    }

    return len;
}


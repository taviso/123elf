/*
 *  libcaca     Colour ASCII-Art library
 *  Copyright © 2002—2018 Sam Hocevar <sam@hocevar.net>
 *              All Rights Reserved
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

/*
 *  This file contains line and polyline drawing functions, with both thin
 *  and thick styles.
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <stdlib.h>
#endif

#include "ttydraw.h"
#include "ttyint.h"

#if !defined(_DOXYGEN_SKIP_ME)
struct line
{
    int x1, y1;
    int x2, y2;
    uint32_t ch;
    void (*draw) (caca_canvas_t *, struct line*);
};
#endif

static void clip_line(caca_canvas_t*, struct line*);
static uint8_t clip_bits(caca_canvas_t*, int, int);
static void draw_solid_line(caca_canvas_t*, struct line*);
static void draw_thin_line(caca_canvas_t*, struct line*);

/** \brief Draw a line on the canvas using the given character.
 *
 *  This function never fails.
 *
 *  \param cv The handle to the libcaca canvas.
 *  \param x1 X coordinate of the first point.
 *  \param y1 Y coordinate of the first point.
 *  \param x2 X coordinate of the second point.
 *  \param y2 Y coordinate of the second point.
 *  \param ch UTF-32 character to be used to draw the line.
 *  \return This function always returns 0.
 */
int caca_draw_line(caca_canvas_t *cv, int x1, int y1, int x2, int y2,
                    uint32_t ch)
{
    struct line s;
    s.x1 = x1;
    s.y1 = y1;
    s.x2 = x2;
    s.y2 = y2;
    s.ch = ch;
    s.draw = draw_solid_line;
    clip_line(cv, &s);

    return 0;
}

/** \brief Draw a polyline.
 *
 *  Draw a polyline on the canvas using the given character and coordinate
 *  arrays. The first and last points are not connected, hence in order to
 *  draw a polygon you need to specify the starting point at the end of the
 *  list as well.
 *
 *  This function never fails.
 *
 *  \param cv The handle to the libcaca canvas.
 *  \param x Array of X coordinates. Must have \p n + 1 elements.
 *  \param y Array of Y coordinates. Must have \p n + 1 elements.
 *  \param n Number of lines to draw.
 *  \param ch UTF-32 character to be used to draw the lines.
 *  \return This function always returns 0.
 */
int caca_draw_polyline(caca_canvas_t *cv, int const x[], int const y[],
                        int n, uint32_t ch)
{
    int i;
    struct line s;
    s.ch = ch;
    s.draw = draw_solid_line;

    for(i = 0; i < n; i++)
    {
        s.x1 = x[i];
        s.y1 = y[i];
        s.x2 = x[i+1];
        s.y2 = y[i+1];
        clip_line(cv, &s);
    }

    return 0;
}

/** \brief Draw a thin line on the canvas, using ASCII art.
 *
 *  This function never fails.
 *
 *  \param cv The handle to the libcaca canvas.
 *  \param x1 X coordinate of the first point.
 *  \param y1 Y coordinate of the first point.
 *  \param x2 X coordinate of the second point.
 *  \param y2 Y coordinate of the second point.
 *  \return This function always returns 0.
 */
int caca_draw_thin_line(caca_canvas_t *cv, int x1, int y1, int x2, int y2)
{
    struct line s;
    s.x1 = x1;
    s.y1 = y1;
    s.x2 = x2;
    s.y2 = y2;
    s.draw = draw_thin_line;
    clip_line(cv, &s);

    return 0;
}

/** \brief Draw an ASCII art thin polyline.
 *
 *  Draw a thin polyline on the canvas using the given coordinate arrays and
 *  with ASCII art. The first and last points are not connected, so in order
 *  to draw a polygon you need to specify the starting point at the end of
 *  the list as well.
 *
 *  This function never fails.
 *
 *  \param cv The handle to the libcaca canvas.
 *  \param x Array of X coordinates. Must have \p n + 1 elements.
 *  \param y Array of Y coordinates. Must have \p n + 1 elements.
 *  \param n Number of lines to draw.
 *  \return This function always returns 0.
 */
int caca_draw_thin_polyline(caca_canvas_t *cv, int const x[], int const y[],
                             int n)
{
    int i;
    struct line s;
    s.draw = draw_thin_line;

    for(i = 0; i < n; i++)
    {
        s.x1 = x[i];
        s.y1 = y[i];
        s.x2 = x[i+1];
        s.y2 = y[i+1];
        clip_line(cv, &s);
    }

    return 0;
}

/*
 * XXX: The following functions are local.
 */

/* Generic Cohen-Sutherland line clipping function. */
static void clip_line(caca_canvas_t *cv, struct line* s)
{
    uint8_t bits1, bits2;

    bits1 = clip_bits(cv, s->x1, s->y1);
    bits2 = clip_bits(cv, s->x2, s->y2);

    if(bits1 & bits2)
        return;

    if(bits1 == 0)
    {
        if(bits2 == 0)
            s->draw(cv, s);
        else
        {
            int tmp;
            tmp = s->x1; s->x1 = s->x2; s->x2 = tmp;
            tmp = s->y1; s->y1 = s->y2; s->y2 = tmp;
            clip_line(cv, s);
        }

        return;
    }

    if(bits1 & (1<<0))
    {
        s->y1 = s->y2 - (s->x2 - 0) * (s->y2 - s->y1) / (s->x2 - s->x1);
        s->x1 = 0;
    }
    else if(bits1 & (1<<1))
    {
        int xmax = cv->width - 1;
        s->y1 = s->y2 - (s->x2 - xmax) * (s->y2 - s->y1) / (s->x2 - s->x1);
        s->x1 = xmax;
    }
    else if(bits1 & (1<<2))
    {
        s->x1 = s->x2 - (s->y2 - 0) * (s->x2 - s->x1) / (s->y2 - s->y1);
        s->y1 = 0;
    }
    else if(bits1 & (1<<3))
    {
        int ymax = cv->height - 1;
        s->x1 = s->x2 - (s->y2 - ymax) * (s->x2 - s->x1) / (s->y2 - s->y1);
        s->y1 = ymax;
    }

    clip_line(cv, s);
}

/* Helper function for clip_line(). */
static uint8_t clip_bits(caca_canvas_t *cv, int x, int y)
{
    uint8_t b = 0;

    if(x < 0)
        b |= (1<<0);
    else if(x >= (int)cv->width)
        b |= (1<<1);

    if(y < 0)
        b |= (1<<2);
    else if(y >= (int)cv->height)
        b |= (1<<3);

    return b;
}

/* Solid line drawing function, using Bresenham's mid-point line
 * scan-conversion algorithm. */
static void draw_solid_line(caca_canvas_t *cv, struct line* s)
{
    int x1, y1, x2, y2;
    int dx, dy;
    int xinc, yinc;

    x1 = s->x1; y1 = s->y1; x2 = s->x2; y2 = s->y2;

    dx = abs(x2 - x1);
    dy = abs(y2 - y1);

    xinc = (x1 > x2) ? -1 : 1;
    yinc = (y1 > y2) ? -1 : 1;

    if(dx >= dy)
    {
        int dpr = dy << 1;
        int dpru = dpr - (dx << 1);
        int delta = dpr - dx;

        for(; dx>=0; dx--)
        {
            caca_put_char(cv, x1, y1, s->ch);
            if(delta > 0)
            {
                x1 += xinc;
                y1 += yinc;
                delta += dpru;
            }
            else
            {
                x1 += xinc;
                delta += dpr;
            }
        }
    }
    else
    {
        int dpr = dx << 1;
        int dpru = dpr - (dy << 1);
        int delta = dpr - dy;

        for(; dy >= 0; dy--)
        {
            caca_put_char(cv, x1, y1, s->ch);
            if(delta > 0)
            {
                x1 += xinc;
                y1 += yinc;
                delta += dpru;
            }
            else
            {
                y1 += yinc;
                delta += dpr;
            }
        }
    }
}

/* Thin line drawing function, using Bresenham's mid-point line
 * scan-conversion algorithm and ASCII art graphics. */
static void draw_thin_line(caca_canvas_t *cv, struct line* s)
{
    uint32_t charmapx[2], charmapy[2];
    int x1, y1, x2, y2;
    int dx, dy;
    int yinc;

    if(s->x2 >= s->x1)
    {
        charmapx[0] = (s->y1 > s->y2) ? ',' : '`';
        charmapx[1] = (s->y1 > s->y2) ? '\'' : '.';
        x1 = s->x1; y1 = s->y1; x2 = s->x2; y2 = s->y2;
    }
    else
    {
        charmapx[0] = (s->y1 > s->y2) ? '`' : '.';
        charmapx[1] = (s->y1 > s->y2) ? ',' : '\'';
        x2 = s->x1; y2 = s->y1; x1 = s->x2; y1 = s->y2;
    }

    dx = abs(x2 - x1);
    dy = abs(y2 - y1);

    if(y1 > y2)
    {
        charmapy[0] = ',';
        charmapy[1] = '\'';
        yinc = -1;
    }
    else
    {
        yinc = 1;
        charmapy[0] = '`';
        charmapy[1] = '.';
    }

    if(dx >= dy)
    {
        int dpr = dy << 1;
        int dpru = dpr - (dx << 1);
        int delta = dpr - dx;
        int prev = 0;

        for(; dx>=0; dx--)
        {
            if(delta > 0)
            {
                caca_put_char(cv, x1, y1, charmapy[1]);
                x1++;
                y1 += yinc;
                delta += dpru;
                prev = 1;
            }
            else
            {
                if(prev)
                    caca_put_char(cv, x1, y1, charmapy[0]);
                else
                    caca_put_char(cv, x1, y1, '-');
                x1++;
                delta += dpr;
                prev = 0;
            }
        }
    }
    else
    {
        int dpr = dx << 1;
        int dpru = dpr - (dy << 1);
        int delta = dpr - dy;

        for(; dy >= 0; dy--)
        {
            if(delta > 0)
            {
                caca_put_char(cv, x1, y1, charmapx[0]);
                caca_put_char(cv, x1 + 1, y1, charmapx[1]);
                x1++;
                y1 += yinc;
                delta += dpru;
            }
            else
            {
                caca_put_char(cv, x1, y1, '|');
                y1 += yinc;
                delta += dpr;
            }
        }
    }
}


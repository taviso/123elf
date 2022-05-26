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
 *  This file contains triangle drawing functions, both filled and outline.
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <stdlib.h>
#endif

#include "ttydraw.h"
#include "ttyint.h"

/** \brief Draw a triangle on the canvas using the given character.
 *
 *  This function never fails.
 *
 *  \param cv The handle to the libcaca canvas.
 *  \param x1 X coordinate of the first point.
 *  \param y1 Y coordinate of the first point.
 *  \param x2 X coordinate of the second point.
 *  \param y2 Y coordinate of the second point.
 *  \param x3 X coordinate of the third point.
 *  \param y3 Y coordinate of the third point.
 *  \param ch UTF-32 character to be used to draw the triangle outline.
 *  \return This function always returns 0.
 */
int caca_draw_triangle(caca_canvas_t * cv, int x1, int y1, int x2, int y2,
                       int x3, int y3, uint32_t ch)
{
    caca_draw_line(cv, x1, y1, x2, y2, ch);
    caca_draw_line(cv, x2, y2, x3, y3, ch);
    caca_draw_line(cv, x3, y3, x1, y1, ch);

    return 0;
}

/** \brief Draw a thin triangle on the canvas.
 *
 *  This function never fails.
 *
 *  \param cv The handle to the libcaca canvas.
 *  \param x1 X coordinate of the first point.
 *  \param y1 Y coordinate of the first point.
 *  \param x2 X coordinate of the second point.
 *  \param y2 Y coordinate of the second point.
 *  \param x3 X coordinate of the third point.
 *  \param y3 Y coordinate of the third point.
 *  \return This function always returns 0.
 */
int caca_draw_thin_triangle(caca_canvas_t * cv, int x1, int y1,
                            int x2, int y2, int x3, int y3)
{
    caca_draw_thin_line(cv, x1, y1, x2, y2);
    caca_draw_thin_line(cv, x2, y2, x3, y3);
    caca_draw_thin_line(cv, x3, y3, x1, y1);

    return 0;
}

/** \brief Fill a triangle on the canvas using the given character.
 *
 *  This function never fails.
 *
 *  \param cv The handle to the libcaca canvas.
 *  \param x1 X coordinate of the first point.
 *  \param y1 Y coordinate of the first point.
 *  \param x2 X coordinate of the second point.
 *  \param y2 Y coordinate of the second point.
 *  \param x3 X coordinate of the third point.
 *  \param y3 Y coordinate of the third point.
 *  \param ch UTF-32 character to be used to fill the triangle.
 *  \return This function always returns 0.
 */
int caca_fill_triangle(caca_canvas_t * cv, int x1, int y1, int x2, int y2,
                       int x3, int y3, uint32_t ch)
{
    int x, y, xmin, xmax, ymin, ymax;
    int xx1, xx2, xa, xb, sl21, sl31, sl32;

    /* Bubble-sort y1 <= y2 <= y3 */
    if (y1 > y2)
        return caca_fill_triangle(cv, x2, y2, x1, y1, x3, y3, ch);

    if (y2 > y3)
        return caca_fill_triangle(cv, x1, y1, x3, y3, x2, y2, ch);

    /* Compute slopes and promote precision */
    sl21 = (y2 == y1) ? 0 : (x2 - x1) * 0x10000 / (y2 - y1);
    sl31 = (y3 == y1) ? 0 : (x3 - x1) * 0x10000 / (y3 - y1);
    sl32 = (y3 == y2) ? 0 : (x3 - x2) * 0x10000 / (y3 - y2);

    x1 *= 0x10000;
    x2 *= 0x10000;
    x3 *= 0x10000;

    ymin = y1 < 0 ? 0 : y1;
    ymax = y3 + 1 < cv->height ? y3 + 1 : cv->height;

    if (ymin < y2)
    {
        xa = x1 + sl21 * (ymin - y1);
        xb = x1 + sl31 * (ymin - y1);
    }
    else if (ymin == y2)
    {
        xa = x2;
        xb = (y1 == y3) ? x3 : x1 + sl31 * (ymin - y1);
    }
    else                        /* (ymin > y2) */
    {
        xa = x3 + sl32 * (ymin - y3);
        xb = x3 + sl31 * (ymin - y3);
    }

    /* Rasterize our triangle */
    for (y = ymin; y < ymax; y++)
    {
        /* Rescale xa and xb, recentering the division */
        if (xa < xb)
        {
            xx1 = (xa + 0x800) / 0x10000;
            xx2 = (xb + 0x801) / 0x10000;
        }
        else
        {
            xx1 = (xb + 0x800) / 0x10000;
            xx2 = (xa + 0x801) / 0x10000;
        }

        xmin = xx1 < 0 ? 0 : xx1;
        xmax = xx2 + 1 < cv->width ? xx2 + 1 : cv->width;

        for (x = xmin; x < xmax; x++)
            caca_put_char(cv, x, y, ch);

        xa += y < y2 ? sl21 : sl32;
        xb += sl31;
    }

    return 0;
}


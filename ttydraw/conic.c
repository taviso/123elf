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
 *  This file contains ellipse and circle drawing functions, both filled
 *  and outline.
 */

#include "config.h"

#if !defined(__KERNEL__)
#   include <stdlib.h>
#endif

#include "ttydraw.h"
#include "ttyint.h"

static void ellipsepoints(caca_canvas_t *, int, int, int, int, uint32_t, int);

/** \brief Draw a circle on the canvas using the given character.
 *
 *  This function never fails.
 *
 *  \param cv The handle to the libcaca canvas.
 *  \param x Center X coordinate.
 *  \param y Center Y coordinate.
 *  \param r Circle radius.
 *  \param ch UTF-32 character to be used to draw the circle outline.
 *  \return This function always returns 0.
 */
int caca_draw_circle(caca_canvas_t *cv, int x, int y, int r, uint32_t ch)
{
    int test, dx, dy;

    /* Optimized Bresenham. Kick ass. */
    for(test = 0, dx = 0, dy = r ; dx <= dy ; dx++)
    {
        ellipsepoints(cv, x, y, dx, dy, ch, 1);
        ellipsepoints(cv, x, y, dy, dx, ch, 1);

        test += test > 0 ? dx - dy-- : dx;
    }

    return 0;
}

/** \brief Fill an ellipse on the canvas using the given character.
 *
 *  This function never fails.
 *
 *  \param cv The handle to the libcaca canvas.
 *  \param xo Center X coordinate.
 *  \param yo Center Y coordinate.
 *  \param a Ellipse X radius.
 *  \param b Ellipse Y radius.
 *  \param ch UTF-32 character to be used to fill the ellipse.
 *  \return This function always returns 0.
 */
int caca_fill_ellipse(caca_canvas_t *cv, int xo, int yo, int a, int b,
                       uint32_t ch)
{
    int d2;
    int x = 0;
    int y = b;
    int d1 = b*b - (a*a*b) + (a*a/4);

    while(a*a*y - a*a/2 > b*b*(x+1))
    {
        if(d1 < 0)
        {
            d1 += b*b*(2*x+1); /* XXX: "Computer Graphics" has + 3 here. */
        }
        else
        {
            d1 += b*b*(2*x*1) + a*a*(-2*y+2);
            caca_draw_line(cv, xo - x, yo - y, xo + x, yo - y, ch);
            caca_draw_line(cv, xo - x, yo + y, xo + x, yo + y, ch);
            y--;
        }
        x++;
    }

    caca_draw_line(cv, xo - x, yo - y, xo + x, yo - y, ch);
    caca_draw_line(cv, xo - x, yo + y, xo + x, yo + y, ch);

    d2 = b*b*(x+0.5)*(x+0.5) + a*a*(y-1)*(y-1) - a*a*b*b;
    while(y > 0)
    {
        if(d2 < 0)
        {
            d2 += b*b*(2*x+2) + a*a*(-2*y+3);
            x++;
        }
        else
        {
            d2 += a*a*(-2*y+3);
        }

        y--;
        caca_draw_line(cv, xo - x, yo - y, xo + x, yo - y, ch);
        caca_draw_line(cv, xo - x, yo + y, xo + x, yo + y, ch);
    }

    return 0;
}

/** \brief Draw an ellipse on the canvas using the given character.
 *
 *  This function never fails.
 *
 *  \param cv The handle to the libcaca canvas.
 *  \param xo Center X coordinate.
 *  \param yo Center Y coordinate.
 *  \param a Ellipse X radius.
 *  \param b Ellipse Y radius.
 *  \param ch UTF-32 character to be used to draw the ellipse outline.
 *  \return This function always returns 0.
 */
int caca_draw_ellipse(caca_canvas_t *cv, int xo, int yo, int a, int b,
                       uint32_t ch)
{
    int d2;
    int x = 0;
    int y = b;
    int d1 = b*b - (a*a*b) + (a*a/4);

    ellipsepoints(cv, xo, yo, x, y, ch, 0);

    while(a*a*y - a*a/2 > b*b*(x+1))
    {
        if(d1 < 0)
        {
            d1 += b*b*(2*x+1); /* XXX: "Computer Graphics" has + 3 here. */
        }
        else
        {
            d1 += b*b*(2*x*1) + a*a*(-2*y+2);
            y--;
        }
        x++;
        ellipsepoints(cv, xo, yo, x, y, ch, 0);
    }

    d2 = b*b*(x+0.5)*(x+0.5) + a*a*(y-1)*(y-1) - a*a*b*b;
    while(y > 0)
    {
        if(d2 < 0)
        {
            d2 += b*b*(2*x+2) + a*a*(-2*y+3);
            x++;
        }
        else
        {
            d2 += a*a*(-2*y+3);
        }

        y--;
        ellipsepoints(cv, xo, yo, x, y, ch, 0);
    }

    return 0;
}

/** \brief Draw a thin ellipse on the canvas.
 *
 *  This function never fails.
 *
 *  \param cv The handle to the libcaca canvas.
 *  \param xo Center X coordinate.
 *  \param yo Center Y coordinate.
 *  \param a Ellipse X radius.
 *  \param b Ellipse Y radius.
 *  \return This function always returns 0.
 */
int caca_draw_thin_ellipse(caca_canvas_t *cv, int xo, int yo, int a, int b)
{
    /* FIXME: this is not correct */
    int d2;
    int x = 0;
    int y = b;
    int d1 = b*b - (a*a*b) + (a*a/4);

    ellipsepoints(cv, xo, yo, x, y, '-', 1);

    while(a*a*y - a*a/2 > b*b*(x+1))
    {
        if(d1 < 0)
        {
            d1 += b*b*(2*x+1); /* XXX: "Computer Graphics" has + 3 here. */
            ellipsepoints(cv, xo, yo, x + 1, y, '0', 1);
        }
        else
        {
            d1 += b*b*(2*x*1) + a*a*(-2*y+2);
            y--;
            ellipsepoints(cv, xo, yo, x + 1, y, '1', 1);
        }
        x++;


    }

    d2 = b*b*(x+0.5)*(x+0.5) + a*a*(y-1)*(y-1) - a*a*b*b;
    while(y > 0)
    {
        if(d2 < 0)
        {
            d2 += b*b*(2*x+2) + a*a*(-2*y+3);
            x++;
            ellipsepoints(cv, xo, yo, x , y - 1, '2', 1);
        }
        else
        {
            d2 += a*a*(-2*y+3);
            ellipsepoints(cv, xo, yo, x , y - 1, '3', 1);
        }

        y--;


    }

    return 0;
}

static void ellipsepoints(caca_canvas_t *cv, int xo, int yo, int x, int y,
                          uint32_t ch, int thin)
{
    uint8_t b = 0;

    if(xo + x >= 0 && xo + x < (int)cv->width)
        b |= 0x1;
    if(xo - x >= 0 && xo - x < (int)cv->width)
        b |= 0x2;
    if(yo + y >= 0 && yo + y < (int)cv->height)
        b |= 0x4;
    if(yo - y >= 0 && yo - y < (int)cv->height)
        b |= 0x8;

    if((b & (0x1|0x4)) == (0x1|0x4)) {
        uint32_t c = ch;

        if(thin) {
            switch(c) {
            case '0':
                c = '-';
                break;
            case '1':
                c = ',';
                break;
            case '2':
                c = '/';
                break;
            case '3':
                c = '|';
                break;
            }

        }
        caca_put_char(cv, xo + x, yo + y, c);
    }
    if((b & (0x2|0x4)) == (0x2|0x4)) {
        uint32_t c = ch;

        if(thin) {
            switch(c) {
            case '0':
                c = '-';
                break;
            case '1':
                c = '.';
                break;
            case '2':
                c = '\\';
                break;
            case '3':
                c = '|';
                break;
            }

        }
        caca_put_char(cv, xo - x, yo + y, c);
    }


    if((b & (0x1|0x8)) == (0x1|0x8)) {
        uint32_t c = ch;

        if(thin) {
            switch(c) {
            case '0':
                c = '-';
                break;
            case '1':
                c = '`';
                break;
            case '2':
                c = '\\';
                break;
            case '3':
                c = '|';
                break;
            }

        }
        caca_put_char(cv, xo + x, yo - y, c);
    }

    if((b & (0x2|0x8)) == (0x2|0x8)) {
        uint32_t c = ch;

        if(thin) {
            switch(c) {
            case '0':
                c = '-';
                break;
            case '1':
                c = '\'';
                break;
            case '2':
                c = '/';
                break;
            case '3':
                c = '|';
                break;
            }

        }
        caca_put_char(cv, xo - x, yo - y, c);
    }
}


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
 *  This file contains functions for attribute management and colourspace
 *  conversions.
 */

#include <stdlib.h>
#include "config.h"

#include "ttydraw.h"
#include "ttyint.h"

int caca_set_attr(caca_canvas_t *cv, uint32_t attr)
{
    cv->curattr = attr;
    return 0;
}

uint32_t caca_get_attr(caca_canvas_t const *cv)
{
    return cv->curattr;
}

uint8_t caca_attr_from_bg_fg(uint8_t fg, uint8_t bg)
{
    bg  &= 7;
    bg <<= 3;
    fg  &= 7;
    return fg | bg;
}

int caca_set_attr_bg_fg(caca_canvas_t *cv, uint8_t fg, uint8_t bg)
{
    return caca_set_attr(cv, caca_attr_from_bg_fg(fg, bg));
}


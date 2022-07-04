//
// This file is derived from libcaca, but modified to add simple drawing
// primitives to a CGA framebuffer for Lotus 1-2-3.
//
// The original copyright notice is below.

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

#include "config.h"
#include <stddef.h>
#include <string.h>

#include "ttydraw.h"
#include "ttyint.h"

caca_canvas_t * caca_create_canvas(int width, int height)
{
    static caca_canvas_t canvas;
    static struct caca_frame frame;
    caca_canvas_t * cv = &canvas;

    if (cv->refcount != 0) {
        // It is not currently possible to change the width/height.
        if (cv->frames[0].width != width || cv->frames[0].height != height)
            return NULL;
        // Just increment the refcount.
        cv->refcount++;
        return cv;
    }

    if (width < 0 || height < 0) {
        seterrno(EINVAL);
        return NULL;
    }

    cv->refcount++;
    cv->autoinc = 0;
    cv->resize_callback = NULL;
    cv->resize_data = NULL;

    cv->frame = 0;
    cv->framecount = 1;
    cv->frames = &frame;
    cv->frames[0].width = width;
    cv->frames[0].height = height;
    cv->frames[0].x = cv->frames[0].y = 0;
    cv->frames[0].handlex = cv->frames[0].handley = 0;
    cv->frames[0].curattr = 0;
    cv->frames[0].name = "frame#00000000";

    _caca_load_frame_info(cv);

    cv->ndirty = 0;
    cv->dirty_disabled = 1;
    cv->ff = NULL;
    return cv;
}

int caca_get_canvas_width(caca_canvas_t const *cv)
{
    return cv->width;
}

int caca_get_canvas_height(caca_canvas_t const *cv)
{
    return cv->height;
}

int caca_free_canvas(caca_canvas_t *cv)
{
    if (cv) {
        cv->refcount--;
    }
    return 0;
}


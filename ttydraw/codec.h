/*
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2002-2014 Sam Hocevar <sam@hocevar.net>
 *                2006 Jean-Yves Lamoureux <jylam@lnxscene.org>
 *                All Rights Reserved
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

ssize_t _import_text(caca_canvas_t *, void const *, size_t);
ssize_t _import_ansi(caca_canvas_t *, void const *, size_t, int);
ssize_t _import_bin(caca_canvas_t *, void const *, size_t);

void *_export_ansi(caca_canvas_t const *, size_t *);
void *_export_plain(caca_canvas_t const *, size_t *);
void *_export_utf8(caca_canvas_t const *, size_t *, int);
void *_export_irc(caca_canvas_t const *, size_t *);


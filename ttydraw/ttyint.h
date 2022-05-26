/*
 *  libcaca       Colour ASCII-Art library
 *  Copyright (c) 2002-2012 Sam Hocevar <sam@hocevar.net>
 *                All Rights Reserved
 *
 *  This library is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What the Fuck You Want
 *  to Public License, Version 2, as published by Sam Hocevar. See
 *  http://www.wtfpl.net/ for more details.
 */

#ifndef __CACA_INTERNALS_H__
#define __CACA_INTERNALS_H__

#include <errno.h>

#include "cacastub.h"

typedef struct caca_timer caca_timer_t;
typedef struct caca_privevent caca_privevent_t;

#if !defined(_DOXYGEN_SKIP_ME)
#   define STAT_VALUES 32
#   define EVENTBUF_LEN 10
#   define MAX_DIRTY_COUNT 8
#endif

struct caca_frame
{
    /* Frame size */
    int width, height;

    /* Painting context */
    int x, y;
    int handlex, handley;
    uint32_t curattr;

    /* Frame name */
    char *name;
};

struct caca_canvas
{
    /* XXX: look at caca_set_canvas_boundaries() before adding anything
     * to this structure. The function is quite hacky. */

    /* Frame information */
    int frame, framecount;
    struct caca_frame *frames;

    /* Canvas management */
    int refcount;
    int autoinc;
    int (*resize_callback)(void *);
    void *resize_data;

    /* Dirty rectangles */
    int ndirty, dirty_disabled;
    struct
    {
        int xmin, ymin, xmax, ymax;
    }
    dirty[MAX_DIRTY_COUNT + 1];

    /* Shortcut to the active frame information */
    int width, height;
    uint32_t curattr;

    /* FIGfont management */
    caca_charfont_t *ff;
};

/* Graphics driver */
enum caca_driver
{
    CACA_DRIVER_NULL = 0,
    CACA_DRIVER_RAW = 1,
#if defined(USE_COCOA)
    CACA_DRIVER_COCOA = 2,
#endif
#if defined(USE_CONIO)
    CACA_DRIVER_CONIO = 3,
#endif
#if defined(USE_GL)
    CACA_DRIVER_GL = 4,
#endif
#if defined(USE_NCURSES)
    CACA_DRIVER_NCURSES = 5,
#endif
#if defined(USE_SLANG)
    CACA_DRIVER_SLANG = 6,
#endif
#if defined(USE_VGA)
    CACA_DRIVER_VGA = 7,
#endif
#if defined(USE_WIN32)
    CACA_DRIVER_WIN32 = 8,
#endif
#if defined(USE_X11)
    CACA_DRIVER_X11 = 9,
#endif
};

/* Available external drivers */
#if defined(USE_COCOA)
int cocoa_install(caca_display_t *);
#endif
#if defined(USE_CONIO)
int conio_install(caca_display_t *);
#endif
#if defined(USE_GL)
int gl_install(caca_display_t *);
#endif
#if defined(USE_NCURSES)
int ncurses_install(caca_display_t *);
#endif
int null_install(caca_display_t *);
int raw_install(caca_display_t *);
#if defined(USE_SLANG)
int slang_install(caca_display_t *);
#endif
#if defined(USE_VGA)
int vga_install(caca_display_t *);
#endif
#if defined(USE_WIN32)
int win32_install(caca_display_t *);
#endif
#if defined(USE_X11)
int x11_install(caca_display_t *);
#endif

/* Timer structure */
struct caca_timer
{
    int last_sec, last_usec;
};

/* Statistic structure for profiling */
struct caca_stat
{
    int itable[STAT_VALUES];
    int32_t imean;
    char *name;
};

/* Private event structure */
struct caca_privevent
{
    enum caca_event_type type;

    union
    {
        struct { int x, y, button; } mouse;
        struct { int w, h; } resize;
        struct { int ch; uint32_t utf32; char utf8[8]; } key;
    } data;
};

/* Internal caca display context */
struct caca_display
{
    /* A link to our caca canvas */
    caca_canvas_t *cv;
    int autorelease;

#if defined(USE_PLUGINS)
    void *plugin;
#endif

    /* Device-specific functions */
    struct drv
    {
        char const * driver;
        enum caca_driver id;
        struct driver_private *p;

        int (* init_graphics) (caca_display_t *);
        int (* end_graphics) (caca_display_t *);
        int (* set_display_title) (caca_display_t *, char const *);
        int (* get_display_width) (caca_display_t const *);
        int (* get_display_height) (caca_display_t const *);
        void (* display) (caca_display_t *);
        void (* handle_resize) (caca_display_t *);
        int (* get_event) (caca_display_t *, caca_privevent_t *);
        void (* set_mouse) (caca_display_t *, int);
        void (* set_cursor) (caca_display_t *, int);
    } drv;

    /* Mouse position */
    struct mouse
    {
        int x, y;
    } mouse;

    /* Window resize handling */
    struct resize
    {
        int resized;   /* A resize event was requested */
        int allow;     /* The display driver allows resizing */
        int w, h; /* Requested width and height */
    } resize;

    /* Framerate handling */
    int delay, rendertime;
    caca_timer_t timer;
#if defined PROF
    struct caca_stat display_stat, wait_stat;
    struct caca_stat ev_sys_stat, ev_wait_stat;
#endif
    int lastticks;

    struct events
    {
#if defined(USE_SLANG) || defined(USE_NCURSES) || defined(USE_CONIO) || defined(USE_GL)
        caca_privevent_t buf[EVENTBUF_LEN];
        int queue;
#endif
#if defined(USE_SLANG) || defined(USE_NCURSES)
        caca_timer_t key_timer;
        int last_key_ticks;
        int autorepeat_ticks;
        caca_privevent_t last_key_event;
#endif
        uint8_t not_empty_struct;
    } events;
};

/* Dirty rectangle functions */
extern void _caca_clip_dirty_rect_list(caca_canvas_t *);

/* Colour functions */
extern uint32_t _caca_attr_to_rgb24fg(uint32_t);
extern uint32_t _caca_attr_to_rgb24bg(uint32_t);

/* Frames functions */
extern void _caca_save_frame_info(caca_canvas_t *);
extern void _caca_load_frame_info(caca_canvas_t *);

/* Internal timer functions */
extern void _caca_sleep(int);
extern int _caca_getticks(caca_timer_t *);

/* Internal event functions */
extern void _caca_handle_resize(caca_display_t *);
#if defined(USE_SLANG) || defined(USE_NCURSES) || defined(USE_CONIO) || defined(USE_GL)
extern void _push_event(caca_display_t *, caca_privevent_t *);
extern int _pop_event(caca_display_t *, caca_privevent_t *);
#endif

/* Internal window functions */
extern void _caca_set_term_title(char const *);

/* Profiling functions */
#if defined PROF
extern void _caca_dump_stats(void);
extern void _caca_init_stat(struct caca_stat *, char const *, ...);
extern void _caca_fini_stat(struct caca_stat *);
#endif

#endif /* __CACA_INTERNALS_H__ */

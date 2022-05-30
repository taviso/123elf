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

/** \file caca.h
 *  \author Sam Hocevar <sam@hocevar.net>
 *  \brief The \e libcaca public header.
 *
 *  This header contains the public types and functions that applications
 *  using \e libcaca may use.
 */

#ifndef __CACA_H__
#define __CACA_H__

#include <stdint.h>
#include <sys/types.h>

#undef __extern
#if defined _DOXYGEN_SKIP_ME
#elif defined _WIN32 && defined __LIBCACA__ && defined DLL_EXPORT
#   define __extern extern __declspec(dllexport)
#elif defined _WIN32 && !defined __LIBCACA__ && !defined CACA_STATIC
#   define __extern extern __declspec(dllimport)
#elif defined CACA_ENABLE_VISIBILITY
#   define __extern extern __attribute__((visibility("default")))
#else
#   define __extern extern
#endif

/** libcaca API version */
#define CACA_API_VERSION_1

#ifdef __cplusplus
extern "C"
{
#endif

/** \e libcaca canvas */
typedef struct caca_canvas caca_canvas_t;
/** dither structure */
typedef struct caca_dither caca_dither_t;
/** character font structure */
typedef struct caca_charfont caca_charfont_t;
/** bitmap font structure */
typedef struct caca_font caca_font_t;
/** file handle structure */
typedef struct caca_file caca_file_t;
/** \e libcaca display context */
typedef struct caca_display caca_display_t;
/** \e libcaca event structure */
typedef struct caca_event caca_event_t;

/** \defgroup caca_attr libcaca attribute definitions
 *
 *  Colours and styles that can be used with caca_set_attr().
 *
 *  @{ */
/** \e libcaca colour keyword */
enum caca_color
{
    CACA_BLACK =        0x00, /**< The colour index for black. */
    CACA_BLUE =         0x01, /**< The colour index for blue. */
    CACA_GREEN =        0x02, /**< The colour index for green. */
    CACA_CYAN =         0x03, /**< The colour index for cyan. */
    CACA_RED =          0x04, /**< The colour index for red. */
    CACA_MAGENTA =      0x05, /**< The colour index for magenta. */
    CACA_BROWN =        0x06, /**< The colour index for brown. */
    CACA_LIGHTGRAY =    0x07, /**< The colour index for light gray. */
    CACA_DARKGRAY =     0x08, /**< The colour index for dark gray. */
    CACA_LIGHTBLUE =    0x09, /**< The colour index for blue. */
    CACA_LIGHTGREEN =   0x0a, /**< The colour index for light green. */
    CACA_LIGHTCYAN =    0x0b, /**< The colour index for light cyan. */
    CACA_LIGHTRED =     0x0c, /**< The colour index for light red. */
    CACA_LIGHTMAGENTA = 0x0d, /**< The colour index for light magenta. */
    CACA_YELLOW =       0x0e, /**< The colour index for yellow. */
    CACA_WHITE =        0x0f, /**< The colour index for white. */
    CACA_DEFAULT =      0x10, /**< The output driver's default colour. */
    CACA_TRANSPARENT =  0x20, /**< The transparent colour. */
};

/** \e libcaca style keyword */
enum caca_style
{
    CACA_BOLD =      0x01, /**< The style mask for bold. */
    CACA_ITALICS =   0x02, /**< The style mask for italics. */
    CACA_UNDERLINE = 0x04, /**< The style mask for underline. */
    CACA_BLINK =     0x08, /**< The style mask for blink. */
};
/*  @} */

/** \brief User event type enumeration.
 *
 *  This enum serves two purposes:
 *  - Build listening masks for caca_get_event().
 *  - Define the type of a \e caca_event_t.
 */
enum caca_event_type
{
    CACA_EVENT_NONE =          0x0000, /**< No event. */

    CACA_EVENT_KEY_PRESS =     0x0001, /**< A key was pressed. */
    CACA_EVENT_KEY_RELEASE =   0x0002, /**< A key was released. */
    CACA_EVENT_MOUSE_PRESS =   0x0004, /**< A mouse button was pressed. */
    CACA_EVENT_MOUSE_RELEASE = 0x0008, /**< A mouse button was released. */
    CACA_EVENT_MOUSE_MOTION =  0x0010, /**< The mouse was moved. */
    CACA_EVENT_RESIZE =        0x0020, /**< The window was resized. */
    CACA_EVENT_QUIT =          0x0040, /**< The user requested to quit. */

};

#define CACA_EVENT_ANY 0xffff  /**< Bitmask for any event. */

/** \brief Handling of user events.
 *
 *  This structure is filled by caca_get_event() when an event is received.
 *  It is an opaque structure that should only be accessed through
 *  caca_event_get_type() and similar functions. The struct members may no
 *  longer be directly accessible in future versions.
 */
struct caca_event
{
    enum caca_event_type type; /**< The event type. */
    union
    {
        struct { int x, y, button; } mouse;
        struct { int w, h; } resize;
        struct { int ch; uint32_t utf32; char utf8[8]; } key;
    } data; /**< The event information data */
#if !defined(_DOXYGEN_SKIP_ME)
    uint8_t padding[16];
#endif
};

/** \brief Option parsing.
 *
 * This structure contains commandline parsing information for systems
 * where getopt_long() is unavailable.
 */
struct caca_option
{
    char const *name;
    int has_arg;
    int *flag;
    int val;
};

/** \brief Special key values.
 *
 *  Special key values returned by caca_get_event() for which there is no
 *  printable ASCII equivalent.
 */
enum caca_key
{
    CACA_KEY_UNKNOWN = 0x00, /**< Unknown key. */

    /* The following keys have ASCII equivalents */
    CACA_KEY_CTRL_A =    0x01, /**< The Ctrl-A key. */
    CACA_KEY_CTRL_B =    0x02, /**< The Ctrl-B key. */
    CACA_KEY_CTRL_C =    0x03, /**< The Ctrl-C key. */
    CACA_KEY_CTRL_D =    0x04, /**< The Ctrl-D key. */
    CACA_KEY_CTRL_E =    0x05, /**< The Ctrl-E key. */
    CACA_KEY_CTRL_F =    0x06, /**< The Ctrl-F key. */
    CACA_KEY_CTRL_G =    0x07, /**< The Ctrl-G key. */
    CACA_KEY_BACKSPACE = 0x08, /**< The backspace key. */
    CACA_KEY_TAB =       0x09, /**< The tabulation key. */
    CACA_KEY_CTRL_J =    0x0a, /**< The Ctrl-J key. */
    CACA_KEY_CTRL_K =    0x0b, /**< The Ctrl-K key. */
    CACA_KEY_CTRL_L =    0x0c, /**< The Ctrl-L key. */
    CACA_KEY_RETURN =    0x0d, /**< The return key. */
    CACA_KEY_CTRL_N =    0x0e, /**< The Ctrl-N key. */
    CACA_KEY_CTRL_O =    0x0f, /**< The Ctrl-O key. */
    CACA_KEY_CTRL_P =    0x10, /**< The Ctrl-P key. */
    CACA_KEY_CTRL_Q =    0x11, /**< The Ctrl-Q key. */
    CACA_KEY_CTRL_R =    0x12, /**< The Ctrl-R key. */
    CACA_KEY_PAUSE =     0x13, /**< The pause key. */
    CACA_KEY_CTRL_T =    0x14, /**< The Ctrl-T key. */
    CACA_KEY_CTRL_U =    0x15, /**< The Ctrl-U key. */
    CACA_KEY_CTRL_V =    0x16, /**< The Ctrl-V key. */
    CACA_KEY_CTRL_W =    0x17, /**< The Ctrl-W key. */
    CACA_KEY_CTRL_X =    0x18, /**< The Ctrl-X key. */
    CACA_KEY_CTRL_Y =    0x19, /**< The Ctrl-Y key. */
    CACA_KEY_CTRL_Z =    0x1a, /**< The Ctrl-Z key. */
    CACA_KEY_ESCAPE =    0x1b, /**< The escape key. */
    CACA_KEY_DELETE =    0x7f, /**< The delete key. */

    /* The following keys do not have ASCII equivalents but have been
     * chosen to match the SDL equivalents */
    CACA_KEY_UP =    0x111, /**< The up arrow key. */
    CACA_KEY_DOWN =  0x112, /**< The down arrow key. */
    CACA_KEY_LEFT =  0x113, /**< The left arrow key. */
    CACA_KEY_RIGHT = 0x114, /**< The right arrow key. */

    CACA_KEY_INSERT =   0x115, /**< The insert key. */
    CACA_KEY_HOME =     0x116, /**< The home key. */
    CACA_KEY_END =      0x117, /**< The end key. */
    CACA_KEY_PAGEUP =   0x118, /**< The page up key. */
    CACA_KEY_PAGEDOWN = 0x119, /**< The page down key. */

    CACA_KEY_F1 =  0x11a, /**< The F1 key. */
    CACA_KEY_F2 =  0x11b, /**< The F2 key. */
    CACA_KEY_F3 =  0x11c, /**< The F3 key. */
    CACA_KEY_F4 =  0x11d, /**< The F4 key. */
    CACA_KEY_F5 =  0x11e, /**< The F5 key. */
    CACA_KEY_F6 =  0x11f, /**< The F6 key. */
    CACA_KEY_F7 =  0x120, /**< The F7 key. */
    CACA_KEY_F8 =  0x121, /**< The F8 key. */
    CACA_KEY_F9 =  0x122, /**< The F9 key. */
    CACA_KEY_F10 = 0x123, /**< The F10 key. */
    CACA_KEY_F11 = 0x124, /**< The F11 key. */
    CACA_KEY_F12 = 0x125, /**< The F12 key. */
    CACA_KEY_F13 = 0x126, /**< The F13 key. */
    CACA_KEY_F14 = 0x127, /**< The F14 key. */
    CACA_KEY_F15 = 0x128  /**< The F15 key. */
};

/** \defgroup libcaca libcaca basic functions
 *
 *  These functions provide the basic \e libcaca routines for library
 *  initialisation, system information retrieval and configuration.
 *
 *  @{ */
__extern caca_canvas_t * caca_create_canvas(int, int);
__extern int caca_manage_canvas(caca_canvas_t *, int (*)(void *), void *);
__extern int caca_unmanage_canvas(caca_canvas_t *, int (*)(void *), void *);
__extern int caca_set_canvas_size(caca_canvas_t *, int, int);
__extern int caca_get_canvas_width(caca_canvas_t const *);
__extern int caca_get_canvas_height(caca_canvas_t const *);
__extern uint32_t const * caca_get_canvas_chars(caca_canvas_t const *);
__extern uint32_t const * caca_get_canvas_attrs(caca_canvas_t const *);
__extern int caca_free_canvas(caca_canvas_t *);
__extern int caca_rand(int, int);
__extern char const * caca_get_version(void);
/*  @} */

/** \defgroup caca_canvas libcaca canvas drawing
 *
 *  These functions provide low-level character printing routines and
 *  higher level graphics functions.
 *
 *  @{ */
#define CACA_MAGIC_FULLWIDTH 0x000ffffe /**< Used to indicate that the previous character was a fullwidth glyph. */
__extern int caca_gotoxy(caca_canvas_t *, int, int);
__extern int caca_wherex(caca_canvas_t const *);
__extern int caca_wherey(caca_canvas_t const *);
__extern int caca_put_char(caca_canvas_t *, int, int, uint32_t);
__extern uint32_t caca_get_char(caca_canvas_t const *, int, int);
__extern int caca_put_str(caca_canvas_t *, int, int, char const *);
__extern int caca_printf(caca_canvas_t *, int, int, char const *, ...);
__extern int caca_clear_canvas(caca_canvas_t *);
__extern int caca_set_canvas_handle(caca_canvas_t *, int, int);
__extern int caca_get_canvas_handle_x(caca_canvas_t const *);
__extern int caca_get_canvas_handle_y(caca_canvas_t const *);
__extern int caca_blit(caca_canvas_t *, int, int, caca_canvas_t const *,
                       caca_canvas_t const *);
__extern int caca_set_canvas_boundaries(caca_canvas_t *, int, int, int, int);
/*  @} */

/** \defgroup caca_dirty libcaca dirty rectangle manipulation
 *
 *  These functions manipulate dirty rectangles for optimised blitting.
 *  @{ */
__extern int caca_disable_dirty_rect(caca_canvas_t *);
__extern int caca_enable_dirty_rect(caca_canvas_t *);
__extern int caca_get_dirty_rect_count(caca_canvas_t *);
__extern int caca_get_dirty_rect(caca_canvas_t *, int, int *, int *,
                                 int *, int *);
__extern int caca_add_dirty_rect(caca_canvas_t *, int, int, int, int);
__extern int caca_remove_dirty_rect(caca_canvas_t *, int, int, int, int);
__extern int caca_clear_dirty_rect_list(caca_canvas_t *);
/*  @} */

/** \defgroup caca_transform libcaca canvas transformation
 *
 *  These functions perform horizontal and vertical canvas flipping.
 *
 *  @{ */
__extern int caca_invert(caca_canvas_t *);
__extern int caca_flip(caca_canvas_t *);
__extern int caca_flop(caca_canvas_t *);
__extern int caca_rotate_180(caca_canvas_t *);
__extern int caca_rotate_left(caca_canvas_t *);
__extern int caca_rotate_right(caca_canvas_t *);
__extern int caca_stretch_left(caca_canvas_t *);
__extern int caca_stretch_right(caca_canvas_t *);
/*  @} */

/** \defgroup caca_attributes libcaca attribute conversions
 *
 *  These functions perform conversions between attribute values.
 *
 *  @{ */
__extern uint32_t caca_get_attr(caca_canvas_t const *);
__extern uint8_t caca_attr_from_bg_fg(uint8_t fg, uint8_t bg);
__extern int caca_set_attr(caca_canvas_t *, uint32_t);
__extern int caca_unset_attr(caca_canvas_t *, uint32_t);
__extern int caca_toggle_attr(caca_canvas_t *, uint32_t);
__extern int caca_put_attr(caca_canvas_t *, int, int, uint32_t);
__extern int caca_set_color_ansi(caca_canvas_t *, uint8_t, uint8_t);
__extern int caca_set_color_argb(caca_canvas_t *, uint16_t, uint16_t);
__extern uint8_t caca_attr_to_ansi(uint32_t);
__extern uint8_t caca_attr_to_ansi_fg(uint32_t);
__extern uint8_t caca_attr_to_ansi_bg(uint32_t);
__extern uint16_t caca_attr_to_rgb12_fg(uint32_t);
__extern uint16_t caca_attr_to_rgb12_bg(uint32_t);
__extern void caca_attr_to_argb64(uint32_t, uint8_t[8]);
/*  @} */

/** \defgroup caca_charset libcaca character set conversions
 *
 *  These functions perform conversions between usual character sets.
 *
 *  @{ */
__extern uint32_t caca_utf8_to_utf32(char const *, size_t *);
__extern size_t caca_utf32_to_utf8(char *, uint32_t);
__extern uint8_t caca_utf32_to_cp437(uint32_t);
__extern uint32_t caca_cp437_to_utf32(uint8_t);
__extern char caca_utf32_to_ascii(uint32_t);
__extern int caca_utf32_is_fullwidth(uint32_t);
/*  @} */

/** \defgroup caca_primitives libcaca primitives drawing
 *
 *  These functions provide routines for primitive drawing, such as lines,
 *  boxes, triangles and ellipses.
 *
 *  @{ */
__extern int caca_draw_line(caca_canvas_t *, int, int, int, int, uint32_t);
__extern int caca_draw_polyline(caca_canvas_t *, int const x[],
                                 int const y[], int, uint32_t);
__extern int caca_draw_thin_line(caca_canvas_t *, int, int, int, int);
__extern int caca_draw_thin_polyline(caca_canvas_t *, int const x[],
                                      int const y[], int);

__extern int caca_draw_circle(caca_canvas_t *, int, int, int, uint32_t);
__extern int caca_draw_ellipse(caca_canvas_t *, int, int, int, int, uint32_t);
__extern int caca_draw_thin_ellipse(caca_canvas_t *, int, int, int, int);
__extern int caca_fill_ellipse(caca_canvas_t *, int, int, int, int, uint32_t);

__extern int caca_draw_box(caca_canvas_t *, int, int, int, int, uint32_t);
__extern int caca_draw_thin_box(caca_canvas_t *, int, int, int, int);
__extern int caca_draw_cp437_box(caca_canvas_t *, int, int, int, int);
__extern int caca_fill_box(caca_canvas_t *, int, int, int, int, uint32_t);

__extern int caca_draw_triangle(caca_canvas_t *, int, int, int, int, int,
                                 int, uint32_t);
__extern int caca_draw_thin_triangle(caca_canvas_t *, int, int, int, int,
                                      int, int);
__extern int caca_fill_triangle(caca_canvas_t *, int, int, int, int, int,
                                 int, uint32_t);
__extern int caca_fill_triangle_textured(caca_canvas_t *cv,
                                         int coords[6],
                                         caca_canvas_t *tex,
                                         float uv[6]);
/*  @} */

/** \defgroup caca_frame libcaca canvas frame handling
 *
 *  These functions provide high level routines for canvas frame insertion,
 *  removal, copying etc.
 *
 *  @{ */
__extern int caca_get_frame_count(caca_canvas_t const *);
__extern int caca_set_frame(caca_canvas_t *, int);
__extern char const *caca_get_frame_name(caca_canvas_t const *);
__extern int caca_set_frame_name(caca_canvas_t *, char const *);
__extern int caca_create_frame(caca_canvas_t *, int);
__extern int caca_free_frame(caca_canvas_t *, int);
/*  @} */

/** \defgroup caca_dither libcaca bitmap dithering
 *
 *  These functions provide high level routines for dither allocation and
 *  rendering.
 *
 *  @{ */
__extern caca_dither_t *caca_create_dither(int, int, int, int,
                                             uint32_t, uint32_t,
                                             uint32_t, uint32_t);
__extern int caca_set_dither_palette(caca_dither_t *,
                                      uint32_t r[], uint32_t g[],
                                      uint32_t b[], uint32_t a[]);
__extern int caca_set_dither_brightness(caca_dither_t *, float);
__extern float caca_get_dither_brightness(caca_dither_t const *);
__extern int caca_set_dither_gamma(caca_dither_t *, float);
__extern float caca_get_dither_gamma(caca_dither_t const *);
__extern int caca_set_dither_contrast(caca_dither_t *, float);
__extern float caca_get_dither_contrast(caca_dither_t const *);
__extern int caca_set_dither_antialias(caca_dither_t *, char const *);
__extern char const * const * caca_get_dither_antialias_list(caca_dither_t
                                                              const *);
__extern char const * caca_get_dither_antialias(caca_dither_t const *);
__extern int caca_set_dither_color(caca_dither_t *, char const *);
__extern char const * const * caca_get_dither_color_list(caca_dither_t
                                                          const *);
__extern char const * caca_get_dither_color(caca_dither_t const *);
__extern int caca_set_dither_charset(caca_dither_t *, char const *);
__extern char const * const * caca_get_dither_charset_list(caca_dither_t
                                                            const *);
__extern char const * caca_get_dither_charset(caca_dither_t const *);
__extern int caca_set_dither_algorithm(caca_dither_t *, char const *);
__extern char const * const * caca_get_dither_algorithm_list(caca_dither_t
                                                              const *);
__extern char const * caca_get_dither_algorithm(caca_dither_t const *);
__extern int caca_dither_bitmap(caca_canvas_t *, int, int, int, int,
                         caca_dither_t const *, void const *);
__extern int caca_free_dither(caca_dither_t *);
/*  @} */

/** \defgroup caca_charfont libcaca character font handling
 *
 *  These functions provide character font handling routines.
 *
 *  @{ */
__extern caca_charfont_t *caca_load_charfont(void const *, size_t);
__extern int caca_free_charfont(caca_charfont_t *);
/*  @} */

/** \defgroup caca_font libcaca bitmap font handling
 *
 *  These functions provide bitmap font handling routines and high quality
 *  canvas to bitmap rendering.
 *
 *  @{ */
__extern caca_font_t *caca_load_font(void const *, size_t);
__extern char const * const * caca_get_font_list(void);
__extern int caca_get_font_width(caca_font_t const *);
__extern int caca_get_font_height(caca_font_t const *);
__extern uint32_t const *caca_get_font_blocks(caca_font_t const *);
__extern int caca_render_canvas(caca_canvas_t const *, caca_font_t const *,
                                 void *, int, int, int);
__extern int caca_free_font(caca_font_t *);
/*  @} */

/** \defgroup caca_figfont libcaca FIGfont handling
 *
 *  These functions provide FIGlet and TOIlet font handling routines.
 *
 *  @{ */
__extern int caca_canvas_set_figfont(caca_canvas_t *, char const *);
__extern int caca_set_figfont_smush(caca_canvas_t *, char const *);
__extern int caca_set_figfont_width(caca_canvas_t *, int);
__extern int caca_put_figchar(caca_canvas_t *, uint32_t);
__extern int caca_flush_figlet(caca_canvas_t *);
/*  @} */

/** \defgroup caca_file libcaca file IO
 *
 *  These functions allow to read and write files in a platform-independent
 *  way.
 *  @{ */
__extern caca_file_t *caca_file_open(char const *, const char *);
__extern int caca_file_close(caca_file_t *);
__extern uint32_t caca_file_tell(caca_file_t *);
__extern size_t caca_file_read(caca_file_t *, void *, size_t);
__extern size_t caca_file_write(caca_file_t *, const void *, size_t);
__extern char * caca_file_gets(caca_file_t *, char *, int);
__extern int caca_file_eof(caca_file_t *);
/*  @} */

/** \defgroup caca_importexport libcaca importers/exporters from/to various
 *  formats
 *
 *  These functions import various file formats into a new canvas, or export
 *  the current canvas to various text formats.
 *
 *  @{ */
__extern ssize_t caca_import_canvas_from_memory(caca_canvas_t *, void const *,
                                                size_t, char const *);
__extern ssize_t caca_import_canvas_from_file(caca_canvas_t *, char const *,
                                              char const *);
__extern ssize_t caca_import_area_from_memory(caca_canvas_t *, int, int,
                                              void const *, size_t,
                                              char const *);
__extern ssize_t caca_import_area_from_file(caca_canvas_t *, int, int,
                                            char const *, char const *);
__extern char const * const * caca_get_import_list(void);
__extern void *caca_export_canvas_to_memory(caca_canvas_t const *,
                                            char const *, size_t *);
__extern void *caca_export_area_to_memory(caca_canvas_t const *, int, int,
                                          int, int, char const *, size_t *);
__extern char const * const * caca_get_export_list(void);
/*  @} */

/** \defgroup caca_display libcaca display functions
 *
 *  These functions provide the basic \e libcaca routines for display
 *  initialisation, system information retrieval and configuration.
 *
 *  @{ */
__extern caca_display_t * caca_create_display(caca_canvas_t *);
__extern caca_display_t * caca_create_display_with_driver(caca_canvas_t *,
                                                          char const *);
__extern char const * const * caca_get_display_driver_list(void);
__extern char const * caca_get_display_driver(caca_display_t *);
__extern int caca_set_display_driver(caca_display_t *, char const *);
__extern int caca_free_display(caca_display_t *);
__extern caca_canvas_t * caca_get_canvas(caca_display_t *);
__extern int caca_refresh_display(caca_display_t *);
__extern int caca_set_display_time(caca_display_t *, int);
__extern int caca_get_display_time(caca_display_t const *);
__extern int caca_get_display_width(caca_display_t const *);
__extern int caca_get_display_height(caca_display_t const *);
__extern int caca_set_display_title(caca_display_t *, char const *);
__extern int caca_set_mouse(caca_display_t *, int);
__extern int caca_set_cursor(caca_display_t *, int);
/*  @} */

/** \defgroup caca_event libcaca event handling
 *
 *  These functions handle user events such as keyboard input and mouse
 *  clicks.
 *
 *  @{ */
__extern int caca_get_event(caca_display_t *, int, caca_event_t *, int);
__extern int caca_get_mouse_x(caca_display_t const *);
__extern int caca_get_mouse_y(caca_display_t const *);
__extern enum caca_event_type caca_get_event_type(caca_event_t const *);
__extern int caca_get_event_key_ch(caca_event_t const *);
__extern uint32_t caca_get_event_key_utf32(caca_event_t const *);
__extern int caca_get_event_key_utf8(caca_event_t const *, char *);
__extern int caca_get_event_mouse_button(caca_event_t const *);
__extern int caca_get_event_mouse_x(caca_event_t const *);
__extern int caca_get_event_mouse_y(caca_event_t const *);
__extern int caca_get_event_resize_width(caca_event_t const *);
__extern int caca_get_event_resize_height(caca_event_t const *);
/*  @} */

/** \defgroup caca_process libcaca process management
 *
 *  These functions help with various process handling tasks such as
 *  option parsing, DLL injection.
 *
 *  @{ */
__extern int caca_optind;
__extern char *caca_optarg;
__extern int caca_getopt(int, char * const[], char const *,
                         struct caca_option const *, int *);
/*  @} */

/** \brief DOS colours
 *
 *  This enum lists the colour values for the DOS conio.h compatibility
 *  layer.
 */
enum CACA_CONIO_COLORS
{
    CACA_CONIO_BLINK = 128,
    CACA_CONIO_BLACK = 0,
    CACA_CONIO_BLUE = 1,
    CACA_CONIO_GREEN = 2,
    CACA_CONIO_CYAN = 3,
    CACA_CONIO_RED = 4,
    CACA_CONIO_MAGENTA = 5,
    CACA_CONIO_BROWN = 6,
    CACA_CONIO_LIGHTGRAY = 7,
    CACA_CONIO_DARKGRAY = 8,
    CACA_CONIO_LIGHTBLUE = 9,
    CACA_CONIO_LIGHTGREEN = 10,
    CACA_CONIO_LIGHTCYAN = 11,
    CACA_CONIO_LIGHTRED = 12,
    CACA_CONIO_LIGHTMAGENTA = 13,
    CACA_CONIO_YELLOW = 14,
    CACA_CONIO_WHITE = 15,
};

/** \brief DOS cursor modes
 *
 *  This enum lists the cursor mode values for the DOS conio.h compatibility
 *  layer.
 */
enum CACA_CONIO_CURSOR
{
    CACA_CONIO__NOCURSOR = 0,
    CACA_CONIO__SOLIDCURSOR = 1,
    CACA_CONIO__NORMALCURSOR = 2,
};

/** \brief DOS video modes
 *
 *  This enum lists the video mode values for the DOS conio.h compatibility
 *  layer.
 */
enum CACA_CONIO_MODE
{
    CACA_CONIO_LASTMODE = -1,
    CACA_CONIO_BW40 = 0,
    CACA_CONIO_C40 = 1,
    CACA_CONIO_BW80 = 2,
    CACA_CONIO_C80 = 3,
    CACA_CONIO_MONO = 7,
    CACA_CONIO_C4350 = 64,
};

/** \brief DOS text area information
 *
 *  This structure stores text area information for the DOS conio.h
 *  compatibility layer.
 */
struct caca_conio_text_info
{
    unsigned char winleft;        /**< left window coordinate */
    unsigned char wintop;         /**< top window coordinate */
    unsigned char winright;       /**< right window coordinate */
    unsigned char winbottom;      /**< bottom window coordinate */
    unsigned char attribute;      /**< text attribute */
    unsigned char normattr;       /**< normal attribute */
    unsigned char currmode;       /**< current video mode:
                                       BW40, BW80, C40, C80, or C4350 */
    unsigned char screenheight;   /**< text screen's height */
    unsigned char screenwidth;    /**< text screen's width */
    unsigned char curx;           /**< x-coordinate in current window */
    unsigned char cury;           /**< y-coordinate in current window */
};

/** \brief DOS direct video control */
__extern int caca_conio_directvideo;

/** \brief DOS scrolling control */
__extern int caca_conio__wscroll;

/** \defgroup conio libcaca DOS conio.h compatibility layer
 *
 *  These functions implement DOS-like functions for high-level text
 *  operations.
 *
 *  @{ */
__extern char * caca_conio_cgets(char *str);
__extern void   caca_conio_clreol(void);
__extern void   caca_conio_clrscr(void);
__extern int    caca_conio_cprintf(const char *format, ...);
__extern int    caca_conio_cputs(const char *str);
__extern int    caca_conio_cscanf(char *format, ...);
__extern void   caca_conio_delay(unsigned int);
__extern void   caca_conio_delline(void);
__extern int    caca_conio_getch(void);
__extern int    caca_conio_getche(void);
__extern char * caca_conio_getpass(const char *prompt);
__extern int    caca_conio_gettext(int left, int top, int right, int bottom,
                                   void *destin);
__extern void   caca_conio_gettextinfo(struct caca_conio_text_info *r);
__extern void   caca_conio_gotoxy(int x, int y);
__extern void   caca_conio_highvideo(void);
__extern void   caca_conio_insline(void);
__extern int    caca_conio_kbhit(void);
__extern void   caca_conio_lowvideo(void);
__extern int    caca_conio_movetext(int left, int top, int right, int bottom,
                                    int destleft, int desttop);
__extern void   caca_conio_normvideo(void);
__extern void   caca_conio_nosound(void);
__extern int    caca_conio_printf(const char *format, ...);
__extern int    caca_conio_putch(int ch);
__extern int    caca_conio_puttext(int left, int top, int right, int bottom,
                                   void *destin);
__extern void   caca_conio__setcursortype(int cur_t);
__extern void   caca_conio_sleep(unsigned int);
__extern void   caca_conio_sound(unsigned int);
__extern void   caca_conio_textattr(int newattr);
__extern void   caca_conio_textbackground(int newcolor);
__extern void   caca_conio_textcolor(int newcolor);
__extern void   caca_conio_textmode(int newmode);
__extern int    caca_conio_ungetch(int ch);
__extern int    caca_conio_wherex(void);
__extern int    caca_conio_wherey(void);
__extern void   caca_conio_window(int left, int top, int right, int bottom);
/*  @} */

#if !defined(_DOXYGEN_SKIP_ME)
    /* Legacy stuff from beta versions, will probably disappear in 1.0 */

#   if defined __GNUC__ && __GNUC__ >= 3
#       define CACA_DEPRECATED __attribute__ ((__deprecated__))
#   else
#       define CACA_DEPRECATED
#   endif

#   if defined __GNUC__ && __GNUC__ > 3 && !defined __APPLE__
#       define CACA_ALIAS(x) __attribute__ ((weak, alias(#x)))
#   else
#       define CACA_ALIAS(x)
#   endif

#   if defined __GNUC__ && __GNUC__ > 3
#       define CACA_WEAK __attribute__ ((weak))
#   else
#       define CACA_WEAK
#   endif


/* Aliases from old libcaca functions */
#if !defined _WIN32 || !defined __GNUC__
__extern ssize_t caca_import_memory(caca_canvas_t *, void const *, size_t,
                                    char const *) CACA_DEPRECATED;
__extern ssize_t caca_import_file(caca_canvas_t *, char const *,
                                  char const *) CACA_DEPRECATED;
__extern void *caca_export_memory(caca_canvas_t const *, char const *,
                                  size_t *) CACA_DEPRECATED;
#endif

#   if !defined __LIBCACA__
#       define caca_get_cursor_x caca_wherex
#       define caca_get_cursor_y caca_wherey
#   endif
#endif

#ifdef __cplusplus
}
#endif

#undef __extern

#endif /* __CACA_H__ */

#include <string.h>
#include <stdlib.h>
#include <err.h>
#include <curses.h>
#include <sys/param.h>

#include "lottypes.h"
#include "lotdefs.h"
#include "lotfuncs.h"
#include "ttydraw.h"
#include "graphics.h"
#include "draw.h"

caca_canvas_t *cv;

// The font used for drawing labels and legends on graphs.
static struct FONTINFO fontinfo = {
    .name       = { 'd', 'e', 'f', 'a', 'u', 'l', 't' },
    .angle      = 0,
    .em_pix_hgt = 1,
    .em_pix_wid = 1,
};

// The description of our output device.
static struct DEVDATA devdata = {
    .ShowMeFlag     = 1,
    .AspectX        = 2,    // Terminal character cells are generally 2:1
    .AspectY        = 1,
    .RHmusPerTHmu   = 1,
    .RHmuPerGHmus   = 1,
    .RVmusPerTVmu   = 1,
    .RVmuPerGVmus   = 1,
    .FNamesCnt      = 1,
    .FontNames      = &fontinfo,
    .FIndexCnt      = 0,
    .FontIndex      = "",
    .FillPatts      = {
      { { 8,  8  }, { 0, 0 }, "#", { 0, 0 } },  // 0
      { { 8,  8  }, { 0, 0 }, "+", { 0, 0 } },  // 1
      { { 14, 14 }, { 0, 0 }, ":", { 0, 0 } },  // 2
      { { 16, 16 }, { 0, 0 }, "@", { 0, 0 } },  // 3
      { { 16, 16 }, { 0, 0 }, "-", { 0, 0 } },  // 4
      { { 14, 14 }, { 0, 0 }, "'", { 0, 0 } },  // 5
      { {  4, 16 }, { 0, 0 }, "*", { 0, 0 } },  // 6
      { {  4, 16 }, { 0, 0 }, "!", { 0, 0 } },  // 7
      { {  4, 16 }, { 0, 0 }, "^", { 0, 0 } },  // 8
      { {  4, 16 }, { 0, 0 }, "_", { 0, 0 } },  // 9
      { {  4, 16 }, { 0, 0 }, "%", { 0, 0 } },  // 10
      { {  4, 16 }, { 0, 0 }, "$", { 0, 0 } },  // 11
    },
    .ColorMap       = {
         0,  1,  2, 3,
         4,  5,  6, 7,
         8,  9, 10, 11,
        12, 13, 14, 15,
    },
    .SrcPatD        = {
        0xFFFF, 0xAAAA, 0xCCCC, 0xCACA,
        0xC0C0, 0x00FF, 0xFF24, 0x0000
      },
    .SrcPatS        = {
        0x0001, 0x0006, 0x000C, 0x0012,
        0x0018, 0x001E, 0x0024, 0x002A
    },
    .DevFuncs       = {
        exprt_scan_linx,
        exprt_fill_rect,
        exprt_thin_diag_line,
        exprt_thin_vert_line,
        exprt_shade_rect,
        exprt_fill_scan_list,
    },
};

// This is called when a graph is requested (F10).
static int tty_disp_graph()
{
    cv = caca_create_canvas(COLS, LINES);
    return 1;
}

// This is called when 1-2-3 exits graphics mode.
static int tty_disp_text()
{
    caca_free_canvas(cv);
    clear();
    refresh();
    move(0, 0);
    return 2;
}

// This function is used to initialize the rasterizer.
static int tty_disp_grph_compute_view(void *p)
{
    return V3_disp_grph_compute_view(p);
}

static int tty_disp_grph_process(struct GRAPH *graph)
{
    return V3_disp_grph_process(graph);
}

static int tty_disp_grph_set_cur_view(void *view)
{
    return V3_disp_grph_set_cur_view(view);
}

// This is called when Lotus has a string to display, and wants us
// to truncate it so that it fits in the size it has avilable.
static int tty_disp_grph_txt_fit(int len, char *str, int maxlen, int *used)
{
    return *used = MIN(len, maxlen);
}

// This function is called when Lotus wants to know how much space
// a text label will require. We don't use any fonts, and just
// write the label to the screen, so the size is the strlen().
static int tty_disp_grph_txt_size(size_t strarglen, const char *strarg)
{
    return strarglen;
}

// The Lotus implementation of this reports that no graphics are supported
// so we must override it to report graphics support.
static void tty_disp_info(struct DISPLAYINFO *dpyinfo)
{
    dpyinfo->num_text_cols = COLS;
    dpyinfo->num_text_rows = LINES;
    dpyinfo->graphics = true;
    dpyinfo->full_screen_graph = true;
    dpyinfo->hpu_per_col = 1;
    dpyinfo->graph_cols = COLS;
    dpyinfo->graph_rows = LINES;
    dpyinfo->graph_col_res = 1;
    dpyinfo->graph_row_res = 1;
    dpyinfo->view_set_size = 178;
    dpyinfo->iscolor = true;
    dpyinfo->sep_graph_win = false;
}

// The original function.
extern int tty_disp_open(struct LOTUSFUNCS *callbacks,
                         struct BDLHDR *vdbptr,
                         const char *cfgname,
                         char deflmbcsgrp);

// Our wrapper that adds support for graphics.
static int caca_disp_open(struct LOTUSFUNCS *callbacks,
                  struct BDLHDR *vdbptr,
                  const char *cfgname,
                  char deflmbcsgrp)
{
   struct DEVDATA *devinfo = callbacks->alloc_mptr(0x27, sizeof *devinfo, 1);
   void *vmrptr = callbacks->drv_get_vmr(1);

    // Save these callbacks for use in other graphics functions.
    core_funcs = callbacks;

    // Update static devdata now we know the size of the screen.
    devdata.RasterHeight = LINES;

    // Copy over our callbacks for line drawing.
    memcpy(devinfo, &devdata, sizeof devdata);

    RastHandle = callbacks->open_rasterizer(vmrptr);

    return tty_disp_open(core_funcs, vdbptr, cfgname, deflmbcsgrp);
}

static void nullfunc()
{
    return;
}

// This array is what the numbers in the Lotus Color menu represents.
// The first element is Color number 1, and so on.
static int curses_colors[] = {
    COLOR_BLACK,
    COLOR_RED,
    COLOR_GREEN,
    COLOR_YELLOW,
    COLOR_BLUE,
    COLOR_MAGENTA,
    COLOR_CYAN,
    COLOR_WHITE,
};

static uint16_t TextAngle;

static void set_text_angle()
{
    uint16_t *angle = (void *) vmr[0];
    switch ((*angle % 3600) / 450) {
        case 0:
        case 7:
            TextAngle = 0;
            break;
        case 1:
        case 2:
            TextAngle = 1;
            break;
        case 3:
        case 4:
            TextAngle = 2;
            break;
        case 5:
        case 6:
            TextAngle = 4;
            break;
    }
}


static void draw_text_label()
{
    uint16_t    x = MapX(*(uint16_t *)(vmr[0] + 0));
    uint16_t    y = MapY(*(uint16_t *)(vmr[0] + 2));
    char    **str = (void *)(vmr[0] + 6);
    uint16_t *len = (void *)(vmr[0] + 4);

    switch (TextAngle) {
        // Horizontal
        case 0: // I think (x,y) is the midpoint.
                x -= *len / 2;
                for (int c = 0; c < *len; c++) {
                    mvaddch(y, x + c, (*str)[c]);
                }
                break;
        // Vertical
        case 1: // I think (x,y) is the midpoint.
                y -= *len / 2;
                // Write each character in a vertical line.
                for (int c = 0; c < *len; c++) {
                    mvaddch(y + c, x, (*str)[c]);
                }
                break;
        case 2:
        case 4:
                warnx("unsupported text angle, please report this!");
    }
    refresh();
    return;
}

static void caca_disp_pre_system()
{
    if (display_turned_off)
        return;

    clear();
    refresh();
    curs_set(1);
    reset_shell_mode();
}

static void caca_disp_post_system()
{
    if (display_turned_off)
        return;

    reset_prog_mode();
    keypad(stdscr, true);
    curs_set(0);
    refresh();
    erase_screen();
    invalidate_screen();
}

int init_unix_display_code()
{
    // The graph type should always be dumb.
    set_actual_grph_type();

    // Initialize our ncurses, which we use for graphing.
    initscr();

    // Switch to application mode so we can use terminfo.
    keypad(stdscr, true);

    // We can draw graphs in color if available.
    start_color();
    use_default_colors();

    for (int fg = 0; fg < 8; fg++) {
        for (int bg = 0; bg < 8; bg++) {
            init_pair(caca_attr_from_bg_fg(fg, bg),
                      curses_colors[fg],
                      -1);
        }
    }

    x_disp_graph = tty_disp_graph;
    x_disp_grph_compute_view = tty_disp_grph_compute_view;
    x_disp_grph_process = tty_disp_grph_process;
    x_disp_grph_set_cur_view = tty_disp_grph_set_cur_view;
    x_disp_grph_txt_fit = tty_disp_grph_txt_fit;
    x_disp_grph_txt_size = tty_disp_grph_txt_size;
    x_disp_info = tty_disp_info;
    x_disp_open = caca_disp_open;
    x_disp_close = tty_disp_close;
    x_disp_close_retain_termcap = tty_disp_close_retain_termcap;
    x_disp_text = tty_disp_text;
    x_disp_txt_clear = gen_disp_txt_clear;
    x_disp_txt_copy = gen_disp_txt_copy;
    x_disp_txt_curs_type = gen_disp_txt_curs_type;

    if (display_turned_off) {
        x_disp_txt_curs_off = nullfunc;
        x_disp_txt_curs_on = nullfunc;
        x_disp_txt_unlock = nullfunc;
        x_disp_pre_system = caca_disp_pre_system;
        x_disp_post_system = caca_disp_post_system;
    } else {
        x_disp_txt_curs_off = gen_disp_txt_curs_off;
        x_disp_txt_curs_on = gen_disp_txt_curs_on;
        x_disp_txt_unlock = gen_disp_txt_unlock;
        x_disp_pre_system = caca_disp_pre_system;
        x_disp_post_system = caca_disp_post_system;
    }

    x_disp_txt_fg_clear = gen_disp_txt_fg_clear;
    x_disp_txt_fit = gen_disp_txt_fit;
    x_disp_txt_lock = gen_disp_txt_lock;
    x_disp_txt_set_bg = gen_disp_txt_set_bg;
    x_disp_txt_set_pos = gen_disp_txt_set_pos;
    x_disp_txt_set_pos_hpu = gen_disp_txt_set_pos_hpu;
    x_disp_txt_size = gen_disp_txt_size;
    x_disp_txt_sync = gen_disp_txt_sync;
    x_disp_txt_write = gen_disp_txt_write;
    x_disp_txt_zone = gen_disp_txt_zone;
    x_disp_grph_load_font = nullfunc;
    Flush = stdio_flush;
    Find_changes = tty_find_changes;
    dliopen = nullfunc;
    dliclose = nullfunc;

    // Intercept these rasterizer opcodes to learn about text labels.
    opcodes[ROP_DRAWTEXT] = draw_text_label;
    opcodes[ROP_SETTEXTANGLE] = set_text_angle;

    return disp_txt_init(char_set_bundle);
}

// This is a re-implementation of setup_screen_mem that handles more than
// UINT8_MAX rows, see issue #79.
int setup_screen_mem(bool alloclines)
{
    struct LINE *plinedata;
    struct LINE *dlinedata;
    int result;
    size_t i;
    int8_t j;
    int8_t k;

    result = get_screen_size();

    if (result == 0) {
        if (alloclines) {
            memset(&pscreen, 0, sizeof(pscreen));
            memset(&dscreen, 0, sizeof(pscreen));

            pscreen.linedata = calloc(sizeof(struct LINE), LINES);
            dscreen.linedata = calloc(sizeof(struct LINE), LINES);

            lfvec = calloc(8, LINES);

            if (!lfvec || !pscreen.linedata || !dscreen.linedata)
                return 3;

            plinedata = pscreen.linedata;
            dlinedata = dscreen.linedata;

            for (i = 0; i < LINES; i++) {
                dlinedata[i].linebuf  = lts_malloc(COLS);
                dlinedata[i].lineattr = lts_malloc(COLS);
                plinedata[i].linebuf  = lts_malloc(COLS);
                plinedata[i].lineattr = lts_malloc(COLS);

                if (!dlinedata[i].linebuf
                 || !dlinedata[i].lineattr
                 || !plinedata[i].linebuf
                 || !plinedata[i].lineattr)
                    return 3;
            }
        }

        clear_screen_buffer(&dscreen);
        clear_screen_buffer(&pscreen);

        bg_equiv_map[0] = 1;
        for ( j = 0; j <= 2; ++j )
            bg_equiv_map[8 * (1 << j) + (1 << j)] = 1;

        fg_equiv_map[0] = 1;

        for ( k = 0; k <= 1; ++k )
            fg_equiv_map[4 * (1 << k) + (1 << k)] = 1;

        tc_setup_line_funcs();

        opline = lts_malloc(COLS + 1);

        if (opline == NULL)
            return 3;
    }
    return result;
}

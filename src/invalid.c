#include <string.h>
#include <stdlib.h>
#include <err.h>
#include <curses.h>
#include <assert.h>

#include "lottypes.h"
#include "lotdefs.h"
#include "lotfuncs.h"
#include "loterrs.h"

static struct CELLCOORD top_left;

// 1-2-3 keeps a list of column indexes (i.e. terminal columns) in the
// DISPLAYWINDOW struct, stored as an array of uint8_t. These are called
// the invalid columns/rows.
//
// This means the screen will get corrupted with more than 256 columns, because
// the indexes can't be stored.
//
// This is a reimplementation of all the routines that touch that array
// to interpret it as uint16_t, allowing up to 65k columns.
//
// Not all of these flags are fully understood.

int16_t move_block(struct DISPLAYWINDOW *dp, uint16_t cnt, int32_t mode)
{
    uint16_t windim;
    uint16_t k;
    uint16_t xpos;
    uint16_t tnum;
    uint16_t *v8;
    uint16_t dim;
    int16_t dnum;
    uint16_t ypos;
    uint16_t j;
    uint16_t v13;
    uint16_t v14;
    uint16_t i;

    if (mode == 2 || mode == 3) {
        ypos = dp->ypos;
        xpos = dp->xpos;
        dim = dp->real_height;
        v8 = LpiVmr(uint16_t *, VMR_2);
        tnum = num_text_cols;
        dnum = dp->regionb.width;
        windim = dp->win.width;
    } else {
        ypos = dp->ypos;
        dim = dp->columns;
        xpos = dp->xpos;
        v8 = LpiVmr(uint16_t *, VMR_1);
        tnum = num_text_rows;
        dnum = dp->regionb.width;
        windim = dp->win.height;
    }

    if (mode != 2 && mode) {
        v13 = cnt;
        for (i = cnt - 1; v8[i] < i && v8[i + 1] == v8[i] + 1 && v8[i] != UINT16_MAX; --i)
            ;
        v14 = i + 1;
    } else {
        v14 = cnt;
        for (j = cnt + 1; v8[j] > j && v8[j - 1] + 1 == v8[j] && tnum > j && v8[j] != UINT16_MAX; ++j)
            ;
        v13 = j - 1;
    }

    if ((dp->field_2C & 8) != 0 && dnum != windim) {
        erase_window_cellhighlight(dp);
        flags[1] &= ~0x20u;
    }

    if (mode == 2 || mode == 3) {
        x_disp_txt_set_pos(v8[v14] + ypos, xpos);
        x_disp_txt_copy(hpu_per_col * (v13 - v14 + 1), dim, hpu_per_col * (v14 + ypos), xpos);
    } else {
        x_disp_txt_set_pos(ypos, v8[v14] + xpos);
        x_disp_txt_copy(hpu_per_col * dim, v13 - v14 + 1, hpu_per_col * ypos, v14 + xpos);
    }

    for (k = v14; k <= v13; ++k)
        v8[k] = k;

    if (mode != 2 && mode)
        return v14;

    return v13;
}

void init_invalidation()
{
    uint16_t *cols = LpiVmr(uint16_t *, VMR_2);
    uint16_t *rows = LpiVmr(uint16_t *, VMR_1);

    for (int i = 0; i < num_text_rows; i++)
        rows[i] = i;

    for (int i = 0; i < num_text_cols; i++)
        cols[i] = i;
}

uint16_t reset_screen_dimensions()
{
    uint16_t result;
    struct DISPLAYINFO dpyinfo;

    tty_disp_info(&dpyinfo);

    num_text_rows = dpyinfo.num_text_rows;
    num_text_cols = dpyinfo.num_text_cols;
    hpu_per_col = dpyinfo.hpu_per_col;
    screen_width_hpus = dpyinfo.hpu_per_col * dpyinfo.num_text_cols;
    row_label_hpus = 4 * dpyinfo.hpu_per_col;
    separate_graph_window = dpyinfo.sep_graph_win;
    display_metrics.hpu_per_col = dpyinfo.hpu_per_col;
    display_metrics.disp_txt_size = x_disp_txt_size;
    display_metrics.disp_txt_fit = x_disp_txt_fit;

    result = init_showme(2 * (num_text_rows - 5));

    if (result != 0)
        return result;

    result = set_dspcache(num_text_rows - 5);

    if (result != 0)
        return result;

    // There are 26 window structures.
    for (int i = 0; i < NUM_WIN; i++) {

        // I don't know what these flags are.
        win[i].field_2C &= ~0x20u;
        win[i].field_2D &= ~2u;

        win[i].row_invalid = alloc_mptr(12, num_text_rows * sizeof(uint16_t), VMR_1);
        win[i].col_invalid = alloc_mptr(12, num_text_cols * sizeof(uint16_t), VMR_2);

        if (!win[i].row_invalid || !win[i].col_invalid)
            return LOTERR_MEMORY_FULL;

        // Initialize the invalid col/row arrays.
        init_invalidation();
    }

    return 0;
}

void move_rectangles(struct DISPLAYWINDOW *dp)
{
    int16_t i;
    int16_t j;
    int16_t k;
    uint16_t *rows;
    uint16_t *cols;

    // I have no idea what this flag is.
    if ((dp->field_2C & 0x20) == 0)
        return;

    map_win_screen_info(dp);

    // This is just indirection around dp->invcols/dp->invrows.
    rows = LpiVmr(uint16_t *, VMR_1);
    cols = LpiVmr(uint16_t *, VMR_2);

    for (i = 0; dp->real_height - 1 > i && (i <= rows[i] || rows[i] == UINT16_MAX); ++i)
        ;

    for (j = 0; j < dp->columns - 1 && (j <= cols[j] || cols[j] == UINT16_MAX); ++j)
        ;

    for (k = 0; i > k; ++k) {
        if (k != rows[k] && rows[k] != UINT16_MAX)
            k = move_block(dp, k, 0);
    }

    for (k = dp->real_height - 1; i <= k; --k) {
        if (k != rows[k] && rows[k] != UINT16_MAX)
            k = move_block(dp, k, 1);
    }

    for (k = 0; k < j; ++k) {
        if (k != cols[k] && cols[k] != UINT16_MAX)
            k = move_block(dp, k, 2);
    }

    for (k = dp->columns - 1; k >= j; --k) {
        if (k != cols[k] && cols[k] != UINT16_MAX)
            k = move_block(dp, k, 3);
    }
}

void move_rows_or_cols(struct DISPLAYWINDOW *dp,
                       uint16_t src,
                       uint16_t count,
                       int16_t dst,
                       uint16_t *rowbuf)
{
    dp->field_2C |= 0x20u;
    memmove(&rowbuf[src + dst], &rowbuf[src], count * sizeof(*rowbuf));
}

void invalidate_rows(struct DISPLAYWINDOW *dp, uint16_t off, uint16_t size)
{
    uint16_t *x;
    uint16_t *y;
    map_row_invalid(dp);
    x = LpiVmr(uint16_t *, VMR_1);
    y = LpiVmr(uint16_t *, VMR_3);
    memset(&x[off + *(y + 3)], 0xff, size * sizeof(uint16_t));
    dp->field_2D |= 2u;
}

void square_copy(int srcy, int srcx, int dsty, int dstx, uint32_t height, uint32_t width)
{
    struct LINE *srcline;
    struct LINE *dstline;
    struct LINE *endline;
    unsigned int maxcol;

    if (srcy <= dsty) {
        endline = &dscreen.linedata[srcy];
        srcline = &dscreen.linedata[height - 1 + srcy];
        dstline = &dscreen.linedata[height - 1 + dsty];
        if (srcline >= endline) {
            maxcol = width + dstx;
            do {
                memmove(&dstline->linebuf[dstx], &srcline->linebuf[srcx], width);
                memmove(&dstline->lineattr[dstx], &srcline->lineattr[srcx], width);
                if (maxcol > dstline->maxy)
                    dstline->maxy = maxcol;
                --srcline;
                --dstline;
            } while (srcline >= endline);
        }
    } else {
        endline = &dscreen.linedata[height + srcy];
        srcline = &dscreen.linedata[srcy];
        dstline = &dscreen.linedata[dsty];
        if (srcline < endline) {
            maxcol = width + dstx;
            do {
                memmove(&dstline->linebuf[dstx], &srcline->linebuf[srcx], width);
                memmove(&dstline->lineattr[dstx], &srcline->lineattr[srcx], width);
                if (maxcol > dstline->maxy)
                    dstline->maxy = maxcol;
                ++srcline;
                ++dstline;
            }
            while (srcline < endline);
        }
    }
    if (height + dsty > dscreen.nlines)
        dscreen.nlines = height + dsty;
    set_dirty(dsty, dstx, height, width, 1);
}

void display_region(uint16_t width, int16_t n, uint16_t *scrinfo)
{
    int16_t height;
    uint16_t valid;
    uint16_t labelsz;
    int labelhpus;
    uint16_t size;
    char label[8];

    height = region->height;
    size = 0;
    if ( (MAX_ROWS - top_left.row) < height )
    {
        height = MAX_ROWS - top_left.row;
        size = region->height - (MAX_ROWS - top_left.row);
    }
    reset_dspcache(top_left.col, top_left.row, height + top_left.row - 1);
    region_top = region->topleftrow;
    while (--height != -1) {
        vmr[0] = (void *) scrinfo;

        valid = 0;
        if (n && *scrinfo != UINT16_MAX)
            valid = 1;
        row_valid = valid;
        ++scrinfo;
        if ((displayed_window->field_2C & 4) == 0 && (flags[0] & 8) != 0) {
            row_set_pos();
            labelhpus = row_label_hpus;
            labelsz = get_row_label(top_left.row, label);
            x_disp_txt_zone(labelsz, label, 0, 0, labelhpus);
        }
        display_scan_row(top_left, width);
        ++region_top;
        ++top_left.row;
    }
    if (size) {
        if ((displayed_window->field_2C & 0x10) == 0) {
            row_set_pos();
            x_disp_txt_fg_clear(hpu_per_col * (displayed_window->columns + 4), size);
        }
    }
}

void setup_invalid_columns(uint16_t n, int16_t count, uint16_t *scrinfo)
{
    int8_t *inv;
    int8_t *prev;
    int8_t state;
    uint16_t v8;
    uint16_t v9;
    uint8_t *v11;

    vmr[0] = (void *) scrinfo;
    v11 = &vmr[3][n + 12];
    inv = cell_col_invalid;
    if ((flags[3] & 1) != 0) {
        while (--count != -1) {
            prev = inv++;
            if (*scrinfo == UINT16_MAX)
                state = -1;
            else
                state = 0;
            *prev = state;
            scrinfo += 2;
        }
    } else {
        while (--count != -1) {
            prev = inv++;
            if ( *v11 )
                v8 = *v11;
            else
                v8 = 0;
            v9 = v8;
            if (v8 && *scrinfo == UINT16_MAX)
                state = -1;
            else
                state = 0;
            *prev = state;
            ++v11;
            scrinfo += v9;
        }
    }
}

void display_cells()
{
    uint16_t hid;
    uint16_t col_title_width;
    struct DISPLAYWINDOW *wnd;
    uint8_t n;
    uint16_t *scrinfo;
    uint16_t x;
    uint16_t wid;
    uint16_t layoutflag;

    x = vmr[3][11];
    layoutflag = *((uint16_t *)vmr[3] + 3);
    scrinfo = &displayed_window->row_invalid[layoutflag];

    if (vmr[3][11] && displayed_window->regiona.width) {
        hid = check_hidden_columns(vmr[3][10], 1);
        wid = x - (hid - vmr[3][10]);
        setup_column_widths(hid, wid);
        setup_invalid_columns(hid, wid, displayed_window->col_invalid);
        if (layoutflag) {
            top_left = xyz2coord(hid, *((uint16_t *)vmr[3] + 2), origz);
            region = &displayed_window->regiond;
            display_region(wid, displayed_window->field_2D & 1, displayed_window->row_invalid);
        }
        top_left = xyz2coord(hid, origy, origz);
        region = &displayed_window->regiona;
        display_region(wid, (displayed_window->field_2C & 0x10) != 0, scrinfo);
    }

    setup_column_widths(origx, displayed_window->field_1A);
    col_title_width = get_col_title_width();
    setup_invalid_columns(origx, displayed_window->field_1A, &displayed_window->col_invalid[col_title_width]);

    if (layoutflag) {
        top_left = xyz2coord(origx, *((uint16_t *)vmr[3] + 2), origz);
        region = &displayed_window->regionc;
        display_region(displayed_window->field_1A, displayed_window->field_2D & 1, displayed_window->row_invalid);
    }

    top_left = xyz2coord(origx, origy, origz);
    region = &displayed_window->regionb;
    display_region(displayed_window->field_1A, (displayed_window->field_2C & 0x10) != 0, scrinfo);
    wnd = displayed_window;
    displayed_window->field_2C |= 4u;
    n = 16 * ((wnd->field_2C & 4) != 0);
    wnd->field_2C &= ~0x10u;
    wnd->field_2C |= n;
    wnd->field_2D |= 1u;
}

void invalidate_cell_cols(struct DISPLAYWINDOW *dp, int16_t count, int16_t m)
{
    uint16_t startcol;
    uint16_t remaining;
    int16_t colwidth;
    uint16_t totalwidth;

    startcol = vmr[3][9];
    colwidth = win_column_width(startcol, count);

    totalwidth = colwidth + get_col_title_width();
    remaining = win_column_width(count + startcol, m);

    if (num_text_cols <= remaining + totalwidth)
        remaining = num_text_cols - totalwidth;

    map_col_invalid(dp);

    memset(&dp->col_invalid[totalwidth], 0xff, remaining * sizeof(uint16_t));

    dp->field_2D |= 2u;
}

int16_t find_invalid_column(uint16_t start, uint16_t end)
{
    int8_t *found;

    found = memchr(&cell_col_invalid[start - top_left.col], 0xff, end - start + 1);

    if (found) {
        return top_left.col + (uint32_t)(found - cell_col_invalid);
    }

    return -1;
}

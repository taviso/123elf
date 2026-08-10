#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "lottypes.h"
#include "lotdefs.h"
#include "lotfuncs.h"
#include "loterrs.h"

#define COLIDX(n) ((n) >> 4)
#define MAX_SHOWME 2048
#pragma pack(1)

// This is 1bit for each column of a row, A-IV.
typedef struct SHOWMEROW {
    int16_t colflags[16];
} SHOWMEROW;

static struct SHOWME {
    int16_t count;
    SHOWMEROW rowdata;
    int16_t rowindex[MAX_SHOWME];
    int16_t sheetindex[MAX_SHOWME];
    SHOWMEROW rows[MAX_SHOWME];
} showme_data;

struct SHOWMECELL {
    uint16_t col;
    uint16_t sheet;
    uint16_t row;
};

// Utility functions for setting/clearing/getting a column flag.
static inline uint16_t get_column_bit(SHOWMEROW *row, uint16_t column)
{
    return !! (row->colflags[column >> 4] & (1 << (column & 0xf)));
}

static inline void clr_column_bit(SHOWMEROW *row, uint16_t column)
{
    row->colflags[column >> 4] &= ~(1 << (column & 0xf));
}

static inline void set_column_bit(SHOWMEROW *row, uint16_t column)
{
    row->colflags[column >> 4] |= (1 << (column & 0xf));
}

SHOWMEROW * find_row(int16_t sheet, int16_t row)
{
    for (int idx = 0; idx < showme_data.count; idx++) {
        if (row != showme_data.rowindex[idx])
            continue;
        if (sheet != showme_data.sheetindex[idx])
            continue;
        // Match
        return &showme_data.rows[idx];
    }

    // Not found.
    return &showme_data.rowdata;
}

int16_t init_showme(int16_t termrows)
{
    if (termrows > MAX_SHOWME)
        return LOTERR_MEMORY_FULL;
    return 0;
}

void reset_showme()
{
    showme_data.count = 0;
}

void clear_showme(int16_t sheet, int16_t row, int16_t numrows)
{
    do {
        if (find_row(sheet, row) == &showme_data.rowdata) {
            if (showme_data.count >= MAX_SHOWME)
                break;
            showme_data.rowindex[showme_data.count] = row;
            showme_data.sheetindex[showme_data.count] = sheet;
            memset(&showme_data.rows[showme_data.count], 0, sizeof(SHOWMEROW));
            ++showme_data.count;
        }
        ++row;
        --numrows;
    } while (numrows);
}

int scan_showme_rows(struct SHOWMECELL *cell)
{
    do {
        if (showme_data.count <= cell->col)
            return 0;
        cell->sheet = showme_data.sheetindex[cell->col];
        cell->row = showme_data.rowindex[cell->col++];
    } while (cell->row > MAX_ROW);
  return 1;
}

void adjust_showme_cols(int16_t startsheet,
                        int16_t endsheet,
                        int16_t begincol,
                        int16_t endcol)
{
    int idx;
    int16_t sheet;
    int16_t startc;
    int16_t maxcol;
    int16_t c;
    int16_t currcol;
    int16_t startcol;
    int16_t j;
    SHOWMEROW *row;
    int16_t i;

    // This is used for insert/delete column
    for (idx = 0, i = showme_data.count; i; --i, idx++) {
        sheet = showme_data.sheetindex[idx];

        // Check if in sheet range.
        if (sheet <= endsheet && sheet >= startsheet) {
            // Get a row pointer to easily clear/set bits.
            row = &showme_data.rows[idx];

            if (endcol >= 0) {
                maxcol = MAX_COL;
                startcol = MAX_COLS - endcol - begincol;
                if (MAX_COLS - endcol != begincol) {
                    do {
                        clr_column_bit(row, maxcol);
                        set_column_bit(row, maxcol - endcol);
                        --maxcol;
                    } while (--startcol);
                }
                for ( j = endcol; j; --maxcol) {
                    set_column_bit(row, maxcol);
                    --j;
                }
            } else {
                startc = begincol;
                c = endcol + MAX_COLS - begincol;
                if ( endcol + MAX_COLS != begincol ) {
                    do {
                        clr_column_bit(row, startc);
                        set_column_bit(row, startc - endcol);
                        ++startc;
                    } while (--c);
                }
                currcol = endcol;
                do {
                    set_column_bit(row, startc);
                    ++startc;
                } while (++currcol);
            }
        }
    }
}

void invalidate_region(struct CELLCOORD rngstart, struct CELLCOORD rngend)
{
    uint16_t startcol;
    uint16_t endcol;
    struct CELLCOORD coord;
    SHOWMEROW *row;
    uint16_t numcols;
    uint16_t endcolmask;
    uint16_t startcolmask;

    // This routine will set the showme bits for all of the columns between
    // start and end for all the rows between them. colflags is an array of 256
    // bits, and say we want to  invalidate all the columns between a and b:
    //
    //    row         a---------------------------------b
    //     0  0000000000000000 0000000000000000 0000000000000000
    //     1  0000000000000000 0000000000000000 0000000000000000
    //     2  0000000000000000 0000000000000000 0000000000000000
    //
    //  The middle word is easy, just to 0xffff. The start and end
    //  word need a mask.

    startcol = COLIDX(rngstart.col);
    endcol   = COLIDX(rngend.col);

    // So these are the masks for the word columns a and b are in.
    //             a-------- [...] -------b
    //      0000000000000000       0000000000000000
    // mask 1111111000000000       0000000011111111
    startcolmask = 0xFFFF << (rngstart.col & 0xF);
    endcolmask   = 0xFFFE << (rngend.col & 0xF);
    endcolmask   = ~endcolmask;

    // If the column starts and ends in the same word, then we need
    // to or these together.
    //            a------b
    //        0000000000000000
    // maska  1111000000000000
    // maskb  0000000000001111
    // result 1111000000001111
    if (endcol == startcol) {
        // Why is this and and not or? is this a bug?
        endcolmask  &= startcolmask;
        startcolmask = endcolmask;
    }

    // Calculate how many whole columns are between start and end.
    // These can be just set to 0xffff.
    if (startcol >= endcol) {
        // Just use the masks.
        numcols = 0;
    } else {
        numcols = endcol - startcol - 1;
    }

    for (int16_t i = 0; i < showme_data.count; i++) {
        coord = xyz2coord(rngstart.col,
                          showme_data.rowindex[i],
                          showme_data.sheetindex[i]);

        // Now check if we need to set this row.
        if (coord_in_range(coord, rngstart, rngend)) {
            row = &showme_data.rows[i];
            row->colflags[COLIDX(rngstart.col)] |= startcolmask;
            row->colflags[COLIDX(rngend.col)] |= endcolmask;
            // Set all the columns between.
            if (numcols) {
                memset(&row->colflags[COLIDX(rngstart.col) + 1], 0xFF, 2 * numcols);
            }
        }
    }
}

// This is used when inserting/removing rows.
void adjust_showme_rows(int16_t startsheet,
                        int16_t endsheet,
                        int16_t startrow,
                        int16_t endrow)
{
    for (int16_t i = 0; i < showme_data.count; i++) {
        int16_t sheet = showme_data.sheetindex[i];

        if (startrow <= showme_data.rowindex[i]
         && endsheet >= sheet
         && sheet >= startsheet) {
            int16_t nextrow = showme_data.rowindex[i] + endrow;

            // Adhjust row
            showme_data.rowindex[i] = nextrow;

            // This is true when we're deleting a row, so make the index invalid.
            if (startrow > nextrow) {
                showme_data.rowindex[i] = -1;
            }
        }
    }
}

void set_showme(struct CELLCOORD cell)
{
    SHOWMEROW *row = find_row(cell.sheet, cell.row);
    set_column_bit(row, cell.col);
}

SHOWMEROW *get_showme_row(uint16_t sheet, uint16_t row)
{
    return find_row(sheet, row);
}

int16_t get_showme(struct CELLCOORD cell)
{
    SHOWMEROW *row = find_row(cell.sheet, cell.row);
    return get_column_bit(row, cell.col);
}

// This searches for the next set column after the specified column.
int scan_showme(SHOWMEROW *row, int16_t column)
{
    for (int i = column; i < MAX_COLS; i++) {
        if (get_column_bit(row, i)) {
            return i;
        }
    }
    return -1;
}

// This searches for the column before the specified column.
//
// I think the 123 implementation is buggy, it won't cross a mod16 boundary. I
// see no reason that would be deliberate. Perhaps nobody noticed because
// nobody could see 16 columns simultaneously? This implementation removes that
// limitation.
int scan_showme_leftwards(SHOWMEROW *row, int16_t column)
{
    for (int i = column; i >= 0; i--) {
        if (get_column_bit(row, i)) {
            return i;
        }
    }
    return -1;
}

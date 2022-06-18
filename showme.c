#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>
#include <assert.h>

#include "lottypes.h"
#include "lotdefs.h"
#include "lotfuncs.h"

// This is 1bit for each column of a row, A-IV.
static uint16_t colflags[16];

static inline uint16_t get_column_bit(uint16_t *row, uint16_t column)
{
    return !! (row[column >> 4] & (1 << (column & 0xf)));
}

static inline void clr_column_bit(uint16_t *row, uint16_t column)
{
    row[column >> 4] &= ~(1 << (column & 0xf));
}

static inline void set_column_bit(uint16_t *row, uint16_t column)
{
    row[column >> 4] |= (1 << (column & 0xf));
}

uint16_t * find_row(int16_t sheet, int16_t row)
{
    return colflags;
}

int16_t init_showme(int16_t termrows)
{
    return 0;
}

void reset_showme()
{
    return;
}

void clear_showme(int16_t sheet, int16_t row, int16_t numrows)
{
    return;
}

int scan_showme_rows(int16_t *a1)
{
  return 0;
}

void adjust_showme_cols(int16_t a1, int16_t a2, int16_t a3, int16_t a4)
{
    return;
}

void adjust_showme_rows(int16_t startsheet, int16_t endsheet, int16_t col, int16_t row)
{
    return;
}

void invalidate_region(struct CELLCOORD rngstart, struct CELLCOORD rngend)
{
    return;
}

void set_showme(struct CELLCOORD cell)
{
    uint16_t *row = find_row(cell.sheet, cell.row);
    set_column_bit(row, cell.col);
}

uint16_t * get_showme_row(uint16_t sheet, uint16_t row)
{
    return find_row(sheet, row);
}

int16_t get_showme(struct CELLCOORD cell)
{
    uint16_t *row = find_row(cell.sheet, cell.row);
    return get_column_bit(row, cell.col);
}

#include <stdio.h>
#include <stdint.h>
#include <alloca.h>
#include <curses.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>

#include <sys/stat.h>

#include "lottypes.h"
#include "lotdefs.h"
#include "lotfuncs.h"
#include "loterrs.h"

// This is a reimplementation of the /File Import code
// with the line length limits removed.
uint16_t fm_file_import(char *filename, int16_t textmode)
{
    uint16_t result;
    int16_t rdcol;
    int16_t mbcs;
    struct CELLCOORD dstcell;
    struct CELLCOORD rdcell;
    int16_t len;
    struct CELLCOORD nextcell;
    int16_t sbcs;
    char *base;
    int retcode;
    int readret;
    uint16_t *table;
    char *outptr;
    char *label;
    uint16_t readcnt;
    struct SYSHANDLE *fd;
    struct CELLCOORD cp;
    static char outbuf[4096];
    static char inbuf[4096];
    struct PATHNAME *pathname;

    pathname = alloca(sizeof *pathname + PATH_MAX);

    memset(pathname, 0, sizeof *pathname + PATH_MAX);

    fd = INVALID_HANDLE_VALUE;
    cp = get_cellpointer();

    if (file_mode != FILE_MODE_UNIX) {
        base = access_resource(0xC42u);
        result = file_name_compose(filename, pathname, base, 1);
    } else {
        result = file_name_compose(filename, pathname, 0, 1);
    }

    if (result != 0)
        return result;

    if (print_file_in_use(pathname))
        return LOTERR_FILE_IN_USE;

    if ((retcode = file_access_read(pathname, &fd, 0)))
        return get_resource_handle(retcode);

    vmr[3] = (void *) filename;

    table = get_fname_content_xlt_tbl(filename, 1);

    // This doesn't seem possible, but the original checks.
    if (cp.row > MAX_ROW)
        goto finished;

    while (true) {
        readret = file_read_line(&fd->fd, sizeof inbuf, inbuf, &readcnt);

        if (readret && readcnt == 0)
            break;

        if (readcnt) {
            if (convert_from_table(table, inbuf, outbuf, sizeof outbuf) == 0) {
                result = 9105;
                goto finished;
            }

            // This is the /File Import Text option
            if (textmode) {
                // Check we can modify the cell first.
                if ((result = cell_immutable(cp)))
                    goto finished;

                // Everything read becomes a label.
                if ((result = make_label_cell(cp, outbuf, '\'')))
                    goto finished;
            } else {
                // Reset column after every line.
                rdcol = cp.col;
                outptr = outbuf;
                while (true) {

                    // Check for end of string.
                    if ((sbcs = peek_next_sbcs(outptr)) == '\0')
                        break;

                    // Check if this is quoted.
                    if (sbcs == '"') {
                        // This is a label, skip past opening quote.
                        skip_next_mbcs(&outptr);

                        // This is the start of a label.
                        label = outptr;

                        // Find the closing quote.
                        if (find_first_sbcs(&outptr, '"')) {
                            mbcs = skip_next_mbcs(&outptr);
                            // Set the quote to end of string.
                            // The original does this, but why not just *outptr = '\0'?
                            outptr[-mbcs] = 0;
                        } else {
                            // No closing quote, just use the end.
                            get_string_end(&outptr);
                        }

                        if ((result = import_outof_bounds(rdcol, cp.row)))
                            goto finished;

                        dstcell = xyz2coord(rdcol, cp.row, cp.sheet);

                        if ((result = cell_immutable(dstcell)))
                            goto finished;

                        rdcell = xyz2coord(rdcol++, cp.row, cp.sheet);
                        // Make cell a label.
                        if ((result = make_label_cell(rdcell, label, '\'')))
                            goto finished;
                    } else {
                        // This is a number.
                        len = strlen(outptr);
                        readcnt = parse_number(outptr, len);
                        if ( readcnt ) {
                            result = import_outof_bounds(rdcol, cp.row);
                            if ( result || ((result = cell_immutable(xyz2coord(rdcol, cp.row, cp.sheet))) != 0)) {
                                drop_one();
                                goto finished;
                            }
                            nextcell = xyz2coord(rdcol++, cp.row, cp.sheet);

                            if ((result = make_number_cell(nextcell)))
                                goto finished;

                            outptr += readcnt;
                        } else {
                            skip_next_mbcs(&outptr);
                        }
                    }
                }
            }
        }
        if (readret == 1) {
            result = 0;
            goto finished;
        }

        // This line is complete, try the next row.
        if (++cp.row > MAX_ROW)
            goto finished;
    }

    // If we reach here, everything worked.
    result = 0;

    if (readret != 1)
        result = 8980;

finished:
    sheet_modified(cp.sheet);
    return file_finished_shell(&fd, result);
}

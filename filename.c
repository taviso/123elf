#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <curses.h>
#include <string.h>

#include "loterrs.h"
#include "lottypes.h"
#include "lotdefs.h"
#include "lotfuncs.h"

// This breaks the specified pathname into the PATHNAME structure, which
// contains offsets of drive, directory, filename, extension, and so on.
//
// This routine handles LMBCS encoded filesnames in DOS mode only.
// In DOS mode, '\\' or '/' are valid directory seperators, and will be
// converted as requested.
//
// Originally, this routine required filenames on UNIX to be under 14
// characters, but this rewrite removes that limitation, see #82.
//
// - base is an optional base directory to assume is prepended to pathname.
// - extension is a file extension to append.
// - resource..
// - If set, expandpath will expand globs.
// - If set, checklen will validate all components are within limits.
uint16_t file_name_split(struct PATHNAME *pstruct,
                         char *pathname,
                         char *root,
                         char *extension,
                         int16_t *resource,
                         int16_t expandpath,
                         int16_t checklen)
{
    char *p;
    unsigned int mbcs;
    char *prepended_root_after_drv;
    char *dirsep;
    char *extptr;
    int extlen;
    int pathend;
    int16_t *fname_content_xlt_tbl;
    int16_t pathtype;
    uint16_t diroff_1;
    uint16_t diroff;
    uint16_t offsetofext;
    uint16_t result;
    char *prepended_root;
    char *path_after_drive;
    char *lastspace;
    char *lastdirptr;
    char *filenamestart;
    char *fileext;
    char *resourcematch;
    char reducedpath[PATH_MAX];

    result  = 0;

    // The translation table for lmbcs encoded filenames.
    fname_content_xlt_tbl = get_fname_content_xlt_tbl(0, 2);

    // If caller requested, validate lengths.
    if (checklen && (result = check_lengths(pathname))) {
        return result;
    }

    // Reset the cracked filename structure.
    file_name_clear(pstruct);

    // Validate parameters.
    if (!pathname ||!*pathname )
        return 0;

    // Skip past any leading blanks.
    while (*pathname == ' ')
        ++pathname;

    p                = pathname;
    fileext          = NULL;
    lastdirptr       = NULL;
    lastspace        = NULL;
    path_after_drive = NULL;
    resourcematch    = NULL;
    pathtype         = 0; // pathtype 2 = resource?

    // If in DOS mode, check if this looks like a drive.
    if (pathname[1] == ':' && file_mode != FILE_MODE_UNIX) {
        p = pathname + 2;
        path_after_drive = pathname + 2;
    }

    // This translates lmbcs codepoints
    if (!file_name_reduce(reducedpath, p))
        return LOTERR_FILENAME_INVALID;

    // Now check through the whole path for any special charaters.
    while (*p) {
        switch (*p) {
            case ' ':
                resourcematch = p;
                lastspace = p;
                if (!resource_substr_match(0xC28u, &resourcematch))
                    pathtype = 2;
                break;
            case '.':
                // There can't be two extensions in DOS mode.
                if (file_mode != FILE_MODE_UNIX && checklen && fileext)
                    return LOTERR_FILENAME_INVALID;
                // Otherwise, the last . begins the extension.
                fileext = p;
                break;
            case '/':
                // The last directory.
                lastdirptr = p;
                break;
            case '<':
            case '>':
                // Invalid characters for some reason? Maybe this is a UNIX
                // limitation, these are perfectly valid on Linux.
                return LOTERR_FILENAME_INVALID;
            case '\\':
                // In DOS mode, this is an acceptable dirseperator, otherwise
                // it's just a normal char on UNIX.
                if (file_mode != FILE_MODE_UNIX)
                    lastdirptr = p;
                break;
            default:
                // For all other characters, maybe do lmbcs translation.
                if (file_mode != FILE_MODE_UNIX) {
                    mbcs = peek_next_mbcs(p);
                    if ( mbcs > 0xFF || mbcs <= 0x1F || !fname_content_xlt_tbl[mbcs - 32] )
                        return LOTERR_FILENAME_INVALID;
                }
                break;
        }

        // If a resource request
        if (pathtype == 2)
            break;

        // Skip over this char (possibly mb char)
        p += char_size(*p);
    }

    // Now we've found the extension, filename, and dirname
    if (lastspace) {
        resourcematch = lastspace;
        if (!resource_substr_match(0xC2Bu, &resourcematch) && !peek_next_mbcs(resourcematch)) {
            pathtype = 1;
            p = lastspace;
        }
    }

    if (resource)
        *resource = pathtype;

    // Is the file extension part of the filename?
    if (lastdirptr && fileext && lastdirptr > fileext)
        fileext = 0;

    prepended_root_after_drv = 0;

    if (file_mode != FILE_MODE_UNIX) {
        // This is an optional "root" to prepend
        if (root) {
            // Check if it's a drive
            prepended_root = root;
            if (root[1] == ':') {
                prepended_root = root + 2;
                prepended_root_after_drv = root + 2;
            }
        }
        if (path_after_drive) {
            pstruct->diroff = path_after_drive - pathname;
            memcpy(pstruct->str, pathname, (path_after_drive - pathname));
            pathname = path_after_drive;
        } else if (prepended_root_after_drv) {
            pstruct->diroff = prepended_root_after_drv - root;
            memcpy(pstruct->str, root, pstruct->diroff);
        }
    }
    else {
        prepended_root = root;
        pstruct->diroff = 0;
        path_after_drive = 0;
    }
    pstruct->start = 0;
    diroff_1 = pstruct->diroff;
    if (lastdirptr) {
        filenamestart = lastdirptr + 1;
        pstruct->filenameoff = filenamestart - pathname;
        memcpy(&pstruct->str[diroff_1], pathname, (filenamestart - pathname));
        pathname = filenamestart;
    }
    else if (root) {
        if (!path_after_drive) {
            pstruct->filenameoff = strlen(prepended_root);
            memcpy(&pstruct->str[diroff_1], prepended_root, pstruct->filenameoff);
            if (pstruct->filenameoff) {
                dirsep = &pstruct->str[pstruct->filenameoff + diroff_1 - 1];
                if (dirsep[0] != '/' && dirsep[0] != '\\') {
                    dirsep[1] = '/';
                    ++pstruct->filenameoff;
                }
            }
        }
    }
    slash_convert(&pstruct->str[diroff_1], pstruct->filenameoff);
    pstruct->diroffpreroot = diroff_1;
    diroff = pstruct->filenameoff + diroff_1;
    pstruct->dirlen = diroff;
    pstruct->namelen = p - pathname;
    if (p != pathname) {
        if (fileext) {
            pstruct->extlen = p - fileext;
            pstruct->namelen -= p - fileext;
        }
        if ( pstruct->extlen + pstruct->namelen + pstruct->dirlen > PATH_MAX) {
            return LOTERR_FILENAME_TOO_LONG;
        }
        memcpy(&pstruct->str[diroff], pathname, pstruct->namelen);
    }
    offsetofext = pstruct->namelen + diroff;
    if (pstruct->extlen) {
        memcpy(&pstruct->str[offsetofext], fileext, pstruct->extlen);
    } else if (extension) {
        pstruct->extlen = strlen(extension) + 1;

        if (pstruct->extlen + pstruct->namelen + pstruct->dirlen > PATH_MAX) {
            return LOTERR_FILENAME_TOO_LONG;
        }

        pstruct->str[offsetofext] = '.';
        extptr = &pstruct->str[offsetofext + 1];
        strcpy(extptr, extension);
        if (checklen)
            fallback_and_coerce_case(extptr);
    }

    pstruct->offext = offsetofext;

    // Extension cannot exceed 3 chars (+.) in DOS mode.
    if (file_mode != FILE_MODE_UNIX && checklen) {
        if (pstruct->extlen > 4)
            extlen = 4;
        else
            extlen = pstruct->extlen;
        pathend = extlen + pstruct->offext;
    } else {
        pathend = pstruct->extlen + pstruct->offext;
    }
    pstruct->str[pathend] = 0;
    if (expandpath)
        result = file_path_expand(pstruct);
    if (!result && checklen)
        file_name_case_check(pstruct->str);
    if (pstruct->extlen && !pstruct->namelen) {
        if (pstruct->filenameoff || pstruct->diroff || pstruct->extlen != 1)
            return LOTERR_WORKSHEET_UNNAMED;
        pstruct->namelen = 1;
        pstruct->extlen = 0;
        pstruct->offext = 1;
    }
    return result;
}


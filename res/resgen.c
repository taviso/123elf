#include <stdio.h>
#include <err.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "lottypes.h"

// ; Comment
// =456 ; group
// 123 "This is a message"

static uint16_t find_max_resource(char **msgtable)
{
    uint16_t max = 0;

    for (int i = 0; i < UINT8_MAX; i++) {
        max = msgtable[i] ? i : max;
    }

    return max;
}

int main(int argc, char **argv)
{
    char *line = NULL;
    size_t n = 0;
    char **msgs;
    uint16_t grpsz;
    struct RESHDR hdr = {
        .magic      = 0x1ad9c,
        .version    = 1,
        .groups     = 0,
    };

    // Resource IDs are 2 8bit selectors, so cannot exceed 2^8
    char **msgtab[256] = {0};

    while (getline(&line, &n, stdin) != -1) {
        char *msg;
        uint16_t id;

        // Skip blank lines or comments.
        if (*line == '\0' || *line == ';' || *line == '\n')
            continue;

        if (*line == '=') {
            hdr.groups++;

            msgtab[hdr.groups] = calloc(sizeof(char *), 256);

            // The rest of the line is an ignored comment or name.
            fprintf(stderr, "group %d\n", hdr.groups);
            continue;
        }

        // Check that there is a message.
        char *suffix = strrchr(line, '"');
        char *prefix = strchr(line, '"');

        // Try to parse the first token as a number.
        id = strtoul(line, &msg, 0);

        if (!suffix || !prefix || suffix == prefix || msg == line) {
            err(EXIT_FAILURE, "failed to parse line `%s`", line);
        }

        if (id > UINT8_MAX) {
            err(EXIT_FAILURE, "id %u was out of range", id);
        }

        // Clear the suffix and prefix.
        *suffix = 0; msg = prefix + 1;

        fprintf(stderr, "id = %d, found msg: %s\n", id, msg);

        // Now add this to our table.
        msgtab[hdr.groups][id] = strdup(msg);
    }

    fwrite(&hdr, sizeof(hdr), 1, stdout);

    // Now we need to write the total size of each group.
    for (int group = 1; group <= hdr.groups; group++)  {
        uint16_t maxid = find_max_resource(msgtab[group]);

        fprintf(stderr, "There are %d messages in group %d\n", maxid, group);

        // We will need at least a word for each entry.
        grpsz = (maxid + 1) * sizeof(uint16_t);

        // And now the strings themselves.
        for (int res = 1; res <= maxid; res++) {
            // If it does exist, add the size.
            if (msgtab[group][res]) {

                grpsz += strlen(msgtab[group][res]);

                // Add space for the terminating nul. "Empty"
                // selectors just point to the nul at the start, so
                // don't take any more room and still return a nul string.
                grpsz++;
            }
        }

        // Write out the size.
        fwrite(&grpsz, sizeof(grpsz), 1, stdout);
    }

    // Now we need to write the strings themselves.
    for (int group = 1; group <= hdr.groups; group++)  {
        uint16_t maxid = find_max_resource(msgtab[group]);

        fprintf(stderr, "There are %d messages in group %d\n", maxid, group);

        // The first one is always 0.
        grpsz = 0;
        fwrite(&grpsz, sizeof(grpsz), 1, stdout);

        // We will need at least a word for each entry.
        grpsz = (maxid + 1) * sizeof(uint16_t);

        // And now the string sizes.
        for (int res = 1; res <= maxid; res++) {
            // If it does exist, add the size.
            if (msgtab[group][res]) {
                fwrite(&grpsz, sizeof(grpsz), 1, stdout);
                grpsz += strlen(msgtab[group][res]);

                // Add space for the terminating nul. "Empty"
                // selectors just point to the nul at the start, so
                // don't take any more room and still return a nul string.
                grpsz++;
            } else {
                uint16_t z = 0;
                fwrite(&z, sizeof(z), 1, stdout);
            }
        }

        // And now the strings.
        for (int res = 1; res <= maxid; res++) {
            if (msgtab[group][res]) {
                fwrite(msgtab[group][res], strlen(msgtab[group][res]), 1, stdout);

                // And a terminating nul
                fputc('\0', stdout);
            }
        }
    }

    free(line);
    return 0;
}

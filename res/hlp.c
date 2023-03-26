#include <stdio.h>
#include <err.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define id2dat(x) (((x) >> 8) & 0xff)
#define id2sek(x) ((x) & 0xff)

struct RESHDR {
    uint32_t magic;
    uint16_t version;
    uint16_t hdrwords;
    uint16_t data[0];
};

// Note: this is not useful, should be doing read_image()
int find_resource_in_file(FILE *handle,
                          struct RESHDR *hdr,
                          uint16_t resid,
                          void *buf,
                          uint16_t bufsiz)
{
    uint16_t dataid = id2dat(resid);
    uint16_t seekid = id2sek(resid) * sizeof(uint16_t);
    off_t objoffset;
    int result;

    fseek(handle, sizeof(*hdr) + hdr->hdrwords * 2, SEEK_SET);

    for (objoffset = seekid; --dataid != 0xFFFF; objoffset += hdr->data[dataid])
        ;

    if (fseek(handle, objoffset, SEEK_CUR) != 0)
        return -1;

    if (fread(&dataid, sizeof(dataid), 1, handle) != 1)
        return -1;

    if (fseek(handle, dataid - seekid - sizeof(dataid), SEEK_CUR) != 0)
        return -1;

    return fread(buf, 1, bufsiz, handle);
}

int main(int argc, char **argv)
{
    struct RESHDR *hdr;
    FILE *resfile;
    uint16_t **data;
    uint32_t magic;

    if ((resfile = fopen(argv[1], "r")) == NULL) {
        err(EXIT_FAILURE, "Failed to open file");
    }

    hdr = malloc(sizeof *hdr);

    if (fread(hdr, sizeof *hdr, 1, resfile) != 1) {
        err(EXIT_FAILURE, "Failed to read header");
    }

    // This is read_file_hdr()
    if (hdr->magic != 0x1ad9c) {
        err(EXIT_FAILURE, "Magic number %#x not correct", hdr->magic);
    }

    if (hdr->version != 1) {
        err(EXIT_FAILURE, "Version %u not correct", hdr->version);
    }

    hdr = realloc(hdr, sizeof(*hdr) + hdr->hdrwords * 2);
    data = calloc(sizeof(void *), hdr->hdrwords);

    fread(hdr->data, 2, hdr->hdrwords, resfile);

    for (int i = 0; i < hdr->hdrwords; i++) {
        data[i] = calloc(1, hdr->data[i]);

        fprintf(stdout, "=group %u, %u bytes\n", i, hdr->data[i]);

        fread(data[i], 1, hdr->data[i], resfile);

        for (int id = 0;; id++) {
            char *msg = (char *)(data[i]) + data[i][id];
            char *end = (char *)(data[i]) + hdr->data[i];

            if (data[i][id] == 0) {
                fprintf(stdout, "; %u <missing>\n", id);
            } else {
                fprintf(stdout, "%u \"%s\"\n", id, msg);
                if (msg + strlen(msg) + 1 >= end) {
                    fprintf(stdout, "\n");
                    break;
                }
            }
        }
    }

    if (fread(&magic, sizeof(magic), 1, resfile) != 1) {
        fprintf(stderr, "no huffman\n");
    } else {
        fprintf(stderr, "magic is %#x\n", magic);
    }

    fclose(resfile);
    return 0;
}

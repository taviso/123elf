#ifndef __LOTTYPES_H
#define __LOTTYPES_H

#pragma pack(push, 1)

struct DISPLAYINFO
{
  uint16_t num_text_cols;
  uint16_t num_text_rows;
  uint16_t graphics;
  uint16_t full_screen_graph;
  uint16_t hpu_per_col;
  uint16_t graph_cols;
  uint16_t graph_rows;
  uint16_t graph_col_res;
  uint16_t graph_row_res;
  uint16_t view_set_size;
  uint16_t iscolor;
  uint16_t sep_graph_win;
};

struct BDLHDR
{
  uint16_t bdllen;
  uint16_t bdlver;
  uint16_t driver_id;
  uint16_t bundle_id;
};

struct BDLRECHDR
{
  uint16_t bdltype;
  uint16_t reclen;
};

struct DEVPRIM
{
  void (*scan_linx)(int, int, int, int);
  void (*fill_rect)(int, int, int, int, int);
  void (*thin_diag_line)(int, int, int, int, int);
  void (*thin_vert_line)(int, int, int, int);
  void (*shade_rect)(int, int, int, int, void *, int);
  void (*fill_scan_list)();
};

typedef struct DEVPRIM DEVPRIM;

struct POINT
{
  uint16_t pty;
  uint16_t ptx;
};

typedef struct POINT POINT;

struct PATT
{
  POINT pattsize;
  POINT patthotSpot;
  char *pattptr;
  POINT pattSecOffset;
};

typedef struct PATT PATT;

struct DEVDATA
{
  uint16_t ShowMeFlag;
  uint16_t RasterHeight;
  uint16_t AspectX;
  uint16_t AspectY;
  uint16_t RHmusPerTHmu;
  uint16_t RHmuPerGHmus;
  uint16_t RVmusPerTVmu;
  uint16_t RVmuPerGVmus;
  PATT FillPatts[12];
  unsigned char ColorMap[16];
  uint16_t SrcPatD[8];
  uint16_t SrcPatS[8];
  uint32_t FIndexCnt;
  void *FontIndex;
  uint32_t FNamesCnt;
  void *FontNames;
  DEVPRIM DevFuncs;
};

struct FONTINFO
{
  char name[9];
  uint8_t  _padding;
  uint16_t angle;
  uint16_t em_pix_hgt;
  uint16_t em_pix_wid;
};

struct GRAPH;

struct LINE
{
    uint8_t field_0;
    uint8_t field_1;
    uint8_t field_2;
    uint8_t field_3;
    uint32_t field_4;
    uint8_t *lineattr;
    uint8_t *linebuf;
};

struct PSCREEN
{
    uint32_t field_0;
    uint32_t field_4;
    struct LINE *linedata;
};

struct PATHNAME {
    uint16_t start;
    uint16_t diroff;
    uint16_t diroffpreroot;
    uint16_t filenameoff;
    uint16_t dirlen;
    uint16_t namelen;
    uint16_t offext;
    uint16_t extlen;
    char str[0];
};

struct MACXRTNS {
    const char * (*get_mac_text)(int16_t n);
    uint32_t mac_stop;
    uint32_t get_mac_name;
    uint32_t mac_restart;
};

#pragma pack(pop)
#endif

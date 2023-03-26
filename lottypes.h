#ifndef __LOTTYPES_H
#define __LOTTYPES_H

#define INVALID_HANDLE_VALUE ((struct SYSHANDLE *)-1)
#define MAX_ROW 8191

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
    uint8_t dirty;
    uint8_t _padding[3];
    uint32_t maxy;
    uint8_t *lineattr;
    char *linebuf;
};

struct PSCREEN
{
    uint8_t dirty;
    uint8_t _padding[3];
    uint32_t nlines;
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
    void (*get_mac_name)(char **str);
    uint32_t mac_restart;
};

struct CELLCOORD {
    uint16_t row;
    uint8_t sheet;
    uint8_t col;
};

struct LINEFUNCS {
    int (*lfprint)(char *str, unsigned len);
    int (*lfmove)(int ypos, int xpos);
};

struct SCREENPOS {
    uint32_t line;
    uint32_t col;
};

struct SYSHANDLE {
    int fd;
    uint16_t flags;
    uint16_t _pad;
};

typedef struct SYSHANDLE SYSHANDLE;

struct RECT {
    uint16_t topleftrow;
    uint16_t topleftcol;
    uint16_t height;
    uint16_t width;
};

struct DISPLAYWINDOW {
    uint16_t *row_invalid;
    uint16_t *col_invalid;
    struct RECT win;
    struct RECT regionb;
    uint16_t sheetnum;
    uint16_t field_1A;
    uint16_t field_1C;
    uint16_t field_1E;
    uint16_t field_20;
    uint16_t field_22;
    uint16_t xpos;
    uint16_t ypos;
    uint16_t real_height;
    uint16_t columns;
    uint8_t field_2C;
    uint8_t field_2D;
    struct RECT regionc;
    struct RECT regiona;
    struct RECT regiond;
    uint16_t field_46;
};

struct RESHDR {
    uint32_t magic;
    uint16_t version;
    uint16_t groups;
    uint32_t data[0];
};

typedef struct RESHDR RESHDR;

#pragma pack(pop)
#endif

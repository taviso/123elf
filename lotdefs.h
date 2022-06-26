#ifndef __LOTDEFS_H
#define __LOTDEFS_H

#include <stdint.h>

enum {
    FILE_MODE_UNIX,
    FILE_MODE_DOSUPPER,
    FILE_MODE_DOSLOWER,
};

extern uint8_t *vmr[];
extern struct SCREENPOS vpos;
extern struct SCREENPOS currpos;
extern struct SCREENPOS real_pos;
extern uint16_t inprint;
extern uint8_t ref_cur_attr;

extern uint16_t *displayed_window;

extern int get_column_labels(uint16_t, uint16_t, char *, uint16_t);

extern int (*x_disp_txt_set_pos)(uint16_t, uint16_t);
extern int (*x_disp_txt_write)(uint16_t byteslen, char *lmbcsptr, int attrs);

extern int V3_disp_grph_compute_view(void *);
extern int V3_disp_grph_process(void *);
extern int V3_disp_grph_set_cur_view(void *);
extern int init_unix_display_code();
extern void set_actual_grph_type();
extern int disp_txt_init(char *bdlpath);

extern char *char_set_bundle;
extern void *Find_changes;
extern void (*Flush)(void);
extern void (*Check_cursor)(void);
extern int (*Initsc)(void);
extern int (*Clrandhome)(void);
extern int (*Clrtoend)(void);
extern int (*Clrtobot)(void);
extern int (*Home)(void);
extern int (*Atset)(char);
extern void (*opcodes[])(void);
extern uint16_t display_turned_off;
extern void gen_disp_txt_clear();
extern void gen_disp_txt_copy();
extern void gen_disp_txt_curs_off();
extern void gen_disp_txt_curs_on();
extern void gen_disp_txt_curs_type();
extern void gen_disp_txt_fg_clear();
extern void gen_disp_txt_fit();
extern void gen_disp_txt_lock();
extern void gen_disp_txt_set_bg();
extern int gen_disp_txt_set_pos(uint16_t, uint16_t);
extern void gen_disp_txt_set_pos_hpu();
extern void gen_disp_txt_size();
extern void gen_disp_txt_sync();
extern void gen_disp_txt_unlock();
extern int gen_disp_txt_write(uint16_t, char *, int);
extern void gen_disp_txt_zone();
extern void stdio_flush();
extern void tty_disp_close();
extern void tty_disp_close_retain_termcap();
extern void tty_disp_post_system();
extern void tty_disp_pre_system();
extern void *x_disp_close;
extern void *x_disp_close_retain_termcap;
extern void *x_disp_graph;
extern void *x_disp_grph_compute_view;
extern void *x_disp_grph_process;
extern void *x_disp_grph_set_cur_view;
extern void *x_disp_grph_txt_fit;
extern void *x_disp_grph_txt_size;
extern void *x_disp_grph_load_font;
extern void *x_disp_info;
extern void *x_disp_open;
extern void *x_disp_post_system;
extern void *x_disp_pre_system;
extern void *x_disp_text;
extern void *x_disp_txt_clear;
extern void *x_disp_txt_copy;
extern void (*x_disp_txt_curs_off)();
extern void (*x_disp_txt_curs_on)();
extern void *x_disp_txt_curs_type;
extern void *x_disp_txt_fg_clear;
extern void *x_disp_txt_fit;
extern void *x_disp_txt_lock;
extern void *x_disp_txt_set_bg;
extern void *x_disp_txt_set_pos_hpu;
extern void *x_disp_txt_size;
extern void *x_disp_txt_sync;
extern void *x_disp_txt_unlock;
extern void *x_disp_txt_zone;
extern void *dliopen;
extern void *dliclose;
extern int (*Replace_cursor)(void);

extern int MapX(uint16_t);
extern int MapY(uint16_t);

extern uint16_t banner_printed;

extern int16_t encode_date(int16_t *datenums);
extern int16_t get_integer();
extern int16_t check_three_numbers();
extern int undo_on_cmd();
extern int undo_off_cmd();
extern int reset_undo(int);
extern void full_redisplay();
extern int erase_screen();
extern int invalidate_screen();
extern int16_t need_to_close;

extern struct PSCREEN pscreen;
extern struct PSCREEN dscreen;
// Note: these are structures.
extern uint8_t bg_equiv_map[64];
extern uint8_t fg_equiv_map[16];

extern struct LINEFUNCS *lfvec;
extern char *opline;
extern uint32_t scr_init_state;
extern int get_screen_size();
extern void *lts_malloc(size_t size);
extern int clear_screen_buffer(struct PSCREEN *screenbuf);
extern char *tc_setup_line_funcs();

extern struct LOTUSFUNCS *core_funcs;
extern int RastHandle;
extern int16_t file_mode;

extern void slash_convert(char *path, uint16_t len);
extern uint16_t fname_real_len(const char *start, const char *end);
extern int file_path_expand(struct PATHNAME *src);
extern int check_lengths(char *);
extern uint16_t *get_fname_content_xlt_tbl(char *, uint16_t);
extern int file_name_clear(struct PATHNAME *name);
extern int16_t file_name_reduce(char *dst, char *pathname);
extern void file_name_case_check(char *pathname);
extern void fallback_and_coerce_case(char *path);
extern int16_t char_size(char);
extern int peek_next_mbcs(char *);
extern uint16_t resource_substr_match(uint16_t resid, char **matchpart);
extern void macro_buff_run(struct MACXRTNS *callbacks);
extern int in_rdy_mode();
extern void kfqueue_submit_kfun(uint16_t kfun);
extern int macro_key_run(char key);
extern struct CELLCOORD xyz2coord(uint8_t col, uint16_t row, uint16_t sheet);
extern struct CELLCOORD get_cellpointer();
extern bool coord_in_range(struct CELLCOORD cell, struct CELLCOORD rngstart, struct CELLCOORD rngend);
extern char * access_resource(uint16_t resid);
extern int16_t file_name_compose(char *filename, struct PATHNAME *path, char *root, uint16_t useroot);
int16_t print_file_in_use(struct PATHNAME *path);
extern int file_access_read(struct PATHNAME *pathname, struct SYSHANDLE **outfd, int16_t share);
extern int get_resource_handle(int16_t);
extern int file_read_line(int *fd, uint16_t bufsiz, char *buf, uint16_t *numread);
extern int16_t convert_from_table(uint16_t *table, char *inbuf, char *outbuf, int16_t outbufsiz);
extern int cell_immutable(struct CELLCOORD cell);
extern int make_label_cell(struct CELLCOORD cell, char *label, uint8_t labeltype);
extern int16_t peek_next_sbcs(char *);
extern uint16_t find_first_sbcs(char **haystack, uint16_t needle);
extern int skip_next_mbcs(char **strptr);
extern int get_string_end(char **str);
extern int import_outof_bounds(uint16_t maxcol, uint16_t maxrow);
extern int16_t parse_number(char *num, int16_t len);
extern int16_t make_number_cell(struct CELLCOORD cell);
extern int file_finished_shell(struct SYSHANDLE **fd, uint16_t result);
extern int drop_one();
extern int sheet_modified(int16_t sheetnum);
#endif

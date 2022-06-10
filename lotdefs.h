#ifndef __LOTDEFS_H
#define __LOTDEFS_H

#include <stdint.h>

enum {
    FILE_MODE_UNIX,
    FILE_MODE_DOSUPPER,
    FILE_MODE_DOSLOWER,
};

extern uint8_t *vmr[];

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
extern void *Flush;
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
extern void tty_find_changes();
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
extern uint8_t bg_equiv_map[36];
extern uint8_t fg_equiv_map[36];

extern uint8_t *lfvec;
extern uint8_t *opline;
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
extern int16_t *get_fname_content_xlt_tbl(char *, uint16_t);
extern int file_name_clear(struct PATHNAME *name);
extern int16_t file_name_reduce(char *dst, char *pathname);
extern void file_name_case_check(char *pathname);
extern void fallback_and_coerce_case(char *path);
extern int16_t char_size(char);
extern int peek_next_mbcs(char *);
extern uint16_t resource_substr_match(uint16_t resid, char **matchpart);
extern void macro_buff_run(struct MACXRTNS *callbacks);
#endif

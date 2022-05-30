#ifndef __LOTDEFS_H
#define __LOTDEFS_H


extern uint8_t *vmr[];

extern uint16_t *displayed_window;

extern int get_column_labels(uint16_t, uint16_t, uint8_t *, uint16_t);

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

#endif

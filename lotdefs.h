#ifndef __LOTDEFS_H
#define __LOTDEFS_H


extern uint8_t *vmr[];

extern uint16_t *displayed_window;

extern int get_column_labels(uint16_t, uint16_t, uint8_t *, uint16_t);

extern int (*x_disp_txt_set_pos)(uint16_t, uint16_t);
extern int (*x_disp_txt_write)(uint16_t byteslen, char *lmbcsptr, int attrs);

#endif

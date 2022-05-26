#ifndef __LOTCALLS_H
#define __LOTCALLS_H

#pragma pack(push, 1)

struct LOTUSFUNCS
{
    int   (*drv_max_segsize)(void);
    void *(*alloc_mptr)(int vmr, int size, int tag);
    void  (*free_mptr)(void *ptr, int size);
    void *(*alloc_fixed_block)(int size);
    int   (*free_fixed_block)(void *ptr, int, int size);
    void *(*drv_map_mptr)(void *m, int v);
    void *(*drv_get_vmr)(int v);
    void *drv_unmap_vmr;
    void *drv_yield_test;
    void *file_name_parse;
    void *file_exist;
    void *file_find;
    void *file_access_read;
    void *file_get_fileinfo;
    void *file_get_filepointer;
    void *file_lseek;
    void *file_read;
    void *file_access_finished;
    void *file_access_write;
    void *file_write;
    int   (*open_rasterizer)(struct DEVDATA *);
    void *rast_init_device;
    void  (*set_strip)(int, int);
    int   (*raster)(int, void *ptr);
    void  (*close_rasterizer)(int hdl);
    int   (*rast_compute_view)(int, void *ptr);
    int   (*rast_txt_size)(int, int, void *ptr1, void *ptr2);
    int   (*rast_txt_fit)(int, int, void *a, int, void *b, void *c);
    void *p_link_init;
    void *p_link_transmit;
    void *p_link_term;
    void *p_link_start_job;
    void *p_link_end_job;
    void *p_signal_user;
    void *p_print_link_alert;
    void *p_print_link_message;
    void *country_reduce_string;
    void *country_convert_string;
    void *dyload;
    void *dyunload;
    void *drv_wait_nticks;
    unsigned (*set_disp_buf)(unsigned *selector, int base, int limit);
    void *circle_position;
    void *drv_display_reset;
    void *open_system_file;
    void *drv_largest_avail;
    void *quit_now;
    void *is_encoded;
    void *command;
    void *x_get_date;
    void *x_get_time;
    void *x_sysbeep;
    void *convert_from_table;
    void *reduce_from_table;
    void *country_text_cnv;
    void (*drop_disp_buf)(int seg);
    void *x_cntry_tbl_release2;
    void *char_size;
};

#pragma pack(pop)
#endif

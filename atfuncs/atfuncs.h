#ifndef __ATFUNCS_H
#define __ATFUNCS_H

enum {
    AT_NA,
    AT_ERR,
    AT_ABS,
    AT_INT,
    AT_SQRT,
    AT_LOG,
    AT_PRODUCT = 83, // Originally unimplemented function @CALL()
    AT_WEEKDAY = 110,
    AT_NUM_FUNCS = 184
};

extern int16_t (*functions[AT_NUM_FUNCS])(void);
extern int8_t fn_numargs[AT_NUM_FUNCS];

int16_t at_date();
int16_t at_weekday();
int16_t at_product();

#endif

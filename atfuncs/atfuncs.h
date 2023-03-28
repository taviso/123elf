#ifndef __ATFUNCS_H
#define __ATFUNCS_H

#define AT_ARGS_VARIABLE 0x80

// Apply an operation to a RANGE.
extern int16_t range_scan_tos(void (*op)());

// Various operations.
extern void op_add();
extern void op_mul();

// Stack operations.
extern int16_t date_valid(int16_t dateval[3]);
extern int16_t encode_date(int16_t *datenums);

extern int16_t check_three_numbers();
extern int16_t check_one_number();
extern int16_t get_integer();
extern int16_t push_integer(uint16_t val);
extern int16_t push_one();
extern int16_t push_zero();
extern void mod_real_d();
extern void int_real_d();
extern void drop_one();
extern void swap_TOS();


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

typedef int16_t (*atfunc_t)(void);

extern atfunc_t functions[AT_NUM_FUNCS];
extern int8_t fn_numargs[AT_NUM_FUNCS];

int16_t at_date();
int16_t at_weekday();
int16_t at_product(int16_t cnt);

void init_at_funcs();

#endif

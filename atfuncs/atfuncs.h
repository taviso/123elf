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

// @PRODUCT replaced the previously unimplemented @CALL
#define AT_CALL AT_PRODUCT

enum {
    AT_NA,
    AT_ERR,
    AT_ABS,
    AT_INT,
    AT_SQRT,
    AT_LOG,
    AT_LN,
    AT_PI,
    AT_SIN,
    AT_COS,
    AT_TAN,
    AT_ATAN2,
    AT_ATAN,
    AT_ASIN,
    AT_ACOS,
    AT_EXP,
    AT_MOD,
    AT_CHOOSE,
    AT_ISNA,
    AT_ISERR,
    AT_FALSE,
    AT_TRUE,
    AT_RAND,
    AT_DATE,
    AT_NOW,
    AT_PMT,
    AT_PV,
    AT_FV,
    AT_IF,
    AT_DAY,
    AT_MONTH,
    AT_YEAR,
    AT_ROUND,
    AT_TIME,
    AT_HOUR,
    AT_MINUTE,
    AT_SECOND,
    AT_ISNUMBER,
    AT_ISSTRING,
    AT_LENGTH,
    AT_VALUE,
    AT_STRING,
    AT_MID,
    AT_CHAR,
    AT_CODE,
    AT_FIND,
    AT_DATEVALUE,
    AT_TIMEVALUE,
    AT_CELLPOINTER,
    AT_SUM,
    AT_AVG,
    AT_COUNT,
    AT_MIN,
    AT_MAX,
    AT_VLOOKUP,
    AT_NPV,
    AT_VAR,
    AT_STD,
    AT_IRR,
    AT_HLOOKUP,
    AT_DSUM,
    AT_DAVG,
    AT_DCOUNT,
    AT_DMIN,
    AT_DMAX,
    AT_DVAR,
    AT_DSTD,
    AT_INDEX,
    AT_COLS,
    AT_ROWS,
    AT_REPEAT,
    AT_UPPER,
    AT_LOWER,
    AT_LEFT,
    AT_RIGHT,
    AT_REPLACE,
    AT_PROPER,
    AT_CELL,
    AT_TRIM,
    AT_CLEAN,
    AT_S,
    AT_N,
    AT_EXACT,
    AT_PRODUCT,
    AT_AT,
    AT_RATE,
    AT_TERM,
    AT_CTERM,
    AT_SLN,
    AT_SYD,
    AT_DDB,
    AT_SPLFUNCS,
    AT_SHEETS,
    AT_INFO,
    AT_SUMPRODUCT,
    AT_ISRANGE,
    AT_DGET,
    AT_DQUERY,
    AT_COORD,
    AT_MATCH,
    AT_TODAY,
    AT_VDB,
    AT_DVARS,
    AT_DSTDS,
    AT_VARS,
    AT_STDS,
    AT_D360,
    AT_BLANK,
    AT_ISAPP,
    AT_ISAAF,
    AT_WEEKDAY,
    AT_DATEDIF,
    AT_RANK,
    AT_NUMBERSTRING,
    AT_DATESTRING,
    AT_DECIMAL,
    AT_HEX,
    AT_DB,
    AT_PMTI,
    AT_SPI,
    AT_FULLP,
    AT_HALFP,
    AT_PUREAVG,
    AT_PURECOUNT,
    AT_PUREMAX,
    AT_PUREMIN,
    AT_PURESTD,
    AT_PUREVAR,
    AT_PURESTDS,
    AT_PUREVARS,
    AT_PMT2,
    AT_PV2,
    AT_FV2,
    AT_TERM2,
    AT_NUM_FUNCS,
};

typedef int16_t (*atfunc_t)(void);

extern atfunc_t functions[AT_NUM_FUNCS];
extern int8_t fn_numargs[AT_NUM_FUNCS];

int16_t at_date();
int16_t at_weekday();
int16_t at_product(int16_t cnt);

void init_at_funcs();

#endif

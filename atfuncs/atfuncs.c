#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "lottypes.h"
#include "lotfuncs.h"
#include "lotdefs.h"
#include "atfuncs.h"

void init_at_funcs()
{
    // Setup any @functions here.
    functions[AT_WEEKDAY] = (atfunc_t) at_weekday;
    functions[AT_PRODUCT] = (atfunc_t) at_product;
    functions[AT_SYSTEM] = (atfunc_t) at_system;

    // You can pass 1 or more parameters to @PRODUCT().
    fn_numargs[AT_PRODUCT] = AT_ARGS_VARIABLE | 1;
    fn_numargs[AT_SYSTEM] = 1;
}

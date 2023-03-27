#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "lottypes.h"
#include "lotfuncs.h"
#include "lotdefs.h"

int16_t at_product(int16_t n)
{
    int16_t result;

    push_one();

    while (n--) {
        swap_TOS();
        result = range_scan_tos(op_mul);
    }

    return result;
}

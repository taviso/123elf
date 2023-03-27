#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "lottypes.h"
#include "lotfuncs.h"
#include "lotdefs.h"

int16_t at_weekday()
{
    if (!check_one_number()) {
        return 0;
    }

    push_integer(7);

    // Find the day number.
    mod_real_d();
    return 1;
}

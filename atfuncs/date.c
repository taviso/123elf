#include <stdint.h>
#include <stddef.h>

#include "../lottypes.h"
#include "../lotfuncs.h"
#include "../lotdefs.h"

int16_t at_date()
{
    int16_t result;
    int16_t datenums[3];

    result = check_three_numbers();

    if (result)
    {
        datenums[2] = get_integer();    // Day
        datenums[1] = get_integer();    // Month
        datenums[0] = get_integer();    // Year

        // If this is a four digit year, adjust it to work with the lotus @DATE
        // syntax by changing it to an offset from 1900. This should work for
        // years up to 2100.
        if (datenums[0] > 999) {
            datenums[0] -= 1900;
        }
        return encode_date(datenums);
    }

    return result;
}

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>

#include "lottypes.h"
#include "lotfuncs.h"
#include "lotdefs.h"
#include "atfuncs.h"

int16_t at_system()
{
    FILE *cmd;
    char buf[512] = {0};

    if (!check_one_string()) {
        return 0;
    }

    // @SYSTEM() is only permitted when autoexec is enabled.
    if (get_allow_autoexec() == false) {
        drop_one();
        push_na();
        return 1;
    }

    cmd = popen(peek_string(0), "r");

    if (cmd == NULL) {
        return drop_one_push_err();
    }

    // We can read at most 512 bytes of output.
    fscanf(cmd, "%511[^\n]", buf);

    // We return @ERR if the command returned failure.
    if (pclose(cmd) != EXIT_SUCCESS) {
        return drop_one_push_err();
    }

    // Return the output as a string.
    return drop_one_push_stack_string(buf);
}

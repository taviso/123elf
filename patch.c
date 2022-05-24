#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <err.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/utsname.h>

int lic_init(void)
{
    fprintf(stderr, "🏴‍☠️  license granted 🏴‍☠️\n");
    return 2;
}

// Printing does not currently work, but this can be fixed in future.
int open_printer_drivers(void)
{
    return 1;
}
int close_printer_drivers(void)
{
    return 0;
}
int load_printer_drivers(void)
{
    return 0;
}
int read_print_config_dir(void)
{
    return 0;
}

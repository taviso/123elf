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

int __wrap_lic_init(void)
{
    fprintf(stderr, "🏴‍☠️  license granted 🏴‍☠️\n");
    return 2;
}

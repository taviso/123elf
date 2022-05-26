#define inline
#define EOVERFLOW E2BIG
#define ENOSYS EINVAL
#define HAVE_VSNPRINTF 1

#define __KERNEL__

#include <stddef.h>

#define abs(x) (((x) < 0) ? (x) * -1 : (x))

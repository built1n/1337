#include "globals.h"

void fatal(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    char buf[256];
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    fprintf(stderr, "FATAL: %s\n", buf);

    exit(EXIT_FAILURE);
}

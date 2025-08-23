#include <stdarg.h>
#include <stdio.h>
#include "common.h"

int debug_enabled = 0;

void debug_print(const char *fmt, ...) {
    if (!debug_enabled) return;
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "[DEBUG] ");
    vfprintf(stderr, fmt, args);
    va_end(args);
}

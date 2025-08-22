#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
extern int debug_enabled;

void debug_print(const char *fmt, ...);

#define DEBUG_PRINT(...) \
    do { if (debug_enabled) fprintf(stderr, __VA_ARGS__); } while (0)

#endif

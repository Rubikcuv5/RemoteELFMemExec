#ifndef DEBUG_H
#define DEBUG_H

#ifdef DEBUG
    #include <stdio.h>
    #define DEBUG_PRINT(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(fmt, ...) /* No-op */
#endif

#endif // DEBUG_H

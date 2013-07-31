#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include "util.h"

void _assert(int pred, const char *s)
{
    if (!pred) {
        perror(s);
        exit(errno);
    }
}

void _dump_mem(const unsigned char* buf, size_t len)
{
    size_t i;
    for (i = 0; i < len; ++i)
        printf("%02x ", buf[i]);
    putchar('\n');
}

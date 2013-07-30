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

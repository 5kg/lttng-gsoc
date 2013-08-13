#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include "util.h"

/* Assert helper function */
void __assert(int pred, const char *s, const char* file, int line)
{
    if (!pred) {
        fprintf(stderr, "%s:%d : ", file, line);
        perror(s);
        exit(errno);
    }
}

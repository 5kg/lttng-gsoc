#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include "util.h"

/* Assert helper function */
void _assert(int pred, const char *s)
{
    if (!pred) {
        perror(s);
        exit(errno);
    }
}

/* Set a given fd to nonblocking */
void set_nonblocking(int fd)
{
    int flags, ret;

    flags = fcntl(fd, F_GETFL);
    _assert(flags >= 0, "fcntl");

    flags |= O_NONBLOCK;
    ret = fcntl(fd, F_SETFL, flags);
    _assert(ret != -1, "fcntl");
}

/* Dump a section of memory to stderr for debug */
void _dump_mem(const unsigned char* buf, size_t len)
{
    size_t i;
    for (i = 0; i < len; ++i)
        fprintf(stderr, "%02x ", buf[i]);
    putchar('\n');
}

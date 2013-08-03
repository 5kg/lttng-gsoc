#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <stddef.h>
#include <sys/mman.h>
#include "util.h"

#define PAGESIZE 4096

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

/* Set a section of memory to be writable */
void set_writable(void* addr, size_t len)
{
    int ret;
    ptrdiff_t mask = ~0xfffUL;

    ret = mprotect((void *) ((ptrdiff_t) addr & mask),
                   (len + PAGESIZE - 1) & mask,
                   PROT_READ|PROT_WRITE|PROT_EXEC);
    _assert(ret != -1, "mprotect");
}

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ptrace.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/epoll.h>

#define MAX_LISTEN 16
#define MAX_EVENTS 64

void __attribute__ ((constructor)) static kaji_init(void);
void __attribute__ ((destructor)) static kaji_fini(void);
void static kaji_loop(void *arg);

static void set_nonblocking(int fd);
static void kaji_assert(int pred, const char *s);

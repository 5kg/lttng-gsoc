#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/ptrace.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/epoll.h>
#include <sys/wait.h>

#define TRACEPOINT_DEFINE
#define TRACEPOINT_CREATE_PROBES
#include "ust_kaji_test.h"

#define MAX_LISTEN 16
#define MAX_EVENTS 64

static void __attribute__ ((constructor)) kaji_init(void);
static void __attribute__ ((destructor)) kaji_fini(void);
static void* kaji_loop(void *arg);
static void kaji_inject(void *addr);
static void kaji_probe(void);

static void set_nonblocking(int fd);
static void kaji_assert(int pred, const char *s);

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
#define MAX_INSN_LENGTH 16

static void __attribute__ ((constructor)) kaji_init(void);
static void __attribute__ ((destructor)) kaji_fini(void);
static void* kaji_loop(void *arg);
static void kaji_inject(void *addr);
void kaji_probe(void);
static int kaji_get_insn_len(void *addr);
static void kaji_install_trampoline(void *addr, const char* orig_insn);

static void set_nonblocking(int fd);
static void _assert(int pred, const char *s);

extern void kaji_trampoline();

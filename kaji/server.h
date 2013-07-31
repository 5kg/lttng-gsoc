#pragma once

static void __attribute__ ((constructor)) kaji_init(void);
static void __attribute__ ((destructor)) kaji_fini(void);
static void* kaji_loop(void *arg);
void kaji_probe(void);
static void kaji_install_trampoline(void *addr, size_t len);

static void set_nonblocking(int fd);

extern void* kaji_trampoline;
extern void* __kaji_trampoline_placeholder;

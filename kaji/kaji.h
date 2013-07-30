#pragma once

static void __attribute__ ((constructor)) kaji_init(void);
static void __attribute__ ((destructor)) kaji_fini(void);
static void* kaji_loop(void *arg);
void kaji_probe(void);
static int kaji_get_insn_len(void *addr);
static void kaji_install_trampoline(void *addr);

static void set_nonblocking(int fd);

extern void* kaji_trampoline;
extern void* __kaji_trampoline_placeholder;

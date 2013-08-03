#pragma once

static void __attribute__ ((constructor)) kaji_init(void);
static void __attribute__ ((destructor)) kaji_fini(void);
static void* kaji_loop(void *arg);
static void kaji_install_trampoline(void *addr, size_t len);

void kaji_probe(void);

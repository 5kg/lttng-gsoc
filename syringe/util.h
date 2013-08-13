#pragma once

#define _assert(pred, s) __assert(pred, s, __FILE__, __LINE__)

void __assert(int pred, const char *s, const char* file, int line);

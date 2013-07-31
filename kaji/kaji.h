#pragma once

struct kaji_command {
    void *addr;
    size_t len;
};

#define KAJI_REPLY_OK 42

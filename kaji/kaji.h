#pragma once

struct kaji_command {
    void *addr;     /* Address to instrument probe */
    size_t len;     /* Length of original instruction */
};

enum kaji_command_reply {
    KAJI_REPLY_OK = 42,
};

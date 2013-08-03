#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/epoll.h>

#include "server.h"
#include "kaji.h"
#include "util.h"

/* Define tracepoint event */
#define TRACEPOINT_DEFINE
#define TRACEPOINT_CREATE_PROBES
#include "ust_kaji_test.h"

#define MAX_LISTEN 16
#define MAX_EPOLL_EVENTS 64

#define min(a, b) ( (a) > (b) ? (a) : (b) )

extern void* kaji_trampoline;
extern void* __kaji_trampoline_placeholder;

void __attribute__ ((constructor)) kaji_init(void)
{
    pthread_t loop_thread;
    pthread_create(&loop_thread, NULL, kaji_loop, NULL);
}

void __attribute__ ((destructor)) kaji_fini(void)
{
    /* TODO */
}

/* Main in-process-agent event loop */
void* kaji_loop(void *arg)
{
    int sock_fd, ret, efd;
    struct sockaddr_un addr;
    const char pathname[] = "/tmp/kaji.sock";
    struct epoll_event ev;
    struct epoll_event *events;

    /* Setup unix domain socket and listen */
    sock_fd = socket(PF_UNIX, SOCK_STREAM, 0);
    _assert(socket >= 0, "socket");

    set_nonblocking(sock_fd);

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    snprintf(addr.sun_path, sizeof(addr.sun_path), pathname);
    addr.sun_path[sizeof(addr.sun_path) - 1] = '\0';
    (void) unlink(pathname);

    ret = bind(sock_fd, (struct sockaddr *) &addr, sizeof(struct sockaddr_un));
    _assert(ret == 0, "bind");

    ret = listen(sock_fd, MAX_LISTEN);
    _assert(ret == 0, "listen");

    /* Setup epoll event loop */
    efd = epoll_create1(0);
    _assert(efd != -1, "epoll_create1");

    memset(&ev, 0, sizeof(ev));
    ev.data.fd = sock_fd;
    ev.events = EPOLLIN;
    ret = epoll_ctl(efd, EPOLL_CTL_ADD, sock_fd, &ev);
    _assert(efd != -1, "epoll_ctl");

    events = calloc(MAX_EPOLL_EVENTS, sizeof(struct epoll_event));

    /* Event loop starts here */
    for(;;) {
        int nr_events, i;

        nr_events = epoll_wait(efd, events, MAX_EPOLL_EVENTS, -1);
        for (i = 0; i < nr_events; i++) {
            if (events[i].data.fd == sock_fd) {
                /* We have a new client, accept it and add to epoll queue*/
                int conn_fd;

                conn_fd = accept(sock_fd, NULL, 0), ret;
                set_nonblocking(conn_fd);

                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = conn_fd;
                ret = epoll_ctl(efd, EPOLL_CTL_ADD, conn_fd, &ev);
                _assert(efd != -1, "epoll_ctl");
            } else {
                /* We've got a new command, handle it */
                ssize_t count;
                char buffer[4096];
                struct kaji_command command;
                int reply;

                /* TODO: handle possible partial message */
                count = read(events[i].data.fd, buffer, sizeof(buffer));
                if (count > 0) {
                    _assert(count == sizeof(struct kaji_command), "read");

                    memcpy(&command, buffer, sizeof(struct kaji_command));
                    kaji_install_trampoline(command.addr, command.len);

                    reply = KAJI_REPLY_OK;
                    write(events[i].data.fd, &reply, sizeof(reply));
                }
            }
        }
    }

    // TODO: Fix memory/fd leak here
    free(events);
    close(efd);
    close(sock_fd);

    return NULL;
}

/* Install trampoline to instrumented process */
void kaji_install_trampoline(void* addr, size_t len)
{
    unsigned char jmp_buff[] = { 0xe9, 0, 0, 0 , 0 };
    int64_t jmp_offset;

    /*
    memcpy(__kaji_trampoline_placeholder, addr, len);
    jmp_offset = addr - (__kaji_trampoline_placeholder + orig_insn_len);
    memcpy(jmp_buff + 1, &jmp_offset, sizeof(jmp_offset));
    kaji_write_insn(pid, __kaji_trampoline_placeholder + orig_insn_len,
            sizeof(jmp_buff), jmp_buff);

    jmp_offset = kaji_trampoline - addr;
    memcpy(jmp_buff + 1, &jmp_offset, sizeof(jmp_offset));
    kaji_write_insn(pid, addr, sizeof(jmp_buff), jmp_buff);
    */
}

/* This is the instrumented probe */
void kaji_probe()
{
    tracepoint(ust_kaji_test, tptest);
}

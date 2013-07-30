#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/epoll.h>

#include "kaji.h"
#include "util.h"

#define TRACEPOINT_DEFINE
#define TRACEPOINT_CREATE_PROBES
#include "ust_kaji_test.h"

#define MAX_LISTEN 16
#define MAX_EVENTS 64
#define MAX_INSN_LENGTH 16

#define min(a, b) ( (a) > (b) ? (a) : (b) )

void __attribute__ ((constructor)) kaji_init(void)
{
    pthread_t loop_thread;
    pthread_create(&loop_thread, NULL, kaji_loop, NULL);
}

void __attribute__ ((destructor)) kaji_fini(void)
{
}


void* kaji_loop(void *arg)
{
    int sock_fd, ret, efd;
    struct sockaddr_un addr;
    const char pathname[] = "/tmp/kaji.sock";
    struct epoll_event ev;
    struct epoll_event *events;

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

    efd = epoll_create1(0);
    _assert(efd != -1, "epoll_create1");

    memset(&ev, 0, sizeof(ev));
    ev.data.fd = sock_fd;
    ev.events = EPOLLIN;
    ret = epoll_ctl(efd, EPOLL_CTL_ADD, sock_fd, &ev);
    _assert(efd != -1, "epoll_ctl");

    events = calloc(MAX_EVENTS, sizeof(struct epoll_event));
    for(;;) {
        int nr_events, i;

        nr_events = epoll_wait(efd, events, MAX_EVENTS, -1);
        for (i = 0; i < nr_events; i++) {
            if (events[i].data.fd == sock_fd) {
                int conn_fd;

                conn_fd = accept(sock_fd, NULL, 0), ret;
                set_nonblocking(conn_fd);

                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = conn_fd;
                ret = epoll_ctl(efd, EPOLL_CTL_ADD, conn_fd, &ev);
                _assert(efd != -1, "epoll_ctl");
            } else {
                ssize_t count;
                char buffer[4096];

                for (;;) {
                    count = read(events[i].data.fd, buffer, sizeof(buffer));
                    if (count == -1) {
                        _assert(errno == EAGAIN, "read");
                        if (errno != EAGAIN) {
                            perror("read");
                            close(events[i].data.fd);
                            exit(errno);
                        }
                        break;
                    } else if (count == 0) {
                        break;
                    }
                    buffer[count] = '\0';
                    if (count > 0) {
                        void *addr = (void*) strtoul(buffer, NULL, 0);
                        kaji_install_trampoline(addr);
                    }
                }
            }
        }
    }

    // TODO: Mem-leak here
    free(events);
    close(efd);
    close(sock_fd);

    return NULL;
}

void set_nonblocking(int fd)
{
    int flags, ret;

    flags = fcntl(fd, F_GETFL);
    _assert(flags >= 0, "fcntl");

    flags |= O_NONBLOCK;
    ret = fcntl(fd, F_SETFL, flags);
    _assert(ret != -1, "fcntl");
}


void kaji_install_trampoline(void* addr)
{
    size_t orig_insn_len = kaji_get_insn_len(addr);
    unsigned char orig_insn_buff[MAX_INSN_LENGTH];
    int64_t jmp_offset;
    unsigned char jmp_buff[] = { 0xe9, 0, 0, 0 , 0 };

    //kaji_read_insn(pid, addr, orig_insn_len, orig_insn_buff);
    //kaji_write_insn(pid, __kaji_trampoline_placeholder,
    //        orig_insn_len, orig_insn_buff);

    jmp_offset = addr - (__kaji_trampoline_placeholder + orig_insn_len);
    memcpy(jmp_buff + 1, &jmp_offset, sizeof(jmp_offset));
    //kaji_write_insn(pid, __kaji_trampoline_placeholder + orig_insn_len,
    //        sizeof(jmp_buff), jmp_buff);

    jmp_offset = kaji_trampoline - addr;
    memcpy(jmp_buff + 1, &jmp_offset, sizeof(jmp_offset));
    //kaji_write_insn(pid, addr, sizeof(jmp_buff), jmp_buff);
}

int kaji_get_insn_len(void* addr)
{
    //TODO
    return 4;
}

void kaji_probe()
{
    tracepoint(ust_kaji_test, tptest);
}

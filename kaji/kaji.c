#include "kaji.h"

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
    kaji_assert(socket >= 0, "socket");

    set_nonblocking(sock_fd);

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    snprintf(addr.sun_path, sizeof(addr.sun_path), pathname);
    addr.sun_path[sizeof(addr.sun_path) - 1] = '\0';
    (void) unlink(pathname);

    ret = bind(sock_fd, (struct sockaddr *) &addr, sizeof(struct sockaddr_un));
    kaji_assert(ret == 0, "bind");

    ret = listen(sock_fd, MAX_LISTEN);
    kaji_assert(ret == 0, "listen");

    efd = epoll_create1(0);
    kaji_assert(efd != -1, "epoll_create1");

    memset(&ev, 0, sizeof(ev));
    ev.data.fd = sock_fd;
    ev.events = EPOLLIN;
    ret = epoll_ctl(efd, EPOLL_CTL_ADD, sock_fd, &ev);
    kaji_assert(efd != -1, "epoll_ctl");

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
                kaji_assert(efd != -1, "epoll_ctl");
            } else {
                ssize_t count;
                char buffer[4096];

                for (;;) {
                    count = read(events[i].data.fd, buffer, sizeof(buffer));
                    if (count == -1) {
                        kaji_assert(errno == EAGAIN, "read");
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
                        kaji_inject(addr);
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
    kaji_assert(flags >= 0, "fcntl");

    flags |= O_NONBLOCK;
    ret = fcntl(fd, F_SETFL, flags);
    kaji_assert(ret != -1, "fcntl");
}

void kaji_assert(int pred, const char *s) {
    if (!pred) {
        perror(s);
        exit(errno);
    }
}

void kaji_inject(void* addr)
{
    int ret, stat, count;
    pid_t ppid = getppid();

    ret = ptrace(PTRACE_ATTACH, ppid, NULL, NULL);
    kaji_assert(ret != -1, "PTRACE_ATTACH");
    ret = waitpid(ppid, &stat, WUNTRACED);
    kaji_assert((ret == ppid) && WIFSTOPPED(stat), "waitpid");

    for (count = 0; count < 4; count += sizeof(long)) {
        errno = 0;
        long word = ptrace(PTRACE_PEEKTEXT, ppid, addr + count, NULL);
        kaji_assert(!errno, "PTRACE_PEEKTEXT");
        printf("%lx ", word);
    }
    putchar('\n');

    //ret = ptrace(PTRACE_CONT, ppid, NULL, NULL);
    //kaji_assert(ret != -1, "PTRACE_CONT");
    ret = ptrace(PTRACE_DETACH, ppid, NULL, 0);
    kaji_assert(ret != -1, "PTRACE_DETACH");
}

void kaji_probe()
{
    tracepoint(ust_kaji_test, tptest);
}

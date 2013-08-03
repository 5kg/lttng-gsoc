#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "kaji.h"
#include "util.h"

void usage(char *progname)
{
    fprintf(stderr, "Usage: %s PID ADDRESS LENGTH\n", progname);
}

int main(int argc, char *argv[])
{
    int sock_fd, ret, stat, reply;
    struct sockaddr_un addr;
    const char pathname[] = "/tmp/kaji.sock";
    struct kaji_command command;
    pid_t pid;

    if (argc != 4) {
        usage(argv[0]);
        exit(-1);
    }

    pid = (pid_t) strtol(argv[1], NULL, 0);

    /*
     * Use ptrace to attach the instrumented process,
     * the in-process-agent thread is untouched.
     */
    ret = ptrace(PTRACE_ATTACH, pid, NULL, NULL);
    _assert(ret != -1, "PTRACE_ATTACH");
    ret = waitpid(pid, &stat, WUNTRACED);
    _assert((ret == pid) && WIFSTOPPED(stat), "waitpid");

    /* Connect to in-process-agent */
    sock_fd = socket(PF_UNIX, SOCK_STREAM, 0);
    _assert(socket >= 0, "socket");

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    snprintf(addr.sun_path, sizeof(addr.sun_path), pathname);
    addr.sun_path[sizeof(addr.sun_path) - 1] = '\0';
    ret = connect(sock_fd, (struct sockaddr*) &addr, sizeof(addr));
    _assert(ret != -1, "connect");

    /* Construct and send command to IPA */
    command.addr = (void*) strtol(argv[2], NULL, 0);
    command.len = (size_t) strtol(argv[3], NULL, 0);
    ret = send(sock_fd, &command, sizeof(command), 0);
    _assert(ret == sizeof(struct kaji_command), "send");

    /* Verify reply form IPA */
    ret = recv(sock_fd, &reply, sizeof(reply), 0);
    printf("%d\n", ret);
    printf("%d\n", reply);
    _assert(ret == sizeof(reply) && reply == KAJI_REPLY_OK, "recv");

    /* Detach the instrumented process */
    ret = ptrace(PTRACE_DETACH, pid, NULL, 0);
    _assert(ret != -1, "PTRACE_DETACH");

    return 0;
}

#include <sys/ptrace.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "util.h"

int main(int argc, char *argv[])
{
    int ret, stat;
    pid_t pid;

    pid = (pid_t) strtol(argv[1], NULL, 0);

    ret = ptrace(PTRACE_ATTACH, pid, NULL, NULL);
    _assert(ret != -1, "PTRACE_ATTACH");
    ret = waitpid(pid, &stat, WUNTRACED);
    _assert((ret == pid) && WIFSTOPPED(stat), "waitpid");

    printf("test");

    ret = ptrace(PTRACE_DETACH, pid, NULL, 0);
    _assert(ret != -1, "PTRACE_DETACH");

    return 0;
}

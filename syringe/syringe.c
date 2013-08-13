#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <stdlib.h>
#include "syringe.h"
#include "util.h"

void syringe_dlopen(pid_t pid, const char* library)
{
    int ret, stat;
    struct user_regs_struct regs;

    /* Use ptrace to attach to the process */
    ret = ptrace(PTRACE_ATTACH, pid, NULL, NULL);
    _assert(ret != -1, "PTRACE_ATTACH");
    ret = waitpid(pid, &stat, WUNTRACED);
    _assert((ret == pid) && WIFSTOPPED(stat), "waitpid");

    /* Save user registers */
    ret = ptrace(PTRACE_GETREGS, pid, NULL, &regs);
    _assert(ret != -1, "PTRACE_GETREGS");
}

### Performance ###
* Overhead
  * Save General Purpose Registers
  * Save Floating Point Registers
    * To turn it off: BPatch::setSaveFPR(false).
    * Might not safe to turn it off if we want to capture floating variables.
    * Dyninst will attempt to analyse if saving floating point registers is required.
  * Generate A Stack Frame
    * To turn it off: BPatch::setInstrStackFrames(false).
    * Default is false.
  * Calculate Thread Index
    * To turn it off: BPatch::setTrampRecursive(true).
    * Safe if instrumentation cannot execute recursively.
  * Test and Set Trampoline Guard
    * Same as above.
  * Execute User Instrumentation
  * Unset Trampoline Guard
  * Clean Stack Frame
  * Restore Floating Point Registers
  * Restore General Purpose Registers

* Detailed Analysis
 * C code for do_stuff

```C
void do_stuff(void)
{
    FILE *file;

    int v = 1;

    file = fopen("/dev/null", "a");
    fprintf(file, "%d", v);
    fclose(file);
    time(NULL);

#ifdef TRACING
    tracepoint(ust_tests_benchmark, tpbench_no_arg);
#endif
}
```

 * Asm code without tracepoint, compiled with -O2

```
Dump of assembler code for function do_stuff:
   0x0000000000400a30 <+0>:     push   %rbx                     # save "%rbx"
   0x0000000000400a31 <+1>:     mov    $0x400b34,%esi           # string "a"
   0x0000000000400a36 <+6>:     mov    $0x400b36,%edi           # string "/dev/null"
   0x0000000000400a3b <+11>:    callq  0x4007b0 <fopen@plt>     # call "fopen"
   0x0000000000400a40 <+16>:    mov    $0x1,%edx                # variable "v"
   0x0000000000400a45 <+21>:    mov    %rax,%rbx                # return value of "fopen", stored in "file"
   0x0000000000400a48 <+24>:    mov    %rax,%rdi                # variable "file"
   0x0000000000400a4b <+27>:    mov    $0x400b40,%esi           # string "%d"
   0x0000000000400a50 <+32>:    xor    %eax,%eax                # number of vector registers "0", see http://stackoverflow.com/questions/6212665/why-is-eax-zeroed-before-a-call-to-printf
   0x0000000000400a52 <+34>:    callq  0x400760 <fprintf@plt>   # call "fprintf"
   0x0000000000400a57 <+39>:    mov    %rbx,%rdi                # variable "file"
   0x0000000000400a5a <+42>:    callq  0x400730 <fclose@plt>    # call "fclose"
   0x0000000000400a5f <+47>:    pop    %rbx                     # restore "%rbx"
   0x0000000000400a60 <+48>:    xor    %edi,%edi                # constant "NULL"
   0x0000000000400a62 <+50>:    jmpq   0x400790 <time@plt>      # call "time"
End of assembler dump.
```

 * Asm code with static tracepoint, compiled with -O2

```
Dump of assembler code for function do_stuff:
   0x0000000000400d70 <+0>:     push   %rbx
   0x0000000000400d71 <+1>:     mov    $0x401844,%esi
   0x0000000000400d76 <+6>:     mov    $0x401846,%edi
   0x0000000000400d7b <+11>:    callq  0x400ae0 <fopen@plt>
   0x0000000000400d80 <+16>:    mov    $0x1,%edx
   0x0000000000400d85 <+21>:    mov    $0x401850,%esi
   0x0000000000400d8a <+26>:    mov    %rax,%rdi
   0x0000000000400d8d <+29>:    mov    %rax,%rbx
   0x0000000000400d90 <+32>:    xor    %eax,%eax
   0x0000000000400d92 <+34>:    callq  0x400a60 <fprintf@plt>
   0x0000000000400d97 <+39>:    mov    %rbx,%rdi
   0x0000000000400d9a <+42>:    callq  0x400a10 <fclose@plt>
   0x0000000000400d9f <+47>:    xor    %edi,%edi                  # same as above
   0x0000000000400da1 <+49>:    callq  0x400a90 <time@plt>        # call "time"
   0x0000000000400da6 <+54>:    mov    0x201abc(%rip),%eax        # state = __tracepoint_ust_tests_benchmark___tpbench_no_arg.state
   0x0000000000400dac <+60>:    test   %eax,%eax                  # if (state != 0)
   0x0000000000400dae <+62>:    jne    0x400db2 <do_stuff+66>     # continue
   0x0000000000400db0 <+64>:    pop    %rbx
   0x0000000000400db1 <+65>:    retq
   0x0000000000400db2 <+66>:    mov    0x201b5f(%rip),%rax        # following is inlined __tracepoint_cb_ust_tests_benchmark___tpbench_no_arg
   0x0000000000400db9 <+73>:    test   %rax,%rax                  # if (!TP_RCU_LINK_TEST())
   0x0000000000400dbc <+76>:    je     0x400db0 <do_stuff+64>     # return
   0x0000000000400dbe <+78>:    callq  *%rax                      # call tp_rcu_read_lock_bp()
   0x0000000000400dc0 <+80>:    mov    0x201aa9(%rip),%rdi        # probes = __tracepoint_ust_tests_benchmark___tpbench_no_arg.probes
   0x0000000000400dc7 <+87>:    callq  *0x201b5b(%rip)            # __tp_probe = tp_rcu_dereference_bp(probes)
   0x0000000000400dcd <+93>:    test   %rax,%rax                  # if (!__tp_probe)
   0x0000000000400dd0 <+96>:    mov    %rax,%rbx                  # %rbx = __tp_probe
   0x0000000000400dd3 <+99>:    je     0x400dea <do_stuff+122>    # goto end
   0x0000000000400dd5 <+101>:   mov    (%rax),%rax                # %rax = __tp_probe->func
loop:
   0x0000000000400dd8 <+104>:   mov    0x8(%rbx),%rdi             # %rdi = __tp_probe->data
   0x0000000000400ddc <+108>:   add    $0x10,%rbx                 # __tp_probe++;
   0x0000000000400de0 <+112>:   callq  *%rax                      # call __tp_probe->func
   0x0000000000400de2 <+114>:   mov    (%rbx),%rax                # %rax = __tp->probe->func
   0x0000000000400de5 <+117>:   test   %rax,%rax                  # if (!%rax)
   0x0000000000400de8 <+120>:   jne    0x400dd8 <do_stuff+104>    # goto loop
end:
   0x0000000000400dea <+122>:   pop    %rbx
   0x0000000000400deb <+123>:   mov    0x201b2e(%rip),%rax
   0x0000000000400df2 <+130>:   jmpq   *%rax                      # call tp_rcu_read_unlock_bp()
End of assembler dump.
```

### Debug ###
~~~~~~~~~
DYNINST_DEBUG_MALWARE: Enabling DyninstAPI malware debug
DYNINST_DEBUG_TRAP: Enabling DyninstAPI debugging using traps
DYNINST_DEBUG_SPRINGBOARD: Enabling DyninstAPI springboard debug
DYNINST_DEBUG_STARTUP: Enabling DyninstAPI startup debug
DYNINST_DEBUG_PARSING: Enabling DyninstAPI parsing debug
DYNINST_DEBUG_PARSE: Enabling DyninstAPI parsing debug
DYNINST_DEBUG_DYNPC, DYNINST_DEBUG_FORKEXEC, DYNINST_DEBUG_INFRPC, DYNINST_DEBUG_SIGNAL, DYNINST_DEBUG_INFERIORRPC, DYNINST_DEBUG_THREAD, DYNINST_DEBUG_MAILBOX, DYNINST_DEBUG_DBI : Enabling DyninstAPI process control debug
DYNINST_DEBUG_STACKWALK: Enabling DyninstAPI stack walking debug
DYNINST_DEBUG_INST: Enabling DyninstAPI inst debug
DYNINST_DEBUG_RELOC: Enabling DyninstAPI relocation debug
DYNINST_DEBUG_RELOCATION: Enabling DyninstAPI relocation debug
DYNINST_DEBUG_SENSITIVITY: Enabling DyninstAPI sensitivity debug
DYNINST_DEBUG_DYN_UNW: Enabling DyninstAPI dynamic unwind debug
DYNINST_DEBUG_MUTEX: Enabling DyninstAPI mutex debug
DYNINST_DEBUG_RTLIB: Enabling DyninstAPI RTlib debug
DYNINST_DEBUG_CATCHUP: Enabling DyninstAPI catchup debug
DYNINST_DEBUG_BPATCH: Enabling DyninstAPI bpatch debug
DYNINST_DEBUG_REGALLOC: Enabling DyninstAPI register allocation debug
DYNINST_DEBUG_AST: Enabling DyninstAPI ast debug
DYNINST_DEBUG_WRITE: Enabling DyninstAPI process write debugging
DYNINST_DEBUG_INFMALLOC, DYNINST_DEBUG_INFERIORMALLOC: Enabling DyninstAPI inferior malloc debugging
DYNINST_DEBUG_CRASH: Enable DyninstAPI crash debugging
DYNINST_DEBUG_DISASS: Enabling DyninstAPI instrumentation disassembly debugging
~~~~~~~~~
Reference: http://git.dyninst.org/?p=dyninst.git;a=blob;f=dyninstAPI/src/debug.C

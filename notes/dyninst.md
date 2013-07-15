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
   0x0000000000400d70 <+0>:     push   %rbx                       # same as above
   0x0000000000400d71 <+1>:     mov    $0x401844,%esi             #      |
   0x0000000000400d76 <+6>:     mov    $0x401846,%edi             #      |
   0x0000000000400d7b <+11>:    callq  0x400ae0 <fopen@plt>       #      |
   0x0000000000400d80 <+16>:    mov    $0x1,%edx                  #      |
   0x0000000000400d85 <+21>:    mov    $0x401850,%esi             #      |
   0x0000000000400d8a <+26>:    mov    %rax,%rdi                  #      |
   0x0000000000400d8d <+29>:    mov    %rax,%rbx                  #      |
   0x0000000000400d90 <+32>:    xor    %eax,%eax                  #      |
   0x0000000000400d92 <+34>:    callq  0x400a60 <fprintf@plt>     #      |
   0x0000000000400d97 <+39>:    mov    %rbx,%rdi                  #      |
   0x0000000000400d9a <+42>:    callq  0x400a10 <fclose@plt>      #      |
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

 * Asm code with dynamic tracepoint using dyninst (default options)

```
Dump of assembler code for function do_stuff:
   0x0000000000400a30 <+0>:     jmpq   0x10000                   # jmp to 0x10000
   0x0000000000400a35 <+5>:     add    %bh,0x400b36(%rdi)        # looks like the following are all broken
   0x0000000000400a3b <+11>:    callq  0x4007b0 <fopen@plt>
   0x0000000000400a40 <+16>:    jmpq   0x10010
   0x0000000000400a45 <+21>:    mov    %rax,%rbx
   0x0000000000400a48 <+24>:    mov    %rax,%rdi
   0x0000000000400a4b <+27>:    mov    $0x400b40,%esi
   0x0000000000400a50 <+32>:    xor    %eax,%eax
   0x0000000000400a52 <+34>:    callq  0x400760 <fprintf@plt>
   0x0000000000400a57 <+39>:    jmpq   0x10027                   # What is this?
   0x0000000000400a5c <+44>:    cld
   0x0000000000400a5d <+45>:    (bad)
   0x0000000000400a5e <+46>:    ljmpq  *<internal disassembler error>
   0x0000000000400a60 <+48>:    lret
   0x0000000000400a61 <+49>:    cmc
   0x0000000000400a62 <+50>:    sar    $0xfd,%bh
   0x0000000000400a65 <+53>:    (bad)
   0x0000000000400a66 <+54>:    jmpq   *0xf(%rsi)
End of assembler dump.

(gdb) info proc map
Mapped address spaces:
          Start Addr           End Addr       Size     Offset objfile
             0x10000           0x110000   0x100000        0x0 /dev/zero     # interesting

(gdb) x/100i 0x10000
   0x10000: push   %rbx                            # copied from do_stuff
   0x10001: mov    $0x400b34,%esi
   0x10006: mov    $0x400b36,%edi
   0x1000b: callq  0x4007b0 <fopen@plt>
   0x10010: mov    $0x1,%edx
   0x10015: mov    %rax,%rbx
   0x10018: mov    %rax,%rdi
   0x1001b: mov    $0x400b40,%esi
   0x10020: xor    %eax,%eax
   0x10022: callq  0x400760 <fprintf@plt>
   0x10027: mov    %rbx,%rdi
   0x1002a: callq  0x400730 <fclose@plt>
   0x1002f: pop    %rbx
   0x10030: xor    %edi,%edi
   0x10032: lea    -0xa8(%rsp),%rsp                # trampoline starts here, move %rsp to skip red zone: http://www.agner.org/optimize/calling_conventions.pdf
   0x1003a: mov    %rax,0x20(%rsp)                 # save %rax
   0x1003f: lea    0xa8(%rsp),%rax                 # %rax now stores original %rsp
   0x10047: and    $0xffffffffffffffe0,%rsp        # align stack
   0x1004b: mov    %rax,(%rsp)                     # save original %rsp
   0x1004f: mov    -0x88(%rax),%rax                # restore %rax
   0x10056: push   %rax                            # save registers
   0x10057: push   %rbx
   0x10058: push   %r8
   0x1005a: push   %r9
   0x1005c: push   %rcx
   0x1005d: push   %rdx
   0x1005e: push   %rsi
   0x1005f: push   %rdi
   0x10060: push   %rax
   0x10061: push   %r12
   0x10063: push   %r13
   0x10065: push   %r14
   0x10067: push   %r15
   0x10069: lea    -0x18(%rsp),%rsp                # align stack by 16 bytes: http://www.agner.org/optimize/calling_conventions.pdf
   0x1006e: movabs $0x0,%rax                       # I suspect this is related to the number of vector registers
   0x10078: movabs $0x7f2ab42d73b0,%rbx            # %rbx = DYNINSTthreadIndex
   0x10082: callq  *%rbx                           # call DYNINSTthreadIndex, calculate thread index
   0x10084: lea    0x18(%rsp),%rsp                 # align stack
   0x10089: mov    %rax,%rbx                       # %rbx = threadIdx
   0x1008c: pop    %r15                            # restore registers
   0x1008e: pop    %r14
   0x10090: pop    %r13
   0x10092: pop    %r12
   0x10094: pop    %rax
   0x10095: mov    %rbx,%r10                       # %r10 = threadIdx
   0x10098: shl    $0x2,%r10                       # offset = threadIdx * sizeof(unsigned)
   0x1009c: movabs $0x7f2ab44ec900,%rbx            # %rbx = DYNINST_default_tramp_guards, this array is for checking recursive call
   0x100a6: mov    %r10,%r11                       # %r11 = offset
   0x100a9: add    %rbx,%r11                       # %r11 += &DYNINST_default_tramp_guards[threadIdx]
   0x100ac: mov    (%r11),%eax                     # if (DYNINST_default_tramp_guards[threadIdx] == 0)
   0x100af: test   %rax,%rax                       #     // if found a recursive call
   0x100b2: je     0x10106                         # goto end
   0x100b8: movabs $0x0,%rax                       # %rax = 0
   0x100c2: mov    %eax,(%r11)                     # DYNINST_default_tramp_guards[threadIdx] = 0
   0x100c5: push   %r11                            # save registers
   0x100c7: push   %r12
   0x100c9: push   %r13
   0x100cb: push   %r14
   0x100cd: push   %r15
   0x100cf: lea    -0x18(%rsp),%rsp                # align stack
   0x100d4: movabs $0x0,%rax                       # number of vector registers
   0x100de: movabs $0x7f2ab36fbac7,%rbx            # %rbx = tpbench_no_arg
   0x100e8: callq  *%rbx                           # call tpbench_no_arg in dyntp.so
   0x100ea: lea    0x18(%rsp),%rsp                 # align stack
   0x100ef: pop    %r15                            # restore registers
   0x100f1: pop    %r14
   0x100f3: pop    %r13
   0x100f5: pop    %r12
   0x100f7: pop    %r11
   0x100f9: movabs $0x1,%rax                      # %rax = 1
   0x10103: mov    %eax,(%r11)                    # DYNINST_default_tramp_guards[threadIdx] = 1
end:
   0x10106: pop    %rdi                           # restore registers
   0x10107: pop    %rsi
   0x10108: pop    %rdx
   0x10109: pop    %rcx
   0x1010a: pop    %r9
   0x1010c: pop    %r8
   0x1010e: pop    %rbx
   0x1010f: pop    %rax
   0x10110: mov    (%rsp),%rsp                    # restore original %rsp
   0x10114: jmpq   0x400790 <time@plt>
```

 * Asm code with dynamic tracepoint using dyninst (saveFPR=false)
     * The generated code is identical to the above one.
     * Dyninst will analyse if saving floating point registers is required.

 * Asm code with dynamic tracepoint using dyninst (setTrampRecursive=true)

```
Dump of assembler code for function do_stuff:                        # no change
   0x0000000000400a30 <+0>:     jmpq   0x10000
   0x0000000000400a35 <+5>:     add    %bh,0x400b36(%rdi)
   0x0000000000400a3b <+11>:    callq  0x4007b0 <fopen@plt>
   0x0000000000400a40 <+16>:    jmpq   0x10010
   0x0000000000400a45 <+21>:    mov    %rax,%rbx
   0x0000000000400a48 <+24>:    mov    %rax,%rdi
   0x0000000000400a4b <+27>:    mov    $0x400b40,%esi
   0x0000000000400a50 <+32>:    xor    %eax,%eax
   0x0000000000400a52 <+34>:    callq  0x400760 <fprintf@plt>
   0x0000000000400a57 <+39>:    jmpq   0x10027
   0x0000000000400a5c <+44>:    cld
   0x0000000000400a5d <+45>:    (bad)
   0x0000000000400a5e <+46>:    ljmpq  *<internal disassembler error>
   0x0000000000400a60 <+48>:    lret
   0x0000000000400a61 <+49>:    cmc
   0x0000000000400a62 <+50>:    sar    $0xfd,%bh
   0x0000000000400a65 <+53>:    (bad)
   0x0000000000400a66 <+54>:    jmpq   *0xf(%rsi)
End of assembler dump.

(gdb) x/100i 0x10000
   0x10000: push   %rbx                            # copied from do_stuff
   0x10001: mov    $0x400b34,%esi
   0x10006: mov    $0x400b36,%edi
   0x1000b: callq  0x4007b0 <fopen@plt>
   0x10010: mov    $0x1,%edx
   0x10015: mov    %rax,%rbx
   0x10018: mov    %rax,%rdi
   0x1001b: mov    $0x400b40,%esi
   0x10020: xor    %eax,%eax
   0x10022: callq  0x400760 <fprintf@plt>
   0x10027: mov    %rbx,%rdi
   0x1002a: callq  0x400730 <fclose@plt>
   0x1002f: pop    %rbx
   0x10030: xor    %edi,%edi
   0x10032: lea    -0xa8(%rsp),%rsp               # save %rsp, same as above
   0x1003a: mov    %rax,0x20(%rsp)
   0x1003f: lea    0xa8(%rsp),%rax
   0x10047: and    $0xffffffffffffffe0,%rsp
   0x1004b: mov    %rax,(%rsp)
   0x1004f: mov    -0x88(%rax),%rax
   0x10056: push   %rax                           # save registers
   0x10057: push   %rbx
   0x10058: push   %r8
   0x1005a: push   %r9
   0x1005c: push   %rcx
   0x1005d: push   %rdx
   0x1005e: push   %rsi
   0x1005f: push   %rdi
   0x10060: push   %r12
   0x10062: push   %r13
   0x10064: push   %r14
   0x10066: push   %r15
   0x10068: lea    -0x20(%rsp),%rsp               # align stack, looks like a bug of dyninst
   0x1006d: movabs $0x0,%rax
   0x10077: movabs $0x7f8d90ac8ac7,%rbx
   0x10081: callq  *%rbx
   0x10083: lea    0x20(%rsp),%rsp                # restore stack frame
   0x10088: pop    %r15                           # restore registers
   0x1008a: pop    %r14
   0x1008c: pop    %r13
   0x1008e: pop    %r12
   0x10090: pop    %rdi
   0x10091: pop    %rsi
   0x10092: pop    %rdx
   0x10093: pop    %rcx
   0x10094: pop    %r9
   0x10096: pop    %r8
   0x10098: pop    %rbx
   0x10099: pop    %rax
   0x1009a: mov    (%rsp),%rsp                    # restore %rsp
   0x1009e: jmpq   0x400790 <time@plt>
```

 * Asm code with dynamic tracepoint at &do_stuff+6 (defualt options)

```
Dump of assembler code for function do_stuff:
   0x0000000000400a30 <+0>:     jmpq   0x10000                    # nothing changes
   0x0000000000400a35 <+5>:     add    %bh,0x400b36(%rdi)
   0x0000000000400a3b <+11>:    callq  0x4007b0 <fopen@plt>
   0x0000000000400a40 <+16>:    jmpq   0x10113
   0x0000000000400a45 <+21>:    mov    %rax,%rbx
   0x0000000000400a48 <+24>:    mov    %rax,%rdi
   0x0000000000400a4b <+27>:    mov    $0x400b40,%esi
   0x0000000000400a50 <+32>:    xor    %eax,%eax
   0x0000000000400a52 <+34>:    callq  0x400760 <fprintf@plt>
   0x0000000000400a57 <+39>:    jmpq   0x1012a
   0x0000000000400a5c <+44>:    cld
   0x0000000000400a5d <+45>:    (bad)
   0x0000000000400a5e <+46>:    ljmpq  *<internal disassembler error>
   0x0000000000400a60 <+48>:    (bad)
   0x0000000000400a61 <+49>:    test   $0xff,%al
   0x0000000000400a64 <+52>:    std
   0x0000000000400a65 <+53>:    (bad)
   0x0000000000400a66 <+54>:    jmpq   *0xf(%rsi)
End of assembler dump.

(gdb) x/100i 0x10000
   0x10000: push   %rbx
   0x10001: mov    $0x400b34,%esi
   0x10006: lea    -0xa8(%rsp),%rsp             # starts here, do_stuff+6
   0x1000e: mov    %rax,0x20(%rsp)
   0x10013: lea    0xa8(%rsp),%rax
   0x1001b: and    $0xffffffffffffffe0,%rsp
   0x1001f: mov    %rax,(%rsp)
   0x10023: mov    -0x88(%rax),%rax
   0x1002a: push   %rax
   0x1002b: push   %r8
   0x1002d: push   %r9
   0x1002f: push   %rcx
   0x10030: push   %rdx
   0x10031: push   %rsi
   0x10032: movabs $0x400a36,%rax              # WTF?
   0x1003c: push   %rax
   0x1003d: push   %rbp
   0x1003e: mov    %rsp,%rbp
   0x10041: lea    -0x200(%rsp),%rsp           # stack space for FPRs
   0x10049: fxsave (%rsp)                      # save FPRs
   0x1004d: push   %rax
   0x1004e: push   %rsp
   0x1004f: push   %r12
   0x10051: push   %r13
   0x10053: push   %r14
   0x10055: push   %r15
   0x10057: lea    -0x10(%rsp),%rsp
   0x1005c: movabs $0x0,%rax
   0x10066: movabs $0x7f2946bc73b0,%rbx
   0x10070: callq  *%rbx
   0x10072: lea    0x10(%rsp),%rsp
   0x10077: mov    %rax,%rbx
   0x1007a: pop    %r15
   0x1007c: pop    %r14
   0x1007e: pop    %r13
   0x10080: pop    %r12
   0x10082: pop    %rsp
   0x10083: pop    %rax
   0x10084: mov    %rbx,%r10
   0x10087: shl    $0x2,%r10
   0x1008b: movabs $0x7f2946ddc900,%rbx
   0x10095: mov    %r10,%r11
   0x10098: add    %rbx,%r11
   0x1009b: mov    (%r11),%eax
   0x1009e: test   %rax,%rax
   0x100a1: je     0x100f7
   0x100a7: movabs $0x0,%rax
   0x100b1: mov    %eax,(%r11)
   0x100b4: push   %rsp
   0x100b5: push   %r11
   0x100b7: push   %r12
   0x100b9: push   %r13
   0x100bb: push   %r14
   0x100bd: push   %r15
   0x100bf: lea    -0x10(%rsp),%rsp
   0x100c4: movabs $0x0,%rax
   0x100ce: movabs $0x7f2945febac7,%rbx
   0x100d8: callq  *%rbx
   0x100da: lea    0x10(%rsp),%rsp
   0x100df: pop    %r15
   0x100e1: pop    %r14
   0x100e3: pop    %r13
   0x100e5: pop    %r12
   0x100e7: pop    %r11
   0x100e9: pop    %rsp
   0x100ea: movabs $0x1,%rax
   0x100f4: mov    %eax,(%r11)
   0x100f7: fxrstor (%rsp)
   0x100fb: leaveq 
   0x100fc: pop    %rax
   0x100fd: pop    %rsi
   0x100fe: pop    %rdx
   0x100ff: pop    %rcx
   0x10100: pop    %r9
   0x10102: pop    %r8
   0x10104: pop    %rax
   0x10105: mov    (%rsp),%rsp
   0x10109: mov    $0x400b36,%edi
   0x1010e: callq  0x4007b0 <fopen@plt>
   0x10113: mov    $0x1,%edx
   0x10118: mov    %rax,%rbx
   0x1011b: mov    %rax,%rdi
   0x1011e: mov    $0x400b40,%esi
   0x10123: xor    %eax,%eax
   0x10125: callq  0x400760 <fprintf@plt>
   0x1012a: mov    %rbx,%rdi
   0x1012d: callq  0x400730 <fclose@plt>
   0x10132: pop    %rbx
   0x10133: xor    %edi,%edi
   0x10135: jmpq   0x400790 <time@plt>
```

```
(gdb) disas do_stuff 
Dump of assembler code for function do_stuff:
   0x0000000000400a30 <+0>:     jmpq   0x10000                   # noting changes
   0x0000000000400a35 <+5>:     add    %bh,0x400b36(%rdi)
   0x0000000000400a3b <+11>:    callq  0x4007b0 <fopen@plt>
   0x0000000000400a40 <+16>:    jmpq   0x100fc
   0x0000000000400a45 <+21>:    mov    %rax,%rbx
   0x0000000000400a48 <+24>:    mov    %rax,%rdi
   0x0000000000400a4b <+27>:    mov    $0x400b40,%esi
   0x0000000000400a50 <+32>:    xor    %eax,%eax
   0x0000000000400a52 <+34>:    callq  0x400760 <fprintf@plt>
   0x0000000000400a57 <+39>:    jmpq   0x10113
   0x0000000000400a5c <+44>:    cld
   0x0000000000400a5d <+45>:    (bad)
   0x0000000000400a5e <+46>:    ljmpq  *<internal disassembler error>
   0x0000000000400a60 <+48>:    mov    $0xf6,%bh
   0x0000000000400a62 <+50>:    sar    $0xfd,%bh
   0x0000000000400a65 <+53>:    (bad)
   0x0000000000400a66 <+54>:    jmpq   *0xf(%rsi)
End of assembler dump.

(gdb) x/100i 0x10000
   0x10000: push   %rbx
   0x10001: mov    $0x400b34,%esi
   0x10006: lea    -0xa8(%rsp),%rsp
   0x1000e: mov    %rax,0x20(%rsp)
   0x10013: lea    0xa8(%rsp),%rax
   0x1001b: and    $0xffffffffffffffe0,%rsp
   0x1001f: mov    %rax,(%rsp)
   0x10023: mov    -0x88(%rax),%rax
   0x1002a: push   %rax
   0x1002b: push   %r8
   0x1002d: push   %r9
   0x1002f: push   %rcx
   0x10030: push   %rdx
   0x10031: push   %rsi
   0x10032: lea    -0x10(%rsp),%rsp
   0x10037: push   %rax
   0x10038: push   %rsp
   0x10039: push   %r12
   0x1003b: push   %r13
   0x1003d: push   %r14
   0x1003f: push   %r15
   0x10041: lea    -0x10(%rsp),%rsp
   0x10046: movabs $0x0,%rax
   0x10050: movabs $0x7f9de59a73b0,%rbx
   0x1005a: callq  *%rbx
   0x1005c: lea    0x10(%rsp),%rsp
   0x10061: mov    %rax,%rbx
   0x10064: pop    %r15
   0x10066: pop    %r14
   0x10068: pop    %r13
   0x1006a: pop    %r12
   0x1006c: pop    %rsp
   0x1006d: pop    %rax
   0x1006e: mov    %rbx,%r10
   0x10071: shl    $0x2,%r10
   0x10075: movabs $0x7f9de5bbc900,%rbx
   0x1007f: mov    %r10,%r11
   0x10082: add    %rbx,%r11
   0x10085: mov    (%r11),%eax
   0x10088: test   %rax,%rax
   0x1008b: je     0x100e1
   0x10091: movabs $0x0,%rax
   0x1009b: mov    %eax,(%r11)
   0x1009e: push   %rsp
   0x1009f: push   %r11
   0x100a1: push   %r12
   0x100a3: push   %r13
   0x100a5: push   %r14
   0x100a7: push   %r15
   0x100a9: lea    -0x10(%rsp),%rsp
   0x100ae: movabs $0x0,%rax
   0x100b8: movabs $0x7f9de4dcbac7,%rbx
   0x100c2: callq  *%rbx
   0x100c4: lea    0x10(%rsp),%rsp
   0x100c9: pop    %r15
   0x100cb: pop    %r14
   0x100cd: pop    %r13
   0x100cf: pop    %r12
   0x100d1: pop    %r11
   0x100d3: pop    %rsp
   0x100d4: movabs $0x1,%rax
   0x100de: mov    %eax,(%r11)
   0x100e1: lea    0x10(%rsp),%rsp
   0x100e6: pop    %rsi
   0x100e7: pop    %rdx
   0x100e8: pop    %rcx
   0x100e9: pop    %r9
   0x100eb: pop    %r8
   0x100ed: pop    %rax
   0x100ee: mov    (%rsp),%rsp
   0x100f2: mov    $0x400b36,%edi
   0x100f7: callq  0x4007b0 <fopen@plt>
   0x100fc: mov    $0x1,%edx
   0x10101: mov    %rax,%rbx
   0x10104: mov    %rax,%rdi
   0x10107: mov    $0x400b40,%esi
   0x1010c: xor    %eax,%eax
   0x1010e: callq  0x400760 <fprintf@plt>
   0x10113: mov    %rbx,%rdi
   0x10116: callq  0x400730 <fclose@plt>
   0x1011b: pop    %rbx
   0x1011c: xor    %edi,%edi
   0x1011e: jmpq   0x400790 <time@plt>
```

```
(gdb) disas do_stuff
Dump of assembler code for function do_stuff:
   0x0000000000400a30 <+0>:     jmpq   0x10000
   0x0000000000400a35 <+5>:     add    %bh,0x400b36(%rdi)
   0x0000000000400a3b <+11>:    callq  0x4007b0 <fopen@plt>
   0x0000000000400a40 <+16>:    jmpq   0x1009b
   0x0000000000400a45 <+21>:    mov    %rax,%rbx
   0x0000000000400a48 <+24>:    mov    %rax,%rdi
   0x0000000000400a4b <+27>:    mov    $0x400b40,%esi
   0x0000000000400a50 <+32>:    xor    %eax,%eax
   0x0000000000400a52 <+34>:    callq  0x400760 <fprintf@plt>
   0x0000000000400a57 <+39>:    jmpq   0x100b2
   0x0000000000400a5c <+44>:    cld
   0x0000000000400a5d <+45>:    (bad)
   0x0000000000400a5e <+46>:    ljmpq  *<internal disassembler error>
   0x0000000000400a60 <+48>:    push   %rsi
   0x0000000000400a61 <+49>:    test   $0xff,%al
   0x0000000000400a64 <+52>:    std
   0x0000000000400a65 <+53>:    (bad)
   0x0000000000400a66 <+54>:    jmpq   *0xf(%rsi)
End of assembler dump.

(gdb) x/100i 0x10000
   0x10000: push   %rbx
   0x10001: mov    $0x400b34,%esi
   0x10006: lea    -0xa8(%rsp),%rsp
   0x1000e: mov    %rax,0x20(%rsp)
   0x10013: lea    0xa8(%rsp),%rax
   0x1001b: and    $0xffffffffffffffe0,%rsp
   0x1001f: mov    %rax,(%rsp)
   0x10023: mov    -0x88(%rax),%rax
   0x1002a: push   %rax
   0x1002b: push   %r8
   0x1002d: push   %r9
   0x1002f: push   %rcx
   0x10030: push   %rdx
   0x10031: push   %rsi
   0x10032: movabs $0x400a36,%rax
   0x1003c: push   %rax
   0x1003d: push   %rbp
   0x1003e: mov    %rsp,%rbp
   0x10041: lea    -0x200(%rsp),%rsp
   0x10049: fxsave (%rsp)
   0x1004d: push   %rsp
   0x1004e: push   %r12
   0x10050: push   %r13
   0x10052: push   %r14
   0x10054: push   %r15
   0x10056: lea    -0x18(%rsp),%rsp
   0x1005b: movabs $0x0,%rax
   0x10065: movabs $0x7fb31fdccac7,%rbx
   0x1006f: callq  *%rbx
   0x10071: lea    0x18(%rsp),%rsp
   0x10076: pop    %r15
   0x10078: pop    %r14
   0x1007a: pop    %r13
   0x1007c: pop    %r12
   0x1007e: pop    %rsp
   0x1007f: fxrstor (%rsp)
   0x10083: leaveq 
   0x10084: pop    %rax
   0x10085: pop    %rsi
   0x10086: pop    %rdx
   0x10087: pop    %rcx
   0x10088: pop    %r9
   0x1008a: pop    %r8
   0x1008c: pop    %rax
   0x1008d: mov    (%rsp),%rsp
   0x10091: mov    $0x400b36,%edi
   0x10096: callq  0x4007b0 <fopen@plt>
   0x1009b: mov    $0x1,%edx
   0x100a0: mov    %rax,%rbx
   0x100a3: mov    %rax,%rdi
   0x100a6: mov    $0x400b40,%esi
   0x100ab: xor    %eax,%eax
   0x100ad: callq  0x400760 <fprintf@plt>
   0x100b2: mov    %rbx,%rdi
   0x100b5: callq  0x400730 <fclose@plt>
   0x100ba: pop    %rbx
   0x100bb: xor    %edi,%edi
   0x100bd: jmpq   0x400790 <time@plt>
```

```
(gdb) disas do_stuff 
Dump of assembler code for function do_stuff:
   0x0000000000400a30 <+0>:     jmpq   0x10000                   # nothing changes
   0x0000000000400a35 <+5>:     add    %bh,0x400b36(%rdi)
   0x0000000000400a3b <+11>:    callq  0x4007b0 <fopen@plt>
   0x0000000000400a40 <+16>:    jmpq   0x10084
   0x0000000000400a45 <+21>:    mov    %rax,%rbx
   0x0000000000400a48 <+24>:    mov    %rax,%rdi
   0x0000000000400a4b <+27>:    mov    $0x400b40,%esi
   0x0000000000400a50 <+32>:    xor    %eax,%eax
   0x0000000000400a52 <+34>:    callq  0x400760 <fprintf@plt>
   0x0000000000400a57 <+39>:    jmpq   0x1009b
   0x0000000000400a5c <+44>:    cld
   0x0000000000400a5d <+45>:    (bad)
   0x0000000000400a5e <+46>:    ljmpq  *<internal disassembler error>
   0x0000000000400a60 <+48>:    (bad)
   0x0000000000400a61 <+49>:    test   $0xff,%al
   0x0000000000400a64 <+52>:    std
   0x0000000000400a65 <+53>:    (bad)
   0x0000000000400a66 <+54>:    jmpq   *0xf(%rsi)
End of assembler dump.

(gdb) x/100i 0x10000
   0x10000: push   %rbx
   0x10001: mov    $0x400b34,%esi
   0x10006: lea    -0xa8(%rsp),%rsp              # starts here
   0x1000e: mov    %rax,0x20(%rsp)
   0x10013: lea    0xa8(%rsp),%rax
   0x1001b: and    $0xffffffffffffffe0,%rsp
   0x1001f: mov    %rax,(%rsp)
   0x10023: mov    -0x88(%rax),%rax
   0x1002a: push   %rax
   0x1002b: push   %r8
   0x1002d: push   %r9
   0x1002f: push   %rcx
   0x10030: push   %rdx
   0x10031: push   %rsi
   0x10032: lea    -0x10(%rsp),%rsp             # align stack
   0x10037: push   %rsp
   0x10038: push   %r12
   0x1003a: push   %r13
   0x1003c: push   %r14
   0x1003e: push   %r15
   0x10040: lea    -0x18(%rsp),%rsp             # align stack
   0x10045: movabs $0x0,%rax
   0x1004f: movabs $0x7f682f61eac7,%rbx         # call tp_no_arg
   0x10059: callq  *%rbx
   0x1005b: lea    0x18(%rsp),%rsp
   0x10060: pop    %r15
   0x10062: pop    %r14
   0x10064: pop    %r13
   0x10066: pop    %r12
   0x10068: pop    %rsp
   0x10069: lea    0x10(%rsp),%rsp
   0x1006e: pop    %rsi
   0x1006f: pop    %rdx
   0x10070: pop    %rcx
   0x10071: pop    %r9
   0x10073: pop    %r8
   0x10075: pop    %rax
   0x10076: mov    (%rsp),%rsp                # we are done
   0x1007a: mov    $0x400b36,%edi
   0x1007f: callq  0x4007b0 <fopen@plt>
   0x10084: mov    $0x1,%edx
   0x10089: mov    %rax,%rbx
   0x1008c: mov    %rax,%rdi
   0x1008f: mov    $0x400b40,%esi
   0x10094: xor    %eax,%eax
   0x10096: callq  0x400760 <fprintf@plt>
   0x1009b: mov    %rbx,%rdi
   0x1009e: callq  0x400730 <fclose@plt>
   0x100a3: pop    %rbx
   0x100a4: xor    %edi,%edi
   0x100a6: jmpq   0x400790 <time@plt>
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

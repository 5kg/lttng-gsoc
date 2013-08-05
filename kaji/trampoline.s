.extern kaji_probe
.section .text
.align 4096
.globl kaji_trampoline
.globl __kaji_trampoline_placeholder
.globl __kaji_trampoline_end
.globl __kaji_trampoline_call
kaji_trampoline:
   # We probably need to skip stack red-zone, ignore here
   # Save registers
   push   %rax
   push   %r8
   push   %r9
   push   %rcx
   push   %rdx
   push   %rsi
   push   %rsp
   push   %r12
   push   %r13
   push   %r14
   push   %r15
   # Call probe, we use a indirect call here
__kaji_trampoline_call:
   movabs $0xffffffffffffffff, %rax
   call   *%rax
   # Restore registers
   pop    %r15
   pop    %r14
   pop    %r13
   pop    %r12
   pop    %rsp
   pop    %rsi
   pop    %rdx
   pop    %rcx
   pop    %r9
   pop    %r8
   pop    %rax
__kaji_trampoline_placeholder:
  .ascii  "Hello, I am a placeholder."
__kaji_trampoline_end:

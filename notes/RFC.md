RFC - Dynamic instrumentation support in UST
============================================

Author: Zifei Tong <soariez@gmail.com>

- Version:
    - v0.1: 07/25/2013
        - Initial proposal

Motivation
----------

TBD

Command Line Interface
----------------------

The proposed command line interface is very similar to the current lttng kernel
dynamic probe interface [1].

To enable a dynamic probe in a running process at given address, you can use:

    lttng enable-event NAME -u --pid PID
        --probe (addr | symbol | symbol+offset)
                               Dynamic UST probe.
                               Addr and offset can be octal (0NNN...),
                               decimal (NNN...) or hexadecimal (0xNNN...)

This will place a bare probe at certain address.

Examples:

    # lttng enable-event aname -u --pid 8964 --probe main+0x10
    # lttng enable-event aname -u --pid 8964 --probe 0xffff7260695

You can also enable a tracepoint at the entry of a given function:

    lttng enable-event NAME -u --pid PID
        --function (addr | symbol | symbol+offset)
                             Dynamic UST function entry probe.
                             Addr and offset can be octal (0NNN...),
                             decimal (NNN...) or hexadecimal (0xNNN...)

This will place a bare probe at the entry point of certain function.

Examples:

    # lttng enable-event aname -u --pid 8964 --function foo

Being only able to attach and instrumente existing processes is sometimes
restricted. If we can have #15 [2] implemented, then we can extend the command
to support dynamic instrumentation.

    lttng trace -c COMMAND
        --probe (addr | symbol | symbol+offset)
        --function (addr | symbol | symbol+offset)

This will execute given program and place probes at certain places.

Examples:

    # lttng trace -c "ls -l" --probe main+0x10
    # lttng trace -c "ls -l" --function main

Add-context

    # lttng add-context -u --pid PID -e aname -t reg:rax
    # lttng add-context -u --pid PID -e aname -t int:varname:0xffff7260695
    # lttng add-context -u --pid PID -e aname -t string:varname:0xffff7260695
    # lttng add-context -u --pid PID -e aname -t backtrace

Public API
----------

TBD.

I need to be more familar with lttng codebase to give a API proposal.

Implementation
--------------

I will give a simple example here.

Suppose function do_stuff has the following assembly code:

    0x0000000000400a30 <+0>:     push   %rbx
    0x0000000000400a31 <+1>:     mov    $0x400b34,%esi
    0x0000000000400a36 <+6>:     mov    $0x400b36,%edi
    0x0000000000400a3b <+11>:    callq  0x4007b0 <fopen@plt>
    0x0000000000400a62 <+16>:    ret

After

    # lttng enable-event aname -u --pid 8964 --probe do_stuff+0x6

function do_stuff will become:

    0x0000000000400a30 <+0>:     push   %rbx
    0x0000000000400a31 <+1>:     mov    $0x400b34,%esi
    0x0000000000400a36 <+6>:     jmpq   0x1002a               # jmp to trampoline
    0x0000000000400a3b <+11>:    callq  0x4007b0 <fopen@plt>
    0x0000000000400a62 <+16>:    ret

    (gdb) x/100i 0x1002a
    0x1002a: push   %rax                       # save registers
    ....................
    0x1003e: push   %r15
    0x1004f: movabs $0x7f682f61eac7,%rbx
    0x10059: callq  *%rbx                      # call lttng probe
    0x10060: pop    %r15                       # restore registers
    ....................
    0x10075: pop    %rax
    0x1007a: jmpq   0x0000000000400a3b         # jmp back


This is a oversimplified example. In reality

[1]: http://bugs.lttng.org/projects/lttng-tools/wiki
[2]: http://bugs.lttng.org/issues/15
[3]: https://github.com/5kg/lttng-gsoc/blob/master/notes/dyninst.md
[4]: http://sourceware.org/systemtap/SystemTap_Beginners_Guide/utargetvariable.html
[5]: http://sourceware.org/systemtap/SystemTap_Beginners_Guide/ustack.html

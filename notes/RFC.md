RFC - Dynamic instrumentation support in UST
============================================

Author: Zifei Tong <soariez@gmail.com>

- Version:
    - v0.1: 07/25/2013
        - Initial proposal
    - v0.2: 08/08/2013
        - Update

Command Line Interface
----------------------

The proposed command line interface is very similar to the current lttng kernel
dynamic uprobes interface [1], [9], [10].

To enable a dynamic probe in a running process at given address, you can use:

    lttng enable-event NAME -u --path PATH
        --probe (addr | symbol | symbol+offset)
                               Dynamic UST probe.
                               Addr and offset can be octal (0NNN...),
                               decimal (NNN...) or hexadecimal (0xNNN...)

This will place a bare probe at certain address.

Examples:

    # lttng enable-event aname -u --path PATH --probe foo+0x6
    # lttng enable-event aname -u --path PATH --probe 0xdeadbeef

You can also enable a tracepoint at the entry/return of a given function:

    lttng enable-event NAME -u --path PATH
        --function (addr | symbol | symbol+offset)
                             Dynamic UST function entry/return probe.
                             Addr and offset can be octal (0NNN...),
                             decimal (NNN...) or hexadecimal (0xNNN...)

This will place a bare probe at the entry and return point of certain function.

Examples:

    # lttng enable-event aname -u --path PATH --function foo

Being only able to attach and instrument existing processes is sometimes
restricted. If we can have #15 [2] implemented, then we can extend the command
to support dynamic instrumentation.

    lttng trace -c COMMAND
        --probe (addr | symbol | symbol+offset)
        --function (addr | symbol | symbol+offset)

This will execute given program and place probes at certain places.

Examples:

    # lttng trace -c "ls -l" --probe main+0x10
    # lttng trace -c "ls -l" --function main

Probes collecting context data are more useful than bare ones. We can extend
the add-context command to support more context types.

    lttng add-context -u -e NAME --path PATH
        -t, --type TYPE     Context type.
                            TYPE can be one of the strings below:
                                regs
                                    all registers
                                reg:reg_name
                                    register with name reg_name
                                var_type:var_name:addr
                                    variable at addr with field name var_name
                                backtrace
                                    stack backtrace

Examples:

    # lttng add-context -u --path PATH -e aname -t reg:rax
    # lttng add-context -u --path PATH -e aname -t int:varname:0xdeadbeef
    # lttng add-context -u --path PATH -e aname -t string:varname:0xdeadbeef
    # lttng add-context -u --path PATH -e aname -t backtrace

These context types appear in other tracers [4],[5],[6], however implement all of
them may be not possible within the GSoC schedule.

The above proposed command line interface require defining tracepoint event at
runtime thus free user form compiling tracepoint probes themselves. But I have
a concern such runtime defined tracepoint event mechanism may eventually evolve
to a tiny compiler/interpreter like Systemtap [7] or gdb [8].


Implementation
--------------

After comparing dyninst with gdb's tracepoint code, I think using dyninst as
current choice of implementing dynamic instrumentation is more appropriate.
gdb's tracepoint function relies on a lot of support code they already have,
such as disassembler, elf/DWARF parser. We'll need to a lot of work to port all
the support code.

The underlying technique of these two are quite similar. I will try if I can
isolate interfaces to make changing back-end of dynamic instrumentation easier
or even configurable.

The technique behind scene is simple. We replace the instrumented instruction
with a jmp to redirect the control flow to a trampoline. After saving registers
in the trampoline, we call the probe callback function. Then restore the saved
registers, execute the original instruction and jmp back to the original place.

I will give a simple example here.

Suppose function do_stuff has the following assembly code:

    0x0000000000400a30 <+0>:     push   %rbx
    0x0000000000400a31 <+1>:     mov    $0x400b34,%esi
    0x0000000000400a36 <+6>:     mov    $0x400b36,%edi
    0x0000000000400a3b <+11>:    callq  0x4007b0 <fopen@plt>
    0x0000000000400a62 <+16>:    ret

After executing the following command:

    # lttng enable-event aname -u --pid 8964 --probe do_stuff+0x6

function do_stuff will become:

    0x0000000000400a30 <+0>:     push   %rbx
    0x0000000000400a31 <+1>:     mov    $0x400b34,%esi
    0x0000000000400a36 <+6>:     jmpq   0x1002a             # jmp to trampoline
    0x0000000000400a3b <+11>:    callq  0x4007b0 <fopen@plt>
    0x0000000000400a62 <+16>:    ret

    (gdb) x/100i 0x1002a
    0x1002a: push   %rax                     # save registers
    ....................
    0x1003e: push   %r15
    0x1004f: movabs $0x7f682f61eac7,%rbx
    0x10059: callq  *%rbx                    # call lttng probe
    0x10060: pop    %r15                     # restore registers
    ....................
    0x10075: pop    %rax
    0x1007a: mov    $0x400b36,%edi           # this is the original instruction
    0x1007f: jmpq   0x0000000000400a3b       # jmp back

This is an oversimplified example. In reality, we need to do more than that,
like aligning stack. You can refer [3] for a detailed line-by-line analysis
on dyninst's behavior.


[1]: http://bugs.lttng.org/projects/lttng-tools/wiki
[2]: http://bugs.lttng.org/issues/15
[3]: https://github.com/5kg/lttng-gsoc/blob/master/notes/dyninst.md
[4]: http://sourceware.org/systemtap/SystemTap_Beginners_Guide/utargetvariable.html
[5]: http://sourceware.org/systemtap/SystemTap_Beginners_Guide/ustack.html
[6]: http://sourceware.org/gdb/onlinedocs/gdb/Tracepoint-Actions.html#Tracepoint-Actions
[7]: http://sourceware.org/systemtap/SystemTap_Beginners_Guide/understanding-how-systemtap-works.html#understanding-architecture-tools
[8]: http://sourceware.org/gdb/onlinedocs/gdb/Agent-Expressions.html
[9]: http://lists.lttng.org/pipermail/lttng-dev/2013-January/019413.html
[10]: http://lists.lttng.org/pipermail/lttng-dev/2013-January/019414.html

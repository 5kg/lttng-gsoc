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

The proposed command line interface is very similar to the current lttng kernel dynamic probe interface [1].

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

Being only able to attach and instrumente existing processes is sometimes restricted. If we can have #15 [2] implemented, then we can extend the command to support dynamic instrumentation.

    lttng trace -c COMMAND
        --probe (addr | symbol | symbol+offset)
        --function (addr | symbol | symbol+offset)

This will execute given program and place probes at certain places.

Examples:

    # lttng trace -c "ls -l" --probe main+0x10
    # lttng trace -c "ls -l" --function main

Public API
----------

TBD

Implementation
--------------

TBD


[1]: http://bugs.lttng.org/projects/lttng-tools/wiki
[2]: http://bugs.lttng.org/issues/15

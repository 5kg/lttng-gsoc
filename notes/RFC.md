RFC - Dynamic instrumentation support in UST
============================================

Author: Zifei Tong <soariez@gmail.com>

Version:
  - v0.1: Jul 25, 2013
    * Initial proposal

Motivation
----------

TBD

Command Line Interface
----------------------

The proposed command line interface is very similar to the current lttng kernel dynamic probe interface [1].

To enable a dynamic probe in a running process at given address:

    lttng enable-event NAME -u --pid PID
        --probe (addr | symbol | symbol+offset)
                               Dynamic probe.
                               Addr and offset can be octal (0NNN...),
                               decimal (NNN...) or hexadecimal (0xNNN...)
Examples:

    # lttng enable-event aname -u --pid 8964 --probe main+0x0
    # lttng enable-event aname -u --pid 8964 --probe 0xffff7260695

You can also enable function tracer.

    lttng enable-event NAME -u --pid PID
        --function (addr | symbol | symbol+offset)
                             Dynamic function entry/return probe.
                             Addr and offset can be octal (0NNN...),
                             decimal (NNN...) or hexadecimal (0xNNN...)
Examples:

    # lttng enable-event aname -u --pid 8964 --function foo


Public API
----------

TBD

Implementation
--------------

TBD


[1]: http://bugs.lttng.org/projects/lttng-tools/wiki

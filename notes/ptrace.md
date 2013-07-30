* Remarks
    * Kernel won't allow threads belong to a same threadgroup [^2] trace each other [^1].

[1]: https://github.com/torvalds/linux/blob/fab840fc2d542fabcab903db8e03589a6702ba5f/kernel/ptrace.c#L301
[2]: http://stackoverflow.com/questions/9305992/linux-threads-and-process

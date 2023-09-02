# Http

This is an implementation of a HTTP server in C++.  When complete, it
will be fully asynchronous, behaving so itself and allowing client code
to behave so as well.

## POSIX

### Docker

This uses the **development** image the **Yarborough** project makes to make the **Http** container.  The **RunDockerImage.cmd**
command file will make, start, and attach to that container.

### Debugging

Use either `gdb` or `lldb`.  Add `__builtin_debugtrap();` where desired to the code.

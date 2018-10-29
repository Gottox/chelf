chelf - Change ELF headers
==========================

Changes or displays the stack size of an ELF binary.

This value is used by [musl-libc](http://musl-libc.org) to increase the
default stack size.

chelf supports both ELF32 and ELF64.

Building
--------

To build chelf a C compiler and a posix compatible libc are needed.

```
make
```

To install chelf:

```
cp chelf /usr/local/bin
```

Examples
--------

read the stack size of a file:

```
chelf /bin/sh
```

change the stack size of a file:

```
chelf -s 1052672 /usr/bin/gnome-shell
```

License
-------

See LICENSE

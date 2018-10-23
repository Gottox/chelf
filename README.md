chstk - Change Stack Size
=========================

Changes or displays the stack size of an ELF binary.

This value is used by [musl-libc](http://musl-libc.org) to increase the
default stack size.

chstk supports both ELF32 and ELF64.

Building
--------

To build chstk a only C compiler and a posix compatible libc are needed.

```
make
```

To install chstk:

```
cp chstk /usr/local/bin
```

Examples
--------

read the stack size of a file:

```
chstk /bin/sh
```

change the stack size of a file:

```
chstk 1052672 /usr/bin/gnome-shell
```

License
-------

See LICENSE

# libo

libo is a library for managing OSC bundles and the odot-expression language,
by John MacCallum.

## Building libo

libo can be built on Mac OS X, Windows 10 (MinGW + MSYS2), and many flavors
of Linux, including Raspberry Pi OS. The result is a static library, `libo.a`,
that can be linked to by other programs, such as the odot objects for Max
and PD.

### Prerequisites for all platforms

* libo requires recent versions of [http://flex.sourceforge.net](Flex)
and [http://www.gnu.org/software/bison/](Bison). These can be installed
using your system's package manager or from source.

Note for OS X: the OS X Developer's
Tools install an old version of Flex and Bison that *will not work*---newer
versions can be installed using [https://brew.sh](homebrew) or from source.

### Mac OS X

```
$ cd <path/to/libo>
$ make
```

### Windows 10

The Windows 10 build is done using MinGW under MSYS2. Make sure you have
installed the 64-bit GCC toolchain (`pacman -S mingw-w64-x86_64-toolchain`),
and Flex (`pacman -S flex`) and Bison (`pacman -S bison`).

```
$ cd <path/to/libo>
$ make win64
```

Note that 32-bit builds on Windows are not supported.

### Linux

The Makefile is set up to use clang. GCC should also work, with the appropriate
changes to the Makefile.

```
$ cd <path/to/libo>
$ make linux

```

### Issues

* If a linker error is produced stating that libfl.a could not be found,
it was probably installed somewhere other than `/usr/local/lib`. You'll have
to find it on your system (or reinstall Flex), and either symlink to
`/usr/local/lib` or add its location to your path.
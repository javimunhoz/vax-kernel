Simple VAX Kernel (SVK)
=======================

This project hosts the required code (bootstrap, loader, kernel and shell) to
drive a MicroVAX 3900.

In detail, the code explores the required minimal bootstrapping code, Memory
Management Unit (MMU) support, interrupt handling, I/O (console support) and
multitasking.

More information about this project:

[http://javiermunhoz.com/blog/2013/09/10/vax-virtual-bare-metal-programming.html](http://javiermunhoz.com/blog/2013/09/10/vax-virtual-bare-metal-programming.html)

Licensing
=========

SVK is freely redistributable under the two-clause BSD License. Use of this
source code is governed by a BSD-style license that can be found in the
`LICENSE` file.

Dependencies
============

1. This code was developed and tested in a GNU/Linux system ([Debian GNU/Linux](http://www.debian.org))
2. A cross-compiler VAX toolchain is required. You can build it using NetBSD.
   See [The NetBSD guide (Chapter 31)](http://www.netbsd.org/docs/guide/en/chap-build.html)
3. You need some hardware to test. You can use [SIMH v3.9](http://simh.trailing-edge.com/sources/simhv39-0.zip)
   in order to get a virtual testbed for MicroVAX 3900. If you use SIMH you
   should check vax and ka655x.bin binaries are available.

Compiling and building
======================

1. Grab the code with Git. Use the following command:

   ~$ git clone https://github.com/javimunhoz/vax-kernel

2. Compile the sources (boot and kernel)

   ~$ cd vax-kernel && make -C src/

3. Build an ISO

   ~$ ./scripts/build-iso.sh

4. Run it

   ~$ ./scripts/boot.sh

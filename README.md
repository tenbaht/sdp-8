# Simple-Data-Processor-8

A very simple PDP-8 simulator (even for embedded systems).

This is just a little fun project, the result of one day reading and hacking
about a classic masterpiece of computer architecture: A very simple PDP-8
simulator.

The best way to _really_ understand a system design is (at least for me)
to write an emulator. And doing so for the PDP-8 is surprisingly simple. The
instruction set architecture is designed to be implemented with discrete DTL
logic and highly optimized to be as simple as possible, but still surprisingly
efficient and powerful. A beautiful example of well-done optimization,
almost a piece of art!

It turns out the the simulator core compiles to only 1 to 1.25kB of code for
popular MCUs like the AVR or the STM8.


## Simulator features

Supported CPU features:

- regular commands, Group 1 and 2 OPR commands incl. microcoding
- 4kWords of core memory
- auto-preincrement registers 0010-0017
- simplyfied I/O: All IOT commands are directed to stdin and stdout
- read paper tape images in BIN and RIM format


Missing features:

- Group 3 OPR commands (multiply, division)
- OSR command (Read the front-panel switches to AC) is ignored


Severe simplifications:

- No accurate timing. It runs as fast as the host CPU is capable of.


## Compilation

	$ make
	cc    -c -o sdp8.o sdp8.c
	cc    -c -o sim.o sim.c
	cc    -c -o memory.o memory.c
	cc    -c -o io.o io.c
	cc    -c -o readtape.o readtape.c
	cc   sdp8.o sim.o memory.o io.o readtape.o   -o sdp8



## Usage

	sdp8 filename [startadr]

Paper tape images can be in BIN or RIM format. The optional start address
can be in decimal, octal (with leading '0') or hex (with leading '0x'). If no
address is given, the execution starts at address 0.

Example:

	./sdp8 examples/arraycopy/arraycopy.bin 0200



## Code size

compiling for different MCUs (only the simulator core, without paper tape
reader and debug output):

- AVR (avr-gcc -O2): 1333 bytes
- STM8 (sdcc 3.9.1 #11242): 1054 bytes
- ARM:


## Introduction into PDP-8 programming

Must-reads:

- [Wikipedia article](https://en.wikipedia.org/wiki/PDP-8) with very
  detailed but compact information about the instruction set and system
  architecture
- [Introduction to programming PDP-8 Family
  Computers](http://bitsavers.informatik.uni-stuttgart.de/pdf/dec/pdp8/handbooks/IntroToProgramming1969.pdf):
  An excellent introduction into assembler programming in general. It covers
  all the basics including the conversion of floating point numbers. This book
  should be part of the standard curriculum to all computer schools.

Useful stuff:

- [MACRO-8 programming
  manual](https://www.grc.com/pdp-8/docs/MACRO-8_Programming_Manual.pdf):
  Good and comprehensive introduction for the PDP-8 instruction set. The
  MACRO-8 assembler is part of the simh simulator package.
- [Detailed description of the I/O commands](http://homepage.cs.uiowa.edu/~jones/pdp8/man/tty.html)


## Software

MACRO8: The simh packages (see below) include macro8x, an assembler for
PDP-8 code. Unfortunatly, the simulator can't read the .bin files generated
by the assembler.

Images of original software for the PDP-8 (incl. FOCAL and OS8):
http://so-much-stuff.com/pdp8/papertape/papertape.php


## Similar projects

There are so many PDP-8 simulators out there. And now I understand why: The
architecture is so simple. So clean. So beautiful. And there is so much to
learn from it. Highly recommanded educational and fun project for every
programmer.

- [A PDP-8 simulator written in JavaScript and HTML](https://programmer209.wordpress.com/2011/01/30/pdp-8-assembly-language-part-1/)
- [simh 4.x](https://github.com/simh/simh): github repository of the more
  powerful 4.x stream of the most comprehensive simulator available
- [simh 3.x](http://simh.trailing-edge.com/): Website for the still maintained
  3.x branch of simh. More simple than the 4.x version.
- [js-pdp8](https://github.com/MircoT/js-pdp8): Another JavaScript PDP-8
  simulator, allows direct input of assembler source code.



[//]: # vim: tw=78


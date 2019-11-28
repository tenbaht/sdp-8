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


Missing features:

- Group 3 OPR commands (multiply, division)
- OSR command (Read the front-panel switches to AC) is ignored


Severe simplifications:

- No accurate timing. It runs as fast as the host CPU is capable of.
- The memory image is compiled into the executable and there is no way to
  exchange or reload it without recompiling.



## Code size

compiling for different MCUs:

- AVR (avr-gcc -O2): 1333 bytes
- STM8 (sdcc 3.9.1 #11242): 1054 bytes
- ARM:


### Software

Images of original software for the PDP-8 (incl. FOCAL and OS8):
http://so-much-stuff.com/pdp8/papertape/papertape.php

[//]: # vim: tw=78


#include <stdio.h>
#include "sim.h"
#include "io.h"

/* --- I/O ----------------------------------------------------------- */

/*
 * (very) simplyfied I/O-handling
 *
 * Input reads from stdin, output writes to stdout, independed of the
 * value of the device field in the instruction.
 *
 * FIXME: I/O operations very incomple
 */
void do_io(void)
{
	// bit 0: skip next instruction if device is ready.
	if (ir & 0001) {
		// FIXME: this always assume ready state
		pc++;
	}


	// bit 2: move a word between ACC and the device
	if (ir & 0004) {
		if (ir & 0010) {
			// do input
			acc = getchar();
		} else {
			// do output
			putchar(acc);
			// bit 1: clear ACC
			if (ir & 0002) {
				acc = 0;
			}
		}
	}
}


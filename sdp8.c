#include <stdio.h>
#include "sdp8.h"
#include "sim.h"
#include "memory.h"
#include "readtape.h"

/* --- main ---------------------------------------------------------- */

void dump_state(void)
{
	printf("%04o: ir=%04o acc=%04o link=%1o\n", pc, ir, acc, link);
}

static void sim_loop()
{
	halt	= 0;
	pc	= 0;
	acc	= 0;
	do {
		ir = mem_read(pc);
		dump_state();
		pc++;
		do_instruction();
		pc &= 07777;
	} while (!halt);
	dump_state();
}


int main(int argc, char *argv[])
{
	if (argc<2) {
		printf("usage: %s filename\n", argv[0]);
		return 1;
	}

	read_tape(argv[1]);
	sim_loop();
}


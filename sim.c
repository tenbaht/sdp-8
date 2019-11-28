#include <stdio.h>
#include <stdint.h>

#ifdef DEBUG
# define DEBUG_PRINT(...) fprintf( stderr, __VA_ARGS__ )
#else
# define DEBUG_PRINT(...) do{ } while (0)
#endif

/*
#if defined(DEBUG) && DEBUG > 0
 #define DEBUG_PRINT(fmt, args...) fprintf(stderr, "DEBUG: %s:%d:%s(): " fmt, \
    __FILE__, __LINE__, __func__, ##args)
*/

// opcodes and bit masks for instruction analysis

#define OP_AND	00000
#define OP_TAD	01000
#define	OP_ISZ	02000
#define	OP_DCA	03000
#define	OP_JMS	04000
#define	OP_JMP	05000
#define	OP_IOT	06000
#define	OP_OPR	07000

#define	BIT_CURRENT_PAGE	00200
#define	BIT_INDIRECT		00400

#define	MASK_OPCODE		OP_OPR
#define MASK_BITS		(BIT_CURRENT_PAGE||BIT_INDIRECT)
#define MASK_PAGE		(MASK_OPCODE||MASK_BITS)
#define MASK_OFFSET		00177


// simulator status codes

#define STATUS_HALT	00001

uint16_t	acc, link, ir, pc;
uint16_t	halt;


/* --- memory --------------------------------------------------------- */

uint16_t	mem[4096]={
	// copy arr1 to arr2
	07300,	//  cla cll
	01410,	//  copy,   tad i pArr1
	03411,	//	    dca i pArr2
	02206,	//          isz count
	05201,	//          jmp copy
	07402,	//          hlt
	07772,	//  count,  7772
	0,
		// auto-indexing register at 0010:
	00017,	//  pArr1,  arr1-1
	00037,	//  pArr2,  arr2-1
	01,02,03,04,05,06,
		// the arrays at 0020:
	01111,	//  arr1,   1111;2222;3333;4444;5555
	02222,
	03333,
	04444,
	05555,

/*
	// I/O-Test: print "ABC"
	07300,	// CLA, CLL
	01010,	// TAD 010
	06006,	// IOT 4, CLA
	01011,	// TAD 011
	06006,	// IOT 4, CLA
	01012,	// TAD 012
	06006,	// IOT 4, CLA
	07402,	// HLT
	'A',
	'B',
	'C',
*/
/*
	// simple loop test: for i=4 to 255; next
//	07300,	// CLA, CLL
	07307,	// CLA, CLL, IAC, RTL (clc, lda #4)
	07001,	// IAC
	07440,	// SZA
	05001,	// JMP 0001
	07402,	// HLT
*/
};


/*
 * read one word from memory at given address
 */
static inline int16_t mem_read(uint16_t adr)
{
	DEBUG_PRINT("read mem[%05o]=%05o\n", adr, mem[adr & 07777]);
	return mem[adr & 07777] & 07777;
}


/*
 * write one word into memory at given address
 */
static inline void mem_write(uint16_t adr, uint16_t val)
{
	DEBUG_PRINT("write mem[%05o]=%05o\n", adr, val);
	mem[adr & 07777] = val & 07777;
}


/*
 * read one word from memory with preincrement
 */
static inline uint16_t mem_preinc(uint16_t adr)
{
	uint16_t	tmp;

	DEBUG_PRINT("++rd mem[%05o]=", adr);
	adr &= 07777;
	tmp = mem[adr] + 1;
	tmp &= 07777;
	mem[adr] = tmp;
	DEBUG_PRINT("%05o\n", tmp);
	return tmp;
}



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

			

/* --- operate instructions ---------------------------------------------- */

/*
 * rotation command: rotate acc through link to the right by one bit
 */
static void rotate_right(void)
{
	if (link) acc |= 010000;
	link = acc & 1;
	acc >>= 1;
}

/*
 * rotation command: rotate acc through link to the left by one bit
 */
static void rotate_left(void)
{
	acc <<= 1;
	if (link) acc |= 1;
	link = ((acc & 010000) != 0);
	acc &= 07777;
}


static void do_group1()
{
	// execution order 1: clear
	// CLA 	7200 	ACC = 0
	if (ir & 0200) {
		acc = 0;
	}
	// CLL 	7100 	LNK = 0
	if (ir & 0100) {
		link = 0;
	}

	// execution order 2: complement
	// CMA 	7040 	ACC = ~ACC
	if (ir & 0040) {
		acc ^= 07777;
	}
	// CML 	7020 	LNK = ~LNK
	if (ir & 0020) {
		link ^= 1;
	}

	// execution order 3: increment with carry (link)
	// IAC 	7001 	ACC++
	if (ir & 0001) {
		acc++;
		link = ((acc & 010000) != 0);
		acc &= 07777;
	}

	// execution order 4: rotation commands
	// FIXME: what happens for 14? Combining left and right shift is
	// illegal? Or NOP?
	// RAR 	7010 	RotR(ACC,LNK,1)
	// RTR 	7012 	RotR(ACC,LNK,2)
	if ((ir & 0014) == 0010) {
		// rotate right
		rotate_right();
		if ( ir&0002 ) {
			rotate_right();
		}
	}
	// RAL 	7004 	RotL(ACC,LNK,1)
	// RTL 	7006 	RotL(ACC,LNK,2)
	if ((ir & 0014) == 0004) {
		// rotate left
		rotate_left();
		if ( ir&0002 ) {
			rotate_left();
		}
	}
	// BSW 	7002 	ByteSwap(ACC)
	if ((ir & 0016) == 0002) {
		uint16_t tmp = acc&0077;
		acc >>= 6;
		acc |= tmp<<6;
	}

	// default: NOP 	7000 	No operation
}

/*
meaning of the bits:
f=fixed, c=clear ACC, s=skip, g=group, h=halt, w=switch reg

OR Sub-Group                            fcs ssg whf
NOP 	7400 	No operation		100 000 000
CLA 	7600 	ACC = 0			110 000 000
SMA 	7500 	Skip if ACC < 0		101 000 000
SZA 	7440 	Skip if ACC == 0	100 100 000
SNL 	7420 	Skip if LNK != 0	100 010 000
OSR	7404	ACC |= switch register	100 000 100
HLT 	7402 	Halt			100 000 010
AND Sub-Group
CLA 	7610 	ACC = 0			110 001 000
SPA 	7510 	Skip if ACC >= 0	101 001 000
SNA 	7450 	Skip if ACC != 0	100 101 000
SZL 	7430 	Skip if LNK == 0	100 011 000
SKP 	7410 	Unconditional Skip	100 001 000

meaning of the bit groups:
 4-6 skip bits

Bit 0: 0
Bit 1: go into halt state
Bit 2: OSR (or with switch register)
Bit 3: 0=OR group, 1=AND group (inverts the meaning)
Bit 4: check for link bit
Bit 5: check for zero bit of ACC
Bit 6: check sign bit of ACC
Bit 7: set ACC=0
Bit 8: 1

*/
static void do_group2()
{
	char skip = 0;

	// execution order 1: check for skip conditions
	// Bits 4-6: conditions for skip commands
	if (ir & 0100) {
		// SMA 	7500 	Skip if ACC < 0
		// SPA 	7510 	Skip if ACC >= 0
		if (acc&04000) skip = 1;
	}
	if (ir & 0040) {
		// SZA 	7440 	Skip if ACC == 0
		// SNA 	7450 	Skip if ACC != 0
		if (acc==0) skip = 1;
	}
	if (ir & 0020) {
		// SNL 	7420 	Skip if LNK != 0
		// SZL 	7430 	Skip if LNK == 0
		if (link) skip = 1;
	}
	// Bit 3: invert the skip conditions (AND/OR-Group)
	if (ir & 0010) {
		// AND Sub-Group means inverting the conditions
		// (this automatically converts NOP into SKP)
		skip ^= 1;
	}
	// implicit default (if no condition code was given):
	// NOP 	7400 	No operation
	// SKP 	7410 	Unconditional Skip
	if (skip) pc++;	// FIXME: how about halt mode? Still increment?

	// execution order 2: clear, read switch register
	// Bit 7: clear ACC
	if (ir & 0200) {
		// CLA 	7600 	ACC = 0	(group 2 OR)
		// CLA 	7610 	ACC = 0 (group 2 AND)
		acc = 0;
	}
	// Bit 2: OSR (read switch register)
	//FIXME: switch register not implemented
	// acc |= sr;

	// execution order 3: halt
	// Bit 1: halt mode
	// FIXME: does HLT 7412 exist (AND group)?
	if (ir & 0002) {
		// HLT 	7402 	Halt
		halt = 1;
	}
}

//FIXME: group 3 not implemented
static void do_group3()
{
	// Bit 7: clear ACC
	if (ir & 0200) {
		// CLA 	7601 	ACC = 0	(group 3)
		acc = 0;
	}
}


/**
 * decode the OPR instructions
 *
 * Since the CLA bit (bit 7) is the same for all three groups it is
 * tempting to handle it right here instead of duplicating the code three
 * times for every group. But this would mess up the execution order of the
 * microcoded sub-instructions (execution order 1 for group 1 and 3, order 2
 * for group 2).
 */
static void do_operate_instructions()
{

	// bit 0 and 8 determine the instruction group 1/2/3
	// only commands within one group can be combined into a single
	// instruction.
	if (ir & 0400) {
		// bit 8 == 1: group 2 or 3
		if (ir & 0001) {
			// bit 0 == 1: group 3
			do_group3();
		} else {
			// bit 0 == 0: group 2
			do_group2();
		}
	} else {
		// bit 8 == 0: group 1	
		do_group1();
	}
}

/* --- general instruction handling -------------------------------------- */


/**
 * decode absolute address from MRI field of the current instruction
 */
static uint16_t decode_address(void)
{
	uint16_t	adr;

	adr = ir & MASK_OFFSET;	// lower 7 bits

	// check for current page bit:
	if (ir & BIT_CURRENT_PAGE) {
		// use the upper 5 bits from the current pc
		// (otherwise default to zero page)
		adr |= pc & 07600;
	};
//	DEBUG_PRINT("adr=0%05o", adr);
	// check for indirect bit:
	if (ir & BIT_INDIRECT) {
		if ((adr & 07770) == 00010) {
			// handle autoindexing for address 0010..0017
			adr = mem_preinc(adr);
		} else {
			// regular read access
			adr = mem_read(adr);
			DEBUG_PRINT(", adr_I=0%05o", adr);
		}
	}
	DEBUG_PRINT("\n");
	return adr;
}



void do_instruction(void)
{
	uint16_t addr;

	// decode the given address reference (if needed)
	if (ir < OP_IOT) {
		addr = decode_address();
	}

	switch (ir & MASK_OPCODE) {
		case OP_AND:	// ACC = ACC & C(Addr)
			acc &= mem_read(addr);
			break;
		case OP_TAD:	// ACC += C(Addr)
			acc += mem_read(addr);
			if (link) acc++;
			link = ((acc & 010000) != 0);
			acc &= 07777;
			break;
		case OP_ISZ:	// C(Addr)++; if(C(Addr)==0) PC++;
			if (mem_preinc(addr) == 0) pc++;
			break;
		case OP_DCA:	// C(Addr) = ACC; ACC = 0;
			mem_write(addr,acc);
			acc = 0;
			break;
		case OP_JMS:	// C(Addr)=PC+1; PC=Addr+1;
			mem_write(addr, pc);	// pc is already incremented
			pc = addr+1;
			break;
		case OP_JMP:	// PC = Addr
			pc = addr;
			break;
		case OP_IOT:	// Communicate with an IO device
			do_io();
			break;
		case OP_OPR:	// Micro-coded Instructions
			do_operate_instructions();
			break;
	};
}

/* --- main ---------------------------------------------------------- */

void dump_state(void)
{
	printf("%04o: ir=%04o acc=%04o link=%1o\n", pc, ir, acc, link);
}

void main()
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



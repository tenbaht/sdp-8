#include <stdio.h>
#include <stdint.h>

#include "sdp8.h"
#include "memory.h"
#include "readtape.h"


//#define BUFSIZE	128

#define vprintf	printf
//#define dprintf(...) do{}while(0)

// state definitions
#define IDLE	0
#define DATA	1

static unsigned char	state;

/*
static void mem_write(uint16_t adr, uint16_t val)
{
	vprintf("mem_write(0%04o)=0%04o\n", adr, val);
}
*/


/**
 * a very simple scanner
 *
 * Ignores the leader part and stops at the trailer.
 *
 * @returns: a valid 13 bit value or EOF (-1)
 */
static int16_t read_word(FILE *fd)
{
	int8_t v,w;	// 8 bit signed is important

	// ignore leader (ignore 0x80 bytes at beginning)
	if (state==IDLE) {
		do {
			v = fgetc(fd);
			DEBUG_PRINT("gotA 0%03o ", v);
			if (v==(int8_t)-1) return EOF;
		} while (v<0);//==0x80);
		state = DATA;
	} else {
		v = fgetc(fd);		// intentional downcast to 8 bit signed
	DEBUG_PRINT("gotB 0%03o ", v);
	}
	if (v<0) return EOF;	// This catches both, EOF and values >= 0x80

	w = fgetc(fd);
	DEBUG_PRINT("gotC 0%03o ", w);
	if (w<0) return EOF;

	return (v<<6) | (w & 0077);
}


int read_tape(char *name)
{
	FILE		*fd;
	int16_t		v;
	uint16_t	words, addr, max_addr;

	fd	= fopen(name,"rb");
	if (!fd) return -1;

	state	= IDLE;
	addr	= 0;
	words	= 0;
	max_addr= 0;
	while ((v=read_word(fd))>=0) {//!=EOF) {
		if (v & 010000) {
			addr = v & 07777;
vprintf("set address to 0%04o\n", addr);
			continue;
		}
		mem_write(addr, v);
		// update the statistics
		words++;
		if (addr > max_addr) {
			max_addr = addr;
		}
		addr++;
	}

	printf(
		"read %d words from file %s, max. address 0%04o\n",
		words, name, max_addr
	);
}



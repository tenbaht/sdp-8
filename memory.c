#include <stdio.h>
#include <stdint.h>
#include "sdp8.h"
#include "memory.h"

uint16_t	mem[4096];


/*
 * read one word from memory at given address
 */
int16_t mem_read(uint16_t adr)
{
	DEBUG_PRINT("read mem[%05o]=%05o\n", adr, mem[adr & 07777]);
	return mem[adr & 07777] & 07777;
}


/*
 * write one word into memory at given address
 */
void mem_write(uint16_t adr, uint16_t val)
{
	DEBUG_PRINT("write mem[%05o]=%05o\n", adr, val);
	mem[adr & 07777] = val & 07777;
}


/*
 * read one word from memory with preincrement
 */
uint16_t mem_preinc(uint16_t adr)
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


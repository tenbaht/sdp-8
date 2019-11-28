#include <stdint.h>

/*
 * read one word from memory at given address
 */
int16_t mem_read(uint16_t adr);


/*
 * write one word into memory at given address
 */
void mem_write(uint16_t adr, uint16_t val);



/*
 * read one word from memory with preincrement
 */
uint16_t mem_preinc(uint16_t adr);


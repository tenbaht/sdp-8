#include <stdint.h>

void do_instruction(void);

// simulator status codes

#define STATUS_HALT	00001

// state of the simulated CPU
extern uint16_t	acc, link, ir, pc;
extern uint16_t	halt;



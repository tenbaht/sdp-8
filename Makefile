sim:

sim-avr.o: sim.c
	avr-gcc -O2 -c $<

sim-stm8.rel: sim.c
	sdcc -mstm8 -c $<


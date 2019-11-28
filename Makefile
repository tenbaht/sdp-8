.PHONY: clean

sdp8: sdp8.o sim.o memory.o io.o readtape.o


sim-avr.o: sim.c
	avr-gcc -O2 -c $<

sim-stm8.rel: sim.c
	sdcc -mstm8 -c $<

clean:
	rm -f *.o *.rel *.asm *.lst *.rst *.sym *.hex *~ *.bak



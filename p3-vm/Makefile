# Makefile for all examples

PROGRAMS = test-vm pagefaults

CFLAGS = -m32 -Wall 

all: $(PROGRAMS)

run:
	./test-vm >test-vm.out

pack:
	tar czvf ijc-new-examples.tar.gz Makefile *.c 

clean:
	rm -f $(PROGRAMS)


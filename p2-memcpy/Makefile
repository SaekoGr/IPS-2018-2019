
# Test of memory transfer rate

CFLAGS = -std=c99 -O2 -Wall -pedantic -march=native -mtune=native
PROGS  = memcpy-bench memset-bench

all: $(PROGS)

run: $(PROGS)
	./memcpy-bench >/dev/null    #warm-up
	./memcpy-bench >memcpy-bench-data
	./memcpy-bench >>memcpy-bench-data
	./memset-bench >memset-bench-data
	./memset-bench >>memset-bench-data
	./plot-result.sh memcpy-bench-data
	./plot-result.sh memset-bench-data

clean:
	rm -f $(PROGS)

clean-all:
	rm -f $(PROGS) memcpy-bench-data memset-bench-data

pack:
	tar czvf profiling-memcpy-memset.tar.gz *.c *.sh Makefile


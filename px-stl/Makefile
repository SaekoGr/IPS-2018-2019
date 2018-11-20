CXX=g++ -Wall -O3 -g

all: test

test: test.cc
	$(CXX) $^ -o $@
run: test
	./test >test.out        # warm-up
	./test >test.out
clean:
	rm -f test.o test test.out
plot: run
	./test-plot.sh
pack:
	tar czvf stl-timing.tar.gz *.cc Makefile *.sh



all:
	bash ips-mat-mul-demo-cachegrind.sh

clean:
	rm -f cachegrind.out.[0-9]* objdump.out
pack:
	tar czvf ips-mat-mul-demo-cachegrind.tar.gz *.c *.sh Makefile *.log


#!/bin/sh

LOG="ips-mat-mul-demo.log"      ## output file
CFLAGS="-O3 -g"                 ## set GCC options
STKLEN=2                        ## set stack size limit (in GB)
N=512

echo Set ulimit -s ${STKLEN}000000
ulimit -s ${STKLEN}000000

L1SZ=$(getconf LEVEL1_DCACHE_SIZE)
L1AS=$(getconf LEVEL1_DCACHE_ASSOC)
L1LS=$(getconf LEVEL1_DCACHE_LINESIZE)
echo "Data L1: $L1SZ $L1AS $L1LS"
I1SZ=$(getconf LEVEL1_ICACHE_SIZE)
I1AS=$(getconf LEVEL1_ICACHE_ASSOC)
I1LS=$(getconf LEVEL1_ICACHE_LINESIZE)
echo "Instr L1: $I1SZ $I1AS $I1LS"
LL=2
LnSZ=$(getconf LEVEL${LL}_CACHE_SIZE)
LnAS=$(getconf LEVEL${LL}_CACHE_ASSOC)
LnLS=$(getconf LEVEL${LL}_CACHE_LINESIZE)
echo "Cache L${LL}: $LnSZ $LnAS $LnLS"

run_test() {
valgrind --tool=cachegrind \
         --I1=$I1SZ,$I1AS,$I1LS \
         --D1=$L1SZ,$L1AS,$L1LS \
         --LL=$LnSZ,$LnAS,$LnLS \
         ./a.out $1
}

echo N=$N
gcc $CFLAGS -DSIZE=$N ips-mat-mul-demo.c -lgsl -lcblas -lm

run_test 1    ## textbook alg
run_test 2    ## optimized version

objdump -S ./a.out >objdump.out

rm ./a.out


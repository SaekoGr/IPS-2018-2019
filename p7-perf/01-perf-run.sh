
CFLAGS="-O3 -g"                 ## set GCC options
STKLEN=2                        ## set stack size limit (in GB)
N=512

echo Set ulimit -s ${STKLEN}000000
ulimit -s ${STKLEN}000000

gcc $CFLAGS -DSIZE=$N ips-mat-mul-demo.c -lgsl -lcblas -latlas -lm

# run
perf record \
 -e cache-misses,L1-dcache-load-misses,faults,cycles,dTLB-load-misses \
 -F 250 \
 -- ./a.out 1
# view
#perf report

#rm ./a.out


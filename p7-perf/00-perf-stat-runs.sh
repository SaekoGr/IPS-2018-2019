
CFLAGS="-O3 -g"                 ## set GCC options
STKLEN=2                        ## set stack size limit (in GB)
N=512

echo Set ulimit -s ${STKLEN}000000
ulimit -s ${STKLEN}000000


run() {
    echo "perf stat ./a.out $1 (CFLAGS=$CFLAGS)"
    echo ==================================================
    perf stat ./a.out $1
}

gcc $CFLAGS -DSIZE=$N ips-mat-mul-demo.c -lgsl -lcblas -latlas -lm
# run
run 1
run 3
run 4

echo "**************************************************"

run 2
CFLAGS="-O3 -g -funroll-loops"
gcc $CFLAGS -DSIZE=$N ips-mat-mul-demo.c -lgsl -lcblas -latlas -lm
run 2

#rm ./a.out


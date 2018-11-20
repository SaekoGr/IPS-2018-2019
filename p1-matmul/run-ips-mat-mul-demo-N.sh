#!/bin/bash

if [ $# = 0 ]; then
    echo "Usage: $0 N"
    exit 1
fi
NN=$1
LOG="ips-mat-mul-demo-N.log"    ## output file
CFLAGS="-O3"                    ## set GCC options
STKLEN=2                        ## set stack size limit (in GB)

echo Set ulimit -s ${STKLEN}000000
ulimit -s ${STKLEN}000000
if [ -f $LOG ]; then
    mv $LOG $LOG-
fi
echo "Measuring matrix multiplication algorithms"       >$LOG
( uname -a; date; echo CFLAGS="$CFLAGS"; echo )         >>$LOG
for N in `seq $((NN-1)) $((NN+1))`; do
    echo "N=$N"
    echo N=$N                                           >>$LOG
    gcc $CFLAGS -DSIZE=$N ips-mat-mul-demo.c -lgsl -lcblas -lm
    for i in 1 2 3 4; do
        ./a.out $i
    done                                                >>$LOG
    echo                                                >>$LOG
done
( echo; date; echo )                                    >>$LOG
rm ./a.out


#!/bin/bash

LOG="ips-mat-mul-demo.log"      ## output file
STKLEN=1                        ## set stack size limit (in GB)
CFLAGS=("-O0" "-O2" "-O3" "-O3 -funroll-loops");        ## array

echo Set ulimit -s ${STKLEN}000000
ulimit -s ${STKLEN}000000
if [ -f $LOG ]; then
    mv $LOG $LOG-
fi
echo "Measuring matrix multiplication algorithms"       >$LOG
( uname -a; date; echo CFLAGS="${CFLAGS[@]}"; echo )         >>$LOG
for N in `seq 400 16 1400`; do
    echo "N=$N"
    echo N=$N                                           >>$LOG
    for i in "${CFLAGS[@]}"; do
        gcc $CFLAGS $i -DSIZE=$N ips-mat-mul-demo.c -lgsl -lcblas -lm
        ./a.out 2                                       >>$LOG
    done
    echo                                                >>$LOG
done
( echo; date; echo )                                    >>$LOG
rm ./a.out
sync

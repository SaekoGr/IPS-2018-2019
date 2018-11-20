#!/bin/sh

LOG="ips-mat-mul-demo.log"      ## output file
CFLAGS="-O3"                    ## set GCC options
STKLEN=2                        ## set stack size limit (in GB)

echo Set ulimit -s ${STKLEN}000000
ulimit -s ${STKLEN}000000
if [ -f $LOG ]; then
    mv $LOG $LOG-
fi
echo "Measuring matrix multiplication algorithms"       >$LOG
( uname -a; date; echo CFLAGS="$CFLAGS"; echo )         >>$LOG
for N in `seq 400 16 1400`; do
    echo "N=$N"
    echo N=$N                                           >>$LOG
    gcc $CFLAGS -DSIZE=$N ips-mat-mul-demo.c -lgsl -lcblas
    for i in 1 2 3 4; do
        ./a.out $i
    done                                                >>$LOG
    echo                                                >>$LOG
done
( echo; date; echo )                                    >>$LOG
rm ./a.out


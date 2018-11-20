#!/bin/bash

if [ $# = 0 ]; then
    echo "Usage: $0 alg"
    exit 1
fi

A=$1
CFLAGS="-O3"                    ## set GCC options

N=4
echo "Test matrix multiplication algorithm #$A ($N x $N)"
gcc $CFLAGS -DPRINT -DSIZE=$N ips-mat-mul-demo.c -lgsl -lcblas -lm
#./a.out $A >test-output-$A
./a.out $A
rm ./a.out


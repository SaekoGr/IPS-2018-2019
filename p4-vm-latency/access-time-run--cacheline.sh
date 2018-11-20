#!/bin/sh
echo "Memory access timing experiment"
P=access-time

gcc -O2 $P.c

CACHE_LINE=`getconf LEVEL1_DCACHE_LINESIZE`

./a.out $CACHE_LINE 200 >$P.out

rm -f ./a.out

echo "Running Gnuplot..."

gnuplot <<__END__
set title "Memory access time -- read freshly allocated memory"
set term postscript color; set output "$P.ps"
set logscale y
set ylabel "access time [clk]"
set xlabel "offset [B]"
set nokey
set grid
set style data points
set style data impulses
plot [-500:][1:] "$P.out" using 1:2 lw 2
__END__

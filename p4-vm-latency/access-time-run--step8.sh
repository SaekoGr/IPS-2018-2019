#!/bin/sh
echo "Memory access timing experiment"
STEP=8
P=access-time   ## program name

gcc -O2 $P.c


./a.out $STEP 550 >$P-$STEP.out

rm -f ./a.out

echo "Running Gnuplot..."

gnuplot <<__END__
set title "Memory access time -- read freshly allocated memory"
set term postscript color; set output "$P-$STEP.ps"
set logscale y
set ylabel "access time [clk]"
set xlabel "offset [B]"
set nokey
set grid
set style data points
set style data impulses
plot [-500:][1:] "$P-$STEP.out" using 1:2 lw 1
__END__

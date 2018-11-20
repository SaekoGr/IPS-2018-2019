#!/bin/sh
if [ $# = 0 ]; then
    echo "Usage: $0 file.out"
    exit 1
fi

echo "Memory access timing experiment (plot)"

P=${1%.out}

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

#!/bin/bash

gnuplot <<__END__
set terminal postscript enhanced; set output "test.ps"
#set terminal png; set output "test.png"

set title "Operace pridani do kontejneru"
set xlabel "Velikost kontejneru [-]"
set ylabel "Cas [periody hodin CPU]"
set style line 1 lt 1 lw 1
set style line 2 lt 2 lw 1
set style data linespoints
set key left
set grid
set logscale x
set logscale y
plot [][1:1e6] "test.out" using 1:2 lw 2 title "vector push\\\\_back", \
               "test.out" using 1:3 lw 2 title "vector insert begin", \
               "test.out" using 1:4 title "list push\\\\_back", \
               "test.out" using 1:5 title "list insert begin", \
               "test.out" using 1:6 lw 3 title "set insert", \
               "test.out" using 1:7 title "deque push\\\\_back", \
               "test.out" using 1:8 title "deque insert begin"

__END__


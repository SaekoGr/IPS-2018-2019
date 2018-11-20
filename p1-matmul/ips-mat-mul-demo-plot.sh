#!/bin/bash
if [ $# = 0 ]; then
    F="ips-mat-mul-demo.log"
else
    F="$1"
fi
LC_ALL=C
DATA=data
echo "# $0  $F" >$DATA
echo "# $(sed -n 2p $F)" >>$DATA        # machine
echo "# $(sed -n 3p $F)" >>$DATA        # date
echo "# $(sed -n 4p $F)" >>$DATA        # CFLAGS used

egrep '(^N=|seconds$)' $F |
  sed 's/seconds.*$//' |
  sed 's/N=//' |
  column -c 40 -x >>$DATA

gnuplot <<__END__
set title "Square matrix multiplication experiment"
set term postscript; set output "data.ps"

set key top left box
set style data linespoints
set xlabel "N"
set ylabel "t [s]"
set grid
plot "data" using 1:2 title "alg1", \
     "data" using 1:3 title "alg2", \
     "data" using 1:4 title "alg3", \
     "data" using 1:5 title "alg4", \
     x**3/550000000 with lines lw 0.5 title "k.x^3"

set logscale y
plot "data" using 1:2 title "alg1", \
     "data" using 1:3 title "alg2", \
     "data" using 1:4 title "alg3", \
     "data" using 1:5 title "alg4", \
     x**3/550000000 with lines lw 0.5 title "k.x^3"
__END__

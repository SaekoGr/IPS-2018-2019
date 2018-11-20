
echo
grep ^flags /proc/cpuinfo | uniq | grep --color constant_tsc
echo
grep ^flags /proc/cpuinfo | uniq | grep --color nonstop_tsc
echo


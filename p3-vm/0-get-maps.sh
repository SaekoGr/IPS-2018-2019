
make
./test-vm >test-vm.out &
XPID=$!
echo $XPID
sleep 0.5
rm -f ./test-vm-maps
#cat /proc/$XPID/maps
cp /proc/$XPID/maps ./test-vm-maps
wait $XPID
less ./test-vm-maps


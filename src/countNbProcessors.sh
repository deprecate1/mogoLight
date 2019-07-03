#!/bin/bash
if [ -e /proc/cpuinfo ]
then
cat /proc/cpuinfo | grep "processor.*:" > tnbProc
nbProc=`cat tnbProc |wc -l`
else
nbProc=1
fi
rm -f tnbProc
echo $nbProc
# echo "We detected" $nbProc "processor(s) for compilation"

#!/bin/bash 
. scripts/sclib.sh
getDescriptionOfEngine

threads=`scripts/countNbProcessors.sh`
echo "$threads threads"
grep cpu /proc/cpuinfo | head -2
mogob=$gc_location
goban=9
echo boardsize $goban
cat scripts/trial9 | $mogob --$goban --useOpeningDatabase 0 --time 3 --dontDisplay 0 --nbThreads $threads | grep "games/sec"
goban=19
echo boardsize $goban
cat scripts/trial19 | $mogob --$goban --useOpeningDatabase 0 --time 3 --dontDisplay 0 --nbThreads $threads | grep "games/sec"

echo BE CAREFUL - these numbers are FOR EACH THREAD 

echo pour comparaison en 9x9 =========================
echo sur chocolat, 1CPU 3.4 GHz, 13400 simus/second
echo sur tipi44, 4CPU 2 GHz, 11420 simus/second per thread - 45680 au total des 4 threads
echo sur tipi44, 4CPU 2 GHz, 11390 simus/second per thread -                              --- avec mogoRelease
echo pour comparaison en 19x19 =========================
echo sur chocolat, 1CPU 3.4 GHz, 3600 simus/second
echo sur tipi44, 4CPU 2 GHz, 3409 simus/second per thread - 13636 au total des 4 threads

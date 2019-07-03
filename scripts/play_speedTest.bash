#!/bin/bash
# Play simulation test for speed evaluation
#
test_threads=4
test_machines=4

. $MOGOBULL_ROOT/scripts/sclib.sh
getLastMogoEngine
getDescriptionOfMoGo
mogob=$gc_location

echo "+++++"
## 9x9 test
goban=9
findLauncherMachine $test_machines
res9=$($gc_launcherString -i $MOGOBULL_ROOT/scripts/trial9 $mogob --$goban --useOpeningDatabase 0 --time 1 --dontDisplay 0 --nbThreads $test_threads 2>&1|  grep games)
echo "$gc_launcherString -i $MOGOBULL_ROOT/scripts/trial9 $mogob --$goban --useOpeningDatabase 0 --time 1 --dontDisplay 0 --nbThreads 4"
speed9=$(echo "$res9" | awk '{print $8}' | cut -d"(" -f2 | tr "\n" " + ")
simu9=$(echo "$res9" | awk '{print $1}'  | tr "\n" " + ")

echo "+++++"
# 19x19 test
goban=19
findLauncherMachine $test_machines
echo "$gc_launcherString -i $MOGOBULL_ROOT/scripts/trial19 $mogob --$goban --useOpeningDatabase 0 --time 1 --dontDisplay 0 --nbThreads 4" 
res19=$($gc_launcherString -i $MOGOBULL_ROOT/scripts/trial19 $mogob --$goban --useOpeningDatabase 0 --time 1 --dontDisplay 0 --nbThreads $test_threads  2>&1|  grep games)
speed19=$(echo "$res19" | awk '{print $9}'  | tr "\n" " + ")
simu19=$(echo "$res19" | awk '{print $1}'  | tr "\n" " + ")

echo "+++++"
# TODO, put those data elsewhere and make a database with it if time to
echo "Comparaison en 9x9 ========================="
echo "| chocolat   | 1CPU 3.4 GHz | 13400 simus/second"
echo "| tipi44     | 4CPU 2 GHz   | 11420 simus/second per thread"
echo "| woodcrest  | 4CPU 3 GHz   | 16000 simus/second per thread"
echo "| NOW        |              | $speed9 simus/second per thread"
echo "                            | (total simu : $simu9)"


echo "Comparaison en 19x19 ========================="
echo "| chocolat,  | 1CPU 3.4 GHz | 3600 simus/second"
echo "| tipi44     | 4CPU 2 GHz   | 3409 simus/second per thread"
echo "| woodcrest  | 4CPU 3 GHz   | 5000 simus/second per thread"
echo "| NOW        |              | $speed19 simus/second per thread"
echo "                            | (total simu : $simu19)"


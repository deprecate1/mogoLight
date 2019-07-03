#!/bin/bash

file=$(ls -rt *.dat | tail -n1)

black=$(grep "BlackCommand" $file | cut -d: -f2)
blackW=$(grep "B+" $file | wc -l)

white=$(grep "WhiteCommand" $file | cut -d: -f2)
whiteW=$(grep "W+" $file | wc -l)

#######
echo "Game played : $(echo $whiteW+$blackW | bc -l)"

if [ $whiteW -eq 0 -a $blackW -eq 0 ]; then
    percB=0.000
    percW=0.000
else    
    percB=$(echo "$blackW/($whiteW+$blackW)" | bc -l | cut -c-4)
    percW=$(echo "$whiteW/($whiteW+$blackW)" | bc -l | cut -c-4)
fi

echo "$percB ===== $black"
echo "$percW ===== $white"









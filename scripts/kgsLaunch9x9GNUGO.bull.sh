#! /bin/bash

echo You are launching GNUGO in 9x9.

#cd `pwd`/`dirname "$0"` || cd `dirname "$0"` || exit 1

sleep 1
java -jar kgsGtp.jar kgsconfigBull9x9GNUGO &
sleep 100


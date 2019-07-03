#! /bin/bash

echo You are launching mogo in 9x9.
echo find below the command line:
grep mogo kgsconfigBull19x19
echo totalTime should be 60 times the number in the following line:
grep rules.time kgsconfigBull19x19
echo Please check the nbThreads.
echo type Ctrl-C if it s not ok.
pause

sleep 1
java -jar kgsGtp.jar kgsconfigBull19x19 &
sleep 100

while true
do
res=`cat MoGo.log-0.log |grep "Opponent has not returned. Leaving game"`
# res=`cat MoGo.log-0.log |grep "FINEST: Queued command sent to engine: version"`

if [ "$res" = "" ];
then

res2=`cat MoGo.log-0.log |grep "Game is over and scored"`
if [ "$res2" = "" ];
then
sleep 1
else
echo "Game finished kill MoGo"
killall java; killall mogo;
exit
fi

else
echo "Opponent has not returned kill MoGo"
killall java; killall mogo;
exit
fi

res=`ps aux|grep mogo|grep -v grep`
if [ "$res" = "" ];
then
echo "MoGo has crashed!! Please fix the bug!"
killall java; killall mogo;
exit
fi
done


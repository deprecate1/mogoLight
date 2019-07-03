#!/bin/bash

PROG=$(basename $0)
function print_stuff()
{
    local thestring="$*"
    if [ ! $tl_verbose -eq 0 ]; then
	echo "$PROG : $thestring"
    fi
}

function usage()
{
cat <<EOF
Replaces the words in files
	    "-l")	list the targets
	    "-a")	add a target (eg. www.rto.org)
	    "-n")	number of tests
	    "-p")	payload size
	    "-i")	interval (min is 0.2)
	    "-v")	verbose mode
	    "-u")	usage
	    "-h")	help

NOTE : can check http://ping.eu/ to get a comparison

EOF
}

function checkApplications(){
    for i in $*; do
	which $i &> /dev/null
	if [ $? != 0 ]; then
	    echo "$PROG : warning, you do not have this dependency"
	fi
    done
}

#########################
#Main
#########################
tl_host=$(hostname)
tl_want_list=0
tl_list="www.yahoo.fr goserver.gokgs.com www.mpi-forum.org www.bull.com www.lri.com"
tl_added_targets=""
tl_number_of_tests=100
tl_payload_size=40
tl_retry_interval=0.2
tl_imagefile_name="mytl_`date +%F-%Hh%M`_$tl_host.png"
tl_verbose=0
tl_record=""
# Gnuplot things
tl_plot="gnuplot"
# Vizu things
tl_vizu="gthumb"

#########
args="$(getopt "la:n:p:i:f:vuh" "$@")"
eval set -- "$args"
while : ; do
	case "$1" in
	    "-l")	tl_want_list=1; shift;;
	    "-a")	tl_added_targets="$2"; shift 2;;
	    "-n")	tl_number_of_tests="$2"; shift 2;;
	    "-p")	tl_payload_size="$2"; shift 2;;
	    "-i")	tl_retry_interval="$2"; shift 2;;
	    "-f")	tl_imagefile_name="$2"; shift 2;;
	    "-v")	tl_verbose=1; shift ;;
	    "-P")	tl_plot="$2"; shift 2;;
	    "-V")	tl_vizu="$2"; shift 2;;
	    "-u")	usage; exit 1 ;;
	    "-h")	usage; exit 1 ;;
	    "--")	shift ; break ;;
	    *)	echo "$PROG: invalid argument '$1'" ; exit 1 ;;
	esac
done

#########
# Sanity check
tl_dependencies="$tl_plot $tl_vizu"
checkApplications $tl_dependencies

if [ ! -z $tl_added_targets ]; then
    tl_list="$tl_list $tl_added_targets"
fi
tl_nbcand=$(echo $tl_list | wc -w)
if [ $tl_want_list -eq 1 ]; then
    for nb in $(seq 1 $tl_nbcand); do
	echo $tl_list | cut -d" " -f$nb
    done	
fi
gnuplotante=$(echo 'set title "Latency with targets from HOST with PAYLOAD B"; set terminal png; set output "IMAGE"; set xlabel "tries"; set ylabel "ms";'  | sed s/IMAGE/$tl_imagefile_name/g | sed s/HOST/$tl_host/g | sed s/PAYLOAD/$tl_payload_size/g)
gnuplotbase='"DATA" using 1:COLUMN title "TITLE" w linespoints'

# check the distances:
echo "Checking for $tl_nbcand sites, with $tl_number_of_tests tests for each"
for i in $tl_list; do
    # testing first 
    ping -c1 $i &> /dev/null
    if [ ! $? -eq 0 ]; then
	echo -ne "E"
	time=""
	for p in $(seq 1 $tl_number_of_tests); do
	    time="$time 0"
	done
	record="$record @ $time"
    else
	echo -ne "*"
	time=$(ping -i $tl_retry_interval -s $tl_payload_size -c $tl_number_of_tests $i |  grep time\= | sed s/.*time\=//g | sed s/\ ms//g)
	record="$record @ $time"
    fi
done

# Temporary files...
rm -f gnup
rm -f toot
for w in $(seq 1 $tl_number_of_tests); do
    line="$w "
    k=0
    for j in $tl_list; do
	k=$((k+1))
	ch=$(echo $record | cut -d@ -f$((k+1)) | awk '{print $'"$w"'}')
	line="$line $ch"
    done
    print_stuff $line
    echo "$line" >> toot
done

# Gnuplot thing
k=1
myline="plot "
for l in $tl_list; do
    k=$((k+1))
    gnuspecific=$(echo $gnuplotbase |\
	sed s/DATA/toot/g |\
	sed s/COLUMN/$k/g |\
	sed s/TITLE/$l/g)
    if [ $k -eq 2 ]; then
	myline="$myline $gnuspecific"    
    else
	myline="$myline, $gnuspecific"    
    fi
done

echo $gnuplotante > gnup
echo $myline >> gnup
which $tl_plot &> /dev/null
if [ $? -eq 0 ]; then
    gnuplot gnup
else
    echo "Can't plot data, aborting"
    exit 1
fi

which $tl_vizu  &> /dev/null
if [ $? -eq 0 ]; then
    gthumb $tl_imagefile_name
else
    echo "Image dumped in $tl_imagefile_name"
fi

rm -f toot
rm -f gnup

exit 0


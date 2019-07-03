# ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 
# || (C) 2007 by Bull S.A.S.                                                               || 
# ||      Copyright (C) 2006 Bull S.A.S. All rights reserved                               || 
# ||      Bull, Rue Jean Jaures, B.P.68, 78340, Les Clayes-sous-Bois                       || 
# ||      This is not Free or Open Source software. Please contact                         || 
# ||      Bull S.A.S. for details about its license.                                       || 
# ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||| 
#############################################
# Go framework script library
#
# All here is in an absolute path, defined when sourcing the mogob.sh file before anything else.
# Defining MOGOBULL_ROOT is done by sourcing this file at the root of the mogo sources.
#
# 4 parts in the library :
#	+ getting description of the games being selected to play
#	+ launching function to help format the execution on clusters
#	+ monitoring functions to help controlling the game during the play
#	+ misc functions useful for any script
#
# TODO :
#	+ more on statistics to keep tracks of the records of each binaries during its life span
#	+ mpirun/ssh commands and others like this for huge clusters working with RMS

#cd maint/  TODO zep this
if [ -z $MOGOBULL_ROOT ]; then
    echo "Define MOGOBULL_ROOT or die"
    exit 2
fi
libEXEPATH="$MOGOBULL_ROOT/$(grep -A1 "EXE_PATH" $MOGOBULL_ROOT/maint/MOGOBULL_exported_vars.gmk | tail -n1 | cut -d"}" -f2)"
libCREALOG="$MOGOBULL_ROOT/$(grep -A1 "VERSION_LOG" $MOGOBULL_ROOT/maint/MOGOBULL_exported_vars.gmk | tail -n1 | cut -d"}" -f2)"
#cd - &> /dev/null


################################################
# DIVERS FUNCTIONS: 
# + To log anything during the runs
# + To cleanup things on clusters
#
#
function log(){
    echo "$*" >> $gc_logfile
}

function getCoreNumber(){
    corenb=$(cat /proc/cpuinfo | grep "processor.*:" |  wc -l)
    export corenb=$corenb
    return $corenb
}

function leon(){
    # type can be for now "slurm" only
    # TODO: develop the simple ssh/mpirun case
    # and rms case, also later lsf case
    # 
    type=$(echo $gc_killApplic | cut -d' ' -f1)
    applic=$(echo $gc_killApplic | sed s/$type//g)
    ## Slurm case, 
    if [ "$type" == "slurm" ]; then
	for game in $applic; do
	    num=$(squeue | grep $game | awk '{print $1}')
	    if [ "$num" == "" ]; then
		echo "The $game application is already gone"
	    else
		scancel $num
	    fi
	done
    fi

    # If some files need to be cleaned, it's done here
    if [ "$twogoFramework" == "yes" -a ! -z $gc_filepattern ]; then
	mkdir -p $MOGOBULL_ROOT/games/
	mv *$gc_filepattern* $MOGOBULL_ROOT/games/  &> /dev/null 
	if [ $? -eq 0 ]; then
	    cd  $MOGOBULL_ROOT/games; gentwogoOutput $nom_fich_res; cd - &> /dev/null
	fi
    fi
}

################################################
# DESCRIPTION FUNCTIONS for the go games:
# All the following is based on the GNUmakefile build system
# This build system uses configuration files and keeps traces
# of what was produced. We thus can describe binaries according
# to some parameters.
#
# + by name
# + only compet tagged mogos
# + by regex
# + the last produced
function getDescriptionOfMoGo(){
    name="$1"
    md5sum="$2"
    gc_logfile=$libCREALOG
    if [ -z $1 -a -z $2 ]; then
	if [ ! -z "$gc_name" -a ! -z "$gc_md5sum" ]; then
	    name=$gc_name
	    md5sum=$gc_md5sum
	    grep -m1 -A8 -B8 $md5sum $gc_logfile | grep -B2 -A14 $name
	else
	    echo "Can't find this Mogo to describe"
	fi
    fi
}

function getLastMogoEngine(){
    gc_lastone=$(ls -alrt $libEXEPATH/mogo* | tail -n 1)
    export gc_location=$(echo $gc_lastone | awk '{print $9}')
    export gc_name=$(basename $gc_location)
    export gc_md5sum=$(md5sum $gc_location | awk '{print $1}')
}

function getLastCompetitionMogoEngine(){
    gc_lastone=$(ls -alrt $libEXEPATH/mogo*compet* | tail -n 1)
    export gc_location=$(echo $gc_lastone | awk '{print $9}')
    export gc_name=$(basename $gc_location)
    export gc_md5sum=$(md5sum $gc_location | awk '{print $1}')
}

function getRXMogoEngine(){
    regex="$1"
    gc_lastone=$(ls -alrt $libEXEPATH/mogo*$regex* | tail -n 1)
    export gc_location=$(echo $gc_lastone | awk '{print $9}')
    export gc_name=$(basename $gc_location)
    export gc_md5sum=$(md5sum $gc_location | awk '{print $1}')
}

############################
# GNUGO
# + multiple versions of gnugo can be produced : 3.6 and 3.7
# + can put tags as well on it and know ID card of binaries
# + use the GNUmakefile system to produce your binaries
#
# + by name
# + only compet tagged mogos
# + by regex
# + the last produced
THIRDPARTY="$MOGOBULL_ROOT/$(grep THIRD\_PARTY\_SOFTWARE $MOGOBULL_ROOT/maint/MOGOBULL_exported_vars.gmk | head -n1 | cut -d= -f2 | sed s/{MOGOBULL_ROOT}//g | cut -c2-)"
export GNUGOV="$(grep GNUGO\_VERSION $MOGOBULL_ROOT/maint/MOGOBULL_exported_vars.gmk | head -n1 | cut -d= -f2 )"
export GNUGODIR="$THIRDPARTY/gnugo/$GNUGOV/"
export GNUGOEXE="$MOGOBULL_ROOT/$(grep INSTALL\_DIR $MOGOBULL_ROOT/maint/MOGOBULL_exported_vars.gmk | head -n1 |sed s/\${MOGOBULL_ROOT}//g | cut -d= -f2)/bin/"

function getConfigureInformation(){
    gc_XTRA="$*"
    gc_logfile="$THIRDPARTY/gnugo/gnugo_build_log"
    gc_date=`date +%F-%Hh%M`
    gc_line="++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
    gc_intro="+++++++ Logging GnuGo game generation $gc_date ++++++++"

    log $gc_line
    log $gc_intro

# Check for last created game
    gc_lastone=$(ls -alrt $GNUGOEXE/gnugo* | tail -n 1)
    gc_owner=$(echo $gc_lastone | awk '{print $3}')
    gc_date=$(echo $gc_lastone | awk '{print $6$7$8}')
    gc_size=$(echo $gc_lastone | awk '{print $5}')
    export gc_location=$(echo $gc_lastone | awk '{print $9}')
    export gc_name=$(basename $gc_location)
    export gc_md5sum=$(md5sum $gc_location | awk '{print $1}')
    tmp=$(dirname $gc_location)
    gc_name="$gc_name$(echo $gc_date | tr ':' '_')"
    mv $gc_location $tmp/$gc_name
    gc_location="$gc_location$(echo $gc_date | tr ':' '_')"
    gc_machine=`hostname`
    log "gogame : $gc_name"
    log "built  : $gc_owner"
    log "machn  : $gc_machine"
    log "date   : $gc_date"
    log "locate : $gc_location"
    log "size   : $gc_size"
    log "md5sum : $gc_md5sum"
    log "++++++++++"

    gc_CC=$(grep "^CC =" $GNUGODIR/engine/Makefile | head -n1 | cut -d= -f2)
    gc_CF=$(grep "^CFLAGS =" $GNUGODIR/engine/Makefile)
    gc_CXX=$(grep "^CXX =" $GNUGODIR/engine/Makefile)
    gc_CXF=$(grep "^CXXFLAGS =" $GNUGODIR/engine/Makefile)
    gc_LD=$(grep "^LDFLAGS =" $GNUGODIR/engine/Makefile)
    
# Check for compilation values
    log "CC      : $gc_CC"
    log "CFLAGS  : $gc_CF" 
    log "CXX     : $gc_CX"
    log "CXXFGS  : $gc_CXF"
    log "LDFLAGS : $gc_LD"
    log "EXTRA   : $gc_XTRA"
    
    log $gc_line
    log ""
}

function getDescriptionOfGnuGo(){
    name="$1"
    md5sum="$2"
    gc_logfile="$THIRDPARTY/gnugo/gnugo_build_log"
    if [ -z $gc_logfile ]; then
        gc_logfile="$MOGOBULL_ROOT/bin/gnugo_build_log"
    fi

    if [ -z $1 -a -z $2 ]; then
        if [ ! -z "$gc_name" -a ! -z "$gc_md5sum" ]; then
            name=$gc_name
            md5sum=$gc_md5sum
            grep -A8 -B8 $md5sum $gc_logfile | grep -B2 -A10 $name
        else
            echo "Can't find this GnuGo to describe"
        fi
    fi
}

function getRXGNUGOEngine(){
    regex="$1"
    gc_lastone=$(ls -alrt $GNUGOEXE/gnugo*$regex* |grep -v log |  tail -n 1)
   if [ -z "$gc_lastone" ]; then
        gc_lastone=$(ls -alrt $MOGOBULL_ROOT/bin/gnugo*$regex* |grep -v log |  tail -n 1)
   fi
    export gc_location=$(echo $gc_lastone | awk '{print $9}')
    export gc_name=$(basename $gc_location)
    export gc_md5sum=$(md5sum $gc_location | awk '{print $1}')
}

function getLastGNUGOEngine(){
    gc_lastone=$(ls -alrt $GNUGOEXE/gnugo* | grep -v log | tail -n 1)
   if [ -z "$gc_lastone" ]; then
        gc_lastone=$(ls -alrt $MOGOBULL_ROOT/bin/gnugo*$regex* |grep -v log |  tail -n 1)
   fi
    export gc_location=$(echo $gc_lastone | awk '{print $9}')
    export gc_name=$(basename $gc_location)
    export gc_md5sum=$(md5sum $gc_location | awk '{print $1}')
}

################################################
# LAUNCHING GAME FUNCTIONS:
#
# Those functions are used to patch configuration file, so we can keep a skeleton
# always available, patched at the last moment, depending on the demand, the cluster...

function patchConfigFile(){
    fileToPatch="$1"
    numberThreads="$2"
    numberMachines="$3"

    if [ -z "$gc_location" ]; then
	echo "Issue here"
	exit 2
    fi
	
    st=$(echo $gc_location | sed s/\\\//\\\\\\\//g )
    cat $fileToPatch | \
	sed s/MOGOENGINE/$st/g | \
	sed s/GNUGOENGINE/$st/g | \
	sed s/NBTHREADS/$numberThreads/g | \
	sed s/\\\/\\\//\\\//g > /tmp/.mgtmp
    pd=$(echo $PWD | sed s/\\\//\\\\\\\//g )
    cat /tmp/.mgtmp | sed s/$pd\\\///g > $fileToPatch

    if [ ! -z $numberMachines ]; then
	findLauncher $fileToPatch $numberMachines
    fi
}

function findLauncher(){
    gc_launcherString=""    
    gc_configfile="$1"
    gc_number_machine="$2"

    # Checking for different tools on the cluster
    # TODO: put more reliability here. Handle more cases
    
    # 1+ if we find srun, which belongs to slurm, we try to use it
    which srun &> /dev/null
    if [ $? -eq 0 ]; then
	if [ "$kgsFramework" == "yes" ]; then
	application=$(basename $(cat $gc_configfile | grep -v "^#" | grep "engine" | cut -d= -f2 | awk '{print $1}') | cut -c1-8)
	elif [ "$twogoFramework" == "yes" ]; then
	application=$(basename $(cat $gc_configfile | awk '{print $2}') | cut -c1-8)
	fi
	grep ^LAUNCHER $gc_configfile &> /dev/null
	if [ $? -eq 0 ]; then
	    gc_launcherString="srun -N $gc_number_machine $(cat $gc_configfile)"
	    echo $gc_launcherString | sed s/LAUNCHER//g > /tmp/.mgtmp	
	    cat /tmp/.mgtmp > $gc_configfile
	else
	    echo "Do something more clever here"
	fi
	
	# Leon preparation:
	if [ -z "$gc_killApplic" ]; then
	    gc_killApplic="slurm"
	fi
	export gc_killApplic="$gc_killApplic $application"
    fi
    # 2+ check for rms
    # 3+ ssh/mpirun    
    # TODO
}

function findLauncherMachine(){
    gc_launcherString=""    
    gc_number_machine="$1"

    # Checking for different tools on the cluster
    # TODO: put more reliability here. Handle more cases
    # if we find srun, which belongs to slurm, we try to use it

    # 1+ check for slurm
    which srun &> /dev/null
    if [ $? -eq 0 ]; then
	application=$(basename $(echo $gc_location) | cut -c1-8)
	#getCoreNumber
	gc_launcherString="srun -N $gc_number_machine "
	#-c $corenb"
	
	# Leon preparation:
	if [ -z "$gc_killApplic" ]; then
	    gc_killApplic="slurm"
	fi
	export gc_killApplic="$gc_killApplic $application"
	gc_launcherString="$gc_launcherString -J $application"
	export gc_launcherString=$gc_launcherString

    fi
    # 2+ check for rms
    # 3+ ssh/mpirun    
    # TODO
}

# Wait for an interactive answer, allowing to make visual controls
function askToRun(){
    res="$1"
    echo "Want to play with this ?"
    read answer
    case $answer in
	[yY])
		;;	
	*)
		echo "Aborting the game"
		return 2
	;;
    esac
    if [ ! -z "$res" ]; then
	export gc_filepattern="$res"
    fi    
    return 0	
}

# Generate the formatted html datas
function gentwogoOutput(){
    filepattern="$1"
    twogtp -analyze *$filepattern*.dat
}

# Launch the game using the launching string, and the game string
# this fork processes to handle the games
function LaunchGo(){
    check=0
    if [ "$kgsFramework" == "yes" ]; then
	echo "$gc_Stat_Played : Launched my go game...playing"
	$gc_launcherString $gc_gogameString &
    	#for testing $gc_launcherString sleep 20 &>/dev/null &
    elif [ "$twogoFramework" == "yes" ]; then
	`twogtp\
	    -black "$MOGOBULL_ROOT/scripts/$gc_cmd2.game.2.$mydate" \
	    -white "$MOGOBULL_ROOT/scripts/$gc_cmd1.game.1.$mydate" \
	    -size $taille -auto \
	    -sgffile $nom_fich_res \
	    -games $nb_games \
	    -alternate -komi 7.5 -verbose 1 2>&1 | grep games` &
    fi
}


################################################
# PLAYING MONITOR FUNCTIONS:
#
# Those functions are used to monitor the games, and try to control the gogames, the signal
# who's winning, when to stop when too much loosing.
#
# 2 frameworks available now:
# + twogo internal playing
# + kgs external forum
# + TODO cgos ?


if [ "$twogoFramework" == "yes" ]; then
# Version for healthmonitor
GC_LogFile="$PWD/healthState_*"
gc_Order_CheckConnection="Protocol version received, playing game"
gc_Order_OpponentNotReturned="Somebody quit the game"
gc_Order_GameOverScored="Game is over and scored"
gc_Order_GameOverLost1="I have resigned"
gc_Order_GameOverLost2="lostalot"
gc_Order_CleanupOfBoard="Cleaning the board"
fi

if [ "$kgsFramework" == "yes" ]; then
# Version of kgsGTP.jar
GC_LogFile="$PWD/MoGo.log-0*"
gc_Order_CheckConnection="FINEST: Got successful response to command \"list_commands\": = protocol_version"
gc_Order_OpponentNotReturned="Opponent has not returned. Leaving game"
gc_Order_GameOverScored="Game is over and scored"
gc_Order_GameOverLost1="lost game"
gc_Order_GameOverLost2="lostalot"
gc_Order_CleanupOfBoard="Cleaning the board"
fi

# TODO : this should integrate the statistic framework one day
gc_Stat_Won=0
gc_Stat_Lost=0
gc_Stat_Played=0
gc_FirstTime=0

check=0
gc_HealthCount=0

# Spin wheel on screen to know if all is okay on monitoring
function printHealth(){
    modCount=$(echo "$gc_HealthCount % 4" | bc)
    if [ "$modCount" == "0" ]; then
	echo -ne "\b|"
    elif [ "$modCount" == "1" ]; then
	echo -ne "\b/"
    elif [ "$modCount" == "2" ]; then
	echo -ne "\b-"
    elif [ "$modCount" == "3" ]; then
	echo -ne "\b\\"
    fi
}

# Check if the jobs are gone, or if the applications got killed
# Depended of the launching systems
function checkHealth(){
    # Avoid overflow
    if [ $gc_HealthCount -gt 10000000 ]; then
	gc_HealthCount=0
    fi

    # Allowed to check
    if [ $check -eq 0 ]; then
	type=$(echo $gc_killApplic | cut -d' ' -f1)
	applic=$(echo $gc_killApplic | sed s/$type//g)
	gc_HealthCount=$((gc_HealthCount+1))
	
        ## 1+ Slurm case, 
	if [ "$type" == "slurm" ]; then
	    for game in $applic; do
		num=$(squeue | grep $game | awk '{print $1}')
		if [ "$num" == "" ]; then
		    echo
		    echo "The $game application is gone...."
		    return 1
		else
		    printHealth
		    return 0
		fi
	    done
	fi
	## 2+ RMS
	## 3+ ssh/mpirun
    else
	# Still in the error handlers, just wait before lauching a new go game
	sleep 5; LaunchGo; sleep 2;
	check=0	
	return 0
    fi
}

# Got a signal, decided that this means we lost the game
function StoppedGo(){
    echo "Got a SIG, and thus lost a game, cleaning, check condition and start a new go game in a minute"
    lost
    if [ $? -eq 0 ]; then
	echo
	return 0
    else
	return 1
    fi
}

# Little girl Leon told how to kill, lightly
function mathilda(){
    check=1
    type=$(echo $gc_killApplic | cut -d' ' -f1)
    applic=$(echo $gc_killApplic | sed s/$type//g)
    
    ## 1+ Slurm case, 
    if [ "$type" == "slurm" ]; then
	for game in $applic; do
	    num=$(squeue | grep $game | awk '{print $1}')
	    if [ "$num" == "" ]; then
		echo "The $game application is already gone"
	    else
		scancel $num
	    fi
	done
    fi
    # 2+ rms
    # 3+ ssh/mpirun
}

# Cleanup after a game or if an issue arose
function cleanup(){
    check=1
    gc_FirstTime=0
    # Remove the monitoring files
    rm -f healthState*
    mathilda
    #too strong leon
}

# Good to come here
function won(){
    check=1
    gc_Stat_Won=$((gc_Stat_Won+1))
    cleanup
}

# Avoid this part
function lost(){
    check=1
    gc_Stat_Lost=$((gc_Stat_Lost+1))
    cleanup	  
	  # Check if issues
    if [ $gc_Stat_Lost -eq $gc_Stat_LoosingThreshold ]; then 
	echo "Tired of loosing, retiring robot"
	return 1
    else
	echo "Still en forme, launching robot again"
	return 0
    fi
}

# Calling this function can be too much sometimes
# Leon is more like Mr Propre,
# Check in the examples to see when to use this
# (for twogo ok, leon is too strong for kgs games)
function activateSigHandler(){
  ## handle signals
    for signal in SIGINT SIGKILL SIGQUIT SIGILL SIGABRT SIGBUS SIGSEGV SIGTERM
      do
      trap leon $signal
    done
}

function QuitTests(){
	echo "Quitting Tests"
	cleanup
	exit 2
}

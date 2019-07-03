#! /bin/bash
##############################
# Parameters for the tournament
##############################
twogoFramework=yes
gc_Stat_LoosingThreshold=100
gc_Stat_MaxGames=100
gc_WaitingToConnect=2
gc_ConnectionTimeout=2

gc_playAgainstGnugo=yes
gc_playwithLastMogo=yes
gc_watchdog_period=0.2

# twogo parameters
nb_games="$gc_Stat_MaxGames"
taille="9"

# source the library
. $MOGOBULL_ROOT/scripts/sclib.sh

##############################
# Parameters for the tournament
##############################
# Main application
gc_cmd1="mogobot_nonreg" # Name
gc_typeMogo1="threads"	# RegEx to get the good game
gc_nbThreads1="4"	# Threads per machine
gc_nbMachines1="1" 	# Machines needed

# Opponent
if [ $gc_playAgainstGnugo == yes ]; then
gc_cmd2="gnugobot"
gc_typeMogo2=""
gc_nbThreads2="1"
gc_nbMachines2="1"
else
gc_cmd2="mogobot"
gc_typeMogo2="mpi"
gc_nbThreads2="1"
gc_nbMachines2="7"
fi

##############################
# Nothing needed below this line a priori
##############################

function cleanLogs(){
   # Clean up the mess
    if [ ! -z $gc_filepattern ]; then
	mkdir -p $MOGOBULL_ROOT/games/
	mv "$nom_fich_res*" $MOGOBULL_ROOT/games/ &> /dev/null
	if [ $? -eq 0 ]; then
	    cd  $MOGOBULL_ROOT/games; gentwogoOutput $nom_fich_res; cd - &> /dev/null
	fi
    fi
}

mydate=$(date +%F-%Hh%M)
nom_fich_res="resultsTestsNonReg_$gc_cmd1.VS.$gc_cmd2_$mydate"
##############################
# One/ Go Application
##############################
###
if [ $gc_playwithLastMogo == "yes" ]; then
    getRXMogoEngine "$gc_typeMogo1"
else
    getLastMogoEngine
fi
getDescriptionOfMoGo | tee -a $nom_fich_res
# Copy the game inputs in a horodated file, ans patch the file for the game
cp $MOGOBULL_ROOT/scripts/$gc_cmd1 "$MOGOBULL_ROOT/scripts/$gc_cmd1.game.1.$mydate"
chmod +x "$MOGOBULL_ROOT/scripts/$gc_cmd1.game.1.$mydate"
patchConfigFile $MOGOBULL_ROOT/scripts/$gc_cmd1.game.1.$mydate $gc_nbThreads1 $gc_nbMachines1

##############################
# Two/ Opposing Go Application
##############################
###
# Is playing with a GNUGo bot, print it's origin
if [ "$gc_cmd2" == "gnugobot" ]; then
    getLastGNUGOEngine
    getDescriptionOfGnuGo | tee -a $nom_fich_res
else   
# Is playing with a mogobot, print it's origin as well
    getRXMogoEngine "$gc_typeMogo2"
    getDescriptionOfMoGo | tee -a $nom_fich_res
fi
# Copy the game inputs in a horodated file, ans patch the file for the game
cp $MOGOBULL_ROOT/scripts/$gc_cmd2 "$MOGOBULL_ROOT/scripts/$gc_cmd2.game.2.$mydate"
chmod +x "$MOGOBULL_ROOT/scripts/$gc_cmd2.game.2.$mydate"
patchConfigFile $MOGOBULL_ROOT/scripts/$gc_cmd2.game.2.$mydate $gc_nbThreads2 $gc_nbMachines2

##############################
# Game scripts :
# - print what is to be launched
# - Are you sure ?
# - twogtp to make stdin/stdout connections between the 2 applications
cat $MOGOBULL_ROOT/scripts/$gc_cmd1.game.1.$mydate | tee -a $nom_fich_res
cat $MOGOBULL_ROOT/scripts/$gc_cmd2.game.2.$mydate | tee -a $nom_fich_res
askToRun $nom_fich_res;
# Sanity checks, and activate signal handlers 
if [ $? -eq 0 ]; then
    #rm -f $GC_LogFile
    activateSigHandler; # needed to clean things up
    echo "Fire!";
else
    cleanLogs
    exit 2;
fi

# Manage your signal interface like you want,
# Here interrupting means the end of testing
for signal in SIGINT
  do
  trap QuitTests $signal
done

#############################
#############################
# Under this point, can be copy pasted in other tournament scripts...
#############################
#############################

# Get the tournament going, launch the first game
LaunchGo
#############################
# start the monitoring the runs here, from the launching node
# + works with conf : 1 management node and the gogame spread on the cluster
#
#
# Dont work at monitoring if somebody is cleaning or killing
waited=1

while true
do
  # Stop playing if tired of playing. Don't want to mistreat mogo
  if [ $gc_Stat_MaxGames -le $gc_Stat_Played ]; then
      echo "Tired of playing, played all my games"
      if [ $gc_Stat_Played -eq 0 ]; then perc="0.000";
      else perc=$(echo "$gc_Stat_Won / $gc_Stat_Played" | bc -l |  cut -c1-5); fi
      echo "Games Won / Played : $gc_Stat_Won / $gc_Stat_Played ($perc)"
      cleanup
      exit 2
  fi
  
  # Check first the contact...
  if [ $gc_FirstTime -eq 0 -a $check -eq 0 ]; then
      # Wait for the Go server to answer something
      sleep 2
      while true; do
      #
	if [ -f $GC_LogFile ]; then
	  grep "$gc_Order_CheckConnection" $GC_LogFile &> /dev/null
	else
	  echo
	fi	
	  if [ $? -eq 0 ]; then
          # Check is okay, then engage match
	      echo "Go ! ---------"
	      echo "Connected, protocol ok, start a go game"
	      gc_FirstTime=1
	      if [ $gc_Stat_Played -eq 0 ]; then perc="0.000";
	      else perc=$(echo "$gc_Stat_Won / $gc_Stat_Played" | bc -l |  cut -c1-5); fi
	      echo "Games Won / Played : $gc_Stat_Won / $gc_Stat_Played ($perc)"
	      gc_Stat_Played=$((gc_Stat_Played+1))
	      break
	  else
	      if [ $waited -le $gc_WaitingToConnect ]; then
		  waited=$((waited+1))
		  echo "Issue connecting, protocol not ready"
		  sleep $gc_ConnectionTimeout
	      else
		  echo "Connection not responding, moving away"
		  cleanup
		  cleanLogs
		  exit 2
	      fi
	  fi
      done
  fi

  if [ $check -eq 0 ]; then
  # Check if opponent has left the game. If yes, then we won, and we should restart 
      grep "$gc_Order_OpponentNotReturned" $GC_LogFile  &> /dev/null
      if [ $? -eq 0 ]; then
      # Opponent left, stat and cleanup, restart in a minute
	  echo "Opponent decided to quit, stat and start a new go game in a minute"
	  won
  # If the opponent did not leave, check if we scored
      else
      # Did we won or did we lost ?
	  grep "$gc_Order_GameOverScored" $GC_LogFile  &> /dev/null
	  if [ $? -eq 0 ]; then
	  # We won
	      echo "Won a game, cleaning and start a new go game in a minute"
	      won	  
	  fi
	  grep -i "$gc_Order_GameOverLost1" $GC_LogFile  &> /dev/null
	  if [ $? -eq 0 ]; then
	  # We lost
	      echo "Resigned, Lost a game, cleaning, check condition and start a new go game in a minute"
	      lost
	      if [ ! $? -eq 0 ]; then		  
		  exit 1
	      fi
	  fi
	  grep -i "$gc_Order_GameOverLost2" $GC_LogFile  &> /dev/null
	  if [ $? -eq 0 ]; then
	  # We lost
	      echo "Lost a game, cleaning, check condition and start a new go game in a minute"
	      lost
	      if [ ! $? -eq 0 ]; then
		  exit 1
	      fi
	  fi

      fi
      # New game..., probably won, not sure of that here
      grep "$gc_Order_CleanupOfBoard" $PWD/healthState_0 &> /dev/null
      if [ $? -eq 0 ]; then
	  recleaned=$(grep "$gc_Order_CleanupOfBoard" $PWD/healthState_0 | wc -l)
	  if [ $recleaned -gt 2 ]; then
	  # We won
	      echo "Cleanup, what happened ? Cleaning and start a new go game in a minute"
	      won	  
	  fi
      fi
  fi

  # Go health watchdog
  checkHealth
  if [ $? -eq 0 ]; then
      # Do nothing sleep...
      sleep $gc_watchdog_period
  else
      echo "MoGo disappeared ... might be a bug or a timeout in the launching, lost the game"
      lost
      if [ ! $? -eq 0 ]; then
	  exit 1
      fi      
  fi
done

cleanLogs

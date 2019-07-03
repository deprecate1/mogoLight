#! /bin/bash
##############################
# Parameters for the tournament
##############################
kgsFramework=yes
gc_Stat_LoosingThreshold=2
gc_Stat_MaxGames=6
gc_WaitingToConnect=3
gc_ConnectionTimeout=5

gc_watchdog_period=0.2
mydate=$(date +%F-%Hh%M)

# KGS files
KGSconfig="$MOGOBULL_ROOT/scripts/kgsconfigBull19x19"
KGSconfiggame="$MOGOBULL_ROOT/scripts/kgsconfigBull19x19.game_$mydate"
# Go game string for the launching librarie
gc_gogameString="java -jar $MOGOBULL_ROOT/scripts/kgsGtp.jar $KGSconfiggame"

# Source the library
. $MOGOBULL_ROOT/scripts/sclib.sh

##############################
# Go Game Application
##############################
GO_REGEX="mpi"		;# regex
GO_MACHINES="8"		;# machines needed
GO_THREADS="1"		;# threads needed per machine

###
# Infos
echo You are launching mogo in 19x19.
echo totalTime should be 60 times the number in the following line:
grep rules.time $KGSconfig
# Get description of the context
echo "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
getRXMogoEngine $GO_REGEX
getDescriptionOfMoGo
cp $KGSconfig $KGSconfiggame
patchConfigFile $KGSconfiggame $GO_THREADS $GO_MACHINES
findLauncherMachine $GO_MACHINES
cat $KGSconfiggame | grep -v "^#"
echo "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
askToRun; if [ $? -eq 0 ]; then echo "Fire!"; else exit 2; fi
# Get the tournament going
LaunchGo

#export gc_name=$gc_name

# Use this for KGS, activateSiGHandler function for other
for signal in SIGINT
  do
  trap StoppedGo $signal
done

# Monitoring the runs here
waited=1
while true
do
  
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
      # This is not usable since kgsGTP doesn't dump immediately but periodically
	  #grep "$gc_Order_CheckConnection" $GC_LogFile &> /dev/null
	  echo "Go ! ---------"
	  if [ $? -eq 0 ]; then
          # Check is okay, then engage match
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
	      echo "Lost a game, cleaning, check condition and start a new go game in a minute"
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
      # New game..., probably won
      recleaned=$(grep "$gc_Order_CleanupOfBoard" $PWD/healthState_0 | wc -l)
      if [ $recleaned -gt 2 ]; then
	  # We won
	  echo "Cleanup, Won a game, cleaning and start a new go game in a minute"
	  won	  
      fi
      # 
  fi


  # Go health watchdog
  checkHealth
  if [ $? -eq 0 ]; then
      # Do nothing sleep...
      sleep 1
  else
      echo "MoGo disappeared ... might be a bug or a timeout in the launching,, lost the game"
      lost
      if [ ! $? -eq 0 ]; then
	  exit 1
      fi      
  fi
done


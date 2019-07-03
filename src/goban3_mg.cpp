#include "goban_mg.h"
#include "gotools_mg.h"
#include "gobannode_mg.h"
#include "innermcgoplayer_mg.h"
#include "smallpatternsdatabase_mg.h"
#include "gogametree_mg.h"


// #define DEBUG_SHISHO
#define DEBUG_SHISHO_2

void MoGo::Goban::initiateNodeUrgencyForShishoNew( GobanNode * node, GoGameTree *tree ) const {/**assert(0);**/

  for (int i=0;i<(int)stringInformation.stringLiberty.size();i++) {/**assert(0);**/
    if (stringInformation.stringPositions[i].size()==0) continue; // removed string
    int liberty=stringInformation.stringLiberty[i];
    Location oneLocation=stringInformation.stringPositions[i][0];
    bool sameColor=gobanState[oneLocation]==playerColor;



    if (liberty==1 && sameColor) {/**assert(0);**/ // should we try to save this chain, or is it a ladder?
      Location shishoMove=PASSMOVE;
      if (shishoReaderNew(oneLocation, shishoMove)==2) {/**assert(0);**/ // it is a ladder, and we can't escape :'(
#ifdef DEBUG_SHISHO_2
        if (node->fatherNode().size()==0) {/**assert(0);**/
          GoTools::print("Forbid "); textModeShowPosition(shishoMove);GoTools::print("\n");
        }
#endif
        node->nodeUrgency().resetNeverPlayUrgency(shishoMove);
        if (tree) tree->removeChild(node, shishoMove);
      }
    } else if (liberty==2 && (!sameColor)) {/**assert(0);**/// should we try to attack this chain or is it a ladder?
#ifdef DEBUG_SHISHO
      GoTools::print("2 liberty for"); textModeShowPosition(oneLocation);
#endif
      Location nextMove=PASSMOVE, lastLiberty=PASSMOVE;
      int res=getShishoLikeMoveNew(oneLocation,nextMove,lastLiberty);
      if (res==1) {/**assert(0);**/
        node->nodeUrgency().resetNeverPlayUrgency(nextMove);
        if (tree) tree->removeChild(node, nextMove);
#ifdef DEBUG_SHISHO_2
        if (node->fatherNode().size()==0) {/**assert(0);**/
          GoTools::print("Forbid "); textModeShowPosition(nextMove);GoTools::print("\n");
        }
#endif

      } else if (res==2) {/**assert(0);**/
        if (isLegal(nextMove) && (!isSelfAtari(nextMove))) {/**assert(0);**/
          Goban tmpGoban(*this);
          tmpGoban.playMoveOnGoban(nextMove);
          if (tmpGoban.shishoReader(nextMove)==1) {/**assert(0);**/
            node->nodeUrgency().resetNeverPlayUrgency(nextMove);
            if (tree) tree->removeChild(node, nextMove);
#ifdef DEBUG_SHISHO_2
            if (node->fatherNode().size()==0) {/**assert(0);**/
              GoTools::print("Forbid "); textModeShowPosition(nextMove);GoTools::print("\n");
            }
#endif

          }
        }
      }
    }
  }
}









int MoGo::Goban::shishoReaderNew( Location oneStoneOnString, Location & shishoMove ) const {/**assert(0);**/
#ifdef DEBUG_SHISHO
  GoTools::print("shishoReaderNew\n");textModeShowPosition(oneStoneOnString);
#endif
  //   Location savingMove;
  if (stringInformation.doesThisStringAtariOneOther(oneStoneOnString))
    //   if (isUnderAtariStringSavableByEatingMove(oneStoneOnString,savingMove))
    return 1;//then can be saved by eatingstones.


  StoneColor escapingStringColor = gobanState[oneStoneOnString];
  Location lastLiberty = PASSMOVE;
  Location escapingStringHead=stringInformation.getHeadOfOneLibertyString(oneStoneOnString);
#ifdef DEBUG_SHISHO
  GoTools::print("escapingStringHead found\n");textModeShowPosition(escapingStringHead);
  GoTools::print("escapingStringHead = %i\n", escapingStringHead);
#endif
  for (int i=0;i<4;i++)
    if (gobanState[escapingStringHead+directionsArray[i]] == EMPTY) {/**assert(0);**/
      lastLiberty = escapingStringHead+directionsArray[i];
      break;
    }//get the lastLiberty for the under-atari string. only consider to liberty near.//TODO
#ifdef DEBUG_SHISHO
  GoTools::print("\nlastLiberty found\n");textModeShowPosition(lastLiberty);
  GoTools::print("lastLiberty = %i\n", lastLiberty);
#endif

  int liberty = 0;
  for (int i=0;i<4;i++)
    if (gobanState[lastLiberty+directionsArray[i]] == EMPTY) liberty++;
    else if (gobanState[lastLiberty+directionsArray[i]] == escapingStringColor &&
             lastLiberty+directionsArray[i]!=escapingStringHead) // connecting move
      return 0;

  if (liberty != 2)
    return 0;
#ifdef DEBUG_SHISHO
  GoTools::print("shishoSimulator to be lanced!\n");
#endif

  if (shishoSimulator(lastLiberty)) {/**assert(0);**/
    shishoMove = lastLiberty;
#ifdef DEBUG_SHISHO
    GoTools::print("shishoSimulator returns 2!\n");
#endif
    return 2;
  }
  return 1;
}










int MoGo::Goban::getShishoLikeMoveNew( Location oneMoveOnString, Location & nextMove, Location & lastLiberty ) const {/**assert(0);**/
#ifdef DEBUG_SHISHO
  GoTools::print("getShishoLikeMoveNew\n");textModeShowPosition(oneMoveOnString);
#endif
  if (oneMoveOnString == PASSMOVE) return 0;
  //   if (goban->getStringLiberty(oneMoveOnString) != 2) return false;
  assert(getStringLiberty(oneMoveOnString) == 2);


  int strNumber=stringInformation.stringNumber[oneMoveOnString];
  const FastSmallVector<int> &positions=stringInformation.stringPositions[strNumber];
  Location head=PASSMOVE;
  bool doWeAtari=false;
  for (int p=0;p<positions.size() && head==PASSMOVE;p++)
    for (int j=0;j<4;j++) {/**assert(0);**/
      Location tmpLocation=positions[p]+direction(j);

      int state=gobanState[tmpLocation];
      if (state==EMPTY) {/**assert(0);**/
        if (head==PASSMOVE) head=positions[p];
        else if (head!=positions[p]) return 0;
      } else if ((state==BLACK || state==WHITE) && (getStringNumber(tmpLocation)!=strNumber) && getStringLiberty(tmpLocation)==1) {/**assert(0);**/
        // we atari another string
        doWeAtari=true;
      }
    }
  assert(head!=PASSMOVE);

#ifdef DEBUG_SHISHO
  GoTools::print("getShishoLikeMoveNew after head\n");textModeShowPosition(head);
#endif


  FastSmallVector<Location> freeLiberty;
  for (int i=0;i<4;i++)
    if (gobanState[head+direction(i)] == EMPTY) {/**assert(0);**/
      freeLiberty.push_back(head+direction(i));
#ifdef DEBUG_SHISHO
      //   GoTools::print("FreeLiberty\n");textModeShowPosition(freeLiberty[freeLiberty.size()-1]);
#endif

    }
  if ((int)freeLiberty.size() != 2) return false;

  int playerColor=3-gobanState[oneMoveOnString];
  FastSmallVector<Location> shishoLikeMove;
  for (int i=0;i<2;i++) {/**assert(0);**/
    int nextLiberty = 0;//if leave freeLiberty[i] as the last liberty (by playing on the other liberty), then when the under-atari string extends at freeLiberty[i], the liberty it will get.
    for (int j=0;j<4;j++) {/**assert(0);**/
      Location tmpLocation = freeLiberty[i]+direction(j);
      if (tmpLocation == head) continue;
      if (gobanState[tmpLocation] == EMPTY)
        nextLiberty++;
      else if (gobanState[tmpLocation] == 3-playerColor)
        nextLiberty+=getStringLiberty(tmpLocation)-1;
      else if (gobanState[tmpLocation] == playerColor &&
               getStringLiberty(tmpLocation) == 1)
        nextLiberty++;
    }
#ifdef DEBUG_SHISHO
    GoTools::print("FreeLiberty %i\n", nextLiberty);textModeShowPosition(freeLiberty[i]);
#endif
    if (nextLiberty <= 2)
      shishoLikeMove.push_back(freeLiberty[1-i]);
  }
#ifdef DEBUG_SHISHO
  GoTools::print("shishoLikeMove size %d\n",shishoLikeMove.size());
#endif

  if ((int)shishoLikeMove.size() == 0)
    return 0;
  else if (shishoLikeMove.size() == 1) {/**assert(0);**/
    nextMove = shishoLikeMove[0];
    for (int i=0;i<2;i++)
      if (freeLiberty[i]!=nextMove)
        lastLiberty = freeLiberty[i];
  } else {/**assert(0);**/
    Location firstPossibility=shishoLikeMove[0]; int nbLiberties1=0; int nbLibertiesOwnStone1=100;
    for (int i=0;i<4;i++) {/**assert(0);**/
      Location tmpLocation=firstPossibility+direction(i);
      if (gobanState[tmpLocation]==EMPTY) nbLiberties1++;
      else if (gobanState[tmpLocation]==playerColor) nbLibertiesOwnStone1=getStringLiberty(tmpLocation);
    }
    Location secondPossibility=shishoLikeMove[1]; int nbLiberties2=0;int nbLibertiesOwnStone2=100;
    for (int i=0;i<4;i++) {/**assert(0);**/
      Location tmpLocation=secondPossibility+direction(i);
      if (gobanState[tmpLocation]==EMPTY) nbLiberties2++;
      else if (gobanState[tmpLocation]==playerColor) nbLibertiesOwnStone2=getStringLiberty(tmpLocation);
    }
#ifdef DEBUG_SHISHO
    GoTools::print("choose between %i and %i (%i,%i) (%i, %i)\n", firstPossibility, secondPossibility, nbLiberties1, nbLiberties2, nbLibertiesOwnStone1, nbLibertiesOwnStone2);
#endif
    if (nbLiberties1==nbLiberties2) {/**assert(0);**/
      if (nbLibertiesOwnStone1==nbLibertiesOwnStone2) {/**assert(0);**/
        int index = GoTools::mogoRand(2);
        nextMove = shishoLikeMove[index];
        lastLiberty= shishoLikeMove[1-index];
      } else if (nbLibertiesOwnStone1<nbLibertiesOwnStone2) {/**assert(0);**/
        nextMove = shishoLikeMove[0];
        lastLiberty= shishoLikeMove[1];
      } else {/**assert(0);**/
        nextMove = shishoLikeMove[1];
        lastLiberty= shishoLikeMove[0];
      }
    } else if (nbLiberties1<nbLiberties2) {/**assert(0);**/
      nextMove = shishoLikeMove[1];
      lastLiberty= shishoLikeMove[0];
    } else {/**assert(0);**/
      nextMove = shishoLikeMove[0];
      lastLiberty= shishoLikeMove[1];
    }
  }

  if (doWeAtari) {/**assert(0);**/
    for (int j=0;j<4;j++) {/**assert(0);**/
      Location tmpLocation = nextMove+direction(j);
      if (getStringNumber(tmpLocation)==strNumber)
        doWeAtari=false;
    }
  }

  if (doWeAtari)
    return 1;
  else
    return 2;
}


















Location  MoGo::Goban::raceForLiberties(LocationList *&locationListIncludingThisLocation, int thresholdSelfAtari) const {/**assert(0);**/
  locationListIncludingThisLocation=0;
  if ( lastPlayedMove == PASSMOVE) {/**assert(0);**/
    return randomOutOneNotUselessAvoidingSelfAtari(locationListIncludingThisLocation,thresholdSelfAtari);
  }
  Location location = PASSMOVE;
  underAtariStringsFirstPosition.clear();
  bool oneR2=false;
  if (isLastMoveRaceFL(underAtariStringsFirstPosition)) {/**assert(0);**/
#ifdef DEBUG_raceForLiberties
    GoTools::print("LastMoveRaceFL %i\n", underAtariStringsFirstPosition.size());
    textModeShowGoban(lastPlayedMove);
#endif
    int r=saveLastMoveRaceFL(location);
#ifdef DEBUG_raceForLiberties
    GoTools::print("r=%i ", r); textModeShowPosition(location); GoTools::print("\n");
    getchar();
#endif
    if (r==1) return location;
    else if (r==2) oneR2=true;
  }
  

  if (oneR2)
    return randomOutOneNotUselessAvoidingSelfAtari(locationListIncludingThisLocation,thresholdSelfAtari);



  assert(lastPlayedMove != PASSMOVE);

  return -10;
}


int MoGo::Goban::isLastMoveRaceFL(FastSmallVector<Location> &underAtariStringsFirstPosition) const {/**assert(0);**/
  assert(underAtariStringsFirstPosition.size() == 0);
  if (lastPlayedMove == PASSMOVE) return 0;

  const FastSmallVector<int> &positions=stringInformation.stringPositions[getStringNumber(lastPlayedMove)];
  for (int p=0;p<positions.size();p++)
    for (int j=0;j<4;j++) {/**assert(0);**/
      Location tmpLocation=positions[p]+direction(j);
      if (gobanState[tmpLocation] == playerColor &&
          getStringLiberty(tmpLocation) <= raceForLibertiesThreshold
          && getStringLiberty(tmpLocation) >= 2
          && getStringLength(tmpLocation) >= raceForLibertiesLengthThreshold) {/**assert(0);**/
        bool found=false;
        for (int k=0;k<(int)underAtariStringsFirstPosition.size();k++)
          if (getStringNumber(underAtariStringsFirstPosition[k])==getStringNumber(tmpLocation))
            found=true;
        if (!found)
          underAtariStringsFirstPosition.push_back(tmpLocation);
        //return true;
      }
    }
  return underAtariStringsFirstPosition.size();
}


int MoGo::Goban::saveLastMoveRaceFL(Location &savingMove) const {/**assert(0);**/
  // sort the strings in length order
  for (int i=0;i<(int)underAtariStringsFirstPosition.size()-1;i++)
    for (int j=i+1;j<(int)underAtariStringsFirstPosition.size();j++)
      if (stringInformation.getStringLength(underAtariStringsFirstPosition[i]) < stringInformation.getStringLength(underAtariStringsFirstPosition[j])) {/**assert(0);**/
        Location tmp = underAtariStringsFirstPosition[i];
        underAtariStringsFirstPosition[i] = underAtariStringsFirstPosition[j];
        underAtariStringsFirstPosition[j] = tmp;
      }

  // study all the strings
  bool allDead=true;
  FastSmallVector<Location> minLibertyPositions;
  for (int i=0;i<(int)underAtariStringsFirstPosition.size();i++) {/**assert(0);**/
    savingMove = PASSMOVE;
    int strNumber=stringInformation.stringNumber[underAtariStringsFirstPosition[i]];
    const FastSmallVector<int> &positions=stringInformation.stringPositions[strNumber];
    int currentLiberties=stringInformation.stringLiberty[strNumber]; assert(currentLiberties>=2);
    int minOpponentLiberties=1000;
    int friendLiberties=0;
    int maxLibToWin=-1; int secondMaxLibToWin=0;


    interestingMoves.clear();minLibertyPositions.clear();

    for (int j=0;j<(int)positions.size();j++) {/**assert(0);**/
      for (int k=0;k<4;k++) {/**assert(0);**/
        Location tmpLocation=positions[j]+direction(k);

        if ( gobanState[tmpLocation] == 3-playerColor ) {/**assert(0);**/
          int lib=getStringLiberty(tmpLocation);

          if (lib<minOpponentLiberties) {/**assert(0);**/
            minOpponentLiberties=lib;
            minLibertyPositions.clear();minLibertyPositions.push_back(tmpLocation);
          } else if (lib==minOpponentLiberties) {/**assert(0);**/
            minLibertyPositions.push_back(tmpLocation);
          }


        } else if (gobanState[tmpLocation] == playerColor) {/**assert(0);**/
          friendLiberties+=getStringLiberty(tmpLocation)-1;
        } else if (gobanState[tmpLocation] == EMPTY) {/**assert(0);**/
          bool libertyAlreadySeen=false;
          for (int l=0;l<(int)interestingMoves.size();l++) if (interestingMoves[i]==tmpLocation) libertyAlreadySeen=true;
          if (!libertyAlreadySeen) {/**assert(0);**/
            int lib=libertiesToWin(playerColor,strNumber,tmpLocation)-1;
#ifdef DEBUG_raceForLiberties
            GoTools::print("lib %i\n", lib);
#endif
            if (lib>=maxLibToWin) {/**assert(0);**/
              if (maxLibToWin>=0)
                secondMaxLibToWin=maxLibToWin;
              if (lib>maxLibToWin) {/**assert(0);**/
                maxLibToWin=lib;  interestingMoves.clear();
              }
              if (maxLibToWin>0)
                interestingMoves.push_back(tmpLocation);
            } else if (lib>secondMaxLibToWin) secondMaxLibToWin=lib;
            friendLiberties+=GoTools::max(lib,0);
          }
        }
      }
    if (minOpponentLiberties<=1) {/**assert(0);**/ allDead=false; break;} // we already found a weaker string near us
    }
    if (minOpponentLiberties<=1) continue; // look for the next string in danger
#ifdef DEBUG_raceForLiberties
    GoTools::print("secondMaxLibToWin %i, currentLiberties %i (%i), minOpponentLiberties %i\n", secondMaxLibToWin, currentLiberties, secondMaxLibToWin+currentLiberties, minOpponentLiberties);
#endif
    if (secondMaxLibToWin+currentLiberties>raceForLibertiesThreshold) {/**assert(0);**/
      allDead=false; continue;
    } // look for the next string in danger // even with another attack, we can be saved, let's wait


    if (secondMaxLibToWin+currentLiberties>minOpponentLiberties) {/**assert(0);**/       // seems safe, but is only if the string can be attacked without being weaker
      if (oneStringCanBeAttacked(playerColor, minLibertyPositions, strNumber)) {/**assert(0);**/ allDead=false; continue;} // look for the next string in danger // even with another attack, we can be saved, let's wait
    }





    // here we are in danger... try to save the string
    if (minOpponentLiberties>currentLiberties) {/**assert(0);**/ // we can't attack an opponent string...
      if (maxLibToWin>=1) {/**assert(0);**/ // we are saved ?
        if (randomOutOneMoveFromInterestingMovesWithAllChecking(savingMove, interestingMoves))
          return 1;
        else continue;
      } else continue; // we can't increase by a liberty, we are dead... :'(. Look for the next one
    } else {/**assert(0);**/
      //       assert(minOpponentLiberties==currentLiberties);
      // here we have two choices: attack a weak opponent string, or increase liberties
      for (int k=0;k<(int)minLibertyPositions.size();k++) addLibertiesOfString(minLibertyPositions[k], interestingMoves);
      if (randomOutOneMoveFromInterestingMovesWithAllChecking(savingMove, interestingMoves))
        return 1;
    }
  }
  if (allDead) return 2; // none can be saved
  else return 0;
}


int MoGo::Goban::libertiesToWin(int stringColor, int strNumber, Location location) const {/**assert(0);**/
#ifdef DEBUG_raceForLiberties
  textModeShowPosition(location); GoTools::print("libertiesToWin (%i)\n", strNumber);
#endif
  locationInformation.clear();
  locationInformation.lastStringPreviewLocation=-1;
  if (!locationInformation.stringPreview(location, gobanState, directionsArray, stringColor)) {/**assert(0);**/
    locationInformation.lastStringPreviewLocation=-1;
    return 0;
  }
  locationInformation.lastStringPreviewLocation=-1;
  int lib=0;
  for (int i=0;i<(int)locationInformation.deadAdjacentStringLocation.size();i++) lib++;
  locationInformation.deadAdjacentStringLocation.clear();
  for (int i=0;i<(int)locationInformation.freeAdjacentLocation.size();i++) {/**assert(0);**/
    int a=locationInformation.freeAdjacentLocation[i];
    bool found=false;
    for (int l=0;l<(int)locationInformation.deadAdjacentStringLocation.size();l++) if (locationInformation.deadAdjacentStringLocation[l]==a) found=true;
    if (!found) {/**assert(0);**/
      lib++;
      locationInformation.deadAdjacentStringLocation.push_back(a);
    }
  }

  for (int i=0;i<(int)locationInformation.friendAdjacentStringLocation.size();i++) {/**assert(0);**/
    int strNumber=stringInformation.stringNumber[locationInformation.friendAdjacentStringLocation[i]];
    const FastSmallVector<int> &positions=stringInformation.stringPositions[strNumber];

    for (int j=0;j<(int)positions.size();j++) {/**assert(0);**/
      for (int k=0;k<4;k++) {/**assert(0);**/
        int a=positions[j]+direction(k);
        if (gobanState[a]==EMPTY) {/**assert(0);**/
          bool found=false;
          for (int l=0;l<(int)locationInformation.deadAdjacentStringLocation.size();l++) if (locationInformation.deadAdjacentStringLocation[l]==a) found=true;
          if (!found) {/**assert(0);**/
            lib++;
            locationInformation.deadAdjacentStringLocation.push_back(a);
          }
        }
      }
    }
  }

  return lib-stringInformation.stringLiberty[strNumber];






  
}


bool MoGo::Goban::randomOutOneMoveFromInterestingMovesWithAllChecking( Location & location, FastSmallVector< Location > & interestingMoves ) const {/**assert(0);**/
  while (interestingMoves.size() > 0) {/**assert(0);**/
    int index=0;
    if (interestingMoves.size() > 1)
      index=MoGo::GoTools::mogoRand(interestingMoves.size());

    Location candidate=interestingMoves[index];
    Location l=getLargestNearOpponentString(candidate, playerColor);
    bool refused=!isNotUseless(candidate);
    if (!refused) {/**assert(0);**/
      if  (l==PASSMOVE) refused=true;
      if ((!refused) && (!isThisMoveWouldBeAGoodSacrifice(candidate,l)))
        refused=true;
    }

    if (!refused/*isNotUseless(interestingMoves[index]) && (isSelfAtari(interestingMoves[index])<__thresholdSelfAtari) *//*&& (!isLocallyWeakerMove(interestingMoves[index]))*/) {/**assert(0);**/
      location=candidate;
      return true;
    } else {/**assert(0);**/
#ifdef DEBUG_raceForLiberties
      textModeShowPosition(interestingMoves[index]);GoTools::print("refused (%i, %i, %i)\n", isNotUseless(interestingMoves[index]), isSelfAtari(interestingMoves[index])>=__thresholdSelfAtari, isLocallyWeakerMove(interestingMoves[index]));
#endif
      interestingMoves[index]=interestingMoves[interestingMoves.size()-1];
      interestingMoves.pop_back();
    }
  }
  return false;
}


void MoGo::Goban::addLibertiesOfString(Location oneStoneOnString, FastSmallVector<Location> &liberties) const {/**assert(0);**/
  int strNumber=stringInformation.stringNumber[oneStoneOnString];
  const FastSmallVector<int> &positions=stringInformation.stringPositions[strNumber];

  for (int j=0;j<(int)positions.size();j++) {/**assert(0);**/
    for (int k=0;k<4;k++) {/**assert(0);**/
      Location tmpLocation=positions[j]+direction(k);
      if (gobanState[tmpLocation] == EMPTY) {/**assert(0);**/
        liberties.push_back(tmpLocation);
      }
    }
  }
}










bool MoGo::Goban::isLocallyWeakerMove( Location location ) const {/**assert(0);**/
  if (gobanState[location]!=EMPTY)
    return true;
  int minOpponentLiberties=1000;
  int friendLiberties=0;
  for (int j=0;j<4;j++) {/**assert(0);**/
    Location tmpLocation=location+direction(j);
    if ( gobanState[tmpLocation] == 3-playerColor ) {/**assert(0);**/
      int lib=getStringLiberty(tmpLocation);
      if (lib<minOpponentLiberties) minOpponentLiberties=lib;
    } else if (gobanState[tmpLocation] == playerColor) {/**assert(0);**/
      friendLiberties+=getStringLiberty(tmpLocation)-1;
    } else if (gobanState[tmpLocation] == EMPTY)
      friendLiberties++;
  }
  return (minOpponentLiberties<1000 && friendLiberties<minOpponentLiberties);
}
bool MoGo::Goban::isLocallyWeakerMoveForSimulation( Location location ) const {/**assert(0);**/
  if (gobanState[location]!=EMPTY)
    return true;
  int minOpponentLiberties=1000;
  int friendLiberties=0;
  int nbFriendStrings=0;
  for (int j=0;j<4;j++) {/**assert(0);**/
    Location tmpLocation=location+direction(j);
    if ( gobanState[tmpLocation] == 3-playerColor ) {/**assert(0);**/
      int lib=getStringLiberty(tmpLocation);
      if (lib<minOpponentLiberties) minOpponentLiberties=lib;
    } else if (gobanState[tmpLocation] == playerColor) {/**assert(0);**/
      friendLiberties+=getStringLiberty(tmpLocation)-1;
      nbFriendStrings++;
    } else if (gobanState[tmpLocation] == EMPTY)
      friendLiberties++;
  }
  // textModeShowPosition(location);
  // GoTools::print("isLocallyWeakerMoveForSimulation %i, %i\n", friendLiberties, minOpponentLiberties);
  // if (friendLiberties>3) {/**assert(0);**/
  //   return (friendLiberties>minOpponentLiberties);
  // }
  return (friendLiberties<minOpponentLiberties);
}



int MoGo::Goban::oneStringCanBeAttacked( int stringColor, const FastSmallVector< Location > & locationOfStrings, int strNumberAttack ) const {/**assert(0);**/
  for (int i=0;i<(int)locationOfStrings.size();i++) {/**assert(0);**/
    Location oneStoneOnString=locationOfStrings[i];
    int strNumber=stringInformation.stringNumber[oneStoneOnString];
    const FastSmallVector<int> &positions=stringInformation.stringPositions[strNumber];

    bool canBeAttacked=true;
    int nbTmpEyes=0;
    for (int j=0;j<(int)positions.size() && canBeAttacked;j++) {/**assert(0);**/
      for (int k=0;k<4;k++) {/**assert(0);**/
        Location tmpLocation=positions[j]+direction(k);
        if (gobanState[tmpLocation] == EMPTY) {/**assert(0);**/
          if (!isLegal(tmpLocation)) {/**assert(0);**/
            nbTmpEyes++;
            if (nbTmpEyes>=2)
              canBeAttacked=false;
          } else {/**assert(0);**/
            bool nearAttack=false;
            int libSup=0;
            for (int l=0;l<4;l++) {/**assert(0);**/
              Location tmpLocation2=tmpLocation+direction(l);
              if (gobanState[tmpLocation2]==EMPTY)
                libSup++;
              else if (gobanState[tmpLocation2]==stringColor && getStringNumber(tmpLocation2)==strNumberAttack)
                nearAttack=true;
            }
            if (nearAttack && libSup==0) canBeAttacked=false;
          }
        }
      }
    }
    if (canBeAttacked) return 1;
  }
  return 0;
}




int MoGo::Goban::isLastLastMoveRaceFL(FastSmallVector<Location> &underAtariStringsFirstPosition) const {/**assert(0);**/
  assert(underAtariStringsFirstPosition.size() == 0);
  if (lastLastPlayedMove == PASSMOVE) return 0;
  int lib=getStringLiberty(lastLastPlayedMove);
  if (lib<=1)
    return 0;
  else if (lib <= raceForLibertiesThreshold
           && getStringLength(lastLastPlayedMove) >= raceForLibertiesLengthThreshold)
    underAtariStringsFirstPosition.push_back(lastLastPlayedMove);
  //return true;
  return underAtariStringsFirstPosition.size();
}








double MoGo::Goban::successForLocation( Location location ) const {/**assert(0);**/
  assert(location>0);
  int representation=patternRepresentation(location);
  if (useGlobalPatterns==1) location=PASSMOVE;
  else if (useGlobalPatterns==2) location=isOnGobanSide(location)?1:0;
  assert(int(nbSeenPattern.size())>location);
  assert(int(nbSuccessPattern.size())>location);
  assert(nbSuccessPattern[location][representation]<=nbSeenPattern[location][representation]);
  if (nbSeenPattern[location][representation]<10)
    return -1.;
  else
    return double(nbSuccessPattern[location][representation])/double(nbSeenPattern[location][representation]);
}
void MoGo::Goban::updateSuccessForLocation( Location location, int pattern, bool success ) const {/**assert(0);**/
  if (useGlobalPatterns==1) location=PASSMOVE;
  else if (useGlobalPatterns==2) location=isOnGobanSide(location)?1:0;

  if (success)
    Goban::nbSuccessPattern[location][pattern]++;
  Goban::nbSeenPattern[location][pattern]++;
}

int MoGo::Goban::patternRepresentation( Location location ) const {/**assert(0);**/
  if (location==PASSMOVE) return 0;
  int numPattern=0;
  assert(gobanState[location]!=OUTSIDE);
for (int j=0;j<8;j++) {/**assert(0);**/ numPattern+=gobanState[location+direction(j)];numPattern=numPattern<<2; }
  numPattern=numPattern>>1; if (playerColor==2) numPattern++;

  return numPattern;
}



















// #define DEBUG_avoidThisSelfAtariInSimulations
// TEMPORARY COMMENT: THIS FUNCTION IS USED
// TEMPORARY COMMENT: BLACK PLAYS 2 TIMES
bool MoGo::Goban::avoidThisSelfAtariInSimulations( Location move, int thresholdSelfAtari ) const {
#ifdef DEBUG_avoidThisSelfAtariInSimulations
  GoTools::print("hello lo\n");
#endif
  if (avoidSelfAtariMode==0)
    return (isSelfAtari(move)>=thresholdSelfAtari);
  else if (avoidSelfAtariMode==10000) {/**assert(0);**/
    int _selfAtari=isSelfAtari(move);
#ifdef DEBUG_avoidThisSelfAtariInSimulations
    GoTools::print("hello %i\n", _selfAtari);
#endif
    if (_selfAtari>=thresholdSelfAtari) return true;
#ifdef DEBUG_avoidThisSelfAtariInSimulations
    textModeShowGoban();
    textModeShowPosition(move);
#endif
    underAtariStringsFirstPosition.clear();
    if (locationInformation.isBeforeAtariNear(move, underAtariStringsFirstPosition)) {/**assert(0);**/
      int length=0;
      for (int i=0;i<(int)underAtariStringsFirstPosition.size();i++) {/**assert(0);**/
#ifdef DEBUG_avoidThisSelfAtariInSimulations
        textModeShowPosition(underAtariStringsFirstPosition[i]);
        GoTools::print("i=%i\n", i);
        //  showFreeLocation();
#endif
        Location savingMove = PASSMOVE;
        int tmp=locationInformation.isUnderBeforeAtariStringSavableByEatingMove(underAtariStringsFirstPosition[i], savingMove);
        if (tmp>0) {/**assert(0);**/
#ifdef DEBUG_avoidThisSelfAtariInSimulations
          textModeShowPosition(savingMove);
#endif
          //          assert(isLegal(savingMove));
          length=tmp>length?tmp:length;
        }
      }
#ifdef DEBUG_avoidThisSelfAtariInSimulations
      GoTools::print("return %i\n", length);
#endif

      return length>=thresholdSelfAtari;
    }
#ifdef DEBUG_avoidThisSelfAtariInSimulations
    GoTools::print("return false\n");
#endif
    return false;
  } else if (avoidSelfAtariMode==10001) {/**assert(0);**/
    int _selfAtari=isSelfAtari(move);
    if (_selfAtari>=thresholdSelfAtari) return true;
    if (thresholdSelfAtari>=3) //FIXME
      return false;
    Vector<double> *moveValues;
    if (isBlacksTurn())
      moveValues=&moveValuesBlack;
    else
      moveValues=&moveValuesWhite;
    if (moveValues->size()==0)
      return false;
    if ((*moveValues)[move]<successThresholdForNonInterestingMoves)
      return true;
    else
      return false;
  } else if (avoidSelfAtariMode==10002) {/**assert(0);**/
    int _selfAtari=isSelfAtari(move);
    if (_selfAtari<thresholdSelfAtari) return false;
    if (!isOneNearFriendStringOnSide(move, playerColor)) return true;
    Location l=getLargestNearOpponentString(move, playerColor);
    if  (l==PASSMOVE) return true;
    //     if (l==PASSMOVE) {/**assert(0);**/textModeShowPosition(move);textModeShowGoban(); assert(0);}
    int length=getStringLength(l);
    if (length<_selfAtari) return true;
    // now we are near the side, and with a not too big string
    // textModeShowGoban();
    // textModeShowPosition(move); textModeShowPosition(l);
    if (isThisMoveWouldBeAGoodSacrifice(move,l)) return false;
    return true;
  } else if (avoidSelfAtariMode==10003) {/**assert(0);**/
    int _selfAtari=isSelfAtari(move);
    if (_selfAtari>=thresholdSelfAtari) return true;
    if (locationInformation.isUgly(move)) return randDouble()<epsilonGreedy;
    return false;
  } else if (avoidSelfAtariMode==10004) {/**assert(0);**/
    int _selfAtari=isSelfAtari(move);
    if (_selfAtari>=thresholdSelfAtari) return true;
    if (_selfAtari==0) return false;
    if (isAtari(move)) return false;
    //     if (isOnGobanSide(move)) {/**assert(0);**/
    bool friendStoneNear=false;
    for (int i=0;i<4;i++) if (gobanState[move+direction(i)]==playerColor) friendStoneNear=true;
    if (!friendStoneNear) {/**assert(0);**/
      //         GoTools::print("refused\n");
      //         textModeShowGoban();textModeShowPosition(move);
      return true;
    }
    //     }
    return false;
  } else if (avoidSelfAtariMode==10005) {/**assert(0);**/
    int _selfAtari=isSelfAtari(move);
    if (_selfAtari>=thresholdSelfAtari) return true;
    if (_selfAtari==0) return false;
    if (isAtari(move)) return false;
    if (isOnGobanSide(move)) {/**assert(0);**/
      bool friendStoneNear=false;
      for (int i=0;i<4;i++) if (gobanState[move+direction(i)]==playerColor) friendStoneNear=true;
      if (!friendStoneNear) {/**assert(0);**/
        //         GoTools::print("refused\n");
        //         textModeShowGoban();textModeShowPosition(move);
        return true;
      }
    }
    return false;
  } else if (avoidSelfAtariMode==10006) {/**assert(0);**/
    int _selfAtari=isSelfAtari(move);
    if (_selfAtari>=thresholdSelfAtari) return true;
    if (_selfAtari==0) return false;
    if (isAtari(move)) {/**assert(0);**/
      Location l=getLargestNearOpponentString(move, playerColor);
      if (l==PASSMOVE) {/**assert(0);**/
        textModeShowGoban(); textModeShowPosition(move);assert(0);
      }
      if (_selfAtari<getStringLength(l)-1) return false; else return true;
    }
    //          if (isOnGobanSide(move)) {/**assert(0);**/
    bool friendStoneNear=false;
    for (int i=0;i<4;i++) if (gobanState[move+direction(i)]==playerColor) friendStoneNear=true;
    if (!friendStoneNear) {/**assert(0);**/
      //         GoTools::print("refused\n");
      //         textModeShowGoban();textModeShowPosition(move);
      return true;
    }
    //          }
    return false;
  } else if (avoidSelfAtariMode==10007) {/**assert(0);**/
    int _selfAtari=isSelfAtari(move);
    if (_selfAtari>=thresholdSelfAtari) return true;
    //     if (_selfAtari==0) return false;

   
    //          if (isOnGobanSide(move)) {/**assert(0);**/
    bool friendStoneNear=false;
    for (int i=0;i<4;i++) if (gobanState[move+direction(i)]==playerColor) friendStoneNear=true;
    Location l=getLargestNearOpponentString(move, playerColor);
    if (l==PASSMOVE && (!friendStoneNear)) return randDouble()<1.;
    else    if (l==PASSMOVE || (!friendStoneNear)) return randDouble()<0.000;
    else return false;
    // if (isLocallyWeakerMove(move))
    // return randDouble()<0.2;

    //     bool friendStoneNear=false;
    //     for (int i=0;i<4;i++) if (gobanState[move+direction(i)]==playerColor) friendStoneNear=true;
    //     if (!friendStoneNear) {/**assert(0);**/
    //         GoTools::print("refused\n");
    //         textModeShowGoban();textModeShowPosition(move);
    //     return randDouble()<0.8;
    //       return true;
    //     } else
    //     return randDouble()<0.00000;

    //          }
    return false;
  } else if (avoidSelfAtariMode==10008) {/**assert(0);**/
    bool stoneNear=false;bool friendStoneNear=false;

    for (int i=0;i<4;i++)
      if (gobanState[move+direction(i)]==playerColor) {/**assert(0);**/
        stoneNear=true;
        friendStoneNear=true;
      } else if (gobanState[move+direction(i)]==3-playerColor)
        stoneNear=true;

    if (!stoneNear) return true;

    int _selfAtari=isSelfAtari(move);
    if (_selfAtari>=thresholdSelfAtari) return true;


    if (_selfAtari==0) return false;
    if (isAtari(move)) return false;

    if (!friendStoneNear) {/**assert(0);**/
      //         GoTools::print("refused\n");
      //         textModeShowGoban();textModeShowPosition(move);
      return true;
    }
    return false;
  } else if (avoidSelfAtariMode==10009) {/**assert(0);**/
    int _selfAtari=isSelfAtari(move);
    if (_selfAtari>=thresholdSelfAtari) return true;
    if (isAtari(move)) return randDouble()<0.5;
    return false;
  } else if (avoidSelfAtariMode==10010) {/**assert(0);**/
    int _selfAtari=isSelfAtari(move);
    if (_selfAtari>=thresholdSelfAtari) return true;
    return randDouble()<0.8;
    double success=successForLocation(move);
    if (success<0.) return false;
    return randDouble()>success;
  } else if (avoidSelfAtariMode==10011) {/**assert(0);**/
    int _selfAtari=isSelfAtari(move);
    if (_selfAtari>=thresholdSelfAtari) return true;
    if (_selfAtari==0) return false;
    if (isAtari(move)) return false;
    //     if (isOnGobanSide(move)) {/**assert(0);**/
    bool friendStoneNear=false;
    for (int i=0;i<4;i++) if (gobanState[move+direction(i)]==playerColor) friendStoneNear=true;
    if (!friendStoneNear) {/**assert(0);**/
      //         GoTools::print("refused\n");
      //         textModeShowGoban();textModeShowPosition(move);
      return true;
    }
    //     }
    return false;
  } else if (avoidSelfAtariMode==10012) {/**assert(0);**/
    if (isSelfAtari(move)>=thresholdSelfAtari) return true;
    if (lastPlayedMove==PASSMOVE || approximateSizeOfPossibleMoves()<10) return false;
    if (isLocallyWeakerMoveForSimulation(move)) return true;
    return false;
  } else if (avoidSelfAtariMode>0) {/**assert(0);**/
    int _selfAtari=isSelfAtari(move);
    if (_selfAtari<thresholdSelfAtari) return false;

    double v=_selfAtari-thresholdSelfAtari+1;
    double vf=exp(-avoidSelfAtariMode*v/1000.);
    return vf<randDouble();
  } else {/**assert(0);**/
    int x=xIndex(move);
    int y=yIndex(move);
    if (x>heightOfGoban/2) x=heightOfGoban-x;
    if (y>widthOfGoban/2) y=widthOfGoban-y;
    int smallSize=heightOfGoban/4+1;
    if (x<=smallSize && y<=smallSize) return false;
    return (isSelfAtari(move)>=thresholdSelfAtari);
  }
}


bool MoGo::Goban::isSimpleVitalPoint( Location location ) const {/**assert(0);**/
  //   bool isVital=false;
  int nbFree=0; int nbOut=0;int nbBlack=0;int nbWhite=0;

  for (int i=0;i<4;i++) {/**assert(0);**/
    int tmpLocation=location+direction(i);
    if (gobanState[tmpLocation]==EMPTY && i<2) {/**assert(0);**/
      if (gobanState[location+direction(i+2)]==EMPTY) nbFree++;
    } else if (gobanState[tmpLocation]==OUTSIDE) nbOut++;
    else if (gobanState[tmpLocation]==BLACK) nbBlack++;
    else if (gobanState[tmpLocation]==WHITE) nbWhite++;
  }
  for (int i=4;i<8;i++) {/**assert(0);**/
    int tmpLocation=location+direction(i);
    if (gobanState[tmpLocation]==BLACK) nbBlack++;
    else if (gobanState[tmpLocation]==WHITE) nbWhite++;
  }
  return nbFree>=1 && nbOut>=1 && (nbBlack>=3 || nbWhite>=3);
}

Location MoGo::Goban::getSimpleVitalPoint( ) const {/**assert(0);**/
  for (int i=0;i<(int)gobanState.size();i++) {/**assert(0);**/
    if (gobanState[i]==EMPTY && isSimpleVitalPoint(i))
      interestingMoves.push_back(i);
  }
  return interestingMoves.size();
}






void MoGo::Goban::getAllAtariMoves( ) const {/**assert(0);**/
  for (int i=0;i<(int)gobanState.size();i++) {/**assert(0);**/
    if (gobanState[i]==EMPTY && isAtari(i))
      interestingMoves.push_back(i);
  }
}






Location MoGo::Goban::randomOutOneNotUselessAvoidingSelfAtari20154458( LocationList *& locationListIncludingThisLocation, int thresholdSelfAtari, bool andPlay ) const {/**assert(0);**/
  locationListIncludingThisLocation=0;

  assert(updateCloseLiberties);
  removedLocationFromGobanAvoidingSelfAtariFreeLocation.clear();
 
  while(closeLiberties.size()>0) {/**assert(0);**/
    int index=GoTools::mogoRand(closeLiberties.size());
    Location location=closeLiberties[index];
    // GoTools::print("index %i\n",index); textModeShowPosition(location);
    if ((!isNotUseless(location)) || avoidThisSelfAtariInSimulations(location, thresholdSelfAtari)) {/**assert(0);**/
      removedLocationFromGobanAvoidingSelfAtariFreeLocation.push_back(location);
      removeCloseLibertiesByPlayingIndex(index);
      continue;
    }
    if (andPlay) removeCloseLibertiesByPlayingIndex(index);
    for (int i=0;i<(int)removedLocationFromGobanAvoidingSelfAtariFreeLocation.size();i++)
      closeLiberties.push_back(removedLocationFromGobanAvoidingSelfAtariFreeLocation[i]);
    removedLocationFromGobanAvoidingSelfAtariFreeLocation.clear();
    locationListIncludingThisLocation=getLocationListIncludingThisLocationForMovePlay(location);

    return location;
  }
  for (int i=0;i<(int)removedLocationFromGobanAvoidingSelfAtariFreeLocation.size();i++)
    closeLiberties.push_back(removedLocationFromGobanAvoidingSelfAtariFreeLocation[i]);
  removedLocationFromGobanAvoidingSelfAtariFreeLocation.clear();
  return PASSMOVE;
}

























int MoGo::Goban::isInterestingMoveBy3015441( Location location, bool also2015445 ) const {/**assert(0);**/
  //      if (isInterestingMoveBy2015441(location, also2015445)) return 1000;
  assert(allPatternWeights.size()>=20);

  int interests=0;

  if (isOnGobanSide(location)) {/**assert(0);**///FIXME hey, i changed only two lines here.
    if (locationInformation.isYoseOnSide(location)) {/**assert(0);**/
      interests=allPatternWeights[0];
    }
  }
  if (isHane(location))
    interests=allPatternWeights[1];
  else if (isCut(location))
    interests=allPatternWeights[2];
  if (isAtari(location))
    interests=allPatternWeights[3];

  interests+=nbAttach(location)*allPatternWeights[4+0];
  interests+=nbExtend(location)*allPatternWeights[4+1];
  interests+=nbDavidHane(location)*allPatternWeights[4+2];
  interests+=nbBlock(location)*allPatternWeights[4+3];
  interests+=nbConnect(location)*allPatternWeights[4+4];
  interests+=nbTurn(location)*allPatternWeights[4+5];
  interests+=nbTurnConnect(location)*allPatternWeights[4+6];
  interests+=nbDavidCrossCut(location)*allPatternWeights[4+7];
  interests+=blockCut(location)*allPatternWeights[4+8];
  interests+=nbDiagonal(location)*allPatternWeights[4+9];
  interests+=nbShoulderHit(location)*allPatternWeights[4+10];
  interests+=nbOnePointJump(location)*allPatternWeights[4+11];
  interests+=nbKnightMove(location)*allPatternWeights[4+12];
  interests+=nbEmptyTriangle(location)*allPatternWeights[4+13];
  interests+=nbDumpling(location)*allPatternWeights[4+14];
  interests+=nbPush(location)*allPatternWeights[4+15];

  //      GoTools::print("interests %i\n", interests);
  //   if (interests>100) {/**assert(0);**/
  if (also2015445) {/**assert(0);**/
    if (!isLocallyWeakerMoveForSimulation(location))
      return interests;
    else
      return 0;
  } else
    return interests;
  //   } else
  //     return 0;
}






bool MoGo::Goban::isLibertyOfFriend( Location location ) const {/**assert(0);**/
  assert(gobanState[location]==EMPTY);
  for (int i=0;i<4;i++) {/**assert(0);**/
    if (isBlacksTurn() && gobanState[location+direction(i)]==BLACK)
      return true;
    if ((!isBlacksTurn()) && gobanState[location+direction(i)]==WHITE)
      return true;
  }
  return false;
}

bool MoGo::Goban::changeLiberties(Location location, int &changeNbLiberties, int &changeNbStrings) const {/**assert(0);**/
  changeNbStrings=0; changeNbLiberties=0;
  if (!locationInformation.stringPreview(location, gobanState, directionsArray, playerColor))
    return false;
  FastSmallVector<int> uniqueFriend;
  FastSmallVector<int> uniqueEnemy;
  for (int i=0;i<(int)locationInformation.freeAdjacentLocation.size();i++) {/**assert(0);**/
    if (!isLibertyOfFriend(locationInformation.freeAdjacentLocation[i])) changeNbLiberties++;
  }
  for (int i=0;i<(int)locationInformation.deadAdjacentStringLocation.size();i++) {/**assert(0);**/
    locationInformation.enemyAdjacentStringLocation.push_back(locationInformation.deadAdjacentStringLocation[i]);
  }
  for (int i=0;i<(int)locationInformation.friendAdjacentStringLocation.size();i++) {/**assert(0);**/
    int id=stringInformation.getStringNumber(locationInformation.friendAdjacentStringLocation[i]);
    bool found=false;
    for (int j=0;j<(int)uniqueFriend.size();j++)
      if (uniqueFriend[j]==id) found = true;
    if (!found) uniqueFriend.push_back(id);
  }
  for (int i=0;i<(int)locationInformation.enemyAdjacentStringLocation.size();i++) {/**assert(0);**/
    int id=stringInformation.getStringNumber(locationInformation.enemyAdjacentStringLocation[i]);
    bool found=false;
    for (int j=0;j<(int)uniqueEnemy.size();j++)
      found |= (uniqueEnemy[j]==id);
    if (!found) uniqueEnemy.push_back(id);
  }
  changeNbStrings-=uniqueFriend.size()-1;
  //   if (uniqueFriend.size()==0) changeNbStrings++;
  changeNbLiberties-=uniqueFriend.size();
  for (int i=0;i<(int)uniqueEnemy.size();i++) {/**assert(0);**/
    if (getStringLiberty(uniqueEnemy[i])==1) changeNbLiberties+=getStringLength(uniqueEnemy[i]);
  }
  return true;
}
//     FastSmallVector< Location > friendAdjacentStringLocation;
//     FastSmallVector< Location > enemyAdjacentStringLocation;
//     FastSmallVector< Location > deadAdjacentStringLocation;
//     FastSmallVector< Location > freeAdjacentLocation;

int MoGo::Goban::countNbLiberties(int &nbStrings) const {/**assert(0);**/
  nbStrings=0;
  int nb=0;
  for (int i=0;i<(int)stringInformation.stringLiberty.size();i++) {/**assert(0);**/
    if (stringInformation.stringLiberty[i]>0) {/**assert(0);**/
      nb+=stringInformation.stringLiberty[i];
      nbStrings++;
    }
  }
  return nb;
}





Location MoGo::Goban::randomOutOneMoveMode10000(int thresholdSelfAtari) const {/**assert(0);**/
  LocationList *locationListIncludingThisLocation=0;
  Location l=coreOfRandomMode44(locationListIncludingThisLocation, thresholdSelfAtari);
  if (l>=0) return l;

  assert(lastPlayedMove != PASSMOVE);

  interestingMoves.clear();
  Location location = PASSMOVE;
  for (int i=0;i<8;i++) {/**assert(0);**/
    //     location = liberties[i];
    location = lastPlayedMove + direction(i);
    if (isInterestingMoveBy2015441(location, true))
      interestingMoves.push_back(location);
  }



  while (interestingMoves.size() > 0) {/**assert(0);**/
    int index=0;
    if (interestingMoves.size() > 1)
      index=MoGo::GoTools::mogoRand(interestingMoves.size());
    if (isNotUseless(interestingMoves[index]) && (!isSelfAtari(interestingMoves[index]))) {/**assert(0);**/
      locationListIncludingThisLocation=getLocationListIncludingThisLocationForMovePlay(interestingMoves[index]);
      return interestingMoves[index];
    } else {/**assert(0);**/
      interestingMoves[index]=interestingMoves[interestingMoves.size()-1];
      interestingMoves.pop_back();
    }
  }


  double max=-10.; Location bestMove=PASSMOVE;
  for (int i=0;i<1000;i++) {/**assert(0);**/
    Location location=randomOutOneNotUselessAvoidingSelfAtari(locationListIncludingThisLocation, thresholdSelfAtari);
    double value=distanceToNearestString(location);
    if (value>max) {/**assert(0);**/
      bestMove=location;
      max=value;
    }
  }

  return bestMove;
}












void MoGo::Goban::addStringPositionsToFrontWithDistance(Location locationOnString, double refDistance, std::priority_queue<double> &fifo, Vector<double> &distances) const {/**assert(0);**/
  if (gobanState[locationOnString]==EMPTY) {/**assert(0);**/
    if (distances[locationOnString] < 0.) {/**assert(0);**/
      distances[locationOnString]=refDistance;
      fifo.push(makeUglyPriority(locationOnString, refDistance));
    }
  } else if (gobanState[locationOnString]!=OUTSIDE) {/**assert(0);**/
    const FastSmallVector<int> &positions=stringInformation.stringPositions[getStringNumber(locationOnString)];
    for (int p=0;p<positions.size();p++) {/**assert(0);**/
      Location tmpLocation=positions[p];
      if (distances[tmpLocation] < 0.) {/**assert(0);**/
        distances[tmpLocation]=refDistance;
        fifo.push(makeUglyPriority(tmpLocation, refDistance));
      }
    }
  }
}

Vector<double> MoGo::Goban::distanceByGroupsFromTwoReferences(Location reference1, Location reference2, double coefDistance) const {/**assert(0);**/
  assert(reference1>PASSMOVE);
  assert(reference2>PASSMOVE);

  Vector<double> distances((height()+2)*(width()+2), -1.);
  std::priority_queue<double> fifo;

  addStringPositionsToFrontWithDistance(reference1, 0., fifo, distances);
  addStringPositionsToFrontWithDistance(reference2, coefDistance, fifo, distances);

  while (!fifo.empty()) {/**assert(0);**/
    Location location=getUglyLocation(fifo.top()); fifo.pop();
    double distance=distances[location];
    for (int i=0;i<4;i++) {/**assert(0);**/
      Location tmpLocation=location+direction(i);
      double tmpDistance=distance+1.;
      //        if (gobanState[location]==3-gobanState[tmpLocation]
      /*&& getStringLiberty(location) >= getStringLiberty(tmpLocation)-2*/
      /* && (getStringLiberty(tmpLocation)<4 || getStringLength(tmpLocation)<=3)*/
      //           && getStringLength(tmpLocation)<=3
      //           ) tmpDistance-=0.5;
      //               if (gobanState[location]!=EMPTY && gobanState[tmpLocation]==EMPTY) tmpDistance-=0.8;
      //               if (gobanState[location]==EMPTY && gobanState[tmpLocation]!=EMPTY) tmpDistance-=0.8;
      addStringPositionsToFrontWithDistance(tmpLocation, tmpDistance, fifo, distances);
    }
  }
  return distances;
}


Vector<double> MoGo::Goban::distanceByGroupsFromReference(Location reference) const {/**assert(0);**/
  if (reference==PASSMOVE)
    return Vector<double>((height()+2)*(width()+2), 1.);

  Vector<double> distances((height()+2)*(width()+2), -1.);
  std::priority_queue<double> fifo;

  addStringPositionsToFrontWithDistance(reference, 0., fifo, distances);

  while (!fifo.empty()) {/**assert(0);**/
    Location location=getUglyLocation(fifo.top()); fifo.pop();
    double distance=distances[location];
    for (int i=0;i<4;i++) {/**assert(0);**/
      Location tmpLocation=location+direction(i);
      double tmpDistance=distance+1.;
      // if (i>=4) tmpDistance+=0.41;
      //        if (gobanState[location]==3-gobanState[tmpLocation]
      /*&& getStringLiberty(location) >= getStringLiberty(tmpLocation)-2*/
      /* && (getStringLiberty(tmpLocation)<4 || getStringLength(tmpLocation)<=3)*/
      //           && getStringLength(tmpLocation)<=3
      //           ) tmpDistance-=0.5;
      //               if (gobanState[location]!=EMPTY && gobanState[tmpLocation]==EMPTY) tmpDistance-=0.8;
      //               if (gobanState[location]==EMPTY && gobanState[tmpLocation]!=EMPTY) tmpDistance-=0.8;
      addStringPositionsToFrontWithDistance(tmpLocation, tmpDistance, fifo, distances);
    }
  }
  return distances;
}




double MoGo::Goban::distanceByGroupsToLastMove( Location location, int &nbFreeLocations ) const {/**assert(0);**/
  return distanceByGroups(location, lastMove(), nbFreeLocations);
}

double MoGo::Goban::distanceByGroupsToLastLastMove( Location location, int &nbFreeLocations ) const {/**assert(0);**/
  return distanceByGroups(location, lastLastMove(), nbFreeLocations);
}


double MoGo::Goban::distanceByGroups( Location location, Location reference, int &nbFreeLocations ) const {/**assert(0);**/
  nbFreeLocations=0;
  if (reference==PASSMOVE) return -1.;
  if (location==PASSMOVE) return -2.;

  Vector<double> distances=distanceByGroupsFromReference(reference);
  for (int i=0;i<(int)gobanState.size();i++) {/**assert(0);**/
    if (gobanState[i]==EMPTY && distances[i]<=distances[location])
      nbFreeLocations++;
  }
  return distances[location];
}

double MoGo::Goban::makeUglyPriority(Location location, double distance) {/**assert(0);**/
  // GoTools::print("%i %f = %f\n", location, distance, floor(distance*1024.)+double(location)/1024.);
  return floor((40.-distance)*1024.)+double(location)/1024.;
}
double MoGo::Goban::getUglyDistance(double priority) {/**assert(0);**/
  return 40.-floor(priority)/1024.;
}
Location MoGo::Goban::getUglyLocation(double priority) {/**assert(0);**/
  // GoTools::print("priority %f (%i)\n",priority, Location(floor((priority-floor(priority))*1024.)));
  return Location(floor((priority-floor(priority))*1024.));
}


double MoGo::Goban::distanceByGroupsTo2LastMoves(Location location, int &nbFreeLocations, double coefDistance) const {/**assert(0);**/
  nbFreeLocations=0;
  Location reference=lastMove();
  Location reference2=lastLastMove();
  if (location==PASSMOVE) return -2.;
  if (reference==PASSMOVE) return -1.;

  if (reference2==PASSMOVE) {/**assert(0);**/
    return distanceByGroups(location, reference, nbFreeLocations);
  }

  Vector<double> distances=distanceByGroupsFromTwoReferences(reference, reference2, coefDistance);
  for (int i=0;i<(int)gobanState.size();i++) {/**assert(0);**/
    if (gobanState[i]==EMPTY && distances[i]<=distances[location])
      nbFreeLocations++;
  }
  return distances[location];
}











double MoGo::Goban::temperatureOfLastMove() const {/**assert(0);**/
  Location reference=lastPlayedMove;
  if (reference==PASSMOVE) return 0.;

  Vector<double> distances=distanceByGroupsFromReference(reference);
  double temperature=0., sum=0.;
  for (int i=0;i<(int)gobanState.size();i++) {/**assert(0);**/
    double distanceValue=1./(pow((distances[i]+4.),2.)/25.);
    if (gobanState[i]==playerColor) {/**assert(0);**/
      temperature+=distanceValue;
    } else if (gobanState[i]==3-playerColor) {/**assert(0);**/
      temperature+=distanceValue*0.5;
    }
    if (gobanState[i]!=OUTSIDE)
      sum+=distanceValue;
  }
  return temperature/(sum+1e-5);
}




long long MoGo::Goban::locationPattern5x5(const Location location) const {/**assert(0);**/
  //   long long representation=1<<50;
  long long representation=(1LL<<50LL);
  for (int sym=0;sym<8;sym++) {/**assert(0);**/
    long long tmpRepresentation=0;
    for (int i=0;i<25;i++) {/**assert(0);**/
      Location tmpLocation=location+directionT25[sym][i];
      //        Location tmpLocation=location+locationInformation.directionT[sym][i];

      int state=OUTSIDE;
      if (tmpLocation>=0 && tmpLocation<(int)gobanState.size()) {/**assert(0);**/
        state=gobanState[tmpLocation];
        if (state==playerColor) state=BLACK;
        else if (state==3-playerColor) state=WHITE;
      }
      tmpRepresentation=(tmpRepresentation<<2LL)+state;
    }
    if (tmpRepresentation<representation) representation=tmpRepresentation;
  }
  return representation;
}

long long MoGo::Goban::locationPattern5x5(const Location location, Vector<long long> &allSymetries) const {/**assert(0);**/
  //   long long representation=1<<50;
  allSymetries.clear();
  long long representation=(1LL<<50LL);
  for (int sym=0;sym<8;sym++) {/**assert(0);**/
    long long tmpRepresentation=0;
    for (int i=0;i<25;i++) {/**assert(0);**/
      Location tmpLocation=location+directionT25[sym][i];
      //        Location tmpLocation=location+locationInformation.directionT[sym][i];

      int state=OUTSIDE;
      if (tmpLocation>=0 && tmpLocation<(int)gobanState.size()) {/**assert(0);**/
        state=gobanState[tmpLocation];
        if (state==playerColor) state=BLACK;
        else if (state==3-playerColor) state=WHITE;
      }
      tmpRepresentation=(tmpRepresentation<<2LL)+state;
    }
    allSymetries.push_back(tmpRepresentation);
    if (tmpRepresentation<representation) representation=tmpRepresentation;
  }
  return representation;
}








long long MoGo::Goban::locationPattern5x5One( const Location location ) const {/**assert(0);**/
  long long tmpRepresentation=0;
  for (int i=0;i<25;i++) {/**assert(0);**/
    Location tmpLocation=location+directionT25[0][i];
    //        Location tmpLocation=location+locationInformation.directionT[sym][i];

    int state=OUTSIDE;
    if (tmpLocation>=0 && tmpLocation<(int)gobanState.size()) {/**assert(0);**/
      state=gobanState[tmpLocation];
      if (state==playerColor) state=BLACK;
      else if (state==3-playerColor) state=WHITE;
    }
    tmpRepresentation=(tmpRepresentation<<2LL)+state;
  }
  return tmpRepresentation;
}

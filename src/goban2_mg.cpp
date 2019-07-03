#include "goban_mg.h"
#include "gotools_mg.h"
#include "randomgoplayer_mg.h"
#include "gobannode_mg.h"
#include "goexperiments_mg.h"
#include "innermcgoplayer_mg.h"
#include "yadtinterface_mg.h"

using namespace MoGo;
Location MoGo::Goban::coreOfRandomMode44(LocationList *& locationListIncludingThisLocation, int thresholdSelfAtari) const {/**assert(0);**/
  locationListIncludingThisLocation=0;
  if ( lastPlayedMove == PASSMOVE) {/**assert(0);**/
    return randomOutOneNotUselessAvoidingSelfAtari(locationListIncludingThisLocation,thresholdSelfAtari);
  }
  Location location = PASSMOVE;
  underAtariStringsFirstPosition.clear();
  if (locationInformation.isLastMoveAtari(underAtariStringsFirstPosition))
    if (saveLastMoveAtari(location)) return location;
    else return randomOutOneNotUselessAvoidingSelfAtari(locationListIncludingThisLocation,thresholdSelfAtari);

  
  assert(lastPlayedMove != PASSMOVE);
  interestingMoves.clear();
  interestingMovesProbabilities.clear();

  return -10;
}
Location MoGo::Goban::coreOfRandomMode44Small(LocationList *& locationListIncludingThisLocation, int ) const {/**assert(0);**/
  locationListIncludingThisLocation=0;
  if ( lastPlayedMove == PASSMOVE) {/**assert(0);**/
    return -5;
  }
  Location location = PASSMOVE;
  underAtariStringsFirstPosition.clear();
  if (locationInformation.isLastMoveAtari(underAtariStringsFirstPosition))
    if (saveLastMoveAtari(location)) return location;
    else {/**assert(0);**/
      locationListIncludingThisLocation=0;
      return -5;
    }

  assert(lastPlayedMove != PASSMOVE);
  interestingMoves.clear();
  interestingMovesProbabilities.clear();

  return -10;
}



Location MoGo::Goban::coreOfRandomMode44_6(LocationList *& locationListIncludingThisLocation, int thresholdSelfAtari) const {/**assert(0);**/
  locationListIncludingThisLocation=0;
  if ( lastPlayedMove == PASSMOVE) {/**assert(0);**/
    return randomOutOneNotUselessAvoidingSelfAtari(locationListIncludingThisLocation,thresholdSelfAtari);
  }
  Location location = PASSMOVE;
  underAtariStringsFirstPosition.clear();
  if (locationInformation.isLastMoveAtari(underAtariStringsFirstPosition) /*&& getStringLength(underAtariStringsFirstPosition[0])>1*/) {/**assert(0);**/
    if (saveLastMoveAtari(location)) return location;
    else return randomOutOneNotUselessAvoidingSelfAtari(locationListIncludingThisLocation,thresholdSelfAtari);
  }
  /*    Location tmp = PASSMOVE;
  if (shishoLikeMoveForced &&
        locationInformation.getShishoLikeMove(location,tmp) &&
        isInRangeLocation(location) &&
        isLegal(location)) return location;*/
  assert(lastPlayedMove != PASSMOVE);
  interestingMoves.clear();
  interestingMovesProbabilities.clear();

  return -10;
#if 0
  locationListIncludingThisLocation=0;
  if ( lastPlayedMove == PASSMOVE) {/**assert(0);**/
    return randomOutOneNotUselessAvoidingSelfAtariBigEatsPrefered(locationListIncludingThisLocation,thresholdSelfAtari);
  }
  Location location = PASSMOVE;
  underAtariStringsFirstPosition.clear();
  if (locationInformation.isLastMoveAtari(underAtariStringsFirstPosition))
    if (saveLastMoveAtari(location) && (!avoidThisSelfAtariInSimulations(location, thresholdSelfAtari))) return location;
    else return randomOutOneNotUselessAvoidingSelfAtariBigEatsPrefered(locationListIncludingThisLocation,thresholdSelfAtari);
  /*    Location tmp = PASSMOVE;
  if (shishoLikeMoveForced &&
        locationInformation.getShishoLikeMove(location,tmp) &&
        isInRangeLocation(location) &&
        isLegal(location)) return location;*/
  assert(lastPlayedMove != PASSMOVE);
  interestingMoves.clear();
  interestingMovesProbabilities.clear();

  return -10;
#endif
}
Location MoGo::Goban::coreOfRandomMode44_7( LocationList *& locationListIncludingThisLocation, int thresholdSelfAtari ) const {/**assert(0);**/
  locationListIncludingThisLocation=0;
  if ( lastPlayedMove == PASSMOVE) {/**assert(0);**/
    return randomOutOneNotUselessAvoidingSelfAtari(locationListIncludingThisLocation,thresholdSelfAtari);
  }
  Location location = PASSMOVE;
  underAtariStringsFirstPosition.clear();
  if (locationInformation.isLastMoveAtari(underAtariStringsFirstPosition)) {/**assert(0);**/
    int t=saveLastMoveAtariSize(location);
    if (t>0 /*&& (!isLocallyWeakerMove(location))*//*&& (!avoidThisSelfAtariInSimulations(location, t+1)*/) {/**assert(0);**/
      return location;
    } else return randomOutOneNotUselessAvoidingSelfAtari(locationListIncludingThisLocation,thresholdSelfAtari);
  }

  location = PASSMOVE;
  underAtariStringsFirstPosition.clear();
  if (locationInformation.isBeforeAtariNear(lastPlayedMove, underAtariStringsFirstPosition)) {/**assert(0);**/
    for (int i=0;i<(int)underAtariStringsFirstPosition.size();i++) {/**assert(0);**/
      Location savingMove = stringInformation.getOneLibertyOfString(underAtariStringsFirstPosition[i]);
      int t=0;
      if( isInRangeLocation(savingMove) &&
          isNotUseless(savingMove) &&
          isAtariSavingMoveTrue(savingMove))
        t=1;
      int free=0;
      for (int j=0;j<4;j++)
        if (gobanState[savingMove+direction(j)]==EMPTY) free++;
     
      if (t>0 && free>=2 /*&& (!isLocallyWeakerMove(location))*//*&& (!avoidThisSelfAtariInSimulations(location, t+1)*/) {/**assert(0);**/
        //  GoTools::print("we play ");
        //  textModeShowGoban(lastPlayedMove);
        //  textModeShowPosition(savingMove);
        return savingMove;
      }
    }
  }


 
  assert(lastPlayedMove != PASSMOVE);
  interestingMoves.clear();
  interestingMovesProbabilities.clear();

  return -10;
}


void MoGo::Goban::getInterestingMovesWithFinder( InterestingMoveFinder * finder) const {/**assert(0);**/
  if (use25BlocksForGP==10) {/**assert(0);**/
    for (int i=0;i<20;i++) {/**assert(0);**/
      Location location = lastPlayedMove + direction(i);
      if ((location<0) || (location >= (int)gobanState.size()))
        continue;
      if (gobanState[location] == EMPTY &&
          isInRangeLocation(location)) {/**assert(0);**/
        int isa = isSelfAtari(location);
        if (isa) continue;
        smallestNeighborhood=getLocationRepresentation(location,5);
        //         GoTools::print(smallestNeighborhood);
        if (finder) {/**assert(0);**/
          if (finder->isInteresting(smallestNeighborhood))
            interestingMoves.push_back(location);
        } else assert(0);
      }
    }
    //     if (interestingMoves.size()==0)
    //       GoTools::print("random\n");
    // else
    //       GoTools::print("lastPlayedMove %i\n", lastPlayedMove);

  } else if  (use25BlocksForGP==11) {/**assert(0);**/
    for (int i=0;i<20;i++) {/**assert(0);**/
      Location location = lastPlayedMove + direction(i);
      if ((location<0) || (location >= (int)gobanState.size()))
        continue;
      if (gobanState[location] == EMPTY &&
          isInRangeLocation(location)) {/**assert(0);**/
        int isa = isSelfAtari(location);
        if (isa) continue;
        smallestNeighborhood=getLocationRepresentation2(location,5);
        //         GoTools::print(smallestNeighborhood);
        if (finder) {/**assert(0);**/
          if (finder->isInteresting(smallestNeighborhood))
            interestingMoves.push_back(location);
        } else assert(0);
      }
    }
    //     if (interestingMoves.size()==0)
    //       GoTools::print("random\n");
    // else
    //       GoTools::print("lastPlayedMove %i\n", lastPlayedMove);
  } else if  (use25BlocksForGP==12) {/**assert(0);**/
    for (int i=0;i<20;i++) {/**assert(0);**/
      Location location = lastPlayedMove + direction(i);
      if ((location<0) || (location >= (int)gobanState.size()))
        continue;
      if (gobanState[location] == EMPTY &&
          isInRangeLocation(location)) {/**assert(0);**/
        int isa = isSelfAtari(location);
        if (isa) continue;
        smallestNeighborhood=getLocationRepresentation2(location,3);
        //         GoTools::print(smallestNeighborhood);
        if (finder) {/**assert(0);**/
          if (finder->isInteresting(smallestNeighborhood))
            interestingMoves.push_back(location);
        } else assert(0);
      }
    }
    //     if (interestingMoves.size()==0)
    //       GoTools::print("random\n");
    // else
    //       GoTools::print("lastPlayedMove %i\n", lastPlayedMove);
  } else if  (use25BlocksForGP==13) {/**assert(0);**/
    for (int i=0;i<(int)gobanState.size();i++) {/**assert(0);**/
      Location location = i;
      if (gobanState[location] == EMPTY &&
          isInRangeLocation(location)) {/**assert(0);**/
        int isa = isSelfAtari(location);
        if (isa) continue;
        smallestNeighborhood=getLocationRepresentation2(location,5);
        //         GoTools::print(smallestNeighborhood);
        if (finder) {/**assert(0);**/
          if (finder->isInteresting(smallestNeighborhood))
            interestingMoves.push_back(location);
        } else assert(0);
      }
    }
    //     if (interestingMoves.size()==0)
    //       GoTools::print("random\n");
    // else
    //       GoTools::print("lastPlayedMove %i\n", lastPlayedMove);
  } else {/**assert(0);**/

    if (!use25BlocksForGP) {/**assert(0);**/
      if (useLibertiesForGP)
        getInterestingMovesNewWithLiberties(finder, 100000);
      else
        getInterestingMovesNew(finder, 100000);
    } else {/**assert(0);**/
      if (useLibertiesForGP)
        getInterestingMovesNew25WithLiberties(finder, 100000);
      else
        getInterestingMovesNew25(finder, 100000);
    }
  }
}

void MoGo::Goban::getInterestingMovesWithFinder1() const {/**assert(0);**/
  getInterestingMovesWithFinder(finder1);
}
void MoGo::Goban::getInterestingMovesWithFinder2() const {/**assert(0);**/
  getInterestingMovesWithFinder(finder2);
}















void MoGo::Goban::getInterestingMovesNew(InterestingMoveFinder *finder, int newMode) const {/**assert(0);**/
  if (smallestNeighborhood.size()!=8)
    smallestNeighborhood.resize(8);
#ifdef DEBUG_GOBAN_SYMETRIES
  textModeShowGoban();
#endif
  for (int i=0;i<8;i++) {/**assert(0);**/
    Location location = lastPlayedMove + direction(i);
    if (gobanState[location] == EMPTY &&
        isInRangeLocation(location)) {/**assert(0);**/
#ifdef DEBUG_GOBAN_SYMETRIES
      Goban t(*this);
      if (isNotUseless(location)) {/**assert(0);**/
        t.playMoveOnGoban(location);
        t.textModeShowGoban(location);
      }
#endif
      int isa = isSelfAtari(location);
      if (isa) continue;
      for (int j=0;j<8;j++)
        smallestNeighborhood[j]=5;

      for (int j=0;j<8;j++) {/**assert(0);**/
#ifdef DEBUG_GOBAN_SYMETRIES
        GoTools::print("Symetry %i\n",j);
#endif
        bool isSmaller=false;
        int c=0;
        for (int k=0;k<9;k++) {/**assert(0);**/
#ifdef DEBUG_GOBAN_SYMETRIES
          if (k%3==0)
            GoTools::print("\n");
#endif
          if (k!=CENTER_)  {/**assert(0);**/
            int s=gobanState[location+locationInformation.directionT[j][k]];
            if (s==EMPTY)
              s=0;
            else if (s==playerColor)
              s=1;
            else if (s==3-playerColor)
              s=2;
            else if (s==OUTSIDE)
              s=3;
            else
              assert(0);
            if (isSmaller || (s<smallestNeighborhood[c])) {/**assert(0);**/
              smallestNeighborhood[c]=s;
              isSmaller=true;
            } else if (s>smallestNeighborhood[c])
              break;
            c++;
          }
#ifdef DEBUG_GOBAN_SYMETRIES
          if (k!=CENTER_)
            GoTools::print("%i ",smallestNeighborhood[c-1]);
          else
            GoTools::print("%c ",'X');
#endif

        }
#ifdef DEBUG_GOBAN_SYMETRIES
        GoTools::print("\n");
#endif

      }
#ifdef DEBUG_GOBAN_SYMETRIES
      int c=0;
      GoTools::print("\n");
      for (int k=0;k<9;k++) {/**assert(0);**/
        if (k%3==0)
          GoTools::print("\n");
        if (k!=CENTER_)  {/**assert(0);**/
          c++;
        }
        if (k!=CENTER_)
          GoTools::print("%i ",smallestNeighborhood[c-1]);
        else
          GoTools::print("%c ",'X');
      }
      GoTools::print("\n");
      getchar();
#endif
      if (finder) {/**assert(0);**/
        if (finder->isInteresting(smallestNeighborhood))
          interestingMoves.push_back(location);
      } else {/**assert(0);**/
        if (isNewInterestingMoves(smallestNeighborhood, newMode))
          interestingMoves.push_back(location);
      }
    }
  }
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::Goban::createRotationArray25( ) {
  int centerx=heightOfGoban/2;
  int centery=widthOfGoban/2;
  Location center=indexOnArray(centerx,centery);
  int c=0;
  directionT25.resize(8);
  for (int i=0;i<8;i++)
    directionT25[i].resize(25);
  for (int i=-2;i<=2;i++) {
    for (int j=-2;j<=2;j++) {
      Location move=indexOnArray(centerx+i,centery+j);
      directionT25[0][c]=move-center;
      directionT25[1][c]=symmetrize(move)-center;
      for (int k=1;k<4;k++) {
        move=rotate(move);
        directionT25[k*2][c]=move-center;
        directionT25[k*2+1][c]=symmetrize(move)-center;
      }
      c++;
    }
  }
}








void MoGo::Goban::getInterestingMovesNew25(InterestingMoveFinder *finder, int newMode) const {/**assert(0);**/
  if (smallestNeighborhood.size()!=24)
    smallestNeighborhood.resize(24);
#ifdef DEBUG_GOBAN_SYMETRIES
  textModeShowGoban();
#endif
  for (int i=0;i<20;i++) {/**assert(0);**/
    Location location = lastPlayedMove + direction(i);
    if ((location<0) || (location >= (int)gobanState.size()))
      continue;
    if (gobanState[location] == EMPTY &&
        isInRangeLocation(location)) {/**assert(0);**/
#ifdef DEBUG_GOBAN_SYMETRIES
      Goban t(*this);
      if (isNotUseless(location)) {/**assert(0);**/
        t.playMoveOnGoban(location);
        t.textModeShowGoban(location);
      }
#endif
      int isa = isSelfAtari(location);
      if (isa) continue;
      for (int j=0;j<24;j++)
        smallestNeighborhood[j]=5;
      for (int j=0;j<8;j++) {/**assert(0);**/
#ifdef DEBUG_GOBAN_SYMETRIES
        GoTools::print("Symetry %i\n",j);
#endif
        bool isSmaller=false;
        int c=0;
        for (int k=0;k<25;k++) {/**assert(0);**/
#ifdef DEBUG_GOBAN_SYMETRIES
          if (k%5==0)
            GoTools::print("\n");
#endif
          if (k!=25/2)  {/**assert(0);**/
            Location target=location+directionT25[j][k];
            int s;
            if ((target<0) || (target >= (int)gobanState.size()))
              s=3;
            else {/**assert(0);**/
              s=gobanState[target];
              if (s==EMPTY)
                s=0;
              else if (s==playerColor)
                s=1;
              else if (s==3-playerColor)
                s=2;
              else if (s==OUTSIDE)
                s=3;
              else
                assert(0);
            }
            if (isSmaller || (s<smallestNeighborhood[c])) {/**assert(0);**/
              smallestNeighborhood[c]=s;
              isSmaller=true;
            } else if (s>smallestNeighborhood[c])
              break;

            c++;
          }
#ifdef DEBUG_GOBAN_SYMETRIES
          if (k!=25/2)
            GoTools::print("%i ",smallestNeighborhood[c-1]);
          else
            GoTools::print("%c ",'X');
          //           GoTools::print("%i ",directionT25[j][k]);
#endif

        }
#ifdef DEBUG_GOBAN_SYMETRIES
        GoTools::print("\n");
#endif

      }
#ifdef DEBUG_GOBAN_SYMETRIES
      int c=0;
      GoTools::print("\n");
      for (int k=0;k<25;k++) {/**assert(0);**/
        if (k%5==0)
          GoTools::print("\n");
        if (k!=25/2)  {/**assert(0);**/
          c++;
        }
        if (k!=25/2)
          GoTools::print("%i ",smallestNeighborhood[c-1]);
        else
          GoTools::print("%c ",'X');
      }
      GoTools::print("\n");
      getchar();
#endif
      if (finder) {/**assert(0);**/
        if (finder->isInteresting(smallestNeighborhood))
          interestingMoves.push_back(location);
      } else {/**assert(0);**/
        if (isNewInterestingMoves(smallestNeighborhood, newMode))
          interestingMoves.push_back(location);
      }
    }
  }
}











int MoGo::Goban::distance( Location l1, Location l2 ) const {/**assert(0);**/
  return abs(xIndex(l1)-xIndex(l2))+abs(yIndex(l1)-yIndex(l2));
}

/** in the smallestNeighborhood a representation of the neighborhood for each possible position
0 for empty
1 for same color string with 1 liberty
2 for same color string with 2 liberties
3 for same color string with 3 or more liberties
4 for other color string with 1 liberty
5 for other color string with 2 liberties
6 for other color string with 3 or more liberties
7 for outside
 
Among the 8 symetrizations of the position, we choose the smallest in the lexicographic sense.
*/
void MoGo::Goban::getInterestingMovesNewWithLiberties(InterestingMoveFinder *finder, int newMode) const {/**assert(0);**/
  if (smallestNeighborhood.size()!=8)
    smallestNeighborhood.resize(8);
#ifdef DEBUG_GOBAN_SYMETRIES
  textModeShowGoban();
#endif
  assert(lastPlayedMove!=PASSMOVE);
  int nbLoops=1; if ((  lastLastLastPlayedMove!=PASSMOVE) && (useLastLastMove)) nbLoops=2;
  for (int llm=0;(llm<nbLoops)&&(interestingMoves.size()==0);llm++) {/**assert(0);**/
    Location lastPlayedMoveTmp;
    if (llm==0)
      lastPlayedMoveTmp=lastPlayedMove;
    else
      lastPlayedMoveTmp=lastLastLastPlayedMove;
    int distanceMin=0;
    for (int i=0;i<8;i++) {/**assert(0);**/
      Location location = lastPlayedMoveTmp + direction(i);
      if (gobanState[location] == EMPTY &&
          isInRangeLocation(location)) {/**assert(0);**/
#ifdef DEBUG_GOBAN_SYMETRIES
        Goban t(*this);
        if (isNotUseless(location)) {/**assert(0);**/
          t.playMoveOnGoban(location);
          t.textModeShowGoban(location);
        }
#endif
        int isa = isSelfAtari(location);
        if (isa) continue;
        for (int j=0;j<8;j++)
          smallestNeighborhood[j]=8;

        for (int j=0;j<8;j++) {/**assert(0);**/
#ifdef DEBUG_GOBAN_SYMETRIES
          GoTools::print("Symetry %i\n",j);
#endif
          bool isSmaller=false;
          int c=0;
          for (int k=0;k<9;k++) {/**assert(0);**/
#ifdef DEBUG_GOBAN_SYMETRIES
            if (k%3==0)
              GoTools::print("\n");
#endif
            if (k!=CENTER_)  {/**assert(0);**/
              Location tmp=location+locationInformation.directionT[j][k];
              int s=gobanState[tmp];
              if (s==EMPTY)
                s=0;
              else if (s==playerColor) {/**assert(0);**/
                int liberties=getStringLiberty(tmp);
                if (liberties>=3)
                  s=3;
                else
                  s=liberties;
              } else if (s==3-playerColor) {/**assert(0);**/
                int liberties=getStringLiberty(tmp);
                if (liberties>=3)
                  s=6;
                else
                  s=3+liberties;
              } else if (s==OUTSIDE)
                s=7;
              else {/**assert(0);**/
                GoTools::print("location %i, s = %i, tmp = %i\n", location, s, tmp);
                assert(0);
              }
              if (isSmaller || (s<smallestNeighborhood[c])) {/**assert(0);**/
                smallestNeighborhood[c]=s;
                isSmaller=true;
              } else if (s>smallestNeighborhood[c])
                break;
              c++;
            }
#ifdef DEBUG_GOBAN_SYMETRIES
            if (k!=CENTER_)
              GoTools::print("%i ",smallestNeighborhood[c-1]);
            else
              GoTools::print("%c ",'X');
#endif

          }
#ifdef DEBUG_GOBAN_SYMETRIES
          GoTools::print("\n");
#endif

        }
#ifdef DEBUG_GOBAN_SYMETRIES
        int c=0;
        GoTools::print("\n");
        for (int k=0;k<9;k++) {/**assert(0);**/
          if (k%3==0)
            GoTools::print("\n");
          if (k!=CENTER_)  {/**assert(0);**/
            c++;
          }
          if (k!=CENTER_)
            GoTools::print("%i ",smallestNeighborhood[c-1]);
          else
            GoTools::print("%c ",'X');
        }
        GoTools::print("\n");
        getchar();
#endif
        if (finder) {/**assert(0);**/
          if (finder->isInteresting(smallestNeighborhood))
            interestingMoves.push_back(location);
        } else {/**assert(0);**/
          if (isNewInterestingMoves(smallestNeighborhood, newMode)) {/**assert(0);**/

            if (useDistanceInGPPatterns) {/**assert(0);**/
              // if (llm==1)
              // GoTools::print("lastLastLastPlayedMove\n");
              int d=distance(location, lastLastPlayedMove);
              // GoTools::print("distance %d, location %i, lastLastPlayedMove %i\n", d, location, lastLastPlayedMove);
              if (lastLastPlayedMove!=PASSMOVE && d>distanceMin) {/**assert(0);**/
                distanceMin=d;
                interestingMoves.clear();
              } else if (lastLastPlayedMove!=PASSMOVE && d<distanceMin)
                continue;
            }
            interestingMoves.push_back(location);
          }
        }
      }
    }
  }
}





/** in the smallestNeighborhood a representation of the neighborhood for each possible position
0 for empty
1 for same color string with 1 liberty
2 for same color string with 2 liberties
3 for same color string with 3 or more liberties
4 for other color string with 1 liberty
5 for other color string with 2 liberties
6 for other color string with 3 or more liberties
7 for outside
 
Among the 8 symetrizations of the position, we choose the smallest in the lexicographic sense.
*/
void MoGo::Goban::getInterestingMovesNew25WithLiberties(InterestingMoveFinder *finder, int newMode) const {/**assert(0);**/
  if (smallestNeighborhood.size()!=24)
    smallestNeighborhood.resize(24);
#ifdef DEBUG_GOBAN_SYMETRIES
  textModeShowGoban();
#endif
  for (int i=0;i<20;i++) {/**assert(0);**/
    Location location = lastPlayedMove + direction(i);
    if ((location<0) || (location >= (int)gobanState.size()))
      continue;
    if (gobanState[location] == EMPTY &&
        isInRangeLocation(location)) {/**assert(0);**/
#ifdef DEBUG_GOBAN_SYMETRIES
      Goban t(*this);
      if (isNotUseless(location)) {/**assert(0);**/
        t.playMoveOnGoban(location);
        t.textModeShowGoban(location);
      }
#endif
      int isa = isSelfAtari(location);
      if (isa) continue;
      for (int j=0;j<24;j++)
        smallestNeighborhood[j]=5;
      for (int j=0;j<8;j++) {/**assert(0);**/
#ifdef DEBUG_GOBAN_SYMETRIES
        GoTools::print("Symetry %i\n",j);
#endif
        bool isSmaller=false;
        int c=0;
        for (int k=0;k<25;k++) {/**assert(0);**/
#ifdef DEBUG_GOBAN_SYMETRIES
          if (k%5==0)
            GoTools::print("\n");
#endif
          if (k!=25/2)  {/**assert(0);**/
            Location target=location+directionT25[j][k];
            int s;
            if ((target<0) || (target >= (int)gobanState.size()))
              s=3;
            else {/**assert(0);**/
              s=gobanState[target];
              if (s==EMPTY)
                s=0;
              else if (s==playerColor) {/**assert(0);**/
                int liberties=getStringLiberty(target);
                if (liberties>=3)
                  s=3;
                else
                  s=liberties;
              } else if (s==3-playerColor) {/**assert(0);**/
                int liberties=getStringLiberty(target);
                if (liberties>=3)
                  s=6;
                else
                  s=3+liberties;
              } else if (s==OUTSIDE)
                s=7;
              else {/**assert(0);**/
                GoTools::print("location %i, s = %i, tmp = %i\n", location, s, target);
                assert(0);
              }
            }
            if (isSmaller || (s<smallestNeighborhood[c])) {/**assert(0);**/
              smallestNeighborhood[c]=s;
              isSmaller=true;
            } else if (s>smallestNeighborhood[c])
              break;
            c++;
          }
#ifdef DEBUG_GOBAN_SYMETRIES
          if (k!=25/2)
            GoTools::print("%i ",smallestNeighborhood[c-1]);
          else
            GoTools::print("%c ",'X');
          //           GoTools::print("%i ",directionT25[j][k]);
#endif

        }
#ifdef DEBUG_GOBAN_SYMETRIES
        GoTools::print("\n");
#endif

      }
#ifdef DEBUG_GOBAN_SYMETRIES
      int c=0;
      GoTools::print("\n");
      for (int k=0;k<25;k++) {/**assert(0);**/
        if (k%5==0)
          GoTools::print("\n");
        if (k!=25/2)  {/**assert(0);**/
          c++;
        }
        if (k!=25/2)
          GoTools::print("%i ",smallestNeighborhood[c-1]);
        else
          GoTools::print("%c ",'X');
      }
      GoTools::print("\n");
      getchar();
#endif
      if (finder) {/**assert(0);**/
        if (finder->isInteresting(smallestNeighborhood))
          interestingMoves.push_back(location);
      } else {/**assert(0);**/
        if (isNewInterestingMoves(smallestNeighborhood, newMode))
          interestingMoves.push_back(location);
      }
    }
  }
}









#define MIN_PROBA_getInterestingMovesNearLastMoveWithProbas (0.5)

void MoGo::Goban::getInterestingMovesNearLastMoveWithProbas(int position1, int position2) const {/**assert(0);**/
  assert(probabilitiesOfinterestingMovesStatic.size()==6);
  for (int i=position1;i<position2;i++) {/**assert(0);**/
    Location location = lastPlayedMove + direction(i);
    if (i>=8 && (location<leftUpLocation_ || location>rightDownLocation_)) continue;
    if (gobanState[location] == EMPTY &&
        isInRangeLocation(location)) {/**assert(0);**/
      int isa = isSelfAtari(location);
      if (isa) continue;
      if (isAtari(location) && (probabilitiesOfinterestingMovesStatic[0]>MIN_PROBA_getInterestingMovesNearLastMoveWithProbas)) {/**assert(0);**/
        interestingMovesProbabilities.push_back(probabilitiesOfinterestingMovesStatic[0]);
        interestingMoves.push_back(location);
      }
      if (isEatingMove(location) && (probabilitiesOfinterestingMovesStatic[1]>MIN_PROBA_getInterestingMovesNearLastMoveWithProbas)) {/**assert(0);**/
        interestingMovesProbabilities.push_back(probabilitiesOfinterestingMovesStatic[1]);
        interestingMoves.push_back(location);
      }
      if (isOnGobanSide(location) && (probabilitiesOfinterestingMovesStatic[2]>MIN_PROBA_getInterestingMovesNearLastMoveWithProbas)) {/**assert(0);**/
        if (locationInformation.isYoseOnSide(location)) {/**assert(0);**/
          interestingMovesProbabilities.push_back(probabilitiesOfinterestingMovesStatic[2]);
          interestingMoves.push_back(location);
        }
        continue;
      }
      //         else if (!(int)atariSavingMoves.size()) {/**assert(0);**/
      if (isHane(location) && (probabilitiesOfinterestingMovesStatic[3]>MIN_PROBA_getInterestingMovesNearLastMoveWithProbas)) {/**assert(0);**/
        interestingMovesProbabilities.push_back(probabilitiesOfinterestingMovesStatic[3]);
        interestingMoves.push_back(location);
      } else if (isCut(location) && (probabilitiesOfinterestingMovesStatic[4]>MIN_PROBA_getInterestingMovesNearLastMoveWithProbas)) {/**assert(0);**/
        interestingMovesProbabilities.push_back(probabilitiesOfinterestingMovesStatic[4]);
        interestingMoves.push_back(location);
      } else if (isCrossCut(location) && (probabilitiesOfinterestingMovesStatic[5]>MIN_PROBA_getInterestingMovesNearLastMoveWithProbas)) {/**assert(0);**/
        interestingMovesProbabilities.push_back(probabilitiesOfinterestingMovesStatic[5]);
        interestingMoves.push_back(location);
      }
    }
  }
}

bool MoGo::Goban::randomOutOneMoveFromInterestingMovesWithProbas(Location &location) const {/**assert(0);**/
  assert(interestingMovesProbabilities.size()==interestingMoves.size());
  if (interestingMoves.size()==0)
    return false;
  if (interestingMoves.size()==1) {/**assert(0);**/
    if (isNotUseless(interestingMoves[0])) {/**assert(0);**/
      location=interestingMoves[0];
      return true;
    } else
      return false;
  }
  double sum=0.;
  for (int i=0;i<(int)interestingMovesProbabilities.size();i++)
    sum+=interestingMovesProbabilities[i];
  while (interestingMoves.size() > 0) {/**assert(0);**/
    double indexDouble=randDouble()*sum;
    int index=0;
    indexDouble-=interestingMovesProbabilities[0];
    while (indexDouble>0.) {/**assert(0);**/
      index++;assert(index<(int)interestingMovesProbabilities.size());
      indexDouble-=interestingMovesProbabilities[index];
    }
    if (isNotUseless(interestingMoves[index])) {/**assert(0);**/
      location=interestingMoves[index];
      return true;
    } else {/**assert(0);**/
      interestingMoves[index]=interestingMoves[interestingMoves.size()-1];
      sum-=interestingMovesProbabilities[index];
      interestingMovesProbabilities[index]=interestingMovesProbabilities[interestingMovesProbabilities.size()-1];
      interestingMoves.pop_back();
      interestingMovesProbabilities.pop_back();
    }
  }
  return false;
}














// TEMPORARY COMMENT: THIS FUNCTION IS USED
// TEMPORARY COMMENT: BLACK PLAYS 2 TIMES
bool MoGo::Goban::retrieveDeadStrings( int randomMode, int nbSimulations, double threshold, Vector< double > & probabilityOfStringSurvival, Vector< Vector < Location > > & locationsOfDeadStrings, double komi ) const {
  //   int totalStringNumber=stringInformation.stringLiberty.size();
  //   Vector<int> nbSurvivalsOfStrings(totalStringNumber);
  Vector<int> score((heightOfGoban+2)*(widthOfGoban+2),0);
  Vector<double> allScores(nbSimulations);
  locationsOfDeadStrings.clear();
  probabilityOfStringSurvival.clear();
  int numSim=0, nbScoredGobans=0;
  for (;numSim<nbSimulations;numSim++) {
    Goban goban(*this); goban.setCFG(false);
    int pass=0;
    int counter = 0;
    goban.setMaintainGobanKey(false);
    Location location;
    do {
      counter++;
      location=goban.randomOutOneMoveAndPlay(randomMode, 3); //FIXME
      if (location==PASSMOVE) pass++;
      else pass=0;
      if (counter>=(int)heightOfGoban*widthOfGoban*2) {/**assert(0);**/
        //         GoTools::print("infinit loop!\n");
        break;
      }
    } while(pass<2);
    if (pass<2) continue;
    //      goban.textModeShowGoban();

    double gobanScore=goban.scoreAdding(score);
    allScores[nbScoredGobans]=gobanScore;
    nbScoredGobans++;
   
  }
  if (nbScoredGobans==0)
    return false;
  int nbTerritoriesUndecided=0;
  double probaOfTerritoriesUndecided=0.;

  double deadStringsUncertainty=-1.; int deadStringsUncertaintyPosition=PASSMOVE;
  int totalStringNumber=stringInformation.stringLiberty.size();
  Vector<int> stringNumberToDeadStringsList(totalStringNumber,-1);
  for (int j=0;j<heightOfGoban;j++)
    for(int k=0;k<widthOfGoban;k++) {
      Location location=indexOnArray(j,k);
      if (gobanState[location]==OUTSIDE)
        continue;
      double ratioTerritory=double(score[location])/double(numSim)/2.+0.5;
      if ((ratioTerritory<=0.5 && ratioTerritory>threshold) || (ratioTerritory>=0.5 && ratioTerritory<1.-threshold)) {
        nbTerritoriesUndecided++;
        if (ratioTerritory<=0.5)
          probaOfTerritoriesUndecided+=ratioTerritory;
        else
          probaOfTerritoriesUndecided+=1.-ratioTerritory;
      }
      //       GoTools::print("ratioTerritory for %i = %f ", location, ratioTerritory); textModeShowPosition(location); GoTools::print("\n");
      if (gobanState[location]==EMPTY)
        continue;
      bool deadLocation=false;
      if (ratioTerritory>0.5 && gobanState[location]==WHITE) // is more black but the string is white
        deadLocation=true;
      else if (ratioTerritory<=0.5 && gobanState[location]==BLACK) // is more white but the string is black
        deadLocation=true;
      double tmp=deadLocation?(gobanState[location]==WHITE?1.-ratioTerritory:ratioTerritory):(gobanState[location]==WHITE?ratioTerritory:1.-ratioTerritory);
      if (tmp>deadStringsUncertainty) {
        deadStringsUncertainty=tmp;
        deadStringsUncertaintyPosition=location;
      }
      int num=stringInformation.stringNumber[location];assert(num>=0);assert(num<totalStringNumber);
      if (num>=(int)probabilityOfStringSurvival.size()) {
        probabilityOfStringSurvival.resize(num+1); probabilityOfStringSurvival[num]=(gobanState[location]==WHITE?/*1.-*/ratioTerritory:1.-ratioTerritory); //FIXME
      }
      if (deadLocation) {
        int index=stringNumberToDeadStringsList[num];
        if (index<0) {
          stringNumberToDeadStringsList[num]=locationsOfDeadStrings.size();
          index=locationsOfDeadStrings.size();
          locationsOfDeadStrings.push_back(Vector<Location>());
        }
        locationsOfDeadStrings[index].push_back(location);
      }
    }
  int nbUnclearScores=0; int nbWinningForBlack=0;
  for (int i=0;i<nbScoredGobans;i++) {
    bool winningForBlack=allScores[i]>komi;
    double minDiff=2.*probaOfTerritoriesUndecided; //if (double(nbTerritoriesUndecided)<minDiff) minDiff=nbTerritoriesUndecided;
    if (winningForBlack && ((allScores[i]-minDiff)<=komi))
      nbUnclearScores++;
    else if ((!winningForBlack) && (allScores[i]+minDiff>=komi))
      nbUnclearScores++;
    if (winningForBlack) nbWinningForBlack++;
  }
  GoTools::print("nbTerritoriesUndecided: %i (prob %f). nbUnclearScores %i/%i = %f (black %f). Dead strings uncertainty %f (", nbTerritoriesUndecided, probaOfTerritoriesUndecided, nbUnclearScores, nbScoredGobans, double(nbUnclearScores)/double(nbScoredGobans), double(nbWinningForBlack)/double(nbScoredGobans), deadStringsUncertainty);
  textModeShowPosition(deadStringsUncertaintyPosition);
  GoTools::print(")\n");

  //   return double(nbUnclearScores)/double(nbScoredGobans)<threshold;
  return deadStringsUncertainty<threshold;
}


// TEMPORARY COMMENT: THIS FUNCTION IS USED
// TEMPORARY COMMENT: BLACK PLAYS 2 TIMES
// returns true if and only if the dead strings are for sure
bool MoGo::Goban::retrieveDeadStrings(int nbSimulations, double threshold, Vector<Vector<Location> >&locationsOfDeadStrings, double komi) const {
  Vector<double> probabilityOfStringSurvival;
  return retrieveDeadStrings(2015441, nbSimulations,threshold, probabilityOfStringSurvival, locationsOfDeadStrings, komi);
}


double MoGo::Goban::score( Vector< int > & positionScores ) const {/**assert(0);**/
  double gobanScore=0.;
  Location location=leftUpLocation_;
  for (int i=0;i<heightOfGoban;i++,location += 2) {/**assert(0);**/
    for (int j=0;j<widthOfGoban;j++,location++)
      if (gobanState[location]==BLACK) {/**assert(0);**/
        gobanScore++;
        positionScores[location]=1;
      } else if (gobanState[location]==WHITE) {/**assert(0);**/
        gobanScore--;
        positionScores[location]=-1;
      } else if (gobanState[location]==EMPTY) {/**assert(0);**/
        int state=0;
        for (int k=0;k<4;k++)
          if (gobanState[location+direction(k)]==WHITE) {/**assert(0);**/ // The point can't go to BLACK
            if (state==1) // was black
            {/**assert(0);**/ state=0; break; }
            state=2;
          } else if (gobanState[location+direction(k)]==BLACK) {/**assert(0);**/ // The point can't go to WHITE
            if (state==2) // was white
            {/**assert(0);**/ state=0; break; }
            state=1;
          }
        if (state==1) {/**assert(0);**/ positionScores[location]=1; gobanScore++; } else if (state==2) {/**assert(0);**/ positionScores[location]=-1; gobanScore--; }
      }
  }
  return gobanScore;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
// TEMPORARY COMMENT: BLACK PLAYS 2 TIMES
double MoGo::Goban::scoreAdding( Vector< int > & positionScores ) const {
  double gobanScore=0.;
  Location location=leftUpLocation_;
  for (int i=0;i<heightOfGoban;i++,location += 2) {
    for (int j=0;j<widthOfGoban;j++,location++)
      if (gobanState[location]==BLACK) {
        gobanScore ++;
        positionScores[location]++;
      } else if (gobanState[location]==WHITE) {
        gobanScore --;
        positionScores[location]--;
      } else if (gobanState[location]==EMPTY) {
        int state=0;
        for (int k=0;k<4;k++)
          if (gobanState[location+direction(k)]==WHITE) { // The point can't go to BLACK
            if (state==1) // was black
            { state=0; break; }
            state=2;
          } else if (gobanState[location+direction(k)]==BLACK) { // The point can't go to WHITE
            if (state==2) // was white
            { state=0; break; }
            state=1;
          }
        if (state==1) { positionScores[location]++; gobanScore++; } else if (state==2) { positionScores[location]--; gobanScore--; }
      }
  }
  return gobanScore;
}

double MoGo::Goban::scoreAddingEyeLocations( Vector<int> &eyeBlack, Vector<int> &eyeWhite ) const {/**assert(0);**/
  double gobanScore=0.;
  Location location=leftUpLocation_;
  for (int i=0;i<heightOfGoban;i++,location += 2) {/**assert(0);**/
    for (int j=0;j<widthOfGoban;j++,location++) {/**assert(0);**/
      if (gobanState[location]==BLACK) {/**assert(0);**/
        gobanScore ++;
        //         positionScores[location]++;
      } else if (gobanState[location]==WHITE) {/**assert(0);**/
        gobanScore --;
        //         positionScores[location]--;
      } else if (gobanState[location]==EMPTY) {/**assert(0);**/
        int state=0;
        for (int k=0;k<4;k++)
          if (gobanState[location+direction(k)]==WHITE) {/**assert(0);**/ // The point can't go to BLACK
            if (state==1) // was black
            {/**assert(0);**/ state=0; break; }
            state=2;
          } else if (gobanState[location+direction(k)]==BLACK) {/**assert(0);**/ // The point can't go to WHITE
            if (state==2) // was white
            {/**assert(0);**/ state=0; break; }
            state=1;
          }
        if (state==1) {/**assert(0);**/eyeBlack[location]++; gobanScore++; } else if (state==2) {/**assert(0);**/ eyeWhite[location]++; gobanScore--; }
      }
    }
  }
  return gobanScore;
}


Vector< int > MoGo::Goban::getLocationRepresentation( Location location, int patternSize ) const {/**assert(0);**/
  Vector<int> smallestNeighborhood;
  if (patternSize==3) {/**assert(0);**/
    smallestNeighborhood.resize(8*2);
    for (int j=0;j<16;j++)
      smallestNeighborhood[j]=100;
    for (int j=0;j<8;j++) {/**assert(0);**/
      int c=0;
      bool isSmaller=false;
      bool isLibertiesSmaller=false;
      for (int k=0;k<9;k++) {/**assert(0);**/
        if (k!=9/2)  {/**assert(0);**/
          Location target=location+locationInformation.directionT[j][k];
          int s;
          int liberties=0;
          if ((target<0) || (target >= (int)gobanState.size()))
            s=3;
          else {/**assert(0);**/
            s=gobanState[target];
            if (s==EMPTY)
              s=0;
            else if (s==playerColor) {/**assert(0);**/
              liberties=getStringLiberty(target);if (liberties>3) liberties=3;
              s=1;
            } else if (s==3-playerColor) {/**assert(0);**/
              liberties=getStringLiberty(target);if (liberties>3) liberties=3;
              s=2;
            } else if (s==OUTSIDE)
              s=3;
            else {/**assert(0);**/
              GoTools::print("location %i, s = %i, tmp = %i\n", location, s, target);
              assert(0);
            }
          }
          if (isSmaller || (s<smallestNeighborhood[c])) {/**assert(0);**/
            smallestNeighborhood[c]=s;
            smallestNeighborhood[c+8]=liberties;
            isSmaller=true;
          } else if (s>smallestNeighborhood[c])
            break;
          else {/**assert(0);**/
            assert(smallestNeighborhood[c]==s);
            if (isLibertiesSmaller || (liberties<smallestNeighborhood[c+8])) {/**assert(0);**/
              //               smallestNeighborhood[c]=s;
              smallestNeighborhood[c+8]=liberties;
              isLibertiesSmaller=true;
            } else if (liberties>smallestNeighborhood[c+8])
              break;
          }
          c++;
        }
      }
    }
  } else if (patternSize==5) {/**assert(0);**/
    smallestNeighborhood.resize(24*2);
    for (int j=0;j<48;j++)
      smallestNeighborhood[j]=100;
    for (int j=0;j<8;j++) {/**assert(0);**/
      int c=0;
      bool isSmaller=false;
      bool isLibertiesSmaller=false;
      for (int k=0;k<25;k++) {/**assert(0);**/
        if (k!=25/2)  {/**assert(0);**/
          Location target=location+directionT25[j][k];
          int s;
          int liberties=0;
          if ((target<0) || (target >= (int)gobanState.size()))
            s=3;
          else {/**assert(0);**/
            s=gobanState[target];
            if (s==EMPTY)
              s=0;
            else if (s==playerColor) {/**assert(0);**/
              liberties=getStringLiberty(target);if (liberties>3) liberties=3;
              s=1;
            } else if (s==3-playerColor) {/**assert(0);**/
              liberties=getStringLiberty(target);if (liberties>3) liberties=3;
              s=2;
            } else if (s==OUTSIDE)
              s=3;
            else {/**assert(0);**/
              GoTools::print("location %i, s = %i, tmp = %i\n", location, s, target);
              assert(0);
            }
          }
          if (isSmaller || (s<smallestNeighborhood[c])) {/**assert(0);**/
            smallestNeighborhood[c]=s;
            smallestNeighborhood[c+24]=liberties;
            isSmaller=true;
          } else if (s>smallestNeighborhood[c])
            break;
          else {/**assert(0);**/
            assert(smallestNeighborhood[c]==s);
            if (isLibertiesSmaller || (liberties<smallestNeighborhood[c+24])) {/**assert(0);**/
              //               smallestNeighborhood[c]=s;
              smallestNeighborhood[c+24]=liberties;
              isLibertiesSmaller=true;
            } else if (liberties>smallestNeighborhood[c+24])
              break;
          }
          c++;
        }
      }
    }
  } else
    assert(0);

  int distanceToLast=0;
  if (lastPlayedMove!=PASSMOVE)
    distanceToLast=distance(location, lastPlayedMove);

  if (distanceToLast>4) distanceToLast=4;
  smallestNeighborhood.push_back(distanceToLast);

  return smallestNeighborhood;
}


































































Vector< int > MoGo::Goban::getLocationRepresentation2( Location location, int patternSize ) const {/**assert(0);**/
  Vector<int> smallestNeighborhood;
  smallestNeighborhood.resize((patternSize*patternSize-1)*2+8,10000);

  int centerx=xIndex(location); int centery=yIndex(location);
  int halfPatternSize=patternSize/2;
  for (int i=0;i<8;i++) {/**assert(0);**/
    Vector<int> neighborhood((patternSize*patternSize-1)*2);
    int c=0;
    for (int xp=-halfPatternSize;xp<=halfPatternSize;xp++) {/**assert(0);**/
      for (int yp=-halfPatternSize;yp<=halfPatternSize;yp++) {/**assert(0);**/
        if (xp==0 && yp==0) continue;
        int s;
        int liberties=0;
        if ((centerx+xp<0) || (centery+yp<0) || (centerx+xp>=widthOfGoban) || (centery+yp>=heightOfGoban) || (gobanState[indexOnArray(centerx+xp,centery+yp)]==OUTSIDE)) {/**assert(0);**/
          s=3;
        } else {/**assert(0);**/
          Location target=completeSymmetry(i,indexOnArray(centerx+xp,centery+yp));

          s=gobanState[target];
          if (s==EMPTY)
            s=0;
          else if (s==playerColor) {/**assert(0);**/
            liberties=getStringLiberty(target);//if (liberties>3) liberties=3;
            s=1;
          } else if (s==3-playerColor) {/**assert(0);**/
            liberties=getStringLiberty(target);//if (liberties>3) liberties=3;
            s=2;
          } else {/**assert(0);**/
            GoTools::print("location %i, s = %i, tmp = %i\n", location, s, target);
            assert(0);
          }
        }
        neighborhood[c]=s;
        neighborhood[c+patternSize*patternSize-1]=liberties;
        c++;
      }
    }
    int dxl=0;int dyl=0;
    int dxll=0;int dyll=0;
    int dxlll=0;int dylll=0;
    Location locationSymetrized=completeSymmetry(i,location);

    int x=xIndex(locationSymetrized);
    int y=yIndex(locationSymetrized);
    if (lastPlayedMove!=PASSMOVE) {/**assert(0);**/
      dxl=x-xIndex(completeSymmetry(i,lastPlayedMove));
      dyl=y-yIndex(completeSymmetry(i,lastPlayedMove));
    }
    if (lastLastPlayedMove!=PASSMOVE) {/**assert(0);**/
      dxll=x-xIndex(completeSymmetry(i,lastLastPlayedMove));
      dyll=y-yIndex(completeSymmetry(i,lastLastPlayedMove));
    }
    if (lastLastLastPlayedMove!=PASSMOVE) {/**assert(0);**/
      dxlll=x-xIndex(completeSymmetry(i,lastLastLastPlayedMove));
      dylll=y-yIndex(completeSymmetry(i,lastLastLastPlayedMove));
    }
    neighborhood.push_back(x);neighborhood.push_back(y);
    neighborhood.push_back(dxl);neighborhood.push_back(dyl);
    neighborhood.push_back(dxll);neighborhood.push_back(dyll);
    neighborhood.push_back(dxlll);neighborhood.push_back(dylll);


    if (neighborhood<smallestNeighborhood)
      smallestNeighborhood=neighborhood;
  }
  //   int distance=0;
  //   if (lastPlayedMove!=PASSMOVE)
  //     distance=distance(location, lastPlayedMove);

  //   if (distance>4) distance=4;
  //   smallestNeighborhood.push_back(distance);

  return smallestNeighborhood;
}

Location MoGo::Goban::completeSymmetry( int numberOfSymetry, Location location ) const {/**assert(0);**/
  assert(numberOfSymetry>=0);
  assert(numberOfSymetry<8);
  for (int i=0;i<numberOfSymetry/2;i++)
    location=rotate(location);
  if ((numberOfSymetry%2)==1)
    location=symmetrize(location);
  return location;
}

Vector< int > MoGo::Goban::getRepresentation3( int &numSymetry ) const {/**assert(0);**/
  Vector<int> smallestNeighborhood;
  smallestNeighborhood.resize((heightOfGoban*widthOfGoban)*2/*+8*/,10000);

  for (int i=0;i<8;i++) {/**assert(0);**/
    Vector<int> neighborhood((heightOfGoban*widthOfGoban)*2);
    int c=0;
    for (int xp=0;xp<widthOfGoban;xp++) {/**assert(0);**/
      for (int yp=0;yp<heightOfGoban;yp++) {/**assert(0);**/
        int s;
        int liberties=0;

        Location target=completeSymmetry(i,indexOnArray(xp,yp));

        s=gobanState[target];
        if (s==EMPTY)
          s=0;
        else if (s==playerColor) {/**assert(0);**/
          liberties=getStringLiberty(target);//if (liberties>3) liberties=3;
          s=1;
        } else if (s==3-playerColor) {/**assert(0);**/
          liberties=getStringLiberty(target);//if (liberties>3) liberties=3;
          s=2;
        } else {/**assert(0);**/
          //           GoTools::print("location %i, s = %i, tmp = %i\n", location, s, target);
          assert(0);
        }
        neighborhood[c]=s;
        neighborhood[c+heightOfGoban*widthOfGoban]=liberties;
        c++;
      }
    }
 

    if (neighborhood<smallestNeighborhood) {/**assert(0);**/
      smallestNeighborhood=neighborhood;
      numSymetry=i;
    }
  }
  //   int distance=0;
  //   if (lastPlayedMove!=PASSMOVE)
  //     distance=distance(location, lastPlayedMove);

  //   if (distance>4) distance=4;
  //   smallestNeighborhood.push_back(distance);

  return smallestNeighborhood;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
// TEMPORARY COMMENT: BLACK PLAYS 2 TIMES
double MoGo::Goban::scoreKGS( const Vector< Vector < Location > > & locationsOfDeadStrings ) const {
  int score=0;
  Vector<bool> blackAlreadySeen(gobanState.size(),false);
  Vector<bool> whiteAlreadySeen(gobanState.size(),false);
  Vector<bool> isDead(gobanState.size(),false);
  for (int i=0;i<(int)locationsOfDeadStrings.size();i++)
    for (int j=0;j<(int)locationsOfDeadStrings[i].size();j++) {
      isDead[locationsOfDeadStrings[i][j]]=true;
      // GoTools::print("locationsOfDeadStrings[%i][%i]=%i\n", i,j,locationsOfDeadStrings[i][j]);
    }
  Location location=leftUpLocation_;
  for (int i=0;i<heightOfGoban;i++,location += 2) {
    for (int j=0;j<widthOfGoban;j++,location++) {
      //  GoTools::print("scoreKGS %i / %i\n", location, (int)isDead[location]);
      if (gobanState[location]==EMPTY || isDead[location]) {
        continue;
      } else if (gobanState[location]==BLACK)
        iterateScoreKGS(location, true, isDead, blackAlreadySeen, score);
      else if (gobanState[location]==WHITE)
        iterateScoreKGS(location, false, isDead, whiteAlreadySeen, score);
    }
  }
  return score;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
// TEMPORARY COMMENT: BLACK PLAYS 2 TIMES
void MoGo::Goban::iterateScoreKGS( Location location, bool isBlack, const Vector<bool> &isDead, Vector< bool > & alreadySeen, int & score ) const {
  if (alreadySeen[location])
    return;
  // GoTools::print("iterateScoreKGS %i / %i\n", location, isBlack);
  alreadySeen[location]=true;
  if (isBlack) score++; else score--;
  for (int k=0;k<4;k++) {
    Location target=location+direction(k);
    if (gobanState[target]==EMPTY || isDead[target]) {
      iterateScoreKGS(target,isBlack,isDead,alreadySeen,score);
    } else if (gobanState[target]==BLACK && isBlack) {
      iterateScoreKGS(target,isBlack,isDead,alreadySeen,score);
    } else if (gobanState[target]==WHITE && (!isBlack)) {
      iterateScoreKGS(target,isBlack,isDead,alreadySeen,score);
    }
  }
}

















Location MoGo::Goban::randomOutOneNotUselessAvoidingSelfAtariMovePreference( int movePreferenceMode, LocationList *& locationListIncludingThisLocation, int thresholdSelfAtari ) const {/**assert(0);**/
  assert(moveValuesBlack.size()==moveValuesWhite.size());
/*  if (moveValuesBlack.size()==0)
    return randomOutOneNotUselessAvoidingSelfAtari(locationListIncludingThisLocation, thresholdSelfAtari);
*/ //FIXME
  if (movePreferenceMode==0) {/**assert(0);**/
//     GoTools::print(moveValuesBlack); GoTools::print(moveValuesWhite);
    double max=-10.;Location bestMove=PASSMOVE;
    Vector<double> *moveValues;
    if (!isBlacksTurn())
      moveValues=&moveValuesBlack;
    else
      moveValues=&moveValuesWhite;
    int iterations=random200Iterations;
    if (moveNumber()>eatingSizePrefered+int(moveValuesBlack[0])) //FIXME
      iterations=1;
    for (int i=0;i<iterations;i++) {/**assert(0);**/
      Location location=randomOutOneNotUselessAvoidingSelfAtari(locationListIncludingThisLocation, thresholdSelfAtari);
//       double value=(*moveValues)[location]; //FIXME
      double value=distanceToNearestString(location);
      if (value>max) {/**assert(0);**/
        bestMove=location;
        max=value;
      }
    }
    locationListIncludingThisLocation=0;
    return bestMove;
  } else if (movePreferenceMode==1) {/**assert(0);**/
    Vector<double> *moveValues;
    if (isBlacksTurn()) moveValues=&moveValuesBlack; else moveValues=&moveValuesWhite;
   
    double max=-10.;Location bestMove=PASSMOVE;
    int iterations=random200Iterations;
    if (moveNumber()>250)
      iterations=1;
    for (int i=0;i<iterations;i++) {/**assert(0);**/
      Location location=randomOutOneNotUselessAvoidingSelfAtari(locationListIncludingThisLocation, thresholdSelfAtari);
      if ((*moveValues)[location]>max) {/**assert(0);**/
        bestMove=location;
        max=(*moveValues)[location];
      }
    }
    //Goban tmp(*this);tmp.playMoveOnGoban(bestMove);tmp.textModeShowGoban(bestMove);
    locationListIncludingThisLocation=0;
    return bestMove;
  } else if (movePreferenceMode==2) {/**assert(0);**/
    double max=-10.;Location bestMove=PASSMOVE;
    Vector<double> *moveValues;
    if (isBlacksTurn()) moveValues=&moveValuesBlack; else moveValues=&moveValuesWhite;
    for (int i=0;i<random200Iterations;i++) {/**assert(0);**/
      Location location=randomOutOneNotUselessAvoidingSelfAtari(locationListIncludingThisLocation, thresholdSelfAtari);
      // GoTools::print("(*moveValues)[location] %f\n", (*moveValues)[location]);
 
      if (moveValuesBlack[location]+moveValuesWhite[location]>max) {/**assert(0);**/
        bestMove=location;
        max=moveValuesBlack[location]+moveValuesWhite[location];
      }
    }
    locationListIncludingThisLocation=0;
    return bestMove;
  } else
    assert(0);
  return PASSMOVE;
}







// #define DEBUG_SAVE_STRING
// TEMPORARY COMMENT: THIS FUNCTION IS USED
// TEMPORARY COMMENT: BLACK PLAYS 2 TIMES
Location MoGo::Goban::randomOutOneMoveMode2015441( LocationList *& locationListIncludingThisLocation, int thresholdSelfAtari ) const {
  locationListIncludingThisLocation=0;

  if (koUseMode>0) {/**assert(0);**/
    Location location=koModeMove(locationListIncludingThisLocation, thresholdSelfAtari);
    if (location!=PASSMOVE) {/**assert(0);**/
      /*      textModeShowGoban();
            GoTools::print("(%i %i) ko move played in ", koModeMoveStep, koModeMoveLocation);//FIXME
            textModeShowPosition(location);
            GoTools::print("\n");
            getchar();*/
      return location;
    }
  }


  if (epsilonGreedy>0.)
    if (randDouble()>1.-epsilonGreedy)
      return randomOutOneNotUselessMove(locationListIncludingThisLocation);

  // GoTools::print("random\n");
  if ( lastPlayedMove == PASSMOVE) {
    // GoTools::print("eating pref with PASSMOVE\n");
    //     showFreeLocation();
    //     printAllStringInformation();
    return randomOutOneNotUselessAvoidingSelfAtari(locationListIncludingThisLocation,thresholdSelfAtari);
  }

  underAtariStringsFirstPosition.clear();
  if (locationInformation.isLastMoveAtari(underAtariStringsFirstPosition)) {
#ifdef DEBUG_SAVE_STRING
    GoTools::print("last move atari found!\n");
#endif

    for (int i=0;i<(int)underAtariStringsFirstPosition.size()-1;i++)
      for (int j=i+1;j<(int)underAtariStringsFirstPosition.size();j++)
        if (stringInformation.getStringLength(underAtariStringsFirstPosition[i]) < stringInformation.getStringLength(underAtariStringsFirstPosition[j])) {
          Location tmp = underAtariStringsFirstPosition[i];
          underAtariStringsFirstPosition[i] = underAtariStringsFirstPosition[j];
          underAtariStringsFirstPosition[j] = tmp;
        }

    for (int i=0;i<(int)underAtariStringsFirstPosition.size();i++) {
      Location savingMove = PASSMOVE;
      if (isUnderAtariStringSavableByEatingMove(underAtariStringsFirstPosition[i],savingMove)) {
        //         textModeShowPosition(savingMove);
        assert(isLegal(savingMove));
        //         if (isInRangeLocation(savingMove)) {/**assert(0);**/
        locationListIncludingThisLocation=getLocationListIncludingThisLocationForMovePlay(savingMove);
        return savingMove;
        //         }
      }
      savingMove = stringInformation.getOneLibertyOfString(underAtariStringsFirstPosition[i]);
#ifdef DEBUG_SAVE_STRING
      textModeShowPosition(savingMove);
      GoTools::print("savingMove isLegal %d isAtariSavingMoveTrue %d\n",isLegal(savingMove),isAtariSavingMoveTrue(savingMove));
#endif

      if( /*isInRangeLocation(savingMove) &&*/
        isLegal(savingMove) &&
        isAtariSavingMoveTrue(savingMove)) {
        locationListIncludingThisLocation=getLocationListIncludingThisLocationForMovePlay(savingMove);
#ifdef DEBUG_SAVE_STRING
        GoTools::print("saved by true\n");
#endif

        return savingMove;
      }
    }
    return randomOutOneNotUselessAvoidingSelfAtari(locationListIncludingThisLocation,thresholdSelfAtari);
  }
 

  // textModeShowPosition(lastPlayedMove);
  // GoTools::print(" lastPlayedMove \n");
  assert(lastPlayedMove != PASSMOVE);
  interestingMoves.clear();
  Location location = PASSMOVE;
  for (int i=0;i<8;i++) {

    location = lastPlayedMove + direction(i);
    if (/*isInRangeLocation(location) &&*/ gobanState[location] == EMPTY) {
      //       int isa = isSelfAtari(location);
      //       if (isa) continue;
      //       if (isEatingMove(location))
      //         interestingMoves.push_back(location);
      int numPattern=0;

     

      for (int j=0;j<8;j++) { numPattern+=gobanState[location+direction(j)];numPattern=numPattern<<2; }
      numPattern=numPattern>>1; if (playerColor==2) numPattern++;
      // GoTools::print("%i %i\n", numPattern, isInterestingPrecomputed.size());
      int t=isInterestingPrecomputed[numPattern];
      if (t>=0) {
        if (t&1)
          interestingMoves.push_back(location);
        if (t>=2)
          if (/*isAtariSure &&*/ isAtari(location))
            interestingMoves.push_back(location);
      } else {
        // static int bouh=0;
        // bouh++;
        // if (bouh%10000==0) GoTools::print("bouh %i\n", bouh);
        int s=interestingMoves.size();
        bool isAtariPossible=false;
        for (int j=0;j<4;j++)
          if ( gobanState[location+direction(j)] == 3-playerColor )
            isAtariPossible=true;



        if (isOnGobanSide(location)) {//FIXME hey, i changed only two lines here.
          if (locationInformation.isYoseOnSide(location))
            interestingMoves.push_back(location);
          //         continue;
        }
        //         else if (!(int)atariSavingMoves.size()) {/**assert(0);**/
        if (isHane(location))
          interestingMoves.push_back(location);
        else if (isCut(location))
          interestingMoves.push_back(location);
        //       else if (isCrossCut(location))
        //         interestingMoves.push_back(location);
        //       else if (isSolid(location))
        //         interestingMoves.push_back(location);
        if (interestingMoves.size()>s || isAtariPossible) {
          if (interestingMoves.size()>s && isAtariPossible)
            isInterestingPrecomputed[numPattern]=3;
          else if (isAtariPossible)
            isInterestingPrecomputed[numPattern]=2;
          else
            isInterestingPrecomputed[numPattern]=1;
        } else
          isInterestingPrecomputed[numPattern]=0;
        if (isAtari(location))
          interestingMoves.push_back(location);
      }
    }
  }








  while (interestingMoves.size() > 0) {
    int index=0;
    if (interestingMoves.size() > 1)
      index=MoGo::GoTools::mogoRand(interestingMoves.size());
    if (isNotUseless(interestingMoves[index]) && (!isSelfAtari(interestingMoves[index]))) {
      locationListIncludingThisLocation=getLocationListIncludingThisLocationForMovePlay(interestingMoves[index]);
      return interestingMoves[index];
    } else {
      interestingMoves[index]=interestingMoves[interestingMoves.size()-1];
      interestingMoves.pop_back();
    }
  }

  // GoTools::print("eating pref\n");
  //     showFreeLocation();
  //     printAllStringInformation();

  return randomOutOneNotUselessAvoidingSelfAtari(locationListIncludingThisLocation,thresholdSelfAtari);
}


Location MoGo::Goban::randomOutNonRandomPartOf2015441(bool also2015445) const {/**assert(0);**/
  if ( lastPlayedMove == PASSMOVE) {/**assert(0);**/
    return PASSMOVE;
  }

  underAtariStringsFirstPosition.clear();
  if (locationInformation.isLastMoveAtari(underAtariStringsFirstPosition)) {/**assert(0);**/
    for (int i=0;i<(int)underAtariStringsFirstPosition.size()-1;i++)
      for (int j=i+1;j<(int)underAtariStringsFirstPosition.size();j++)
        if (stringInformation.getStringLength(underAtariStringsFirstPosition[i]) < stringInformation.getStringLength(underAtariStringsFirstPosition[j])) {/**assert(0);**/
          Location tmp = underAtariStringsFirstPosition[i];
          underAtariStringsFirstPosition[i] = underAtariStringsFirstPosition[j];
          underAtariStringsFirstPosition[j] = tmp;
        }

    for (int i=0;i<(int)underAtariStringsFirstPosition.size();i++) {/**assert(0);**/
      Location savingMove = PASSMOVE;
      if (isUnderAtariStringSavableByEatingMove(underAtariStringsFirstPosition[i],savingMove)) {/**assert(0);**/
        assert(isLegal(savingMove));
        return savingMove;
      }
      savingMove = stringInformation.getOneLibertyOfString(underAtariStringsFirstPosition[i]);
      if(
        isLegal(savingMove) &&
        isAtariSavingMoveTrue(savingMove)) {/**assert(0);**/
        return savingMove;
      }
    }
    return PASSMOVE;
  }
  // textModeShowPosition(lastPlayedMove);
  // GoTools::print(" lastPlayedMove \n");
  assert(lastPlayedMove != PASSMOVE);
  interestingMoves.clear();
  Location location = PASSMOVE;
  for (int i=0;i<8;i++) {/**assert(0);**/
    //     location = liberties[i];
    location = lastPlayedMove + direction(i);
    if (isInterestingMoveBy2015441(location, also2015445))
      interestingMoves.push_back(location);
  }


  while (interestingMoves.size() > 0) {/**assert(0);**/
    int index=0;
    if (interestingMoves.size() > 1)
      index=MoGo::GoTools::mogoRand(interestingMoves.size());
    if (isNotUseless(interestingMoves[index]) && (!isSelfAtari(interestingMoves[index]))) {/**assert(0);**/
      return interestingMoves[index];
    } else {/**assert(0);**/
      interestingMoves[index]=interestingMoves[interestingMoves.size()-1];
      interestingMoves.pop_back();
    }
  }

  return PASSMOVE;
}

Location MoGo::Goban::saveStringOnAtari( ) const {/**assert(0);**/
  if ( lastPlayedMove == PASSMOVE) {/**assert(0);**/
    return PASSMOVE;
  }

  underAtariStringsFirstPosition.clear();
  if (locationInformation.isLastMoveAtari(underAtariStringsFirstPosition)) {/**assert(0);**/
    for (int i=0;i<(int)underAtariStringsFirstPosition.size()-1;i++)
      for (int j=i+1;j<(int)underAtariStringsFirstPosition.size();j++)
        if (stringInformation.getStringLength(underAtariStringsFirstPosition[i]) < stringInformation.getStringLength(underAtariStringsFirstPosition[j])) {/**assert(0);**/
          Location tmp = underAtariStringsFirstPosition[i];
          underAtariStringsFirstPosition[i] = underAtariStringsFirstPosition[j];
          underAtariStringsFirstPosition[j] = tmp;
        }

    for (int i=0;i<(int)underAtariStringsFirstPosition.size();i++) {/**assert(0);**/
      Location savingMove = PASSMOVE;
      if (isUnderAtariStringSavableByEatingMove(underAtariStringsFirstPosition[i],savingMove)) {/**assert(0);**/
        assert(isLegal(savingMove));
        return savingMove;
      }
      savingMove = stringInformation.getOneLibertyOfString(underAtariStringsFirstPosition[i]);
      if(
        isLegal(savingMove) &&
        isAtariSavingMoveTrue(savingMove)) {/**assert(0);**/
        return savingMove;
      }
    }
    return PASSMOVE;
  }
  return PASSMOVE;
}













Location MoGo::Goban::randomOutOneMoveMode2015445( LocationList *& locationListIncludingThisLocation, int thresholdSelfAtari ) const {/**assert(0);**/
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


  return randomOutOneNotUselessAvoidingSelfAtari(locationListIncludingThisLocation,thresholdSelfAtari);

}









void MoGo::Goban::getAllCloseLiberties(Location location, Vector<bool> &strNumberVisited, FastSmallVector<int> &liberties) const {/**assert(0);**/
  int strNumber=stringInformation.stringNumber[location];
  if (strNumberVisited[strNumber])
    return;
  strNumberVisited[strNumber]=true;

  for (int i=0;i<(int)stringInformation.stringPositions[strNumber].size();i++) {/**assert(0);**/
    for (int j=0;j<8;j++) {/**assert(0);**/
      if (liberties.size()>100)
        return;
      Location tmpLocation=stringInformation.stringPositions[strNumber][i]+direction(j);
      if (gobanState[tmpLocation]==EMPTY)
        liberties.push_back(tmpLocation);
      else if (gobanState[tmpLocation]!=OUTSIDE)
        getAllCloseLiberties(tmpLocation, strNumberVisited, liberties);
    }
  }
}
void MoGo::Goban::addAllClosestLiberties( Location locationOnString, Vector< Location > & frontLocations, Bitmap & bitmap ) const {/**assert(0);**/
  //   if (GobanBitmap::getBitValue(bitmap, locationOnString)) return;
  int strNumber=stringInformation.stringNumber[locationOnString];

  for (int i=0;i<(int)stringInformation.stringPositions[strNumber].size();i++) {/**assert(0);**/
    Location location=stringInformation.stringPositions[strNumber][i];
    GobanBitmap::setBitValue(bitmap, location, true);
    frontLocations.push_back(location);
 
  }
}





int MoGo::Goban::isInterestingMoveBy2015441( Location location, bool also2015445 ) const {/**assert(0);**/
  if (gobanState[location] == EMPTY) {/**assert(0);**/
    int numPattern=0;

    for (int j=0;j<8;j++) {/**assert(0);**/ numPattern+=gobanState[location+direction(j)];numPattern=numPattern<<2; }
    numPattern=numPattern>>1; if (playerColor==2) numPattern++;

    int t=isInterestingPrecomputed[numPattern];
    if (t>=0) {/**assert(0);**/
      if (t&1) {/**assert(0);**/
        if (!also2015445)
          return 1;
        else
          return !isLocallyWeakerMoveForSimulation(location);
      }
      if (t>=2)
        if (isAtari(location)) {/**assert(0);**/
          // GoTools::print("atari\n");
          if (!also2015445)
            return 1;
          else
            return !isLocallyWeakerMoveForSimulation(location);
        }
    } else {/**assert(0);**/
      bool isAtariPossible=false;
      int answer=0;

      if (!also2015445) {/**assert(0);**/
        for (int j=0;j<4;j++)
          if ( gobanState[location+direction(j)] == 3-playerColor )
            isAtariPossible=true;
      } else {/**assert(0);**/
        int minOpponentLiberties=1000;int friendLiberties=0;
        for (int j=0;j<4;j++) {/**assert(0);**/
          Location tmpLocation=location+direction(j);
          if ( gobanState[tmpLocation] == 3-playerColor ) {/**assert(0);**/
            isAtariPossible=true;
            int lib=getStringLiberty(tmpLocation);
            if (lib<minOpponentLiberties) minOpponentLiberties=lib;
          } else if (gobanState[tmpLocation] == playerColor) {/**assert(0);**/
            friendLiberties+=getStringLiberty(tmpLocation)-1;
          } else if (gobanState[tmpLocation] == EMPTY)
            friendLiberties++;
        }
        if (friendLiberties<minOpponentLiberties) return 0;
      }



      if (isOnGobanSide(location)) {/**assert(0);**///FIXME hey, i changed only two lines here.
        if (locationInformation.isYoseOnSide(location)) {/**assert(0);**/
          answer++;
          // GoTools::print("isYose\n");
        }
      }
      if (isHane(location))
        answer++;
      else if (isCut(location))
        answer++;
      //  GoTools::print("yose %i, hane %i, cut %i (answer %i)\n", locationInformation.isYoseOnSide(location), isHane(location), isCut(location), answer);
      if (answer>0 || isAtariPossible) {/**assert(0);**/
        if (answer>0 && isAtariPossible)
          isInterestingPrecomputed[numPattern]=3;
        else if (isAtariPossible)
          isInterestingPrecomputed[numPattern]=2;
        else
          isInterestingPrecomputed[numPattern]=1;
      } else
        isInterestingPrecomputed[numPattern]=0;
      if (isAtari(location))
        answer++;
      return answer;
    }
  }
  return 0;
}
double MoGo::Goban::scoreEyeLocationEdges( Vector< int > & eyeBlackEdge, Vector< int > & eyeWhiteEdge ) const {/**assert(0);**/
  Location location=leftUpLocation_;
  for (int i=0;i<heightOfGoban;i++,location += 2) {/**assert(0);**/
    for (int j=0;j<widthOfGoban;j++,location++) {/**assert(0);**/
      eyeBlackEdge[location]=0;eyeWhiteEdge[location]=0;
    }
  }
  double gobanScore=0.;
  location=leftUpLocation_;
  for (int i=0;i<heightOfGoban;i++,location += 2) {/**assert(0);**/
    for (int j=0;j<widthOfGoban;j++,location++) {/**assert(0);**/
      if (gobanState[location]==BLACK) {/**assert(0);**/
        gobanScore ++;
        //         positionScores[location]++;
      } else if (gobanState[location]==WHITE) {/**assert(0);**/
        gobanScore --;
        //         positionScores[location]--;
      } else if (gobanState[location]==EMPTY) {/**assert(0);**/
        int state=0;
        for (int k=0;k<4;k++)
          if (gobanState[location+direction(k)]==WHITE) {/**assert(0);**/ // The point can't go to BLACK
            if (state==1) // was black
            {/**assert(0);**/ state=0; break; }
            state=2;
          } else if (gobanState[location+direction(k)]==BLACK) {/**assert(0);**/ // The point can't go to WHITE
            if (state==2) // was white
            {/**assert(0);**/ state=0; break; }
            state=1;
          }

        if (state==1) {/**assert(0);**/ gobanScore++; } else if (state==2) {/**assert(0);**/ gobanScore--; }
        for (int k=0;k<4;k++) {/**assert(0);**/
          Location tmpLocation=location+direction(k);
          if (gobanState[tmpLocation]==OUTSIDE || gobanState[tmpLocation]==EMPTY)
            continue;
        if (state==1) {/**assert(0);**/eyeBlackEdge[tmpLocation]++; } else if (state==2) {/**assert(0);**/ eyeWhiteEdge[tmpLocation]++; }

        }

      }
    }
  }
  return gobanScore;
}






Location MoGo::Goban::koModeMove( LocationList *& locationListIncludingThisLocation, int  ) const {/**assert(0);**/
  if (koModeMoveStep>0) {/**assert(0);**/
    /** koModeMoveStep == 1
    */

    Location moveToPlay=PASSMOVE;

    if (!isLastMoveAtari()) {/**assert(0);**/ // fail to have a threat => we have the ko
      if (gobanState[koModeMoveLocation]!=EMPTY) {/**assert(0);**/
        // textModeShowPosition(koModeMoveLocation);
        // textModeShowGoban();
        // assert(0);
        koModeMoveStep=0;
        koModeMoveLocation=0;
        return PASSMOVE;
      }
      Location stoneAlone=PASSMOVE;
      for (int j=0;j<4;j++) {/**assert(0);**/
        Location tmpLocation=koModeMoveLocation+direction(j);
        if ((gobanState[tmpLocation]==playerColor || gobanState[tmpLocation]==3-playerColor) &&
            getStringLength(tmpLocation)==1 &&
            getStringLiberty(tmpLocation)==1)
          stoneAlone=tmpLocation;
      }
      if (stoneAlone==PASSMOVE) {/**assert(0);**/
        // textModeShowPosition(koModeMoveLocation);
        // textModeShowGoban();
        // assert(0);
        koModeMoveStep=0;
        koModeMoveLocation=0;
        return PASSMOVE;
      }
      if (gobanState[stoneAlone]==playerColor) {/**assert(0);**/ // we won the ko!!! Let's enjoy
        for (int j=0;j<4;j++) {/**assert(0);**/
          Location tmpLocation=stoneAlone+direction(j);
          if (gobanState[tmpLocation]==3-playerColor &&
              getStringLiberty(tmpLocation)==1)
            moveToPlay=stringInformation.getOneLibertyOfString(tmpLocation);
        }
        if (moveToPlay!=PASSMOVE && (!isNotUseless(moveToPlay))) moveToPlay=PASSMOVE;

        if (moveToPlay==PASSMOVE && (!isSelfAtari(koModeMoveLocation)) && isNotUseless(koModeMoveLocation))
          moveToPlay=koModeMoveLocation;
      } else {/**assert(0);**/ // retake the ko
        if (isNotUseless(koModeMoveLocation)) {/**assert(0);**/
          moveToPlay=koModeMoveLocation;
          koModeMoveStep=2;
        }
      }
    }
    if (moveToPlay!=PASSMOVE) {/**assert(0);**/
      koModeMoveStep=0;
      koModeMoveLocation=0;
    }
    return moveToPlay;
  }
 

  if (ko==PASSMOVE /*|| (!isSelfAtari(ko))*/) {/**assert(0);**/
    koModeMoveStep=0;
    koModeMoveLocation=PASSMOVE;
    return PASSMOVE;
  }
  /*  GoTools::print("here\n");
    textModeShowPosition(ko);
    GoTools::print("ko\n");
    textModeShowGoban(); //FIXME*/
  locationListIncludingThisLocation=0;
  Location location=chooseThreatingMoveRandomly();
  koModeMoveStep=1;
  koModeMoveLocation=ko;
  return location;
}


























Location  MoGo::Goban::randomOutOneNotUselessAvoidingSelfAtariBigEatsPrefered( LocationList *& locationListIncludingThisLocation, int thresholdSelfAtari ) const {/**assert(0);**/
  //  GoTools::print("randomOutOneNotUselessMoveEatingPreferedAvoidingSelfAtari\n");
  LocationList *opponentSelfKillLocation = 0;
  EatingLocationListType *playerEatingLocation = 0;
  if (playerColor==BLACK) {/**assert(0);**/
    opponentSelfKillLocation=&gobanLists.whiteSelfKillLocation;
    playerEatingLocation=&gobanLists.blackEatingLocation;
  } else {/**assert(0);**/
    opponentSelfKillLocation=&gobanLists.blackSelfKillLocation;
    playerEatingLocation=&gobanLists.whiteEatingLocation;
  }

  Location l=randomOutOneNotUselessMoveEatingPrefered(locationListIncludingThisLocation);
  removedLocationFromGobanAvoidingSelfAtariFreeLocation.clear();
  removedLocationFromGobanAvoidingSelfAtariEatingLocation.clear();
  removedLocationFromGobanAvoidingSelfAtariSelfKillLocation.clear();
  //       GoTools::print("proposed random (%i %i)\n", getStringLength(l), playerEatingLocation->isLocationIn(l)); textModeShowPosition(l);
  // showFreeLocation();
  while (l!=PASSMOVE && (avoidThisSelfAtariInSimulations(l, thresholdSelfAtari) || (lengthOfEatString(l)<__thresholdEatingMove && playerEatingLocation->isLocationIn(l)))) {/**assert(0);**/
    if (lengthOfEatString(l)<__thresholdEatingMove && playerEatingLocation->isLocationIn(l)) {/**assert(0);**/} else {/**assert(0);**/
      //       GoTools::print("proposed move eating\n"); textModeShowPosition(l);
      if (gobanLists.freeLocation.isLocationIn(l)) {/**assert(0);**/
        gobanLists.freeLocation.deleteLocation(l);
        removedLocationFromGobanAvoidingSelfAtariFreeLocation.push_back(l);
      } else {/**assert(0);**/
        assert(opponentSelfKillLocation->isLocationIn(l));
        opponentSelfKillLocation->deleteLocation(l);
        removedLocationFromGobanAvoidingSelfAtariSelfKillLocation.push_back(l);
      }
    }
    if (playerEatingLocation->isLocationIn(l)) {/**assert(0);**/
      playerEatingLocation->deleteLocation(l);
      removedLocationFromGobanAvoidingSelfAtariEatingLocation.push_back(l);
    }
    //     const_cast<MoGo::Goban *>(this)->gobanState[l]=OUTSIDE;
    l=randomOutOneNotUselessMoveEatingPrefered(locationListIncludingThisLocation);
  }
  for (int i=0;i<(int)removedLocationFromGobanAvoidingSelfAtariFreeLocation.size();i++)
    gobanLists.freeLocation.addLocation(removedLocationFromGobanAvoidingSelfAtariFreeLocation[i]);
  for (int i=0;i<(int)removedLocationFromGobanAvoidingSelfAtariEatingLocation.size();i++)
    playerEatingLocation->addLocation(removedLocationFromGobanAvoidingSelfAtariEatingLocation[i]);
  for (int i=0;i<(int)removedLocationFromGobanAvoidingSelfAtariSelfKillLocation.size();i++)
    opponentSelfKillLocation->addLocation(removedLocationFromGobanAvoidingSelfAtariSelfKillLocation[i]);

  return l;
}

//
// C++ Implementation: fastboard_mg
//
// Description:
//
//
// Author: Sylvain Gelly <sylvain.gelly@lri.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "fastboard_mg.h"
#include "goban_mg.h"
#include "gotools_mg.h"
#include "goexperiments_mg.h"
#include "cputimechronometer_mg.h"
#include "gobannode_mg.h"
#include "initiatefastboard_mg.h"


#include <assert.h> ///TODO A_EFFACER

#define COEF_CACHE_MISS_FAST_BOARD ((1024))

extern double torsion;//int handicapLevel;

using namespace MoGo;

// #define DEBUG_ALL_FastBoard
// #define DEBUG_ALL_FastBoard2


namespace MoGo {

  int FBS_handler::board_size        = 9;
  int FBS_handler::board_area        = board_size * board_size;
  int FBS_handler::board_big_area        = (board_size+2) * (board_size+2);
  int FBS_handler::max_playout_length  = board_area * 20;
  int FBS_handler::dNS = (FBS_handler::board_size + 2);
  int FBS_handler::dWE = 1;
  int FBS_handler::directions[8];
  int FBS_handler::mercy_threshold=25;
  int FBS_handler::mercyThresholdDivider=4;




  //   int FastBoard::avoidSuicide;
  int FastBoard::precomputedValues[65536*2];
  int FastBoard::precomputedValues4[256*2];
  int FastBoard::precomputedValues_nb[256*2];
  int FastBoard::simplePatternDatabase[65536*2];
  int FastBoard::vitalPointPatternDatabase[65536*2];

  int FastBoard::playNearMode;
  int FastBoard::stopPlayNearMode;
  int FastBoard::startPlayNearMode;
  int FastBoard::ignoreEndKo;
  int FastBoard::finishBoard=0;
  int FastBoard::initiateByFile=0;
  int FastBoard::scoreMode=0;
  int FastBoard::japanese_rules=0;
  int FastBoard::nakade=0;
  int FastBoard::fillboard=0;
  double FastBoard::factorForSide=0.;

  double FastBoard::approachProbability=0.5;

// TEMPORARY COMMENT: THIS FUNCTION IS USED
  FastBoard::FastBoard() {
    clear();
  }


  FastBoard::~FastBoard() {/**assert(0);**/}



  enum play_ret_t { play_ok, play_suicide, play_ss_suicide, play_ko, play_non_empty };


// TEMPORARY COMMENT: THIS FUNCTION IS USED
  void FastBoard::clear () {

    setKomi (7.5);
    stonesPerPlayer[0]=0;stonesPerPlayer[1]=0;
    freeLocationsIndex=0;freeLocationsLastIndex=0;
#ifdef GOLDENEYE_MC
    semeaiW.resize(0);
    semeaiB.resize(0);
#endif
    ko=PASSMOVE;
    for (int i=0;i<FBS_handler::board_big_area;i++) {
      state[i]=FB_EDGE;
#ifdef KEEP_TRACK_OF_CAPTURES_IN_FAST_BOARD
      locationCaptured[i]=0;
      captureIndex=0;
#endif
      chainNext[i]=i;
      chainId[i]=i;
      chainTrueLib[i]=100;
      chainLength[i]=0;
      intersectionUpdated[i]=0;
      intersectionUpdatedIndex=0;
      chainIdUpdatedIndex=0;


      blackEatenStoneNumber=0;
      whiteEatenStoneNumber=0;


      if (FBS_handler::isOnBoard(i)) {
        state[i]=FB_EMPTY;
        freeIndex[i]=freeLocationsIndex; freeLocations[freeLocationsIndex++]=i;
      }
    }

    for (int i=0;i<FBS_handler::board_big_area;i++) {
      if (!FBS_handler::isOnBoard(i)) continue;
      //       printLocation(i);
      neighborhood[i]=getNeighborhood8(i);
    }

    lastPlayedMove=0;
    //     lastMoveWasAtari=-1;
    isBlackTurn=1;
#ifdef KEEP_CAPTURE_MOVES_PER_NODE
    oneCaptureOccured=false;
#endif

  }

  void FastBoard::playPass() {
    ko=PASSMOVE;
    isBlackTurn=!isBlackTurn;
    lastPlayedMove=PASSMOVE;
  }

  // TEMPORARY COMMENT: THIS FUNCTION IS USED
  /*all_inline */int FastBoard::playNoPass(Location location) {
    //     lastMoveWasAtari=-1;
    chainIdUpdatedIndex=chainIdUpdatedIndex>intersectionUpdatedIndex
                        ?chainIdUpdatedIndex:intersectionUpdatedIndex;
    chainIdUpdatedIndex++;
    intersectionUpdatedIndex=chainIdUpdatedIndex;

    if (state[location]!=FB_EMPTY) return play_non_empty;
    //     if (isOpponentEye4(getNeighborhood4Fast(location), isBlackTurn))
    if (isOpponentEye4(neighborhood[location]&0xFF, isBlackTurn))
      return playEye (location);
    else
      return playNoEye (location);
  }

// TEMPORARY COMMENT: THIS FUNCTION IS USED
  int MoGo::FastBoard::playNoPassFaster( Location location ) {
    //     lastMoveWasAtari=-1;
    chainIdUpdatedIndex=chainIdUpdatedIndex>intersectionUpdatedIndex
                        ?chainIdUpdatedIndex:intersectionUpdatedIndex;
    chainIdUpdatedIndex++;
    intersectionUpdatedIndex=chainIdUpdatedIndex;

    //     if (isOpponentEye4(getNeighborhood4Fast(location), isBlackTurn))
    if (isOpponentEye4(neighborhood[location]&0xFF, isBlackTurn))
      return playEye (location);
    else
      return playNoEyeFaster (location);
  }

 // TEMPORARY COMMENT: THIS FUNCTION IS USED
  bool FastBoard::isSelfSuicideMove(int v) const {
    // if (getFreeNeighborhood(neighborhood[v]&0xFF)>0) return false;
    for (int i=0;i<4;i++) {
      Location location=v+FBS_handler::directions[i];
      if (state[location]==FB_EMPTY) return false;
      else if (state[location]==isBlackTurn) { // same color
        if (chainTrueLib[chainId[location]]>1) return false; // saved
      } else if (state[location]==isBlackTurn^1) {
        if (chainTrueLib[chainId[location]]==1) return false; // capture
      }
    }
    return true;
  }

// TEMPORARY COMMENT: THIS FUNCTION IS USED
  int FastBoard::playNoEye(int v) {
    if (/*avoidSuicide && */isSelfSuicideMove(v)) {
      return play_ss_suicide;
    }

    freeLocationsLastIndex = freeLocationsIndex;
    ko = PASSMOVE;

    placeStone (v);
    for (int i=0;i<4;i++) {
      Location location=v+FBS_handler::directions[i];
      processNewNeighborhood(location,v);
      addNeighborhood(i,location);
    }
    for (int i=4;i<8;i++) {
      Location location=v+FBS_handler::directions[i]; addNeighborhoodCorner(i,location);
    }
    for (int i=0;i<4;i++)
      removeChainNewNeighborhood(v+FBS_handler::directions[i],v);

    isBlackTurn=!isBlackTurn;
    return play_ok;
  }

  /*no_inline */int FastBoard::playEye (int v) {
    if (v == ko)
      return play_ko;

    uint all_nbr_live = true;
    for (int i=0;i<4;i++)
      all_nbr_live &= chainTrueLib[chainId [v+FBS_handler::directions[i]]]>1;

    if (all_nbr_live) {
#ifdef DEBUG_ALL_FastBoard2
      GoTools::print("play_ss_suicide2\n");
      printLocation(v);
#endif
      return play_ss_suicide;
    }


    freeLocationsLastIndex = freeLocationsIndex;

    placeStone(v);
    for (int i=0;i<4;i++) {
      Location location=v+FBS_handler::directions[i];
      /*if (state[location]==FB_EMPTY)*/ addNeighborhood(i,location);
      /*else*/ if (state[location]<FB_EMPTY) {
        uint id=chainId[location];
        if (intersectionUpdated[id]<chainIdUpdatedIndex) {// not already updated by the stone
          intersectionUpdated[id]=chainIdUpdatedIndex;
          chainTrueLib[id] --;
        }
      }
    }
    for (int i=4;i<8;i++) {
      Location location=v+FBS_handler::directions[i]; addNeighborhoodCorner(i,location);
    }
    for (int i=0;i<4;i++) {
      Location location=v+FBS_handler::directions[i];
      if (state[location]==(!isBlackTurn)) // opponent color
        if ((chainTrueLib [chainId [location]] == 0))
          removeChain (location);
    }

    if (freeLocationsLastIndex == freeLocationsIndex) { // only case of ko
      ko = freeLocations [freeLocationsIndex - 1]; // then ko
      // static int countKo=0; countKo++; if (countKo%1000==0) GoTools::print("countKo %i\n", countKo);
      // print(); getchar();
    } else {
      ko = PASSMOVE;
    }

    isBlackTurn=!isBlackTurn;
    return play_ok;
  }


// TEMPORARY COMMENT: THIS FUNCTION IS USED
  int MoGo::FastBoard::playNoEyeFaster( int v ) {
    freeLocationsLastIndex = freeLocationsIndex;
    ko = PASSMOVE;
    //     lastMoveWasAtari=0;

    placeStone (v);
    for (int i=0;i<4;i++) {
      Location location=v+FBS_handler::directions[i];
      processNewNeighborhood(location,v);
      addNeighborhood(i,location);
    }
    for (int i=4;i<8;i++) {
      Location location=v+FBS_handler::directions[i]; addNeighborhoodCorner(i,location);
    }
    for (int i=0;i<4;i++)
      removeChainNewNeighborhood(v+FBS_handler::directions[i],v);

    isBlackTurn=!isBlackTurn;
    return play_ok;
  }



// TEMPORARY COMMENT: THIS FUNCTION IS USED
  void FastBoard::processNewNeighborhood(Location newLocation, Location oldLocation) {
    if (state[newLocation]>=FB_EMPTY) return; // empty or edge

    if (chainId [newLocation] != chainId [oldLocation]) {
      uint id=chainId[newLocation];
      if (intersectionUpdated[id]<chainIdUpdatedIndex) {// not already updated by the stone
        intersectionUpdated[id]=chainIdUpdatedIndex;
        chainTrueLib[id] --;
      }
    }

    if (state[newLocation] != isBlackTurn) { // same color of groups
      return;
    }

    if (chainId [newLocation] == chainId [oldLocation]) return; // already joined

    if (chainLength[chainId [oldLocation]] >= chainLength [chainId [newLocation]])
      mergeChains (oldLocation, newLocation);
    else
      mergeChains (newLocation, oldLocation);

  }

  // TEMPORARY COMMENT: THIS FUNCTION IS USED
  void FastBoard::removeChainNewNeighborhood(Location newLocation, Location /*oldLocation*/) {
    if (state[newLocation]==1-isBlackTurn) {
      if (chainTrueLib [chainId [newLocation]] == 0) removeChain (newLocation);
      //       if (chainTrueLib [chainId [newLocation]] == 1) lastMoveWasAtari=1;
    }
  }

  // TEMPORARY COMMENT: THIS FUNCTION IS USED
  bool FastBoard::isOneLibertyOfChain(int v, int chain) const {
    // we here assume that v is empty
    for (int i=0;i<4;i++) {
      Location location=v+FBS_handler::directions[i];
      if (state[location]<FB_EMPTY && chainId[location]==chain)
        return true;
    }
    return false;
  }

  // TEMPORARY COMMENT: THIS FUNCTION IS USED
  void FastBoard::mergeChains (int baseChainLocation, int newChainLocation) {
    int currentLocation=newChainLocation;
    int baseId=chainId [baseChainLocation];
    chainLength[baseId]+=chainLength[chainId[newChainLocation]];
    intersectionUpdatedIndex++;
    do {
      if (getFreeNeighborhood(neighborhood[currentLocation]&0xFF)>0) {
        for (int i=0;i<4;i++) {
          Location location=currentLocation+FBS_handler::directions[i];
          if (state[location]==FB_EMPTY && intersectionUpdated[location]<intersectionUpdatedIndex) {

#ifdef DEBUG_ALL_FastBoard
            printf("merge_chain %i, %i %i (lib base %i, small %i) (isOnelib %i)\n", i, intersectionUpdated[location], intersectionUpdatedIndex,chainTrueLib[baseId],chainTrueLib[chainId[location]],isOneLibertyOfChain(location, baseId));
            printLocation(currentLocation);printLocation(location);
            printf("\n");
#endif



            if (!isOneLibertyOfChain(location, baseId))
              chainTrueLib[baseId]++;
            intersectionUpdated[location]=intersectionUpdatedIndex;
          }
        }
      }
      chainId [currentLocation] = chainId [baseChainLocation];
      currentLocation = chainNext[currentLocation];
    } while (currentLocation != newChainLocation);

    int tmp=chainNext[baseChainLocation];chainNext[baseChainLocation]=chainNext[newChainLocation];chainNext[newChainLocation]=tmp;
  }

  //   no_inline
  void FastBoard::removeChain (Location location) {
    Location currentLocation=location;
    Location tmpLocation;
#ifdef KEEP_TRACK_OF_CAPTURES_IN_FAST_BOARD
    captureIndex+=2;
#endif
#ifdef KEEP_CAPTURE_MOVES_PER_NODE
    oneCaptureOccured=true;
#endif
    currentLocation = location;
    do {
      removeStone (currentLocation);
      currentLocation = chainNext[currentLocation];
    } while (currentLocation != location);

    do {
      chainIdUpdatedIndex++;
      for (int i=0;i<4;i++) {
        Location tmpLocation2=currentLocation+FBS_handler::directions[i];
        removeNeighborhood(i,tmpLocation2);
        if ((state[tmpLocation2]<FB_EMPTY) && intersectionUpdated[chainId[tmpLocation2]]<chainIdUpdatedIndex) {
          chainTrueLib[chainId[tmpLocation2]]++;
          intersectionUpdated[chainId[tmpLocation2]]=chainIdUpdatedIndex;
        }
      }
      for (int i=4;i<8;i++) {
        Location location=currentLocation+FBS_handler::directions[i]; removeNeighborhoodCorner(i,location);
      }



      tmpLocation = currentLocation;
      currentLocation = chainNext[currentLocation];
      chainNext[tmpLocation] = tmpLocation;
    } while (currentLocation != location);
  }

  // TEMPORARY COMMENT: THIS FUNCTION IS USED
  void FastBoard::placeStone (Location location) {
    stonesPerPlayer[isBlackTurn]++;
    state[location] = isBlackTurn;

    freeLocationsIndex--;
    freeIndex [freeLocations [freeLocationsIndex]] = freeIndex [location];
    freeLocations [freeIndex [location]] = freeLocations [freeLocationsIndex];

    chainId [location] = location;
    chainTrueLib[chainId[location]] = getFreeNeighborhood(neighborhood[location]&0xFF);
    //     GoTools::print("nbLibs %i (neighborhood %i)\n", chainTrueLib[chainId[location]], neighborhood[location]);
    chainLength[chainId[location]]=1;
    lastPlayedMove=location;
#ifdef KEEP_CAPTURE_MOVES_PER_NODE
    oneCaptureOccured=false;
#endif

  }

// TEMPORARY COMMENT: THIS FUNCTION IS USED
  void FastBoard::removeStone (Location location) {
#ifdef KEEP_TRACK_OF_CAPTURES_IN_FAST_BOARD
    if (locationCaptured[location]==0)
      locationCaptured[location]=captureIndex+isBlackTurn;
#endif
    stonesPerPlayer[!isBlackTurn]--;
    state[location] = FB_EMPTY;

    // Modification pour le scoring japonais
    if (isBlackTurn) whiteEatenStoneNumber++;
    else blackEatenStoneNumber++;
	
    //     printLocation(location); GoTools::print("is removed (%i)\n", freeLocationsIndex);
    freeIndex [location] = freeLocationsIndex;
    freeLocations [freeLocationsIndex++] = location;
    chainId[location]=location;
    //     neighborhood[location]=getNeighborhood4(location);
  }

  // TEMPORARY COMMENT: THIS FUNCTION IS USED
  int FastBoard::approx_score () const {
    return stonesPerPlayer[FB_BLACK] -  stonesPerPlayer[FB_WHITE];
  }

  // TEMPORARY COMMENT: THIS FUNCTION IS USED
  int FastBoard::score () const {
    int eye_score;
    eye_score = 0;





 /* double torsion;
    double maxFuturSize=FBS_handler::board_size*FBS_handler::board_size;
      double pastSize=2.*()/maxFuturSize;
  if ((handicapLevel>0)&&(pastSize<1))
	    {
		    	torsion=(handicapLevel*7)*(1-pastSize);
			  }*/







    for (int i=0;i<freeLocationsIndex;i++) {
      int v=neighborhood[freeLocations[i]]&0xFF;
      if (isOpponentEye4(v,0)) eye_score++;
      else if (isOpponentEye4(v,1)) eye_score--;
    }
    //     GoTools::print("stonesPerPlayer[FB_BLACK] %i,  stonesPerPlayer[FB_WHITE] %i\n", stonesPerPlayer[FB_BLACK], stonesPerPlayer[FB_WHITE]);
    static double averageScore=0.;
    averageScore=0.99*averageScore+0.01*(approx_score() + eye_score);
 /*   if (drand48()<0.07)
    fprintf(stderr,"approx_score+eye_score=%d, torsion=%g\n",approx_score()+eye_score,torsion);*/
    return approx_score () + eye_score 
	//     - (averageScore-komi)/2;
#ifndef NODK
             -torsion
#endif
	     ;
  }

  // tentative de scoring en regle japonaise
  int FastBoard::japanese_score () const {
    int eye_score = 0;
    int eaten_stones_score = 0;


    for (int i=0;i<freeLocationsIndex;i++) {
      int v=neighborhood[freeLocations[i]]&0xFF;
      if (isOpponentEye4(v,0)) eye_score++;
      else if (isOpponentEye4(v,1)) eye_score--;
    }
	
    //     GoTools::print("stonesPerPlayer[FB_BLACK] %i,  stonesPerPlayer[FB_WHITE] %i\n", stonesPerPlayer[FB_BLACK], stonesPerPlayer[FB_WHITE]);

    eaten_stones_score =  whiteEatenStoneNumber - blackEatenStoneNumber;
	//GoTools::print("score pierres mang�s %i\n", eaten_stones_score);

    return eye_score + eaten_stones_score;
  }

  // TO DELETE: WHEN THE OPTION SCORE_MODE WILL BECOME OBSOLETE
  double MoGo::FastBoard::score( double coefForSide ) const {/**assert(0);**/
	assert(0);
    double coefForCenter=getFactorForCenter(coefForSide);
    double distanceToBorderMax=FBS_handler::board_size;
    //     GoTools::print("coefForSide %f, coefForCenter %f\n", coefForSide, coefForCenter);
    double score=0.;
    for (int i=0;i<FBS_handler::board_big_area;i++) {/**assert(0);**/
      if (!FBS_handler::isOnBoard(i)) continue;
      double v=owner(i);
      int row=GoTools::min(FBS_handler::row(i), FBS_handler::board_size-1-FBS_handler::row(i));
      int col=GoTools::min(FBS_handler::col(i), FBS_handler::board_size-1-FBS_handler::col(i));
      double distanceToBorder=row+col;
      distanceToBorder/=distanceToBorderMax;
      double thisCoef=coefForSide+(coefForCenter-coefForSide)*distanceToBorder;
      // printLocation(i); GoTools::print("distanceToBorder %f, thisCoef %f, v*thisCoef %f\n",distanceToBorder, thisCoef, v*thisCoef);
      //       int nb=nbOnGobanSide(i);
      //       if (nb>=2)
      //         thisCoef=coefForSide*2.;
      //       else if (nb==1)
      //         thisCoef=coefForSide;
      score+=v*thisCoef;

    }
    // print(); GoTools::print("score=%f\n", score);
    return score;
  }



}













































































using namespace std;

namespace MoGo {
  Vector<FastSmallVector<Location> >FastBoard::__adjacentFriendTwoLibertiesStrings;
  Vector<FastSmallVector<Location> >FastBoard::__interestingMoves;
  Vector<FastSmallVector<Location> >FastBoard::__underAtariStringsFirstPosition;

  Vector<FastBoard *> FastBoard::allBoards(NUM_THREADS_MAX);
  FastBoard *FastBoard::referenceBoard=0;
}






FastBoard * MoGo::FastBoard::fromGoban( const Goban & goban ) {/**assert(0);**/
#ifdef PARALLELIZED
  pthread_mutex_lock(&ThreadsManagement::threadsManagementMutex);
  FBS_handler::changeSize(goban.height());
  pthread_mutex_unlock(&ThreadsManagement::threadsManagementMutex);
#else
  FBS_handler::changeSize(goban.height());
#endif

  FastBoard *board=new FastBoard();
  for (int i=0;i<(int)goban.gobanState.size();i++) {/**assert(0);**/
    int v=goban.gobanState[i];
    if (v==BLACK) {/**assert(0);**/
      if (!board->isBlackTurn) board->playPass();
      board->playNoPass(i);
    } else if (v==WHITE) {/**assert(0);**/
      if (board->isBlackTurn) board->playPass();
      board->playNoPass(i);
    }
  }
  board->lastPlayedMove=goban.lastMove();
  board->isBlackTurn=goban.isBlacksTurn();
  board->ko=goban.koPosition();
  return board;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::FastBoard::fromGoban( const Goban & goban, FastBoard & board ) {
#ifdef PARALLELIZED
  pthread_mutex_lock(&ThreadsManagement::threadsManagementMutex);
  FBS_handler::changeSize(goban.height());
  pthread_mutex_unlock(&ThreadsManagement::threadsManagementMutex);
#else
  FBS_handler::changeSize(goban.height());
#endif

  board.clear();


  for (int i=0;i<(int)goban.gobanState.size();i++) {
    int v=goban.gobanState[i];
    if (v==BLACK) {
      if (!board.isBlackTurn) board.playPass();
      board.playNoPass(i);
    } else if (v==WHITE) {
      if (board.isBlackTurn) board.playPass();
      board.playNoPass(i);
    }
  }
  board.lastPlayedMove=goban.lastMove();
  board.isBlackTurn=goban.isBlacksTurn();
  board.ko=goban.koPosition();
}


// TEMPORARY COMMENT: THIS FUNCTION IS USED
int MoGo::FastBoard::oneRollOut( int num, int choiceB, int choiceW, Vector< int > & allAtFirstData, FastSmallVector< int > & usedMovesAllAtFirst, double komi, int numThread ) {
#ifdef GOLDENEYE_MC
	semeai=true;
#endif
  setKomi(komi);
  double coefForSide=1.;
  if (scoreMode>0) {/**assert(0);**/
    int moveNumber=(stonesPerPlayer[0]+stonesPerPlayer[1]);
    if (moveNumber<scoreMode) coefForSide=(1.-double(moveNumber)/double(scoreMode))*(factorForSide-1.)+1.;
    //     coefForSide*=factorForSide;
    //     coefForSide++;
  }

  int intKomi=int(komi);
  int currentMercyThreshold=FBS_handler::mercy_threshold+abs(approx_score()-intKomi);//GoTools::max(abs(approx_score()-intKomi)-FBS_handler::mercy_threshold,0);



  int numIterationAllAtFirst=allAtFirstData[0];
  // printf("blackToPlay %i\n", blackToPlay);
  int       move_no;
  int pass=0;
  int status;
  move_no     = 0;
  int lastLastKo=ko;
  int lastKo=ko;

  int print=0;
  do {
    lastLastKo=lastKo;
    lastKo=ko;
    Location location=playOne(num, choiceB, choiceW, numThread);

#define PASPRINTTOUTLETEMPS
#ifndef PASPRINTTOUTLETEMPS
         //Goban goban(19,19); toGoban(goban); goban.textModeShowGoban(location); getchar();
	if ((semeaiW.size()>0)||(semeaiB.size()>0)||print)
	{
		print=1;
	Goban goban(19,19); toGoban(goban); goban.textModeShowGoban(location); sleep(1);
	}
#endif

    if (location!=PASSMOVE) {
      if (allAtFirstData[location]<=numIterationAllAtFirst) // if no stone has been played here before
        if (isBlackTurn) { // so it was a white stone
          allAtFirstData[location]=numIterationAllAtFirst+1;
#ifdef KEEP_CAPTURE_MOVES_PER_NODE
          Location locTmp=location+(getOneCaptureOccured()<<16); usedMovesAllAtFirst.push_back(-locTmp);
#else
          usedMovesAllAtFirst.push_back(-location);
#endif

        } else { // so it was a black stone played
          allAtFirstData[location]=numIterationAllAtFirst+2;
#ifdef KEEP_CAPTURE_MOVES_PER_NODE
          Location locTmp=location+(getOneCaptureOccured()<<16); usedMovesAllAtFirst.push_back(locTmp);
#else
          usedMovesAllAtFirst.push_back(location);
#endif

        }
      pass=0;
    } else {
      pass++;
    }

    move_no++;
#ifdef DEBUG_ALL_FastBoard2
    print();GoTools::print("ko %i, freeLocationsIndex %i, freeLocations [freeLocationsIndex - 1] %i, isOpponentEye4Slow(location) %i\n", ko, freeLocationsIndex, freeLocations [freeLocationsIndex - 1], isOpponentEye4Slow(location)); //getchar();
#endif
    if ((pass==2) && (ignoreEndKo || (lastLastKo==PASSMOVE)))    { status=0; break;}
    if (int (abs (approx_score ()-intKomi)) > currentMercyThreshold)  { status=1; break;}
    if (move_no >= FBS_handler::max_playout_length)                          { status=2; break;}
  } while (true);


   // printf("number=%d status=%d   (maxplayout=%d)\n",move_no,status,FBS_handler::max_playout_length);fflush(stdout);



  int blackWins=0;
  switch (status) {/**assert(0);**/
      case 0:
      if (japanese_rules == 1)
	blackWins=japanese_score()>komi;
      else 
	{ 
		if (scoreMode==0)
        		blackWins=score()>komi;
	       else
        		blackWins=score(coefForSide)>komi;
	}
      break;
      case 1:
      blackWins=approx_score()>0;
      // print(); GoTools::print("blackWins %i\n", blackWins);getchar();
      break;
      case 2:
      break;
  }
  return blackWins;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::FastBoard::setEnvironnement( int argc, char ** argv ) {
  FastBoard::initAll();
  //   pm::srand (GoTools::mogoRand());
  //   int avoidSuicide=1; GoExperiments::findArgValue(argc, argv, "avoidSuicide", avoidSuicide); FastBoard::avoidSuicide=avoidSuicide;
  int initiateByFile=1;GoExperiments::findArgValue(argc,argv,"initiateByFile",initiateByFile);
  FastBoard::initiateByFile=initiateByFile;

  int useFastBoard=0;GoExperiments::findArgValue(argc,argv,"useFastBoard",useFastBoard);
  if (useFastBoard>1) {
    precomputeSimplePatternDatabase();
    precomputeVitalPointPatternDatabase();
  }
  int playNearMode=0;GoExperiments::findArgValue(argc,argv,"playNearMode",playNearMode);
  int stopPlayNearMode=100000;GoExperiments::findArgValue(argc,argv,"stopPlayNearMode",stopPlayNearMode);
  int startPlayNearMode=0;GoExperiments::findArgValue(argc,argv,"startPlayNearMode",startPlayNearMode);
  int ignoreEndKo=1;GoExperiments::findArgValue(argc,argv,"ignoreEndKo",ignoreEndKo);
  int finishBoard=0;GoExperiments::findArgValue(argc,argv,"finishBoard",finishBoard);
  FastBoard::playNearMode=playNearMode;
  FastBoard::stopPlayNearMode=stopPlayNearMode;
  FastBoard::startPlayNearMode=startPlayNearMode;
  FastBoard::ignoreEndKo=ignoreEndKo;
  FastBoard::finishBoard=finishBoard;

  int gobanSize=9;GoExperiments::findArgValue(argc,argv,"gobanSize",gobanSize);
  FBS_handler::changeSize(gobanSize);
}









// TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::FastBoard::saveLastMoveAtari( FastSmallVector< Location > & underAtariStringsFirstPosition, Location & savingMove, int numThread ) const {
  for (int i=0;i<(int)underAtariStringsFirstPosition.size()-1;i++)
    for (int j=i+1;j<(int)underAtariStringsFirstPosition.size();j++)
      if (chainLength[chainId[underAtariStringsFirstPosition[i]]] < chainLength[chainId[underAtariStringsFirstPosition[j]]]) {
        Location tmp = underAtariStringsFirstPosition[i];
        underAtariStringsFirstPosition[i] = underAtariStringsFirstPosition[j];
        underAtariStringsFirstPosition[j] = tmp;
      }

  for (int i=0;i<(int)underAtariStringsFirstPosition.size();i++) {
    savingMove = PASSMOVE;

    if (isUnderAtariStringSavableByEatingMove(underAtariStringsFirstPosition[i],savingMove))
      return true;

    savingMove = getOneLibertyOfString(underAtariStringsFirstPosition[i]);

    if( /*isLegal(savingMove) &&*/
      isAtariSavingMoveTrue(savingMove, numThread))
      return true;
  }
  return false;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::FastBoard::isUnderAtariStringSavableByEatingMove( Location underAtariString, Location & savingMove ) const {
  int enemyColor = 1^state[underAtariString];
  //   int strNb = chainId[underAtariString];
#ifdef DEBUG_ALL_FastBoard
  GoTools::print("isUnderAtariStringSavableByEatingMove\n");
  printPosition(underAtariString);
#endif

  int current = underAtariString;
  do {
    for (int j=0;j<4;j++) {
      int v=current+FBS_handler::directions[j];
      if (state[v]==enemyColor && liberty(v)==1) {
        savingMove=getOneLibertyOfString(v);
        if (savingMove!=int(ko)) return true;
      }
    }
    current = chainNext[current];
  } while (int(current) != underAtariString);

  return false;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
Location MoGo::FastBoard::getOneLibertyOfString( Location location) const {
  int current = location;
  do {

    int firstDirection=firstFreeDirection(neighborhood[current]&0xFF);
    if (firstDirection<4) return current+FBS_handler::directions[firstDirection];
 
    current = chainNext[current];
  } while (int(current) != location);

  assert(0);
  return PASSMOVE;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::FastBoard::isAtariSavingMoveTrue( const Location savingMove, int numThread ) const {
 

  if( (savingMove!=(int)ko) &&
      ((!isOnGobanSide(savingMove) && (!isSelfSuicideMove(savingMove))) ||
       isConnectingMove(savingMove))  && (!isSelfAtari(savingMove, numThread)))
    return true;
  return false;
}


// TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::FastBoard::isOnGobanSide( const Location move ) const { //TODO faster
  //   return (precomputedValues4[neighborhood[move]]>>7)&1;
  for (int j=0;j<4;j++) {
    int v=move+FBS_handler::directions[j];
    if (state[v]==FB_EDGE)
      return true;
  }
  return false;
}




// TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::FastBoard::isConnectingMove( const Location move ) const {
  for (int i=0;i<4;i++) {
    int v=move+FBS_handler::directions[i];
    if (int(state[v]) == isBlackTurn && (liberty(v)>1)/*&& (chain_lib_cnt[chainId[v]]>1)*/)
      return true;
  }
  return false;
}






















// TEMPORARY COMMENT: THIS FUNCTION IS USED
int MoGo::FastBoard::isSelfAtari( const Location move, int numThread ) const {
  //   if (goban->gobanState[location]!=EMPTY) return 0;
  FastSmallVector<Location> &adjacentFriendTwoLibertiesStrings=__adjacentFriendTwoLibertiesStrings[numThread*COEF_CACHE_MISS_FAST_BOARD];
  int freeL=getFreeNeighborhood(neighborhood[move]&0xFF);
  if (freeL>=2) {
    return 0;
  }
  int opponent=isBlackTurn^1;
  int free = 0;
  int length = 1;
  Location firstFreeLocation = PASSMOVE;
  adjacentFriendTwoLibertiesStrings.clear();
  for (int i=0;i<4;i++) {
    Location tmp=move+FBS_handler::directions[i];
    int stateTmp=state[tmp]; int strNumber=chainId[tmp];
    if (stateTmp==FB_EMPTY) {
      if (free == 0) firstFreeLocation = tmp;
      free++;
      if (free == 2) return 0;
    } else if (stateTmp==isBlackTurn) { // friend
      int liberties=liberty(tmp);//chain_lib_cnt[strNumber];
      if (liberties>=3)
        return 0;
      if (liberties<=1) {
        length+=chainLength[strNumber];
        continue;
      }
      bool found=false;
      for (int j=0;j<(int)adjacentFriendTwoLibertiesStrings.size();j++)
        if (adjacentFriendTwoLibertiesStrings[j]==strNumber)
          found=true;
      if (found) continue;
      length+=chainLength[strNumber];
      adjacentFriendTwoLibertiesStrings.push_back(strNumber);
    } else if (stateTmp == opponent &&
               liberty(tmp)==1) {
      return 0;// Yizao thinks it is good to do that. I trust him :-)
    }
  }

  if ((free==1 && adjacentFriendTwoLibertiesStrings.size() == 0)
      || (free==0 && adjacentFriendTwoLibertiesStrings.size() == 1))
    return length ;
  if (free==1) {
    assert(firstFreeLocation!=PASSMOVE);
    for (int i=0;i<(int)adjacentFriendTwoLibertiesStrings.size();i++)
      for (int j=0;j<4;j++)
        if (int(chainId[firstFreeLocation+FBS_handler::directions[j]])==adjacentFriendTwoLibertiesStrings[i])
          return length ;
    return 0;

    return length;
  }

  for (int i=0;i<(int)adjacentFriendTwoLibertiesStrings.size()-1;i++) {
    if (!areTwoStringsSharingTheSameTwoLiberties(adjacentFriendTwoLibertiesStrings[i],adjacentFriendTwoLibertiesStrings[i+1]))
      return 0;
  }
  //   if (adjacentFriendTwoLibertiesStrings.size()>1)
  //     return 0;
  //   else
  return length;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
int MoGo::FastBoard::playOne( int /*num*/, int choiceB, int choiceW, int numThread ) 
{
  return playOne3(choiceB,choiceW, numThread);
}


// TEMPORARY COMMENT: THIS FUNCTION IS USED
int MoGo::FastBoard::coreOfRandomMode44( int thresholdSelfAtari, int numThread ) {

 assert(lastPlayedMove!=0);
  Location location = PASSMOVE;

 
  if (nakade) {
  // Debut ajout Point vital
  Location center = PASSMOVE;

  for (int i=0;i<4;i++) {
	location = lastPlayedMove+FBS_handler::directions[i];

	//if ( (state[location] != FB_EDGE) && (vitalPointPatternDatabase[patternValue(location)] == 1 ) ) 
	if ( (state[location] == FB_EMPTY) || (state[location] == isBlackTurn) )
		center = getVitalPoint(location);


	
        if (center>0) {
		 int val=playNoPass (center);
        	 if (val < play_ss_suicide) return center;
	}			
      }
  // Fin ajout Point vital  
  }

  location = PASSMOVE;
  FastSmallVector<Location> &underAtariStringsFirstPosition=__underAtariStringsFirstPosition[numThread*COEF_CACHE_MISS_FAST_BOARD];
  //   if (ko==PASSMOVE) {
  underAtariStringsFirstPosition.clear();
  if (isLastMoveAtari(underAtariStringsFirstPosition))
    if (saveLastMoveAtari(underAtariStringsFirstPosition, location, numThread)) {
      playNoPassFaster(location);
      return location;
    }  else    return playOneAvoidBigSelfAtari (thresholdSelfAtari, numThread);
  //   }

  //   assert(lastPlayedMove != PASSMOVE); //TODO

  return -10;
}


// TEMPORARY COMMENT: THIS FUNCTION IS USED
// Joue aux emplacements vide d'abord
int MoGo::FastBoard::coreOfRandomMode45(  ) {

  Location location = PASSMOVE;

  if (fillboard) {  
  bool is_alone = true; 
   // find random place in vector of empty vertices
    int start = GoTools::mogoRand2(freeLocationsIndex);

    // search for a move in start ... freeLocations_cnt-1 interval
    for (int j = start; ( (j != freeLocationsIndex) && ((j-start)<6)); j++) {
      location = freeLocations [j];
	is_alone = true;
      for (int i=0;i<8;i++) {
        if (state[location+FBS_handler::directions[i]] != FB_EMPTY)
	{is_alone = false;break;}
      }

      if ( is_alone) {
	playNoPassFaster(location);
    	return location;
	}

    
   }
  }
  return -10;
}


int MoGo::FastBoard::playCapture()
{
  int choice = MoGo::GoTools::mogoRand2(10);
  //int choice = rand()%10;
  
  if(choice!=0)
    return PASSMOVE;
  
  if(lastPlayedMove!=PASSMOVE)
    if(liberty(lastPlayedMove)==1)
      return getOneLibertyOfString(lastPlayedMove);
  
  return PASSMOVE;
}

//
Location MoGo::FastBoard::getVitalPoint(Location location) {
  
  // location doit etre un emplacement vide
  
  // for (int i=0;i<4;i++) {
  // 		location_studied = vitalPoint+FBS_handler::directions[i];
  // 		if( state[location_studied] == FB_EMPTY)
  // 			return vitalPoint;
  // 	}
  
  /*
    return vitalPoint;
    //	return PASSMOVE;
    */

  /*
    Location location_studied = PASSMOVE;
    Location location_vital=PASSMOVE;
    int l,direction;
    
    for (int i=0;i<4;i++){
    location_studied=location+FBS_handler::directions[i];
    l=precomputedValues_nb[getNeighborhood4Fast(location_studied)];
    if ((l&3)==1){
    location_vital=location_studied+FBS_handler::directions[direction=(l>>5)&3];
    l=precomputedValues_nb[getNeighborhood4Fast(location_vital)];
    if (((l&3)==2)&&(((l>>3)&3)==isBlackTurn)){
    location_studied=location_vital+((-direction==((l>>5)&3))?((l>>7)&3):((l>>5)&3));
    l=precomputedValues_nb[getNeighborhood4Fast(location_studied)];
    if (((l>>3)&3)==isBlackTurn){
    if ((l&3)==1) return location_vital;
    else if ((l&3)==2){
    location_studied+=((-direction==((l>>5)&3))?((l>>7)&3):((l>>5)&3));
    l=precomputedValues_nb[getNeighborhood4Fast(location_studied)];
    if (((l&3)==1)&&(((l>>3)&3)==isBlackTurn)) return location_vital;
    }
    }
    }
    }
    }
    return PASSMOVE;
  */
  
  
  //      return PASSMOVE;
  
       Location location_studied = location;
       Location location_vital=PASSMOVE;
       int l,direction;
               l=precomputedValues_nb[getNeighborhood4Fast(location_studied)+((isBlackTurn)<<8)];
               switch (l&7){
               case 1:
                       location_vital=location_studied+FBS_handler::directions[direction=(l>>5)&3];
                       l=precomputedValues_nb[getNeighborhood4Fast(location_vital)+((isBlackTurn)<<8)];
                       //if (debug) printf("\nTo verify the second location %i\n",l);
                       if ((l&7)==2){
			       int temp=direction-((l>>5)&3);
                               location_studied=location_vital+FBS_handler::directions[((temp==2)||(temp==-2))?((l>>7)&3):((l>>5)&3)];
                               l=precomputedValues_nb[getNeighborhood4Fast(location_studied)+((isBlackTurn)<<8)];
                               if (((l&7)==1)&&(state[location_vital]==FB_EMPTY)){
                                              // if (!debug) printf("The new one is not in the old one 1!\n");
						return location_vital;
                               }
                       }
                       break;

               case 2:
                       if (((precomputedValues_nb[((neighborhood[location_studied]>>8)&255)+((isBlackTurn)<<8)]&7)<=1)&&(state[location_studied]==FB_EMPTY)){
                               location_studied=location+FBS_handler::directions[(l>>5)&3];
                               direction=(l>>7)&3;
                               l=precomputedValues_nb[getNeighborhood4Fast(location_studied)+((isBlackTurn)<<8)];
                               if ((l&7)==1){
                                       location_studied=location+FBS_handler::directions[direction];
                                       l=precomputedValues_nb[getNeighborhood4Fast(location_studied)+((isBlackTurn)<<8)];
                                       if ((l&7)==1) {
						//if (!debug) printf("The new one is not in the old one 3!\n");
                                               return location;
                                       }
                               }
                       }
                       break;


               default:
                       ;
               }

/*	if (debug) {
	if (location==vitalPoint) printf("!!!!!!!!!!!!!!!\n");
	printf("%i\n",state[location] );
	for (int i=0;i<4;i++){
		location_studied=location+FBS_handler::directions[i];
		printf("%i\n",state[location_studied] );
		for (int j=0;j<4;j++){
			location_vital=location_studied+FBS_handler::directions[j];
			printf("%i",state[location_vital] );
		}
		printf("\n");
	}

	l=precomputedValues_nb[neighborhood[location_studied]>>8+((isBlackTurn)<<8)];
	printf("To check these values %i %i\n", (l)&7,(l>>3)&3);
	}*/
       return PASSMOVE;
  
}
  
  
  
int MoGo::FastBoard::nb_neighboard(Location location, Location from, int nb_actuel) {

	if ( nb_actuel > 3)
		return -1;  

	int res = nb_actuel;
	Location location_studied = PASSMOVE;

	for (int i=0;i<4;i++) {
		location_studied = location+FBS_handler::directions[i];
		if ( (location_studied != from) && ( (state[location_studied] == isBlackTurn) || (state[location_studied] == FB_EMPTY))) {
			res = nb_neighboard(location_studied, location, res+1);
			if (res == -1)
				return -1;
		}

	}
	return res;	

}

Location MoGo::FastBoard::center3(Location location) {
	

	int nb_free = 0;
	Location location_studied;
	Location res = PASSMOVE;
	for (int i=0;i<4;i++) {
		location_studied = location+FBS_handler::directions[i];
		if ( (state[location_studied] == isBlackTurn) || state[location_studied] == FB_EMPTY ) {
			nb_free++;
			res = location_studied;
		}
	}

	if (nb_free == 1)
		return res;
	else if (nb_free == 2)
		return location;
	else {
		assert(0);
		return PASSMOVE;
	}
}

Location MoGo::FastBoard::center4(Location location) {

	int nb_free = 0;
	Location location_studied;
	Location first_center = PASSMOVE;
	Location first_side = PASSMOVE;
	Location second_center = PASSMOVE;
	
	for (int i=0;i<4;i++) {
		location_studied = location+FBS_handler::directions[i];
		if ( (state[location_studied] == isBlackTurn) || state[location_studied] == FB_EMPTY ) {
			nb_free++;
			first_side = location_studied;
		}
	}

	if (nb_free == 1) {
		first_center = first_side;
		first_side = location;
		nb_free = 0;
		for (int i=0;i<4;i++) {
			location_studied = first_center+FBS_handler::directions[i];
			if ( ((state[location_studied] == isBlackTurn) || state[location_studied] == FB_EMPTY) && (location_studied != first_side) ) {
				nb_free++;
				second_center = location_studied;
			}
		}
		if ( (nb_free == 2) && (state[first_center] == FB_EMPTY) ) {
			return first_center;
		}

	} else if (nb_free == 2) {
		first_center = location;

		Location location_studied_2 = PASSMOVE;

		for (int i=0;i<4;i++) {
			location_studied = first_center+FBS_handler::directions[i];
			if ( (state[location_studied] == isBlackTurn) || state[location_studied] == FB_EMPTY ) {
				nb_free =0;
				for (int j=0;j<4;j++) {
					location_studied_2 = location_studied+FBS_handler::directions[j];
					if ( (state[location_studied_2] == isBlackTurn) || state[location_studied_2] == FB_EMPTY ) {
						nb_free++;
					}
				}
				if (nb_free == 2) {
					second_center = location_studied;
					break;
				}
			}
		}
		
	} else if (nb_free == 3) {
		if (state[location] == FB_EMPTY)
			return location;
		
	}

	

	if ( (state[first_center] == isBlackTurn) && (state[second_center] == FB_EMPTY))
		return second_center;

	if ( (state[first_center] == FB_EMPTY) && (state[second_center] == isBlackTurn))
		return first_center;

	

	return PASSMOVE;
}


Location MoGo::FastBoard::saveStringOnAtari( int numThread ) const {/**assert(0);**/
  if ( lastPlayedMove == PASSMOVE) {/**assert(0);**/
    return PASSMOVE;
  }
  Location location = PASSMOVE;
  FastSmallVector<Location> &underAtariStringsFirstPosition=__underAtariStringsFirstPosition[numThread*COEF_CACHE_MISS_FAST_BOARD];
  //   if (ko==PASSMOVE) {/**assert(0);**/
  underAtariStringsFirstPosition.clear();
  if (isLastMoveAtari(underAtariStringsFirstPosition))
    if (saveLastMoveAtari(underAtariStringsFirstPosition, location, numThread)) {/**assert(0);**/
      return location;
    }
  return PASSMOVE;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::FastBoard::isLastMoveAtari( FastSmallVector< Location > & underAtariStringsFirstPosition ) const {
  // if (lastMoveWasAtari==0) return false;

  //   assert(underAtariStringsFirstPosition.size() == 0);
  if (lastPlayedMove == PASSMOVE) return false;

  /*      int row=GoTools::min(FBS_handler::row(lastPlayedMove), FBS_handler::board_size-1-FBS_handler::row(lastPlayedMove));
        int col=GoTools::min(FBS_handler::col(lastPlayedMove), FBS_handler::board_size-1-FBS_handler::col(lastPlayedMove));
  if (row<=5 && col<=5 && chainTrueLib[chainId[lastPlayedMove]]==1) return false; 
  */

  for (int i=0;i<4;i++) {
    int v=lastPlayedMove+FBS_handler::directions[i];
    if (int(state[v])==isBlackTurn && liberty(v)==1/*chain_lib_cnt[chainId[v]]==1*/) { // approximation here (pseudo liberties)
      underAtariStringsFirstPosition.push_back(v);
    }
  }
  return (underAtariStringsFirstPosition.size()>0);
}


void MoGo::FastBoard::toGoban( Goban & goban) const {/**assert(0);**/
  goban.clearGoban();
  for (int i=0;i<(int)goban.gobanState.size();i++) {/**assert(0);**/
    if (state[i]==FB_WHITE) {/**assert(0);**/
      if (goban.isBlacksTurn())
        goban.playMoveOnGoban(PASSMOVE);
      goban.playMoveOnGoban(i);
    } else if (state[i]==FB_BLACK) {/**assert(0);**/
      if (!goban.isBlacksTurn())
        goban.playMoveOnGoban(PASSMOVE);
      goban.playMoveOnGoban(i);
    }
  }
  if (goban.isBlacksTurn()^isBlackTurn)
    goban.playMoveOnGoban(PASSMOVE);
  goban.lastPlayedMove=lastPlayedMove;
  if (ko>0)
    goban.ko=ko;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
// Genere un coup de la partie monte-carlo
int MoGo::FastBoard::playOne3( int choiceB, int choiceW, int numThread ) {
Location l;

int thresholdSelfAtari;
if (isBlackTurn)
	thresholdSelfAtari=choiceB;
else
	thresholdSelfAtari=choiceW;


 if ( lastPlayedMove != PASSMOVE) {
   
   // protege les groupes en atari et nakade
#ifdef BIZARMC
if (numThread!=8)
//if ((numThread%4)!=0)
{
#endif
  l=coreOfRandomMode44(thresholdSelfAtari, numThread);
  if (l>=0) {
#ifdef DEBUG_ALL_FastBoard
    GoTools::print("l=%i on:\n", l);
    printPosition(l);printPosition(lastPlayedMove);
    printf ("\n%s", to_string().data ());
    //     getchar(); //ADDED
#endif
    return l;
  }
#ifdef BIZARMC
}
#endif
  

#ifdef GOLDENEYE_MC
if (semeai)
{
	if (isBlackTurn)
	{
		//fprintf(stderr,"beuh %d\n",semeaiB.size());fflush(stderr);
		for (int k=0;k<semeaiB.size();k++)
		{	
			int l=semeaiB[k];	
			int val=playNoPass (l);
			if (val < play_ss_suicide) 
			{semeaiB[k]=semeaiB[semeaiB.size()-1];semeaiB.resize(semeaiB.size()-1);return l;}
		}
		semeai=false;
	}
	else
	{
		for (int k=0;k<semeaiW.size();k++)
		{	
			int l=semeaiW[k];	
			int val=playNoPass (l);
			if (val < play_ss_suicide) 
			{semeaiW[k]=semeaiW[semeaiW.size()-1];semeaiW.resize(semeaiW.size()-1);return l;}
		}
		semeai=false;

	}
}
#endif

 l=coreOfRandomMode45();
  if (l>=0) {
#ifdef DEBUG_ALL_FastBoard
    GoTools::print("l=%i on:\n", l);
    printPosition(l);printPosition(lastPlayedMove);
    printf ("\n%s", to_string().data ());
    //     getchar(); //ADDED
#endif
    return l;
  }
  


  // choisit un coup parmi les paternes
  l=playOneInterestingMove(numThread);
  if (l>0) {
    playNoPassFaster(l);
    return l;
  }

  //l = playCapture();  
  //if(l!=PASSMOVE) return l;


}
  // choisit un coup au hasard parmi les coups corrects
  l=playOneAvoidBigSelfAtari (thresholdSelfAtari, numThread);
#ifdef DEBUG_ALL_FastBoard
  printPosition(l);
  printf ("\n%s", to_string().data ());
  //   getchar(); //ADDED
#endif
  return l;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
int MoGo::FastBoard::playOneInterestingMove(int numThread) {
  FastSmallVector<Location> &interestingMoves=__interestingMoves[numThread*COEF_CACHE_MISS_FAST_BOARD];

  interestingMoves.clear();
  assert(lastPlayedMove!=0);

  for (int i=0;i<8;i++) {
    int v=lastPlayedMove+FBS_handler::directions[i];
    if (state[v]==FB_EMPTY) {
      if (isInterestingByPattern(v))
        interestingMoves.push_back(v);
    }
  }

  

  while (interestingMoves.size() > 0) {
    int index=0;
    if (interestingMoves.size() > 1)
      index=MoGo::GoTools::mogoRand2(interestingMoves.size());

    if ((interestingMoves[index]!=(int)ko) /*&& (!isEye (interestingMoves[index]))*/
        && (!isSelfSuicideMove(interestingMoves[index]))
        && (!isSelfAtari(interestingMoves[index], numThread))
        && (!isLocallyWeakerMoveForSimulation(interestingMoves[index]))
       ) {
      return interestingMoves[index];
    } else {
      interestingMoves[index]=interestingMoves[interestingMoves.size()-1];
      interestingMoves.pop_back();
    }
  }
  return -1;
}


 // TEMPORARY COMMENT: THIS FUNCTION IS USED
int MoGo::FastBoard::patternValue( Location move ) const {
  return (getNeighborhood8Fast(move)<<1)+isBlackTurn;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
int MoGo::FastBoard::isInterestingByPattern( Location move ) const {
  int t=simplePatternDatabase[patternValue(move)];

  if (t<0) return 0;
  /*if (t>=0)*/ {
    if (t&1) {
      return 1;
    }
    if (t>=2)
      if (isAtari(move)) {
        return 1;
      }
  }
  return 0;
}

 // TEMPORARY COMMENT: THIS FUNCTION IS USED
int MoGo::FastBoard::interestingPatternValue( Location move ) const {
  if (state[move]!=FB_EMPTY || move==ko || isSelfSuicideMove(move)) return -1;
  return simplePatternDatabase[patternValue(move)];
}


// TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::FastBoard::isAtari( Location move ) const {
  //   int currentPlayer=player::other(last_player);
 
  int opponent=!isBlackTurn;
  for (int i=0;i<4;i++) {
    int v=move+FBS_handler::directions[i];
    if (int(state[v])==opponent && (liberty(v)==2) /*&& chainLength[chainId [v]]>1*/ /*chain_lib_cnt[chainId[v]]==2*/) {
      return true;
    }
  }
  return false;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::FastBoard::precomputeSimplePatternDatabase( ) {
  for (int i=0;i<65536*2;i++) simplePatternDatabase[i]=-1;
  for (int i=0;i<256;i++) {
    int nbFreeLocations=getFreeNeighborhoodSlow(i);
    int isEye4White=isOpponentEye4Slow(i,0);
    int isEye4Black=isOpponentEye4Slow(i,1);
    int firstDirection=firstFreeDirectionSlow(i);
    int neighborColor=sameColor(i);

    precomputedValues4[i+(0<<8)]=nbFreeLocations+(isEye4White<<3)+(firstDirection<<4)/*+(isOnSide<<7)*/;
    precomputedValues4[i+(1<<8)]=nbFreeLocations+(isEye4Black<<3);
    
    
    //precomputedValues_nb used in nb_neighborhood function
    precomputedValues_nb[i]=getNeighborWhite(i)+(neighborColor<<3)+(nbFreeLocations<<11);
    precomputedValues_nb[i+(1<<8)]=getNeighborBlack(i)+(neighborColor<<3)+(nbFreeLocations<<11);
    int shift1=0,shift2=0;
    for (int j=0;j<4;j++){
      if ((valueNeighborhood(i, j)==FB_EMPTY)
	  ||(valueNeighborhood(i,j)==FB_WHITE)) {
	precomputedValues_nb[i]+=(j<<(5+shift1));
	shift1+=2;
      }
      if ((valueNeighborhood(i, j)==FB_EMPTY)||(valueNeighborhood(i,j)==FB_BLACK)) {
	precomputedValues_nb[i+(1<<8)]+=(j<<(5+shift2));
	shift2+=2;
      }
    }
    
    
    /*
    //precomputedValues_nb used in nb_neighborhood function
    precomputedValues_nb[i]=nbFreeLocations+(neighborColor<<3)+(firstDirection<<5);
    if (nbFreeLocations==2){
    for (int j=0;j<4;j++)
    if ((valueNeighborhood(i, j)==FB_EMPTY)&&(j!=firstDirection)) precomputedValues_nb[i]+=(j<<7);
   }
    */
    //     precomputedValues4[i]=-1;
  }
  
  int size=FBS_handler::board_size; if (GobanLocation::height>0) size=GobanLocation::height;
  FBS_handler::changeSize(size);
  if (initiateByFile) {
    InitiateFastBoard::initiatePrecomputedValues(precomputedValues, precomputedValues4, simplePatternDatabase);
    return ;
  }
  
  
  
  for (int i=0;i<int(20000*9*9/FBS_handler::board_size/FBS_handler::board_size);i++) {/**assert(0);**/
    //     GoTools::print("i=%i\n", i);
    Goban goban(FBS_handler::board_size,FBS_handler::board_size);goban.setCFG(false);
    FastBoard board;
    int pass=0;
    int counter = 0;
    Location location;
    do {/**assert(0);**/
      counter++;
      location=goban.randomOutOneMove(0);
      if (location==PASSMOVE) pass++;
      else {/**assert(0);**/
        //         GoTools::print("location %i\n", location);
        int patternResult=goban.patternValueMatchedBy2015441(location);
        int numPattern=board.patternValue(location);
	
        //         int numPattern4=board.getNeighborhood4(location)+(board.isBlackTurn<<8);
        int nbFreeLocations=getFreeNeighborhoodSlow(board.neighborhood[location]&0xFF);
        int isEye4=board.isOpponentEye4Slow(location);
        assert(board.getNeighborhood8(location)==board.neighborhood[location]);
        assert(nbFreeLocations==getFreeNeighborhood(board.getNeighborhood4(location)));
        assert(isEye4==isOpponentEye4(board.getNeighborhood4(location), board.isBlackTurn));
        assert(numPattern>=0);
	
        //                  goban.textModeShowGoban();
        //                   goban.textModeShowPosition(location);
        //                  board.print();
        //         getchar();

        assert(board.isBlackTurn==goban.isBlacksTurn());
        if (simplePatternDatabase[numPattern]==-1) {/**assert(0);**/
          simplePatternDatabase[numPattern]=patternResult;
          goban.isInterestingMoveBy2015441(location, false);
        }
        int res=board.playNoPass(location);
        if (res!=play_ok) {/**assert(0);**/
          goban.textModeShowGoban();
          goban.textModeShowPosition(location);
          board.print();
          GoTools::print("res = %i\n", res);
          assert(0);
        }
        pass=0;
      }
      goban.playMoveOnGoban(location);
      board.isBlackTurn=goban.isBlacksTurn();
      if (counter>=int(FBS_handler::board_size*FBS_handler::board_size*2)) {/**assert(0);**/
        //         GoTools::print("infinit loop!\n");
        break;
      }

    } while(pass<2);
    if (pass==2 && int(goban.score())!=board.score()) {/**assert(0);**/
      goban.textModeShowGoban();
      board.print();
      GoTools::print("goban.score() %i, board.score() %i\n", int(goban.score()), board.score());
      //       getchar();
      assert(0);
    }
  }


  FILE *f=fopen("precomputedValuesTmp","w");
  assert(f);
  for (int i=0;i<65536*2;i++) {/**assert(0);**/
    int value=simplePatternDatabase[i]; int size=0; int start=i;
    for (int j=i;j<65536*2;j++, i++, size++) if (simplePatternDatabase[j]!=value) break;
    fprintf(f, "for (int i=%i;i<%i;i++) simplePatternDatabase[i]=%i;\n", start, start+size, value); i--;
  }
  for (int i=0;i<65536*2;i++) {/**assert(0);**/
    int value=precomputedValues[i]; int size=0; int start=i;
    for (int j=i;j<65536*2;j++, i++, size++) if (precomputedValues[j]!=value) break;
    fprintf(f, "for (int i=%i;i<%i;i++) precomputedValues[i]=%i;\n", start, start+size, value); i--;
  }
  for (int i=0;i<256*2;i++) {/**assert(0);**/
    int value=precomputedValues4[i]; int size=0; int start=i;
    for (int j=i;j<256*2;j++, i++, size++) if (precomputedValues4[j]!=value) break;
    fprintf(f, "for (int i=%i;i<%i;i++) precomputedValues4[i]=%i;\n", start, start+size, value); i--;
  }
  fclose(f);
  //   int FastBoard::precomputedValues[65536*2];
  //   int FastBoard::precomputedValues4[256*2];
  //   int FastBoard::simplePatternDatabase[65536*2];



}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::FastBoard::precomputeVitalPointPatternDatabase( ) {
  for (int i=0;i<65536*2;i++) vitalPointPatternDatabase[i]=1;

  // Mettre �1 les pattern qui peuvent conduire �des VP
  //On parcourt tous les cas de 9 pierres.
  //Quand ca match les criteres on met �1.
  
   

}





// TEMPORARY COMMENT: THIS FUNCTION IS USED
/*all_inline*/ int MoGo::FastBoard::playOneAvoidBigSelfAtari( int thresholdSelfAtari, int numThread ) {
  int selfAtari;
  if (thresholdSelfAtari>=100) {/**assert(0);**/
    Location v;
    // find random place in vector of empty vertices
    int start = GoTools::mogoRand2(freeLocationsIndex);

    // search for a move in start ... freeLocations_cnt-1 interval
    for (int ev_i = start; ev_i != freeLocationsIndex; ev_i++) {/**assert(0);**/
      v = freeLocations [ev_i];

      if ( (v!=ko) && /*acceptThisMove(v) && */simplePatternDatabase[patternValue(v)]>=0 /*&& !isEye (v)*//* && isSelfAtariSmallerThan(v, thresholdSelfAtari, numThread)*//*isSelfAtari(v)<thresholdSelfAtari*/) {/**assert(0);**/
        int val=playNoPass (v); //GoTools::print("val %i\n", val);
        if (val < play_ss_suicide) return v;
      }
    }

    // search for a move in 0 ... start interval
    for (int ev_i = 0; ev_i != start; ev_i++) {/**assert(0);**/
      v = freeLocations [ev_i];
      if ( (v!=ko) && /*acceptThisMove(v) && */simplePatternDatabase[patternValue(v)] >=0/*&& !isEye (v)*/ /*&& isSelfAtariSmallerThan(v, thresholdSelfAtari, numThread)*//*isSelfAtari(v)<thresholdSelfAtari*/) {/**assert(0);**/
        int val=playNoPass (v); //GoTools::print("val %i\n", val);
        if (val < play_ss_suicide) return v;
      }
    }
    playPass();
    return PASSMOVE;
  }

  if (finishBoard) {/**assert(0);**/
    Location v;
    // find random place in vector of empty vertices
    int start = GoTools::mogoRand2(freeLocationsIndex);
    bool refusedBySelfAtari=false;
    // search for a move in start ... freeLocations_cnt-1 interval
    for (int ev_i = start; ev_i != freeLocationsIndex; ev_i++) {/**assert(0);**/
      v = freeLocations [ev_i];

      if ( (v!=ko) && acceptThisMove(v) && simplePatternDatabase[patternValue(v)]>=0) {/**assert(0);**/
	if ((selfAtari=isSelfAtariSmallerThanFunction2(v, thresholdSelfAtari, numThread))==0) {/**assert(0);**/
          refusedBySelfAtari=true; continue;
	  
        }
	if (selfAtari==-1 || double(GoTools::mogoRand2(100000))/100000.>approachProbability){
          int val=playNoPass (v); //GoTools::print("val %i\n", val);
          if (val < play_ss_suicide) return v;
	}
	else{
          int v2=returnOtherLibertyWhenPlayingSelfAtari(v);
	  int val=playNoPass (v2); //GoTools::print("val %i\n", val);
          if (val < play_ss_suicide) return v2;
          else{
            val=playNoPass (v); //GoTools::print("val %i\n", val);
            if (val < play_ss_suicide) return v;
	  }
	}
      }
    }
    
    // search for a move in 0 ... start interval
    for (int ev_i = 0; ev_i != start; ev_i++) {/**assert(0);**/
      v = freeLocations [ev_i];
      if ( (v!=ko) && acceptThisMove(v) && simplePatternDatabase[patternValue(v)] >=0) {/**assert(0);**/
	if ((selfAtari=isSelfAtariSmallerThanFunction2(v, thresholdSelfAtari, numThread))==0) {/**assert(0);**/
          refusedBySelfAtari=true; continue;
        }
	if (selfAtari==-1 || double(GoTools::mogoRand2(100000))/100000.>approachProbability){
          int val=playNoPass (v); //GoTools::print("val %i\n", val);
          if (val < play_ss_suicide) return v;
	}
	else{
	  int v2=returnOtherLibertyWhenPlayingSelfAtari(v);
	  int val=playNoPass (v2); //GoTools::print("val %i\n", val);
          if (val < play_ss_suicide) return v2;
          else{
            val=playNoPass (v); //GoTools::print("val %i\n", val);
            if (val < play_ss_suicide) return v;
            }
	}
      }
    }

    if (refusedBySelfAtari)
      for (int ev_i = 0; ev_i != freeLocationsIndex; ev_i++) {/**assert(0);**/
        v = freeLocations [ev_i];
        if ( (v!=ko) && simplePatternDatabase[patternValue(v)]>=0) {/**assert(0);**/
          int val=playNoPass (v); //GoTools::print("val %i\n", val);
          if (val < play_ss_suicide) return v;
        }
      }
  } else {
    Location v;
    // find random place in vector of empty vertices
    int start = GoTools::mogoRand2(freeLocationsIndex);

    // search for a move in start ... freeLocations_cnt-1 interval
    for (int ev_i = start; ev_i != freeLocationsIndex; ev_i++) {
      v = freeLocations [ev_i];
      
      if ( (v!=ko) && acceptThisMove(v) && simplePatternDatabase[patternValue(v)]>=0 /*&& !isEye (v)*/  && (selfAtari=isSelfAtariSmallerThanFunction2(v, thresholdSelfAtari, numThread))!=0  /*isSelfAtari(v)<thresholdSelfAtari*/) {
	
	if (selfAtari==-1 || double(GoTools::mogoRand2(100000))/100000.>approachProbability){
          int val=playNoPass (v); //GoTools::print("val %i\n", val);
          if (val < play_ss_suicide) return v;
	}
	else{
	  int v2=returnOtherLibertyWhenPlayingSelfAtari(v);
	  int val=playNoPass (v2); //GoTools::print("val %i\n", val);
          if (val < play_ss_suicide) return v2;
          else{
            val=playNoPass (v); //GoTools::print("val %i\n", val);
            if (val < play_ss_suicide) return v;
	  }
	}
      }
    }

    // search for a move in 0 ... start interval
    for (int ev_i = 0; ev_i != start; ev_i++) {
      v = freeLocations [ev_i];
      if ( (v!=ko) &&  acceptThisMove(v)  && simplePatternDatabase[patternValue(v)]>=0 /*&& !isEye (v)*/  && (selfAtari=isSelfAtariSmallerThanFunction2(v, thresholdSelfAtari, numThread))!=0  /*isSelfAtari(v)<thresholdSelfAtari*/) {
	

	if (selfAtari==-1 || double(GoTools::mogoRand2(100000))/100000.>approachProbability){
          int val=playNoPass (v); //GoTools::print("val %i\n", val);
          if (val < play_ss_suicide) return v;
	}
	else{
	  int v2=returnOtherLibertyWhenPlayingSelfAtari(v);
	  int val=playNoPass (v2); //GoTools::print("val %i\n", val);
          if (val < play_ss_suicide) return v2;
          else{
            val=playNoPass (v); //GoTools::print("val %i\n", val);
            if (val < play_ss_suicide) return v;
	  }
	}
      }
    }
    
  }
  
  playPass();
  return PASSMOVE;
}
















// TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::FastBoard::areTwoStringsSharingTheSameTwoLiberties(int pos1, int pos2) const {
  int strNb1=chainId[pos1], strNb2=chainId[pos2];
  assert(liberty(pos1) == 2);
  assert(liberty(pos2) == 2);
  if (chainLength[strNb1] > chainLength[strNb2] ) {
    swapFB(strNb1, strNb2); swapFB(pos1, pos2);
  }
  int liberty = 0;
  Location testedLiberty = PASSMOVE;

  Location current = pos1;
  do {
    for (int j=0;j<4 && liberty<2;j++) {
      int v=current+FBS_handler::directions[j];

      if (
        state[v] == FB_EMPTY &&
        testedLiberty != int(v)) {
        liberty ++;
        testedLiberty = v;
        if (!isOneLibertyOfChain(testedLiberty, strNb2)) return false;
      }

    }
    current= chainNext[current];
  } while (int(current) != pos1);


  assert(liberty == 2);
  return true;
}



// TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::FastBoard::playOnGoban(Location location) {
  if (location ==PASSMOVE) {
    playPass();
    return true;
  } else {
    return playNoPass(location)< play_ss_suicide;
  }
}


// TEMPORARY COMMENT: THIS FUNCTION IS USED
// static int c=0;
void MoGo::FastBoard::initiateOwnBoardForThread() {
  int numThread=0;
#ifdef PARALLELIZED
  numThread=ThreadsManagement::getNumThread();
#endif
  if (allBoards[numThread]==0) allBoards[numThread]=new FastBoard();
  //   c++;
  *allBoards[numThread]=*referenceBoard;
}
void MoGo::FastBoard::initiateOwnBoardForThread( const Vector< Location > & moves ) {/**assert(0);**/
  int numThread=0;
#ifdef PARALLELIZED
  numThread=ThreadsManagement::getNumThread();
#endif
  if (allBoards[numThread]==0) allBoards[numThread]=new FastBoard();

  *allBoards[numThread]=*referenceBoard;

  for (int i=1;i<(int)moves.size();i++)
    allBoards[numThread]->playOnGoban(moves[i]);
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::FastBoard::playOwnBoardForThread( Location location ) {
  int numThread=0;
#ifdef PARALLELIZED
  numThread=ThreadsManagement::getNumThread();
#endif
  assert(allBoards[numThread]);



  return allBoards[numThread]->playOnGoban(location);
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::FastBoard::initiateReferenceBoard( const Goban & goban ) {
  if (referenceBoard==0) referenceBoard=new FastBoard();
  fromGoban(goban, *(referenceBoard));
  //   referenceBoard->clearCaptures();
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::FastBoard::initiateOwnBoardForThread( const Goban & goban ) {
  int numThread=0;
#ifdef PARALLELIZED
  numThread=ThreadsManagement::getNumThread();
#endif
  if (allBoards[numThread]==0) allBoards[numThread]=new FastBoard();
  fromGoban(goban, *(allBoards[numThread]));
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
int MoGo::FastBoard::oneRollOutThread( int num, int choiceB, int choiceW, Vector< int > & allAtFirstData, FastSmallVector< int > & usedMovesAllAtFirst, double komi ) {
  int numThread=0;
#ifdef PARALLELIZED
  numThread=ThreadsManagement::getNumThread();
#endif

  return allBoards[numThread]->oneRollOut(num, choiceB, choiceW, allAtFirstData, usedMovesAllAtFirst, komi, numThread);
}



// TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::FastBoard::isLocallyWeakerMoveForSimulation( Location location ) const {
  int opponent=!isBlackTurn;
  int minOpponentLiberties=1000;
  int friendLiberties=getFreeNeighborhood(neighborhood[location]&0xFF);
  //   int nbFriendStrings=0;
  for (int j=0;j<4;j++) {
    Location tmpLocation=location+FBS_handler::directions[j];
    if ( state[tmpLocation] == opponent ) {
      int lib=chainTrueLib[chainId[tmpLocation]];
      if (lib<minOpponentLiberties) minOpponentLiberties=lib;
    } else if (state[tmpLocation] == isBlackTurn) {
      friendLiberties+=chainTrueLib[chainId[tmpLocation]]-1;
      //       nbFriendStrings++;
    } /*else if (state[tmpLocation] == FB_EMPTY)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      friendLiberties++;*/
  }
  // if (friendLiberties>3) {/**assert(0);**/
  //   return (friendLiberties>minOpponentLiberties);
  // }

  return (friendLiberties<minOpponentLiberties);
}













// TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::FastBoard::acceptThisMove( Location location ) const {
  if (playNearMode==1) {/**assert(0);**/
    for (int j=0;j<4;j++) {/**assert(0);**/
      int v=location+FBS_handler::directions[j];
      if (FB_isPlayer(state[v]))
        return true;
    }
    return false;
  } else if (playNearMode==2) {/**assert(0);**/
    int nbMoves=stonesPerPlayer[FB_BLACK] +  stonesPerPlayer[FB_WHITE];
    if (nbMoves<startPlayNearMode) return true;
    else if (nbMoves>stopPlayNearMode) return true;
    for (int j=0;j<4;j++) {/**assert(0);**/
      int v=location+FBS_handler::directions[j];
      if (FB_isPlayer(state[v]))
        return true;
    }
    return false;

  } else return true;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::FastBoard::initAll( ) {
  FBS_handler::initAll();
}

void MoGo::FastBoard::print( ) const {/**assert(0);**/
  Goban goban(FBS_handler::board_size,FBS_handler::board_size);
  toGoban(goban);
  goban.textModeShowGoban();

  for (int i=0;i<FBS_handler::board_size;i++) {/**assert(0);**/
    for (int j=0;j<FBS_handler::board_size;j++) {/**assert(0);**/
      GoTools::print("%i ", chainTrueLib[chainId[(i+1)*(FBS_handler::board_size+2)+j+1]]);
    }
    GoTools::print("\n");
  }
  GoTools::print("\n");
  for (int i=0;i<FBS_handler::board_size;i++) {/**assert(0);**/
    for (int j=0;j<FBS_handler::board_size;j++) {/**assert(0);**/
      GoTools::print("%i ", state[(i+1)*(FBS_handler::board_size+2)+j+1]);
    }
    GoTools::print("\n");
  }
  verifyNeighborhood();
}

void MoGo::FastBoard::printLocation( Location location ) const {/**assert(0);**/
  Goban goban(FBS_handler::board_size,FBS_handler::board_size);
  goban.textModeShowPosition(location);
}

void MoGo::FastBoard::verifyNeighborhood( ) const {/**assert(0);**/
  for (int i=0;i<FBS_handler::board_big_area;i++) {/**assert(0);**/
    if (FBS_handler::isOnBoard(i)/*state[i]==FB_EMPTY*/) {/**assert(0);**/
      int v=neighborhood[i];
      if(v!=getNeighborhood8(i)) {/**assert(0);**/
        printLocation(i);
        GoTools::print("v %i, neighborhood %i\n", v, getNeighborhood8(i));
        assert(0);
      }
    }
  }
}








// TEMPORARY COMMENT: THIS FUNCTION IS USED
int MoGo::FastBoard::isSelfAtariSmallerThan( const Location move, int thresholdSelfAtari, int numThread ) const {
  //   if (thresholdSelfAtari>=100) return true;
  return isSelfAtari(move, numThread)<thresholdSelfAtari;

}


//Ajout Chris
// TEMPORARY COMMENT: THIS FUNCTION IS USED
int MoGo::FastBoard::isSelfAtariSmallerThanFunction2( const Location move, int thresholdSelfAtari, int numThread ) const {
  int selfAtari=isSelfAtari(move, numThread);
  if (selfAtari==0 && thresholdSelfAtari>0)
    return -1;
  else
  {
    if (selfAtari<thresholdSelfAtari)
      return 1;
    else
      return 0;
  }
}

// Ajout Chris
// TEMPORARY COMMENT: THIS FUNCTION IS USED
Location MoGo::FastBoard::returnOtherLibertyWhenPlayingSelfAtari(Location location){
  bool isAlone=true;
  Location otherLiberty=location;
  Location locationStudied;
  for (int i=0;i<4;i++)
  {
    locationStudied=location+FBS_handler::directions[i];
    if (state[locationStudied]==isBlackTurn)
      isAlone=false;
    else if (state[locationStudied]==FB_EMPTY)
      otherLiberty=locationStudied;
  }
  if (isAlone) // La pierre est toute seule en auto atari
  {
    /*if (otherLiberty != location)
      cerr<<"Approche"<<endl;*/
    //return otherLiberty;
    return location; // Apparamment ça marche mieux si on enleve cette option. Pour la réactiver, il faut retourner otherLiberty à la place de location
  }
  else // Auto atari d'un groupe entier
  {
    //cerr<<"nouveau test"<<endl;
    otherLiberty=location;
    for (int i=0;i<4;i++)
    {
      locationStudied=location+FBS_handler::directions[i];
      if (state[locationStudied]==isBlackTurn && liberty(locationStudied)==2)
      {
	otherLiberty=getOtherLiberty(location, locationStudied, 0);
	if (otherLiberty!=location)
	{
	  //cerr<<"cool, ça marche :D"<<endl;
	  return otherLiberty;
	}
	//else
	  //cerr<<"celui la n'est pas pris :)"<<endl;
      }
      //else if(state[locationStudied]==isBlackTurn)
	//cerr<<"un cote n'a pas de libertes"<<endl;
    }
    return otherLiberty;
  }
}

// Ajout Chris
// TEMPORARY COMMENT: THIS FUNCTION IS USED
Location MoGo::FastBoard::getOtherLiberty(Location firstLiberty, Location from, int nb_actuel) {
	if ( nb_actuel > 3)
		return firstLiberty;
	Location secondLiberty=firstLiberty;
	Location location_studied;
	for (int i=0;i<4;i++) {
		location_studied = from+FBS_handler::directions[i];
		if ( (state[location_studied]==FB_EMPTY) && (location_studied != firstLiberty)) {
			return location_studied;
		}
		else if (state[location_studied]==isBlackTurn) {
			getOtherLiberty(firstLiberty, location_studied, nb_actuel+1);
			if (secondLiberty != firstLiberty)
				return secondLiberty;
		}
	}
	return secondLiberty;	
}





// TEMPORARY COMMENT: THIS FUNCTION IS USED
FastBoard * MoGo::FastBoard::getBoardForThread() {
  int numThread=0;
#ifdef PARALLELIZED
  numThread=ThreadsManagement::getNumThread();
#endif

  return allBoards[numThread];
}




// TO DELETE: WHEN THE OPTION SCORE_MODE WILL BECOME OBSOLETE
int MoGo::FastBoard::owner( Location location ) const {/**assert(0);**/
  if (state[location]==FB_BLACK) return 1;
  else if (state[location]==FB_WHITE) return -1;
  else if (isOpponentEye4(getNeighborhood4Fast(location), false))
    return 1;
  else if (isOpponentEye4(getNeighborhood4Fast(location), true))
    return -1;
  else
    return 0;
}


bool MoGo::FastBoard::isCaptureMove(Location location) const {/**assert(0);**/
  if (state[location]!=FB_EMPTY)
    return false;
  if (isBlackTurn) {/**assert(0);**/
    for (int i=0;i<4;i++) {/**assert(0);**/
      Location tmpLocation=location+FBS_handler::directions[i];
      if (state[tmpLocation]==FB_WHITE && chainTrueLib[chainId[tmpLocation]]==1)
        return true;
    }
    return false;

  } else {/**assert(0);**/
    for (int i=0;i<4;i++) {/**assert(0);**/
      Location tmpLocation=location+FBS_handler::directions[i];
      if (state[tmpLocation]==FB_BLACK && chainTrueLib[chainId[tmpLocation]]==1)
        return true;
    }
    return false;

  }
}





// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::FastBoard::initStaticVariables() {
  __adjacentFriendTwoLibertiesStrings.resize(NUM_THREADS_MAX*COEF_CACHE_MISS_FAST_BOARD);
  __interestingMoves.resize(NUM_THREADS_MAX*COEF_CACHE_MISS_FAST_BOARD);
  __underAtariStringsFirstPosition.resize(NUM_THREADS_MAX*COEF_CACHE_MISS_FAST_BOARD);
}








bool MoGo::FastBoard::isMakeKo( const Location location ) const {/**assert(0);**/
  int size=0;
  int opponent=!isBlackTurn;
  for (int i=0;i<4;i++) {/**assert(0);**/
    Location tmpLocation=location+FBS_handler::directions[i];
    if (state[tmpLocation] == opponent) {/**assert(0);**/
      if (chainTrueLib[chainId[tmpLocation]]== 1)
        size+=chainLength[chainId[tmpLocation]];
    } else if (state[tmpLocation] == isBlackTurn || state[tmpLocation] == FB_EMPTY)
      return false;
  }
  return size==1;
}


// TEMPORARY COMMENT: THIS FUNCTION IS USED
const GobanKey MoGo::FastBoard::getGobanKey( ) const {
  GobanKey keyArray;
  int heightOfGoban=FBS_handler::board_size;
  int widthOfGoban=FBS_handler::board_size;
  int size=heightOfGoban*widthOfGoban;
  int playerColor=2; if (isBlackTurn) playerColor=1;

  static int FBStatesToGobanStates[4]; static bool init=false;
  if (!init) {
    FBStatesToGobanStates[FB_EMPTY]=EMPTY;
    FBStatesToGobanStates[FB_EDGE]=OUTSIDE;
    FBStatesToGobanStates[FB_BLACK]=BLACK;
    FBStatesToGobanStates[FB_WHITE]=WHITE;
    init=true;
  }

  if (size%20<=1) keyArray.resize(size/20+1);  //3^20<2^32<3^21, 81 = 361 mod 20
  else keyArray.resize(size/20+2);
  //GoTools::print("size of gobankey : %d\n",keyArray.size());
  keyArray[0]=(unsigned int)heightOfGoban+(unsigned int)widthOfGoban*32+(unsigned int)ko*1024; //height/width(probably we don't have this two size, what is the interest to have a rectangle goban?)/ko
  keyArray[0]+=(unsigned int)(playerColor-1)*(1024*512);//2^19;//Color of Player
  int _state=FBStatesToGobanStates[state[FBS_handler::toLocation(0,0)]]; if (_state==OUTSIDE) _state=EMPTY;
  keyArray[0]+=_state*(1024*1024);//2^20; //trick here. For 9x9 and 19x19 goban, the situation of 20(no nore) positions could be saved in a 32bits type.

  //GoTools::print("%d\n",keyArray[0]);

  int c=-1,index=1;keyArray[1]=0;
  unsigned int three_exp=1;
  for (int i=1;i<widthOfGoban;i++) {
    c++;
    if (c==20) {/**assert(0);**/
      c=0;
      index++;
      keyArray[index]=0;
      three_exp=1;
    }
    _state=FBStatesToGobanStates[state[FBS_handler::toLocation(0,i)]]; if (_state==OUTSIDE) _state=EMPTY;
    keyArray[index]+=_state*three_exp;
    three_exp*=3;
  }
  // changement of c before keyArray update and changement of three_exp after, is because, ... hard to say but rather tricky.
  for (int i=1;i<heightOfGoban;i++)
    for (int j=0;j<widthOfGoban;j++) {
      c++;
      if (c==20) {
        c=0;
        index++;
        keyArray[index]=0;
        three_exp=1;
      }
      _state=FBStatesToGobanStates[state[FBS_handler::toLocation(i,j)]]; if (_state==OUTSIDE) _state=EMPTY;
      keyArray[index]+=_state*three_exp;
      three_exp*=3;
    }
  return keyArray;
}





void MoGo::FastBoard::addStringPositionsToFrontWithDistance( Location location, double refDistance, std::queue< Location > & fifo, double * distances ) const {/**assert(0);**/
  if (state[location]==FB_EMPTY) {/**assert(0);**/
    if (distances[location] < 0.) {/**assert(0);**/
      distances[location]=refDistance;
      fifo.push(location);
    }
  } else if (state[location]!=FB_EDGE) {/**assert(0);**/

    Location current = location;
    do {/**assert(0);**/
      if (distances[current] < 0.) {/**assert(0);**/
        distances[current]=refDistance;
        fifo.push(current);
      }
      current= chainNext[current];
    } while (int(current) != location);
  }
}







void MoGo::FastBoard::computeDistancesByGroup( Location startingPoint, double * distances ) const {/**assert(0);**/
  int size=(FBS_handler::board_big_area);
  for (int i=0;i<size;i++) distances[i]=-1.;

  assert(startingPoint>PASSMOVE);

  std::queue<Location> fifo;

  addStringPositionsToFrontWithDistance(startingPoint, 0., fifo, distances);

  while (!fifo.empty()) {/**assert(0);**/
    Location location=fifo.front(); fifo.pop();
    double distance=distances[location];
    for (int i=0;i<4;i++) {/**assert(0);**/
      Location tmpLocation=location+FBS_handler::directions[i];
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
}



int MoGo::FastBoard::getLibertiesOfString(Location location, list<Location>& liberties, 
					  int nb_liberties) const 
{
  int current = location;
  
  do 
    {
      for(int i = 0; i<4; i++)
	{
	  int neighbour = current+FBS_handler::directions[i];
	  if(not FBS_handler::isOnBoard(neighbour)) continue;
	  
	  if(state[neighbour]==FB_EMPTY)
	    if(not ListOperators::is_in(liberties,neighbour))
	      {
		liberties.push_back(neighbour);
		
		if(nb_liberties>0)
		  if((int)liberties.size()>=nb_liberties)
		    return nb_liberties;
	      }
	}
      
      current = chainNext[current];
    } 
  while (int(current) != location);
  
  return (int)liberties.size();
}

void MoGo::FastBoard::print_fastboard(Location last_move) const 
{
  Goban goban(getGobanKey());
  
  if(last_move>0)
    goban.textModeShowGoban(last_move);
  else goban.textModeShowGoban();
  
  std::cout<<std::endl;
}


/**
 * @brief know the number of stones around a location
 * @param location the considered intersection of the goban 
 * @param rayon the "middle size" of the selected window (width = 2*rayon+1)
 * @param color_stone the color of the stone
 * @param nb_locations the saveguard of the number locations which have visited.
                       (could be different of (2*rayon+1)*(2*rayon+1), 
		       for example, the case of the corner))
 * @return the number of counted stones
 */
int MoGo::FastBoard::getNumberStones(int location, int rayon, int color_stone, 
				     int* nb_locations) const
{
  int nb_stones = 0;
  int nb_loc = 0;
  
  int r_start;
  int c_start;
  int loc_start;
  
  int r_end;
  int c_end;
  int loc_end;
  
  int r_last;
  int last_location;
  
  
  int r_loc = FBS_handler::row(location);
  int c_loc = FBS_handler::col(location);
  
  r_start = r_loc-rayon;
  c_start = c_loc-rayon;
  if(r_start<0) r_start = 0;
  if(c_start<0) c_start = 0;
  
  r_end = r_start;
  c_end = c_loc+rayon;
  if(c_end>=FBS_handler::board_size) c_end = FBS_handler::board_size-1;
  
  loc_start = FBS_handler::toLocation(r_start,c_start);
  loc_end = FBS_handler::toLocation(r_end,c_end);
  
  r_last = r_loc+rayon;
  if(r_last>=FBS_handler::board_size) r_last = FBS_handler::board_size-1;
  last_location = FBS_handler::toLocation(r_last,c_end);
  
  while(loc_start<last_location)
    {
      for(int i = loc_start; i<=loc_end; i++)
	{
	  if(state[i]==color_stone)
	    nb_stones++;
	}
      
      loc_start+=FBS_handler::dNS;
      loc_end+=FBS_handler::dNS;
      
      nb_loc+=loc_end-loc_start+1;
    }
  
  if(nb_locations) (*nb_locations) = nb_loc;
  if(color_stone==FB_EMPTY) nb_stones = nb_loc-nb_stones;
  
  //cerr<<nb_stones<<" - "<<nb_loc<<endl;
  
  return nb_stones;
}

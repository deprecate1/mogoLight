//
// C++ Implementation: randomgoplayer_mg
//
// Description:
//
//
// Author: Yizao Wang <yizao.wang@polytechnique.edu> and Sylvain Gelly <gelly@lri.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "randomgoplayer_mg.h"
//#include "qscoreevaluator_mg.h"
#include <math.h>

using namespace MoGo;


MoGo::RandomGoPlayerNear::RandomGoPlayerNear(int thresholdRandom, int patternSize, bool stableRegion, bool useFirstMove, bool chooseNearestFromCenter, const double komi) {/**assert(0);**/
  setKomi(komi);
  lastOtherPlayerMove=-1;
  this->patternSize=patternSize;
  res.resize(patternSize*patternSize);
  locations.resize(patternSize*patternSize);
  this->thresholdRandom = thresholdRandom;
  this->tryToChooseCleverMove = false;
  this->stableRegion = stableRegion;
  this->useFirstMove = useFirstMove;
  this->chooseNearestFromCenter = chooseNearestFromCenter;
}


MoGo::RandomGoPlayerNear::~RandomGoPlayerNear( ) {/**assert(0);**/}


RandomGoPlayerNear * MoGo::RandomGoPlayerNear::clone( ) const {/**assert(0);**/
  return new RandomGoPlayerNear(*this);
}

std::string MoGo::RandomGoPlayerNear::getName( ) const {/**assert(0);**/
  return "RandomGoPlayerNear";
}

void MoGo::RandomGoPlayerNear::setKomi( const double komi ) {/**assert(0);**/
  this->komi=komi;
}


Location MoGo::RandomGoPlayerNear::generateOneMove( const Goban & , double  ) {/**assert(0);**/
  assert(0);
}

Location MoGo::RandomGoPlayerNear::generateOneMove( const Location lastOtherPlayerMove, const Goban & goban, double  ) {/**assert(0);**/
  Location other=-1;
  if (lastOtherPlayerMove >= 0)
    other = lastOtherPlayerMove;
  else
    other = this->lastOtherPlayerMove;
  bool uniformRandomMode=(thresholdRandom>=0) ? (goban.approximateSizeOfPossibleMoves() <= thresholdRandom) : (goban.approximateSizeOfPossibleMoves() > -thresholdRandom);
  if (uniformRandomMode || (other == -1) || (other == PASSMOVE)/* || ((MoGo::GoTools::mogoRand()%5) == 0)*/) {/**assert(0);**/
    this->lastOtherPlayerMove=goban.randomOutOneMove();
    return this->lastOtherPlayerMove;
  } else {/**assert(0);**/
    double minDistanceToCenter=1e50;int minDistanceToCenterIndex=-1;
    int x=goban.xIndex(other);int y=goban.yIndex(other);
    assert((patternSize%2)==1);
    int dec=patternSize/2;
    double nbOk=0;
    //     MoGo::GoTools::print("x %i, y %i\n", x, y);
    for (int i = 0 ; i < patternSize ; i++) {/**assert(0);**/
      for (int j = 0 ; j < patternSize ; j++) {/**assert(0);**/
        if (i == dec && j == dec) continue;
        Location index=gobanIndex(goban, x+i-dec,y+j-dec);
        //   MoGo::GoTools::print("x %i, y %i index %i\n", i, j, index);
        if (index < 0) {/**assert(0);**/
          res[i*patternSize+j]=0;
        } else {/**assert(0);**/
          if (goban.isLegal(index) && goban.isNotUseless(index)) {/**assert(0);**/
	    res[i*patternSize+j]=1;//2*dec-(abs(i-dec)+abs(j-dec));
            locations[i*patternSize+j]=index;
            nbOk += res[i*patternSize+j];
            if (chooseNearestFromCenter) {/**assert(0);**/
              double d=distanceToCenter(goban,x+i-dec,y+j-dec);
              if (d < minDistanceToCenter) {/**assert(0);**/ minDistanceToCenter=d; minDistanceToCenterIndex=i*patternSize+j;}
            }
          } else
            res[i*patternSize+j]=0;
        }
      }
    }

   
    if (nbOk <= 0.) {/**assert(0);**/
      this->lastOtherPlayerMove=goban.randomOutOneMove();
      return this->lastOtherPlayerMove;
    }
    if (chooseNearestFromCenter)
      return locations[minDistanceToCenterIndex];
    double okSeen=0.;double index=randDouble()*nbOk;

    for (int i = 0 ; i < (int)res.size() ; i++) {/**assert(0);**/
      okSeen += res[i];
      if (okSeen>index) {/**assert(0);**/
        Location l=locations[i];
        if (!stableRegion)
          this->lastOtherPlayerMove=l;
        return l;
      }
    }
    assert(0);
    return PASSMOVE;
  }
}


void MoGo::RandomGoPlayerNear::start( const Goban & goban, const Location lastOtherPlayerMove ) {/**assert(0);**/
  GoPlayer::start(goban, lastOtherPlayerMove);
  reset(lastOtherPlayerMove);
}

void MoGo::RandomGoPlayerNear::reset(const Location lastOtherPlayerMove) {/**assert(0);**/
  if (useFirstMove)
    this->lastOtherPlayerMove=lastOtherPlayerMove;
  else
    this->lastOtherPlayerMove=-1;
}

Location MoGo::RandomGoPlayerNear::gobanIndex( const Goban & goban, int i, int j ) {/**assert(0);**/
  //   MoGo::GoTools::print("gobanIndex %i, %i\n", i, j);
  if (i < 0)
    return -1;
  if (j < 0)
    return -1;
  if (i >= goban.height())
    return -1;
  if (j >= goban.width())
    return -1;
  //   MoGo::GoTools::print("fsdfds\n");
  return goban.indexOnArray(i,j);
}

double MoGo::RandomGoPlayerNear::distanceToCenter( const Goban & goban, int i, int j ) {/**assert(0);**/
  double dx=i-goban.height()/2;
  double dy=j-goban.width()/2;
  return sqrt(dx*dx+dy*dy);
}


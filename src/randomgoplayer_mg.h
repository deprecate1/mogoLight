//
// C++ Interface: randomgoplayer_mg
//
// Description:
//
//
// Author: Yizao Wang <yizao.wang@polytechnique.edu> and Sylvain Gelly <gelly@lri.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MOGORANDOMGOPLAYER_MG_H
#define MOGORANDOMGOPLAYER_MG_H

#include "goplayer_mg.h"

namespace MoGo {
  
  /** Plays randomly but always near the last move of the preceding player */
  class RandomGoPlayerNear : public GoPlayer {
  public:
    RandomGoPlayerNear(int thresholdRandom, int patternSize, bool stableRegion, bool useFirstMove, bool chooseNearestFromCenter, const double komi = 7.5);
    
    virtual ~RandomGoPlayerNear();
    
    /** Given a situation of goban, generate a move within time limited.*/
    virtual Location generateOneMove(const Goban &goban, double time=60);
    /** Generate a move in a game.
	The player is assumed to play always the same color and the other move is given by "lastOtherPlayerMove".
	The goban is here only for convenience as it could be deduced from the past moves.
	The move given by the player is immediately played (please return only legal moves!!!).
    */
    virtual Location generateOneMove(const Location lastOtherPlayerMove, const Goban &goban, double time=60);
    /** Clone itself */
    virtual RandomGoPlayerNear *clone() const;
    virtual std::string getName() const;
    virtual void setKomi(const double komi);
    //     void setRandomMode( const int mode );
    /** Start a game from goban */
    virtual void start(const Goban &goban, const Location lastOtherPlayerMove=-1);
    /** Return the status of the player */
    virtual void reset(const Location lastOtherPlayerMove);
    static double distanceToCenter(const Goban &goban, int i, int j);
  private:
    static Location gobanIndex(const Goban &goban, int i, int j);
    double komi;
    Location lastOtherPlayerMove;
    int patternSize;
    Vector<double> res;
    Vector<Location> locations;
    //     int randomMode;
    int thresholdRandom;
    bool tryToChooseCleverMove;
    bool stableRegion;
    bool useFirstMove;
    bool chooseNearestFromCenter;
  };

}

#endif

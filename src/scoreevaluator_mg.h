//
// C++ Interface: scoreevaluator_mg
//
// Description:
//
//
// Author: Yizao Wang <yizao.wang@polytechnique.edu> and Sylvain Gelly <gelly@lri.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MOGOSCOREEVALUATOR_MG_H
#define MOGOSCOREEVALUATOR_MG_H

#include "typedefs_mg.h"
#include "goban_mg.h"
#include "treeupdator_mg.h"

class RlMogoWrapper;
class FastBoard;

namespace MoGo {

 
  class ScoreEvaluator {
  public:
    ScoreEvaluator() ;
    virtual ~ScoreEvaluator();

    /** Evaluate a position in a game(thus goban must be given). Return an evaluation with a certainty.
      The evaluation is for the position between 0 and 1 (not all subclasses respect that though...). This evaluation is for the black player.*/
    virtual double evaluate(const Goban &goban,double* certainty=0,double time=0.)=0;
    /** evaluate the position (between 0 and 1) for black and return an evaluation for each moves from this position. The vector is n*n entry and illegal moves have eval 0 (again for black, 1 if it's white's turn)*/
    virtual Vector<double> evaluateAll(const Goban &goban, double *certainty=0);
    virtual double evaluate(Goban &goban, NodeSequence & currentSequence);//non-const for the ZonesScoreEvaluator
    virtual void setKomi(double komi);
    /** This is for the metaplayer. Normally I should have set const = 0 but for there are some methods created by Sylvain and I dont want to change his codes for the moment.

    Ha!! I have said this? sooooo funny.*/
    virtual ScoreEvaluator* clone() const ;
    virtual void loadRootGoban(const Goban & goban, const Location lastMove );
    virtual void loadRootGoban(const Goban & goban, const Vector < Location > & lastMoves );
    virtual bool isUseful(const Goban & goban,double time = 100.) const;
  protected:
    double komi;

  };
  class RlScoreEvaluator : public ScoreEvaluator {
  public:
    RlScoreEvaluator(RlMogoWrapper *rlWrapper, int learnMode, int useMode, bool useMC, double coefAmplitude);
    ~RlScoreEvaluator() {}
    virtual double evaluate(const Goban &goban,double* certainty=0,double time=0.);
    virtual double evaluate(Goban &goban, NodeSequence & currentSequence);
    virtual bool isUseful(const Goban & goban,double time = 100.) const;
    virtual ScoreEvaluator* clone() const {
      return new RlScoreEvaluator(*this);
    }
    virtual void loadRootGoban(const Goban & , const Vector < Location > & );
    virtual Vector<double> evaluateAll(const Goban &goban, double *certainty=0);
    void reset();
    void newGame(const Goban &goban);
    void newGenmove(const Goban &goban);
    void play(Location l, bool blackPlay);
    void update(bool blackWins);
    /** Simulate many complete games from specified MoGo position */
    void think(const Goban &goban, int numgames);
  private:
    RlMogoWrapper *rlWrapper;
    bool useMC;
    double coefAmplitude;
    int learnMode; int useMode;
  };
  /** Give a bonus for moves like atari, cut, etc... Only the evaluate(goban) (give evaluation for all possible moves) is implemented
      This evaluator does not fullfill the constraint of having values in [0,1]
  */
  class SimpleHeuristicScoreEvaluator : public ScoreEvaluator {
  public:
    SimpleHeuristicScoreEvaluator(double coefAmplitude, int mode, double valueForSaveMove, double equivalentSimulationsForInitializationForSaveMove, double decDistance, double coefDistance, double powDistance) {
      this->coefAmplitude=coefAmplitude;this->mode=mode; this->valueForSaveMove=valueForSaveMove; this->equivalentSimulationsForInitializationForSaveMove=equivalentSimulationsForInitializationForSaveMove;
      this->decDistance=decDistance; this->coefDistance=coefDistance;
      this->powDistance=powDistance;
    }
    ~SimpleHeuristicScoreEvaluator() {}
    virtual double evaluate(const Goban &goban,double* certainty=0,double time=0.);
    virtual double evaluate(Goban &goban, NodeSequence & currentSequence);
    virtual bool isUseful(const Goban & goban,double time = 100.) const;
    virtual ScoreEvaluator* clone() const {
      return new SimpleHeuristicScoreEvaluator(*this);
    }
    virtual void loadRootGoban(const Goban & , const Vector < Location > & );
    virtual Vector<double> evaluateAll(const Goban &goban, double *certainty=0);
    int getMode() const {
      return mode;
    }
    double evaluateMoveByHeuristic(const Goban &goban, Location move);
    double evaluateMoveByHeuristicOnFastBoard(const FastBoard &board, Location move, Location saveMove, int numThread);
    void initialiseNodeUrgencyByFastBoard(const FastBoard &board, GobanNode *node, double equivalentSimulationsForInitialization);
    void initialiseNodeUrgencyByFastBoardAndDistances(const FastBoard &board, GobanNode *node, double equivalentSimulationsForInitialization, double *distances);

    void initialiseNodeUrgencyByFastBoardAndDistancesAnd5x5(const FastBoard &board, GobanNode *node, double equivalentSimulationsForInitialization, double *distances);
  private:
    double coefAmplitude;
    double valueForSaveMove;
    double equivalentSimulationsForInitializationForSaveMove;
    double equivalentSimulationsByDistance(double distance) const;
    double decDistance; double coefDistance; double powDistance;
    int mode;
    static MoGoHashMap(long long, float, hashLongLong, equalLongLong) patternElos;
  };


  class StupidCenterOpeningEvaluator : public ScoreEvaluator {
    friend class Goban;
    friend class StringInformation;
    friend class LocationInformation;
  public:
    StupidCenterOpeningEvaluator( int mode = 0, int height = 9, int width = 9 );
    ~StupidCenterOpeningEvaluator();
    void setCenterOpeningAreaBorder(const Vector < Location > & openingAreaBorder);
    virtual double evaluate(const Goban &goban,double* certainty=0,double time=0.);

    virtual ScoreEvaluator* clone() const;


  private:

    /**Here the openingAreaBorder saves the border locations. What's more, the first and the last location should be connected and repeated once in favor of the score function. More precisely, a n-length border is saved as an n+2-length vector. The order of the locations must assure the continuity of the border.*/
    Vector < Location > openingAreaBorder;
    Vector < Location > bonusLocation;
    Vector < int > bonusLocationIndex;
    int mode;
    int height;
    int width;
    void initiateDefaultAreaBorder();
    void initiateBonusLocation();
    void countBonus( const Goban & goban, double & score ) const;
    void countStones( const Goban & goban, double & score ) const;
    void countBorderScore( const Goban & goban, double & score ) const;
    void countUnderAtariStones( const Goban & goban, double & score ) const;
  };
  


 

}

#endif

//
// C++ Interface: moveselector_mg
//
// Description:
//
//
// Author: Yizao Wang <yizao.wang@polytechnique.edu> and Sylvain Gelly <gelly@lri.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MOGOMOVESELECTOR_MG_H
#define MOGOMOVESELECTOR_MG_H

#include "typedefs_mg.h"
#include "gobannode_mg.h"
#include <queue>

namespace MoGo {



  class NodeSequence;
  class UCTUpdatorGlobal;
  class TreeUpdator;
  class ScoreEvaluator;
  class LocalPatternsDatabase;
  class FastBoard;
  class GoGameTree;
  /**
  To select one move given a node (where a gobanKey is saved, then a goban).
  It is possible that a sequence of node (goban) is given in order to have more information (history) on the game.

  We have A LOT here. but only the MoveSelector, MoveSelector1 are useful at the very end.
   
  @author Yizao Wang, Sylvain Gelly
  */
  class MoveSelector {
    friend class InnerMCGoPlayer;

  public:
    MoveSelector();
    virtual ~MoveSelector();
    /** Select a move on the node given (corresponding to the goban). The returned move MUST be legal. The child classes have to reimplement either this function or the selectOneMoveUnsafe one */
    virtual Location selectOneMove( GobanNode * node, const Goban & goban, const int depth = 0 );
    /** Select a move on the node given (corresponding to the goban). The returned move doesn't have to be legal. The child classes have to reimplement either this function or the selectOneMoveUnsafe one */
    virtual Location selectOneMoveUnsafe(GobanNode * node, const Goban & goban);
    virtual Location selectOneMoveMPI( GobanNode * /*node*/, const Goban & /*goban*/, const int /*depth*/ = 0 ) {assert(0);return PASSMOVE;}
    virtual void neverPlayMove(GobanNode * node, const Location &location);
    virtual void firstPlayMove(GobanNode * node, const Location &location);
   
    virtual void updateUrgencyByCurrentSequence(const NodeSequence & currentSequence);
    /** clone the object (return a copy of itself) */
    virtual MoveSelector *clone() const {return new MoveSelector(*this); }
    virtual double valueOfLocation( const GobanNode * , const Location  ) const { return -5;}
    virtual double valueOfLocationForDisplay( const GobanNode * node, const Location location ) const { return valueOfLocation(node, location);}
    virtual Vector < Location > preferedMoves( GobanNode * node, const Goban & goban, const int number, Vector<GobanNode *> *preferedChilds=0 );
    void clearForbiddenMoves();
    void addForbiddenMove(Location move);
    void removeForbiddenMove(Location move);
    void showForbiddenMoves();
    

  


    void showCurrentSequenceBitmap( const NodeSequence & currentSequence) const;

    void setLastMoves(Vector < Location > & lastMoves);
    void setOpeningMode ( int mode = 0, int openingPeriodMoveNumber = 10 );
    void setUpdator(TreeUpdator *treeUpdator) { this->treeUpdator=treeUpdator; }
    void setDatabase(LocalPatternsDatabase *localDatabase, int localDatabaseMode) {
      this->localDatabase=localDatabase; this->localDatabaseMode=localDatabaseMode;
    }
    void setTenukiMode(int tenukiMode) { this->tenukiMode=tenukiMode; }
    virtual void clearCacheHistory() {}
    Location getHighestValueMoveInLastNode() const {return highestValueMoveInLastNode;}
    double getHighestValueInLastNode() const {return  highestValueInLastNode;}

    Location setBeforeLastMove(const Goban& gob) { before_last_move = gob.lastLastMove(); return before_last_move;}
    Location setBeforeLastMove(const Location blf) { before_last_move = blf; return blf;}

  protected:
    /** This variable helps solving the superko situations */
    FastSmallVector<Location> forbiddenMoves;
    int freeLocationSize;
    int distance1;
    int distance2;
    Vector<Location> lastMoves;
    int openingMode;
    int openingMoveNumber;
    bool initiateGobanNodeUrgencyWhenAsked;
    TreeUpdator *treeUpdator;
    LocalPatternsDatabase *localDatabase;
    int localDatabaseMode;
    int tenukiMode;
    Location highestValueMoveInLastNode;
    double highestValueInLastNode;

    //JBH
    Location before_last_move; //use by expertise-go basic_instinct
  };

 
  

  class UCTSelector1 : public MoveSelector {
  public:
    UCTSelector1(int selectorMode=0);
    virtual~UCTSelector1();
    virtual Location selectOneMove( GobanNode * node, const Goban & goban, const int depth = 0 );
    virtual Location selectOneMoveMPI( GobanNode * node, const Goban & goban, const int depth = 0 );
    /** clone the object (return a copy of itself) */
    virtual UCTSelector1 *clone() const {return new UCTSelector1(*this); }
    virtual double valueOfLocation( const GobanNode * node, const Location location ) const;
    virtual double valueOfLocation( const GobanNode *child) const;
    virtual double valueOfLocationForDisplay( const GobanNode * node, const Location location ) const;
    virtual double valueOfLocationForDisplay( const GobanNode *child ) const;
    virtual Vector < Location > preferedMoves( GobanNode * node, const Goban & goban, const int number, Vector<GobanNode *> *preferedChilds=0);
  private:
    int selectorMode;

  };


  /** A move selector which dont use the urgency of the node, but a global urgency array */

  class MoveSelectorGlobal : public MoveSelector {
    friend class Goban;
    friend class UCTUpdatorGlobal;

  public:
    
    static double weight_stones_quantity;
    static double pow_stones_quantity;

    static double coef_expertise;

    #ifdef CRITICALITY
    static double coef_criticality;
    static double coef_variance;
    #endif


    MoveSelectorGlobal(int useDistancesByBlock, double powStarCountDown, int nBestSize, int startCountDown, int maxRandomOptimizeSelectMove, int useBitmapNeighborhood, int useNewTrainOneGame, int avoidDeltaForRoot, double deltaPow, double delta, double nbEqEstimator2, double bias2, double coefBonusAMAF, double biasInAMAFCoef, double powTerritoriesBonus, double movingConstantSpeed, int movingConstantMode, int keepBonusNbSimulations, int historySizeForPrecomputed, int optimizeSelectMove, double bias, double limitInterestingMoveCoef, int passOnlyIfNoChoice, double powForLogReplacement, int interestingMoveMode, double minInterestingMoves, double powInterestingMoves, double coefInterestingMoves, int hardLocalFirst, int randomModeInTree, int nbSimulationsRandomInTree, int equivalentSimulationsForInitialization, ScoreEvaluator *optionalInitialization, bool initiateGobanNodeUrgencyWhenAsked, int localFirst, int maxDepth, double territoriesLimit, bool initializeByGrandFather, int stopUsingGlobal, double explorationParam, double explorationParam2, double coefParam, double coefPow, int urgencyUpdatorMode, int urgencySelectorMode, double urgencyUpdatorParameter2, double wAmaf);
    virtual ~MoveSelectorGlobal();
    virtual Location selectOneMove( GobanNode * node, const Goban & goban, const int depth = 1 );
    virtual Location selectOneMoveNew( GobanNode * node, const Goban & goban, int &nextChildIndex, int mode);
    virtual Location selectOneMoveNewExplorator( GobanNode * node, const Goban & goban, int &nextChildIndex, int mode);
  
#ifdef GOLDENEYE
    void initiateGobanNodeUrgencyWithFastBoard(GobanNode * node, const Goban &gobanAtRoot, FastBoard &board, const NodeSequence & currentSequence, GoGameTree *playerTree);
#else
    void initiateGobanNodeUrgencyWithFastBoard(GobanNode * node, const Goban &gobanAtRoot, const FastBoard &board, const NodeSequence & currentSequence, GoGameTree *playerTree);
#endif
    double valueForInterestingMoveFastBoard( GobanNode * node, const Goban & gobanAtRoot, const FastBoard &board, Location move, int mTI, Location &savingMove);


    virtual void neverPlayMove(GobanNode * node, const Location &location);
    virtual void firstPlayMove(GobanNode * node, const Location &location);
    virtual void updateUrgencyByCurrentSequence(const NodeSequence & currentSequence);
    /** clone the object (return a copy of itself) */
    virtual MoveSelectorGlobal *clone() const {return new MoveSelectorGlobal(*this); }
    //     virtual double valueOfLocation( const GobanNode * node, const Location location ) const ;

    virtual Vector < Location > preferedMoves( GobanNode * node, const Goban & goban, const int number, Vector<GobanNode *> *preferedChilds=0 );
    virtual UCTUpdatorGlobal *updator();
    virtual void clearCacheHistory();
    ScoreEvaluator *getOptionalInitialization() {
      return optionalInitialization;
    }
    void lockThisNode(GobanNode *node);
    void unlockThisNode(GobanNode *node);
    void updateNodeBitmap(const Goban &goban, GobanNode *node);
  private:
    void updateNodeUrgency( GobanNode * node, const Goban &goban, const int depth = 1 );
    int urgencyUpdatorMode;
    int urgencySelectorMode;
    double urgencyUpdatorParameter2;
    double explorationParam;
    double explorationParam2;
    double coefParam;
    double coefPow;
    int stopUsingGlobal;
    Location selectOneMove17New(GobanNode * node, const Goban & goban, int &nextChildIndex, int mode);
    Location selectOneMove17NewExplorator(GobanNode * node, const Goban & goban, int &nextChildIndex, int mode);
  
    
  
   
    //     Vector<double> urgency;
    Vector<GobanNode *>updatedUrgencyNode;
    Vector<Vector<Vector<double> > >urgencies;
    MoGoHashMap(int, int, hashInt, equalInt) nodeToUrgencyNumber;
    Vector<Vector<GobanNode *> >historyOfUpdatedNodes;
    Vector<int> kForHistoryOfUpdatedNodes;
    Vector<int> lastKForHistoryOfUpdatedNodes;
    bool initializeByGrandFather;
    double territoriesLimit;
    double powTerritoriesBonus;
    int maxDepth;
    int localFirst;
    int hardLocalFirst;
    ScoreEvaluator *optionalInitialization;
    int equivalentSimulationsForInitialization;
    int nbSimulationsRandomInTree;
    int randomModeInTree;
    double coefInterestingMoves;
    double minInterestingMoves;
    double powInterestingMoves;
    double powForLogReplacement;
    int interestingMoveMode;
    int passOnlyIfNoChoice;
    double limitInterestingMoveCoef;
    double bias;
    double Wamaf;
    int optimizeSelectMove;
    int maxRandomOptimizeSelectMove;
    int keepBonusNbSimulations;
    double saveValueForInterestingMoveValue(GobanNode * node, const Goban & goban, Location move, double v);

    int movingConstantMode;
    double movingConstantSpeed;
    double biasInAMAFCoef;
    double coefBonusAMAF;
 

    //     Vector<double> correlations;
    double nbEqEstimator2;
    double bias2;
 

   
  
    double delta;
    double deltaPow;
    int avoidDeltaForRoot;
    int useNewTrainOneGame;
    Vector<GobanNode *>lockedNodes;
    mutable pthread_mutex_t nodesMutex;
    mutable pthread_mutex_t blockNodeMutex;
    bool findOrAddBlockedNode(GobanNode *node);
    int useBitmapNeighborhood;
    int startCountDown;
    int nBestSize;
    double powStarCountDown;
    int useDistancesByBlock;
   

    Location chooseBest17FromNBest(GobanNode * node, const Goban & goban, int &nextChildIndex, double biasToUse, double coefInterestingMovesModified, FastBoard *board, int mTI, Location savingMove);
    double greedyValueForMoveWithChild(GobanNode * node, const Goban & goban, int index, double biasToUse, double coefInterestingMovesModified, FastBoard *board, int mTI, Location savingMove, float stones_density) ;
    double greedyValueForMoveWithoutChild(GobanNode * node, const Goban & goban, Location location, int i, int j, double biasToUse, double coefInterestingMovesModified, FastBoard *board, int mTI, Location savingMove, float stones_density) ;

    void computeDistancesByGroup(const FastBoard &board, Location startingPoint, double *distances) const;

    void addStringPositionsToFrontWithDistance(const FastBoard &board, Location location, double refDistance, std::queue<Location> &fifo, double * distances) const;

  };





}

#endif

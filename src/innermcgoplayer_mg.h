//
// C++ Interface: innermcgoplayer_mg
//
// Description:
//
//
// Author: Yizao Wang <yizao.wang@polytechnique.edu> and Sylvain Gelly <gelly@lri.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MOGOINNERMCGOPLAYER_MG_H
#define MOGOINNERMCGOPLAYER_MG_H

#include "goplayer_mg.h"
#include "typedefs_mg.h"
#include "goban_mg.h"
#include "gogametree_mg.h"
#include "gobannode_mg.h"
#include "gobanurgency_mg.h"
#include "moveselector_mg.h"
#include "treeupdator_mg.h"
#include <math.h>
#include <pthread.h>
#include <string>


#ifdef GOLDENEYE
#include "semeai.h"
#include "bibliotheque.h"
#include "all_include.h"
#endif



namespace MoGo {
  class CPUTimeChronometer;
  class ScoreEvaluator;
  class MachinesPool;
  class RegisteredGobanSimulations;
  class LocalPatternsDatabase;
 

  /**
  This is the core part of MoGo. It keeps one tree, callsMsimulationMoveSelector

  - simulationMoveSelector for choosing child node in the tree; (For example using UCB1 formulae)

  - random simulations or ScoreEvaluator for giving a score; (When the random simulation is called, then one random game is simulated by randomMode, and the score is returned. The ScoreEvaluator may return one score without random simulation, although this kind of ScoreEvaluator is too strong and then too rare.)

  - treeUpdator for value update (in the UCT case, updating the mean and variance value of each node passed in the last simulation. More precisely, we keep the sum of the mean and the sum of the square of the value.);

  - playMoveSelector for playing one move. (In UCT case, this is different from the UCB1 formulae. We then compare only the mean term without counting the confident bound.)
   
   (Play one move is different from simulate one in UCB. In the former only the mean is considered.)
   
   Mainly generateOneMove is called from outside. generateOneMove normally calls selfTrain, which is the core of this class.
   
   
  @author Yizao Wang, Sylvain Gelly
  */
  class InnerMCGoPlayer : public GoPlayer {
    //friend class NodeValue;
    friend class GoExperiments;
  public:
#ifdef LEARNING
        static int miningMove;
#endif
#ifdef JYMODIF_NEW
	volatile int nbRemainingSimulations;
	int totalSims;
#endif
#ifdef JY2
	int myIndex[50];
#endif
    int lastShare;
    /** the simulationMoveSelector, the moveSelector and treeUpdator objects are assumed to be owned by the caller. Theses objects are cloned in the constructor. If the moveSelector or the treeUpdator are omitted (or NULL), some default ones are created */
    InnerMCGoPlayer(const Goban & goban,
                    const MoveSelector * simulationMoveSelector = NULL,
                    const MoveSelector * playMoveSelector = NULL,
                    const TreeUpdator * treeUpdator = NULL,
                    const double komi = 0);
    //     InnerMCGoPlayer(const Goban & goban,
    //                     InnerMCGoPlayer * innerMCGoPlayer);
    virtual ~InnerMCGoPlayer();

    /** Given a situation of goban, generate a move within time limited. generateOneMove normally will first call selfTrain, which generates one tree in the memory by iterative simulations. Then one move is retured according value of the chid-nodes of the root node. */
    virtual Location generateOneMoveNOMPI(const Goban &goban, double time=60);
    virtual Location generateOneMove(const Goban &goban, double time=60);
    /** Given a situation of goban, give the evaluation of this position. If this position is unseen in the tree,
        then return 0. Return also the number of simulations (0.5 if unseen). The value is the probability of winning given for the black. */
    double evaluatePosition(const Goban &goban, int &nbSimulations);

    /** Clone itself */
    virtual GoPlayer *clone() const;

    /** Return true iif the goban has an associated node in the tree */
    bool isGobanSavedBefore(const Goban & goban) const;

    //     virtual bool isLastOtherPlayerMoveLegal(const Location lastOtherPlayerMove) const ;
    /** show the memory state.*/
    void showHashMapSize() const;
    /** by default(firstMove = -1), showPreferedSequence shows the prefered sequence proposed by the current tree. otherwise it looks for the firstMove at the root and shows the prefered sequence lead by that move.*/
    void showPreferedSequence( const Goban & goban, NodeSequence &currentSequence, const Location firstMove = -1, const int number = 3  ) ;
    void showPreferedSequence( const Goban & goban, const Location firstMove = -1, const int number = 3  ) ;
    virtual void showPreferedMoves( const Goban & goban, const int number );

    int getDeepestDepthOfTree() const { return deepestDepthOfTree; } ;

    void dottyOut(const int number = 0) const {playerTree.dottyOut(number); } ;
    void showGobanUrgency(const Goban & goban, const NodeSequence &currentSequence) ;
    void showGobanUrgency(const Goban & goban) ;


    /** This is the core part of InnerMCGoPlayer. In general selfTrain consists of:
     
    - selfTrainInitiation : This is done once during each training (containing thousands of thousands simulations). It deals mainly with whether keep the tree kept before.
     
    - selfTrainOneGame : This corresponds one simulation, containing one part in the tree and one part giving one score (usually by calling selfTrainFastToEndAndGetOneScore).
     
    - selfTrainFastToEndAndGetOneScore : from one goban (usually the goban at one leaf of the tree), generates random moves until the game is finished and return the score.
     
    */
    void selfTrain(const Goban & goban, const int number, const double time = 15, int multiThreadsIdentifier=0);
    void selfTrainNOMPI(const Goban & goban, const int number, const double time = 15, int multiThreadsIdentifier=0);
    void selfTrainNew4( const Goban & goban, const int number, const double time, int multiThreadsIdentifier);
    void selfTrainNew4NOMPI( const Goban & goban, const int number, const double time, int multiThreadsIdentifier);
    void selfTrainNewSlave4(const Goban & goban, int multiThreadsIdentifier);
    void slaveOneOperation(const Goban &goban, NodeSequence &currentSequence, Vector<Location> &currentSequenceMoves, int multiThreadsIdentifier, GobanNode*lastNode, int choiceB, int choiceW, int mode=0,double weightSimu=1.,Vector<int>* guessLocation=0,Vector<int>* guessColor=0,Vector<double>* guessWeight=0);
    void slaveOneOperationAfterScore(double score, const Goban &goban, NodeSequence &currentSequence, Vector<Location> &currentSequenceMoves, int multiThreadsIdentifier,GobanNode*lastNode,int choiceB, int choiceW,double weightSimu);

    void selfTrainNewMaster(const Goban & goban, const int number, const double time, int multiThreadsIdentifier, int mode);
    void selfTrainNewMasterNOMPI(const Goban & goban, const int number, const double time, int multiThreadsIdentifier, int mode);
    void selfTrainMultithreads(const Goban & goban, const int number, const double time = 15);
    void selfTrainMultithreadsNOMPI(const Goban & goban, const int number, const double time = 15);
    //     void *selfTrainThread(void *infos);

    virtual std::string getName() const;
    virtual void setKomi(const double komi);

    void setRandomModes(const int mode, const int mode2) {randomMode=mode; randomMode2=mode2;}
    ///////      void setRandomModeByGTPEngine(const int mode, const Goban & goban);
    void setCorrelationMode( const bool modeOn ) ;
    int numberOfSharedNodes() const;
    Vector < GobanNode * > sharedNodes() ;
    bool hasResigned()const;
    void setPlayerForFastFinishingGame(GoPlayer *playerForFastFinishingGame);
    void setPositionEvaluator(ScoreEvaluator *positionEvaluator, bool deletePositionEvaluator=false);
    void setNbSimulationsForEachNode(int nbSimulationsForEachNode);
    void setNoiseLevel(double noiseLevel);
    void setOpeningMode(int mode = STUPID_CENTER_OPENING_MODE);
    void setBlockMode(int mode);
    void setNbTotalSimulations(int nbTotalSimulations);

    double getKomi() const;
    void setKeepTreeIfPossible(bool keepTreeIfPossible);
    bool getKeepTreeIfPossible() const { return keepTreeIfPossible; }
    bool isTreeAtSizeLimit() const;

    /** Start a game from goban */
    virtual void start(const Goban &goban, const Location lastOtherPlayerMove=-1);
    /** Tells to the player that the game is finished and gives the score */
    virtual void finish(double score);
    void setGobanSize( int gobanSize );
    static void setLimitTreeSize ( int treeSize );
    static void setFirstMoveFixed ( int value ) {firstMoveFixed = value;}
    /** in order to get a range more reasonable for the simulations. put in public for test. */
    void getBitmapByCombiningCurrentSequenceBitmap(Bitmap & bitmap, NodeSequence & currentSequence, const Vector < Location > &currentSequenceMove) const;
   
    void setImitateMode(int mode, int moveNumber);


    static bool dontDisplay;
    bool getInfosOnNode(const GobanKey &key, double &value, int &nbSimulations);
    /** In the format of GobanState (height+2)*(width+2) */
    Vector<int>getNbSimulationsAtRoot() const;

    static bool earlyStop;
    /** stopDifference = true: if the difference of the stones eaten of the two players are bigger than stopDifference, then stop random simulations. for acceleration of simulations on large Goban.*/
    static int stopDifference;
    /** allowDoublePassInTree = 1: consider the double pass case. 0 only if no other move is possible. -1 never double pass*/
    static int allowDoublePassInTree;
    /** if the goban is with range (normally the range is the entire goban), then in the random simulations the random moves are first played in the range.*/
    static bool fillRangeFirstMode;
    virtual int getRandomMode() const;
    static int nbThreads;
    static int securityThreads;
    /** accelerateMode = true: If the most searched move is 10 times more searched than the second most, then stop. */
    static int accelerateMode;
    /** shishoCheckMode = true: check the shicho case for the strings. Very important in the large Goban.*/
    static int shishoCheckMode;
    static int shishoCheckModeNew;
    /** Sylvain made this. From its name, using the territory as the score instead of 0/1.*/
    static int territoryWinningMode;
    /** similar as accelerateMode, made by Sylvain.*/
    static int earlyCut;
    static double earlyCutRatioFirstMove;
    static double earlyCutRatioVSSecondMove;

    static int mixRandomMode1;
    static int mixRandomMode2;
    /** One out of mixRandomModeRatio times mixRandomMode2 is used. Otherwise mixRandomMode1 is used. Intuitively this gives a random mode more robust. We don't have time to try this on CGOS.*/
    static int mixRandomModeRatio;
    static int mixRandomModeCounter;

    void forgetTraining();

    /** resignThreshold is set as 0.99 (the estimation of the node at depth 1, then the estimation of opponent) by default.*/
    static double resignThreshold;
    /** in openingTestMode, stop the game when the winning rate is bigger than 1-openingTestThreshold*/
    static int openingTestMode;
    static double openingTestThreshold;
    /** together with openingTestMode */
    static bool earlyStopForOpeningTest;
    void setLocalDatabase(LocalPatternsDatabase *localDatabase, int localDatabaseMode) {
      this->localDatabase=localDatabase; this->localDatabaseMode=localDatabaseMode;
    }
    void setKomiVariation(double addKomiMax, double addKomiMin, double addKomiAlpha) {
      this->addKomiMax=addKomiMax;this->addKomiMin=addKomiMin;this->addKomiAlpha=addKomiAlpha;
    }
    void setAutoKomi(int autoKomiMode, double winLimitIncreaseKomi, double winLimitDecreaseKomi, int autoKomiMin, int autoKomiMax, int iterationsBeforeModifyKomi);
    void setStartSimulationMode(int startSimulationMode, int startSimulationNbSimulations, int startSimulationNbNodes) {
      this->startSimulationMode=startSimulationMode;
      this->startSimulationNbSimulations=startSimulationNbSimulations;
      this->startSimulationNbNodes=startSimulationNbNodes;
    }
    void setInitiateStaticAnalysisOfBoardMode(int initiateStaticAnalysisOfBoardMode) {
      this->initiateStaticAnalysisOfBoardMode=initiateStaticAnalysisOfBoardMode;
    }
    static int stopAMAFOnBigCaptures;
    static int unlockChooseMove;
    static int exitTreeNbSimulations;
    static int useLibEgoTest;
    static int useFastBoard;
    const GoGameTree &getTree() const;
    double getLastNbSimulationsUsed() const {
      return lastNbSimulationsUsed;
    }

  private:
    GoGameTree playerTree;
    void share(GobanNode * node,int k,GoGameTree* playerTree);
#ifdef CREATE_OPENINGS
    void dump(GobanNode * node,std::vector<int> suite=std::vector<int>());
#endif

    void getFromDouble(double *summary,GobanNode * node,GoGameTree * playerTree);


    void putInDouble(double* summary,GobanNode *node);   //this function adds in "summary" the infos in node


    void lockSafer() const;
    void unlockSafer() const;
    void lock() const;
    void unlock() const;
    void lockOneGameSafer(int multiThreadsIdentifier) const;
    void unlockOneGameSafer(int multiThreadsIdentifier) const;
    void lockCutTree() const;
    void unlockCutTree() const;
    //     NodeSequence currentSequence;

    MoveSelector* simulationMoveSelector;
    MoveSelector* playMoveSelector;
    TreeUpdator* treeUpdator;
    GoPlayer *playerForFastFinishingGame;
    ScoreEvaluator *positionEvaluator;
    bool deletePositionEvaluator;
    bool isMCGoEvaluatorStarted;
    //     bool isInTheTreeNow;

    bool correlationModeOn;
    int correlationSeed;
    int deepestDepthOfTree;

    void selfTrainInitiation( const Goban &goban, NodeSequence &currentSequence, double time = 100. );

    /** selfTrainOneGame consists of simulating one game from the root to the very end of the game.

    In the tree, each situation of playerGoban corresponds one node. currentSequence saves these nodes in this simulation. 

    When there is no node corresponding the situation, then new node is created and added to the tree, and (normally) one random simulation is called to the very end in order to return one score. 

    */

 
    void showCurrentSequence( const Goban &currentGoban, const NodeSequence &currentSequence ) const; //Just for DEBUG
    void showCurrentSequenceSimple( const NodeSequence &currentSequence ) const ;
    void showCurrentSequenceBitmap(NodeSequence & currentSequence, const Vector < Location > &currentSequenceMove) const;


    void addPlayerGobanToNode( const Goban &currentGoban, GobanNode * node, const Location location );
    
    //void initiatePlayerGobanNodeValue( const Goban &currentGoban, GobanNode *node ) const;  //TOBEREMOVED
    void initiatePlayerGobanNode( const Goban &currentGoban, GobanNode *node, const NodeSequence & currentSequence, bool lock=false ) const;
    bool isPlayerGobanInCurrentSequence( const Goban &currentGoban, const NodeSequence &currentSequence );
    //void updateCurrentSequenceSumValueAndSumSquareOfValue(const double score);
    Location chooseOneMoveAtNode( GobanNode * node, const Goban &goban, const int depth );
    void showCurrentTree(CPUTimeChronometer &chronometer, const NodeSequence &currentSequence, int n) const ;
    double komi;
    int randomMode; int randomMode2;

    /** This answers the question: whether is it interesting to continue training. No interest when it definitely loses or wins. Be careful, even it definitely wins the game, it is not always that it can pass! */
    double isInterestingToContinue( const Goban & goban, NodeSequence &currentSequence );
    bool hasResigned_;
    int nbSimulationsForEachNode;
    double noiseLevel;
    void setSimulationRange(Goban & goban);
    int openingMode;
    int blockMode;
    int nbTotalSimulations;

    /** SuperKo check needs copy of the goban. */
    bool isSuperKo( const Goban & goban, NodeSequence *currentSequence=0) const;
    GobanNode *getNode(const Goban &goban, const NodeSequence &currentSequence, bool testForInitiate=true, bool lockUnlock=false) const;
    //    MoGoHashMap(GobanKey, int, hashGobanKey, equalGobanKey) historyGoban;
    bool keepTreeIfPossible;

    Location getMoveFromCurrentSequence( const NodeSequence &currentSequence,int n, const Vector < Location > &currentSequenceMove) const;
    /** Select one move using the selector avoiding superko */
    Location safeSelectOneMove(MoveSelector *selector, GobanNode *node, const Goban &goban, NodeSequence & currentSequence, bool considerPassAsNormal=false);
    NodeSequence currentSequence;
    //     NodeSequence currentSequence2;
    /** current sequence saves the nodes passed by in the tree in each simulation. */
    //     Vector<NodeSequence> currentSequences;
    Vector<NodeSequence> currentSequences;
    /** in fact we can get currentSequenceMove from currentSequences. Obviously we had fotgotten it... */
    //     Vector < Location > currentSequenceMove;
    Vector<Vector< Location > >currentSequenceMoves;
    //     Vector < Location > currentSequenceMove2;

    /** START For the Master-Slave option */
    Vector<Vector < Location > >currentSequenceMovesStack;
    Vector<NodeSequence> currentSequencesStack;
    //     Vector<Goban> currentGobanStack;
    Vector<Vector<int> >currentStateStack;
    Vector<Vector<int > >currentStateStackIndex;
    bool stopSlaves;
    mutable bool pauseSlaves;
    bool startSlaves;
    /** END For the Master-Slave option */

    int gobanSize;



    /** to control the tree size for the memory reason. */
    static int limitTreeSize;
    /** to control the tree size before removing nodes (kind of garbage collector) */
    static int collectorLimitTreeSize;
    /** to control the memory used by cutting the tree (memory in MB)*/
    static double limitMemoryUsed;
    /** some easy heuristic for opening. implementation in GoPlayer. */
    static int firstMoveFixed;
    int nbChildsAlreadyDone;
    int nbJobsSent;
    /** imitate-   play the symmetric move. */
    int imitateMode;
    int imitateMoveNumber;
    bool stillImitating;
    bool imitateMove( const Goban & goban, Location & location );
    int addingOwnSimulations; int multiplyingOwnSimulations; bool useSimpleServerVersion; bool treatMeanScore;
 
    MoGoHashMap(GobanKey, float, hashGobanKey, equalGobanKey) alreadyComputedGobans;
    int nbAlreadyComputedGobans;
    Vector<Location> computedMovesAtRoot;
    Vector<Location> forbiddenAddedMovesAtRoot;

    Goban undoGoban;
    mutable bool blockVariable;
    mutable bool saferBlockVariable;

   
    /** Remove some unused nodes */
    void cutTree();
    bool handleCutTree(int iteration);
    mutable pthread_mutex_t saferMutex;
    mutable pthread_mutex_t saferOneGameMutex;
    mutable Vector<pthread_mutex_t> workerMutexes;
    mutable pthread_mutex_t mutexProtectNodes;
    mutable pthread_mutex_t stackMutex;
    mutable pthread_mutex_t initiateMutex;

    int selfTrainAverageLength;
    static bool useLockSafer;
    static int useOneGameLocker;
    static bool allAllMovesAtFirst;
    static int imitateOnlyAsWhite;
    static int minNumberOfSimulationsOfResign;
    double firstSecondRatio;
    bool isAThreadInCutTree;
    int selfTrainCounter;
    Vector<Vector<int> >allAtFirstData;
    Vector<Vector<int> >allAtFirstDataInTree;
    Vector<FastSmallVector<int> >usedMovesAllAtFirst;
    Vector<Vector<int> >territoriesScores;
    Vector<Vector<int> >eyeScoresBlack;
    Vector<Vector<int> >eyeScoresWhite;
    LocalPatternsDatabase *localDatabase; int localDatabaseMode;
    double addKomiMax;
    double addKomiMin;
    double addKomiAlpha;
    double komiModifier;
    int autoKomiMode;
    double winLimitIncreaseKomi;
    double winLimitDecreaseKomi;
    int autoKomiMin;
    int autoKomiMax;
    int iterationsBeforeModifyKomi;
    int iterationsModifyKomi;
    double autoKomiModifier;

    double getModifiedKomi() const {
      return komi+komiModifier+autoKomiModifier;
    }
    void updateAutoKomi(const Goban &goban, double score);
    void initiateKomis(const Goban &goban);

    int startSimulationMode;
    int startSimulationNbSimulations;
    int startSimulationNbNodes;

    Location startSimulation(Goban &currentGoban, GobanNode *&currentNode, int &counter);

    int initiateStaticAnalysisOfBoardMode;
    void initiateStaticAnalysisOfBoard(const Goban &goban) const;

    int typeOfComputation;

    bool stopThinking(const CPUTimeChronometer & cpuTimer, double simulationTime, int iteration, int nbTotalSimulations) const;
    void registerStopThinking(const CPUTimeChronometer & cpuTimer, double simulationTime, int iteration, int nbTotalSimulations);

    double totalThinkingTime;
    double totalWantedThinkingTime;
    int nbWantedSimulations;
    int nbSimulationUsed;
    int nbThoughtMoves;
    Vector<double> scoreDistributionSum;
    double scoreDistributionNb;

    
    mutable FastSmallVector<GobanNode *>protectedNodes;
    void protectedNode(GobanNode *node) const;
    void unProtectedNode(GobanNode *node) const;

    static Vector<Goban *> allBoards;
    static Goban &getGobanCopy(const Goban &goban);
    static int reAllowMovesOutsideBitmapAtRoot;
    int nbSimulationsAtRoot;

    void reInitiateGobanBitmaps(const Goban &goban);

    
    void selfTrainOneGameAsynch(int iteration, const Goban &goban, int multiThreadsIdentifier, GobanNode *root);
  

    //     void selfTrainAsynchMaster(const Goban & goban, const int number, const double time, int multiThreadsIdentifier, int mode);
    bool selfTrainOneGameAsynchCore( MoveSelectorGlobal *simulationMoveSelectorTmp, const Goban & goban, /*Goban &currentGoban, */int multiThreadsIdentifier, int mTI, GobanNode * root, int &pass, double &score, GobanNode** lastNode,double&weightSimu,Vector<int>* guessLocation,Vector<int>* guessColor,Vector<double>* guessWeight);
    void selfTrainOneGameAsynchInitiate(const Goban &goban, int multiThreadsIdentifier, GobanNode *root);
    bool selfTrainOneGameAsynchInTree( GobanNode *node, /*Goban &currentGoban, */Location location, int multiThreadsIdentifier, int childIndex, GobanNode *&childPlayed, int locationMadeACapture);

#ifdef GOLDENEYE
    void initiatePlayerGobanNodeAsynch( MoveSelectorGlobal *simulationMoveSelectorTmp, UCTUpdatorGlobal *treeUpdatorTmp, const Goban & gobanAtRoot, FastBoard &board, GobanNode * node, const NodeSequence & currentSequence) const;
#else
    void initiatePlayerGobanNodeAsynch( MoveSelectorGlobal *simulationMoveSelectorTmp, UCTUpdatorGlobal *treeUpdatorTmp, const Goban & gobanAtRoot, const FastBoard &board, GobanNode * node, const NodeSequence & currentSequence) const;
#endif
    
#ifdef GOLDENEYE
    void initiatePlayerGobanNodeUrgencyAsynch(MoveSelectorGlobal *simulationMoveSelectorTmp, const Goban &gobanAtRoot, FastBoard &board, GobanNode *node, const NodeSequence & currentSequence) const;
#else
    void initiatePlayerGobanNodeUrgencyAsynch(MoveSelectorGlobal *simulationMoveSelectorTmp, const Goban &gobanAtRoot, const FastBoard &board, GobanNode *node, const NodeSequence & currentSequence) const;
#endif





    double selfTrainFastToEndAndGetOneScoreNew3( int multiThreadsIdentifier, NodeSequence & currentSequence, const Vector < Location > &currentSequenceMove, int choiceB,int choiceW );

    int getIndexSlave(int multiThreadsIdentifier);
    bool addJobToStack(int iteration, const NodeSequence &currentSequence, const Vector<Location> &currentSequencel);
    void removeIndexSlave(int index, int multiThreadsIdentifier);
    void clearStacks();

    void logNodeInfosForStats(const NodeSequence &currentSequence, const Goban &goban);

    void tryUseSequencesInTree(int multiThreadsIdentifier, NodeSequence & currentSequence, const Vector< Location > & currentSequenceMove);
    void registerSequencesInTree(int multiThreadsIdentifier, NodeSequence & currentSequence, const Vector< Location > & currentSequenceMove);
    void handleSequencesInTree(int multiThreadsIdentifier, NodeSequence & currentSequence, const Vector< Location > & currentSequenceMove);

    bool isLoopInCurrentSequence(const NodeSequence &currentSequence, GobanNode *node) const;

    Location getOneMoveByFuseki(const Goban&) const;
    Location getOneMoveByFuseki19(const Goban&) const;


    Vector<GobanNode *> interestingNodes;
    Goban currentRootGoban;

    static int useNewTrainOneGame;
    static int commonStack;
    static int moveSelectionByUpdate;
    static int stackSizePerThread;

    static int useSequencesInTree;
    static int usesSuperKoInTree;

    static int nbMovesRandomAtOpening;
    double sizePerNode;
    int gobanSizeForSizePerNodeRecord;
    int collectorLimitTreeSizeAutomatic;
    void initializeThreadVariables();

    /** Some debugging variables */
    static int innerMCGoPlayerStaticV1;
    /** Some debugging variables */
    static int innerMCGoPlayerStaticV2;

    int checkSuperKoByLoop;

    double lastNbSimulationsUsed;

    //for fuseki
    static std::string pathsFuseki;
    static std::string saveTreesFuseki;
     static std::string loadOB;
       static std::string saveOB;
         static std::string pathsOB;  
  };


}

#endif

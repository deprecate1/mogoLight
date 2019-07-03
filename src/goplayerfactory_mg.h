//
// C++ Interface: goplayerfactory_mg
//
// Description:
//
//
// Author: Yizao Wang <yizao.wang@polytechnique.edu> and Sylvain Gelly <gelly@lri.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MOGOGOPLAYERFACTORY_MG_H
#define MOGOGOPLAYERFACTORY_MG_H

#include"typedefs_mg.h"
class RlMogoWrapper;

namespace MoGo {
  class GoPlayer;
  class MoveSelector;
  class TreeUpdator;
  class ScoreEvaluator;
  class QScoreEvaluator;
  class MoGoServer;
  class ZonesScoreEvaluator;
  class InnerMCGoPlayer;
  class ZoneMCGoPlayer;
  class LocalPatternsDatabase;
  /**
  Mother class of factories which can produce go players. This is usefull for testing differents players and so on. Ask Sylvain for details :-)
   
  @author Sylvain Gelly, Yizao Wang
  */
  class GoPlayerFactory {
  public:
    GoPlayerFactory();

    virtual ~GoPlayerFactory();
    virtual GoPlayer *createPlayer(int taille=9) const=0;
    /** Clone itself (return a copy of itself) */
    virtual GoPlayerFactory *clone() const=0;
  };
 
  class RandomNearPlayerFactory : public GoPlayerFactory {
  public:
    /** The evaluator is owned by the caller. The given evaluator must live until the factory AND ALL its copies are destroyed */
    RandomNearPlayerFactory(int thresholdRandom, int patternSize, bool stableRegion, bool useFirstMove, bool chooseNearestFromCenter, QScoreEvaluator *evaluator=0);
    virtual GoPlayer *createPlayer(int taille=9) const;
    /** Clone itself (return a copy of itself) */
    virtual RandomNearPlayerFactory *clone() const { return new RandomNearPlayerFactory(*this);}
  private:
    int thresholdRandom;
    int patternSize;
    bool stableRegion;
    bool useFirstMove;
    bool chooseNearestFromCenter;
    QScoreEvaluator *evaluator;
  };
  class GnuGoPlayerFactory : public GoPlayerFactory {
  public:
    GnuGoPlayerFactory(bool fastPlay=false) {
      this->fastPlay = fastPlay;
      this->isStochastic=0;
      this->level=8;
      if (fastPlay)
        this->level=0;
      japanese=false;
    }
    virtual GoPlayer *createPlayer(int taille=9) const;
    /** Clone itself (return a copy of itself) */
  virtual GnuGoPlayerFactory *clone() const { return new GnuGoPlayerFactory(*this);}
    void setStochastic(int isStochastic) { this->isStochastic=isStochastic; }
    void setLevel(int level) { this->level=level; }
    void setJapanese(bool jap) {
      japanese=jap;
    }

  protected:
    bool fastPlay;
    int isStochastic;
    int level;
    bool japanese;
  };
  class GTPMoGoPlayerFactory : public GoPlayerFactory {
  public:
    GTPMoGoPlayerFactory(const std::string &options) {
      this->options=options;
    }
    virtual GoPlayer *createPlayer(int taille=9) const;
    /** Clone itself (return a copy of itself) */
    virtual GTPMoGoPlayerFactory *clone() const { return new GTPMoGoPlayerFactory(*this);}
  protected:
    std::string options;
  };
  class InnerMCGoPlayerFactory : public GoPlayerFactory {
  public:
    /** The given pointers are assumed to be owned by the caller */
    InnerMCGoPlayerFactory(const MoveSelector *simulationSelector, const MoveSelector *playSelector, const TreeUpdator *treeUpdator);
    /** Copy constructor */
    InnerMCGoPlayerFactory(const InnerMCGoPlayerFactory &player);
    virtual ~InnerMCGoPlayerFactory();
    virtual GoPlayer *createPlayer(int taille=9) const;
    /** Clone itself (return a copy of itself) */
    virtual InnerMCGoPlayerFactory *clone() const { return new InnerMCGoPlayerFactory(*this);}
    void setPlayerForFastFinishingGameFactory(const GoPlayerFactory *playerForFastFinishingGameFactory);
    /** The positionEvaluator is not deleted on this object. The given evaluator must live until the factory AND ALL its copies are destroyed */
    void setEvaluatorForFastFinishingGame(ScoreEvaluator *positionEvaluator);
    void setCorrelationMode(bool correlationMode);
    void setNbSimulationsForEachNode(int nbSimulationsForEachNode);
    void setRandomModes(int randomMode, int randomMode2);
    void setUseOpeningDatabase(bool useOpeningDatabase);
    void setNoiseLevel(double noiseLevel);
    void setNbTotalSimulations(int nbTotalSimulations);
    void setOpeningMode(int openingMode = STUPID_CENTER_OPENING_MODE);
    void setBlockMode(int mode);
    void setKeepTreeIfPossible(bool keepTreeIfPossible);
    void setGobanSize(int gobanSize = 9);
    void setImitateMode(int mode, int number);
    void setKomiVariation(double addKomiMax, double addKomiMin, double addKomiAlpha) {
      this->addKomiMax=addKomiMax;this->addKomiMin=addKomiMin;this->addKomiAlpha=addKomiAlpha;
    }
    void setAutoKomi(int autoKomiMode, double winLimitIncreaseKomi, double winLimitDecreaseKomi, int autoKomiMin, int autoKomiMax, int iterationsBeforeModifyKomi) {
      this->autoKomiMode=autoKomiMode;
      this->winLimitIncreaseKomi=winLimitIncreaseKomi;
      this->winLimitDecreaseKomi=winLimitDecreaseKomi;
      this->autoKomiMin=autoKomiMin;
      this->autoKomiMax=autoKomiMax;
      this->iterationsBeforeModifyKomi=iterationsBeforeModifyKomi;
    }
    /** Set the server for the player. The server must live until the end of the factory.
       The server must be deleted by the caller 
    */
    void setServer(MoGoServer *server);
    void setOwnSimulationsModifiers(int addingOwnSimulations, int multiplyingOwnSimulations, int useSimpleServerVersion);
    void setLocalDatabase(LocalPatternsDatabase *localDatabase, int localDatabaseMode) {
      this->localDatabase=localDatabase; this->localDatabaseMode=localDatabaseMode;
    }
    void setStartSimulationMode(int startSimulationMode, int startSimulationNbSimulations, int startSimulationNbNodes) {
      this->startSimulationMode=startSimulationMode;
      this->startSimulationNbSimulations=startSimulationNbSimulations;
      this->startSimulationNbNodes=startSimulationNbNodes;
    }
    void setInitiateStaticAnalysisOfBoardMode(int initiateStaticAnalysisOfBoardMode) {
      this->initiateStaticAnalysisOfBoardMode=initiateStaticAnalysisOfBoardMode;
    }
  protected:
    MoveSelector *playSelector;
    MoveSelector *simulationSelector;
    TreeUpdator *treeUpdator;
    MoGoServer *server;
    GoPlayerFactory *playerForFastFinishingGameFactory;
    ScoreEvaluator *positionEvaluator;
    bool correlationMode;
    int nbSimulationsForEachNode;
    int randomMode;
    int randomMode2;
    bool useOpeningDatabase;
    double noiseLevel;
    int nbTotalSimulations;
    int openingMode;
    int blockMode;
    bool keepTreeIfPossible;
    int gobanSize;
    int imitateMode;
    int imitateMoveNumber;
    int addingOwnSimulations; int multiplyingOwnSimulations; int useSimpleServerVersion;
    LocalPatternsDatabase *localDatabase; int localDatabaseMode;
    double addKomiMax;
    double addKomiMin;
    double addKomiAlpha;
    int autoKomiMode;
    double winLimitIncreaseKomi;
    double winLimitDecreaseKomi;
    int autoKomiMin;
    int autoKomiMax;
    int iterationsBeforeModifyKomi;
    int startSimulationMode;
    int startSimulationNbSimulations;
    int startSimulationNbNodes;
    int initiateStaticAnalysisOfBoardMode;
  };
  class ZoneMCGoPlayerFactory : public GoPlayerFactory {
  public:
    /** The player and the evaluator are owned by the caller and must live until the factory AND ALL its copies are destroyed */
    ZoneMCGoPlayerFactory(InnerMCGoPlayer *player, ZonesScoreEvaluator *zoneEvaluator);
    virtual ~ZoneMCGoPlayerFactory() {}
    virtual GoPlayer *createPlayer(int taille=9) const;
    /** Clone itself (return a copy of itself) */
    virtual ZoneMCGoPlayerFactory *clone() const { return new ZoneMCGoPlayerFactory(*this);}
    void setChooseZoneMode(int chooseZoneMode);
  private:
    InnerMCGoPlayer *player;
    ZonesScoreEvaluator *zoneEvaluator;
    int chooseZoneMode;
  };

  class MinimaxPlayerFactory : public GoPlayerFactory {
  public:
    /** The evaluator is owned by the caller. The given evaluator must live until the factory AND ALL its copies are destroyed */
    MinimaxPlayerFactory(int depth,QScoreEvaluator *evaluator=0,bool useGnugo=true);
    ~MinimaxPlayerFactory();
    virtual GoPlayer *createPlayer(int taille=9) const;
    /** Clone itself (return a copy of itself) */
    virtual MinimaxPlayerFactory *clone() const { return new MinimaxPlayerFactory(*this);}
    void setDeleteEvaluator(bool deleteEvaluator);
  protected:
    QScoreEvaluator *evaluator;
    bool deleteEvaluator;
    bool useGnugo;
    int depth;
  };
  class RLGoPlayerFactory : public GoPlayerFactory {
  public:
    /** The wrapper is owned by the caller. The given wrapper must live until the factory AND ALL its copies are destroyed */
    RLGoPlayerFactory(RlMogoWrapper *wrapper) {this->wrapper=wrapper;}
    ~RLGoPlayerFactory() {}
    virtual GoPlayer *createPlayer(int taille=9) const;
    /** Clone itself (return a copy of itself) */
    virtual RLGoPlayerFactory *clone() const { return new RLGoPlayerFactory(*this);}
  protected:
    RlMogoWrapper *wrapper;
  };

}

#endif

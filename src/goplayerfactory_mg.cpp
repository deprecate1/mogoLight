//
// C++ Implementation: goplayerfactory_mg
//
// Description:
//
//
// Author: Yizao Wang <yizao.wang@polytechnique.edu> and Sylvain Gelly <gelly@lri.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "goplayerfactory_mg.h"
#include "randomgoplayer_mg.h"
#include "innermcgoplayer_mg.h"
#include "treeupdator_mg.h"
#include "moveselector_mg.h"

//#include "qscoreevaluator_mg.h"

#include <string>

using namespace MoGo;
namespace MoGo {

// TEMPORARY COMMENT: THIS FUNCTION IS USED
  GoPlayerFactory::GoPlayerFactory() {}


  GoPlayerFactory::~GoPlayerFactory() {/**assert(0);**/}}




MoGo::InnerMCGoPlayerFactory::InnerMCGoPlayerFactory( const InnerMCGoPlayerFactory & player ) : GoPlayerFactory(player) {/**assert(0);**/
  this->playSelector = player.playSelector->clone();
  this->simulationSelector = player.simulationSelector->clone();
  this->treeUpdator = player.treeUpdator->clone();
  if (player.playerForFastFinishingGameFactory)
    this->playerForFastFinishingGameFactory=player.playerForFastFinishingGameFactory->clone();
  else
    this->playerForFastFinishingGameFactory=0;
  if (player.positionEvaluator)
    this->positionEvaluator = player.positionEvaluator;
  else
    this->positionEvaluator = 0;
  this->correlationMode = player.correlationMode;
  this->nbSimulationsForEachNode = player.nbSimulationsForEachNode;
  this->randomMode = player.randomMode; this->randomMode2=player.randomMode2;
  this->useOpeningDatabase = player.useOpeningDatabase;
  this->noiseLevel = player.noiseLevel;
  this->nbTotalSimulations = player.nbTotalSimulations;
  this->openingMode = player.openingMode;
  this->keepTreeIfPossible=player.keepTreeIfPossible;
  this->gobanSize = player.gobanSize;
  this->server = player.server;
  this->blockMode = player.blockMode;
  this->imitateMode = player.imitateMode;
  this->imitateMoveNumber = player.imitateMoveNumber;
  this->addingOwnSimulations=player.addingOwnSimulations;
  this->multiplyingOwnSimulations=player.multiplyingOwnSimulations;
  this->useSimpleServerVersion=player.useSimpleServerVersion;
  this->localDatabase=player.localDatabase;
  this->localDatabaseMode=player.localDatabaseMode;
  this->addKomiMax=player.addKomiMax;
  this->addKomiMin=player.addKomiMin;
  this->addKomiAlpha=player.addKomiAlpha;

  this->autoKomiMode=player.autoKomiMode;
  this->winLimitIncreaseKomi=player.winLimitIncreaseKomi;
  this->winLimitDecreaseKomi=player.winLimitDecreaseKomi;
  this->autoKomiMin=player.autoKomiMin;
  this->autoKomiMax=player.autoKomiMax;
  this->iterationsBeforeModifyKomi=player.iterationsBeforeModifyKomi;

  this->startSimulationMode=player.startSimulationMode;
  this->startSimulationNbSimulations=player.startSimulationNbSimulations;
  this->startSimulationNbNodes=player.startSimulationNbNodes;

  this->initiateStaticAnalysisOfBoardMode=player.initiateStaticAnalysisOfBoardMode;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
MoGo::InnerMCGoPlayerFactory::InnerMCGoPlayerFactory( const MoveSelector *simulationSelector, const MoveSelector *playSelector, const MoGo::TreeUpdator * treeUpdator ) {
  this->playSelector = playSelector->clone();
  this->simulationSelector = simulationSelector->clone();
  this->treeUpdator = treeUpdator->clone();
  this->playerForFastFinishingGameFactory=0;
  this->positionEvaluator=0;
  this->correlationMode=false;
  this->nbSimulationsForEachNode=1;
  this->randomMode=EATING_PREFERED_RANDOM_MODE_1;
  this->randomMode2=randomMode;
  this->useOpeningDatabase=true;
  this->noiseLevel=0.;
  this->nbTotalSimulations=-1;
  this->openingMode = 0;
  this->keepTreeIfPossible=false;
  this->gobanSize = 9;
  this->server=0;
  this->blockMode = 0;
  this->imitateMode = 0;
  this->imitateMoveNumber = 0;
  this->setOwnSimulationsModifiers(1000,0,true);
  this->localDatabase=0;
  this->localDatabaseMode=0;
  this->addKomiMax=0.;
  this->addKomiMin=0.;
  this->addKomiAlpha=1.;
  autoKomiMode=0; winLimitIncreaseKomi=0.; winLimitDecreaseKomi=0.; autoKomiMin=0; autoKomiMax=0; iterationsBeforeModifyKomi=100;

  startSimulationMode=0;
  startSimulationNbSimulations=0;
  startSimulationNbNodes=0;
  initiateStaticAnalysisOfBoardMode=0;
}
MoGo::InnerMCGoPlayerFactory::~InnerMCGoPlayerFactory( ) {/**assert(0);**/
  delete playSelector;
  delete simulationSelector;
  delete treeUpdator;
  if (playerForFastFinishingGameFactory)
    delete playerForFastFinishingGameFactory;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
MoGo::GoPlayer * MoGo::InnerMCGoPlayerFactory::createPlayer(int taille) const {
  InnerMCGoPlayer *tmp=new InnerMCGoPlayer(Goban(taille,taille), simulationSelector, playSelector, treeUpdator,0.);
  if (playerForFastFinishingGameFactory)
    tmp->setPlayerForFastFinishingGame(playerForFastFinishingGameFactory->createPlayer(taille));
  if (positionEvaluator)
    tmp->setPositionEvaluator(positionEvaluator, false);
  tmp->setCorrelationMode(correlationMode);
  tmp->setNbSimulationsForEachNode(nbSimulationsForEachNode);
  //   if (randomMode >= 0)
  tmp->setRandomModes(randomMode, randomMode2);
  if (useOpeningDatabase)
    tmp->loadOpeningDatabase();
  tmp->setNoiseLevel(noiseLevel);
  tmp->setNbTotalSimulations(nbTotalSimulations);
  tmp->setOpeningMode(openingMode);
  tmp->setKeepTreeIfPossible(keepTreeIfPossible);
  tmp->setGobanSize(gobanSize);
  tmp->setBlockMode(blockMode);
  tmp->setLocalDatabase(localDatabase, localDatabaseMode);
  tmp->setKomiVariation(addKomiMax, addKomiMin, addKomiAlpha);
  tmp->setAutoKomi(autoKomiMode, winLimitIncreaseKomi, winLimitDecreaseKomi, autoKomiMin, autoKomiMax, iterationsBeforeModifyKomi);
  tmp->setStartSimulationMode(startSimulationMode, startSimulationNbSimulations, startSimulationNbNodes);
  tmp->setInitiateStaticAnalysisOfBoardMode(initiateStaticAnalysisOfBoardMode);

  return tmp;
}

void MoGo::InnerMCGoPlayerFactory::setPlayerForFastFinishingGameFactory( const GoPlayerFactory *playerForFastFinishingGameFactory) {/**assert(0);**/
  if (playerForFastFinishingGameFactory)
    this->playerForFastFinishingGameFactory = playerForFastFinishingGameFactory->clone();
  else if (this->playerForFastFinishingGameFactory) {/**assert(0);**/
    delete this->playerForFastFinishingGameFactory;
    this->playerForFastFinishingGameFactory = 0;
  }
}
void MoGo::InnerMCGoPlayerFactory::setEvaluatorForFastFinishingGame( ScoreEvaluator * positionEvaluator ) {/**assert(0);**/
  if (positionEvaluator)
    this->positionEvaluator = positionEvaluator;
  else
    this->positionEvaluator = 0;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayerFactory::setCorrelationMode( bool correlationMode ) {
  this->correlationMode = correlationMode;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayerFactory::setNbSimulationsForEachNode( int nbSimulationsForEachNode ) {
  this->nbSimulationsForEachNode = nbSimulationsForEachNode;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayerFactory::setRandomModes( int randomMode, int randomMode2 ) {
  this->randomMode = randomMode; this->randomMode2=randomMode2;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayerFactory::setUseOpeningDatabase( bool useOpeningDatabase ) {
  this->useOpeningDatabase = useOpeningDatabase;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayerFactory::setNoiseLevel( double noiseLevel ) {
  this->noiseLevel=noiseLevel;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayerFactory::setNbTotalSimulations( int nbTotalSimulations ) {
  this->nbTotalSimulations = nbTotalSimulations;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayerFactory::setKeepTreeIfPossible( bool keepTreeIfPossible ) {
  this->keepTreeIfPossible=keepTreeIfPossible;
}
void MoGo::InnerMCGoPlayerFactory::setOpeningMode( int openingMode ) {/**assert(0);**/
  this->openingMode = openingMode;
}
void MoGo::InnerMCGoPlayerFactory::setGobanSize( int gobanSize ) {/**assert(0);**/
  this->gobanSize = gobanSize;
}
void MoGo::InnerMCGoPlayerFactory::setServer( MoGoServer * server ) {/**assert(0);**/
  this->server=server;
}


void MoGo::InnerMCGoPlayerFactory::setBlockMode( int mode ) {/**assert(0);**/
  blockMode = mode;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayerFactory::setOwnSimulationsModifiers( int addingOwnSimulations, int multiplyingOwnSimulations, int useSimpleServerVersion ) {
  if (useSimpleServerVersion>0) {
    addingOwnSimulations=0; multiplyingOwnSimulations=0;
  }
  this->addingOwnSimulations=addingOwnSimulations;this->multiplyingOwnSimulations=multiplyingOwnSimulations;this->useSimpleServerVersion=useSimpleServerVersion;
}











// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayerFactory::setImitateMode( int mode, int number ) {
  imitateMode = mode;
  imitateMoveNumber = number;
}











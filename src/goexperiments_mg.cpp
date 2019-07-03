//
// C++ Implementation: goexperiments_mg
//
// Description:
//
//
// Author: Yizao Wang <yizao.wang@polytechnique.edu> and Sylvain Gelly <gelly@lri.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//


#include <fstream>

#include "scoreevaluator_mg.h"

#include "goexperiments_mg.h"
#include "typedefs_mg.h"
#include "goban_mg.h"
#include "gogame_mg.h"
#include "innermcgoplayer_mg.h"
#include "gobannode_mg.h"


#include "randomgoplayer_mg.h"

#include <stdio.h>
#include "gogametree_mg.h"
#include "gtpengine_mg.h"
#include "gotools_mg.h"
#include <ext/hash_map>
#include "goplayerfactory_mg.h"
#include "basic_instinct_fast_mg.h"

#include "openingdatabase_mg.h"

#include <sys/timeb.h>
#include "yadtinterface_mg.h"
#include "localpatternsdatabase_mg.h"
#include "pbmb_io.h"
#include "fastboard_mg.h"
#include "smallsquarepatternsdb_mg.h"
#include "openingbook_mg.h"
#include "Forme_kppv.h"

#ifdef USE_SGL
#include <regressionscoreevaluator_mg.h>
#include <regressionqscoreevaluator_mg.h>
#endif

#include "pattern.h"


using namespace std;
using namespace MoGo;
namespace MoGo {

  GoExperiments::GoExperiments() {/**assert(0);**/}


  GoExperiments::~GoExperiments() {/**assert(0);**/}}

bool MoGo::GoExperiments::debugMode = false;





// TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::GoExperiments::findArgValue( int argc, char ** argv, const std::string & argument, double & v ) {
  bool hasFound=false;
  for (int i = 0 ; i < argc-1 ; i++)
    if (std::string(argv[i]) == ("--"+argument)) {
      double t;
      if (GoTools::toDouble(argv[i+1],t)) {
        v=t;
        hasFound=true;
      }
    }
  return hasFound;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::GoExperiments::findArgValue( int argc, char ** argv, const std::string & argument, std::string & s ) {
  bool hasFound=false;
  string lookFor="--";lookFor+=argument ;
  for (int i = 0 ; i < argc-1 ; i++) {
    if (std::string(argv[i]) == lookFor) {
      s=std::string(argv[i+1]);
      hasFound=true;
    }
  }

  return hasFound;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::GoExperiments::findArgValue( int argc, char ** argv, const std::string & argument, int & v ) {
  bool hasFound=false;
  for (int i = 0 ; i < argc-1 ; i++)
    if (std::string(argv[i]) == ("--"+argument)) {
      int t;
      if (GoTools::toInt(argv[i+1],t)) {
        v=t;
        hasFound=true;
      }
    }
  return hasFound;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
MoGo::GoPlayerFactory * MoGo::GoExperiments::createPlayerFactory( int argc, char ** argv, const std::string & name, std::string &infos ) {
  int randomMode=EATING_PREFERED_RANDOM_MODE_1;findArgValue(argc,argv,"randomMode",randomMode);
  int randomMode2=randomMode;findArgValue(argc,argv,"randomMode2",randomMode2); // the randomMode 2 is used when there is less than thresholdRandomSwitch possible moves at the leaf.

  int mixRandomModeRatio = 0;
  findArgValue(argc,argv,"mixRandomModeRatio",mixRandomModeRatio);
  if (mixRandomModeRatio>0) {/**assert(0);**/
    int mixRandomMode1=EATING_PREFERED_RANDOM_MODE_1;findArgValue(argc,argv,"mixRandomMode1",mixRandomMode1);
    int mixRandomMode2=1;findArgValue(argc,argv,"mixRandomMode2",mixRandomMode2); // the
    InnerMCGoPlayer::mixRandomMode1 = mixRandomMode1;
    InnerMCGoPlayer::mixRandomMode2 = mixRandomMode2;
    InnerMCGoPlayer::mixRandomModeRatio = mixRandomModeRatio;
  }
  int randomThreshold = 0;
  findArgValue(argc,argv,"randomThreshold",randomThreshold);
  int thresholdRandomSwitch = 30;
  findArgValue(argc,argv,"thresholdRandomSwitch",thresholdRandomSwitch); Goban::setThresholdRandomSwitch(thresholdRandomSwitch);
  int thresholdSelfAtari = 2;findArgValue(argc,argv,"thresholdSelfAtari",thresholdSelfAtari); Goban::setThresholdSelfAtari(thresholdSelfAtari);
  int thresholdEatingMove = 0;findArgValue(argc,argv,"thresholdEatingMove",thresholdEatingMove); Goban::__thresholdEatingMove=thresholdEatingMove;

  string databaseName="openingValues_merged";findArgValue(argc,argv,"databaseName",databaseName);
  GoPlayer::databaseName=databaseName;
  int minimumDatabaseUse=20;findArgValue(argc,argv,"minimumDatabaseUse",minimumDatabaseUse);
  Database3::minimumDatabaseUse=minimumDatabaseUse;

  if (name == "InnerMCGoPlayer") {
    double p1=1.; double p2=1.;
    findArgValue(argc,argv,"p1",p1);findArgValue(argc,argv,"p2",p2);

    double komi2 = 0.;
    findArgValue(argc,argv,"komi2",komi2);

    int nbSimulationsForEachNode=1;
    findArgValue(argc,argv,"nbSimulationsForEachNode",nbSimulationsForEachNode);
    int nbTotalSimulations=-1;findArgValue(argc,argv,"nbTotalSimulations",nbTotalSimulations);
    int useOpeningDatabase=0;findArgValue(argc, argv, "useOpeningDatabase", useOpeningDatabase);
    int useLocalDatabase=0;findArgValue(argc, argv, "useLocalDatabase", useLocalDatabase);
    string localDatabaseName="localDatabase19";findArgValue(argc,argv,"localDatabaseName",localDatabaseName);
    LocalPatternsDatabase *localDatabase=0;
    if (useLocalDatabase)
      localDatabase=LocalPatternsDatabase::loadFromFile(localDatabaseName.c_str());



    int numUpdateFormulaeMoveSelector=4;findArgValue(argc,argv,"numUpdateFormulaeMoveSelector",numUpdateFormulaeMoveSelector);

    double firstPlayUrgency = 10000;
    findArgValue(argc,argv,"firstPlayUrgency",firstPlayUrgency);

    int openingMode = 0;
    findArgValue(argc,argv,"openingMode",openingMode);

    int openingMoveNumber = 10;
    findArgValue(argc,argv,"openingMoveNumber",openingMoveNumber);


    int distance1 = 2, distance2 = 1;
    findArgValue(argc,argv,"distance1",distance1);findArgValue(argc,argv,"distance2",distance2);
    int grDistance1 = 2, grDistance2 = 1;
    findArgValue(argc,argv,"grDistance1",grDistance1);findArgValue(argc,argv,"grDistance2",grDistance2);
    CFGGroup::setGroupDistance(grDistance1,grDistance2);

    int pondering=0;findArgValue(argc,argv,"pondering",pondering);
    int keepTreeIfPossible=pondering;findArgValue(argc,argv,"keepTreeIfPossible",keepTreeIfPossible);

    int urgencySelectorMode=0;findArgValue(argc,argv,"urgencySelectorMode",urgencySelectorMode);
    int nearMode=-1;findArgValue(argc,argv,"nearMode",nearMode); int sizeNear=0; findArgValue(argc,argv,"sizeNear",sizeNear);
    int nearMode2=-1;findArgValue(argc,argv,"nearMode2",nearMode2);
    double alphaSqrt=0.5;findArgValue(argc,argv,"alphaSqrt",alphaSqrt);
    int useServer=0;findArgValue(argc,argv,"useServer",useServer);
    int nbChildsAlreadyDone=0;findArgValue(argc,argv,"nbChildsAlreadyDone",nbChildsAlreadyDone);
    int bufferSizeForEachMachine=2;findArgValue(argc,argv,"bufferSizeForEachMachine",bufferSizeForEachMachine);
    int addingOwnSimulations=1000;findArgValue(argc,argv,"addingOwnSimulations",addingOwnSimulations);
    int multiplyingOwnSimulations=0;findArgValue(argc,argv,"multiplyingOwnSimulations",multiplyingOwnSimulations);
    int useSimpleServerVersion=0;findArgValue(argc,argv,"useSimpleServerVersion",useSimpleServerVersion);
    int nbSimulationsByNodeServer=1;findArgValue(argc,argv,"nbSimulationsByNodeServer",nbSimulationsByNodeServer);
    int typeOfComputation=2;findArgValue(argc,argv,"typeOfComputation",typeOfComputation);

    int firstPlayUrgencyByGrandFather=-1;findArgValue(argc,argv,"firstPlayUrgencyByGrandFather",firstPlayUrgencyByGrandFather);
    int earlyCut=0;findArgValue(argc, argv, "earlyCut", earlyCut); MoGo::InnerMCGoPlayer::earlyCut=earlyCut;
    double earlyCutRatioFirstMove=0.3;findArgValue(argc, argv, "earlyCutRatioFirstMove", earlyCutRatioFirstMove); MoGo::InnerMCGoPlayer::earlyCutRatioFirstMove=earlyCutRatioFirstMove;
    double earlyCutRatioVSSecondMove=2.;findArgValue(argc, argv, "earlyCutRatioVSSecondMove", earlyCutRatioVSSecondMove); MoGo::InnerMCGoPlayer::earlyCutRatioVSSecondMove=earlyCutRatioVSSecondMove;
    int selectorMode=0;findArgValue(argc,argv,"selectorMode",selectorMode);

    double passMoveFirstPlayUrgency=-1.;findArgValue(argc,argv,"passMoveFirstPlayUrgency",passMoveFirstPlayUrgency);
   
    ///////////////////A EFFACER (following --> made ?)

    double explorationParam=1.;findArgValue(argc,argv,"explorationParam",explorationParam);
    double explorationParam2=0.;findArgValue(argc,argv,"explorationParam2",explorationParam2);
    double coefParam=1.;findArgValue(argc,argv,"coefParam",coefParam);
    double coefPow=0.5;findArgValue(argc,argv,"coefPow",coefPow);
    int stopUsingGlobal=0;findArgValue(argc,argv,"stopUsingGlobal",stopUsingGlobal);
    int initializeByGrandFather=0;findArgValue(argc,argv,"initializeByGrandFather",initializeByGrandFather);
    int treeDepthAllAtFirstLimit=3;findArgValue(argc,argv,"treeDepthAllAtFirstLimit",treeDepthAllAtFirstLimit);
    int simulationDepthAllAtFirstLimit=1000;findArgValue(argc,argv,"simulationDepthAllAtFirstLimit",simulationDepthAllAtFirstLimit);
    int keepDataForCorrelations=0;findArgValue(argc,argv,"keepDataForCorrelations",keepDataForCorrelations);
    int globalPlayerUseMemoryEconomy=0;findArgValue(argc,argv,"globalPlayerUseMemoryEconomy",globalPlayerUseMemoryEconomy);
    int useEndTerritories=0;findArgValue(argc,argv,"useEndTerritories",useEndTerritories);
    double territoriesLimit=0.2;findArgValue(argc,argv,"territoriesLimit",territoriesLimit);
    int updateFormula=0;findArgValue(argc,argv,"updateFormula",updateFormula);
    int useRLInitializor=0;findArgValue(argc, argv, "useRLInitializor", useRLInitializor); int equivalentSimulationsForInitialization=0;findArgValue(argc, argv, "equivalentSimulationsForInitialization", equivalentSimulationsForInitialization);
    double coefForTerritoryScore=0.;findArgValue(argc, argv, "coefForTerritoryScore", coefForTerritoryScore);
    double betaForTerritoryScore=1.;findArgValue(argc, argv, "betaForTerritoryScore", betaForTerritoryScore);
    double coefTwice=0.;findArgValue(argc, argv, "coefTwice", coefTwice);
    int keepNodesOrdered=0;findArgValue(argc, argv, "keepNodesOrdered", keepNodesOrdered);
    double lambdaForAMAF=1.;findArgValue(argc, argv, "lambdaForAMAF", lambdaForAMAF);
    double lambdaForAMAF2=1.;findArgValue(argc, argv, "lambdaForAMAF2", lambdaForAMAF2);
    int useEyes=0;findArgValue(argc, argv, "useEyes", useEyes);
    int updatorMode=1;findArgValue(argc, argv, "updatorMode", updatorMode);
    double discountBeta=1.;findArgValue(argc, argv, "discountBeta", discountBeta);
    int keepSequences=0;findArgValue(argc, argv, "keepSequences", keepSequences);
    int avoidAMAFPreventInTree=0;findArgValue(argc, argv, "avoidAMAFPreventInTree", avoidAMAFPreventInTree);
    double powUpdate=0.; findArgValue(argc, argv, "powUpdate", powUpdate);
    double alphaUpdate=1.;findArgValue(argc, argv, "alphaUpdate", alphaUpdate);
    int useNewTrainOneGame=0;findArgValue(argc,argv,"useNewTrainOneGame",useNewTrainOneGame);
    UCTUpdatorGlobal moGoGlobalUpdator(useNewTrainOneGame, powUpdate, alphaUpdate, avoidAMAFPreventInTree, keepSequences, discountBeta, useEyes, lambdaForAMAF2, lambdaForAMAF, keepNodesOrdered, coefTwice, betaForTerritoryScore, coefForTerritoryScore,
                                       equivalentSimulationsForInitialization,
                                       updateFormula,useEndTerritories,(bool)initializeByGrandFather,(bool)globalPlayerUseMemoryEconomy, (bool)keepDataForCorrelations,
                                       treeDepthAllAtFirstLimit,simulationDepthAllAtFirstLimit, updatorMode);
    int maxDepth=1;findArgValue(argc,argv,"maxDepth",maxDepth);
    int localFirst=0;findArgValue(argc,argv,"localFirst",localFirst);
    int initiateGobanNodeUrgencyWhenAsked=1;findArgValue(argc,argv,"initiateGobanNodeUrgencyWhenAsked",initiateGobanNodeUrgencyWhenAsked);
    int nbSimulationsRandomInTree=-1;findArgValue(argc,argv,"nbSimulationsRandomInTree",nbSimulationsRandomInTree);
    int randomModeInTree=0;findArgValue(argc,argv,"randomModeInTree",randomModeInTree);
    int hardLocalFirst=0;findArgValue(argc,argv,"hardLocalFirst",hardLocalFirst);
    double coefInterestingMoves=0.;findArgValue(argc,argv,"coefInterestingMoves",coefInterestingMoves);
    double minInterestingMoves=0.; findArgValue(argc,argv,"minInterestingMoves",minInterestingMoves);
    double powInterestingMoves=0.5;findArgValue(argc,argv,"powInterestingMoves",powInterestingMoves);
    int interestingMoveMode=0;findArgValue(argc,argv,"interestingMoveMode",interestingMoveMode);
    double powForLogReplacement=0.5;findArgValue(argc,argv,"powForLogReplacement",powForLogReplacement);
    double coefAmplitude=1.;findArgValue(argc,argv,"coefAmplitude",coefAmplitude);
    int passOnlyIfNoChoice=0;findArgValue(argc,argv,"passOnlyIfNoChoice",passOnlyIfNoChoice);
    double limitInterestingMoveCoef=0.;findArgValue(argc,argv,"limitInterestingMoveCoef",limitInterestingMoveCoef);
    double bias=0.;findArgValue(argc,argv,"bias",bias);
    double nbEqEstimator2=0.;findArgValue(argc,argv,"nbEqEstimator2",nbEqEstimator2);
    double bias2=0.;findArgValue(argc,argv,"bias2",bias2);
    int historySizeForPrecomputed=10;findArgValue(argc,argv,"historySizeForPrecomputed",historySizeForPrecomputed);
    int optimizeSelectMove=-1;findArgValue(argc,argv,"optimizeSelectMove",optimizeSelectMove);
    int keepBonusNbSimulations=10;findArgValue(argc,argv,"keepBonusNbSimulations",keepBonusNbSimulations);
    int movingConstantMode=0;findArgValue(argc,argv,"movingConstantMode",movingConstantMode);
    double movingConstantSpeed=4.;findArgValue(argc,argv,"movingConstantSpeed",movingConstantSpeed);
    double powTerritoriesBonus=1.;findArgValue(argc,argv,"powTerritoriesBonus",powTerritoriesBonus);
    double biasInAMAFCoef=0.;findArgValue(argc,argv,"biasInAMAFCoef",biasInAMAFCoef);
    double coefBonusAMAF=0.;findArgValue(argc,argv,"coefBonusAMAF",coefBonusAMAF);
    int exitTreeNbSimulations=0;findArgValue(argc,argv,"exitTreeNbSimulations",exitTreeNbSimulations);

#ifdef AUTOTUNE
    int fn=0;
    findArgValue(argc,argv,"configNumber",fn);
    fprintf(stderr,"fileNumber=%d\n",fn);fflush(stdout);
    BasicInstinctFast::fileNumber=fn;
    double Wamaf=1.0;//exp(BasicInstinctFast::configValue("Wamaf")/4.);
    //InnerMCGoPlayer::exitTreeNbSimulations=exitTreeNbSimulations+int(BasicInstinctFast::configValue("coefAttente"));

    //  if(FBS_handler::board_size==9)
    //       {
    
    // 	MoveSelectorGlobal::pow_stones_quantity *= exp(BasicInstinctFast::configValue("pow_stones_quantity")/4.0);
    // 	MoveSelectorGlobal::weight_stones_quantity *= exp(BasicInstinctFast::configValue("weight_stones_quantity")/4.0);
    //       }
    
    MoveSelectorGlobal::coef_expertise *= exp(BasicInstinctFast::configValue("coef_expertise")/4.0);
    

#else
    //InnerMCGoPlayer::exitTreeNbSimulations=exitTreeNbSimulations;//+int(BasicInstinctFast::configValue("coefAttente"));
    double Wamaf=1.0;
#endif

#ifdef GUILLAUME
//cerr <<" # # # # # # " << FBS_handler::board_size << endl;

if (FBS_handler::board_size==19)
{
    bias*=exp(-3.63/4.);
    Wamaf*=exp((-0.59-0.2)/4);
}

#endif

/*
#ifdef AUTOTUNE
    int fn=0;
    findArgValue(argc,argv,"configNumber",fn);
    fprintf(stderr,"fileNumber=%d\n",fn);fflush(stdout);
    BasicInstinctFast::fileNumber=fn;
    //InnerMCGoPlayer::exitTreeNbSimulations=exitTreeNbSimulations+int(BasicInstinctFast::configValue("coefAttente"));
#endif


#ifdef AUTOTUNE
	Wamaf*=exp(BasicInstinctFast::configValue("Wamaf")/4.0);
#endif
*/

//    printf("%d \n",FBS_handler::board_size);assert(0);
    if (FBS_handler::board_size==19)
    InnerMCGoPlayer::exitTreeNbSimulations=exitTreeNbSimulations+int(5);//BasicInstinctFast::configValue("coefAttente"));
    else
    InnerMCGoPlayer::exitTreeNbSimulations=exitTreeNbSimulations;//BasicInstinctFast::configValue("coefAttente"));


    ScoreEvaluator *rlEvaluator=0;
    int RLlearnMode=0; findArgValue(argc,argv,"RLlearnMode",RLlearnMode);
    int RLuseMode=0;findArgValue(argc,argv,"RLuseMode",RLuseMode);
    double valueForSaveMove=1.;findArgValue(argc,argv,"valueForSaveMove",valueForSaveMove);
    double equivalentSimulationsForInitializationForSaveMove=equivalentSimulationsForInitialization;findArgValue(argc,argv,"equivalentSimulationsForInitializationForSaveMove",equivalentSimulationsForInitializationForSaveMove);
    double decDistanceSH=4.;findArgValue(argc,argv,"decDistanceSH",decDistanceSH);
    double coefDistanceSH=0.2;findArgValue(argc,argv,"coefDistanceSH",coefDistanceSH);
    double powDistanceSH=1.;findArgValue(argc,argv,"powDistanceSH",powDistanceSH);
    
#ifdef GUILLAUME
    if (FBS_handler::board_size==19)
      {
	equivalentSimulationsForInitializationForSaveMove*=exp((-0.35)/4.0);
	decDistanceSH*=exp((0.27)/4.0);
	coefDistanceSH*=exp((0.15)/4.0);
      }
#endif


/*
#ifdef AUTOTUNE
    equivalentSimulationsForInitializationForSaveMove*=exp(BasicInstinctFast::configValue("equivSimuTruc")/4.0);
    decDistanceSH*=exp(BasicInstinctFast::configValue("decDistance")/4.0);
    coefDistanceSH*=exp(BasicInstinctFast::configValue("coefDist")/4.0);
#endif
*/

    //DEBUG RLEVALUATOR
//     if (useRLInitializor==1) {
//       //        (new RlMogoWrapper("mogo-settings.txt"/*"rlgo/default-settings.txt"*/,rlGoParameters(argc, argv)))->Test();
//       //        GoTools::print("test ok\n");

//       rlEvaluator=new RlScoreEvaluator(new RlMogoWrapper("rlgo-settings.txt"/*"rlgo/default-settings.txt"*/,rlGoParameters(argc, argv)), RLlearnMode, RLuseMode, false, coefAmplitude);
//     } else if (useRLInitializor>=2)
    rlEvaluator=new SimpleHeuristicScoreEvaluator(coefAmplitude,useRLInitializor, valueForSaveMove, equivalentSimulationsForInitializationForSaveMove, decDistanceSH, coefDistanceSH, powDistanceSH);
    //END DEBUG RLEVALUATOR


    double deltaPow=1.;findArgValue(argc,argv,"deltaPow",deltaPow);
    double delta=0.1;findArgValue(argc,argv,"delta",delta);
    int avoidDeltaForRoot=0;findArgValue(argc,argv,"avoidDeltaForRoot",avoidDeltaForRoot);
    int useBitmapNeighborhood=0;findArgValue(argc,argv,"useBitmapNeighborhood",useBitmapNeighborhood);
    int maxRandomOptimizeSelectMove=30;findArgValue(argc,argv,"maxRandomOptimizeSelectMove",maxRandomOptimizeSelectMove);
    int startCountDown=10;findArgValue(argc,argv,"startCountDown",startCountDown);
    int nBestSize=4;findArgValue(argc,argv,"nBestSize",nBestSize);
    double powStarCountDown=0.;findArgValue(argc,argv,"powStarCountDown",powStarCountDown);
    int useDistancesByBlock=0;findArgValue(argc,argv,"useDistancesByBlock",useDistancesByBlock);

    MoveSelectorGlobal MoGoGlobalSimulationSelector(useDistancesByBlock, powStarCountDown, nBestSize, startCountDown, maxRandomOptimizeSelectMove, useBitmapNeighborhood, useNewTrainOneGame, avoidDeltaForRoot, deltaPow, delta, nbEqEstimator2, bias2, coefBonusAMAF, biasInAMAFCoef, powTerritoriesBonus, movingConstantSpeed, movingConstantMode, keepBonusNbSimulations, historySizeForPrecomputed, optimizeSelectMove, bias, limitInterestingMoveCoef, passOnlyIfNoChoice, powForLogReplacement, interestingMoveMode, minInterestingMoves, powInterestingMoves, coefInterestingMoves, hardLocalFirst, randomModeInTree, nbSimulationsRandomInTree, equivalentSimulationsForInitialization, rlEvaluator,
        (bool)initiateGobanNodeUrgencyWhenAsked, localFirst, maxDepth, territoriesLimit,
        (bool)initializeByGrandFather, stopUsingGlobal,explorationParam,
        explorationParam2, coefParam, coefPow, numUpdateFormulaeMoveSelector,urgencySelectorMode,p2,Wamaf);

    /*
    //TO DELETE
    int blockMode = 0;
    findArgValue(argc,argv,"blockMode",blockMode);
    int metaBlockMode = 0;
    findArgValue(argc,argv,"metaBlockMode",metaBlockMode);
    */
    int freeLocationSize = 50;
    findArgValue(argc,argv,"freeLocationSize",freeLocationSize);
    
    /*
    MoGoGlobalSimulationSelector.setBlockMode(blockMode,metaBlockMode,distance1,distance2,freeLocationSize);
    */
    MoGoGlobalSimulationSelector.setOpeningMode(openingMode,openingMoveNumber);

    MoGo::UCTSelector1 MoGoPlaySelector(selectorMode);
    MoGo::UCTSelector1 MoGoPlaySelector2(1);
    int uctUpdateMode = 1;
    findArgValue(argc,argv,"uctUpdateMode",uctUpdateMode);
    MoGo::UCTUpdator MoGoUpdator(1+(nbSimulationsForEachNode>1));
    if (uctUpdateMode <= 0)
      MoGoUpdator.changeMode(uctUpdateMode);

  

 

    double alpha = 0.002;
    findArgValue(argc,argv,"alpha",alpha);

    //A EFFACER (Following)

   
    double beta = 1;
    findArgValue(argc,argv,"beta",beta);
    //A EFFACER (Following made ?)
  

    int mixThreshold = -1;
    findArgValue(argc,argv,"mixThreshold",mixThreshold);
    //A EFFACER (Following)
    MoGo::UCTSelector1 playSelectorMix;
  

    //A EFFACER (Following)

    double alpha2 = 0.25;
    findArgValue(argc,argv,"alpha2",alpha2);
    //A EFFACER (Following)
    MoGo::UCTSelector1 playSelectorPower;
    MoGo::UCTUpdator updatorPower(1.,1);


    int relativeDepthMax = 0;
    findArgValue(argc,argv,"relativeDepthMax",relativeDepthMax);
    //A EFFACER (Following made ?)
    MoGo::UCTSelector1 playSelectorDepth;
    MoGo::UCTUpdator updatorDepth(1.,1);

    MoGo::MoveSelector * simulationSelector = 0;
    MoGo::MoveSelector * playSelector = 0;
    MoGo::TreeUpdator * updator = 0;

    //A EFFACER (Following)
   
    MoGo::UCTSelector1 playSelectorOpening1;

    int meta = 0;
    findArgValue(argc,argv,"meta",meta);



    std::string playerName = "MoGo";
    findArgValue(argc,argv,"playerName",playerName);
    
    if (playerName=="global") {
      simulationSelector=&MoGoGlobalSimulationSelector;
      playSelector=&MoGoPlaySelector;
      updator=&moGoGlobalUpdator;
    } 

    
    int localDatabaseModeTree=0;findArgValue(argc, argv, "localDatabaseModeTree", localDatabaseModeTree);
    simulationSelector->setDatabase(localDatabase, localDatabaseModeTree);
    int tenukiMode=0;GoExperiments::findArgValue(argc,argv,"tenukiMode",tenukiMode);simulationSelector->setTenukiMode(tenukiMode);

    //GoTools::print("%s created.\n",playerName.c_str());
    
    infos="p1_"+GoTools::toString(p1,true)+"_p2_"+GoTools::toString(p2,true);
    MoGo::InnerMCGoPlayerFactory *tmp=new MoGo::InnerMCGoPlayerFactory(simulationSelector,playSelector,updator);

    int imitateMode = 0;
    int imitateMoveNumber = 40;
    findArgValue(argc,argv,"imitateMode",imitateMode);
    findArgValue(argc,argv,"imitateMoveNumber",imitateMoveNumber);
    tmp->setImitateMode(imitateMode,imitateMoveNumber);
    int imitateOnlyAsWhite=0; GoExperiments::findArgValue(argc,argv,"imitateOnlyAsWhite",imitateOnlyAsWhite); InnerMCGoPlayer::imitateOnlyAsWhite = imitateOnlyAsWhite; //GoTools::print("imitateOnlyAsWhite = %d\n",imitateOnlyAsWhite);

    tmp->setRandomModes(randomMode, randomMode2);
    tmp->setNbSimulationsForEachNode(nbSimulationsForEachNode);
    tmp->setKeepTreeIfPossible(keepTreeIfPossible);
    bool correlatedOn = false;
    std::string correlatedMode = "off";
    findArgValue(argc,argv,"correlatedMode", correlatedMode);
    if (correlatedMode == "on" ) correlatedOn = true;
    tmp->setCorrelationMode(correlatedOn);
    tmp->setUseOpeningDatabase(useOpeningDatabase);
    tmp->setNbTotalSimulations(nbTotalSimulations);
    double noiseLevel=0.;findArgValue(argc,argv,"noiseLevel",noiseLevel);
    tmp->setNoiseLevel(noiseLevel);
    int localDatabaseModePlayer=0;findArgValue(argc, argv, "localDatabaseModePlayer", localDatabaseModePlayer);
    tmp->setLocalDatabase(localDatabase, localDatabaseModePlayer);


   

  
    double addKomiMax=0.;findArgValue(argc, argv, "addKomiMax", addKomiMax);
    double addKomiMin=0.;findArgValue(argc, argv, "addKomiMin", addKomiMin);
    double addKomiAlpha=1.;findArgValue(argc, argv, "addKomiAlpha", addKomiAlpha);
    tmp->setKomiVariation(addKomiMax, addKomiMin, addKomiAlpha);

    int autoKomiMode=0;findArgValue(argc, argv, "autoKomiMode", autoKomiMode);
    double winLimitIncreaseKomi=0.7;findArgValue(argc, argv, "winLimitIncreaseKomi", winLimitIncreaseKomi);
    double winLimitDecreaseKomi=0.3;findArgValue(argc, argv, "winLimitDecreaseKomi", winLimitDecreaseKomi);
    int autoKomiMin=0;findArgValue(argc, argv, "autoKomiMin", autoKomiMin);
    int autoKomiMax=10;findArgValue(argc, argv, "autoKomiMax", autoKomiMax);
    int iterationsBeforeModifyKomi=100;findArgValue(argc, argv, "iterationsBeforeModifyKomi", iterationsBeforeModifyKomi);
    tmp->setAutoKomi(autoKomiMode, winLimitIncreaseKomi, winLimitDecreaseKomi, autoKomiMin, autoKomiMax, iterationsBeforeModifyKomi);

    int startSimulationMode=0;findArgValue(argc, argv, "startSimulationMode", startSimulationMode);
    int startSimulationNbSimulations=100;findArgValue(argc, argv, "startSimulationNbSimulations", startSimulationNbSimulations);
    int startSimulationNbNodes=10;findArgValue(argc, argv, "startSimulationNbNodes", startSimulationNbNodes);
    tmp->setStartSimulationMode(startSimulationMode, startSimulationNbSimulations, startSimulationNbNodes);
    int initiateStaticAnalysisOfBoardMode=0; findArgValue(argc, argv, "initiateStaticAnalysisOfBoardMode", initiateStaticAnalysisOfBoardMode);
    tmp->setInitiateStaticAnalysisOfBoardMode(initiateStaticAnalysisOfBoardMode);

    return tmp;
  }

  GoTools::print((std::string("can't find ")+name+"\n").c_str());
  return 0;
}



















// TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::GoExperiments::GTPEngineMode( int argc, char ** argv ) {
  GoTools::setOutputMode(STDERR_MODE);
  double komi = 7.5;
  double timeEachMove = 20;
  int allGameTime = 2000;

  findArgValue(argc,argv,"time",timeEachMove);
  findArgValue(argc,argv,"komi",komi);
  findArgValue(argc,argv,"allGameTime",allGameTime);
  int pondering=0;findArgValue(argc,argv,"pondering",pondering);
  int thinkOverOtherTime=pondering;findArgValue(argc,argv,"thinkOverOtherTime",thinkOverOtherTime);
  int nbThreads=1; GoExperiments::findArgValue(argc, argv, "nbThreads", nbThreads);

  // timeMultiplier ser à prendre en compte le fait que le chronometre retourne un temps cpu et donc un temps multiplié par le nombre de threads.
  double timeMultiplier=nbThreads;findArgValue(argc,argv,"timeMultiplier",timeMultiplier);
  // En MPI, on utilise une autre fonction chronometre et on n'a donc plus besoin de cette astuce.
#ifdef MPIMOGO
  timeMultiplier=1;
#endif

  

  double timeDivider=14.;findArgValue(argc,argv,"timeDivider",timeDivider);
  double timeShift=0.;findArgValue(argc,argv,"timeShift",timeShift);
  double forgetKomiValue=-100000.;findArgValue(argc,argv,"forgetKomiValue",forgetKomiValue);
  int playsAgainstHuman=0;findArgValue(argc,argv,"playsAgainstHuman",playsAgainstHuman);
  int defaultParameterSize=0;findArgValue(argc,argv,"defaultParameterSize",defaultParameterSize);
  int timeManagementMode=0;findArgValue(argc,argv,"timeManagementMode",timeManagementMode);
  double maxBeginning=0.; findArgValue(argc,argv,"maxBeginning",maxBeginning);
  if (maxBeginning<0.) maxBeginning=timeEachMove/4.;
  int numMoveBeginning=0; findArgValue(argc,argv,"numMoveBeginning",numMoveBeginning);

  std::string name="InnerMCGoPlayer"; findArgValue(argc, argv, "player", name);

  std::string infos;
//  cerr << "#### pos1: " << FBS_handler::board_size << endl;
  GoPlayerFactory *factory=createPlayerFactory(argc, argv, name, infos);
//  cerr << "#### pos1.5: " << FBS_handler::board_size << endl;
  //   GoTools::print("GTPEngineMode, player %s, infos: %s\n", name.c_str(), infos.c_str());
  GoPlayer *player=factory->createPlayer(FBS_handler::board_size);
  FBS_handler::changeSize(defaultParameterSize);
//  cerr << "#### pos2: " << FBS_handler::board_size << endl;
  player->setKomi(komi);

  MoGo::GTPEngine gtpEngine(player, playsAgainstHuman, timeMultiplier, timeDivider, timeShift, komi, forgetKomiValue, defaultParameterSize);
  //FBS TODOTODOTODO
  gtpEngine.setEachMoveTime(timeEachMove, timeManagementMode, maxBeginning, numMoveBeginning);
  gtpEngine.setAllGameTime(allGameTime);
  gtpEngine.setThinkWhileOtherThinks(thinkOverOtherTime);
  //gtpEngine.showCommandList();

//  cerr << "#### pos3: " << FBS_handler::board_size << endl;
  bool saveMode = false;
  String saveModeOn = "off";
  findArgValue(argc,argv,"saveMode",saveModeOn);
  if (saveModeOn=="on") saveMode = true;
  gtpEngine.setSaveMode(saveMode);

  gtpEngine.start();

  delete player;
  delete factory;
 // cerr << "#### pos4: " << FBS_handler::board_size << endl;
  return EXIT_SUCCESS;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::GoExperiments::initiateEnviroment( int argc, char ** argv ) {

  MoGo::GoTools::setOutputMode(STDERR_MODE);

  Goban::probabilitiesOfinterestingMovesStatic.resize(6,1.);
  Goban::probabilitiesOfinterestingMovesStatic[0]=0.2;
  Goban::probabilitiesOfinterestingMovesStatic[1]=0.2;
  Goban::probabilitiesOfinterestingMovesStatic[2]=0.8;
  Goban::probabilitiesOfinterestingMovesStatic[3]=0.8;
  Goban::probabilitiesOfinterestingMovesStatic[4]=0.8;
  Goban::probabilitiesOfinterestingMovesStatic[5]=0.2;
  int nbThreads=1; GoExperiments::findArgValue(argc, argv, "nbThreads", nbThreads); MoGo::InnerMCGoPlayer::nbThreads=nbThreads;
  //   GoTools::print("nbThreads = %d\n",nbThreads);
  int securityThreads=0; GoExperiments::findArgValue(argc, argv, "securityThreads", securityThreads); MoGo::InnerMCGoPlayer::securityThreads=securityThreads;
  //   GoTools::print("securityThreads = %d\n",securityThreads);
  int useColor=1;
  GoExperiments::findArgValue(argc, argv, "useColor", useColor);
  MoGo::GoTools::displayWithColor(useColor);
  //   GoTools::print("useColor = %d\n",useColor);

  int fastRand=0;
  GoExperiments::findArgValue(argc, argv, "fastRand", fastRand);
  MoGo::GoTools::mogoSRand(33); MoGo::GoTools::setFastRandMode(fastRand);
  //   GoTools::print("fastRand = %d\n",fastRand);

  int limitTreeSize = 400000;
  GoExperiments::findArgValue(argc,argv,"limitTreeSize",limitTreeSize);
  MoGo::InnerMCGoPlayer::setLimitTreeSize(limitTreeSize);
  int collectorLimitTreeSize = -1;
  GoExperiments::findArgValue(argc,argv,"collectorLimitTreeSize",collectorLimitTreeSize);
  MoGo::InnerMCGoPlayer::collectorLimitTreeSize=collectorLimitTreeSize;
  //   GoTools::print("limitTreeSize = %d, collectorLimitTreeSize %i\n",limitTreeSize,collectorLimitTreeSize);

  int cfg = 0;
  GoExperiments::findArgValue(argc,argv,"cfgMode",cfg);
  MoGo::Goban::setCFGGlobal(cfg);
  //   GoTools::print("cfgMode = %d\n",cfg);

  int blockMode = 0;
  int zoneMode = 0;
  GoExperiments::findArgValue(argc,argv,"blockMode",blockMode);
  GoExperiments::findArgValue(argc,argv,"zoneMode",zoneMode);
  if ((blockMode || zoneMode) && !cfg) {/**assert(0);**/
    GoTools::print("Warning: blockMode or zoneMode founded! cfgMode is set to 2!\n");
    MoGo::Goban::setCFGGlobal(2);
  }


  int firstMoveFixed = 0;
  GoExperiments::findArgValue(argc,argv,"firstMoveFixed",firstMoveFixed);
  MoGo::InnerMCGoPlayer::setFirstMoveFixed(firstMoveFixed);
  //   GoTools::print("firstMoveFixed = %d\n",firstMoveFixed);

  int dontDisplay = 0;
  GoExperiments::findArgValue(argc,argv,"dontDisplay",dontDisplay);
  MoGo::InnerMCGoPlayer::dontDisplay = (bool)dontDisplay;
  //   GoTools::print("dontDisplay = %d\n",dontDisplay);

  String srandByTime = "off";
  GoExperiments::findArgValue(argc,argv,"srandByTime",srandByTime);
  if (srandByTime == "on") {
    MoGo::GoTools::mogoSRand();
    //     MoGo::GoTools::mogoSRand(time(NULL));
  }
  struct timeb tp;
  ftime(&tp);
  //   GoTools::print("clock=%i/%i\n", tp.millitm, tp.time);
  //   GoTools::print("srandByTime = %s\n",srandByTime.c_str());

  int gobanSize = 9;
  findArgValue(argc,argv,"gobanSize",gobanSize);
  Goban goban(gobanSize,gobanSize);
  GobanLocation::resize(goban);
  GobanBitmap::initiate(goban);
  //   GoTools::print("gobanSize = %d\n",gobanSize);

  int uselessMode = 0;
  GoExperiments::findArgValue(argc,argv,"uselessMode",uselessMode);
  MoGo::Goban::setNotUselessMode(uselessMode);
  //   GoTools::print("uselessMode = %d\n",uselessMode);

  int use25BlocksForGP=1;GoExperiments::findArgValue(argc,argv,"use25BlocksForGP",use25BlocksForGP);MoGo::Goban::use25BlocksForGP=use25BlocksForGP;
  int useLibertiesForGP=1;GoExperiments::findArgValue(argc,argv,"useLibertiesForGP",useLibertiesForGP);MoGo::Goban::useLibertiesForGP=(bool)useLibertiesForGP;
  int useLastLastMove=0;GoExperiments::findArgValue(argc,argv,"useLastLastMove",useLastLastMove);MoGo::Goban::useLastLastMove=(bool)useLastLastMove;
  int earlyStop = 0;
  findArgValue(argc,argv,"earlyStop",earlyStop);
  int stopDifference = 30;
  findArgValue(argc,argv,"stopDifference",stopDifference);
  InnerMCGoPlayer::earlyStop = earlyStop;
  InnerMCGoPlayer::stopDifference = stopDifference;
  //   GoTools::print("earlyStop = %d\n",earlyStop);
  //   GoTools::print("stopDifference = %d\n",stopDifference);

  if (uselessMode == 2)
    GoTools::print("Warning: uselessMode 2 is only for some rare cases and with some problems for the moment.\n");

  int recheckEyeMode = 0;
  GoExperiments::findArgValue(argc,argv,"recheckEyeMode",recheckEyeMode);
  int updateEatingLocations=1;
  GoExperiments::findArgValue(argc,argv,"updateEatingLocations",updateEatingLocations);
  MoGo::Goban::recheckEyeModeStatic=recheckEyeMode;
  MoGo::Goban::updateEatingLocations=updateEatingLocations;
  //   GoTools::print("recheckEyeMode = %d, updateEatingLocations %i\n",recheckEyeMode, updateEatingLocations);

  int isSelfAtariMode = 0;
  GoExperiments::findArgValue(argc,argv,"isSelfAtariMode",isSelfAtariMode);
  LocationInformation::setIsSelfAtariMode(isSelfAtariMode);
  //   GoTools::print("isSelfAtariMode = %d\n",isSelfAtariMode);

  int allowDoublePassInTree=0;GoExperiments::findArgValue(argc,argv,"allowDoublePassInTree",allowDoublePassInTree);
  //   GoTools::print("allowDoublePassInTree = %d\n",allowDoublePassInTree);
  InnerMCGoPlayer::allowDoublePassInTree=allowDoublePassInTree;

  int showBigGobanStringNumber = 0;
  GoExperiments::findArgValue(argc,argv,"showBigGobanStringNumber",showBigGobanStringNumber);
  Goban::showBigGobanStringNumber = showBigGobanStringNumber;
  //   GoTools::print("showBigGobanStringNumber = %d\n",showBigGobanStringNumber);

  int shishoLikeMoveForced = 0;
  GoExperiments::findArgValue(argc,argv,"shishoLikeMoveForced",shishoLikeMoveForced);
  Goban::shishoLikeMoveForced = shishoLikeMoveForced;//bool
  //   GoTools::print("shishoLikeMoveForced = %d\n",shishoLikeMoveForced);

  int debugMode = 0;
  GoExperiments::findArgValue(argc,argv,"debugMode",debugMode);
  GoExperiments::debugMode = debugMode;
  //   GoTools::print("debugMode = %d\n",debugMode);

  int fillRangeFirstMode = 0;
  GoExperiments::findArgValue(argc,argv,"fillRangeFirstMode",fillRangeFirstMode);
  InnerMCGoPlayer::fillRangeFirstMode = fillRangeFirstMode;
  //   GoTools::print("fillRangeFirstMode = %d\n",fillRangeFirstMode);

  int accelerateMode = 0;
  GoExperiments::findArgValue(argc,argv,"accelerateMode",accelerateMode);
  InnerMCGoPlayer::accelerateMode = accelerateMode;
  //   GoTools::print("accelerateMode = %d\n",accelerateMode);

 

  int undoMode = 1;
  GoExperiments::findArgValue(argc,argv,"undoMode",undoMode);
  GTPEngine::undoMode = undoMode;
  //   GoTools::print("undoMode = %d\n",undoMode);

  int shishoCheckMode = 0; GoExperiments::findArgValue(argc,argv,"shishoCheckMode",shishoCheckMode);
  InnerMCGoPlayer::shishoCheckMode = shishoCheckMode; //GoTools::print("shishoCheckMode (in InnerMCGoPlayer) = %d\n",shishoCheckMode);
  int shishoCheckModeNew = 0; GoExperiments::findArgValue(argc,argv,"shishoCheckModeNew",shishoCheckModeNew);
  InnerMCGoPlayer::shishoCheckModeNew = shishoCheckModeNew;

  int playsAgainstHuman=0;findArgValue(argc,argv,"playsAgainstHuman",playsAgainstHuman);
  double resignThreshold = 0.99; if (playsAgainstHuman==1) resignThreshold=0.8;
  GoExperiments::findArgValue(argc,argv,"resignThreshold",resignThreshold);
  InnerMCGoPlayer::resignThreshold = resignThreshold;
  int minNumberOfSimulationsOfResign=10000;GoExperiments::findArgValue(argc,argv,"minNumberOfSimulationsOfResign",minNumberOfSimulationsOfResign);
  InnerMCGoPlayer::minNumberOfSimulationsOfResign = minNumberOfSimulationsOfResign;
  //   GoTools::print("resignThreshold = %f, minNumberOfSimulationsOfResign %i\n",resignThreshold,minNumberOfSimulationsOfResign);

  double openingTestThreshold= 0.1;
  GoExperiments::findArgValue(argc,argv,"openingTestThreshold",openingTestThreshold);
  InnerMCGoPlayer::openingTestThreshold = openingTestThreshold;
  //   GoTools::print("openingTestThreshold = %f\n",openingTestThreshold);

  int openingTestMode = 0;
  GoExperiments::findArgValue(argc,argv,"openingTestMode",openingTestMode);
  InnerMCGoPlayer::openingTestMode = openingTestMode;
  //   GoTools::print("openingTestMode = %d\n",openingTestMode);

  int slowScore = 0; GoExperiments::findArgValue(argc,argv,"slowScore",slowScore); Goban::slowScore = slowScore; //GoTools::print("slowScore = %d\n",slowScore);
  int useLockSafer = 0; GoExperiments::findArgValue(argc,argv,"useLockSafer",useLockSafer); InnerMCGoPlayer::useLockSafer = useLockSafer; //GoTools::print("useLockSafer = %d\n",useLockSafer);

  int allAllMovesAtFirst = 0; GoExperiments::findArgValue(argc,argv,"allAllMovesAtFirst",allAllMovesAtFirst); InnerMCGoPlayer::allAllMovesAtFirst = allAllMovesAtFirst; //GoTools::print("allAllMovesAtFirst = %d\n",allAllMovesAtFirst);

  int useDistanceInGPPatterns=0;GoExperiments::findArgValue(argc,argv,"useDistanceInGPPatterns",useDistanceInGPPatterns);Goban::useDistanceInGPPatterns=(bool)useDistanceInGPPatterns;


  int loadTree=0;GoExperiments::findArgValue(argc,argv,"loadTree",loadTree);
  if (loadTree) {/**assert(0);**/
    DecisionTree *tree=YaDTInterface::readTreeFromXMLFile("tree_tmp.xml");
    assert(tree);
    Goban::setInterestingMoveFinder(new InterestingMoveFinderDecision(tree));
  }

  int random200Iterations=3;GoExperiments::findArgValue(argc,argv,"random200Iterations",random200Iterations);Goban::random200Iterations=random200Iterations;
  int moveChangedMode=0;GoExperiments::findArgValue(argc,argv,"moveChangedMode",moveChangedMode);Goban::moveChangedMode=moveChangedMode;
  double powProbabilityOfStringSurvival=1.;
  GoExperiments::findArgValue(argc,argv,"powProbabilityOfStringSurvival",powProbabilityOfStringSurvival);
  Goban::powProbabilityOfStringSurvival=powProbabilityOfStringSurvival;
  double epsilonGreedy=0.;GoExperiments::findArgValue(argc,argv,"epsilonGreedy",epsilonGreedy);
  Goban::epsilonGreedy=epsilonGreedy;

  double successThresholdForInterestingMoves=0.7;GoExperiments::findArgValue(argc,argv,"successThresholdForInterestingMoves",successThresholdForInterestingMoves);
  double successThresholdForNonInterestingMoves=0.5;GoExperiments::findArgValue(argc,argv,"successThresholdForNonInterestingMoves",successThresholdForNonInterestingMoves);
  int retryForNonInterestingMoves=0;GoExperiments::findArgValue(argc,argv,"retryForNonInterestingMoves",retryForNonInterestingMoves);
  MoGo::Goban::successThresholdForInterestingMoves=successThresholdForInterestingMoves;
  MoGo::Goban::successThresholdForNonInterestingMoves=successThresholdForNonInterestingMoves;
  MoGo::Goban::retryForNonInterestingMoves=retryForNonInterestingMoves;

  

  int koUseMode=0;findArgValue(argc, argv, "koUseMode", koUseMode); Goban::koUseMode=koUseMode;
  int avoidSelfAtariMode=0;findArgValue(argc, argv, "avoidSelfAtariMode", avoidSelfAtariMode); Goban::avoidSelfAtariMode=avoidSelfAtariMode;
  int eatingSizePrefered=3;findArgValue(argc, argv, "eatingSizePrefered", eatingSizePrefered); Goban::eatingSizePrefered=eatingSizePrefered;

  int raceForLibertiesThreshold=3;findArgValue(argc, argv, "raceForLibertiesThreshold", raceForLibertiesThreshold);
  MoGo::Goban::raceForLibertiesThreshold=raceForLibertiesThreshold;
  int raceForLibertiesLengthThreshold=3;findArgValue(argc, argv, "raceForLibertiesLengthThreshold", raceForLibertiesLengthThreshold);
  MoGo::Goban::raceForLibertiesLengthThreshold=raceForLibertiesLengthThreshold;

  int stopAMAFOnBigCaptures=-1;findArgValue(argc, argv, "stopAMAFOnBigCaptures", stopAMAFOnBigCaptures);
  MoGo::InnerMCGoPlayer::stopAMAFOnBigCaptures=stopAMAFOnBigCaptures;


  int useGlobalPatterns=0;findArgValue(argc, argv, "useGlobalPatterns", useGlobalPatterns);
  Goban::useGlobalPatterns=useGlobalPatterns;

  int updateCloseLiberties=0;findArgValue(argc, argv, "updateCloseLiberties", updateCloseLiberties);
  Goban::updateCloseLiberties=updateCloseLiberties;
  int unlockChooseMove=0;findArgValue(argc, argv, "unlockChooseMove", unlockChooseMove);
  InnerMCGoPlayer::unlockChooseMove=unlockChooseMove;
  //   GoTools::print("Enviroment initiation finished. These parameters are set but only as global/static ones.\n");

  std::string smallDatabaseName="test.db";findArgValue(argc,argv,"smallDatabaseName",smallDatabaseName);
  int useSmallDatabase=0;findArgValue(argc,argv,"useSmallDatabase",useSmallDatabase);
  int minPatternSize=0;findArgValue(argc,argv,"minPatternSize",minPatternSize);
  if (useSmallDatabase) {/**assert(0);**/
    Goban::smallDatabase.buildByGnugoDatabase(smallDatabaseName.c_str(), minPatternSize);
    Goban::smallDatabase.compile();
  }

  int allowAttackInLadderWhenAdjacent=0;findArgValue(argc,argv,"allowAttackInLadderWhenAdjacent",allowAttackInLadderWhenAdjacent);
  Goban::allowAttackInLadderWhenAdjacent=allowAttackInLadderWhenAdjacent;
  int reAllowMovesOutsideBitmapAtRoot=0;findArgValue(argc,argv,"reAllowMovesOutsideBitmapAtRoot",reAllowMovesOutsideBitmapAtRoot); InnerMCGoPlayer::reAllowMovesOutsideBitmapAtRoot=reAllowMovesOutsideBitmapAtRoot;

  int useLibEgoTest=0;findArgValue(argc,argv,"useLibEgoTest",useLibEgoTest);
  InnerMCGoPlayer::useLibEgoTest=useLibEgoTest;
#ifdef WITH_LIBEGO_TEST
  AllLibEgoCode::setEnvironnement(argc, argv);
#endif
  int useFastBoard=0;findArgValue(argc,argv,"useFastBoard",useFastBoard);
  InnerMCGoPlayer::useFastBoard=useFastBoard;
  FastBoard::setEnvironnement(argc, argv);
  initAllPatternWeights(argc, argv);

  int useNewTrainOneGame=0;findArgValue(argc,argv,"useNewTrainOneGame",useNewTrainOneGame); InnerMCGoPlayer::useNewTrainOneGame=useNewTrainOneGame;
  int stackSizePerThread=50; findArgValue(argc,argv,"stackSizePerThread",stackSizePerThread); InnerMCGoPlayer::stackSizePerThread=stackSizePerThread;
  int commonStack=0; findArgValue(argc,argv,"commonStack",commonStack); InnerMCGoPlayer::commonStack=commonStack;
  int moveSelectionByUpdate=0; findArgValue(argc,argv,"moveSelectionByUpdate",moveSelectionByUpdate); InnerMCGoPlayer::moveSelectionByUpdate=moveSelectionByUpdate;

  int useSequencesInTree=0; findArgValue(argc,argv,"useSequencesInTree",useSequencesInTree); InnerMCGoPlayer::useSequencesInTree=useSequencesInTree;
  int usesSuperKoInTree=1; findArgValue(argc,argv,"usesSuperKoInTree",usesSuperKoInTree); InnerMCGoPlayer::usesSuperKoInTree=usesSuperKoInTree;
  int nbMovesRandomAtOpening=0; findArgValue(argc,argv,"nbMovesRandomAtOpening",nbMovesRandomAtOpening); InnerMCGoPlayer::nbMovesRandomAtOpening=nbMovesRandomAtOpening;

  double limitMemoryUsed=500.; findArgValue(argc,argv,"limitMemoryUsed",limitMemoryUsed); InnerMCGoPlayer::limitMemoryUsed=limitMemoryUsed;

  GoTools::initMemoryUsed();

  int innerMCGoPlayerStaticV1=0; findArgValue(argc,argv,"innerMCGoPlayerStaticV1",innerMCGoPlayerStaticV1); InnerMCGoPlayer::innerMCGoPlayerStaticV1=innerMCGoPlayerStaticV1;
  int innerMCGoPlayerStaticV2=0; findArgValue(argc,argv,"innerMCGoPlayerStaticV2",innerMCGoPlayerStaticV2); InnerMCGoPlayer::innerMCGoPlayerStaticV2=innerMCGoPlayerStaticV2;

  int mercyThresholdDivider=4;findArgValue(argc,argv,"mercyThresholdDivider",mercyThresholdDivider);
  FBS_handler::mercyThresholdDivider=mercyThresholdDivider;

  int scoreMode=0;findArgValue(argc,argv,"scoreMode",scoreMode);
  FastBoard::scoreMode=scoreMode;
  double factorForSide=0.;findArgValue(argc,argv,"factorForSide",factorForSide);
  FastBoard::setFactorForSide(factorForSide);

  // ajout pour les regles japonaises
  int japanese_rules = 0;findArgValue(argc,argv,"japaneseRules",japanese_rules);
   FastBoard::japanese_rules=japanese_rules;

 //ajout pour la gestion des nakade
  int nakade = 0;findArgValue(argc,argv,"nakade",nakade);
   FastBoard::nakade=nakade;

 //ajout pour la gestion de fillboard
  int fillboard = 0;findArgValue(argc,argv,"fillboard",fillboard);
   FastBoard::fillboard=fillboard;

  
  string paths_fuseki = "";findArgValue(argc,argv,"pathsFuseki",paths_fuseki);
  InnerMCGoPlayer::pathsFuseki=paths_fuseki;
  //std::cerr<<InnerMCGoPlayer::pathsFuseki<<std::endl;

 string save_fuseki = "";findArgValue(argc,argv,"saveTreesFuseki",save_fuseki);
    InnerMCGoPlayer::saveTreesFuseki=save_fuseki;
      //      //std::cerr<<InnerMCGoPlayer::saveTreesFuseki<<std::endl;
      //
      //


  string paths_OB = "";findArgValue(argc,argv,"pathsOB",paths_OB);
  InnerMCGoPlayer::pathsOB=paths_OB;

  string saveOB = "";findArgValue(argc,argv,"saveOB",saveOB);
  InnerMCGoPlayer::saveOB=saveOB;
  
  string loadOB= "";findArgValue(argc,argv,"loadOB",loadOB);
  InnerMCGoPlayer::loadOB=loadOB;
  //std::cerr<<InnerMCGoPlayer::saveTreesFuseki<<std::endl;


  // Patterns : Alpha tests
  double alpha1=0;
  findArgValue(argc, argv, "alpha1", alpha1);
  Forme_kppv::alpha1=alpha1;

  double alpha2=0;
  findArgValue(argc, argv, "alpha2", alpha2);
  Forme_kppv::alpha2=alpha2;

  fprintf(stderr,"opening book stuff ..... \n");
  OpeningBook::getOpeningBook()->setSize(gobanSize);
  OpeningBook::getOpeningBook()->addSGFListDir(&paths_fuseki);
  OpeningBook::getOpeningBook()->undump(&loadOB);
  OpeningBook::getOpeningBook()->dump(&saveOB);
  fprintf(stderr,"opening book stuff done\n");

 
#ifdef RANDOM_PATTERN
  GE_Pattern::black_location = FB_BLACK;
  GE_Pattern::white_location = FB_WHITE;
  GE_Pattern::empty_location = FB_EMPTY;
  GE_Pattern::edge_location = FB_EDGE;
  
  string loadRP; findArgValue(argc,argv,"loadRP",loadRP);
  BasicInstinctFast::load_random_patterns(loadRP.c_str());
#endif



  return true;
}



/*
  verify the input options
 */
bool MoGo::GoExperiments::verifyCommandLine( int & argc, char **& argv ) 
{/**assert(0);**/
  const char* filename = "options_list";
  ifstream options(filename);
  list<string> l_valid_options;
  
  bool is_correct = true;

  // verification of the opening of the stream
  if(not options.is_open())
    {/**assert(0);**/
      printf("*** ERROR *** the opening of the file options_list has failed\n"); 
      fflush(stdout);
      assert(0);
      return false;
    }
  
  // conversion the file of valid options in a list of string of valid options
  while(not options.eof())
    {/**assert(0);**/
      string valid_option;
      options>>valid_option;
      l_valid_options.push_back(valid_option);
    }
  
  options.close(); //close the stream
  

  //verify the input options
  for(int i = 0; i<argc; i++)
    {/**assert(0);**/
      if(argv[i][0]!='-') continue; //no treated
      
      string option(argv[i]);
      if(option.size()<=2) continue; //no treated
      
      if(option[1]!='-') //perhaps, it lacks '-'
	{/**assert(0);**/
	  if((option[1]<'0')||(option[1]>'9'))
	    {/**assert(0);**/
	      std::cout<<"*** WARNING *** "<<option<<" is not defined"<<std::endl; 
	      std::cout<<"    --> CORRECTION : I suggest -"<<option<<std::endl;
	      is_correct = false;
	    }
	  continue;
	}
      
      if(option[1]=='-') //the option begins by "--"
	{/**assert(0);**/
	  // Is the option in the list of valid options ?

	  bool is_in = false;
	  list<string>::const_iterator i_valid_options = l_valid_options.begin();
	  while(i_valid_options!=l_valid_options.end())
	    {/**assert(0);**/
	      if(option==*i_valid_options)
		{/**assert(0);**/
		  is_in = true;
		  break;
		}
	      i_valid_options++;			
	    }
	  
	  if(not is_in)
	    {/**assert(0);**/
	      std::cout<<"*** WARNING *** "<<option<<" is unknown"<<std::endl; 
	      is_correct = false;
	    }
	  
	}
      
    }
  
  //assert(is_correct);
  
  return is_correct;
}

















// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GoExperiments::expandCommandLine( int & argc, char **& argv ) 
{
  
  /*verifyCommandLine( argc, argv ); */

  Vector<int> indexes;
  Vector<Vector<string> > expandedCommands;

  int addedSize=0;
  for (int i=0;i<argc;i++) {
    Vector<string> expandedTmp;
    if (expand(argc, argv, argv[i], expandedTmp)) {
      indexes.push_back(i);
      expandedCommands.push_back(expandedTmp);
      addedSize+=expandedTmp.size();
    }
  }

  char **newArgv=new char *[argc+addedSize-indexes.size()];

  int c=0;int cOld=0;
  for (int i=0;i<(int)expandedCommands.size();i++) {
    int start=0; if (i>0) start=indexes[i-1]+1;
    for (int j=start;j<indexes[i];j++,c++)
      newArgv[c]=argv[j];
    for (int j=0;j<(int)expandedCommands[i].size();j++,c++) {
      char *command=new char [expandedCommands[i][j].length()+1];
      strcpy(command, expandedCommands[i][j].c_str());
      newArgv[c]=command;
    }
    cOld=indexes[i]+1;
  }
  argc+=addedSize-indexes.size();

  for ( ; c < argc ; c++, cOld++)
    newArgv[c]=argv[cOld];
  argv=newArgv; // memory leak, but so small and only once...
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::GoExperiments::expand( int argc, char ** argv, const char * _command, Vector<std::string> &expandedTmp) {
  string command(_command);
  if (command == "--default9") {/**assert(0);**/
    string s("--randomMode 2015445 --fastRand 1 --isSelfAtariMode 1 --thresholdSelfAtari 3 --recheckEyeMode 1 --useColor 0 --playerName global --treeDepthAllAtFirstLimit 1000 --simulationDepthAllAtFirstLimit 1000 --selectorMode 2 --numUpdateFormulaeMoveSelector 16 --bias 0.0125 --globalPlayerUseMemoryEconomy 1  --updateEatingLocations 0 --useRLInitializor 10 --equivalentSimulationsForInitialization 15 --earlyCut 10000 --earlyCutRatioFirstMove 1. --earlyCutRatioVSSecondMove 200 --optimizeSelectMove 1 --exitTreeNbSimulations 5 --avoidSuicide 1 --useFastBoard 3 --thresholdSelfAtari 4 --playNearMode 0 --avoidAMAFPreventInTree 1 --timeDivider 14  --playsAgainstHuman 1 --collectorLimitTreeSize 1000000 --limitTreeSize 1200000");
    expandedTmp=GoTools::toVector(s);
    // --cfgMode 2 --blockMode 3 --localFirst 10000 --initiateGobanNodeUrgencyWhenAsked 0 --metaBlockMode 5 --distance1 1
    return true;
  } else if (command == "--default13") {/**assert(0);**/
    expandedTmp=GoTools::toVector("--randomMode 2015445 --fastRand 1 --isSelfAtariMode 1 --thresholdSelfAtari 3 --recheckEyeMode 1 --useColor 0 --playerName global --treeDepthAllAtFirstLimit 1000 --simulationDepthAllAtFirstLimit 1000 --selectorMode 2 --globalPlayerUseMemoryEconomy 1 --cfgMode 2 --blockMode 3 --metaBlockMode 6 --gobanSize 13 --firstMoveFixed 4 --updateEatingLocations 0  --numUpdateFormulaeMoveSelector 16 --useRLInitializor 6 --equivalentSimulationsForInitialization 15 --bias 0.0125 --tenukiMode 1 --shishoCheckMode 2 --earlyCut 10000 --earlyCutRatioFirstMove 1. --earlyCutRatioVSSecondMove 200 --optimizeSelectMove 1 --exitTreeNbSimulations 5 --avoidSuicide 1 --useFastBoard 3 --thresholdSelfAtari 4 --playNearMode 0 --avoidAMAFPreventInTree 1 --timeDivider 25 --playsAgainstHuman 1 --collectorLimitTreeSize 700000 --limitTreeSize 800000");

    return true;
  } else if (command == "--default19") {/**assert(0);**/
    expandedTmp=GoTools::toVector("--randomMode 2015445 --fastRand 1 --isSelfAtariMode 1 --thresholdSelfAtari 3 --recheckEyeMode 1 --useColor 0 --playerName global --treeDepthAllAtFirstLimit 1000 --simulationDepthAllAtFirstLimit 1000 --selectorMode 2 --globalPlayerUseMemoryEconomy 1 --cfgMode 2 --blockMode 3 --metaBlockMode 6 --gobanSize 19 --firstMoveFixed 7 --updateEatingLocations 0 --coefInterestingMoves 10. --minInterestingMoves .1 --powInterestingMoves 1. --numUpdateFormulaeMoveSelector 16 --bias 0.0125 --tenukiMode 1 --shishoCheckMode 2 --useOpeningDatabase 1 --minimumDatabaseUse 10 --databaseName openingValues_19  --keepBonusNbSimulations -1 --interestingMoveMode 11  --earlyCut 10000 --earlyCutRatioFirstMove 1. --earlyCutRatioVSSecondMove 200 --optimizeSelectMove 1 --exitTreeNbSimulations 5 --avoidSuicide 1 --useFastBoard 3 --thresholdSelfAtari 4 --playNearMode 1 --avoidAMAFPreventInTree 1 --timeDivider 50 --playsAgainstHuman 1 --collectorLimitTreeSize 470000 --limitTreeSize 600000");
    return true;
  } else if (command == "--defaultNew9") {/**assert(0);**/
    expandedTmp=GoTools::toVector("--randomMode 2015445 --fastRand 1 --isSelfAtariMode 1 --thresholdSelfAtari 3 --recheckEyeMode 1 --useColor 0 --playerName global --treeDepthAllAtFirstLimit 1000 --simulationDepthAllAtFirstLimit 1000 --selectorMode 2 --numUpdateFormulaeMoveSelector 16 --bias 0.0125 --globalPlayerUseMemoryEconomy 1  --updateEatingLocations 0 --useRLInitializor 10 --equivalentSimulationsForInitialization 15 --earlyCut 10000 --earlyCutRatioFirstMove 1. --earlyCutRatioVSSecondMove 200 --optimizeSelectMove -1 --exitTreeNbSimulations 5 --avoidSuicide 1 --useFastBoard 3 --thresholdSelfAtari 4 --playNearMode 0 --avoidAMAFPreventInTree 1 --timeDivider 14  --playsAgainstHuman 1 --limitMemoryUsed -1 --collectorLimitTreeSize 1000000 --limitTreeSize 1200000 --optimizeSelectMove -1 --useNewTrainOneGame 4 --moveSelectionByUpdate 0 --numUpdateFormulaeMoveSelector 17 --mercyThresholdDivider 4  --updateFormula 0  --innerMCGoPlayerStaticV1 3 --nBestSize 6 --startCountDown 1 --powStarCountDown 0.5 --defaultParameterSize 9 --firstMoveFixed 1 --useOpeningDatabase 1 --minimumDatabaseUse 1000000 --databaseName opening");
    return true;
  } else if (command == "--defaultNew19") {/**assert(0);**/
    expandedTmp=GoTools::toVector("--randomMode 2015445 --fastRand 1 --isSelfAtariMode 1 --thresholdSelfAtari 3 --recheckEyeMode 1 --useColor 0 --playerName global --treeDepthAllAtFirstLimit 1000 --simulationDepthAllAtFirstLimit 1000 --selectorMode 2 --numUpdateFormulaeMoveSelector 16 --bias 0.0125 --globalPlayerUseMemoryEconomy 1  --updateEatingLocations 0 --useRLInitializor 10 --equivalentSimulationsForInitialization 15 --earlyCut 10000 --earlyCutRatioFirstMove .8 --earlyCutRatioVSSecondMove 10 --optimizeSelectMove -1 --exitTreeNbSimulations 5 --avoidSuicide 1 --useFastBoard 3 --thresholdSelfAtari 4 --playNearMode 0 --avoidAMAFPreventInTree 1 --timeDivider 40  --playsAgainstHuman 1  --limitMemoryUsed -1 --collectorLimitTreeSize 400000 --limitTreeSize 600000 --optimizeSelectMove -1 --useNewTrainOneGame 4 --moveSelectionByUpdate 0 --numUpdateFormulaeMoveSelector 17 --mercyThresholdDivider 10  --innerMCGoPlayerStaticV1 3 --nBestSize 6 --startCountDown 1 --powStarCountDown 0.5 --gobanSize 19 --updateFormula 1 --equivalentSimulationsForInitialization 15 --valueForSaveMove 1.3 --equivalentSimulationsForInitializationForSaveMove 50 --useDistancesByBlock 1 --decDistanceSH 4 --coefDistanceSH 0.2 --powDistanceSH 1.  --firstMoveFixed 7 --useOpeningDatabase 1 --minimumDatabaseUse 10 --databaseName openingValues_19 --playNearMode 1 --tenukiMode 0 --defaultParameterSize 19 --shishoCheckMode 2 --reAllowMovesOutsideBitmapAtRoot 1 --allowAttackInLadderWhenAdjacent 1 ");
    return true;
  } else if (command == "--defaultNew20") {/**assert(0);**/
    expandedTmp=GoTools::toVector("--randomMode 2015445 --fastRand 1 --isSelfAtariMode 1 --thresholdSelfAtari 3 --recheckEyeMode 1 --useColor 0 --playerName global --treeDepthAllAtFirstLimit 1000 --simulationDepthAllAtFirstLimit 1000 --selectorMode 2 --bias 0.0125 --globalPlayerUseMemoryEconomy 1  --updateEatingLocations 0 --useRLInitializor 10 --equivalentSimulationsForInitialization 15 --earlyCut 10000 --earlyCutRatioFirstMove .8 --earlyCutRatioVSSecondMove 10 --optimizeSelectMove -1 --exitTreeNbSimulations 5 --avoidSuicide 1 --useFastBoard 3 --thresholdSelfAtari 4 --playNearMode 0 --avoidAMAFPreventInTree 1 --timeDivider 40  --playsAgainstHuman 1  --limitMemoryUsed -1 --collectorLimitTreeSize 400000 --limitTreeSize 600000 --optimizeSelectMove -1 --useNewTrainOneGame 4 --moveSelectionByUpdate 0 --numUpdateFormulaeMoveSelector 17 --mercyThresholdDivider 10  --innerMCGoPlayerStaticV1 3 --nBestSize 6 --startCountDown 1 --powStarCountDown 0.5 --gobanSize 19 --updateFormula 666 --equivalentSimulationsForInitialization 15 --valueForSaveMove 1.3 --equivalentSimulationsForInitializationForSaveMove 50 --useDistancesByBlock 1 --decDistanceSH 4 --coefDistanceSH 0.2 --powDistanceSH 1.  --firstMoveFixed 7 --useOpeningDatabase 1 --minimumDatabaseUse 10 --databaseName openingValues_19 --playNearMode 1 --tenukiMode 0 --defaultParameterSize 19 --shishoCheckMode 2 --reAllowMovesOutsideBitmapAtRoot 1 --allowAttackInLadderWhenAdjacent 1 ");
    return true;
  }
  else if (command == "--9") {
    expandedTmp=GoTools::toVector("--mode gtp --randomMode 2015445 --fastRand 1 --isSelfAtariMode 1 --recheckEyeMode 1 --useColor 0 --playerName global --treeDepthAllAtFirstLimit 1000 --simulationDepthAllAtFirstLimit 1000 --selectorMode 2 --numUpdateFormulaeMoveSelector 16 --bias 0.0125 --globalPlayerUseMemoryEconomy 1  --updateEatingLocations 0 --useRLInitializor 10 --equivalentSimulationsForInitialization 15 --earlyCut 10000 --earlyCutRatioFirstMove 1. --earlyCutRatioVSSecondMove 200 --optimizeSelectMove -1 --exitTreeNbSimulations 5 --avoidSuicide 1 --useFastBoard 3 --thresholdSelfAtari 5 --playNearMode 0 --avoidAMAFPreventInTree 1 --timeDivider 12  --playsAgainstHuman 1 --limitMemoryUsed -1 --collectorLimitTreeSize 400000 --limitTreeSize 1000000 --optimizeSelectMove -1 --useNewTrainOneGame 4 --moveSelectionByUpdate 0 --numUpdateFormulaeMoveSelector 17 --mercyThresholdDivider 4  --updateFormula 0  --innerMCGoPlayerStaticV1 3 --nBestSize 6 --startCountDown 1 --powStarCountDown 0.5 --defaultParameterSize 9 --firstMoveFixed 1 --useOpeningDatabase 0 --minimumDatabaseUse 1000000 --databaseName opening --srandByTime on --nakade 1 --fillboard 0 --limitTreeSize 3000000 --collectorLimitTreeSize 1500000");
    double totalTime=-1.;findArgValue(argc,argv,"totalTime",totalTime);
    if (totalTime>0.) {
      expandedTmp.push_back("--time");
      expandedTmp.push_back(GoTools::toString(totalTime/13.));
    }
    return true;
  }
    else if (command == "--7") {
	        expandedTmp=GoTools::toVector("--mode gtp --randomMode 2015445 --fastRand 1 --isSelfAtariMode 1 --recheckEyeMode 1 --useColor 0 --playerName global --treeDepthAllAtFirstLimit 1000 --simulationDepthAllAtFirstLimit 1000 --selectorMode 2 --numUpdateFormulaeMoveSelector 16 --bias 0.0125 --globalPlayerUseMemoryEconomy 1  --updateEatingLocations 0 --useRLInitializor 10 --equivalentSimulationsForInitialization 15 --earlyCut 10000 --earlyCutRatioFirstMove 1. --earlyCutRatioVSSecondMove 200 --optimizeSelectMove -1 --exitTreeNbSimulations 5 --avoidSuicide 1 --useFastBoard 3 --thresholdSelfAtari 5 --playNearMode 0 --avoidAMAFPreventInTree 1 --timeDivider 12  --playsAgainstHuman 1 --limitMemoryUsed -1 --collectorLimitTreeSize 400000 --limitTreeSize 1000000 --optimizeSelectMove -1 --useNewTrainOneGame 4 --moveSelectionByUpdate 0 --numUpdateFormulaeMoveSelector 17 --mercyThresholdDivider 4  --updateFormula 0  --innerMCGoPlayerStaticV1 3 --nBestSize 6 --startCountDown 1 --powStarCountDown 0.5 --defaultParameterSize 7 --firstMoveFixed 1 --useOpeningDatabase 0 --minimumDatabaseUse 1000000 --databaseName opening --srandByTime on --nakade 1 --fillboard 0 --limitTreeSize 3000000 --collectorLimitTreeSize 1500000");
		    double totalTime=-1.;findArgValue(argc,argv,"totalTime",totalTime);
		        if (totalTime>0.) {
				      expandedTmp.push_back("--time");
				            expandedTmp.push_back(GoTools::toString(totalTime/13.));
					        }
			    return true;
			      }
  else if (command == "--f9") {/**assert(0);**/
    expandedTmp=GoTools::toVector("--randomMode 2015445 --fastRand 1 --isSelfAtariMode 1 --thresholdSelfAtari 3 --recheckEyeMode 1 --useColor 0 --playerName global --treeDepthAllAtFirstLimit 1000 --simulationDepthAllAtFirstLimit 1000 --selectorMode 2 --numUpdateFormulaeMoveSelector 16 --bias 0.0125 --globalPlayerUseMemoryEconomy 1  --updateEatingLocations 0 --useRLInitializor 10 --equivalentSimulationsForInitialization 15 --earlyCut 10000 --earlyCutRatioFirstMove 1. --earlyCutRatioVSSecondMove 200 --optimizeSelectMove -1 --exitTreeNbSimulations 5 --avoidSuicide 1 --useFastBoard 3 --thresholdSelfAtari 4 --playNearMode 0 --avoidAMAFPreventInTree 1 --timeDivider 12  --playsAgainstHuman 1 --limitMemoryUsed -1 --collectorLimitTreeSize 400000 --limitTreeSize 1000000 --optimizeSelectMove -1 --useNewTrainOneGame 4 --moveSelectionByUpdate 0 --numUpdateFormulaeMoveSelector 17 --mercyThresholdDivider 4  --updateFormula 0  --innerMCGoPlayerStaticV1 3 --nBestSize 6 --startCountDown 1 --powStarCountDown 0.5 --defaultParameterSize 9 --firstMoveFixed 1 --useOpeningDatabase 1 --minimumDatabaseUse 1000000 --databaseName opening --srandByTime on");
    double totalTime=-1.;findArgValue(argc,argv,"totalTime",totalTime);
    if (totalTime>0.) {/**assert(0);**/
      expandedTmp.push_back("--time");
      expandedTmp.push_back(GoTools::toString(totalTime/13.));
    }
    return true;

  } else if (command == "--19") {/**assert(0);**/
    expandedTmp=GoTools::toVector("--mode gtp --randomMode 2015445 --fastRand 1 --isSelfAtariMode 1 --thresholdSelfAtari 3 --recheckEyeMode 1 --useColor 0 --playerName global --treeDepthAllAtFirstLimit 1000 --simulationDepthAllAtFirstLimit 1000 --selectorMode 2 --numUpdateFormulaeMoveSelector 16 --bias 0.0125 --globalPlayerUseMemoryEconomy 1  --updateEatingLocations 0 --useRLInitializor 10 --equivalentSimulationsForInitialization 15 --earlyCut 10000 --earlyCutRatioFirstMove .8 --earlyCutRatioVSSecondMove 10 --optimizeSelectMove -1 --exitTreeNbSimulations 5  --avoidSuicide 1 --useFastBoard 3 --thresholdSelfAtari 4 --playNearMode 0 --avoidAMAFPreventInTree 1 --timeDivider 40  --playsAgainstHuman 1  --limitMemoryUsed -1 --collectorLimitTreeSize 500000 --limitTreeSize 1000000 --useNewTrainOneGame 4 --moveSelectionByUpdate 0 --numUpdateFormulaeMoveSelector 17 --mercyThresholdDivider 10  --innerMCGoPlayerStaticV1 3 --nBestSize 6 --startCountDown 1 --powStarCountDown 0.5 --gobanSize 19 --updateFormula 1 --equivalentSimulationsForInitialization 15 --valueForSaveMove 1.3 --equivalentSimulationsForInitializationForSaveMove 50 --useDistancesByBlock 1 --decDistanceSH 4 --coefDistanceSH 0.2 --powDistanceSH 1.  --firstMoveFixed 7 --useOpeningDatabase 0 --minimumDatabaseUse 10 --databaseName openingValues_19 --playNearMode 1 --tenukiMode 0 --defaultParameterSize 19 --shishoCheckMode 2 --reAllowMovesOutsideBitmapAtRoot 1 --allowAttackInLadderWhenAdjacent 1 --timeManagementMode 3 --numMoveBeginning 75 --maxBeginning -1 --srandByTime on --nakade 1 --fillboard 1 --limitTreeSize 3000000 --collectorLimitTreeSize 1500000");
    double totalTime=-1.;findArgValue(argc,argv,"totalTime",totalTime);
    if (totalTime>0.) {/**assert(0);**/
      expandedTmp.push_back("--time");
      expandedTmp.push_back(GoTools::toString(totalTime/80.));
    } else {/**assert(0);**/
      expandedTmp.push_back("--timeManagementMode");
      expandedTmp.push_back(GoTools::toString(0));
    }
    return true;
  } else if (command == "--13") {/**assert(0);**/
    expandedTmp=GoTools::toVector("--mode gtp --randomMode 2015445 --fastRand 1 --isSelfAtariMode 1 --thresholdSelfAtari 3 --recheckEyeMode 1 --useColor 0 --playerName global --treeDepthAllAtFirstLimit 1000 --simulationDepthAllAtFirstLimit 1000 --selectorMode 2 --numUpdateFormulaeMoveSelector 16 --bias 0.0125 --globalPlayerUseMemoryEconomy 1  --updateEatingLocations 0 --useRLInitializor 10 --equivalentSimulationsForInitialization 15 --earlyCut 10000 --earlyCutRatioFirstMove .8 --earlyCutRatioVSSecondMove 10 --optimizeSelectMove -1 --exitTreeNbSimulations 5  --avoidSuicide 1 --useFastBoard 3 --thresholdSelfAtari 4 --playNearMode 0 --avoidAMAFPreventInTree 1 --timeDivider 25  --playsAgainstHuman 1  --limitMemoryUsed -1 --collectorLimitTreeSize 200000 --limitTreeSize 1000000 --useNewTrainOneGame 4 --moveSelectionByUpdate 0 --numUpdateFormulaeMoveSelector 17 --mercyThresholdDivider 5  --innerMCGoPlayerStaticV1 3 --nBestSize 6 --startCountDown 1 --powStarCountDown 0.5 --gobanSize 13 --updateFormula 1 --equivalentSimulationsForInitialization 15 --valueForSaveMove 1.3 --equivalentSimulationsForInitializationForSaveMove 50 --useDistancesByBlock 1 --decDistanceSH 4 --coefDistanceSH 0.2 --powDistanceSH 1.  --firstMoveFixed 7 --playNearMode 0 --tenukiMode 0 --defaultParameterSize 13 --shishoCheckMode 2 --reAllowMovesOutsideBitmapAtRoot 1 --allowAttackInLadderWhenAdjacent 1  --srandByTime on");
    double totalTime=-1.;findArgValue(argc,argv,"totalTime",totalTime);
    if (totalTime>0.) {/**assert(0);**/
      expandedTmp.push_back("--time");
      expandedTmp.push_back(GoTools::toString(totalTime/40.));
    }
    return true;
  }

  return false;
}













// TEMPORARY COMMENT: THIS FUNCTION IS USED
Vector< int > MoGo::GoExperiments::getIntTab( const std::string & prefix, int argc, char ** argv, int maxSize ) {
  Vector<int> res(maxSize);
  for (int i = 0 ; i < maxSize ; i++) {
    int value;
    bool ok=findArgValue(argc, argv,prefix+GoTools::toString(i),value);
    //     GoTools::print("%s ok %i\n", (prefix+GoTools::toString(i)).c_str(), int(ok));
    if (!ok) {
      res[i]=-1000000;
      continue;
    }
    GoTools::print("found %s %i\n", (prefix+GoTools::toString(i)).c_str(), value);
    res[i]=value;
  }

  return res;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GoExperiments::initAllPatternWeights( int argc, char ** argv ) {
  Goban::allPatternWeights.resize(16+4);
  Goban::allPatternWeights[0]=6000;
  Goban::allPatternWeights[1]=1000;
  Goban::allPatternWeights[2]=5000;
  Goban::allPatternWeights[3]=10000;
  Goban::allPatternWeights[4+0]=10; //ATTACH (+10)
  Goban::allPatternWeights[4+1]=20; //EXTEND (+20)
  Goban::allPatternWeights[4+2]=100; //HANE (+100)
  Goban::allPatternWeights[4+3]=100; //BLOCK (+100)
  Goban::allPatternWeights[4+4]=20; //CONNECT (+20)
  Goban::allPatternWeights[4+5]=150; //TURN (+150)
  Goban::allPatternWeights[4+6]=100; //TURN-CONNECT (+100)
  Goban::allPatternWeights[4+7]=50; //CROSS-CUT (+50)
  Goban::allPatternWeights[4+8]=20; //BLOCK-CUT (+20)
  Goban::allPatternWeights[4+9]=30; //DIAGONAL MOVE (+30)
  Goban::allPatternWeights[4+10]=10; //SHOULDER-HIT (+10)
  Goban::allPatternWeights[4+11]=50; //ONE-POINT JUMP (+50)
  Goban::allPatternWeights[4+12]=10; //KNIGHT'S-MOVE (+10)
  Goban::allPatternWeights[4+13]=-100; //EMPTY TRIANGLE (-100)
  Goban::allPatternWeights[4+14]=-200; //DUMPLING (-200)
  Goban::allPatternWeights[4+15]=70; //PUSH (70)

  Vector<int> res=getIntTab("weight", argc, argv, 20);

  for (int i=0;i<(int)Goban::allPatternWeights.size();i++) {
    if (res[i]>-1000000)
      Goban::allPatternWeights[i]=res[i];
  }
}

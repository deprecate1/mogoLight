//
// C++ Implementation: innermcgoplayer_mg
//
// Description:
//
//
// Author: Yizao Wang <yizao.wang@polytechnique.edu> and Sylvain Gelly <gelly@lri.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifdef MOGOMPI
#include "mpi.h"
#endif
#include "innermcgoplayer_mg.h"
#include "cputimechronometer_mg.h"
#include "gotools_mg.h"
#include "gobannode_mg.h"
#include "scoreevaluator_mg.h"
#include <pthread.h>
#include "localpatternsdatabase_mg.h"
#include "basic_instinct_fast_mg.h"
#include "fastboard_mg.h"

#include "libfuseki.h"
#include "openingbook_mg.h"


// /* YK stuff */
// #include <sched.h>
// #include <unistd.h>
// #include <sys/types.h>
// #include <linux/unistd.h>
// #include <syscall.h>
// /* TODO : this should be parametrized at runtime */
// #define CPU_NUMBER 4


extern int MPI_NUMBER;
extern int MPI_NUMERO;

//#define DEBUG_INNERMCGOPLAYER_ON
//#define DEBUG_INNERMCGOPLAYER_SELFTRAIN_ON
//#define DEBUG_DOTTY_ON
//#define DEBUG_INNERMCGOPLAYER_UPDATE_ON
// #define DEBUG_PARALLELIZED_INNER_ON

// #define DEBUG_MASTER_SLAVE

#define LIMIT_TREE_SIZE (400000)


int MoGo::InnerMCGoPlayer::limitTreeSize = 400000;
int MoGo::InnerMCGoPlayer::firstMoveFixed = 0;
bool MoGo::InnerMCGoPlayer::dontDisplay=false;
bool MoGo::InnerMCGoPlayer::earlyStop = false;
int MoGo::InnerMCGoPlayer::stopDifference = 30;
int MoGo::InnerMCGoPlayer::allowDoublePassInTree=0;
bool MoGo::InnerMCGoPlayer::fillRangeFirstMode = false;
int MoGo::InnerMCGoPlayer::nbThreads=1;
int MoGo::InnerMCGoPlayer::securityThreads=0;
int MoGo::InnerMCGoPlayer::accelerateMode=0;
int MoGo::InnerMCGoPlayer::shishoCheckMode = 0;
int MoGo::InnerMCGoPlayer::shishoCheckModeNew = 0;
int MoGo::InnerMCGoPlayer::territoryWinningMode = 0;
int MoGo::InnerMCGoPlayer::earlyCut=0;
double MoGo::InnerMCGoPlayer::earlyCutRatioFirstMove=0.3;
double MoGo::InnerMCGoPlayer::earlyCutRatioVSSecondMove=2.;

bool MoGo::InnerMCGoPlayer::useLockSafer=true;
int MoGo::InnerMCGoPlayer::useOneGameLocker=0;

int MoGo::InnerMCGoPlayer::mixRandomMode1 = 0;
int MoGo::InnerMCGoPlayer::mixRandomMode2 = 0;
int MoGo::InnerMCGoPlayer::mixRandomModeRatio = 0;
int MoGo::InnerMCGoPlayer::mixRandomModeCounter = 0;

double MoGo::InnerMCGoPlayer::resignThreshold = 0.99;
int MoGo::InnerMCGoPlayer::openingTestMode = 0;
double MoGo::InnerMCGoPlayer::openingTestThreshold = 0.1;
bool MoGo::InnerMCGoPlayer::earlyStopForOpeningTest = false;

bool MoGo::InnerMCGoPlayer::allAllMovesAtFirst=false;
int MoGo::InnerMCGoPlayer::imitateOnlyAsWhite=0;
int MoGo::InnerMCGoPlayer::collectorLimitTreeSize=-1;
int MoGo::InnerMCGoPlayer::minNumberOfSimulationsOfResign=10000;
int MoGo::InnerMCGoPlayer::stopAMAFOnBigCaptures=-1;
int MoGo::InnerMCGoPlayer::unlockChooseMove=0;
int MoGo::InnerMCGoPlayer::exitTreeNbSimulations=0;
int MoGo::InnerMCGoPlayer::useLibEgoTest=0;
int MoGo::InnerMCGoPlayer::useFastBoard=0;
int MoGo::InnerMCGoPlayer::reAllowMovesOutsideBitmapAtRoot=0;
int MoGo::InnerMCGoPlayer::useNewTrainOneGame=0;
int MoGo::InnerMCGoPlayer::stackSizePerThread=50; // WARNING This size is "per thread" only when "commonStack==0". In "commonStack==1", then it corresponds to the total size of the stack
int MoGo::InnerMCGoPlayer::commonStack=0;
int MoGo::InnerMCGoPlayer::moveSelectionByUpdate=0;
int MoGo::InnerMCGoPlayer::useSequencesInTree=0;
int MoGo::InnerMCGoPlayer::usesSuperKoInTree=1;
int MoGo::InnerMCGoPlayer::nbMovesRandomAtOpening=0;
double MoGo::InnerMCGoPlayer::limitMemoryUsed=-1.;

int MoGo::InnerMCGoPlayer::innerMCGoPlayerStaticV1=0;
int MoGo::InnerMCGoPlayer::innerMCGoPlayerStaticV2=0;

string MoGo::InnerMCGoPlayer::pathsFuseki="";
string MoGo::InnerMCGoPlayer::saveTreesFuseki="";

string MoGo::InnerMCGoPlayer::pathsOB="";
string MoGo::InnerMCGoPlayer::saveOB="";
string MoGo::InnerMCGoPlayer::loadOB="";

#define COEF_CACHE_MISS_InnerMCGoPlayer ((1024))


Vector<MoGo::Goban *> MoGo::InnerMCGoPlayer::allBoards(NUM_THREADS_MAX);

#ifdef MOGOMPI
static double mpi_stats_allreduce_time = 0.0;
static int    mpi_stats_allreduce_nb = 0;
static int    mpi_stats_allreduce_nb_for_this_move=0;
#endif

using namespace MoGo;

namespace MoGo {

// TEMPORARY COMMENT: THIS FUNCTION IS USED
	InnerMCGoPlayer::InnerMCGoPlayer(const Goban & goban,
					 const MoveSelector * simulationMoveSelector,
					 const MoveSelector * playMoveSelector,
					 const TreeUpdator * treeUpdator ,
					 const double komi )
      : GoPlayer(), playerTree(goban) {
#ifdef JY2
	for (int i=0;i<50;i++) myIndex[i]=0;
#endif
    this->playerForFastFinishingGame=0;
    this->positionEvaluator=0;deletePositionEvaluator=false;
    this->komi=komi;
 	 this->totalSims=0;
    if (simulationMoveSelector)
      this->simulationMoveSelector=simulationMoveSelector->clone();
    
    if (playMoveSelector)
      this->playMoveSelector=playMoveSelector->clone();
    
    if (treeUpdator)
      this->treeUpdator=treeUpdator->clone();
   
        this->simulationMoveSelector->setUpdator(this->treeUpdator);

        initiatePlayerGobanNode(goban,playerTree.getRootNodeAddress(), currentSequence);

        deepestDepthOfTree=1;
        this->randomMode=EATING_PREFERED_RANDOM_MODE_1; this->randomMode2=this->randomMode;
        //this->randomMode=NORMAL_RANDOM_MODE;

        correlationModeOn=false;
        nbSimulationsForEachNode=1;
        noiseLevel=0.;
        openingMode=0;
        nbTotalSimulations=-1;
        keepTreeIfPossible=false;
        this->gobanSize = goban.height();
        if (goban.height() != goban.width()) GoTools::print("warning! goban is not squared.\n");
        ////A EFFACER this->server=0;this->parallelizedUpdator=0;this->nbChildsAlreadyDone=0;this->nbJobsSent=0;nbAlreadyComputedGobans=0;

        imitateMode = 0;
        imitateMoveNumber = 40;
        stillImitating = false;

 
        initializeThreadVariables();

        isAThreadInCutTree=false;

        localDatabase=0; localDatabaseMode=0;
        this->komiModifier=0;this->addKomiMax=0.;this->addKomiMin=0.;this->addKomiAlpha=1.;

        autoKomiMode=0; winLimitIncreaseKomi=0.; winLimitDecreaseKomi=0.; autoKomiMin=0; autoKomiMax=0; iterationsBeforeModifyKomi=0; autoKomiModifier=0.;iterationsModifyKomi=0;

        startSimulationMode=0;
        startSimulationNbSimulations=0;
        startSimulationNbNodes=0;
        initiateStaticAnalysisOfBoardMode=0;

        this->typeOfComputation=-1;

        totalThinkingTime=0.;
        nbSimulationUsed=0;
        nbThoughtMoves=0;
        totalWantedThinkingTime=0.;
        nbWantedSimulations=0;
        protectedNodes.clear();
        nbSimulationsAtRoot=0;
        stopSlaves=false; pauseSlaves=false;
        startSlaves=false;
        lastNbSimulationsUsed=0;

        sizePerNode=0;
        gobanSizeForSizePerNodeRecord=0;
        collectorLimitTreeSizeAutomatic=1<<30;


        checkSuperKoByLoop=0;
    }

 

    InnerMCGoPlayer::~InnerMCGoPlayer() {/**assert(0);**/
        delete treeUpdator;
        delete simulationMoveSelector;
        delete playMoveSelector;
        if (playerForFastFinishingGame)
            delete playerForFastFinishingGame;
        if (positionEvaluator && deletePositionEvaluator)
            delete positionEvaluator;
    }
}

bool MoGo::InnerMCGoPlayer::isGobanSavedBefore(const Goban & goban ) const {/**assert(0);**/
    return playerTree.isInTheTree(goban.getGobanKey());
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
Location MoGo::InnerMCGoPlayer::generateOneMoveNOMPI( const Goban & goban, double time ) 
{

  

  
  
  //TODO not tested with MPI or game with handicap stones 
  Location move_by_fuseki = getOneMoveByFuseki(goban);
  if(move_by_fuseki!=PASSMOVE) return move_by_fuseki;
  


    hasResigned_=false;
    earlyStopForOpeningTest = false;

    currentRootGoban=goban;
    initiateKomis(goban);
    initiateStaticAnalysisOfBoard(goban);
    clearStacks();

    Location locationOpen;

    // symetric playing
    if ((imitateOnlyAsWhite==0) || (goban.isBlacksTurn() && (imitateOnlyAsWhite==-1)) || ((!goban.isBlacksTurn()) && (imitateOnlyAsWhite==1)))
        if (imitateMove(goban,locationOpen)) {/**assert(0);**/
            GoTools::print("imitateMove played\n");
            return locationOpen;
        }

    // play an opening move
    if (/*goban.height() == 9 && goban.width() == 9 &&*/
        openingMove(goban,locationOpen)) {/**assert(0);**/
        GoTools::print("openingMove played (%i)\n", locationOpen);
        //     goban.textModeShowGoban();goban.textModeShowPosition(locationOpen);
        return locationOpen;
    }

    
    // a list of first good moves ? 4 corners ?
    if (firstMove(firstMoveFixed,goban,locationOpen)) {/**assert(0);**/
         GoTools::print("firstMove(firstMoveFixed,goban,locationOpen) holds\n");
         return locationOpen;
    }

    // first moves at random ?
    if (goban.moveNumber()<nbMovesRandomAtOpening) {/**assert(0);**/
        return goban.randomOutOneMove(10000);
    }


    if ((time<=0) && (nbTotalSimulations<=0))//if GTPEngine give time = 0 (what I set right now), then choose one move quickly.
  
    {/**assert(0);**/// mode panic
        selfTrain(goban,3000,1.);//simulate 3000 games in order to not give an illegal move. 2000 simulations at the end of games takes less than 0.5 seconds.
    } else {
        if (shishoCheckMode) //FIXME put it also on parallelized part, but verify
            goban.shishoCheck(shishoCheckMode, true);


        if (useFastBoard) FastBoard::initiateReferenceBoard(goban);
#ifdef PARALLELIZED
        selfTrainMultithreadsNOMPI(goban,1000000000,time);
#else
        selfTrain(goban,1000000000,time);
#endif

    }

    GoTools::print("generateOneMoveNompi a pense\n");
    showGobanUrgency(goban, currentSequence);
    if (goban.isScorable()) return PASSMOVE;


    double estimedScore=isInterestingToContinue(goban, currentSequence);

    if (estimedScore>=resignThreshold /*|| estimedScore < 0.01*/ ||
        (goban.height() == 19 && estimedScore>=0.95)) {
        hasResigned_=true;
        return PASSMOVE;
    }


    if (getNode(goban, currentSequence) == 0) {/**assert(0);**/
        GoTools::print("The root is not in the tree (bug). Tell Yizao please.\n");
        return PASSMOVE;
    }
    Location location=PASSMOVE;int i=0;
    // we select one move, and check that no infinite loop and that it's ok for rules
//  printf("before the loop, proc %d\n",MPI_NUMERO);fflush(stdout);
    for (;i<21*21;i++) { // i is a security to avoid endless loop
        Goban tmpGoban=goban;

	location=playMoveSelector->selectOneMove(getNode(goban,currentSequence), goban);
	GoTools::print("I play %d\n", location);
        if (!goban.isLegal(location)) {/**assert(0);**/
            GoTools::print("An illegal move is proposed by MoGo, probably caused by little time left. Tell Yizao please. Thanks.\n");
            return PASSMOVE;
        }
        tmpGoban.playMoveOnGoban(location);
        if (location != PASSMOVE && isSuperKo(tmpGoban))
            playMoveSelector->addForbiddenMove(location);
        else
            break;
    }
    // printf("after the loop, proc %d\n",MPI_NUMERO);fflush(stdout);
    if (i == 21*21) {/**assert(0);**/
        GoTools::print("An infinite loop has occurred!!\n");
        playMoveSelector->showForbiddenMoves();
        goban.textModeShowGoban();
        playMoveSelector->clearForbiddenMoves();
        return PASSMOVE;
    }

    playMoveSelector->clearForbiddenMoves();
    //assert(goban.isNotUseless(location));

    GoTools::print("generateOneMoveNOMPI a fini\n");
   
    return location;
}


// TEMPORARY COMMENT: THIS FUNCTION IS USED
Location MoGo::InnerMCGoPlayer::generateOneMove( const Goban & goban, double time ) 
{
  hasResigned_=false;
  earlyStopForOpeningTest = false;
  
  currentRootGoban=goban;
  initiateKomis(goban);
  initiateStaticAnalysisOfBoard(goban);
  clearStacks();
  
  Location locationOpen;

  Location move_by_fuseki = getOneMoveByFuseki(goban);
  if(move_by_fuseki!=PASSMOVE) return move_by_fuseki;
 

#ifdef GOLDENEYE
#ifdef PRINT_GOLDENEYE 
  GE_Bibliotheque::init(goban.height(),goban.width()); //initialise aussi GE_Directions
  
  GE_Goban ge_gob(goban.height(), goban.width());
  for(int big_location = 0; big_location<FBS_handler::getBigArea(); big_location++)
    {
      int location = GE_Bibliotheque::to_goban[big_location];  

      if(location<0)
	continue;
      
      int color_stone = goban.position(big_location);
      int ge_color_stone = GE_WITHOUT_STONE;
      switch(color_stone)
	{
	case BLACK : ge_color_stone = GE_BLACK_STONE; break;
	case WHITE : ge_color_stone = GE_WHITE_STONE; break;
	case EMPTY : ge_color_stone = GE_WITHOUT_STONE; break;
	default : ;
	}
      
      ge_gob.set_stone(location,ge_color_stone);
    }
  
  team_color ge_color_player = GE_BLACK;
  if(goban.isBlacksTurn())
    ge_color_player = GE_BLACK;
  else ge_color_player = GE_WHITE;
  
  GE_Game ge_game(ge_gob,ge_color_player);
  
  GE_FastGoban ge_fg;
  GE_FastGoban ge_fg_for_simu;
  
  //GE_Directions::init(gob.height, gob.width);
  ge_fg.init();
  ge_fg_for_simu.init();
  ge_fg.fromGame(ge_game);
  
  //GE_FastMCLocationsInformations ge_fmcli(ge_color_player);
  
  clock_t t_start;
  clock_t t_end;
  float timing = 0.f;
  
  vector<vector<int> > v_semeai;
  v_semeai.resize(GE_Directions::board_area);
  for(int i = 0; i<(int)v_semeai.size(); i++)
    {
      v_semeai[i].resize(GE_Directions::board_area, 0);
    }
  
  ge_fg.print_error_goban();
  
  int num_sim = 0; 
  int max_sim = 100;
  int min_capture = 4;
  //double my_komi = 7.5f; //Comment recuperer l'information du komi
  while(true)
    {      
      t_start = clock();
      //fg.fromGame(*game);
      ge_fg_for_simu.copy(ge_fg);
      ge_fg_for_simu.launch_simulation(min_capture);
      //ge_fmcli.update(ge_fg_for_simu, my_komi);
      
      GE_FastMCLocationsInformations::update_stats_semeai(ge_fg_for_simu, v_semeai);
      
      t_end = clock();
      
      timing+=((t_end-t_start)/(float)CLOCKS_PER_SEC);
      
      num_sim++;
      
      
      if(num_sim>=max_sim)
	break;
    }
  
  GE_Chains chs;
  GE_AnalyzeSemeai a_sem;
  a_sem.setSemeais(ge_game, v_semeai, max_sim, chs);
//  a_sem.print_error_moves_for_semeai(ge_game);
  list<int> l_moves;
  a_sem.moves_for_semeais(ge_game,l_moves);

  vector<int> semeai_moves;
  while(not l_moves.empty())
    {
      int location = l_moves.front(); 

      int big_location = GE_Bibliotheque::to_big_goban[location];
      semeai_moves.push_back(big_location);

      l_moves.pop_front();
    }
  

  cerr<<"tableau de coups interessants pour tous les semeais: "<<endl;
  for(int i = 0; i<(int)semeai_moves.size(); i++)
    {
      goban.textModeShowGoban(semeai_moves[i]);
      cerr<<" ";
    }

  cerr<<endl;
  cerr<<"end semeai"<<endl;
  
  



/*
  int num_semeai = 1;
  list<GE_Semeai>::const_iterator i_sem = semeais.begin();
  while(i_sem!=semeais.end())
    {
      cerr<<"semeai "<<num_semeai<<endl;
      i_sem->print_error_semeai();
      cerr<<endl;
      
      cerr<<"moves for semeai: "<<endl;
      GE_Marker mk(GE_Directions::board_area);
      list<int> l_moves;
      getInterestingMoves(g, *i_sem, mk, l_moves);
      GE_ListOperators::print_error(l_moves, move_to_string, 
				 GE_Directions::board_size);
      
      cerr<<endl;
      
      i_sem++;
      num_semeai++;
    }
*/

  //GE_Bibliotheque::to_big_goban[big_location];  
  //GE_Marker mk(GE_Directions::board_area);



  //list<int> l_moves;
  //a_sem.getInterestingMoves(g, *i_sem, mk, l_moves);




#endif
#endif
  

 
#ifdef LEARNING
 /*   if (miningMove) {
	    miningMove=0;
        selfTrain(goban,3000,1.);
    }
    else*/ {
#endif
  
  
    // symetric playing
    if ((imitateOnlyAsWhite==0) || (goban.isBlacksTurn() && (imitateOnlyAsWhite==-1)) || ((!goban.isBlacksTurn()) && (imitateOnlyAsWhite==1)))
        if (imitateMove(goban,locationOpen)) {/**assert(0);**/
            GoTools::print("imitateMove played\n");
            return locationOpen;
        }

    // play an opening move
    if (/*goban.height() == 9 && goban.width() == 9 &&*/
        openingMove(goban,locationOpen)) {/**assert(0);**/
        GoTools::print("openingMove played (%i)\n", locationOpen);
        //     goban.textModeShowGoban();goban.textModeShowPosition(locationOpen);
        return locationOpen;
    }

    
    // a list of first good moves ? 4 corners ?
         if (firstMove(firstMoveFixed,goban,locationOpen)) {
         GoTools::print("firstMove(firstMoveFixed,goban,locationOpen) holds\n");
         return locationOpen;
         }
    


    // first moves at random ?
    if (goban.moveNumber()<nbMovesRandomAtOpening) {/**assert(0);**/
        return goban.randomOutOneMove(10000);
    }

       // GoTools::print("selfTrainMultithreads to be done\n", locationOpen);

    if ((time<=0) && (nbTotalSimulations<=0))//if GTPEngine give time = 0 (what I set right now), then choose one move quickly.
 
    {/**assert(0);**/// mode panic
	   // GoTools::print("vers selfTrain panic");
        selfTrain(goban,3000,0.1);//simulate 3000 games in order to not give an illegal move. 2000 simulations at the end of games takes less than 0.5 seconds.
    } else {
	  //  GoTools::print("no panic");
        if (shishoCheckMode) //FIXME put it also on parallelized part, but verify
            goban.shishoCheck(shishoCheckMode, true);


        if (useFastBoard) FastBoard::initiateReferenceBoard(goban);
#ifdef PARALLELIZED
	  //  GoTools::print("vers selfTrain multithread");
        selfTrainMultithreads(goban,1000000000,time);

#else
        selfTrain(goban,1000000000,time);
#endif
    GoTools::print("generateOneMove a pense\n");

    }

#ifdef LEARNING
}
#endif    
        //GoTools::print("selfTrainMultithreads done\n", locationOpen);
    //showGobanUrgency(goban, currentSequence);
    if (goban.isScorable()) return PASSMOVE;


    double estimedScore=isInterestingToContinue(goban, currentSequence);
    GoTools::print("estimatedscore avant partage = %g\n",estimedScore);

#ifndef NOPEDAGO
{
    char parlons[500];
    char bonnePierre[7];
    static double estimedScores[500];
    static int first=-7;
    static int loosingMove=-7;
    static int neverSaid=-7;
    fprintf(stderr,"lastMoves.size()=%i\n",lastMoves.size());
    if (first<0) first=lastMoves.size();

   if ((estimedScore>0.55)&&(lastMoves.size()>0))
   { 
	   loosingMove=lastMoves.size();
	   int location=lastMoves[lastMoves.size()-1];
  if (location==PASSMOVE) {
	loosingMove=-7;
		    }
  int x,y;
    x=goban.xIndex(location);
      y=goban.yIndex(location);
        if (y>=8)
		    sprintf(bonnePierre,"%c%i ",(char)('A'+y+1),gobanSize-x);
	  else
		      sprintf(bonnePierre,"%c%i ",(char)('A'+y),gobanSize-x);
	//    if (gobanSize>9 && gobanSize-x<10) GoTools::print(" ");

   }
    estimedScores[lastMoves.size()]=estimedScore;
//espeak -v fr -s 30 "J'ai $* % de chances de gagner" -w /tmp/truc${dt} ; aplay /tmp/truc${dt} ; rm /tmp/truc${dt}
 
    if ((estimedScore>estimedScores[first]+0.2)&&(estimedScore<0.35)&&(loosingMove>1)) {sprintf(parlons,"  (espeak -v fr  -w /tmp/trucaz.wav \"C'est mal parti... Essaie autre chose. Reviens à la pierre %s\"; aplay /tmp/trucaz.wav ; rm /tmp/trucaz.wav ) & ",bonnePierre); system(parlons);}
    else if ((estimedScore>estimedScores[first]+0.2)&&(estimedScore<0.35)) {sprintf(parlons,"  (espeak -v fr  -w /tmp/trucaz.wav \"C'est mal parti... Reprends au début.\"; aplay /tmp/trucaz.wav ; rm /tmp/trucaz.wav ) & "); system(parlons);}
    else 
    if ((estimedScores[first]>0.7)&&(estimedScore>0.65)) 
    {
	    if (neverSaid<0){
	    sprintf(parlons,"  (espeak -v fr  -w /tmp/trucaz.wav \"Dans une partie handicap il faut jouer simple et solide. Prends le temps de protéger tes pierres.\"; aplay /tmp/trucaz.wav ; rm /tmp/trucaz.wav ) & "); system(parlons);neverSaid=5;}
	    else 
	    {neverSaid--;
	    sprintf(parlons,"  (espeak -v fr  -w /tmp/trucaz.wav \"Hum.\"; aplay /tmp/trucaz.wav ; rm /tmp/trucaz.wav ) & "); system(parlons);}
    }
     else
    if ((lastMoves.size()>1)&&(estimedScore>estimedScores[lastMoves.size()-2]+0.05)){sprintf(parlons,"  (espeak -v fr  -w /tmp/trucaz.wav \"Bon coup.\"; aplay /tmp/trucaz.wav ; rm /tmp/trucaz.wav ) & "); system(parlons);} else
    if ((lastMoves.size()>1)&&(estimedScore<estimedScores[lastMoves.size()-2]-.2)) {sprintf(parlons,"  (espeak -v fr  -w /tmp/trucaz.wav \"Très très mauvais coup. Essaie autre chose.\"; aplay /tmp/trucaz.wav ; rm /tmp/trucaz.wav ) & "); system(parlons);} else
    if ((lastMoves.size()>1)&&(estimedScore<estimedScores[lastMoves.size()-2]-.1)) {sprintf(parlons,"  (espeak -v fr  -w /tmp/trucaz.wav \"Mauvais coup. Essaie autre chose.\"; aplay /tmp/trucaz.wav ; rm /tmp/trucaz.wav ) & "); system(parlons);} else
    if ((lastMoves.size()>1)&&(estimedScore<estimedScores[lastMoves.size()-2]-0.05)) {sprintf(parlons,"  (espeak -v fr  -w /tmp/trucaz.wav \"Il y a de meilleurs coups.\"; aplay /tmp/trucaz.wav ; rm /tmp/trucaz.wav ) & "); system(parlons);} else
    
 //   sprintf(parlons,"( espeak -v fr   -w /tmp/trucaz.wav \"J'ai %i pourcents de chances de gagner.\"; aplay /tmp/trucaz.wav ; rm /tmp/trucaz.wav )  & ",100-(int)ceil(estimedScore*100)); system(parlons);}
    if (estimedScore>0.65)
    {sprintf(parlons,"  (espeak -v fr  -w /tmp/trucaz.wav \"Tu as l'avantage: joue sans violence.\"; aplay /tmp/trucaz.wav ; rm /tmp/trucaz.wav ) & "); system(parlons);}
     else 
    if (estimedScore<0.15)
    {sprintf(parlons,"  (espeak -v fr  -w /tmp/trucaz.wav \"Tu vas perdre. Recommence.\"; aplay /tmp/trucaz.wav ; rm /tmp/trucaz.wav ) & "); system(parlons);} else
    if (estimedScore<0.35)
{ sprintf(parlons,"  (espeak -v fr  -w /tmp/trucaz.wav \"Tu es mal parti. Tu devrais recommencer; ou alors il faut tenter quelque chose de fort.\"; aplay /tmp/trucaz.wav ; rm /tmp/trucaz.wav ) & "); system(parlons);}
}
#endif

#ifdef MOGOMPI
// TODO mpi share
    double otherEstimedScore = 0;
    GoTools::print("generateOneMove va estimer\n");
    /*if (MPI_NUMERO>0)
    {
        assert(MPI_Send((double*)&estimedScore,1,MPI_DOUBLE,0,MPI_TAG_ESTIMATE,MPI_COMM_WORLD)==MPI_SUCCESS);
	assert(MPI_Recv((double*)&estimedScore,1,MPI_DOUBLE,0,MPI_TAG_ESTIMATE,MPI_COMM_WORLD,MPI_STATUS_IGNORE)==MPI_SUCCESS);
    }
    else
    {

        for (int i=1;i<MPI_NUMBER;i++)
        {
            assert(MPI_Recv((double*)&otherEstimedScore,1,MPI_DOUBLE,i,MPI_TAG_ESTIMATE,MPI_COMM_WORLD,MPI_STATUS_IGNORE)==MPI_SUCCESS);
            estimedScore += otherEstimedScore;
        }
        estimedScore /= MPI_NUMBER;
	 
        for (int i=1;i<MPI_NUMBER;i++)
            assert(MPI_Send((double*)&estimedScore,1,MPI_DOUBLE,i,MPI_TAG_ESTIMATE,MPI_COMM_WORLD)==MPI_SUCCESS);
	 	
    }*/

    MPI_Allreduce(&estimedScore,&otherEstimedScore,1,MPI_DOUBLE,MPI_SUM,MPI_COMM_WORLD); //FIXME EN MPI2, MPI_IN_PLACE serait le bon 1er argument
    estimedScore=otherEstimedScore/double(MPI_NUMBER);

 //   GoTools::print("generateOneMove a estime\n");
 //   fprintf(stderr,"Estimated score for the opponent (PARTAGE)  = %g\n",estimedScore);
#endif

    if (estimedScore>=resignThreshold /*|| estimedScore < 0.01*/ ||
        (goban.height() == 19 && estimedScore>=0.95)) {/**assert(0);**/
        hasResigned_=true;
#ifndef NOPEDAGO
	{
	char parlons[500];
	//system(" ( espeak -v fr -w /tmp/trucaz.wav \" Bien joué! J'abandonne.  \" ; aplay /tmp/trucaz.wav; rm /tmp/trucaz.wav ) &\"");
    sprintf(parlons,"( espeak -v fr   -w /tmp/trucaz.wav \"Bien joué! J'abandonne. \"; aplay /tmp/trucaz.wav ; rm /tmp/trucaz.wav )   "); system(parlons);}
    exit(0);

#endif
        return PASSMOVE;
    }


    if (getNode(goban, currentSequence) == 0) {/**assert(0);**/
        GoTools::print("The root is not in the tree (bug). Tell Yizao please.\n");
        return PASSMOVE;
    }
    Location location=PASSMOVE;int i=0;
    // we select one move, and check that no infinite loop and that it's ok for rules
//  printf("before the loop, proc %d\n",MPI_NUMERO);fflush(stdout);
    for (;i<21*21;i++) { // i is a security to avoid endless loop
        Goban tmpGoban=goban;

#ifdef MOGOMPI
    	location=playMoveSelector->selectOneMoveMPI(getNode(goban,currentSequence), goban);
#else
	location=playMoveSelector->selectOneMove(getNode(goban,currentSequence), goban);
#endif
	GoTools::print("I play %d\n", location);

        if (!goban.isLegal(location)) {/**assert(0);**/
            GoTools::print("An illegal move is proposed by MoGo, probably caused by little time left. Tell Yizao please. Thanks.\n");
            return PASSMOVE;
        }
        tmpGoban.playMoveOnGoban(location);
        if (location != PASSMOVE && isSuperKo(tmpGoban))
            playMoveSelector->addForbiddenMove(location);
        else
            break;
    }
    // printf("after the loop, proc %d\n",MPI_NUMERO);fflush(stdout);
    if (i == 21*21) {/**assert(0);**/
        GoTools::print("An infinite loop has occurred!!\n");
        playMoveSelector->showForbiddenMoves();
        goban.textModeShowGoban();
        playMoveSelector->clearForbiddenMoves();
        return PASSMOVE;
    }

    playMoveSelector->clearForbiddenMoves();
    //assert(goban.isNotUseless(location));
  
    return location;
}



MoGo::GoPlayer * MoGo::InnerMCGoPlayer::clone( ) const {/**assert(0);**/
    return new InnerMCGoPlayer(*this);
}

namespace MoGo {
    class selfTrainThreadInfos {
        //     friend class InnerMCGoPlayer;
          public:

        // TEMPORARY COMMENT: THIS FUNCTION IS USED
        selfTrainThreadInfos(InnerMCGoPlayer *player, const Goban & goban, const int number, const double time, int identifier) {
            this->player=player;this->goban=goban; this->number=number; this->time=time; this->identifier=identifier;
        }
        Goban goban;
        int number;
        double time;
        InnerMCGoPlayer *player;
        int identifier;
    };
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void *selfTrainThread(void *infos) {
    MoGo::selfTrainThreadInfos *infosTmp=(MoGo::selfTrainThreadInfos *)infos;
    //   if (!dontDisplay)
    //     GoTools::print("start of thread %i\n", infosTmp->identifier);
    //   while(true) GoTools::print("%i\n",pthread_self());
#ifdef UseQuickMemoryManagementForFastVector
#ifdef PARALLELIZED
    bool readyToGo=false;
    GoTools::print("we are in selfTrainThread\n");
    pthread_mutex_lock(&ThreadsManagement::threadsManagementMutex);
    ThreadsManagement::addThread();
    pthread_mutex_unlock(&ThreadsManagement::threadsManagementMutex);
   GoTools::print("we are still in selfTrainThread\n");

    while (!readyToGo) {
        pthread_mutex_lock(&ThreadsManagement::threadsManagementMutex);
        readyToGo=ThreadsManagement::threadsManagementValue;
        pthread_mutex_unlock(&ThreadsManagement::threadsManagementMutex);
    };
   GoTools::print("we are still still in selfTrainThread\n");
#endif
#endif
    /* Placing code : sched_setaffinity to bind things on processors 
    int thId;
    thId = syscall(SYS_gettid);
    unsigned mask = 1 << (ThreadsManagement::getNumThread() % CPU_NUMBER);
    GoTools::print("I'm %d tid with mask %ld\n", thId, mask);
    if (sched_setaffinity(thId, sizeof(unsigned long), (const cpu_set_t *)&mask))
        GoTools::print("Pb with sched_setaffinity");
    */
    

    // while (1) {/**assert(0);**/
    // GoTools::mogoRand(GoTools::mogoRand());
    // }
    /*  int numThread=0;
        #ifdef PARALLELIZED
        numThread=ThreadsManagement::getNumThread();
        GoTools::print("Start speed test for %i\n", numThread);
        #endif
        CPUTimeChronometer chronometer;*/

    // int number=1000000;
    //   for (int j=0;j<100;j++)
    //     for (int i=0;i<2*number;i++) {/**assert(0);**/
    // GoTools::mogoRand2(1000);
    //      }
 

    //   GoTools::print("speed : %4.2f  (%f)\n", double(number)/chronometer.getTime(), chronometer.getTime());


   //GoTools::print("dans le vif du sujet  -- pas ok ");
    infosTmp->player->selfTrain(infosTmp->goban, infosTmp->number, infosTmp->time, infosTmp->identifier);
   // GoTools::print("dans le vif du sujet  --- ok");

    //   if (!dontDisplay)
    //     GoTools::print("end of thread %i\n", infosTmp->identifier);
    GoTools::print("====================== THREAD DIES =====================\n");
    pthread_exit(0);
    return 0;
}

void *selfTrainThreadNOMPI(void *infos) {
    MoGo::selfTrainThreadInfos *infosTmp=(MoGo::selfTrainThreadInfos *)infos;
    //   if (!dontDisplay)
    //     GoTools::print("start of thread %i\n", infosTmp->identifier);
    //   while(true) GoTools::print("%i\n",pthread_self());
#ifdef UseQuickMemoryManagementForFastVector
#ifdef PARALLELIZED
    bool readyToGo=false;
    GoTools::print("we are in selfTrainThread\n");
    pthread_mutex_lock(&ThreadsManagement::threadsManagementMutex);
    ThreadsManagement::addThread();
    pthread_mutex_unlock(&ThreadsManagement::threadsManagementMutex);
   GoTools::print("we are still in selfTrainThread\n");

    while (!readyToGo) {
        pthread_mutex_lock(&ThreadsManagement::threadsManagementMutex);
        readyToGo=ThreadsManagement::threadsManagementValue;
        pthread_mutex_unlock(&ThreadsManagement::threadsManagementMutex);
    };
   GoTools::print("we are still still in selfTrainThread\n");
#endif
#endif
    /* Placing code : sched_setaffinity to bind things on processors 
    int thId;
    thId = syscall(SYS_gettid);
    unsigned mask = 1 << (ThreadsManagement::getNumThread() % CPU_NUMBER);
    GoTools::print("I'm %d tid with mask %ld\n", thId, mask);
    if (sched_setaffinity(thId, sizeof(unsigned long), (const cpu_set_t *)&mask))
        GoTools::print("Pb with sched_setaffinity");
    */
    

    // while (1) {/**assert(0);**/
    // GoTools::mogoRand(GoTools::mogoRand());
    // }
    /*  int numThread=0;
        #ifdef PARALLELIZED
        numThread=ThreadsManagement::getNumThread();
        GoTools::print("Start speed test for %i\n", numThread);
        #endif
        CPUTimeChronometer chronometer;*/

    // int number=1000000;
    //   for (int j=0;j<100;j++)
    //     for (int i=0;i<2*number;i++) {/**assert(0);**/
    // GoTools::mogoRand2(1000);
    //      }
 

    //   GoTools::print("speed : %4.2f  (%f)\n", double(number)/chronometer.getTime(), chronometer.getTime());


   //GoTools::print("dans le vif du sujet  -- pas ok ");
    infosTmp->player->selfTrainNOMPI(infosTmp->goban, infosTmp->number, infosTmp->time, infosTmp->identifier);
   // GoTools::print("dans le vif du sujet  --- ok");

    //   if (!dontDisplay)
    //     GoTools::print("end of thread %i\n", infosTmp->identifier);
    GoTools::print("====================== THREAD DIES =====================\n");
    pthread_exit(0);
    return 0;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayer::selfTrainMultithreads( const Goban & goban, const int number, const double time ) {
#ifdef PARALLELIZED
    int nbThreadsToUse=nbThreads;
    if (goban.approximateSizeOfPossibleMoves()<securityThreads)
        nbThreadsToUse=1;
    Vector<pthread_t> threads(nbThreadsToUse);
    Vector<selfTrainThreadInfos *> infos(threads.size());
    //GoTools::print("TRALALA Using a pool of %d threads\n", threads.size());
#ifdef UseQuickMemoryManagementForFastVector
    ThreadsManagement::clearNumThreads();
    //GoTools::print("Using a pool of %d threads bis\n", threads.size());
    pthread_mutex_lock(&ThreadsManagement::threadsManagementMutex);
    //GoTools::print("Using a pool of %d threads ter\n", threads.size());
    ThreadsManagement::threadsManagementValue=false;
    //GoTools::print("Using a pool of %d threads quad\n", threads.size());
    pthread_mutex_unlock(&ThreadsManagement::threadsManagementMutex);
#endif
    //GoTools::print("locks passes");

    startSlaves=false; stopSlaves=false; pauseSlaves=false;
    pthread_attr_t attr;
    int rc, status;
    for (int i=0;i<(int)threads.size();i++) {
        infos[i]=new selfTrainThreadInfos(this,goban, number, time, i);
        if (pthread_attr_init(&attr)) assert(0);
        //     if(pthread_attr_setscope(&attr,PTHREAD_SCOPE_PROCESS/*PTHREAD_SCOPE_SYSTEM*/)) assert(0);
        if (pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_JOINABLE) != 0) {
            GoTools::print("pthread_attr_setdetachstate error\n");
        }
        if (pthread_create(&threads[i], &attr, selfTrainThread, (void *)infos[i])) {/**assert(0);**/
            GoTools::print("pthread_create error\n");
        }
	else GoTools::print("========================= THREAD CREATED ===================");
        //  if (pthread_detach(threads[i])) {/**assert(0);**/
        //       GoTools::print("pthread_detach error\n");
        // }
    }
    //GoTools::print("init ok");
#ifdef UseQuickMemoryManagementForFastVector
    pthread_mutex_lock(&ThreadsManagement::threadsManagementMutex);
    ThreadsManagement::threadsManagementValue=true;
    pthread_mutex_unlock(&ThreadsManagement::threadsManagementMutex);
#endif
   // GoTools::print("relock ok ");
   usleep(10); //VOUS AVEZ CONFIANCE, LA ?  FIXMEFIXMEFIXME
    pthread_attr_destroy(&attr);// TODO teytaud-patch
    for (int i=0;i<(int)threads.size();i++) {
        //          while(true);
	    GoTools::print("join %d to be done",i);
        rc = pthread_join(threads[i], (void **)&status);
	    GoTools::print("join %d done",i);
        if (rc) { GoTools::print("ERROR; return code from pthread_join() is %d\n", rc); exit(-1);}
        /* YK
           GoTools::print("Completed join with thread %d status= %d\n",i, status);
        */
        //       GoTools::print("pthread_join error\n");
        delete infos[i];
        //     GoTools::print("locks: %i,%i\n", blockVariable, saferBlockVariable);
    }
    //pthread_attr_destroy(&attr); //TODO teytaud-patch
#else
    Goban g=goban;
    int a=number; double t=time;t+=a;
#endif
    stopSlaves=false; startSlaves=false; pauseSlaves=false;
 //   GoTools::print("on sort de selfTrainMultiThread");
}

void MoGo::InnerMCGoPlayer::selfTrainMultithreadsNOMPI( const Goban & goban, const int number, const double time ) {
#ifdef PARALLELIZED
    int nbThreadsToUse=nbThreads;
    if (goban.approximateSizeOfPossibleMoves()<securityThreads)
        nbThreadsToUse=1;
    Vector<pthread_t> threads(nbThreadsToUse);
    Vector<selfTrainThreadInfos *> infos(threads.size());
    //GoTools::print("TRALALA Using a pool of %d threads\n", threads.size());
#ifdef UseQuickMemoryManagementForFastVector
    ThreadsManagement::clearNumThreads();
    //GoTools::print("Using a pool of %d threads bis\n", threads.size());
    pthread_mutex_lock(&ThreadsManagement::threadsManagementMutex);
    //GoTools::print("Using a pool of %d threads ter\n", threads.size());
    ThreadsManagement::threadsManagementValue=false;
    //GoTools::print("Using a pool of %d threads quad\n", threads.size());
    pthread_mutex_unlock(&ThreadsManagement::threadsManagementMutex);
#endif
    //GoTools::print("locks passes");

    startSlaves=false; stopSlaves=false; pauseSlaves=false;
    pthread_attr_t attr;
    int rc, status;
    for (int i=0;i<(int)threads.size();i++) {
        infos[i]=new selfTrainThreadInfos(this,goban, number, time, i);
        if (pthread_attr_init(&attr)) assert(0);
        //     if(pthread_attr_setscope(&attr,PTHREAD_SCOPE_PROCESS/*PTHREAD_SCOPE_SYSTEM*/)) assert(0);
        if (pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_JOINABLE) != 0) {
            GoTools::print("pthread_attr_setdetachstate error\n");
        }
        if (pthread_create(&threads[i], &attr, selfTrainThreadNOMPI, (void *)infos[i])) {/**assert(0);**/
            GoTools::print("pthread_create error\n");
        }
	else GoTools::print("========================= THREAD CREATED ===================");
        //  if (pthread_detach(threads[i])) {/**assert(0);**/
        //       GoTools::print("pthread_detach error\n");
        // }
    }
    //GoTools::print("init ok");
#ifdef UseQuickMemoryManagementForFastVector
    pthread_mutex_lock(&ThreadsManagement::threadsManagementMutex);
    ThreadsManagement::threadsManagementValue=true;
    pthread_mutex_unlock(&ThreadsManagement::threadsManagementMutex);
#endif
   // GoTools::print("relock ok ");
   usleep(10); //VOUS AVEZ CONFIANCE, LA ?  FIXMEFIXMEFIXME
    pthread_attr_destroy(&attr);// TODO teytaud-patch
    for (int i=0;i<(int)threads.size();i++) {
        //          while(true);
	    GoTools::print("join %d to be done",i);
        rc = pthread_join(threads[i], (void **)&status);
	    GoTools::print("join %d done",i);
        if (rc) { GoTools::print("ERROR; return code from pthread_join() is %d\n", rc); exit(-1);}
        /* YK
           GoTools::print("Completed join with thread %d status= %d\n",i, status);
        */
        //       GoTools::print("pthread_join error\n");
        delete infos[i];
        //     GoTools::print("locks: %i,%i\n", blockVariable, saferBlockVariable);
    }
    //pthread_attr_destroy(&attr); //TODO teytaud-patch
#else
    Goban g=goban;
    int a=number; double t=time;t+=a;
#endif
    stopSlaves=false; startSlaves=false; pauseSlaves=false;
 //   GoTools::print("on sort de selfTrainMultiThread");
}



// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayer::selfTrain( const Goban & goban, const int number, const double time, int multiThreadsIdentifier) 
{
    selfTrainNew4(goban, number, time, multiThreadsIdentifier);
}
void MoGo::InnerMCGoPlayer::selfTrainNOMPI( const Goban & goban, const int number, const double time, int multiThreadsIdentifier) 
{
    selfTrainNew4NOMPI(goban, number, time, multiThreadsIdentifier);
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayer::selfTrainInitiation(const Goban &goban, NodeSequence &currentSequence,double time) {
    selfTrainAverageLength = 0;
    const GobanKey & gobanKey = goban.getGobanKey();
    simulationMoveSelector->setLastMoves(lastMoves);
    simulationMoveSelector->clearCacheHistory();

    for (int i=0;i<(int)nbThreads;i++) for (int j=0;j<(int)allAtFirstData[i*COEF_CACHE_MISS_InnerMCGoPlayer].size();j++) allAtFirstData[i*COEF_CACHE_MISS_InnerMCGoPlayer][j]=0;
    for (int i=0;i<(int)nbThreads;i++) for (int j=0;j<(int)allAtFirstDataInTree[i*COEF_CACHE_MISS_InnerMCGoPlayer].size();j++) allAtFirstDataInTree[i*COEF_CACHE_MISS_InnerMCGoPlayer][j]=0;


    if ((goban.height()!=GobanNode::height) || (!playerTree.isInTheTree(gobanKey))
        || (playerTree.getRootNodeAddress()!=playerTree.getGobanNodeAddress(gobanKey) && (!keepTreeIfPossible) )) {
        //          GoTools::print("Start with a new goban! Delete the tree before.\n");
        //CPUTimeChronometer cpuTimer;

        playerTree.reInitiate(gobanKey);
        //GoTools::print("tree reinitiation time: %6.2f seconds\n",cpuTimer.getTime());

        //FIXME here is the for a bug. Let me try to solve it.
        //GobanNode * node=playerTree.getGobanNodeAddress(gobanKey);
        //if (node->numberOfSimulation()==0) node->resetNumberOfSimulation(1);
        initiatePlayerGobanNode(goban,playerTree.getGobanNodeAddress(gobanKey), currentSequence);
        //     currentNode->nodeUrgency().textModeShowUrgency();
        //     getchar();
        deepestDepthOfTree=1;
    } else if (keepTreeIfPossible && (playerTree.getRootNodeAddress()!=playerTree.getGobanNodeAddress(gobanKey)) ) {
        // GoTools::print("selfTrainInitiation %i\n",(int)playerTree.getGobanNodeAddress(gobanKey));
        // GoTools::print(gobanKey);
        playerTree.reduceTreeToNewRoot(playerTree.getGobanNodeAddress(gobanKey));
        // GoTools::print("selfTrainInitiation %i, %i\n",(int)playerTree.getRootNodeAddress(),(int)playerTree.getGobanNodeAddress(gobanKey));
        deepestDepthOfTree=1;
        initiatePlayerGobanNode(goban,playerTree.getGobanNodeAddress(gobanKey), currentSequence);

        if (reAllowMovesOutsideBitmapAtRoot)
            reInitiateGobanBitmaps(goban);


    } else {
        if (reAllowMovesOutsideBitmapAtRoot)
            reInitiateGobanBitmaps(goban);


    }
    if (positionEvaluator &&
        positionEvaluator->isUseful(goban,time))
        //positionEvaluator->loadRootGoban(goban,goban.lastMove());
        positionEvaluator->loadRootGoban(goban,lastMoves);

    //GoTools::print("self train initiated. random mode %d\n",randomMode);
    //showGobanUrgency(goban);
}














Location MoGo::InnerMCGoPlayer::chooseOneMoveAtNode(GobanNode * node, const Goban &goban, const int depth ) {/**assert(0);**/
    //  return dynamic_cast<MoveSelectorGlobal *>(simulationMoveSelector)->selectOneMoveNew(node, goban, node);
    return simulationMoveSelector->selectOneMove(node, goban, depth);
    /*  if (node->numberOfSimulation()>playerGoban.height()*playerGoban.width()*2)
        return node->nodeUrgency().selectTheUrgentestMove();
        else
        return playerGoban.randomOutOneMove();*/
}



// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayer::initiatePlayerGobanNode( const Goban & currentGoban, GobanNode * node, const NodeSequence & currentSequence, bool /*lock*/ ) const {
    if (node->getInitiated()) return;

    //if (useNewTrainOneGame>=4) 
    {
        FastBoard::initiateOwnBoardForThread(currentGoban);
        initiatePlayerGobanNodeAsynch( dynamic_cast<MoveSelectorGlobal *>(simulationMoveSelector), dynamic_cast<UCTUpdatorGlobal *>(treeUpdator), currentGoban, *FastBoard::getBoardForThread(), node, currentSequence);
        //       initiatePlayerGobanNodeUrgencyAsynch(dynamic_cast<MoveSelectorGlobal *>(simulationMoveSelector), currentGoban, *FastBoard::getBoardForThread(), node, currentSequence);
    } 
    //  else 
    // TOBEREMOVED 
    //   {/**assert(0);**/
    //       if (lock) {/**assert(0);**/
    //         pthread_mutex_lock(&initiateMutex);
    //         if (node->getInitiated()) {/**assert(0);**/
    //           pthread_mutex_unlock(&initiateMutex);
    //           return;
    //         }
    //       }
    //       initiatePlayerGobanNodeUrgency(currentGoban, node, currentSequence);
    //       //GoTools::print("initiatePlayerGobanNodeUrgency OK\n");
    //       initiatePlayerGobanNodeValue(currentGoban, node);
    //       node->setInitiated(true);
    //       if (lock) pthread_mutex_unlock(&initiateMutex);
    //       //GoTools::print("initiatePlayerGobanNodeValue OK\n");
    //     }
}

// TOBEREMOVED
// void MoGo::InnerMCGoPlayer::initiatePlayerGobanNodeValue( const Goban & , GobanNode * node ) const {/**assert(0);**/
//   treeUpdator->nodeValueInitiation(node);
// }




bool MoGo::InnerMCGoPlayer::isPlayerGobanInCurrentSequence(const Goban &currentGoban, const NodeSequence &currentSequence) {/**assert(0);**/
    GobanNode *node=getNode(currentGoban, currentSequence);
    for (int i=0;i<(int)currentSequence.size();i++)
        if (node==currentSequence[i]) {/**assert(0);**/
            /*
              GoTools::print("playerGoban is the same as the %dth goban of the current sequence.\nplayerGoban:",i);
              playerGoban.textModeShowGoban();
              playerGoban.printGobanKey();
              GoTools::print("%dth goban",i);
              currentSequence[i]->showGoban();
              //getchar();
              */
            return true;
        }
    return false;
}













/** Here starts the display functions */


void MoGo::InnerMCGoPlayer::showCurrentSequence(const Goban &currentGoban, const NodeSequence &currentSequence) const {/**assert(0);**/
    /*
      #ifdef DEBUG_INNERMCGOPLAYER_ON
      GoTools::print("Show current sequcence as goban.\n");
      for (int i=0;i<(int)currentSequence.size();i++)
      currentSequence[i]->showGoban();
      #endif
    */
    GoTools::print("Show current sequence in details, length: %d.\nBeginning goban:",currentSequence.size());
    GobanNode * tmpNode=currentSequence[0];
    assert(currentSequence.size()>0);
    for (int i=1;i<(int)currentSequence.size();i++) {/**assert(0);**/
        int indexOfChild=-1;
        tmpNode->showNodeInformation();
        for (int j=0;j<(int)tmpNode->childNode().size();j++) {/**assert(0);**/
            if (currentSequence[i]==tmpNode->childNode()[j])
                indexOfChild=j;

        }
        if (indexOfChild==-1) {/**assert(0);**/
            GoTools::print("Goban of next node: ");
            currentSequence[i]->showGoban();
            assert(0);
        }
        GoTools::print ("\nmove %d: ",i);
        currentGoban.textModeShowPosition(tmpNode->childMove()[indexOfChild]);GoTools::print("\n");
        //    GoTools::print("Child goban by childNode:\n");
        //    tmpNode->childNode()[indexOfChild]->showGoban();
        //    GoTools::print("Child goban by current sequence:\n");
        //    currentSequence[i]->showGoban();
        tmpNode=currentSequence[i];
    }
    GoTools::print("End of current sequence show.\n");
}

void MoGo::InnerMCGoPlayer::showCurrentSequenceSimple(const NodeSequence &currentSequence) const {/**assert(0);**/
 
    MoGo::Goban tmpGoban(currentSequence[0]->getHeight(),currentSequence[0]->getWidth());
    for (int i=0;i<(int)currentSequence.size()-1;i++)
        for (int j=0;j<(int)currentSequence[i]->childNode().size();j++)
            if (currentSequence[i+1] == currentSequence[i]->childNode()[j]) {/**assert(0);**/
                tmpGoban.textModeShowPosition(currentSequence[i]->childMove()[j]);
                break;
            }

}

void MoGo::InnerMCGoPlayer::showCurrentSequenceBitmap(NodeSequence & currentSequence, const Vector < Location > &currentSequenceMove) const {/**assert(0);**/
    if ((int)currentSequence.size()<=2) return;
    Vector < Bitmap > currentSequenceBitmaps;
    for (int i=0;i<(int)currentSequence.size();i++) {/**assert(0);**/
        GobanLocation::textModeShowPosition(currentSequenceMove[i]);
        if (currentSequence[i]->isNodeBitmapCreated())
            currentSequenceBitmaps.push_back(currentSequence[i]->bitmap());
        else
            GoTools::print("node %d in currentSequence has no bitmap.\n",i);
    }
    GoTools::print("\n");
    GobanBitmap::showBitmap(currentSequenceBitmaps);
    getchar();
}

void MoGo::InnerMCGoPlayer::showGobanUrgency( const Goban & goban ) {/**assert(0);**/
    showGobanUrgency(goban, currentSequence);
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayer::showGobanUrgency( const Goban & goban, const NodeSequence &currentSequence ) {
    if (dontDisplay)
        return ;
    if (!playerTree.isInTheTree(goban.getGobanKey())) {/**assert(0);**/
        GoTools::print("showGobanUrgency: The root is not in the tree (bug). Tell Yizao please.\nOr, you are running the MCGoPlayer mode? Then this warning is normal but should not be very often.\n");
        return;
    }
    GobanNode* node=getNode(goban, currentSequence);
    //node->nodeUrgency().textModeShowUrgencyRange(goban);
    GobanUrgency tmpUrgency=node->nodeUrgency();
    node->needToUpdateUrgencyOn();

    if (dynamic_cast<MoveSelectorGlobal *>(simulationMoveSelector)) {
        Location location = node->indexOnArray(0,0);
        for (int i=0;i<GobanNode::height;i++, location+=2)
            for (int j=0;j<GobanNode::width;j++, location++) {
                node->nodeUrgency().resetUrgency(location, 2.+node->nodeUrgency()[location]/node->nodeValue()[location+3]);
            }
    }
    for (int i=0;i<(int)node->childNode().size();i++)
        if (node->nodeUrgency()[node->childMove()[i]] != 1.)  { //if no AMAF value ?
    
            node->nodeUrgency().resetUrgency(node->childMove()[i],1.-node->childNode()[i]->nodeValue()[1]/node->childNode()[i]->numberOfSimulation());
            if (node->nodeUrgency()[node->childMove()[i]] == 0.)
                node->nodeUrgency().resetUrgency(node->childMove()[i],0.0001);
            //if we had 0 here then it would be misleading in the goban range mode.
        } 
    //node->nodeUrgency().textModeShowUrgency();
    if (node->bitmap().size()>0) {/**assert(0);**/
        //     assert(node->bitmap().size()>0);
        Location location = node->indexOnArray(0,0);
        for (int i=0;i<GobanNode::height;i++, location+=2)
            for (int j=0;j<GobanNode::width;j++, location++) {/**assert(0);**/
                if (!GobanBitmap::getBitValue(node->bitmap(), i, j))
                    node->nodeUrgency().resetUrgency(location, 0.);
            }
    }

    node->nodeUrgency().textModeShowUrgencyRange(goban);




    if (dynamic_cast<UCTUpdatorGlobal *>(treeUpdator))
        if (dynamic_cast<UCTUpdatorGlobal *>(treeUpdator)->getUseEndTerritories()) {/**assert(0);**/
            UCTUpdatorGlobal *updator=dynamic_cast<UCTUpdatorGlobal *>(treeUpdator);
            if ( updator->getUseEndTerritories() && node->getStats().getNbStats()>0) {/**assert(0);**/
                Location location = node->indexOnArray(0,0);
                for (int i=0;i<GobanNode::height;i++, location+=2)
                    for (int j=0;j<GobanNode::width;j++, location++) {/**assert(0);**/
                        double tmp=node->getStats().getAverage(0,location)+0.5;
                        node->nodeUrgency().resetUrgency(location, tmp);
                    }
            }
            node->nodeUrgency().textModeShowUrgencyRange(goban);
        }
    node->nodeUrgency()=tmpUrgency;
}

void MoGo::InnerMCGoPlayer::showCurrentTree( CPUTimeChronometer & cpuTimer, const NodeSequence &currentSequence, int n ) const {/**assert(0);**/
    int nn=30;
    static int i=0;
    if (i%nn==0) {/**assert(0);**/
        double timeUsed=cpuTimer.getTime();
        GoTools::print("%d games trained!time used : %f\n",nn,timeUsed);
        showCurrentSequenceSimple(currentSequence);
        playerTree.dottyOut(n);
        //getchar();
        cpuTimer.restart();
    }
    i++;
}



void MoGo::InnerMCGoPlayer::showHashMapSize( ) const {/**assert(0);**/
    playerTree.showHashMapSize();
}
void MoGo::InnerMCGoPlayer::showPreferedSequence( const Goban & goban, const Location firstMove, const int number ) {/**assert(0);**/
    showPreferedSequence(goban, currentSequence, firstMove, number);
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayer::showPreferedSequence( const Goban & goban, NodeSequence &currentSequence, const Location firstMove ,const int /*number*/ ) {

    if (dontDisplay)
        return;
#ifdef PARALLELIZED
    lockSafer();
#endif
    GobanNode * node=getNode(goban, currentSequence);
    //assert(node!=NULL);
    if (node==NULL) {/**assert(0);**/
        GoTools::print("goban not in innerMCGoPlayer.\n");
#ifdef PARALLELIZED
        unlockSafer();
#endif
        return;
    }
    //showPreferedMoves(goban,number);
    Location location = PASSMOVE;
    if (firstMove == -1)
        location=safeSelectOneMove(playMoveSelector, node, goban, currentSequence, true);
    else {/**assert(0);**/
        location = firstMove;
    }

    //      Location location=playMoveSelector->selectOneMove(node,goban);
    int index=-1;

    Vector < Location > firstSimulatedMoves = simulationMoveSelector->preferedMoves(node,goban,5);
    int number0 = 0, number1 = 0;
    GobanNode *bestNode=0;
    for (int i=0;i<(int)node->childMove().size();i++) {
        if (node->childMove()[i]==location)
            index=i;
        if (firstSimulatedMoves.size()>0 && node->childMove()[i] == firstSimulatedMoves[0]) {
            bestNode=node->childNode()[i];
            number0 = (int)bestNode->numberOfSimulation();
        } else if (firstSimulatedMoves.size()>1 && node->childMove()[i] == firstSimulatedMoves[1])
            number1 = (int)node->childNode()[i]->numberOfSimulation();
    }
    int nbBestRefutations=0; double refutationValue=0.;
    if  (bestNode) {
        for (int i=0;i<(int)bestNode->childNode().size();i++)
            if (bestNode->childNode()[i]->numberOfSimulation()>nbBestRefutations) {
                nbBestRefutations=(int)bestNode->childNode()[i]->numberOfSimulation();
                refutationValue=1.-bestNode->childNode()[i]->nodeValue()[1]/double(nbBestRefutations);
            }
    }

    if (number0 > 0) firstSecondRatio = number1/(double)number0;
    else firstSecondRatio = 10.;
    //ratio = number1/(double)node->numberOfSimulation()*100.;
    // GoTools::print("n1 %i, n2 %i\n",number0,number1);
    double winningRate = 0.;
    if (goban.isBlacksTurn()) winningRate = selfTrainCounter/5000.*100;
    else winningRate = 100-selfTrainCounter/50.;


#ifdef MOGOMPI
	GoTools::print("Proc %d || ",MPI_NUMERO);
#endif

    if (index!=-1) {
/*    if (1-playMoveSelector->valueOfLocationForDisplay(node,location)>0.8)
      goban.weAreTheBest=true;
      else
      goban.weAreTheBest=false;*/
        GoTools::print("%f(%2.f%%) || %6d/%6d(%2.f%%)(%2.f%%)(%2.f%%/%0.2f) ||",
                       1-playMoveSelector->valueOfLocationForDisplay(node,location),
                       winningRate,
                       int(node->childNode()[index]->numberOfSimulation()),
                       int(node->numberOfSimulation()),
                       node->childNode()[index]->numberOfSimulation()/(double)node->numberOfSimulation()*100.,
                       firstSecondRatio*100., double(nbBestRefutations)/node->childNode()[index]->numberOfSimulation()*100., refutationValue);
    } else {/**assert(0);**/
        goban.textModeShowPosition(location);
        GoTools::print("is not in the root node. No sequence prefered.\n");
#ifdef PARALLELIZED
        unlockSafer();
#endif
        return;
    }

    GoTools::print("%6d |%2d || ",playerTree.getSharedChildrenNumber(),deepestDepthOfTree);
    //GoTools::print("%2d || ",deepestDepthOfTree);


    int j=0;

	

    while(1) {
        goban.textModeShowPosition(location);

        int index=-1;
        for (int i=0;i<(int)node->childMove().size();i++)
            if (node->childMove()[i]==location) {
                node=node->childNode()[i];
                index=0;
                break;
            }
        if (index==-1) break;
        j++;
        if (node->childNode().size()==0 || j>=10) break;

        // node->nodeUrgency().textModeShowUrgency();
        if (goban.isGobanOutsideStrange()) {/**assert(0);**/
            Goban tmpGoban(goban);
            tmpGoban.reloadByGobanKey(node->gobanKey(),true);
            location=safeSelectOneMove(playMoveSelector, node, tmpGoban, currentSequence, true);
        } else
            location=safeSelectOneMove(playMoveSelector, node, Goban(node->gobanKey()), currentSequence, true);
    }

    for (;j<10;j++) {
        GoTools::print("   ");
        if (goban.height()>9) GoTools::print(" ");
    }
    //showPreferedMoves(goban,5);

    node = playerTree.getGobanNodeAddress(goban);
    GoTools::print("||SSP:");
    goban.showMoves(firstSimulatedMoves);
    GoTools::print("||PSP:");
    goban.showMoves(playMoveSelector->preferedMoves(node,goban,5));

    GoTools::print("\n");
#ifdef PARALLELIZED
    unlockSafer();
#endif

}

void MoGo::InnerMCGoPlayer::showPreferedMoves( const Goban & goban, const int number ) {/**assert(0);**/
    GobanNode * node = playerTree.getGobanNodeAddress(goban);
    GoTools::print("||SSP:");
    goban.showMoves(simulationMoveSelector->preferedMoves(node,goban,number));
    GoTools::print("||PSP:");
    goban.showMoves(playMoveSelector->preferedMoves(node,goban,number));
}

// TEMPORARY COMMENT: GOGUI
// TEMPORARY COMMENT: THIS FUNCTION IS USED
std::string MoGo::InnerMCGoPlayer::getName( ) const {
    return "MoGo";
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayer::setKomi( const double komi ) {
    // GoTools::print("setKomi %f\n", komi);
    this->komi=komi;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
double MoGo::InnerMCGoPlayer::isInterestingToContinue( const Goban & goban, NodeSequence &currentSequence ) {
    //   return 0.5;
    //ATTENTION, here the value returned is the opponent's value.
    // GoTools::print(goban.getGobanKey());

    GobanNode * node=getNode(goban, currentSequence);

    //assert(node!=NULL);
    if (node==NULL) {/**assert(0);**/
        GoTools::print("--------------------------------BUG!!!--------------------------\nbug found at isInterestingToContinue. normally the goban should be the root of the tree but obviously InnerMCGoPlayer did not find it.\ntell yizao please.\ncurrent goban:\n");
        goban.textModeShowGoban();
        GoTools::print("root goban in the tree:\n");
        playerTree.getRootNodeAddress()->showGoban();
        return 0.5;
    }
    if (node->numberOfSimulation()<minNumberOfSimulationsOfResign /*||
                                                                    (node->numberOfSimulation()<2000 && goban.height() == 19)*/) {GoTools::print("%d < %d\n",node->numberOfSimulation(),minNumberOfSimulationsOfResign);return 0.5;}




    //showPreferedMoves(goban,number);
    //   Location location=playMoveSelector->selectOneMove(node,goban);
    Location location=safeSelectOneMove(playMoveSelector, node, goban, currentSequence);

GoTools::print("location chosen for estimation:%d\n",location);
    int index=-1;
    for (int i=0;i<(int)node->childMove().size();i++)
        if (node->childMove()[i]==location) {
            index=i;
            break;
        }

    if (index!=-1 && node->childNode()[index]->nodeValue().size()>=2) {
	    if (node->childNode()[index]->numberOfSimulation()<10) return 0.5;
        double value = node->childNode()[index]->nodeValue()[1]/node->childNode()[index]->numberOfSimulation();
        return value;
    }


    return 0.5;
}

int MoGo::InnerMCGoPlayer::numberOfSharedNodes( ) const {/**assert(0);**/
    return playerTree.sharedNumber();
}

Vector< MoGo::GobanNode * > MoGo::InnerMCGoPlayer::sharedNodes( ) {/**assert(0);**/
    return playerTree.sharedNodes();
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::InnerMCGoPlayer::hasResigned( ) const {
    return hasResigned_;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayer::setCorrelationMode( const bool modeOn ) {
    correlationModeOn=modeOn;
}

void MoGo::InnerMCGoPlayer::setPlayerForFastFinishingGame(GoPlayer * playerForFastFinishingGame) {/**assert(0);**/
    this->playerForFastFinishingGame = playerForFastFinishingGame;
}

void MoGo::InnerMCGoPlayer::setPositionEvaluator( ScoreEvaluator * positionEvaluator, bool deletePositionEvaluator ) {/**assert(0);**/
    this->positionEvaluator = positionEvaluator;
    this->deletePositionEvaluator = deletePositionEvaluator;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayer::setNbSimulationsForEachNode( int nbSimulationsForEachNode ) {
    this->nbSimulationsForEachNode = nbSimulationsForEachNode;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayer::setNoiseLevel( double noiseLevel ) {
    this->noiseLevel=noiseLevel;
}

void MoGo::InnerMCGoPlayer::setSimulationRange( Goban & goban ) {/**assert(0);**/
    Vector < Location > simulationRange;
    Location location = goban.leftUpLocation()+2;
    for (int i=2;i<goban.height()-2;i++,location+=6)
        for (int j=2;j<goban.width()-2;j++,location++)
            simulationRange.push_back(location);
    //simulationRange.push_back(PASSMOVE);
    goban.setSimulationRange(simulationRange);
    //goban.showFreeLocation();
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayer::setOpeningMode( int mode ) {
    openingMode = mode;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayer::setNbTotalSimulations( int nbTotalSimulations ) {
    this->nbTotalSimulations=nbTotalSimulations;
}


// TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::InnerMCGoPlayer::isSuperKo( const Goban & goban, NodeSequence *currentSequence ) const {
    GobanKey gobanKey = goban.getGobanKey();
    gobanKey[0]-=gobanKey[0]%(1024*1024)/(1024*512)*(1024*512);
    gobanKey[0]-=gobanKey[0]%(1024*512)/1024*1024;
    if (historyGoban.count(gobanKey)>0)
        return true;

    /*  if (currentSequence)
        for (int i=0;i<(int)currentSequence->size()-1;i++)
        if ((*currentSequence)[i]->superKoGobanKey() == gobanKey) return true;
    */
    if (currentSequence)
        for (int i=0;i<(int)currentSequence->size()-1;i++)
            if (currentSequence->getSuperKoKey(i) == gobanKey) return true;



    return false;
}

double MoGo::InnerMCGoPlayer::getKomi( ) const {/**assert(0);**/
    return komi;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
MoGo::GobanNode * MoGo::InnerMCGoPlayer::getNode( const Goban & goban, const NodeSequence &currentSequence, bool testForInitiate, bool /*lockUnlock*/ ) const {
#ifdef PARALLELIZED
    lock();
    //   if (useNewTrainOneGame==1) pthread_mutex_lock(&mutexProtectNodes);

#endif
    GobanNode *node=playerTree.getGobanNodeAddress(goban);
    assert(node);

#if 0
#ifdef PARALLELIZED
    if (unlockChooseMove) {/**assert(0);**/
        if (lockUnlock)
            unlockSafer();
        protectedNode(node);
    }
#endif
#endif
    if (testForInitiate && !node->getInitiated())
        initiatePlayerGobanNode(goban, node, currentSequence);
#if 0
#ifdef PARALLELIZED
    if (unlockChooseMove) {/**assert(0);**/
        unProtectedNode(node);
        if (lockUnlock)
            lockSafer();
    }
#endif
#endif

#ifdef PARALLELIZED
    unlock();
    //   if (useNewTrainOneGame==1) pthread_mutex_unlock(&mutexProtectNodes);
#endif
    return node;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayer::setKeepTreeIfPossible( bool keepTreeIfPossible ) {
    this->keepTreeIfPossible=keepTreeIfPossible;
}


// TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::InnerMCGoPlayer::isTreeAtSizeLimit( ) const {
    return (int)playerTree.hashMapSize()>=limitTreeSize;      // tree too big, can't keep it in the memory
}

Location MoGo::InnerMCGoPlayer::getMoveFromCurrentSequence( const NodeSequence & , int n, const Vector < Location > &currentSequenceMove ) const {/**assert(0);**/
    assert(n<(int)currentSequenceMove.size());
    return currentSequenceMove[n];
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
Location MoGo::InnerMCGoPlayer::safeSelectOneMove( MoveSelector * selector, GobanNode *node, const Goban & goban, NodeSequence & currentSequence, bool considerPassAsNormal ) {
    Location location=PASSMOVE; int securityCounter=0;
    for ( ; securityCounter < 21*21 ; securityCounter++) {
        Goban tmpGoban=goban;
        location=selector->selectOneMove(node, tmpGoban, considerPassAsNormal?-123:0);
        if (!tmpGoban.isLegal(location)) {/**assert(0);**/
            GoTools::print("goban %d:\n",goban.isGobanOutsideStrange());
            goban.textModeShowGoban();
            GoTools::print("tmp goban %d:\n",tmpGoban.isGobanOutsideStrange());
            tmpGoban.textModeShowGoban();
            tmpGoban.textModeShowPosition(location);
            showGobanUrgency(tmpGoban, currentSequence);
            GoTools::print("\n");
            //assert(0);
        }
        tmpGoban.playMoveOnGoban(location);
        if (location != PASSMOVE && isSuperKo(tmpGoban, &currentSequence)) {/**assert(0);**/
            selector->addForbiddenMove(location);
        } else
            break;
    }
    if (securityCounter >= 21*21) {/**assert(0);**/
        GoTools::print("An infinite loop has occurred (in safeSelectOneMove) !!\n");
        playMoveSelector->showForbiddenMoves();
        goban.textModeShowGoban();
        selector->clearForbiddenMoves();
        return PASSMOVE;
    }
    selector->clearForbiddenMoves();
    return location;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayer::setGobanSize( int gobanSize ) {
    if (this->gobanSize != gobanSize) {/**assert(0);**/
        playerTree.reInitiate(Goban(gobanSize,gobanSize));

        this->gobanSize = gobanSize;
    }
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayer::setLimitTreeSize( int treeSize ) {
    if (treeSize <= 4000000)
        MoGo::InnerMCGoPlayer::limitTreeSize = treeSize;
    else
        GoTools::print("Tree Size too big. Failed.\n");
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayer::setBlockMode( int mode ) {
    blockMode = mode;
}




void MoGo::InnerMCGoPlayer::getBitmapByCombiningCurrentSequenceBitmap( Bitmap & bitmap, NodeSequence & currentSequence, const Vector < Location > &currentSequenceMove ) const {/**assert(0);**/
    GobanBitmap::clear(bitmap);
    Vector < Bitmap > currentSequenceBitmaps;
    for (int i=0;i<(int)currentSequence.size();i++) {/**assert(0);**/
        if (currentSequence[i]->isNodeBitmapCreated())
            GobanBitmap::logicalOR(bitmap,currentSequence[i]->bitmap());
        else {/**assert(0);**/
            //GoTools::print("node %d in currentSequence has no bitmap.\n",i);
            if (i == (int)currentSequence.size()-1)
                GobanBitmap::fullfill(bitmap);
        }
        if (GobanLocation::height == 13 && GobanLocation::width == 13)
            GobanBitmap::logicalOR(bitmap,GobanBitmap::zoneBitmap(GobanBitmap::getZoneBitmapNumber(currentSequenceMove[i])));
    }
}







































// TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::InnerMCGoPlayer::imitateMove( const Goban & goban, Location & location ) {
    if (imitateMode && goban.moveNumber()<2) stillImitating = true;
    if (imitateMode == 0 ||
        !stillImitating ||
        goban.moveNumber()>imitateMoveNumber ) return false;
    GoTools::print("imitateMove tried. (%d,%d)\n",goban.moveNumber(),imitateMoveNumber);
    if (goban.moveNumber() == 0) {/**assert(0);**/
        location = goban.indexOnArray(goban.height()/2,goban.width()/2);
        return true;
    }
    int x = GobanLocation::xLocation(goban.lastMove());
    int y = GobanLocation::yLocation(goban.lastMove());
    x = goban.height()-1-x;
    y = goban.width()-1-y;
    goban.textModeShowPosition(goban.indexOnArray(x,y));
    GoTools::print("\n");
    if (!goban.isLegal(GobanLocation::locationXY(x,y))) {/**assert(0);**/
        stillImitating = false;
        return false;
    }
    if (imitateMode == 1) {/**assert(0);**/
        location = GobanLocation::locationXY(x,y);
        return true;
    }
    if (imitateMode == 2) {/**assert(0);**/
        int x1 = x+GoTools::mogoRand(3)-1;
        int y1 = y+GoTools::mogoRand(3)-1;
        if (goban.isLegal(GobanLocation::locationXY(x1,y1)))
            location = GobanLocation::locationXY(x1,y1);
        else location = GobanLocation::locationXY(x,y);
        return true;
    }
    stillImitating = false;
    return false;

}

void MoGo::InnerMCGoPlayer::setImitateMode( int mode, int moveNumber ) {/**assert(0);**/
    imitateMode = mode;
    imitateMoveNumber = moveNumber;
}





bool MoGo::InnerMCGoPlayer::getInfosOnNode( const GobanKey & key, double & value, int & nbSimulations ) {/**assert(0);**/
    if (!playerTree.isInTheTree(key))
        return false;

    GobanNode *node=playerTree.getGobanNodeAddress(key);
    value=node->nodeValue()[1];
    nbSimulations=(int)node->numberOfSimulation();

    return true;
}




// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayer::lockSafer( ) const {
#ifdef DEBUG_PARALLELIZED_INNER_ON
    GoTools::print("enter lockSafer()\n");
#endif
    while (pthread_mutex_trylock(&saferMutex));
    //   pthread_mutex_lock(&saferMutex);
    saferBlockVariable=true;
#ifdef DEBUG_PARALLELIZED_INNER_ON
    GoTools::print("exit lockSafer() \n");
#endif

}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayer::unlockSafer( ) const {
#ifdef DEBUG_PARALLELIZED_INNER_ON
    GoTools::print("enter unlockSafer()\n");
#endif
    saferBlockVariable=false;
    pthread_mutex_unlock(&saferMutex);
#ifdef DEBUG_PARALLELIZED_INNER_ON
    GoTools::print("exit unlockSafer()\n");
#endif

}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayer::lock( ) const {
#ifdef DEBUG_PARALLELIZED_INNER_ON
    GoTools::print("enter lock()\n");
#endif
#ifdef DEBUG_PARALLELIZED_INNER_ON
    GoTools::print("exit lock()\n");
#endif

}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayer::unlock( ) const {
#ifdef DEBUG_PARALLELIZED_INNER_ON
    GoTools::print("enter unlock()\n");
#endif
    //   blockVariable=false;
    //    pthread_mutex_unlock(&saferMutex);
#ifdef DEBUG_PARALLELIZED_INNER_ON
    GoTools::print("exit unlock()\n");
#endif

}

int MoGo::InnerMCGoPlayer::getRandomMode( ) const {/**assert(0);**/
    return randomMode;
}

void MoGo::InnerMCGoPlayer::forgetTraining( ) {/**assert(0);**/
    playerTree.deleteTree();
}


// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayer::cutTree( ) {
    isAThreadInCutTree=true;

#ifdef PARALLELIZED
    lockCutTree();
    lockSafer();

#endif
    //   if ((collectorLimitTreeSize>0)&&((int)playerTree.hashMapSize()>=collectorLimitTreeSize))
    playerTree.cutTree(playerTree.hashMapSize()*7/8+100);

    if (useNewTrainOneGame>=3) clearStacks();

#ifdef PARALLELIZED
    unlockSafer();
    unlockCutTree();
#endif
    isAThreadInCutTree=false;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayer::lockOneGameSafer(int multiThreadsIdentifier) const {
#if 0
    //   while (pthread_mutex_trylock(&saferOneGameMutex));
    pthread_mutex_lock(&saferOneGameMutex);
    while (pauseSlaves) {/**assert(0);**/
        pthread_mutex_unlock(&saferOneGameMutex);
#ifdef LINUX
        while(pauseSlaves) {/**assert(0);**/
            //       if (!thread_mutex_trylock(&saferOneGameMutex)) pthread_mutex_unlock(&saferOneGameMutex);
            pthread_yield();
            // static int a=0; for (int i=0;i<1000000;i++) a++;
        }
#else
        while(pauseSlaves);
#endif
        pthread_mutex_lock(&saferOneGameMutex);
    }
    useOneGameLocker++;
    pthread_mutex_unlock(&saferOneGameMutex);
#else
    //   pthread_mutex_lock(&workerMutexes[multiThreadsIdentifier]);
    while (pthread_mutex_trylock(&workerMutexes[multiThreadsIdentifier]));
#endif
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayer::unlockOneGameSafer(int multiThreadsIdentifier) const {
#if 0
    //   while (pthread_mutex_trylock(&saferOneGameMutex));
    pthread_mutex_lock(&saferOneGameMutex);
    useOneGameLocker--; assert(useOneGameLocker>=0);
    pthread_mutex_unlock(&saferOneGameMutex);
#else
    pthread_mutex_unlock(&workerMutexes[multiThreadsIdentifier]);
#endif
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayer::lockCutTree( ) const {
#if 0
    while (true) {/**assert(0);**/
        //     while (pthread_mutex_trylock(&saferOneGameMutex));
        pthread_mutex_lock(&saferOneGameMutex);
        pauseSlaves=true;
        if (useOneGameLocker==0)
            break;
        pthread_mutex_unlock(&saferOneGameMutex);
    }
#else
    Vector<bool> locked(NUM_THREADS_MAX, false);
    bool cont=true;
    while (cont) {
        for (int i=0;i<NUM_THREADS_MAX;i++) {
            if (locked[i]==false && !pthread_mutex_trylock(&workerMutexes[i])) locked[i]=true;
        }
        cont=false; for (int i=0;i<NUM_THREADS_MAX;i++) if (!locked[i]) cont=true;
    }
#endif
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayer::unlockCutTree( ) const {
#if 0
    pauseSlaves=false;
    pthread_mutex_unlock(&saferOneGameMutex);
#else
    for (int i=0;i<NUM_THREADS_MAX;i++) pthread_mutex_unlock(&workerMutexes[i]);
#endif
}








double MoGo::InnerMCGoPlayer::evaluatePosition( const Goban & goban, int & nbSimulations ) {/**assert(0);**/
#ifdef PARALLELIZED
    lockSafer();
#endif
    GobanNode * node = playerTree.getGobanNodeAddress(goban);
    if (node==NULL) {/**assert(0);**/
        nbSimulations=0;
#ifdef PARALLELIZED
        unlockSafer();
#endif
        return 0.;
    }
    nbSimulations=(int)node->numberOfSimulation();
    double value=0.5;
    if (nbSimulations>0.)
        value=node->nodeValue()[1]/node->numberOfSimulation();
    if (!goban.isBlacksTurn())
        value=1.-value;
#ifdef PARALLELIZED
    unlockSafer();
#endif
    return value;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayer::setAutoKomi( int autoKomiMode, double winLimitIncreaseKomi, double winLimitDecreaseKomi, int autoKomiMin, int autoKomiMax, int iterationsBeforeModifyKomi) {
    this->autoKomiMode=autoKomiMode;
    this->winLimitIncreaseKomi=winLimitIncreaseKomi;
    this->winLimitDecreaseKomi=winLimitDecreaseKomi;
    this->autoKomiMin=autoKomiMin;
    this->autoKomiMax=autoKomiMax;
    this->iterationsBeforeModifyKomi=iterationsBeforeModifyKomi;
}





// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayer::updateAutoKomi( const Goban & goban, double score ) {
    if (autoKomiMode!=1) return;
    double pureScore=score-komi;
    if (!goban.isBlacksTurn()) pureScore=-pureScore;

    scoreDistributionNb++;
    if (pureScore<double(autoKomiMin)) scoreDistributionSum[0]++;
    else if (pureScore>=double(autoKomiMax)) scoreDistributionSum[scoreDistributionSum.size()-1]++;
    else {/**assert(0);**/
        int c=1;
        for (int v=autoKomiMin+1;v<=autoKomiMax;v++,c++) {/**assert(0);**/
            assert(c<(int)scoreDistributionSum.size());
            if (pureScore<double(v)) {/**assert(0);**/
                scoreDistributionSum[c]++;
                break;
            }
        }
        assert(c<(int)scoreDistributionSum.size()-1);
    } // we have the distribution

    iterationsModifyKomi++;
    if ((iterationsModifyKomi%iterationsBeforeModifyKomi)==0) {/**assert(0);**/
        double nb=scoreDistributionNb;
        double nbHigher=scoreDistributionSum[scoreDistributionSum.size()-1];
        if (nbHigher/nb>winLimitIncreaseKomi)
            autoKomiModifier=autoKomiMax;
        else {/**assert(0);**/
            int c=scoreDistributionSum.size()-2;
            for (int v=autoKomiMax-1;c>=0;v--,c--) {/**assert(0);**/
                nbHigher+=scoreDistributionSum[c];
                if (nbHigher/nb>winLimitIncreaseKomi) {/**assert(0);**/
                    autoKomiModifier=v;
                    break;
                } else if (v<=0) {/**assert(0);**/ // we are considering loosing...
                    if (nbHigher/nb>winLimitDecreaseKomi) {/**assert(0);**/
                        autoKomiModifier=v;
                        break;
                    }
                }
            }
            if (c==0) autoKomiModifier=autoKomiMin;
        }
        if (!goban.isBlacksTurn())
            autoKomiModifier=-autoKomiModifier;
    }


    if ((iterationsModifyKomi%(iterationsBeforeModifyKomi*50))==0) {/**assert(0);**/
        Vector<double> distrib(scoreDistributionSum.size());
        for (int i=0;i<(int)distrib.size();i++) distrib[i]=double(scoreDistributionSum[i])/double(scoreDistributionNb);
        GoTools::print(distrib);
        GoTools::print("autoKomiModifier %f (%i)\n", autoKomiModifier, iterationsModifyKomi);
    }
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayer::initiateKomis( const Goban & goban ) {
    komiModifier=(addKomiMax-addKomiMin)*pow((double(goban.approximateSizeOfPossibleMoves())/double(goban.height()*goban.width())),addKomiAlpha)+addKomiMin;
    if (!goban.isBlacksTurn()) komiModifier=-komiModifier;
    if (komiModifier!=0.) GoTools::print("komiModifier : %f\n", komiModifier);

    autoKomiModifier=0.;
    if (autoKomiMode==1) {/**assert(0);**/
        scoreDistributionSum.clear(); scoreDistributionNb=0.;
        scoreDistributionSum.resize(autoKomiMax-autoKomiMin+2);
        iterationsModifyKomi=0;
    }
    checkSuperKoByLoop=0;
    lastNbSimulationsUsed=0;
}



Location MoGo::InnerMCGoPlayer::startSimulation( Goban & currentGoban, GobanNode *&currentNode, int &counter ) {/**assert(0);**/
    if (currentGoban.approximateSizeOfPossibleMoves()<15)
        return -1;

    if (startSimulationMode>0) {/**assert(0);**/

        if (currentNode==0 || currentNode->numberOfSimulation()<startSimulationNbSimulations) {/**assert(0);**/
            counter++;
            currentNode=playerTree.getRootNodeAddress();
        }
        //    currentGoban.textModeShowGoban();
        //    GoTools::print("counter %i, startSimulationMode %i, startSimulationNbSimulations %i, currentNode nb sim %i\n", counter, startSimulationMode, startSimulationNbSimulations, currentNode->numberOfSimulation());
        //    getchar();
        if (counter>startSimulationMode || playerTree.getRootNodeAddress()->numberOfSimulation()<startSimulationNbSimulations)
            return -1;
        if (currentNode->numberOfSimulation()<startSimulationNbSimulations || currentNode->childNode().size()==0) {/**assert(0);**/
            currentNode=0;
            return PASSMOVE;
        }
        assert(currentNode->childNode().size()>0);
        int index=GoTools::mogoRand(GoTools::min(startSimulationNbNodes,currentNode->childNode().size()));
        Location location=currentNode->childMove()[index];
        currentNode=currentNode->childNode()[index];

        //currentGoban.textModeShowGoban();
        //      GoTools::print("index %i, location %i\n", index, location);

        if (location!=PASSMOVE
            && currentGoban.isBlacksTurn()^(currentNode->isBlacksTurn())
            && currentGoban.isNotUseless(location)
            && (!currentGoban.isSelfAtari(location))) {/**assert(0);**/
            //     currentGoban.textModeShowGoban();
            //     GoTools::print("play "); currentGoban.textModeShowPosition(location);
            //     getchar();
            currentGoban.playMoveOnGoban(location);
            return location;
        } else
            return PASSMOVE;
    } else {/**assert(0);**/
        assert(startSimulationMode<0);
        int startSimulationModeTmp=-startSimulationMode;
        //        counter++;
        //     GoTools::print("(counter %i)\n", counter);

        if (currentNode==0 || currentNode->numberOfSimulation()<startSimulationNbSimulations) {/**assert(0);**/
            counter++;
            currentNode=playerTree.getRootNodeAddress();
        }
        //    currentGoban.textModeShowGoban();
        //    GoTools::print("counter %i, startSimulationMode %i, startSimulationNbSimulations %i, currentNode nb sim %i\n", counter, startSimulationMode, startSimulationNbSimulations, currentNode->numberOfSimulation());
        //    getchar();
        if (counter>startSimulationModeTmp || playerTree.getRootNodeAddress()->numberOfSimulation()<startSimulationNbSimulations)
            return -1;
        //     GoTools::print("(RETURN %i)\n", counter);
        if (currentNode->numberOfSimulation()<startSimulationNbSimulations || currentNode->childNode().size()==0) {/**assert(0);**/
            currentNode=0;
            return PASSMOVE;
        }
        assert(currentNode->childNode().size()>0);
        if (currentGoban.isBlacksTurn()^(currentNode->isBlacksTurn()))
            return PASSMOVE;

        //     for (int i=0 ; i<(int)currentNode->childNode().size(); i++)
        //       GoTools::print("(%i)=nbs(%i) ", i, currentNode->childNode()[i]->numberOfSimulation());
        //     GoTools::print("\n");


        Location location=PASSMOVE;
        for (int index=0 ; (index < startSimulationNbNodes) && (index<(int)currentNode->childNode().size()) && (currentNode->childNode()[index]->numberOfSimulation()>=startSimulationNbSimulations) ; index++) {/**assert(0);**/
            if (GoTools::mogoRand(1000)<500) continue; //FIXME
            Location tmp=currentNode->childMove()[index];
            if (currentGoban.isNotUseless(tmp) && (!currentGoban.isSelfAtari(tmp))) {/**assert(0);**/
                location=tmp;
                currentNode=currentNode->childNode()[index];
                break;
            }
        }
        if (location==PASSMOVE) {/**assert(0);**/
            currentNode=0;
            return PASSMOVE;
        }
        /* currentGoban.textModeShowGoban();
           Goban(currentNode->gobanKey()).textModeShowGoban();
           GoTools::print("location %i (nbS %i) (counter %i)\n", location, currentNode->numberOfSimulation(),counter);
           currentGoban.textModeShowPosition(location);
           getchar();*/
        assert(currentGoban.isLegal(location));
        currentGoban.playMoveOnGoban(location);
        return location;
    }
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayer::initiateStaticAnalysisOfBoard( const Goban & goban ) const {
    if (initiateStaticAnalysisOfBoardMode==0)
        return;

    assert(0);
}

Vector < int > MoGo::InnerMCGoPlayer::getNbSimulationsAtRoot( ) const {/**assert(0);**/
    GobanNode *root=playerTree.getRootNodeAddress();
    assert(root);
    Vector < int >res((GobanNode::height+2)*(GobanNode::width+2),0);

    for (int i=0;i<(int)root->childNode().size();i++)
        res[root->childMove()[i]]=(int)root->childNode()[i]->numberOfSimulation();

    return res;
}

void MoGo::InnerMCGoPlayer::finish( double score) {/**assert(0);**/
    if (earlyCut>0) {/**assert(0);**/
        GoTools::print("game finished (score %f)\n", score);
        GoTools::print("time used %4.2f, nbSimulations %i, nbMoves %i\n", totalThinkingTime, nbSimulationUsed, nbThoughtMoves);
        GoTools::print("instead of: time used %4.2f (%4.2f\%), nbSimulations %i (%4.2f\%)\n", totalWantedThinkingTime, totalThinkingTime/totalWantedThinkingTime*100., nbWantedSimulations, double(nbSimulationUsed)/double(nbWantedSimulations)*100.);
    }
}


// TEMPORARY COMMENT: GOGUI
// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayer::start( const Goban & goban, const Location lastOtherPlayerMove ) {
    setGobanSize(goban.height());
    GoPlayer::start(goban, lastOtherPlayerMove);

    totalThinkingTime=0.;
    nbSimulationUsed=0;
    nbThoughtMoves=0;
    totalWantedThinkingTime=0.;
    nbWantedSimulations=0;




}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::InnerMCGoPlayer::stopThinking( const CPUTimeChronometer & cpuTimer, double simulationTime, int iteration, int nbTotalSimulations ) const {
    bool earlyCutPossible=false; bool earlyHardCutPossible=false; bool earlyVeryHardCutPossible=false;
    int max1=0, max2=0, sum=0;
    if (earlyCut>0 && (playerTree.getRootNodeAddress()->numberOfSimulation()-nbSimulationsAtRoot>=earlyCut)) {
        double timeRatio=0.; if (nbTotalSimulations<0) timeRatio=double(cpuTimer.getTime())/simulationTime; else timeRatio=double(iteration)/double(nbTotalSimulations);
        GobanNode *node=playerTree.getRootNodeAddress();
        for (int i=0;i<(int)node->childNode().size();i++) {
            int n = (int)node->childNode()[i]->numberOfSimulation();
            if (n>max1) {
                max2=max1;
                max1=n;
            } else if (n>max2) max2=n;
            sum+=n;
        }
        if (double(max1)/double(sum+1)>earlyCutRatioFirstMove) // the most visited node takes more than x% of simulations
            if (double(max1)/double(max2+1)>earlyCutRatioVSSecondMove) {/**assert(0);**/ // the most visited node takes more than y times the simulations of the second
                earlyCutPossible=true;
                if (double(max1)/double(sum+1)>1.-(1.-earlyCutRatioFirstMove)/2.) // the most visited node takes more than x% of simulations
                    if (double(max1)/double(max2+1)>earlyCutRatioVSSecondMove*3.) // the most visited node takes more than y times the simulations of the second
                        earlyHardCutPossible=true;
            }
        double speedRatio=double(sum-nbSimulationsAtRoot)/timeRatio;
        if ((1.-timeRatio)*speedRatio*1.05<double(max1-max2))
            earlyVeryHardCutPossible=true;
    }


    if (nbTotalSimulations < 0) {
        if (cpuTimer.getTime()>simulationTime)
            return true;
        if ((earlyVeryHardCutPossible || (earlyCutPossible && cpuTimer.getTime()>simulationTime/2.) || (earlyHardCutPossible && cpuTimer.getTime()>simulationTime/4.))) {
            if (!dontDisplay)
                GoTools::print("earlyCut : max1 %i; max2 %i; sum %i  max1/sum %f ; max1/max2 %f (hard %i)\n", max1, max2, sum, double(max1)/double(sum+1), double(max1)/double(max2+1), earlyHardCutPossible);
            return true;
        }
    } else {/**assert(0);**/
        if (iteration > nbTotalSimulations)
            return true;
        if (earlyVeryHardCutPossible || (earlyCutPossible && iteration>nbTotalSimulations/2) || (earlyHardCutPossible && iteration>nbTotalSimulations/4))
            return true;
    }
    return false;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayer::registerStopThinking( const CPUTimeChronometer & cpuTimer, double simulationTime, int iteration, int nbTotalSimulations ) {
    totalThinkingTime+=cpuTimer.getTime();
    nbSimulationUsed+=iteration;
    nbThoughtMoves++;
    totalWantedThinkingTime+=simulationTime;
    nbWantedSimulations+=nbTotalSimulations;
    lastNbSimulationsUsed=iteration;
}













void MoGo::InnerMCGoPlayer::protectedNode( GobanNode * node ) const {/**assert(0);**/
#ifdef PARALLELIZED
    pthread_mutex_lock(&mutexProtectNodes);
    int index=GoTools::find(protectedNodes, node);
    while (index>=0) {/**assert(0);**/
        pthread_mutex_unlock(&mutexProtectNodes);
#ifdef LINUX
        pthread_yield();
#endif
        pthread_mutex_lock(&mutexProtectNodes);
        index=GoTools::find(protectedNodes, node);
    }
    protectedNodes.push_back(node);
    // GoTools::print("protectedNodes\n");
    // for (int i=0;i<(int)protectedNodes.size();i++) GoTools::print("%i \n", protectedNodes[i]);
    pthread_mutex_unlock(&mutexProtectNodes);
#else
    node=0;
    assert(0);
#endif
}

void MoGo::InnerMCGoPlayer::unProtectedNode( GobanNode * node ) const {/**assert(0);**/
#ifdef PARALLELIZED
    pthread_mutex_lock(&mutexProtectNodes);
    // GoTools::print("unprotectedNodes\n");
    // for (int i=0;i<(int)protectedNodes.size();i++) GoTools::print("%i \n", protectedNodes[i]);
    assert(GoTools::find(protectedNodes, node)>=0);
    GoTools::remove(protectedNodes, node);
    pthread_mutex_unlock(&mutexProtectNodes);
#else
    node=0;
    assert(0);
#endif
}


const GoGameTree & MoGo::InnerMCGoPlayer::getTree( ) const {/**assert(0);**/
    return playerTree;
}

Goban & MoGo::InnerMCGoPlayer::getGobanCopy( const Goban & goban ) {/**assert(0);**/
    int numThread=
#ifdef PARALLELIZED
        ThreadsManagement::getNumThread();
#else
    0;
#endif
    if (allBoards[numThread]!=0)
        *allBoards[numThread]=goban;
    else
        allBoards[numThread]=new Goban(goban);

    return *allBoards[numThread];
}

void MoGo::InnerMCGoPlayer::reInitiateGobanBitmaps(const Goban &goban) {/**assert(0);**/

	
    MoveSelectorGlobal *moveSelector=dynamic_cast<MoveSelectorGlobal *>(simulationMoveSelector);
    if (!moveSelector) return;
    GobanNode *node=playerTree.getRootNodeAddress();
    if (moveSelector->tenukiMode==0) {/**assert(0);**/
        if (shishoCheckMode && node->isShallowerThan(shishoCheckMode-2))//shallow nodes only!
            goban.initiateNodeUrgencyForShisho(node, &playerTree);
        if (shishoCheckModeNew && node->isShallowerThan(shishoCheckModeNew-1)) // shallow nodes only!
            goban.initiateNodeUrgencyForShishoNew(node, &playerTree);
        return;
    }
    if (node->fatherNode().size()>0) return;
    if (node->bitmap().size()!=0) {/**assert(0);**/
        //   GoTools::print("reInitiateGobanBitmaps\n");
        Location location=goban.indexOnArray(0,0);
        for (int i=0;i<GobanNode::height;i++, location+=2)
            for (int j=0;j<GobanNode::width;j++, location++) {/**assert(0);**/
                if (GobanBitmap::getBitValue(node->bitmap(),i,j)) continue;

                //       goban.textModeShowPosition(location);
                if (node->nodeUrgency()[location]>0.) {/**assert(0);**/
                    continue;
                } else {/**assert(0);**/
                    if (goban.isNotUseless(location))
                        node->nodeUrgency()[location]=0.0001;
                }
            }
    }
    if (shishoCheckMode && node->isShallowerThan(shishoCheckMode-2))//shallow nodes only!
        goban.initiateNodeUrgencyForShisho(node);
    if (shishoCheckModeNew && node->isShallowerThan(shishoCheckModeNew-1)) // shallow nodes only!
        goban.initiateNodeUrgencyForShishoNew(node);

}





























































void MoGo::InnerMCGoPlayer::selfTrainNew4( const Goban & goban, const int number, const double time, int multiThreadsIdentifier) {
	GoTools::print("selfTrainNew4(...,%d)\n",multiThreadsIdentifier);
    assert(useLibEgoTest || useFastBoard);
	GoTools::print("assert ok (...,%d)\n",multiThreadsIdentifier);
    if (multiThreadsIdentifier==0)
        selfTrainNewMaster(goban, number, time, multiThreadsIdentifier, 1);
    else
        selfTrainNewSlave4(goban, multiThreadsIdentifier);
}
void MoGo::InnerMCGoPlayer::selfTrainNew4NOMPI( const Goban & goban, const int number, const double time, int multiThreadsIdentifier) {
	GoTools::print("selfTrainNew4(...,%d)\n",multiThreadsIdentifier);
    assert(useLibEgoTest || useFastBoard);
	GoTools::print("assert ok (...,%d)\n",multiThreadsIdentifier);
    if (multiThreadsIdentifier==0)
        selfTrainNewMasterNOMPI(goban, number, time, multiThreadsIdentifier, 1);
    else
        selfTrainNewSlave4(goban, multiThreadsIdentifier);
}

#ifdef MOGOMPI
#ifdef MPI_INTENSIVE
void MoGo::InnerMCGoPlayer::putInDouble(double* summary,GobanNode *node)   //this function adds in "summary" the infos in node
{
    int cinqCents=(gobanSize+2)*(gobanSize+4);
    //fprintf(stderr,"%d children\n",int(node->childNode().size()));fflush(stdout);
    for (int i=0;i<(int)node->childNode().size();i++) 
    {/**assert(0);**/
	//if ( node->childNode()[i]->getInitiated())
	{
        	assert(node->childMove()[i]>=0);
        	assert(node->childMove()[i]<cinqCents);
	  	if (summary[cinqCents+node->childMove()[i]]>0.) {assert(Goban(node->gobanKey()).isLegal(node->childMove()[i]));}
        	summary[node->childMove()[i]]=node->childNode()[i]->nodeValue()[1];
        	summary[cinqCents+node->childMove()[i]]=node->childNode()[i]->numberOfSimulation();
	}
    }
	
    //fprintf(stderr,"boudiou\n");fflush(stderr);
    Location location = node->indexOnArray(0,0);
    //if (node->nodeUrgency().size()>0)
    for (int i=0;i<GobanNode::height;i++, location+=2)
        for (int j=0;j<GobanNode::width;j++, location++) {
		if (summary[3*cinqCents+location]>0.) {assert(Goban(node->gobanKey()).isLegal(location));}

            summary[2*cinqCents+location]=node->nodeUrgency()[location];
            summary[3*cinqCents+location]=node->nodeValue()[location+3];
        }             
    //             fprintf(stderr,"aaaboudiou2\n");fflush(stderr);
    return;
}

void MoGo::InnerMCGoPlayer::getFromDouble(double *summary,GobanNode * node,GoGameTree * playerTree){
    //fprintf(stderr,"ICI A(%d)\n",MPI_NUMERO);fflush(stderr);
    int cinqCents=(gobanSize+2)*(gobanSize+4);

    MoveSelectorGlobal *simulationMoveSelectorTmp=dynamic_cast<MoveSelectorGlobal *>(simulationMoveSelector);

    // update of non-AMAf values for children which are initiated
    for (int i=0;i<(int)node->childNode().size();i++) 
    {/**assert(0);**/
	//if (node->childNode()[i]->getInitiated())
	{
		assert(node->childMove()[i]>=0);
		assert(node->childMove()[i]<cinqCents);
		
		node->childNode()[i]->nodeValue()[1]=summary[node->childMove()[i]]; //number of wins
		node->childNode()[i]->resetNumberOfSimulation(int(ceil(summary[cinqCents+node->childMove()[i]]))); //number of simus
		summary[node->childMove()[i]]=0.;
		
	
		summary[cinqCents+node->childMove()[i]]=0.;
	
	
		GobanNode* childPlayed=node->childNode()[i];
			if (!childPlayed->getInitiated())
			{ 
			  
                Goban goban(node->gobanKey());
			goban.playMoveOnGoban(node->childMove()[i]);

			  
			  FastBoard* p_fb = FastBoard::fromGoban(goban);  //pointer on FastBoard
			  initiatePlayerGobanNodeAsynch(simulationMoveSelectorTmp, NULL, Goban(playerTree->getRootNodeAddress()->gobanKey()),*p_fb, childPlayed, currentSequences[0]);
			  
			  if(p_fb)
			    {
			      delete p_fb;
			      p_fb = 0;
			    }
			}

	}
	/*if (!childPlayed->getInitiated())
		{ 
			
			initiatePlayerGobanNodeAsynch(simulationMoveSelectorTmp, NULL, Goban(playerTree->getRootNodeAddress()->gobanKey()), *FastBoard::getBoardForThread(), childPlayed, currentSequences[0]);
			
		}

	*/


    }
	  
	// update of non-AMAF values for children which are not initiated or do not exist
   // fprintf(stderr,"ICI B(%d)\n",MPI_NUMERO);fflush(stderr);
    Location location = node->indexOnArray(0,0);
    //if (node->nodeUrgency().size()>0)
    for (int i=0;i<GobanNode::height;i++, location+=2)
        for (int j=0;j<GobanNode::width;j++, location++) {
            if (summary[cinqCents+location]>0.)   //FIXMEFIXMEFIXME   // if this node should exist and be initiated according to other nodes
            {
		
		//  assert(0);
		//fprintf(stderr,"Proc %d debut create %d\n",MPI_NUMERO,location);fflush(stderr);
                GobanNode* childPlayed;
                Goban goban(node->gobanKey());
		//fprintf(stderr,"Proc %d create A\n",MPI_NUMERO);fflush(stderr);
		//fprintf(stderr,"printing \n");fflush(stderr);
		//goban.textModeShowGoban();
		/*fprintf(stderr,"Proc %d can play on:\n",MPI_NUMERO);fflush(stderr);
		for (int ii=0;ii<(int)node->childNode().size();ii++)
					goban.textModeShowPosition(node->getChild(node->childNode()[ii]));
		fprintf(stderr,"and I study:\n");fflush(stderr); 
		goban.textModeShowPosition(location);
		fprintf(stderr,"printing done (%d)\n",location);fflush(stderr);*/              
		
 //if (1==0)
		if ((goban.isLegal(location)))
		{
			/*fprintf(stderr,"avant:\n");
			goban.textModeShowGoban();
			goban.textModeShowPosition(location);
			fprintf(stderr,"\n");*/
			//fprintf(stderr,"Proc %d playMoveOnGoban %d\n",MPI_NUMERO,location);fflush(stderr);
			goban.playMoveOnGoban(location);
			/*fprintf(stderr,"apres:\n");
			goban.textModeShowGoban();*/
			//fprintf(stderr,"Proc %d create B\n",MPI_NUMERO);fflush(stderr);
			//fprintf(stderr,"Proc %d connectToTree %d\n",MPI_NUMERO,location);fflush(stderr);
			playerTree->connectToTree(node, goban.getGobanKey(),location, -1, childPlayed, true);
			/* connectToTree( node = father, key = key of child, location, -1 because we don't want to take care of the existence of this child, childPlayed is the output, and true because we mutex)
			*/

			//childPlayed->nodeUrgency()[PASSMOVE]=0.00001;
			//fprintf(stderr,"Proc %d debut create location %d\n",MPI_NUMERO,location);fflush(stderr);
			if (!childPlayed->getInitiated())
			{ 
			  

			  
			  FastBoard* p_fb = FastBoard::fromGoban(goban);  //pointer on FastBoard
			  
			  initiatePlayerGobanNodeAsynch(simulationMoveSelectorTmp, NULL, Goban(playerTree->getRootNodeAddress()->gobanKey()),*p_fb, childPlayed, currentSequences[0]);
			  
			  if(p_fb)
			    {
			      delete p_fb;
			      p_fb = 0;
			    }
			}

			//fprintf(stderr,"Proc %d fin create location %d\n",MPI_NUMERO,location);fflush(stderr);
		}	
		
		  
              childPlayed->nodeValue()[1]=summary[location];
                childPlayed->resetNumberOfSimulation(int(ceil(summary[cinqCents+location])));
		//fprintf(stderr,"Proc %d fin set value\n",MPI_NUMERO);fflush(stderr);

            }	
	      
            node->nodeUrgency()[location]= summary[2*cinqCents+location]; //AMAF wins
            node->nodeValue()[location+3]= summary[3*cinqCents+location]; //AMAF simus
        }
   // fprintf(stderr,"ICI C(%d)\n",MPI_NUMERO);fflush(stderr);             


    return;
}

#ifdef CREATE_OPENINGS
void MoGo::InnerMCGoPlayer::dump(GobanNode * node,std::vector<int> suite)
{
  
static FILE* outputOpening=fopen("myOpening","w");
      Location location = node->indexOnArray(0,0);
      
      for (int i=0;i<GobanNode::height;i++, location+=2)
	{
	  for (int j=0;j<GobanNode::width;j++, location++) 
	    {
	      
	      GobanNode *cNode = node->getChild(location);
	      
	      //assert(cNode!=0); 
	      //if (cNode) assert(playerTree->getGobanNodeAddress(cNode->gobanKey()));
	      if ((cNode))//&&(playerTree->getGobanNodeAddress(cNode->gobanKey())))   
		{
	//	  fprintf(stderr,"ZZZ %d a un fils %d ",MPI_NUMERO,location);
		  /*if (!cNode->getInitiated())
		    {
		    fprintf(stderr," instancie\n");
		    *
		    simulationMoveSelectorTmp->lockThisNode(cNode);
		    cNode->resizeUrgency();
		    cNode->resizeExpertise();
		    cNode->nodeValue().resize(3+(GobanNode::height+2)*(GobanNode::width+2));
		    cNode->setInitiated(true);
		    simulationMoveSelectorTmp->unlockThisNode(cNode);
		    *
		    initiatePlayerGobanNodeAsynch(simulationMoveSelectorTmp, NULL, Goban(playerTree->getRootNodeAddress()->gobanKey()), *FastBoard::getBoardForThread(), cNode, currentSequences[0]);
		    }
		    else
		    fprintf(stderr," \n");*/
		  
		  if ((cNode->getInitiated())&&(cNode->numberOfSimulation()>100000))
		    {
		      fprintf(stderr,"I am proc %d, I work on location %d (%d)yyyy\n",MPI_NUMERO,location,k);fflush(stderr);
			suite.push_back(location);
		        dump(cNode,suite);
			fprintf(outputOpening,"opening ");
			for (int i=0;i<int(node->gobanKey().size());i++) fprintf(outputOpening,"%d ",node->gobanKey()[i]);
			fprintf(outputOpening," %d %g %d\n",location,cNode->nodeValue()[1],cNode->numberOfSimulation());
		    }
		}
	      
	    }
	}
      //while (1);
}
#endif
void MoGo::InnerMCGoPlayer::share(GobanNode * node,int k,GoGameTree* playerTree)
{
  
    int cinqCents=(gobanSize+2)*(gobanSize+4);
  //fprintf(stderr,"Proc %d 0\n",MPI_NUMERO);fflush(stderr);
  double sharedData[4*cinqCents]; //that which will be sent
  double sharedData2[4*cinqCents]; //that which will be receive
 //   GoTools::print("put in double\n");
    MoveSelectorGlobal *simulationMoveSelectorTmp=dynamic_cast<MoveSelectorGlobal *>(simulationMoveSelector);
  for (int j=0;j<4*cinqCents;j++) sharedData[j]=0.;
  putInDouble(sharedData,node);
#ifdef SUMTRICK
    if (node->isShared)
    {
   assert(node->lastShared);
  //fprintf(stderr,"using archive\n");fflush(stderr);
  for (int j=0;j<4*cinqCents;j++) sharedData[j]=sharedData[j]-node->lastShared[j];
    }else
    assert(!node->lastShared);
  //fprintf(stderr,"using archive done\n");fflush(stderr);
#endif
 //   GoTools::print("mpiallreduce %d to be done\n",k);
    // Le nombre de simulations de la racine n'est jamais partagï¿½e sinon
    // On stocke donc le nombre de simulation du noeud pere ï¿½ l'adresse 4*cinqCents-1
    //simulationMoveSelectorTmp->lockThisNode(node);
    //simulationMoveSelectorTmp->unlockThisNode(node);
    
    double t1 = MPI_Wtime();
//if (node!=playerTree->getRootNodeAddress())
//    MPI_Allreduce(sharedData,sharedData2,4*cinqCents,MPI_DOUBLE,MPI_SUM,MPI_COMM_WORLD); //FIXME EN MPI2, MPI_IN_PLACE serait le bon 1er argument
 //   else
    {
    MPI_Allreduce(sharedData,sharedData2,4*cinqCents,MPI_DOUBLE,MPI_SUM,MPI_COMM_WORLD); //FIXME EN MPI2, MPI_IN_PLACE serait le bon 1er argument
  for (int j=0;j<4*cinqCents;j++) 
	    sharedData2[j]/=double(MPI_NUMBER);
    }
    double t2 = MPI_Wtime();
    mpi_stats_allreduce_time += t2 - t1;
    mpi_stats_allreduce_nb++;
    
if (node==playerTree->getRootNodeAddress())
{
mpi_stats_allreduce_nb_for_this_move=0;
double total;total=0;
for (int j=0;j<cinqCents;j++) total+=sharedData2[cinqCents+j];
    sharedData2[4*cinqCents-1]=total;
 }  
mpi_stats_allreduce_nb_for_this_move++;
if (MPI_NUMERO==0)
    fprintf(stderr,"rank %d: mpiallreduce %d done. This call: %gs, Avg time %gs (%gs for this sharing), nb call %d\n",MPI_NUMERO,k,t2-t1,mpi_stats_allreduce_time/mpi_stats_allreduce_nb,mpi_stats_allreduce_time*mpi_stats_allreduce_nb_for_this_move/mpi_stats_allreduce_nb,mpi_stats_allreduce_nb);
#ifdef SUMTRICK
    if (!node->isShared) {assert(!node->lastShared);}
    if (node->isShared){assert(node->lastShared);}
    if (!node->lastShared)
		{node->lastShared=new double[4*cinqCents];
		for (int j=0;j<4*cinqCents;j++)
		   node->lastShared[j]=0.;}
 //   fprintf(stderr,"archiving\n");fflush(stderr);
    //if (k>0)
    for (int j=0;j<4*cinqCents;j++) //plein de choses ajoutees par cumul 
   		node->lastShared[j]=sharedData2[j]-sharedData[j];
    /*else //rien d'ajoute par cumul
    {
    //pour les feuilles de l'arbre partage, on moyenne seulement
    for (int j=0;j<4*cinqCents;j++) 
	    sharedData2[j]/=double(MPI_NUMBER);
    for (int j=0;j<4*cinqCents;j++) 
   		node->lastShared[j]=0.;
    }*/
   // fprintf(stderr,"archiving done\n");fflush(stderr);
    node->isShared=1;
	// lastShared contient tout ce qu'on a recu en partage
	
#else
 //   for (int j=0;j<4*cinqCents;j++) 
//	    sharedData2[j]/=double(MPI_NUMBER);
#endif

    if (node==playerTree->getRootNodeAddress())
    {
        fprintf(stderr,"Proc %d: root has %d sims\n",MPI_NUMERO,int(node->numberOfSimulation()));fflush(stderr);
	lastShare=int(ceil(sharedData2[4*cinqCents-1]));
	//fprintf(stderr,"total number of sims = %d\n",lastShare);
    }
    GoTools::print("get from double\n");
    //fprintf(stderr,"Proc %d B\n",MPI_NUMERO);fflush(stderr);
   int shareDeeper[cinqCents];
    for (int j=0;j<cinqCents;j++)
    {
    if ( sharedData2[j+cinqCents]>double(lastShare)*PROBA_SHARING)
    	{GoTools::print("location %d (%g)\n",j,(sharedData2[j+cinqCents]/double(lastShare)));
	    shareDeeper[j]=1;}
    else
		    shareDeeper[j]=0;
    }
    simulationMoveSelectorTmp->lockThisNode(node);
    //node->resetNumberOfSimulation(int(ceil(sharedData2[4*cinqCents-1]/MPI_NUMBER))); 	
    //if (k==100)
    getFromDouble(sharedData2,node,playerTree);
    simulationMoveSelectorTmp->unlockThisNode(node);
    GoTools::print("get from double done\n");
  
  //fprintf(stderr,"Proc %d B\n",MPI_NUMERO);fflush(stderr);
  //for (int j=0;j<4*cinqCents;j++) sharedData2[j]/=double(MPI_NUMBER);
  
  //getFromDouble(sharedData2,node,playerTree);
  
  
  //fprintf(stderr,"Proc %d C\n",MPI_NUMERO);fflush(stderr);
  if (k>0)
    {
      Location location = node->indexOnArray(0,0);
      
      for (int i=0;i<GobanNode::height;i++, location+=2)
	{
	  for (int j=0;j<GobanNode::width;j++, location++) 
	    {
	      
	      GobanNode *cNode = node->getChild(location);
	      
	      //assert(cNode!=0); 
	      //if (cNode) assert(playerTree->getGobanNodeAddress(cNode->gobanKey()));
	      if ((cNode))//&&(playerTree->getGobanNodeAddress(cNode->gobanKey())))   
		{
	//	  fprintf(stderr,"ZZZ %d a un fils %d ",MPI_NUMERO,location);
		  /*if (!cNode->getInitiated())
		    {
		    fprintf(stderr," instancie\n");
		    *
		    simulationMoveSelectorTmp->lockThisNode(cNode);
		    cNode->resizeUrgency();
		    cNode->resizeExpertise();
		    cNode->nodeValue().resize(3+(GobanNode::height+2)*(GobanNode::width+2));
		    cNode->setInitiated(true);
		    simulationMoveSelectorTmp->unlockThisNode(cNode);
		    *
		    initiatePlayerGobanNodeAsynch(simulationMoveSelectorTmp, NULL, Goban(playerTree->getRootNodeAddress()->gobanKey()), *FastBoard::getBoardForThread(), cNode, currentSequences[0]);
		    }
		    else
		    fprintf(stderr," \n");*/
		  
		  if ((cNode->getInitiated())&&(shareDeeper[location]))
		    {
		      //fprintf(stderr,"I am proc %d, I work on location %d (%d)yyyy\n",MPI_NUMERO,location,k);fflush(stderr);
		      share(cNode,k-1,playerTree);
		    }
		}
	      
	    }
	}
      //while (1);
    }
}


#endif
#endif

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayer::selfTrainNewMaster( const Goban & goban, const int number, const double time, int multiThreadsIdentifier, int mode) {

	GoTools::print("selfTrainNewMaster(...)\n");
    //  if (goban.isScorable()) return ;
    assert(multiThreadsIdentifier==0 || mode>0);
    assert(int(currentSequences.size())>multiThreadsIdentifier);
    assert(int(currentSequenceMoves.size())>multiThreadsIdentifier);
    assert(multiThreadsIdentifier==0);
    int endThinking = 0;
    int nbSimulations = 0;
    double simulationTime;
    if (time>1) simulationTime=time-.2;
    else
        simulationTime=time*.97;
    int i=0;
    CPUTimeChronometer cpuTimer0;
	GoTools::print("selfTrainNewMaster(...) --- selfTrainInitiation\n");
    selfTrainInitiation(goban, currentSequences[multiThreadsIdentifier],simulationTime);



    double initiateTime = cpuTimer0.getTime();
    simulationTime-=initiateTime;

    if (!dontDisplay)
        GoTools::print("Self train initiation time: %f\n",initiateTime);

    CPUTimeChronometer cpuTimer;
    double estimedScore=isInterestingToContinue(goban, currentSequences[multiThreadsIdentifier]);
    if ( estimedScore<=0.01 || estimedScore>=resignThreshold )
        if (!dontDisplay)
            GoTools::print("No interest to think more...\n");
    GobanNode *root=getNode(goban, currentSequences[multiThreadsIdentifier],true,true);



	GoTools::print("selfTrainNewMaster(...) --- ben on est la \n");


    selfTrainCounter = 0;
    nbSimulationsAtRoot=int(playerTree.getRootNodeAddress()->numberOfSimulation()); nbSimulationsAtRoot=GoTools::max(0,nbSimulationsAtRoot);


    startSlaves=true;
    //   GoTools::print("start slaves\n");

#ifdef MOGOMPI
    double tLastShare=MPI_Wtime();
#endif
#ifdef CREATE_OPENINGS
	double openingDate=MPI_Wtime()+600;
#endif
    for (i=1; true ;i++) {
 

#ifdef MOGOMPI
#ifdef MPI_INTENSIVE
    	//if ((MPI_NUMERO==0)&&(i%100==0))
	//	fprintf(stderr,"node 0 is still alive\n");fflush(stderr);

        if ((multiThreadsIdentifier==0)) {
      
            int out=0;
            MPI_Status status;
	//    GoTools::print("dansla grosse boucle %d\n",i);
            // Case machine 0
            if (MPI_NUMERO == 0)
            {
                // Is it time to share?
#ifdef CREATE_OPENINGS
		if (MPI_Wtime()>openingDate)
		{
			openingDate=MPI_Wtime()+600;
			printf("dumping opening\n");
			dump(root);
		}
#endif
                if (MPI_Wtime()>tLastShare+DELTATIME_SHARING)//i%NB_ITER_BEFORE_SHARING_MASTER==0)
                {
			tLastShare=MPI_Wtime();
	  //    	GoTools::print("je demande un sharing\n");
                    for (int j=1;j<MPI_NUMBER;j++)
                    {
                      //  std::cerr<< "machine : " << MPI_NUMERO << " send sharing\n";fflush(stderr);
                        assert(MPI_Send(NULL,0,MPI_INT,j,MPI_TAG_SHARING,MPI_COMM_WORLD)==MPI_SUCCESS);
                    }
	      	//GoTools::print("tout le monde m'a recu, je share\n");
                    //std::cerr<< "machine : " << MPI_NUMERO << " sharing\n";fflush(stderr);
                    //showGobanUrgency(goban, currentSequences[multiThreadsIdentifier]);
                //lockCutTree(); //JBH: to avoid swap
		//playerTree.lock();
		//lockSafer();
                    //std::cerr<< "this machine is in lock " << MPI_NUMERO << " \n";fflush(stderr);
                    share(root,DEPTH_SHARING,&playerTree);
                    //std::cerr<< "this machine can unlock " << MPI_NUMERO << " \n";fflush(stderr);
		//unlockSafer();
		//playerTree.unlock();
		//unlockCutTree();
                    //std::cerr<< "this machine has unlocked " << MPI_NUMERO << " \n";fflush(stderr);
                    //std::cerr<< "machine : " << MPI_NUMERO << " end sharing\n";
	      
                }
	  
                // then all machine should receive the information stoping
                if (endThinking)
                {
			        //            std::cerr<< "machine : " << MPI_NUMERO << " wants to stop \n";
		//	GoTools::print("thread 0 proc 0: je veux m'arreter\n");
                    for (int j=1;j<MPI_NUMBER;j++)
                    {
                        std::cerr<< "machine : " << MPI_NUMERO << " send endthinking\n";
                        assert(MPI_Send(&endThinking,1,MPI_INT,j,MPI_TAG_ENDTHINKING,MPI_COMM_WORLD)==MPI_SUCCESS);
		  
                    }
			                    std::cerr<< "machine : " << MPI_NUMERO << " has said stop \n";
			GoTools::print("tout le monde sait qu'il doit s'arreter\n");
                    stopSlaves=true;
                    break;
                }
	  
	  
	  
	  
            }
            // case machine > 0
            if (MPI_NUMERO > 0)
            {
                // does machine 0 wants to share?
                if (i%NB_ITER_BEFORE_CHECKING_SLAVE==0 )
                {
	//		GoTools::print("on va peut-etre sharer\n ");
                    MPI_Iprobe(0,MPI_TAG_SHARING,MPI_COMM_WORLD,&out,&status);
                    if (out)
                    {
	//		GoTools::print("on va vraiment sharer\n ");
                        //std::cerr<< "machine : " << MPI_NUMERO << " receive sharing\n";
                        assert(MPI_Recv(NULL,0,MPI_INT,0,MPI_TAG_SHARING,MPI_COMM_WORLD,MPI_STATUS_IGNORE)==MPI_SUCCESS);
		//	GoTools::print("on va vraiment sharer 2\n ");
          //             std::cerr<< "machine : " << MPI_NUMERO << " sharing\n";
                        //showGobanUrgency(goban, currentSequences[multiThreadsIdentifier]);
                        
                    //std::cerr<< "this machine will lock " << MPI_NUMERO << " \n";fflush(stderr);
                //lockCutTree(); //JBH: to avoid swap
			//playerTree.lock();
			//lockSafer();
   //                 std::cerr<< "this machine is in lock " << MPI_NUMERO << " \n";fflush(stderr);
                    share(root,DEPTH_SHARING,&playerTree);
 //                   std::cerr<< "this machine can unlock " << MPI_NUMERO << " \n";fflush(stderr);
			//unlockSafer();
			//playerTree.unlock();				
		//unlockCutTree();
     //               std::cerr<< "this machine has unlocked " << MPI_NUMERO << " \n";fflush(stderr);
                    //std::cerr<< "machine : " << MPI_NUMERO << " end sharing\n";
		  
                    }
	      
	      
                    MPI_Iprobe(0,MPI_TAG_ENDTHINKING,MPI_COMM_WORLD,&out,&status);
                    if (out)
                    {
			  //  GoTools::print("je dois m'arreter\n");
                        std::cerr<< "machine : " << MPI_NUMERO << " (thread 0) receives endthinking\n";
                        assert(MPI_Recv(&endThinking,1,MPI_INT,0,MPI_TAG_ENDTHINKING,MPI_COMM_WORLD,MPI_STATUS_IGNORE)==MPI_SUCCESS);
                        stopSlaves=true;
                        break;
                    }
                }
	  
	  
	  
            }
        }
else assert(0);
#endif
#endif
    

#ifndef MPI_INTENSIVE
	if (endThinking) 
	{
            stopSlaves=true;
            break;
	}
#endif
	if (!endThinking)
      	{
#ifdef JYMODIF_NEW
		double elapsedTime=double(cpuTimer.getTime());
	assert(elapsedTime>=0.);
       if (time>1)
	    nbRemainingSimulations= int(0.97*(( (simulationTime-elapsedTime) / elapsedTime) * double(totalSims)));
	else nbRemainingSimulations=100000000;
          
		
	//    if (double(cpuTimer.getTime())/simulationTime); 
	//printf("nbSimu=%d\n",totalSims);fflush(stdout);//node->numberOfSimulations()
            selfTrainOneGameAsynch(i, goban, multiThreadsIdentifier, root /*, nbRemainingSimulations*/);
#else
            selfTrainOneGameAsynch(i, goban, multiThreadsIdentifier, root);
#endif
            nbSimulations++;

	   
            endThinking = stopThinking(cpuTimer, simulationTime, i, nbTotalSimulations);		
            if (endThinking) { GoTools::print("endThinking registered");registerStopThinking(cpuTimer, simulationTime, i, nbTotalSimulations);}

            if (handleCutTree(i)) endThinking = 1;

		
            if (i%(NB_ITER_BEFORE_DISPLAY*(mode==1?1:nbThreads))==0 && multiThreadsIdentifier==0 && dontDisplay==0) {
		//fprintf(stderr,"lockCutTree %d \n",MPI_NUMERO);fflush(stderr);
                lockCutTree(); //JBH: to avoid swap
		//lockSafer();
                showPreferedSequence(goban, currentSequences[multiThreadsIdentifier]);
		//unlockSafer();
                unlockCutTree(); //JBH: to avoid SWAP
		//fprintf(stderr,"unLockCutTree %d \n",MPI_NUMERO);fflush(stderr);
                selfTrainCounter = 0;
            }

#ifdef MOGOMPI
            if (MPI_NUMERO == 0)
#endif
            {
                if (i%NB_ITER_BEFORE_ISINTERESTING==0 && multiThreadsIdentifier==0 && nbThreads==1) {
                    double estimedScore=isInterestingToContinue(goban, currentSequences[multiThreadsIdentifier]);
                    if ( estimedScore<=0.01 || estimedScore>=resignThreshold ) {
                        if (!dontDisplay)
                            GoTools::print("No interest to think more...\n");
                        endThinking = 1;
                    }
                }
            }

            if ( i > number) endThinking = 1;
		
	}

	//if (MPI_NUMERO > 0)
	//std::cerr << "nb itï¿½ration : " << i << "\n";
	 
    }
 

    lockCutTree();
    showPreferedSequence(goban, currentSequences[multiThreadsIdentifier]);
    unlockCutTree();
    double dTime=cpuTimer.getTime();


// le chronometer utilisï¿½ en mode non MPI est multipliï¿½ par le nombre de threads
#ifndef MPIMOGO
    dTime/=nbThreads;
#endif
	
    //nbSimulations*=nbThreads;
    if (!dontDisplay) {
        GoTools::print("nb Threads: %i \n",nbThreads);

/*#ifdef MOGOMPI
	 GoTools::print("proc %d: %d simulations(average length:%d) done, time used: %6.2f seconds.(%7.1f games/sec)\n",MPI_NUMERO,nbSimulations,selfTrainAverageLength/nbSimulations,dTime,(double)nbSimulations/dTime);
#else
        GoTools::print("%d simulations(average length:%d) done, time used: %6.2f seconds.(%7.1f games/sec)\n",nbSimulations,selfTrainAverageLength/nbSimulations,dTime,(double)nbSimulations/dTime);
#endif*/
        GoTools::print("%d simulations(average length:%d) done, time used: %6.2f seconds.(%7.1f games/sec pour le thread 0)\n",nbSimulations,selfTrainAverageLength/nbSimulations,dTime,(double)nbSimulations/dTime);
    }



}

void MoGo::InnerMCGoPlayer::selfTrainNewMasterNOMPI( const Goban & goban, const int number, const double time, int multiThreadsIdentifier, int mode)
{

	GoTools::print("selfTrainNewMaster(...)\n");
    //  if (goban.isScorable()) return ;
    assert(multiThreadsIdentifier==0 || mode>0);
    assert(int(currentSequences.size())>multiThreadsIdentifier);
    assert(int(currentSequenceMoves.size())>multiThreadsIdentifier);
    assert(multiThreadsIdentifier==0);
    int endThinking = 0;
    int nbSimulations = 0;
    double simulationTime;
    if (time>1) simulationTime=time-.2;
    else
        simulationTime=time*.97;
    int i=0;
    CPUTimeChronometer cpuTimer0;
	GoTools::print("selfTrainNewMaster(...) --- selfTrainInitiation\n");
    selfTrainInitiation(goban, currentSequences[multiThreadsIdentifier],simulationTime);



    double initiateTime = cpuTimer0.getTime();
    simulationTime-=initiateTime;

    if (!dontDisplay)
        GoTools::print("Self train initiation time: %f\n",initiateTime);

    CPUTimeChronometer cpuTimer;
    double estimedScore=isInterestingToContinue(goban, currentSequences[multiThreadsIdentifier]);
    if ( estimedScore<=0.01 || estimedScore>=resignThreshold )
        if (!dontDisplay)
            GoTools::print("No interest to think more...\n");
    GobanNode *root=getNode(goban, currentSequences[multiThreadsIdentifier],true,true);



	GoTools::print("selfTrainNewMaster(...) --- ben on est la \n");


    selfTrainCounter = 0;
    nbSimulationsAtRoot=int(playerTree.getRootNodeAddress()->numberOfSimulation()); nbSimulationsAtRoot=GoTools::max(0,nbSimulationsAtRoot);


    startSlaves=true;
    //   GoTools::print("start slaves\n");

#ifdef CREATE_OPENINGS
	double openingDate=MPI_Wtime()+600;
#endif
    for (i=1; true ;i++) {
 

	if (endThinking) 
	{
            stopSlaves=true;
            break;
	}
	if (!endThinking)
      	{

            selfTrainOneGameAsynch(i, goban, multiThreadsIdentifier, root);
            nbSimulations++;

	   
            endThinking = stopThinking(cpuTimer, simulationTime, i, nbTotalSimulations);		
            if (endThinking) { GoTools::print("endThinking registered");registerStopThinking(cpuTimer, simulationTime, i, nbTotalSimulations);}

            if (handleCutTree(i)) endThinking = 1;

		
            if (i%(NB_ITER_BEFORE_DISPLAY*(mode==1?1:nbThreads))==0 && multiThreadsIdentifier==0 && dontDisplay==0) {
		//fprintf(stderr,"lockCutTree %d \n",MPI_NUMERO);fflush(stderr);
                lockCutTree(); //JBH: to avoid swap
		//lockSafer();
                showPreferedSequence(goban, currentSequences[multiThreadsIdentifier]);
		//unlockSafer();
                unlockCutTree(); //JBH: to avoid SWAP
		//fprintf(stderr,"unLockCutTree %d \n",MPI_NUMERO);fflush(stderr);
                selfTrainCounter = 0;
            }

            {
                if (i%NB_ITER_BEFORE_ISINTERESTING==0 && multiThreadsIdentifier==0 && nbThreads==1) {
                    double estimedScore=isInterestingToContinue(goban, currentSequences[multiThreadsIdentifier]);
                    if ( estimedScore<=0.01 || estimedScore>=resignThreshold ) {
                        if (!dontDisplay)
                            GoTools::print("No interest to think more...\n");
                        endThinking = 1;
                    }
                }
            }

            if ( i > number) endThinking = 1;
		
	}

	//if (MPI_NUMERO > 0)
	//std::cerr << "nb itï¿½ration : " << i << "\n";
	 
    }
 

    lockCutTree();
    showPreferedSequence(goban, currentSequences[multiThreadsIdentifier]);
    unlockCutTree();
    double dTime=cpuTimer.getTime();


// le chronometer utilisï¿½ en mode non MPI est multipliï¿½ par le nombre de threads
    dTime/=nbThreads;
	
    //nbSimulations*=nbThreads;
    if (!dontDisplay) {
        GoTools::print("nb Threads: %i \n",nbThreads);

        GoTools::print("%d simulations(average length:%d) done, time used: %6.2f seconds.(%7.1f games/sec pour le thread 0)\n",nbSimulations,selfTrainAverageLength/nbSimulations,dTime,(double)nbSimulations/dTime);
    }



}



// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayer::selfTrainNewSlave4(const Goban & goban, int multiThreadsIdentifier) {
    assert(int(currentSequences.size())>multiThreadsIdentifier);
    assert(int(currentSequenceMoves.size())>multiThreadsIdentifier);
#define yk
#ifdef yk
    while(!stopSlaves && !startSlaves) {
#ifdef LINUX
      pthread_yield();
#endif
    }
#else /* old stuff, patched here ok was volatile, made it static */
    static bool ok=false;
    while(!stopSlaves && !startSlaves) {
        //    lockOneGameSafer(multiThreadsIdentifier);
        //    unlockOneGameSafer(multiThreadsIdentifier);
        ok=stopSlaves || startSlaves; if (ok) break;
#ifdef LINUX
        //     pthread_yield();
#endif        
    }
#endif
    int i=0;
    while(!stopSlaves) {
        selfTrainOneGameAsynch(i, goban, multiThreadsIdentifier, 0);
        i++;
    }
    GoTools::print("thread %d: %d simulations\n",multiThreadsIdentifier,i);
}















// TEMPORARY COMMENT: THIS FUNCTION IS USED
double MoGo::InnerMCGoPlayer::selfTrainFastToEndAndGetOneScoreNew3( int mTI, NodeSequence & currentSequence, const Vector< Location > & currentSequenceMove, int choiceB, int choiceW ) {
    if (useSequencesInTree) handleSequencesInTree(mTI, currentSequence, currentSequenceMove);

    allAtFirstData[mTI][0]+=3;
 //   int thresholdSelfAtari=choice;//Goban::getThresholdSelfAtari();
    if (useFastBoard) {
        bool blackWins=FastBoard::oneRollOutThread(useFastBoard/*, currentGoban*/, choiceB,choiceW , allAtFirstData[mTI], usedMovesAllAtFirst[mTI], getModifiedKomi());
        if (blackWins) return getModifiedKomi()+10.; else return getModifiedKomi()-10.;
    } else
        assert(0);

    return 0.;
}

int MoGo::InnerMCGoPlayer::getIndexSlave(int multiThreadsIdentifier) {/**assert(0);**/
    while (!stopSlaves && !isAThreadInCutTree) {/**assert(0);**/
        //     while (pthread_mutex_trylock(&stackMutex));
        //     while (pthread_mutex_lock(&stackMutex));
        if (commonStack==0) {/**assert(0);**/
            int min=1<<30; int ind=-1;
            for (int i=0;i<stackSizePerThread;i++) {/**assert(0);**/
                static int getIndexSlaveCount=0; getIndexSlaveCount++;
                if (getIndexSlaveCount%100000000==0) {/**assert(0);**/
                    GoTools::print("getIndexSlaveCount %i (%i) %i\n", getIndexSlaveCount, multiThreadsIdentifier, stopSlaves);
                }
                if (currentStateStack[multiThreadsIdentifier][i]==2 && currentStateStackIndex[multiThreadsIdentifier][i]<min) {/**assert(0);**/ // something to work on
                    min=currentStateStackIndex[multiThreadsIdentifier][i];ind=i;
                }
            }
            if (ind>=0) {/**assert(0);**/
                currentStateStack[multiThreadsIdentifier][ind]=1; // buzzy
                return ind;
            }
        } else {/**assert(0);**/ // commonStack>0
            int size=stackSizePerThread;//*(nbThreads-1);
            static int getIndexSlaveCount=0; getIndexSlaveCount++;
            if (getIndexSlaveCount%100000000==0) {/**assert(0);**/
                GoTools::print("getIndexSlaveCount %i (%i) %i\n", getIndexSlaveCount, multiThreadsIdentifier, stopSlaves);
            }
            pthread_mutex_lock(&stackMutex);
            for (int i=0;i<size;i++) {/**assert(0);**/
                if (currentStateStack[0][i]==2) {/**assert(0);**/ // something to work on
                    currentStateStack[0][i]=1; // buzzy
                    pthread_mutex_unlock(&stackMutex);
                    return i;
                }
            }
            pthread_mutex_unlock(&stackMutex);
        }

#ifdef DEBUG_MASTER_SLAVE
        static int nothing=0; nothing++; if (nothing%1000000==0) GoTools::print("nothing=%i\n", nothing);
#endif
        //     pthread_mutex_unlock(&stackMutex);
    }
    if (stopSlaves)
        return -1;
    else
        return -2;
}



// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayer::slaveOneOperation(const Goban &goban, NodeSequence &_currentSequence, Vector<Location> &_currentSequenceMoves, int mTI, GobanNode* lastNode,int choiceB,int choiceW,int ,double weightSimu,vector<int>* guessLocation,vector<int>* guessColor,vector<double>*guessWeight) {
    GobanNode* gn=lastNode;
    usedMovesAllAtFirst[mTI].clear();

     int numThread=0;
#ifdef PARALLELIZED
       numThread=ThreadsManagement::getNumThread();
#endif
 //      assert(numThread==mTI+1);

#ifdef GOLDENEYE_MC

       cerr<<"*** GOLDENEYE_MC *** BUG A CORRIGER dans innermcgo_player"<<endl;
       assert(0);
       /*
       //ATTENTION ATTTENTION ATTENTION BUG BUG BUG
       //boucle infinie qui se produit assez plutot RAREMENT
       
       (gn->semeaiW.size()==0)&&(gn->semeaiB.size()==0)&&(gn->fatherNode().size()>0))
       en principe, quand on arrive à la racine gn->fatherNode() devrait etre nul, mais je pense que la cause du bug est que si on revient à la position de départ (ce qui est extremement rare) alors le noeud peut avoir un noeud pere
       
       2 solutions pour corriger ce bug : une securite avec une variable stop ou mettre un autre test que
       gn->fatherNode().size()>0 pour vérifier qu'on n'est pas à la racine.
       
       Bug non corrigé (car code obsolete). L'assert(0) est donc une securite pour prevenir ce bug si on veut reactiver GOLDENEYE_MC
       */
       


       FastBoard::allBoards[numThread]->semeaiW=gn->semeaiW;
       FastBoard::allBoards[numThread]->semeaiB=gn->semeaiB;
    while ((gn->semeaiW.size()==0)&&(gn->semeaiB.size()==0)&&(gn->fatherNode().size()>0))
    {

	gn=gn->fatherNode()[0];
	FastBoard::allBoards[numThread]->semeaiW=gn->semeaiW;
	FastBoard::allBoards[numThread]->semeaiB=gn->semeaiB;
    }
#endif
#ifdef GOLDENEYE
	assert(guessLocation);assert(guessLocation->size()==guessColor->size());assert(guessLocation->size()==guessWeight->size());
#endif
    double score=selfTrainFastToEndAndGetOneScoreNew3(mTI, _currentSequence, _currentSequenceMoves, choiceB,choiceW);
#ifdef GOLDENEYE
    if (guessLocation->size()>0)
	    for (int k=0;k<guessLocation->size();k++)
    {
	 /*   fprintf(stderr,"it is supposed that the stone ");
		lastNode->textModeShowPosition(guessLocation);
	    fprintf(stderr," is %d (1==black): ",guessColor);*/
	    if ((*guessColor)[k]!=FastBoard::getBoardForThread()->position((*guessLocation)[k]))
	    {
		weightSimu*=(*guessWeight)[k];
	//	fprintf(stderr,"simu cancelled\n");
	    }
	/*else 
	{
		fprintf(stderr,"simu accepted\n");
	}*/
    }
#endif
 /*   else
    {
    fprintf(stderr,"no guessed location\n");
    }*/
     
    slaveOneOperationAfterScore(score, goban, _currentSequence, _currentSequenceMoves, mTI,lastNode,choiceB,choiceW,weightSimu);
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayer::slaveOneOperationAfterScore( double score, const Goban & goban, NodeSequence & _currentSequence, Vector< Location > & _currentSequenceMoves, int mTI,GobanNode*lastNode,int choiceB,int choiceW,double weightSimu ) {
    UCTUpdatorGlobal *treeUpdatorTmp=dynamic_cast<UCTUpdatorGlobal *>(treeUpdator);
    MoveSelectorGlobal *simulationMoveSelectorTmp=dynamic_cast<MoveSelectorGlobal *>(simulationMoveSelector);

    if(FBS_handler::board_size==9) //a security avoid useless updating
      {
	lastNode->nbMC++;
	if (score>getModifiedKomi()) 
	  {
	    selfTrainCounter++;
	    lastNode->winBlack[choiceB-1]++;
	    lastNode->lossWhite[choiceW-1]++;
	  }
	else
	  {
	    lastNode->lossBlack[choiceB-1]++;
	    lastNode->winWhite[choiceW-1]++;
	  }
      }
    else
      {
	if (score>getModifiedKomi()) 
	  selfTrainCounter++;
      }



    //here we update the tree and we apply the AMAF update
    treeUpdatorTmp->updateAllInOne(simulationMoveSelectorTmp, playerTree,_currentSequence,goban/*currentGoban*/,score,getModifiedKomi(),usedMovesAllAtFirst[mTI], allAtFirstData[mTI], allAtFirstDataInTree[mTI], _currentSequenceMoves,weightSimu);
    //        treeUpdatorTmp->update(playerTree,_currentSequence,goban/*currentGoban*/,score,getModifiedKomi());//here the currentGoban has been changed. in fact, its gobanState is useless in treeUpdator here.
    //        treeUpdatorTmp->update(simulationMoveSelectorTmp, playerTree,_currentSequence,goban/*currentGoban*/,score,getModifiedKomi(),usedMovesAllAtFirst[mTI], allAtFirstData[mTI], allAtFirstDataInTree[mTI], _currentSequenceMoves);

    updateAutoKomi(goban, score);

    if (_currentSequence.size()>deepestDepthOfTree) deepestDepthOfTree=_currentSequence.size();
    //   simulationMoveSelector->updateUrgencyByCurrentSequence(_currentSequence);
    //   for (int i=0;i<(int)_currentSequence.size();i++) _currentSequence[i]->needToUpdateUrgencyOn();

    if (moveSelectionByUpdate) {/**assert(0);**/
        int childIndex=-1;
        for (int i=innerMCGoPlayerStaticV1;i<(int)_currentSequence.size()-2-innerMCGoPlayerStaticV2;i++) {/**assert(0);**///FIXME (why put -2 rather than -1? The performance is strange here)
            //       if (_currentSequence[i]->numberOfSimulation()<openingTestMode) break;
            simulationMoveSelectorTmp->selectOneMoveNew(_currentSequence[i], goban, childIndex, 1);
        }
    }
}


bool MoGo::InnerMCGoPlayer::addJobToStack(int iteration, const NodeSequence &currentSequence, const Vector<Location> &currentSequenceMoves) {/**assert(0);**/

    if (commonStack==0) {/**assert(0);**/
        for (int i=0;i<stackSizePerThread;i++) {/**assert(0);**/
            for (int j=1;j<nbThreads;j++) {/**assert(0);**/ // begin at "1" because the thread num 0 is the master thread
                if (currentStateStack[j][i]==0) {/**assert(0);**/ // something empty
                    currentStateStack[j][i]=3; // to treat
                    currentStateStackIndex[j][i]=iteration;
                    currentSequencesStack[j*stackSizePerThread+i]=currentSequence;
                    currentSequenceMovesStack[j*stackSizePerThread+i]=currentSequenceMoves;

                    /*     GoTools::print("add that sequence to %i (%i):\n", i, j);
                           Goban tmpGoban; for (int k=1;k<(int)currentSequenceMoves.size();k++) tmpGoban.textModeShowPosition(currentSequenceMoves[k]);
                           GoTools::print("\n");*/
                    currentStateStack[j][i]=2; // to treat
                    //       pthread_mutex_unlock(&stackMutex);
#ifdef DEBUG_MASTER_SLAVE
                    GoTools::print("addJobToStack true %i (%i)\n", i, j);
#endif
                    return true;
                }
            }
        }
    } else {/**assert(0);**/
        int size=stackSizePerThread*(nbThreads-1);
        for (int i=0;i<size;i++) {/**assert(0);**/
            if (currentStateStack[0][i]==0) {/**assert(0);**/ // something empty
                currentStateStackIndex[0][i]=iteration;
                currentSequencesStack[i]=currentSequence;
                currentSequenceMovesStack[i]=currentSequenceMoves;
                currentStateStack[0][i]=2; // to treat
                //       pthread_mutex_unlock(&stackMutex);
#ifdef DEBUG_MASTER_SLAVE
                GoTools::print("addJobToStack true %i\n", i);
#endif
                return true;
            }
        }
    }



    //   pthread_mutex_unlock(&stackMutex);
#ifdef DEBUG_MASTER_SLAVE
    GoTools::print("addJobToStack false\n");
#endif
    return false;
}

void MoGo::InnerMCGoPlayer::removeIndexSlave( int index, int multiThreadsIdentifier ) {/**assert(0);**/
    if (commonStack==0) {/**assert(0);**/
        assert(currentStateStack[multiThreadsIdentifier][index]==1);
        currentStateStack[multiThreadsIdentifier][index]=0; // empty
        //   GoTools::print("removeIndexSlave %i (%i)\n", index, multiThreadsIdentifier);
#ifdef DEBUG_MASTER_SLAVE
        GoTools::print("removeIndexSlave %i (%i)\n", index, multiThreadsIdentifier);
#endif

    } else {/**assert(0);**/
        assert(currentStateStack[0][index]==1);
        currentStateStack[0][index]=0; // empty
        //   GoTools::print("removeIndexSlave %i (%i)\n", index, multiThreadsIdentifier);
    }
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayer::clearStacks( ) {
    for (int i=0;i<(int)currentStateStack.size();i++)
        for (int j=0;j<(int)currentStateStack[i].size();j++)
            currentStateStack[i][j]=0;
    interestingNodes.clear();
}




//./mogo --test playMultipleTournaments --player0 InnerMCGoPlayer --player1 gnugoDefault --nbRuns 100 --displayGames 0 --dontDisplay 1 --default9 --gobanSize 9 --dontDisplay 1 --thresholdSelfAtari 3 --minNumberOfSimulationsOfResign 1000 --dontDisplay 0 --displayGames 1 --playsAgainstHuman 1 --useLibEgoTest 0 --useFastBoard 3 --playNearMode 0 --numUpdateFormulaeMoveSelector 16 --delta 0.00001 --useRLInitializor 10 --useNewTrainOneGame 3 --srandOnTime 1 --optimizeSelectMove -1 --nbTotalSimulations 1000 --nbThreads 1 --useBitmapNeighborhood 0 --coefParam 1. --coefPow 0.5



void MoGo::InnerMCGoPlayer::logNodeInfosForStats(const NodeSequence &currentSequence, const Goban &goban) {/**assert(0);**/
    GobanNode *root=currentSequence[0];
    Vector<GobanNode *>preferedChilds;
    Vector < Location > firstSimulatedMoves = playMoveSelector->preferedMoves(root,goban,5, &preferedChilds);
    if (root->fatherNode().size()==0 && root->numberOfSimulation()>0) {/**assert(0);**/
        static bool first=true;
        static GobanNode *lastRoot=root;
        FILE *f=0;
        if (first) {/**assert(0);**/
            f=fopen("stats", "w");
        } else
            f=fopen("stats", "a");
        static Vector<int> nbSimuls; static Vector<double> values; static Vector<int> depths; static Vector<Vector<double> > bestMovesValues;
        // if (root->numberOfSimulation()) ;
        if (lastRoot!=root) {/**assert(0);**/
            for (int i=0;i<(int)nbSimuls.size();i++)
                fprintf(f, "%i %4.2f %i %4.2f %4.2f %4.2f ", nbSimuls[i], values[i], depths[i], bestMovesValues[i][0], bestMovesValues[i][1], bestMovesValues[i][2]);
            fprintf(f,"\n");
            if (goban.moveNumber()<2) {/**assert(0);**/
                for (int i=0;i<(int)nbSimuls.size();i++)
                    fprintf(f, "%i %4.2f 0 0 0 0 ", nbSimuls[i], randDouble());
                fprintf(f,"\n");
            }
            nbSimuls.clear(); values.clear(); depths.clear(); bestMovesValues.clear();
        }
        if (root->numberOfSimulation()>int(nbSimuls.size()*nbSimuls.size())) {/**assert(0);**/
            nbSimuls.push_back(int(root->numberOfSimulation()));
            values.push_back(root->nodeValue()[1]/double(root->numberOfSimulation()));
            bestMovesValues.push_back(Vector<double>());
            for (int i=0;i<(int)preferedChilds.size();i++) {/**assert(0);**/
                if (preferedChilds[i] && preferedChilds[i]->numberOfSimulation()>0)
                    bestMovesValues[bestMovesValues.size()-1].push_back(1.-preferedChilds[i]->nodeValue()[1]/double(preferedChilds[i]->numberOfSimulation()));
                else
                    bestMovesValues[bestMovesValues.size()-1].push_back(-1.);
            }
            depths.push_back(currentSequence.size());
        }
        lastRoot=root;
        first=false;
        fclose(f);
    }


    for (int i=0;i<(int)currentSequence.size()-1;i++) {/**assert(0);**/
        GobanNode *node=currentSequence[i];
        if (node->numberOfSimulation()<100) {/**assert(0);**/ //TODO
            node->nodeValue()[0]=node->nodeValue()[1]/double(node->numberOfSimulation()); // value when the tree is shallow
        } else {/**assert(0);**/
            double value=node->nodeValue()[1]/double(node->numberOfSimulation());
            if (fabs(value-node->nodeValue()[0])>0.1) {/**assert(0);**/ //TODO
                if (int(node->numberOfSimulation())%100==0) {/**assert(0);**/
                    Goban tmpGoban(node->gobanKey());
                    tmpGoban.textModeShowGoban();
                    GoTools::print("in the previous position (sims %i) sequence is:\n", node->numberOfSimulation());
                    GobanNode *tmpNode=node;
                    while (tmpNode->numberOfSimulation()>50) {/**assert(0);**/
                        int childIndex=-1;
                        const Location location=dynamic_cast<MoveSelectorGlobal *>(simulationMoveSelector)->selectOneMoveNew(tmpNode, goban, childIndex, 0);
                        goban.textModeShowPosition(location);
                        Goban(tmpNode->gobanKey()).textModeShowGoban();
                        if (childIndex<0) break;
                        tmpNode=tmpNode->childNode()[childIndex];
                    }
                    double newValue=tmpNode->nodeValue()[1]/double(tmpNode->numberOfSimulation()); if (tmpNode->isBlacksTurn()!=node->isBlacksTurn()) newValue=1.-newValue;
                    GoTools::print("Values are %f (was %f) (new %f/%i)\n", value, node->nodeValue()[0], newValue, tmpNode->numberOfSimulation());

                }
            }
        }

    }

}

void MoGo::InnerMCGoPlayer::tryUseSequencesInTree( int mTI, NodeSequence & /*currentSequence*/, const Vector< Location > & /*currentSequenceMove*/ ) {/**assert(0);**/
    if (interestingNodes.size()==0) return;
    int index=GoTools::mogoRand(interestingNodes.size());
    GobanNode *node=interestingNodes[index];

    if (node->numberOfSimulation()>=100) {/**assert(0);**/ //TODO
        //     node->nodeValue()[0]=node->nodeValue()[1]/double(node->numberOfSimulation()); // value when the tree is shallow
        //     double value=node->nodeValue()[1]/double(node->numberOfSimulation());
        /* GoTools::print("In this position:\n", index);
           Goban(currentSequence[currentSequence.size()-1]->gobanKey()).textModeShowGoban();
           GoTools::print("use sequence in tree %i\n", index);
           Goban(node->gobanKey()).textModeShowGoban();*/

        GobanNode *tmpNode=node;
        int numIterationAllAtFirst=allAtFirstData[mTI][0];
        while (tmpNode->numberOfSimulation()>50) {/**assert(0);**/
            int childIndex=-1;
            const Location location=dynamic_cast<MoveSelectorGlobal *>(simulationMoveSelector)->selectOneMoveNew(tmpNode, currentRootGoban, childIndex,1);
            //       Goban(node->gobanKey()).textModeShowPosition(location);
            bool success=true;

            if (useFastBoard) success=FastBoard::playOwnBoardForThread(location);
            if (success && location!=PASSMOVE) {/**assert(0);**/
                if (allAtFirstData[mTI][location]<=numIterationAllAtFirst) // if no stone has been played here before
                    if (tmpNode->isBlacksTurn()) {/**assert(0);**/ // so it was a white stone
                        allAtFirstData[mTI][location]=numIterationAllAtFirst+1;
                        usedMovesAllAtFirst[mTI].push_back(-location);
                    } else {/**assert(0);**/ // so it was a black stone played
                        allAtFirstData[mTI][location]=numIterationAllAtFirst+2;
                        usedMovesAllAtFirst[mTI].push_back(location);
                    }
            }

            if (childIndex<0 || !success) break;
            tmpNode=tmpNode->childNode()[childIndex];
        }
    }


}

void MoGo::InnerMCGoPlayer::registerSequencesInTree( int /*multiThreadsIdentifier*/, NodeSequence & currentSequence, const Vector< Location > & /*currentSequenceMove*/ ) {/**assert(0);**/
    for (int i=0;i<(int)currentSequence.size()-1;i++) {/**assert(0);**/
        GobanNode *node=currentSequence[i];
        if (node->numberOfSimulation()<100) {/**assert(0);**/ //TODO
            node->nodeValue()[0]=node->nodeValue()[1]/double(node->numberOfSimulation()); // value when the tree is shallow
        } else {/**assert(0);**/
            // GoTools::print("hello1\n");
            //       node->nodeValue()[0]=node->nodeValue()[1]/double(node->numberOfSimulation()); // value when the tree is shallow
            double value=node->nodeValue()[1]/double(node->numberOfSimulation());
            //  GoTools::print("hello2 %f %f\n", value, node->nodeValue()[0]);
            if (fabs(value-node->nodeValue()[0])>0.1) {/**assert(0);**/ //TODO
                bool found=false; for (int j=0;j<(int)interestingNodes.size();j++) if (interestingNodes[j]==node) found=true;
                if (!found && interestingNodes.size()<10) {/**assert(0);**/ //TODO
                    interestingNodes.push_back(node);
                    //           GoTools::print("add node %i\n", node);
                }

            }
        }
    }
}

void MoGo::InnerMCGoPlayer::handleSequencesInTree( int mTI, NodeSequence & currentSequence, const Vector< Location > & currentSequenceMove ) {/**assert(0);**/
    tryUseSequencesInTree(mTI, currentSequence, currentSequenceMove);
    registerSequencesInTree(mTI, currentSequence, currentSequenceMove);
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::InnerMCGoPlayer::handleCutTree(int iteration) {

    if (limitMemoryUsed<=0) {
        if ((collectorLimitTreeSize>0)&&((int)playerTree.hashMapSize()>=collectorLimitTreeSize)) // we want to cut the tree
        {
            cutTree();
        }
        if ((int)playerTree.hashMapSize()>=limitTreeSize) // tree too big, can't keep it in the memory
        {/**assert(0);**/
            GoTools::print("tree size limit reached. no more training.\n");
            return true;
        }
    } else {/**assert(0);**/
        if (iteration%10000==0) {/**assert(0);**/
            double memoryUsed=GoTools::memoryUsed();
            double additionalMemoryUsed=GoTools::additionalMemoryUsed();
            //       GoTools::print("additionalMemoryUsed %f, memoryUsed %f (%f) (%i) : %i (%i==%i)\n", additionalMemoryUsed, memoryUsed, limitMemoryUsed, playerTree.hashMapSize(), collectorLimitTreeSizeAutomatic, GobanNode::height, gobanSizeForSizePerNodeRecord);
            if (additionalMemoryUsed>10. && memoryUsed>limitMemoryUsed && playerTree.hashMapSize()>10000) {/**assert(0);**/

                if (gobanSizeForSizePerNodeRecord!=GobanNode::height) {/**assert(0);**/
                    sizePerNode=additionalMemoryUsed/double(playerTree.hashMapSize()+1);
                    gobanSizeForSizePerNodeRecord=GobanNode::height;
                    collectorLimitTreeSizeAutomatic=playerTree.hashMapSize();

                }

                if (int(playerTree.hashMapSize())>=collectorLimitTreeSizeAutomatic) cutTree();
            }
        }
    }


    return false;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayer::initializeThreadVariables( ) {
    blockVariable=false;saferBlockVariable=false;
    currentSequences.resize(NUM_THREADS_MAX); // less than NUM_THREADS_MAX threads !)
    currentSequenceMoves.resize(NUM_THREADS_MAX);
    allAtFirstData.resize(NUM_THREADS_MAX*COEF_CACHE_MISS_InnerMCGoPlayer);
    allAtFirstDataInTree.resize(NUM_THREADS_MAX*COEF_CACHE_MISS_InnerMCGoPlayer);
    territoriesScores.resize(NUM_THREADS_MAX);
    eyeScoresBlack.resize(NUM_THREADS_MAX, Vector<int>(1000)); // to handle every goban size. The memory usage is not a problem here.
    eyeScoresWhite.resize(NUM_THREADS_MAX, Vector<int>(1000)); // to handle every goban size. The memory usage is not a problem here.
    usedMovesAllAtFirst.resize(NUM_THREADS_MAX*COEF_CACHE_MISS_InnerMCGoPlayer);

    for (int i=0;i<NUM_THREADS_MAX;i++) {
        allAtFirstData[i*COEF_CACHE_MISS_InnerMCGoPlayer].resize(1000);// to handle every goban size. The memory usage is not a problem here.
        allAtFirstDataInTree[i*COEF_CACHE_MISS_InnerMCGoPlayer].resize(1000);// to handle every goban size. The memory usage is not a problem here.
        usedMovesAllAtFirst[i*COEF_CACHE_MISS_InnerMCGoPlayer].resize(1000);// to handle every goban size. The memory usage is not a problem here.
    }

    if (commonStack==0) {
        currentStateStack.resize(NUM_THREADS_MAX);
        for (int i=0;i<NUM_THREADS_MAX;i++) currentStateStack[i].resize(stackSizePerThread);
        currentStateStackIndex.resize(NUM_THREADS_MAX);
        for (int i=0;i<NUM_THREADS_MAX;i++) currentStateStackIndex[i].resize(stackSizePerThread);
    } else {/**assert(0);**/
        currentStateStack.resize(1); currentStateStackIndex.resize(1);
        currentStateStack[0].resize(NUM_THREADS_MAX*stackSizePerThread);

        currentStateStackIndex[0].resize(NUM_THREADS_MAX*stackSizePerThread);
    }
    currentSequenceMovesStack.resize(NUM_THREADS_MAX*stackSizePerThread);
    currentSequencesStack.resize(NUM_THREADS_MAX*stackSizePerThread);
    workerMutexes.resize(NUM_THREADS_MAX);

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    //          pthread_mutexattr_settype(&attr,/*PTHREAD_MUTEX_FAST_NP*//*PTHREAD_MUTEX_ADAPTIVE_NP*/45);
    pthread_mutex_init(&saferMutex,&attr);
    pthread_mutex_init(&saferOneGameMutex,&attr);
    for (int i=0;i<NUM_THREADS_MAX;i++) pthread_mutex_init(&workerMutexes[i],&attr);
    pthread_mutex_init(&mutexProtectNodes,&attr);
    pthread_mutex_init(&stackMutex,&attr);
    pthread_mutex_init(&initiateMutex,&attr);
}












// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayer::selfTrainOneGameAsynch(int /*iteration*/, const Goban &goban, int multiThreadsIdentifier, GobanNode *root) {
	vector<int> guessLocation;
	vector<int> guessColor;
	vector<double> guessWeight;
	double weightSimu;
    if (root==0) root=playerTree.getRootNodeAddress();

    GobanNode* lastNode = NULL;
    int mTI=multiThreadsIdentifier*COEF_CACHE_MISS_InnerMCGoPlayer;
    //   allAtFirstDataInTree[mTI][0]+=MAX_DEPTH_TREE_AMAF; // we are safe if depth of tree is < 100, which is quite conservative. // It should not more be used. I comment it out. Let's see what happens :-p.
    //   Goban &currentGoban=getGobanCopy(goban);
    MoveSelectorGlobal *simulationMoveSelectorTmp=dynamic_cast<MoveSelectorGlobal *>(simulationMoveSelector); assert(simulationMoveSelectorTmp);

    //here the tree part of the simulation (we possibly create a node here ?)
    int pass=0; double score=0.;
    if (!selfTrainOneGameAsynchCore(simulationMoveSelectorTmp, goban, /*currentGoban, */multiThreadsIdentifier, mTI, root, pass, score, &lastNode,weightSimu,&guessLocation,&guessColor,&guessWeight)) return;

    if(lastNode==NULL)
      assert(0);

    //std::cerr<<"nbMC "<<lastNode->nbMC<<std::endl;
    
    
    int choiceB = Goban::getThresholdSelfAtari();
    int choiceW = Goban::getThresholdSelfAtari();

    if(FBS_handler::board_size==9)
      {
	// UCB1 comes back in mogo
	double ucb1=-1.;
	
	for (int i=0;i<6;i++)
	  {
	    //std::cerr<<"winBlack "<<i<<" - "<<(lastNode->winBlack)[i]<<std::endl;
	    //std::cerr<<"lossBlack "<<i<<" - "<<(lastNode->lossBlack)[i]<<std::endl;
	    
	    
	    double nbTot=lastNode->winBlack[i]+lastNode->lossBlack[i];
	    double localUcb1=(lastNode->winBlack[i]+0.5)/(nbTot+1)+sqrt(0.2*log(lastNode->nbMC+1)/(1+nbTot));
	    if (localUcb1>ucb1)
	      {
		ucb1=localUcb1;
		choiceB=i+1;
	      }
	  }
	
	//std::cerr<<"black choice : "<<choiceB<<std::endl;
	
	ucb1=-1.;
	for (int i=0;i<6;i++)
	  {
	    //std::cerr<<"winWhite "<<i<<" - "<<(lastNode->winWhite)[i]<<std::endl;
	    //std::cerr<<"lossWhite "<<i<<" - "<<(lastNode->lossWhite)[i]<<std::endl;
	    
	    
	    double nbTot=lastNode->winWhite[i]+lastNode->lossWhite[i];
	    double localUcb1=(lastNode->winWhite[i]+0.5)/(nbTot+1)+sqrt(0.2*log(lastNode->nbMC+1)/(1+nbTot));
	    if (localUcb1>ucb1)
	      {
		ucb1=localUcb1;
		choiceW=i+1;
	      }
	  }
      }
    


    //std::cerr<<"white choice : "<<choiceW<<std::endl;
    
    
    // now the Monte-Carlo part
    if (pass>=3) {/**assert(0);**/
        slaveOneOperationAfterScore(score, goban, currentSequences[multiThreadsIdentifier], currentSequenceMoves[multiThreadsIdentifier], mTI,lastNode,choiceB,choiceW,weightSimu);
    }  else {
        slaveOneOperation(goban, currentSequences[multiThreadsIdentifier], currentSequenceMoves[multiThreadsIdentifier], mTI, lastNode,choiceB,choiceW,1,weightSimu,&guessLocation,&guessColor,&guessWeight);
    }

    unlockOneGameSafer(multiThreadsIdentifier);
    currentSequences[multiThreadsIdentifier].clear();
    currentSequenceMoves[multiThreadsIdentifier].clear();
}






















































/** Beginning of the main algorithm version, trying to be as asynchroneous as possible (and also as fast as possible :-p) */

// void MoGo::InnerMCGoPlayer::selfTrainAsynchMaster( const Goban & goban, const int number, const double time, int multiThreadsIdentifier, int mode ) {/**assert(0);**/
// }


// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::InnerMCGoPlayer::selfTrainOneGameAsynchInitiate( const Goban &goban, int multiThreadsIdentifier, GobanNode *root ) {

    if (useFastBoard) FastBoard::initiateOwnBoardForThread();

    currentSequences[multiThreadsIdentifier].clear();
    currentSequenceMoves[multiThreadsIdentifier].clear();
    //      assert(playerTree.isInTheTree(goban.getGobanKey()));
    currentSequences[multiThreadsIdentifier].push_back(root, -1, 0);
    //   currentSequences[multiThreadsIdentifier].push_BestValue(PASSMOVE, 0.);
    currentSequenceMoves[multiThreadsIdentifier].push_back(goban.lastMove());

    //   simulationMoveSelector->clearForbiddenMoves();
    usedMovesAllAtFirst[multiThreadsIdentifier*COEF_CACHE_MISS_InnerMCGoPlayer].clear();
}







// TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::InnerMCGoPlayer::selfTrainOneGameAsynchCore( MoveSelectorGlobal *simulationMoveSelectorTmp, const Goban & goban, /*Goban &currentGoban, */int multiThreadsIdentifier, int mTI, GobanNode * root, int &pass, double &score, GobanNode** lastNode,double& weightSimu,vector<int>* guessLocation,vector<int>* guessColor,vector<double>*guessWeight) {
    assert(useFastBoard);
    //   Goban currentGoban(goban);
    UCTUpdatorGlobal *treeUpdatorTmp=dynamic_cast<UCTUpdatorGlobal *>(treeUpdator); assert(treeUpdatorTmp);
    lockOneGameSafer(multiThreadsIdentifier);
    if (innerMCGoPlayerStaticV1==1) lockSafer();

    selfTrainOneGameAsynchInitiate(goban, multiThreadsIdentifier, root);

    pass=goban.lastMove()==PASSMOVE;
    int depth = 0; int securityCounter=0;
    GobanNode *node=root;

#ifdef GOLDENEYE
#define NB_SIMU_BEFORE_SEMEAI 10
        if (innerMCGoPlayerStaticV1==3) simulationMoveSelectorTmp->lockThisNode(node);
    if ((node->numberOfSimulation()>NB_SIMU_BEFORE_SEMEAI)&&(node->golden==0))
      { 
	cerr<<"nb simulations in the node : "<<node->numberOfSimulation()<<endl;

	int color = (FastBoard::getBoardForThread()->isBlacksTurn()) ? BLACK : WHITE;
	BasicInstinctFast::computeSlow(FastBoard::getBoardForThread(),color,&(node->guessLocation),&(node->guessColor),&(node->guessWeight),node);
	
#ifdef GOLDENEYE_MC
	node->semeaiW=FastBoard::getBoardForThread()->semeaiW;
	node->semeaiB=FastBoard::getBoardForThread()->semeaiB;
#endif
	
	node->golden=1;
      }
        if (innerMCGoPlayerStaticV1==3) simulationMoveSelectorTmp->unlockThisNode(node);
#endif

	
    // ONE GAME STARTS HERE
    // GoTools::print("start loop\n");
#ifdef GOLDENEYE
    guessLocation->resize(0);
    guessColor->resize(0);
    guessWeight->resize(0);
#endif
    weightSimu=1.;
    for (depth = 0 ; (pass < 2) && (depth < goban.width()*goban.height()*2) && (securityCounter<goban.width()*goban.height()*4) ; depth ++, securityCounter++) {
        int childIndex=-1;
#ifdef GOLDENEYE
	if (node->goldenbis)
	{
		(*guessLocation)=node->guessLocation;
	//	fprintf(stderr,"golden node!\n");
		(*guessColor)=node->guessColor;
		(*guessWeight)=(node->guessWeight);
	}
#endif
        if (innerMCGoPlayerStaticV1==2) lockSafer();
        if (innerMCGoPlayerStaticV1==3) simulationMoveSelectorTmp->lockThisNode(node);
#ifdef ANTILOCK
        node->isLocked=1;
#endif
#ifdef JYMODIF_NEW
	Location location=-1;






	int exploreNewNode=0;

	
	if (depth==0)
	{
#ifdef JY2
	if (totalSims*9<nbRemainingSimulations)
	{
		int nbLeg=0;
		int local = node->indexOnArray(0,0);
		for (int i=0;i<GobanNode::height;i++, local+=2)
		for (int j=0;j<GobanNode::width;j++, local++) 
			if (node->nodeUrgency()[local]>1e-10)
				nbLeg++;

		myIndex[mTI]=(myIndex[mTI]+1)%nbLeg;

		int bestLocation=-1;
		int myIdx=myIndex[mTI];

		local = node->indexOnArray(0,0);
		for (int i=0;i<GobanNode::height;i++, local+=2)
		for (int j=0;j<GobanNode::width;j++, local++) {
		if (node->nodeUrgency()[local] > 1e-10)
			{
				myIdx--;
				if (myIdx<0)
				{
					bestLocation=local;
					i=1000;
					break;
				}
			}
				
		}  // this move is illegal
		assert(bestLocation>=0);

	}
	else
#endif
	{
		//0. calculer k=k(n)
		//1. boucler sur les fils existants et garder les k(n) plus gros
		//2. si somme(n1-ni,1<i<=k) < nbRemainingSimulations
		//location=move(k)
		static double biggest[361];
		static int indexBiggest[361];
		int k = int( pow(nbRemainingSimulations+totalSims,0.25)/2.5);
	    //nbRemainingSimulations= int(0.9*(( (simulationTime-elapsedTime) / elapsedTime) * double(totalSims)));
		if (k<1) k=1;
		if (k>node->childNode().size()) {k=node->childNode().size();exploreNewNode=1;}
		k=2;
		if ((k>1)&&(int(node->childNode().size())>k+2)&&(!exploreNewNode))
		{
			for (int i=0;i<k;i++) {	biggest[i]=0; indexBiggest[i]=-1;}


			totalSims=0;
			for (int i=0;i<(int)node->childNode().size();i++) {
				Location move=node->childMove()[i];
				GobanNode *child=node->childNode()[i]; if (!child) continue; //FIXME : I have a strange bug here, child could not be 0!! Let's see what happen if I do that.
				assert(child);
				double numberOfSimulation_i=child->numberOfSimulation();
				//printf("from %d to \n",totalSims);
				totalSims+=int(numberOfSimulation_i);
				int indexInTab=0;
				while ((biggest[indexInTab]>numberOfSimulation_i)&&(indexInTab<k))
					indexInTab++;
				// indexInTab is the index where we must insert the new element
				for (int j=k-1;j>=indexInTab;j--)
				{
					biggest[j+1]=biggest[j];
					indexBiggest[j+1]=indexBiggest[j];

				}
				biggest[indexInTab]=numberOfSimulation_i;
				indexBiggest[indexInTab]=move;
				//printf("%d\n",totalSims);

			}

			double neededSimulations=0;
			for (int i=1;i<k;i++)
			{
				neededSimulations+=biggest[0]-biggest[i];
				if (int(neededSimulations)>nbRemainingSimulations)
				{
					location=indexBiggest[i];
					break;
				}
			}
				/*for (int i=0;i<k;i++)
					fprintf(stderr,"move %d is explored %d times\n",indexBiggest[i],int(biggest[i]));*/
				//fprintf(stderr,"==>we play %d\n",location);
		}
	}}













	if (location<0)
	  {
	    if (exploreNewNode)
	      location=simulationMoveSelectorTmp->selectOneMoveNewExplorator(node, goban/*currentGoban*/, childIndex, 0 );
	    else
	      location=simulationMoveSelectorTmp->selectOneMoveNew(node, goban/*currentGoban*/, childIndex, 0 );
	  }
#else
	Location location=simulationMoveSelectorTmp->selectOneMoveNew(node, goban/*currentGoban*/, childIndex, 0);
#endif
#ifdef ANTILOCK
        node->isLocked=0;
#endif

        if (innerMCGoPlayerStaticV1==3) simulationMoveSelectorTmp->unlockThisNode(node);
        if (innerMCGoPlayerStaticV1==2) unlockSafer();


	//assert(Goban(FastBoard::getBoardForThread()->getGobanKey()).isLegal(location)); FIXMEFIXMEFIXMEFIXME
        //            GoTools::print("urgentestLocation = %i, %i\n", location, pass);
        //     currentGoban.playMoveOnGoban(location);
        FastBoard::playOwnBoardForThread(location);
        int locationMadeACapture=false;
#ifdef KEEP_CAPTURE_MOVES_PER_NODE
        locationMadeACapture=FastBoard::getBoardForThread()->getOneCaptureOccured();
#endif

        if (location==PASSMOVE) pass++;
        else pass=0;
   

        GobanNode *childNodeChosen=0;
	
        if (selfTrainOneGameAsynchInTree(node, /*currentGoban, */location, multiThreadsIdentifier, childIndex, childNodeChosen, locationMadeACapture)) {
            break;
        }
        node=childNodeChosen;assert(node);
        if (checkSuperKoByLoop && isLoopInCurrentSequence(currentSequences[multiThreadsIdentifier], node)) {/**assert(0);**/
            pass=3; score=node->isBlacksTurn()?100.:-100.; // We make the last player loose the game
            break;
        }


#ifdef KEEP_CAPTURE_MOVES_PER_NODE
        location=location+(locationMadeACapture<<16); //16th bit to one if this is a capture
#endif
        if (node->numberOfSimulation()<exitTreeNbSimulations) { //not enough simulations, we do not create the node
            currentSequences[multiThreadsIdentifier].push_back(node, childIndex, 0);
            currentSequenceMoves[multiThreadsIdentifier].push_back(location);
            //currentSequences[multiThreadsIdentifier].push_BestValue(simulationMoveSelector->getHighestValueMoveInLastNode(), simulationMoveSelector->getHighestValueInLastNode());
            break;
        } else {
            if (!node->getInitiated()) { //we "really" create the node; there are sufficiently many simus, and TODOTODO 
                initiatePlayerGobanNodeAsynch(simulationMoveSelectorTmp, treeUpdatorTmp, goban, *FastBoard::getBoardForThread(), node, currentSequences[multiThreadsIdentifier]);
                //                    initiatePlayerGobanNode(currentGoban, node, currentSequences[multiThreadsIdentifier], true);
            }
#ifdef GOLDENEYE
	    else 
	    {
	      //#define NB_SIMU_BEFORE_SEMEAI 100
        if (innerMCGoPlayerStaticV1==3) simulationMoveSelectorTmp->lockThisNode(node);
	      if ((node->numberOfSimulation()>NB_SIMU_BEFORE_SEMEAI)&&(node->golden==0))
		{ 
		  cerr<<"nb simulations in the node : "<<node->numberOfSimulation()<<endl;

		  int color = (FastBoard::getBoardForThread()->isBlacksTurn()) ? BLACK : WHITE;
		  BasicInstinctFast::computeSlow(FastBoard::getBoardForThread(),color,&(node->guessLocation),&(node->guessColor),&(node->guessWeight),node);
		  
#ifdef GOLDENEYE_MC
		  node->semeaiW=FastBoard::getBoardForThread()->semeaiW;
		  node->semeaiB=FastBoard::getBoardForThread()->semeaiB;
#endif
		  
		  node->golden=1;
		}
        if (innerMCGoPlayerStaticV1==3) simulationMoveSelectorTmp->unlockThisNode(node);
	    }
#endif
            currentSequences[multiThreadsIdentifier].push_back(node, childIndex, 0);
            currentSequenceMoves[multiThreadsIdentifier].push_back(location);
            //currentSequences[multiThreadsIdentifier].push_BestValue(simulationMoveSelector->getHighestValueMoveInLastNode(), simulationMoveSelector->getHighestValueInLastNode());
        }
    }
    //   simulationMoveSelector->clearForbiddenMoves();
    (*lastNode)=node;
    //GoTools::print("get score!\n");
    if ((depth >= goban.width()*goban.height()+60) || (securityCounter >= goban.width()*goban.height()*4)) {/**assert(0);**///sometimes this happens. probably not very serious. not having time to check it yet.
        checkSuperKoByLoop=1;

        static int nbExit=0; nbExit++;
        if (nbExit%1000==0) {/**assert(0);**/
            if (!dontDisplay) {/**assert(0);**/
                GoTools::print("EXIT !!! %i %i (%i)\n", depth, securityCounter, nbExit);
               // for (int i=0;i<(int) currentSequences[multiThreadsIdentifier].size();i++)
                //    GoTools::print("node(%i)=%i\n", i, (int)currentSequences[multiThreadsIdentifier][i]);
                //         GoTools::print("isLoopInCurrentSequence %i\n", isLoopInCurrentSequence(currentSequences[multiThreadsIdentifier], currentSequences[multiThreadsIdentifier][currentSequences[multiThreadsIdentifier].size()-1] ));
            }
        }
        currentSequences[multiThreadsIdentifier].clear();
        currentSequenceMoves[multiThreadsIdentifier].clear();
        if (innerMCGoPlayerStaticV1==1) unlockSafer();
        unlockOneGameSafer(multiThreadsIdentifier);
        return false;
    }
 

    if (innerMCGoPlayerStaticV1==1) unlockSafer();
    return true;
}




// TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::InnerMCGoPlayer::selfTrainOneGameAsynchInTree( GobanNode *node, /*Goban &currentGoban, */Location location, int multiThreadsIdentifier, int childIndex, GobanNode *&childPlayed, int locationMadeACapture) {
    //      GoTools::print("call connectToTree %i -> %i\n", node, childPlayed);

	
    if (childIndex>=0) 
      {
        assert(childIndex<(int)node->childNode().size());
        childPlayed=node->childNode()[childIndex];
        return false;
      }

    /*  static int countGetGobanKey=0;
        countGetGobanKey++;
        if (countGetGobanKey%1000==0) GoTools::print("countGetGobanKey %i\n", countGetGobanKey);*/
    if (playerTree.connectToTree(node, FastBoard::getBoardForThread()->getGobanKey(), location, childIndex, childPlayed, true)) {

        currentSequences[multiThreadsIdentifier].push_back(childPlayed, childIndex, 0);
#ifdef KEEP_CAPTURE_MOVES_PER_NODE
        location=location+(locationMadeACapture<<16);
#else
        locationMadeACapture=0;
#endif
        currentSequenceMoves[multiThreadsIdentifier].push_back(location);

        //     static int countTrue=0;
        //     countTrue++;
        //     if (countTrue%1000==0) GoTools::print("countTrue %i\n", countTrue);


        return true;
    } else {
        /*    static int countFalse=0;
              countFalse++;
              if (countFalse%1000==0) GoTools::print("countFalse %i\n", countFalse);
        */
        return false;
    }
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
// we here initiate the info in a node
#ifdef GOLDENEYE
void MoGo::InnerMCGoPlayer::initiatePlayerGobanNodeAsynch( MoveSelectorGlobal *simulationMoveSelectorTmp, UCTUpdatorGlobal */*treeUpdatorTmp*/, const Goban & gobanAtRoot, FastBoard &board, GobanNode * node, const NodeSequence & currentSequence) const {
#else
void MoGo::InnerMCGoPlayer::initiatePlayerGobanNodeAsynch( MoveSelectorGlobal *simulationMoveSelectorTmp, UCTUpdatorGlobal */*treeUpdatorTmp*/, const Goban & gobanAtRoot, const FastBoard &board, GobanNode * node, const NodeSequence & currentSequence) const {
#endif
    if (innerMCGoPlayerStaticV1==3) {
        simulationMoveSelectorTmp->lockThisNode(node);
        if (node->getInitiated()) {
            simulationMoveSelectorTmp->unlockThisNode(node);
            return;
        }
    } else {/**assert(0);**/
        pthread_mutex_lock(&initiateMutex);
        if (node->getInitiated()) {/**assert(0);**/
            pthread_mutex_unlock(&initiateMutex);
            return;
        }
    }
    initiatePlayerGobanNodeUrgencyAsynch(simulationMoveSelectorTmp, gobanAtRoot, board, node, currentSequence);
  
    node->setInitiated(true);

    if (innerMCGoPlayerStaticV1==3) {
        simulationMoveSelectorTmp->unlockThisNode(node);
    } else {/**assert(0);**/
        pthread_mutex_unlock(&initiateMutex);
    }
}



// TEMPORARY COMMENT: THIS FUNCTION IS USED
#ifdef GOLDENEYE
void MoGo::InnerMCGoPlayer::initiatePlayerGobanNodeUrgencyAsynch(MoveSelectorGlobal *simulationMoveSelectorTmp, const Goban &gobanAtRoot, FastBoard &board, GobanNode *node, const NodeSequence & currentSequence) const {
#else
void MoGo::InnerMCGoPlayer::initiatePlayerGobanNodeUrgencyAsynch(MoveSelectorGlobal *simulationMoveSelectorTmp, const Goban &gobanAtRoot, const FastBoard &board, GobanNode *node, const NodeSequence & currentSequence) const {
#endif
  node->resizeUrgency();
  node->resizeExpertise();
  node->nodeValue().resize(3+(GobanNode::height+2)*(GobanNode::width+2));
  
  
  int numThread=0;
#ifdef PARALLELIZED
  numThread=ThreadsManagement::getNumThread(); //from 1 to nbThreads (not from 0 to nbThreads-1) 
#endif
  
  if(numThread!=0)
    numThread = numThread - 1;
  

  int before_last_move = PASSMOVE;
  
  if(currentSequenceMoves.size()>0)
    {
      const int& current_sequence_size = currentSequenceMoves[numThread].size();
      
      if((current_sequence_size>=2)
	 &&(currentSequenceMoves[numThread][current_sequence_size-2]>=0))
	{
	  before_last_move = currentSequenceMoves[numThread][current_sequence_size-2];

	  // cerr<<"sequence : ";
	  // 	  gobanAtRoot.textModeShowPosition(before_last_move); 
	  // 	  cerr<<" - size of current sequence : "<<current_sequence_size;
	  // 	  cerr<<" - numero de thread "<<numThread<<endl;
	  
	}
      else
	{
	  before_last_move = gobanAtRoot.lastLastMove();
	  
	  //  cerr<<"goban : ";
	  // 	  gobanAtRoot.textModeShowPosition(before_last_move); 
	  // 	  cerr<<" - size of current sequence : "<<current_sequence_size;
	  // 	  cerr<<" - numero de thread "<<numThread<<endl;
	  
	}
      
      
      //gobanAtRoot.textModeShowGoban(last_move);
     
    }
  
  simulationMoveSelectorTmp->setBeforeLastMove(before_last_move);
  
  simulationMoveSelectorTmp->initiateGobanNodeUrgencyWithFastBoard(node, gobanAtRoot, board, currentSequence, const_cast<GoGameTree *>(&playerTree));
}


bool MoGo::InnerMCGoPlayer::isLoopInCurrentSequence(const NodeSequence &currentSequence, GobanNode *node) const {/**assert(0);**/
    for (int i=0;i<(int)currentSequence.size();i++)
        if (node==currentSequence[i]) return true;
    return false;
}


Location MoGo::InnerMCGoPlayer::getOneMoveByFuseki(const Goban& goban) const
{

/*	fprintf(stderr,"=====================\n");
	fprintf(stderr,"lastMoves.size()=%d\n",lastMoves.size());
	for (int i=0;i<lastMoves.size();i++)
		fprintf(stderr,"lastMoves[%d]=%d\n",i,lastMoves[i]);
	fprintf(stderr,"=====================\n");*/

  if((goban.height()==19)&&(goban.width()==19))
    return getOneMoveByFuseki19(goban);
  
if((goban.height()==9)&&(goban.width()==9))
    return getOneMoveByFuseki19(goban);

 
 return PASSMOVE;
}

#ifdef GROSCAMBOUIS_SUPERFLU

Location MoGo::InnerMCGoPlayer::getOneMoveByFuseki19(const Goban& goban) const
{ 
  static int first = 1;
  static int finish = 0;
  
  if ((int)lastMoves.size() == 1 || (int)lastMoves.size() == 2)
    finish = 0;
  
  //  std::cerr << "getOneMoveByFuseki19..." << std::endl;
  
  if(finish == 1) return PASSMOVE;
  
  // Detection du joueur en fct du nombre de coup au démarrage
  if ((lastMoves.size()% 2) == 0)
    libfuseki_setPlayer(1);
  else
    libfuseki_setPlayer(0);
  
  if (first == 1)  {
    first = 0;
    
    libfuseki_addListDir(&pathsFuseki);

    // Fusion & sauvegarde des arbres dans un fichier sgf
    if((pathsFuseki!="")&&(saveTreesFuseki!=""))
      {
	string black_tree = "black_";
	black_tree+=saveTreesFuseki;

	string white_tree = "white_";
	white_tree+=saveTreesFuseki;
	
	if((saveTreesFuseki.size()<4)
	   ||(saveTreesFuseki.substr(saveTreesFuseki.size()-4,4)!=".sgf"))
	  {
	    black_tree+=".sgf";
	    white_tree+=".sgf";
	  }
	
	libfuseki_saveTreesSGF(black_tree, white_tree, goban.height());
      }
  }
  
  libfuseki_print();


  list<int> move_x;
  list<int> move_y;
  
  list<int> p_move_x ;
  list<int> p_move_y ;
  list<int> count ;
    
  std::cerr << "nb_last_moves : " << lastMoves.size() << std::endl;

  for(int i = 1; i < (int)lastMoves.size(); i++) {

      int r;// = goban.xIndex(lastMoves[i]);
      int c;// = goban.yIndex(lastMoves[i]);

      goban.getXYIndice(lastMoves[i],c,r);

      move_x.push_back(r);
      move_y.push_back(c); 

      std::cerr<< r << " " << c << std::endl;
    }
    
  std::cerr << "libfuseki_getmove ..." << std::endl;
  libfuseki_getMove(&move_x,&move_y,
		    &p_move_x,&p_move_y,
		    &count, goban.height());
  
  
  list<int> possible_moves;  
  list<int>::iterator itx_max, ity_max, itcount_max;
  list<int>::iterator itx, ity, itcount;

  // switch player
  if (libfuseki_getPlayer() == 0)
    libfuseki_setPlayer(1);
  else
    libfuseki_setPlayer(0);


  // check in the opponent tree
  list<float> count_float;
  float threshold_fuseki = 10.0;

  for (itx = p_move_x.begin(), ity =  p_move_y.begin(),
	   itcount = count.begin();
	 itx != p_move_x.end(); itx++, ity++, itcount++) {
    

    list<int> opp_move_x = move_x;
    list<int> opp_move_y = move_y;
    
    list<int> opp_p_move_x;
    list<int> opp_p_move_y;
    list<int> opp_count;
    
    opp_move_x.push_back(*itx);
    opp_move_y.push_back(*ity);

    libfuseki_getMove(&opp_move_x,   &opp_move_y,
		      &opp_p_move_x, &opp_p_move_y,
		      &opp_count,    goban.height());

    int opp_total = 0;
    for (list<int>::iterator opp_itcount = opp_count.begin();
	 opp_itcount != opp_count.end();
	 opp_itcount++)
      opp_total += (*opp_itcount);
    
#ifdef LEARNING
    float tmp_score = (float)((((float)(*itcount))+0.5) / (((float)opp_total + (float)(*itcount))+1.0) );
#else
    float tmp_score = (float)((float)(*itcount) / ((float)opp_total + (float)(*itcount)) * (float)(*itcount));
      if (tmp_score <= threshold_fuseki)
	tmp_score = 0.0;
#endif
    count_float.push_back(tmp_score);
    
    }


  // switch player
  if (libfuseki_getPlayer() == 0)
    libfuseki_setPlayer(1);
  else
    libfuseki_setPlayer(0);

  // Choix du coup
  float max_count = 0;
  list<float>::iterator it_float_count;
  for (itx = p_move_x.begin(), ity =  p_move_y.begin(),
	 it_float_count = count_float.begin();
       itx != p_move_x.end(); itx++, ity++, it_float_count++) {
    
    std::cerr<< (*itx) << " - " << (*ity) <<" : "<< *it_float_count <<std::endl;
    
    if(max_count < (*it_float_count)) {
      max_count = (*it_float_count);
      // itcount_max = it_float_count;
      itx_max     = itx;
      ity_max     = ity; 
    }
  }
#ifdef LEARNING
  if (max_count < 0.1)
  {
	  finish=1;
	  return PASSMOVE;
  }
#endif

  if (max_count == 0) {
    finish = 1;
    return PASSMOVE;
  }
  
  goban.textModeShowGoban();
  return goban.indexOnArray((*ity_max), (*itx_max));
}

#endif //end ifndef FUSEKI_RATIO


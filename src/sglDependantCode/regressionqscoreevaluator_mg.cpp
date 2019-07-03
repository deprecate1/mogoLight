//
// C++ Implementation: regressionqscoreevaluator_mg
//
// Description:
//
//
// Author: Yizao Wang and Sylvain Gelly <yizao.wang@polytechnique.edu and sylvain.gelly@m4x.org>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "regressionqscoreevaluator_mg.h"
#include <regression.h>
#include <QFile>
#include <QDataStream>
#include "goexperiments_mg.h"
#include "typedefs_mg.h"
#include "goban_mg.h"
#include "gogame_mg.h"
#include "mcscoreevaluator_mg.h"
#include "gtpgoplayer_mg.h"
#include "gobannode_mg.h"
#include "mcgoevaluator_mg.h"
//#include "fastsmallvector_mg.h"
#include "randomgoplayer_mg.h"
#include "tournament_mg.h"
#include "results_mg.h"
#include <stdio.h>
#include "gogametree_mg.h"
#include "mcgoplayer_mg.h"
#include "gtpengine_mg.h"
#include "cputimechronometer_mg.h"
#include "gotools_mg.h"
#include <ext/hash_map>
#include "goplayerfactory_mg.h"
#include "minimaxplayer_mg.h"
#include <testregressioncommandlinesgl.h>
#include <leaderthreadgelly.h>
#include <parametersgelly.h>
#include <javajni.h>
#include "regressionscoreevaluator_mg.h"
#include <vectorstatic.h>

using namespace std;

namespace MoGo {

  RegressionQScoreEvaluator::RegressionQScoreEvaluator()
      : QScoreEvaluator() {
    patternSize=5;
  }


  RegressionQScoreEvaluator::~RegressionQScoreEvaluator() {}}

double MoGo::RegressionQScoreEvaluator::evaluate( const Goban & goban, const Location location,double  ) {
  if (!regression) {
    if (!loadRegression()) {
      MoGo::GoTools::print("loadRegression failed\n");
      return 0.;
    }
  }
  int x=goban.xIndex(location);int y=goban.yIndex(location);
  return regression->getValue(toVector(RegressionScoreEvaluator::toVector(goban), x, y, goban.height(), patternSize));
}

Vector< double > MoGo::RegressionQScoreEvaluator::evaluate( const Goban & goban, const Vector< Location > & locations, double  ) {
  if (!regression) {
    if (!loadRegression()) {
      MoGo::GoTools::print("loadRegression failed\n");
      return Vector<double>(locations.size());
    }
  }
  Vector<double> gobanVector=RegressionScoreEvaluator::toVector(goban);
  Vector<double> res(locations.size());
  for (int i = 0 ; i < locations.size() ; i++) {
    int x=goban.xIndex(locations[i]);int y=goban.yIndex(locations[i]);
    Vector<double> v=toVector(gobanVector, x, y, goban.height(), patternSize);
    //     GVS::print(gobanVector);GVS::print(v);
    res[i] = regression->getValue(v);
  }
  return res;
}

Vector< double > MoGo::RegressionQScoreEvaluator::toVector( const Vector< double > & goban, int iOnGoban, int jOnGoban, int sizeGoban, int patternSize ) {
  assert((patternSize%2)==1);
  int dec=patternSize/2;
  assert((int)goban.size()==sizeGoban*sizeGoban);
  Vector<double> res(patternSize*patternSize*2);
  for (int i = 0 ; i < patternSize ; i++) {
    for (int j = 0 ; j < patternSize ; j++) {
      int index = getIndex(iOnGoban+i-dec,jOnGoban+j-dec, sizeGoban);
      assert(index < (int)goban.size());
      if (index < 0) {
        res[i*patternSize*2+j*2]=0;
        res[i*patternSize*2+j*2+1]=1;
      } else {
        res[i*patternSize*2+j*2]=goban[index];
        res[i*patternSize*2+j*2+1]=-1;
      }
    }
  }
  return res;
}

bool MoGo::RegressionQScoreEvaluator::loadRegression( ) {
  return RegressionScoreEvaluator::loadRegression(regression, "regression.qmodel");
}

int MoGo::RegressionQScoreEvaluator::getIndex( int i, int j, int sizeGoban ) {
  if (j<0)
    return -1;
  if (i<0)
    return -1;
  if (j>=sizeGoban)
    return -1;
  if (i>=sizeGoban)
    return -1;
  return i*sizeGoban+j;
}

bool MoGo::RegressionQScoreEvaluator::generateLearningValues( int argc, char ** argv ) {
  GoTools::print("generateLearningValues\n");
  MoGo::GoTools::mogoSRand();
  double komi=1.5; GoExperiments::findArgValue(argc,argv,"komi",komi);
  int nbGames=10; GoExperiments::findArgValue(argc,argv,"nbGames",nbGames);
  int nbSimulations=3; GoExperiments::findArgValue(argc,argv,"nbSimulations",nbSimulations);
  int patternSize=5; GoExperiments::findArgValue(argc,argv,"patternSize",patternSize);
  string nameOfLearningOuputFile="resLearning"; GoExperiments::findArgValue(argc,argv,"nameOfLearningOuputFile", nameOfLearningOuputFile);

  int iteration=0;
  //   GoPlayer *p = new MoGo::MinimaxPlayer(0,1,3,false);
  //       GTPGoPlayer *p=new GTPGoPlayer("gnugo");p->setVerbose(false);p->setFastPlay(true);p->setStochastic(true);
  MoGo::MCScoreEvaluator evaluator(0,0,0.,0.,true,true);evaluator.setKomi(komi);
  GTPGoPlayer *blackTmp = new GTPGoPlayer("gnugo");blackTmp->setFastPlay(true); blackTmp->setStochastic(true); GoPlayer *white = new RandomGoPlayer();
  GoPlayer *black=blackTmp;//GoPlayer *white=black;
  //    RandomGoPlayer *black = new RandomGoPlayer();GoPlayer *white=black;
  Vector<Goban> allGobans;
  for (int i=0 ; i < nbGames ; i++) {
    Vector<Goban> allGobans;Vector<Location> allMoves;
    Goban goban(9,9) ; bool displayGames=false;
    MoGoHashMap(GobanKey, int, hashGobanKey, equalGobanKey) currentSequence;
    //     GoPlayer *black = new MoGo::MinimaxPlayer(0,1,1,5,false);GoPlayer *white=black;

    int pass=0;
    Location lastMove=-1;
    black->start(goban);
    if (white != black)
      white->start(goban);
    do {
      /*double score=evaluator.scoreEstimation(goban,nbSimulations,false,true);
      if (!goban.isBlacksTurn())
        score = 1.-score;
      saveLearningResults(nameOfLearningOuputFile.c_str(),goban,score,iteration!=0);*/

      Location location;
      GoPlayer *currentPlayer=0; double timeAllowed=0.;
      if (goban.isBlacksTurn()) {
        timeAllowed=0.; currentPlayer = black;
      } else {
        timeAllowed = 0.; currentPlayer = white;
      }
      allGobans.push_back(goban);
      location = currentPlayer->generateAndPlay(lastMove,goban,timeAllowed);
      allMoves.push_back(location);
      if (black != white) lastMove=location;

      if (location==PASSMOVE) pass ++; else pass=0;

      if (displayGames)
        goban.textModeShowGoban(location);
      if (currentSequence.count(goban.getGobanKey())>0)
        pass=2; // if we find a superko, we stop the game
      currentSequence[goban.getGobanKey()] = 1;
      iteration ++;
    } while(pass<2);
    double score=goban.score();

    if (white->getName()=="gnugo")
      score=dynamic_cast<MoGo::GTPGoPlayer *>(white)->getScore();
    else if ((black!=white) && (black->getName()=="gnugo"))
      score=dynamic_cast<MoGo::GTPGoPlayer *>(black)->getScore();

    black->finish(score);
    if (white != black)
      white->finish(score);

    //     score=score>komi?1.:0.;
    for (int j = 0 ; j < (int)allGobans.size() ; j++) {
      saveLearningResults(nameOfLearningOuputFile.c_str(),allGobans[j],allMoves[j],score,patternSize,i+j!=0);
      score = -score;
    }
    GoPlayer *t=black;black=white;white=t;
  }

  if (white != black) delete white;
  delete black;


  return true;
}

void MoGo::RegressionQScoreEvaluator::saveLearningResults( const char * name, const Goban & goban, const Location moveLocation, double score, int patternSize, bool append ) {
  int x=goban.xIndex(moveLocation);int y=goban.yIndex(moveLocation);
  Vector<double> v=toVector(RegressionScoreEvaluator::toVector(goban), x, y, goban.height(), patternSize);

  FILE *f=0;
  if (append)
    f=fopen(name, "a");
  else
    f=fopen(name, "w");
  if (f==0)
    return ;
  fprintf(f, "%f ", score);
  for (int i = 0 ; i < (int)v.size() ; i++)
    fprintf(f, "%4.2f ", v[i]);
  fprintf(f, "\n");
  fclose(f);
}

bool MoGo::RegressionQScoreEvaluator::test( int argc, char ** argv ) {
  JavaJNI::setPreventUseOfJava(true) ;
  LeaderThread::getParameters()->setStringParameter("sgLibraryPath","/home/sylvain/Documents/boulot/these/travaux/codes/sgLibrary/");
  //   LeaderThread::getParameters()->setStringParameter("sgLibraryPath","/home/sylvain/boulot/codes/sgLibrary/");

  double timeAllowed=15.;int nbRuns=1;double komi=7.5;
  GoExperiments::findArgValue(argc,argv,"nbRuns",nbRuns);
  GoExperiments::findArgValue(argc,argv,"time",timeAllowed);
  GoExperiments::findArgValue(argc,argv,"komi",komi);
  double p1=1.; double p2=1.;
  GoExperiments::findArgValue(argc,argv,"p1",p1);GoExperiments::findArgValue(argc,argv,"p2",p2);

  RegressionQScoreEvaluator *evaluator = new RegressionQScoreEvaluator();//scoreEvaluator->evaluate(Goban(9,9));

  Vector<double> times;
  Vector<const MoGo::GoPlayerFactory *> factories;
  MoGo::RandomPlayerFactory random;
  MoGo::GnuGoPlayerFactory gnugoSlow;
  MoGo::GnuGoPlayerFactory gnugoFast(true);
  MoGo::GnuGoPlayerFactory gnugoFast2(true);
  MoGo::MinimaxPlayerFactory minimax(1,evaluator,false) ;

  MoGo::UCTSelector1 playSelector; MoGo::MoveSelector1 simulationSelector(4,0,1.,0,p1,p2); MoGo::UCTUpdator updator(1.,1);
  MoGo::InnerMCGoPlayerFactory innerMCGo(&simulationSelector,&playSelector,&updator);
  //      innerMCGo.setPlayerForFastFinishingGameFactory(&minimax);
  //   innerMCGo.setPlayerForFastFinishingGameFactory(&gnugoFast2);
  //    innerMCGo.setEvaluatorForFastFinishingGame(evaluator2);
  //   MoGo::InnerMCGoPlayerFactory innerMCGo2(&simulationSelector,&playSelector,&updator);

  //   factories.push_back(&minimax);times.push_back(timeAllowed);
  factories.push_back(&random);times.push_back(0.);
  //   factories.push_back(&innerMCGo);times.push_back(timeAllowed);
  //    factories.push_back(&random);times.push_back(0.);
  //   factories.push_back(&innerMCGo2);times.push_back(timeAllowed);
  //   factories.push_back(&gnugoSlow);times.push_back(timeAllowed);
  factories.push_back(&gnugoFast);times.push_back(timeAllowed);
  //   factories.push_back(&gnugoFast2);times.push_back(timeAllowed);



  MoGo::Tournament tournament(factories,nbRuns,komi,times);
  MoGo::Results results = tournament.runTournament(9,true,false);
  results.print();
  string name="outputMoGo";
  MoGo::GoTools::saveOutputMode();
  MoGo::GoTools::setOutputMode(FILE_MODE);
  MoGo::GoTools::changeFileName(name,false);

  //   MoGo::GoTools::print("%f %f %f %f %f\n", timeAllowed, p1, p2, results.getProbaVictory(0,1), results.getNbTotalMatchsPlayed(0,1));
  MoGo::GoTools::restoreOutputMode();
  results.print(name.c_str(),true);

  MoGo::GoTools::print("time taken : %f\n", tournament.getDurationOfTournament());
  //   tournament.saveAllGames();

  delete evaluator;
  return true;
}

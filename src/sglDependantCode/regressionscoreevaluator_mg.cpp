//
// C++ Implementation: regressionscoreevaluator_mg
//
// Description:
//
//
// Author: Yizao Wang and Sylvain Gelly <yizao.wang@polytechnique.edu and sylvain.gelly@m4x.org>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "regressionscoreevaluator_mg.h"
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
#include "qscoreevaluator_mg.h"

using namespace std;
namespace MoGo {

  RegressionScoreEvaluator::RegressionScoreEvaluator()
      : ScoreEvaluator() {}


  RegressionScoreEvaluator::~RegressionScoreEvaluator() {}}

double MoGo::RegressionScoreEvaluator::evaluate( const Goban & goban, double * certainty, double  ) {
  if (certainty)
    *certainty = 0;
  if (!regression) {
    if (!loadRegression()) {
      MoGo::GoTools::print("loadRegression failed\n");
      return 0.;
    }
  }
  if (goban.isScorable()) {
    if (goban.isBlacksTurn()) return goban.score(); else return -goban.score();
  }
  return regression->getValue(toVector(goban));
  //        return randDouble();
}

Vector< double > MoGo::RegressionScoreEvaluator::toVector( const Goban & goban ) {
  Vector<double> res(goban.height()*goban.width());

  int index=0;
  for (int i = 0 ; i < goban.height() ; i++)
    for (int j = 0 ; j < goban.width() ; j++) {
      PositionState s=goban.position(i,j);
      int liberty=goban.getStringLiberty(goban.indexOnArray(i,j));
      if (s == EMPTY)
        res[index]=0*liberty;
      else if (s == BLACK && goban.isBlacksTurn())
        res[index]=1*liberty;
      else if (s == BLACK && (!goban.isBlacksTurn()))
        res[index]=-1*liberty;
      else if (s == WHITE && goban.isBlacksTurn())
        res[index]=-1*liberty;
      else if (s == WHITE && (!goban.isBlacksTurn()))
        res[index]=1*liberty;
      else
        assert(0);
      index ++;
    }
  assert(index == res.size());
  return res;
}

bool MoGo::RegressionScoreEvaluator::loadRegression( ) {
  return loadRegression(regression, "regression2.model");
  //   return loadRegression(regression, "t.model");
}
bool MoGo::RegressionScoreEvaluator::loadRegression( ISP< Regression > & regression, const QString &name ) {
  /*  QFile f("/home/sylvain/boulot/codes/sgLibrary/bin/regression.model");
    if (!f.open(QIODevice::ReadOnly))
      return false;
    QDataStream in(&f);QString name;in >> name;
    GoTools::print("We found regression %s\n",name.toAscii().data());
    regression = Regression::createRegression(name) ;
   
    if (regression == 0)
      return false ;
    regression->undump(in) ;
    f.close() ;
   
    return regression != 0 ;*/
  //   QString path="/home/sylvain/Documents/boulot/these/travaux/codes/sgLibrary/";
  QString path="/home/sylvain/boulot/codes/sgLibrary/";
  regression = TestRegressionCommandLine::loadRegression(path+"/bin/"+name) ;
  return regression != 0;
}




bool MoGo::RegressionScoreEvaluator::test(int argc, char **argv) {
  JavaJNI::setPreventUseOfJava(true) ;
  //   LeaderThread::getParameters()->setStringParameter("sgLibraryPath","/home/sylvain/Documents/boulot/these/travaux/codes/sgLibrary/");
  LeaderThread::getParameters()->setStringParameter("sgLibraryPath","/home/sylvain/boulot/codes/sgLibrary/");

  double timeAllowed=15.;int nbRuns=1;double komi=7.5;
  GoExperiments::findArgValue(argc,argv,"nbRuns",nbRuns);
  GoExperiments::findArgValue(argc,argv,"time",timeAllowed);
  GoExperiments::findArgValue(argc,argv,"komi",komi);
  double p1=1.; double p2=1.;
  GoExperiments::findArgValue(argc,argv,"p1",p1);GoExperiments::findArgValue(argc,argv,"p2",p2);

  RegressionScoreEvaluator *scoreEvaluator = new RegressionScoreEvaluator();scoreEvaluator->evaluate(Goban(9,9));
  QScoreEvaluatorFromScore *evaluator = new QScoreEvaluatorFromScore(scoreEvaluator,true);
  MCScoreEvaluator *scoreEvaluator2 = new MCScoreEvaluator(0,0,0,0,false,false);scoreEvaluator2->setOptions(-1, true);
  QScoreEvaluatorFromScore *evaluator2 = new QScoreEvaluatorFromScore(scoreEvaluator2,true);

  Vector<double> times;
  Vector<const MoGo::GoPlayerFactory *> factories;
  MoGo::RandomPlayerFactory random;
  MoGo::GnuGoPlayerFactory gnugoSlow;
  MoGo::GnuGoPlayerFactory gnugoFast(true);
  MoGo::GnuGoPlayerFactory gnugoFast2(true);
  MoGo::MinimaxPlayerFactory minimax(1,evaluator,false) ;
  MoGo::MinimaxPlayerFactory minimax2(1,evaluator2,false) ;

  MoGo::UCTSelector1 playSelector; MoGo::MoveSelector1 simulationSelector(4,0,1.,0,p1,p2); MoGo::UCTUpdator updator(1.,1);
  MoGo::InnerMCGoPlayerFactory innerMCGo(&simulationSelector,&playSelector,&updator);
  //   innerMCGo.setPlayerForFastFinishingGameFactory(&minimax);
  //   innerMCGo.setPlayerForFastFinishingGameFactory(&gnugoFast2);
//   innerMCGo.setEvaluatorForFastFinishingGame(scoreEvaluator);
  //   innerMCGo.setCorrelationMode(true);
  //    innerMCGo.setNbSimulationsForEachNode(3);
  MoGo::InnerMCGoPlayerFactory innerMCGo2(&simulationSelector,&playSelector,&updator);

//   factories.push_back(&minimax);times.push_back(timeAllowed);
   factories.push_back(&minimax2);times.push_back(timeAllowed);
  //   factories.push_back(&random);times.push_back(0.);
     factories.push_back(&innerMCGo2);times.push_back(timeAllowed);
  //    factories.push_back(&random);times.push_back(0.);
//    factories.push_back(&innerMCGo);times.push_back(timeAllowed);
  //   factories.push_back(&gnugoSlow);times.push_back(timeAllowed);
  //   factories.push_back(&gnugoFast);times.push_back(timeAllowed);
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
  // delete evaluator2;
  return true;
}


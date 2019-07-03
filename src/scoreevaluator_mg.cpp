//
// C++ Implementation: scoreevaluator_mg
//
// Description:
//
//
// Author: Yizao Wang <yizao.wang@polytechnique.edu> and Sylvain Gelly <gelly@lri.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "scoreevaluator_mg.h"
#include "gotools_mg.h"
#include "treeupdator_mg.h"
#include "goexperiments_mg.h"
#include <math.h>
#include "fastboard_mg.h"
#include "smallsquarepatternsdb_mg.h"

#ifdef RL_SCORE
#include "RlMogoWrap.h"
void RlForceLink();
#endif



MoGoHashMap(long long, float, hashLongLong, equalLongLong) MoGo::SimpleHeuristicScoreEvaluator::patternElos;


namespace MoGo {

  ScoreEvaluator::ScoreEvaluator() {/**assert(0);**/this->komi = 7.5;}


  ScoreEvaluator::~ScoreEvaluator() {/**assert(0);**/}}

void MoGo::ScoreEvaluator::setKomi( double komi ) {/**assert(0);**/ this->komi = komi; }
Vector< double > MoGo::ScoreEvaluator::evaluateAll( const Goban & goban, double *certainty ) {/**assert(0);**/
  Vector<double> res(goban.height()*goban.width());
  int c=0;
  for (int i=0;i<goban.height();i++)
    for (int j=0;j<goban.width();j++) {/**assert(0);**/
      Location location = goban.indexOnArray(i,j);
      if (goban.isNotUseless(location)) {/**assert(0);**/
        Goban tmp(goban); tmp.playMoveOnGoban(location);
        res[c]=evaluate(tmp, 0, 0.);
      } else
        res[c]=0.;
      c++;
    }
  if (certainty) *certainty=1.;
  return res;
}

MoGo::ScoreEvaluator* MoGo::ScoreEvaluator::clone( ) const {/**assert(0);**/
  return 0;
}
void MoGo::ScoreEvaluator::loadRootGoban( const Goban &, const Location ) {/**assert(0);**/}

double MoGo::ScoreEvaluator::evaluate( Goban & , NodeSequence & ) {/**assert(0);**/
  GoTools::print("sorry but yizao has changed here. search for FIXME EVALUATE please.\n");
  return 0.;
}

bool MoGo::ScoreEvaluator::isUseful( const Goban & ,const double ) const {/**assert(0);**/
  ;
  return true;
}

void MoGo::ScoreEvaluator::loadRootGoban( const Goban & , const Vector< Location > & ) {/**assert(0);**/
  assert(0);

}

































//////////////////////////////////
// StupidCenterOpeningEvaluator //
//////////////////////////////////

MoGo::StupidCenterOpeningEvaluator::StupidCenterOpeningEvaluator( int mode , int height, int width ) {/**assert(0);**/
  this->mode = mode;
  this->height = height;
  this->width = width;
  initiateDefaultAreaBorder();
  initiateBonusLocation();
}


MoGo::StupidCenterOpeningEvaluator::~ StupidCenterOpeningEvaluator( ) {/**assert(0);**/}

void MoGo::StupidCenterOpeningEvaluator::setCenterOpeningAreaBorder( const Vector< Location > & openingAreaBorder ) {/**assert(0);**/
  this->openingAreaBorder.clear();
  this->openingAreaBorder.push_back(openingAreaBorder[openingAreaBorder.size()-1]);
  for (int i=0;i<(int)openingAreaBorder.size();i++)
    this->openingAreaBorder.push_back(openingAreaBorder[i]);
  this->openingAreaBorder.push_back(openingAreaBorder[0]);
}

void MoGo::StupidCenterOpeningEvaluator::initiateDefaultAreaBorder( ) {/**assert(0);**/
  openingAreaBorder.clear();
  if (height == 9 && width == 9) {/**assert(0);**/
    Goban goban(9,9);
    openingAreaBorder.push_back(goban.indexOnArray(3,2));
    for (int i=2;i<7;i++)
      openingAreaBorder.push_back(goban.indexOnArray(2,i));
    for (int i=3;i<7;i++)
      openingAreaBorder.push_back(goban.indexOnArray(i,6));
    for (int i=5;i>1;i--)
      openingAreaBorder.push_back(goban.indexOnArray(6,i));
    for (int i=5;i>1;i--)
      openingAreaBorder.push_back(goban.indexOnArray(i,2));
  }
}

double MoGo::StupidCenterOpeningEvaluator::evaluate( const Goban & goban,double* ,double ) {/**assert(0);**/
  if (goban.height()!=height || goban.width()!=width) {/**assert(0);**/
    GoTools::print("StupidCenterOpeningScorer does not have the same size.\n");
    return 0.5;
  }
  double score = 0.;

  countBorderScore(goban,score);
  //countStones(goban,score);
  countBonus(goban,score);
  //countUnderAtariStones(goban,score);
  assert(openingAreaBorder.size()>2);
  score = score/((double)openingAreaBorder.size()-2.)+.5;
  if (score < 0.) score = 0.;
  else if (score > 1.) score = 1.;
  return score;
}

void MoGo::StupidCenterOpeningEvaluator::countStones( const Goban & goban, double & score ) const {/**assert(0);**/
  for (int i=0;i<goban.height();i++)
    for (int j=0;j<goban.width();j++)
      if ((goban.gobanState[goban.indexOnArray(i,j)] == BLACK
           || goban.gobanState[goban.indexOnArray(i,j)] == WHITE) &&
          goban.stringInformation.stringLiberty[goban.indexOnArray(i,j)]>3)
        score += goban.gobanState[goban.indexOnArray(i,j)] == BLACK ? 1 : -1;

}

void MoGo::StupidCenterOpeningEvaluator::initiateBonusLocation( ) {/**assert(0);**/
  MoGo::Goban goban(9,9);
  for (int i=1;i<16;i+=2)
    bonusLocationIndex.push_back(i);
  /*
    bonusLocation.push_back(goban.indexOnArray(2,2));
    bonusLocation.push_back(goban.indexOnArray(2,6));
    bonusLocation.push_back(goban.indexOnArray(6,6));
    bonusLocation.push_back(goban.indexOnArray(6,2));
    bonusLocation.push_back(goban.indexOnArray(2,4));
    bonusLocation.push_back(goban.indexOnArray(4,2));
    bonusLocation.push_back(goban.indexOnArray(4,6));
    bonusLocation.push_back(goban.indexOnArray(6,4));
  */
  bonusLocation.push_back(goban.indexOnArray(3,3));
  bonusLocation.push_back(goban.indexOnArray(3,5));
  bonusLocation.push_back(goban.indexOnArray(5,5));
  bonusLocation.push_back(goban.indexOnArray(5,3));

  bonusLocation.push_back(goban.indexOnArray(2,2));
  bonusLocation.push_back(goban.indexOnArray(2,6));
  bonusLocation.push_back(goban.indexOnArray(6,6));
  bonusLocation.push_back(goban.indexOnArray(6,2));


}

void MoGo::StupidCenterOpeningEvaluator::countBonus( const Goban & goban, double & score ) const {/**assert(0);**/
  for (int i=0;i<(int)bonusLocation.size();i++)
    if (goban.gobanState[bonusLocation[i]]!=EMPTY)
      if (goban.stringInformation.getStringLiberty(bonusLocation[i])<=3)

        score += goban.gobanState[bonusLocation[i]] == BLACK ? -1 : 1;

      else if(goban.stringInformation.getStringLength(bonusLocation[i])>3)
        score += goban.gobanState[bonusLocation[i]] == BLACK ? 3 : -3;
  /*
    for (int i=0;i<(int)bonusLocationIndex.size();i++)
      if (goban.gobanState[openingAreaBorder[bonusLocationIndex[i]]]!=EMPTY)
        if (goban.stringInformation.getStringLiberty(openingAreaBorder[bonusLocationIndex[i]])<=3)
   
          score += goban.gobanState[openingAreaBorder[bonusLocationIndex[i]]] == BLACK ? -1 : 1;
   
        else if(goban.stringInformation.getStringLength(openingAreaBorder[bonusLocationIndex[i]])>3)
          score += goban.gobanState[openingAreaBorder[bonusLocationIndex[i]]] == BLACK ? 1 : -1;
  */

}





void MoGo::StupidCenterOpeningEvaluator::countBorderScore( const Goban & goban, double & score ) const {/**assert(0);**/
  int counter = 0;
  for (int i=1;i<(int)openingAreaBorder.size()-2;i++)
    if (goban.gobanState[openingAreaBorder[i]]==BLACK ||
        goban.gobanState[openingAreaBorder[i]]==WHITE) {/**assert(0);**/
      if (goban.gobanState[openingAreaBorder[i]]==goban.gobanState[openingAreaBorder[i-1]])
        counter ++;
      else {/**assert(0);**/
        if (counter > 2)
          score += (goban.gobanState[openingAreaBorder[i-1]]==BLACK) ? 1 : -1;
        counter = 0;
      }

      if (goban.stringInformation.stringLiberty[goban.stringInformation.stringNumber[openingAreaBorder[i]]]>3)
        score += (goban.gobanState[openingAreaBorder[i]]==BLACK) ? 1 : -1;
      else if (goban.getStringLiberty(openingAreaBorder[i])==2) {/**assert(0);**/
        score += (goban.gobanState[openingAreaBorder[i]]==BLACK) ? -1 : 1;
      }
      if (goban.stringInformation.stringLiberty[goban.stringInformation.stringNumber[openingAreaBorder[i]]]==1) {/**assert(0);**/
        score += (goban.gobanState[openingAreaBorder[i]]==BLACK) ? -1 : 1;
        if (goban.gobanState[openingAreaBorder[i-1]]!=goban.gobanState[openingAreaBorder[i]])
          score += (goban.gobanState[openingAreaBorder[i]]==BLACK) ? -1 : 1;
        if (goban.gobanState[openingAreaBorder[i+1]]!=goban.gobanState[openingAreaBorder[i]])
          score += (goban.gobanState[openingAreaBorder[i]]==BLACK) ? -1 : 1;
      }
    } else if (goban.gobanState[openingAreaBorder[i]]==EMPTY) {/**assert(0);**/} else {/**assert(0);**/
      goban.textModeShowPosition(openingAreaBorder[i]);
      assert(0);
    }

}

MoGo::ScoreEvaluator* MoGo::StupidCenterOpeningEvaluator::clone( ) const {/**assert(0);**/
  return new StupidCenterOpeningEvaluator(*this);
}

void MoGo::StupidCenterOpeningEvaluator::countUnderAtariStones( const Goban & goban, double & score ) const {/**assert(0);**/
  for (int i=1;i<(int)openingAreaBorder.size()-2;i++)
    if (goban.gobanState[openingAreaBorder[i]]!=EMPTY)
      if (goban.getStringLiberty(openingAreaBorder[i])==1)
        score += 2*goban.gobanState[openingAreaBorder[i]]==BLACK ? -2*goban.getStringLength(openingAreaBorder[i]) : goban.getStringLength(openingAreaBorder[i]);
}





























































































MoGo::RlScoreEvaluator::RlScoreEvaluator( RlMogoWrapper * rlWrapper, int learnMode, int useMode, bool useMC, double coefAmplitude ) {/**assert(0);**/
  this->rlWrapper=rlWrapper; this->useMC=useMC; this->coefAmplitude=coefAmplitude;//this->reset();
  this->learnMode=learnMode; this->useMode=useMode;
}




double MoGo::RlScoreEvaluator::evaluate( const Goban & goban, double * , double  ) {/**assert(0);**/
#ifdef RL_SCORE
  RlForceLink();
#endif
  Vector<int> position=goban.getGobanState();
  if (useMC) {/**assert(0);**/
    int win=rlWrapper->Simulate(position,(!goban.isBlacksTurn())+1);
    if (win)
      return komi+10;
    else
      return komi-10;
  } else {/**assert(0);**/
    //      goban.textModeShowGoban();
    double v=(double)rlWrapper->GetValue(position, (!goban.isBlacksTurn())+1);
    //      GoTools::print("v=%f\n",v);
    // getchar();
    //     return v>0.0?komi+10.:komi-10.;
    //     v=v/3.+0.5;
    //      if (v<0. || v>1.)
    //             GoTools::print("v=%f\n",v);
    //     v=atan(v)/3.14159*2.+0.5;
    if (v<0.) v=0.; if (v>1.) v=1.;
    return v-10000.; //trick in the updated score
  }
}

bool MoGo::RlScoreEvaluator::isUseful( const Goban & , double ) const {/**assert(0);**/
  return true;
}

void MoGo::RlScoreEvaluator::loadRootGoban( const Goban &, const Vector< Location > & ) {/**assert(0);**/}

double MoGo::RlScoreEvaluator::evaluate( Goban & goban, NodeSequence &  ) {/**assert(0);**/
  return evaluate(goban, 0, 0.);
}

// #define DEBUG_ALL_RlScoreEvaluator
bool newGameCalled=false;//FIXME
bool blackToPlay=true;//FIXME
int nbMoves=0;//FIXME
Vector< double > MoGo::RlScoreEvaluator::evaluateAll( const Goban & goban, double *certainty ) {/**assert(0);**/
#ifdef DEBUG_ALL_RlScoreEvaluator
  GoTools::print("evaluateAll\n");
  // goban.textModeShowGoban();
#endif
  GoTools::print("evaluateAll in this position\n");
  goban.textModeShowGoban();
  Vector<int> position=goban.getGobanState();
  //   if (!newGameCalled) //
  rlWrapper->NewGame(position, (!goban.isBlacksTurn())+1);
  std::vector<float> values;
  rlWrapper->GetAllValues((!goban.isBlacksTurn())+1, values);
  Vector<double> res(values.size());
  double min=100., max=-100.;
  for (int i=0;i<(int)res.size();i++) {/**assert(0);**/
    double v=values[i];
    v=(v-0.5)*coefAmplitude+0.5; if (v>0.99) v=0.99; else if (v<0.01) v=0.01;
    res[i]=v;
    min=GoTools::min(v, min); max=GoTools::max(v, max);
  }
  if (useMode==1) {/**assert(0);**/
    if (max-min>1e-3) {/**assert(0);**/
      double a=1./(max-min); double b=-min/(max-min);
      for (int i=0;i<(int)res.size();i++) {/**assert(0);**/
        res[i]=a*res[i]+b;
      }
    }
  }
  if (certainty) *certainty=1.;
  return res;
}


void MoGo::RlScoreEvaluator::reset( ) {/**assert(0);**/
#ifdef DEBUG_ALL_RlScoreEvaluator
  GoTools::print("reset\n");
#endif
  rlWrapper->Reset();
  newGameCalled=false;//FIXME
  blackToPlay=true;//FIXME
}

void MoGo::RlScoreEvaluator::newGame( const Goban & goban ) {/**assert(0);**/
  if (learnMode==1) {/**assert(0);**/
    return;
  } else {/**assert(0);**/
    newGameCalled=true;//FIXME
    nbMoves=0;
    blackToPlay=goban.isBlacksTurn();//FIXME
#ifdef DEBUG_ALL_RlScoreEvaluator
    GoTools::print("newGame\n");
    goban.textModeShowGoban();
    getchar();
#endif
    Vector<int> position=goban.getGobanState();
    rlWrapper->NewGame(position, (!goban.isBlacksTurn())+1);
#ifdef DEBUG_ALL_RlScoreEvaluator
    rlWrapper->Print();
    std::vector<float> values;
    rlWrapper->GetAllValues((!goban.isBlacksTurn())+1, values);
    GoTools::print(values);
#endif

  }
}
void MoGo::RlScoreEvaluator::newGenmove( const Goban & goban ) {/**assert(0);**/
  if (learnMode==1) {/**assert(0);**/
    GoTools::print("Start RLGO thinking on the following position:\n");
    goban.textModeShowGoban();
    think(goban, 1000);
    GoTools::print("End RLGO thinking\n");
  }
}

void MoGo::RlScoreEvaluator::play( Location l, bool blackPlay ) {/**assert(0);**/
  if (learnMode==1) return;
#ifdef DEBUG_ALL_RlScoreEvaluator
  GoTools::print("play\n");
#endif
  assert(blackToPlay==blackPlay);
  blackToPlay=!blackPlay;



  rlWrapper->Play(GobanLocation::yLocation(l)+1,GobanLocation::xLocation(l)+1,(!blackPlay)+1);
  rlWrapper->Update(0.,false);

  if (nbMoves==0) {/**assert(0);**/
    static int count=0;
    if (count%100==0) {/**assert(0);**/
      GoTools::print("count = %i, value = %f\n", count, rlWrapper->GetCurrentValue());
    }
    count++;
  }
  nbMoves++;

#ifdef DEBUG_ALL_RlScoreEvaluator
  Goban tmp;tmp.textModeShowPosition(tmp.symmetrize(l)); GoTools::print("\n");
  rlWrapper->Print();
#endif
}

void MoGo::RlScoreEvaluator::update( bool blackWins ) {/**assert(0);**/
  if (learnMode==1) return;
#ifdef DEBUG_ALL_RlScoreEvaluator
  GoTools::print("update %i\n", blackWins);
#endif
  //   static int count=0; static int blackWinsCount=0;
  //   if (count%100==0) {/**assert(0);**/
  //     GoTools::print("countUpdate = %i, value = %f\n", count, double(blackWinsCount)/double(count));
  //   }
  //   count++; blackWinsCount+=blackWins;

  //   rlWrapper->Update((!blackWins)?1.:0.,true);
  //   if (blackToPlay)
  rlWrapper->Update((blackWins)?1.:0.,true);
  //   else
  //        rlWrapper->Update((!blackWins)?1.:0.,true);
  newGameCalled=false;//FIXME
}


void MoGo::RlScoreEvaluator::think( const Goban & goban, int numgames ) {/**assert(0);**/
  Vector<int> position=goban.getGobanState();
  rlWrapper->Think(position, (!goban.isBlacksTurn())+1, numgames);
}
























double MoGo::SimpleHeuristicScoreEvaluator::evaluate( const Goban & , double * , double  ) {/**assert(0);**/
  assert(0);
}

double MoGo::SimpleHeuristicScoreEvaluator::evaluate( Goban & , NodeSequence & ) {/**assert(0);**/
  assert(0);
  return 0.;
}

Vector< double > MoGo::SimpleHeuristicScoreEvaluator::evaluateAll( const Goban & goban, double *certainty ) {/**assert(0);**/
  Vector<double> res(goban.height()*goban.width());
  FastSmallVector<Location> savingMoves;
  int c=0;
  double max=0.; double min=1.;
  if (mode==3 || mode==5) {/**assert(0);**/
    savingMoves=goban.getSavingMovesForLastMoveAtari();
    for (int i=0;i<(int)savingMoves.size();i++) {/**assert(0);**/
      res[goban.xIndex(savingMoves[i])*goban.width()+goban.yIndex(savingMoves[i])]=1.;
    }
  }

  for (int i=0;i<goban.height();i++)
    for (int j=0;j<goban.width();j++) {/**assert(0);**/
      Location location = goban.indexOnArray(i,j);
      double v=0.;
      if (goban.isNotUseless(location)) {/**assert(0);**/
        if (res[i*goban.width()+j]>0.)
          v=1.;
        else {/**assert(0);**/
          v=evaluateMoveByHeuristic(goban, location);
          v=(v-0.5)*coefAmplitude+0.5;
        }
        if (v<=0.01) v=0.01; else if (v>=0.99) v=0.99;
        max=GoTools::max(res[c],max);
        min=GoTools::min(res[c],min);
      }
      if (goban.isBlacksTurn())
        res[c]=v;
      else
        res[c]=1.-v;
      c++;
    }
  if (certainty) {/**assert(0);**/
    if (mode==2 || mode==5 || mode==6 || mode==8 || mode==9|| mode==10)
      *certainty=1.;
    else
      *certainty=GoTools::max(0.,max-min);
  }
 
  return res;
}

void MoGo::SimpleHeuristicScoreEvaluator::loadRootGoban( const Goban &, const Vector< Location > & ) {/**assert(0);**/}

bool MoGo::SimpleHeuristicScoreEvaluator::isUseful( const Goban & , double ) const {/**assert(0);**/
  return true;
}
double MoGo::SimpleHeuristicScoreEvaluator::evaluateMoveByHeuristic(const Goban &goban, Location move) {/**assert(0);**/
  if (mode==2 || mode==4) {/**assert(0);**/
    /* if (goban.isEatingMove(move))
       return 1.;
     else*/ if (goban.isSelfAtari(move))
      return 0.;
    else if (goban.isAtari(move))
      return 1.;
    else if (move==goban.saveStringOnAtari())
      return 1.;
    else if (goban.isOnGobanSide(move)) {/**assert(0);**/
      if (goban.isYoseOnSide(move))
        return 1.;
    }
    if (goban.isHane(move))
      return 1.;
    else if (goban.isCut(move))
      return 1.;
    //       else if (isCrossCut(location))
    //         interestingMoves.push_back(location);
    //       else if (isSolid(location))
    //         interestingMoves.push_back(location);
    return 0.5;
  } else if (mode==6 || mode==7) {/**assert(0);**/
    if (goban.isSelfAtari(move))
      return 0.;
    if (goban.isInterestingMoveBy2015441(move, mode==7))
      return 1.;
    else if (move==goban.saveStringOnAtari())
      return 1.;
    return 0.5;

  } else if (mode==8) {/**assert(0);**/
    if (goban.isSelfAtari(move))
      return 0.;
    if (move==goban.saveStringOnAtari())
      return 1.;
    else if (goban.isInterestingMoveBy2015441(move, mode==7))
      return .7;
    return 0.5;
  } else if (mode==3 || mode==5) {/**assert(0);**/
    if (goban.isSelfAtari(move))
      return 0.;
    else
      return 0.5;
  } else if (mode==9) {/**assert(0);**/
    int dx=100;int dy=100;
    if (goban.lastMove()!=PASSMOVE) {/**assert(0);**/
      dx=abs(goban.xIndex(goban.lastMove())-goban.xIndex(move));
      dy=abs(goban.yIndex(goban.lastMove())-goban.yIndex(move));
    }
    int distance=dx+dy+GoTools::max(dx,dy);
    double distanceModifier=0.;
    if (distance==2) distanceModifier=0.4;
    else if (distance==3) distanceModifier=0.25;
    else if (distance==4) distanceModifier=0.1;
    else if (distance>4) distanceModifier=-0.1;
    if (goban.isSelfAtari(move))
      return 0.+distanceModifier;
    if (goban.isAtari(move))
      return 0.9+distanceModifier;
    if (goban.isInterestingMoveBy2015441(move, mode==7))
      return 0.8+distanceModifier;
    else if (move==goban.saveStringOnAtari())
      return 1.+distanceModifier;
    return 0.5+distanceModifier;

  } else if (mode==10) {/**assert(0);**/

    int _selfAtari=goban.isSelfAtari(move);
    if (_selfAtari>0) {/**assert(0);**/
      if (_selfAtari>=2)
        if (goban.isAtari(move)) return 0.9;
     
      return 0.;
    }
    if (goban.isMakeKo(move)) return 1.;//FIXME

    if (goban.isInterestingMoveBy2015441(move, mode==7))
      return 1.;
    else if (move==goban.saveStringOnAtari())
      return 1.;
    return 0.5;
  }

  assert(0);
  return 0.;
}


double MoGo::SimpleHeuristicScoreEvaluator::evaluateMoveByHeuristicOnFastBoard( const FastBoard & board, Location move, Location saveMove, int numThread ) {/**assert(0);**/
  int v=board.interestingPatternValue(move);
  if (v<0) return -1.;


  int _selfAtari=board.isSelfAtari(move, numThread);
  if (_selfAtari>0) {/**assert(0);**/
    if (_selfAtari>=2)
      if (board.isAtari(move)) return 0.9;
    return 0.;
  }

  if (move==saveMove) return valueForSaveMove;

  if (v&1) {/**assert(0);**/
    return 1.;
  } else if (v>=2) {/**assert(0);**/
    if (board.isAtari(move)) {/**assert(0);**/
      return 1.;
    }
  }

  if (board.isMakeKo(move)) return 1.;
  //   if (board.maxSizeCaptureMove(move)==1) return 1.;
  //    if (board.maxSizeCaptureMove(move)>1) return 1.;
  return 0.5;
}




void MoGo::SimpleHeuristicScoreEvaluator::initialiseNodeUrgencyByFastBoard( const FastBoard & board, GobanNode * node, double equivalentSimulationsForInitialization ) {
  int numThread=0;
#ifdef PARALLELIZED
  numThread=ThreadsManagement::getNumThread();
#endif

	
  assert(mode==10);

  Location saveMove=board.saveStringOnAtari(numThread);
  // board.print();
  //   Goban goban;board.toGoban(goban);

  Location location=node->nodeUrgency().indexOnArray(0,0);
  for (int i=0;i<GobanNode::height;i++,location+=2)
    for (int j=0;j<GobanNode::width;j++,location++) {/**assert(0);**/
      double val2=evaluateMoveByHeuristicOnFastBoard(board, location, saveMove, numThread);
    
      if (val2>=0.) {/**assert(0);**/
#ifdef KEEP_CAPTURE_MOVES_PER_NODE
        if (board.isCaptureMove(location)) node->addCaptureMove(location);
#endif
        if (val2>=valueForSaveMove) {/**assert(0);**/
          node->nodeUrgency()[location]=val2*equivalentSimulationsForInitializationForSaveMove+0.1;
          node->nodeValue()[3+location]=equivalentSimulationsForInitializationForSaveMove;
        } else {/**assert(0);**/
          node->nodeUrgency()[location]=val2*equivalentSimulationsForInitialization+0.1;
          node->nodeValue()[3+location]=equivalentSimulationsForInitialization;
        }
      } else {
        node->nodeUrgency().resetNeverPlayUrgency(location);
        node->nodeValue()[3+location]=equivalentSimulationsForInitialization;
      }

    
    
    }

	
}









void MoGo::SimpleHeuristicScoreEvaluator::initialiseNodeUrgencyByFastBoardAndDistances( const FastBoard & board, GobanNode * node, double equivalentSimulationsForInitialization, double * distances ) {/**assert(0);**/
  int numThread=0;
#ifdef PARALLELIZED
  numThread=ThreadsManagement::getNumThread();
#endif

  assert(mode==10);

  Location saveMove=board.saveStringOnAtari(numThread);
  // board.print();
  //   Goban goban;board.toGoban(goban);

  Location location=node->nodeUrgency().indexOnArray(0,0);
  for (int i=0;i<GobanNode::height;i++,location+=2)
    for (int j=0;j<GobanNode::width;j++,location++) {/**assert(0);**/
      double val2=evaluateMoveByHeuristicOnFastBoard(board, location, saveMove, numThread);
      if (val2>=0.) {/**assert(0);**/
#ifdef KEEP_CAPTURE_MOVES_PER_NODE
        if (board.isCaptureMove(location)) node->addCaptureMove(location);
#endif

        double coef=equivalentSimulationsByDistance(distances[location]);
        //         if (i==0 || j==0) coef*=3.; else if (i==1 || j==1) coef*=1.5;
        //  GoTools::print("distance %f, coef %f, %i\n", distances[location], coef, location);
        if (val2>=valueForSaveMove) {/**assert(0);**/
          node->nodeUrgency()[location]=val2*equivalentSimulationsForInitializationForSaveMove+0.001;
          node->nodeValue()[3+location]=equivalentSimulationsForInitializationForSaveMove*coef;
        } else {/**assert(0);**/
          node->nodeUrgency()[location]=val2*equivalentSimulationsForInitialization+0.001;
          node->nodeValue()[3+location]=equivalentSimulationsForInitialization*coef;
        }
      } else {/**assert(0);**/
        node->nodeUrgency().resetNeverPlayUrgency(location);
        node->nodeValue()[3+location]=equivalentSimulationsForInitialization;
      }
    }
}

double MoGo::SimpleHeuristicScoreEvaluator::equivalentSimulationsByDistance( double distance ) const {/**assert(0);**/
  if (powDistance==1.)
    return coefDistance*(distance+decDistance);
  else
    return coefDistance*pow(distance+decDistance, powDistance);
}








void MoGo::SimpleHeuristicScoreEvaluator::initialiseNodeUrgencyByFastBoardAndDistancesAnd5x5( const FastBoard & board, GobanNode * node, double equivalentSimulationsForInitialization, double * distances ) {/**assert(0);**/
  Goban goban(GobanNode::height,GobanNode::height); board.toGoban(goban); //FIXME
  if (patternElos.size()==0) {/**assert(0);**/
    SmallSquarePatternsDB::load("patterns5x5", patternElos);
    assert(patternElos.size()>0);
  }

  int numThread=0;
#ifdef PARALLELIZED
  numThread=ThreadsManagement::getNumThread();
#endif

  assert(mode==10);

  Location saveMove=board.saveStringOnAtari(numThread);
  // board.print();
  //   Goban goban;board.toGoban(goban);
  //goban.textModeShowGoban();
  Location location=node->nodeUrgency().indexOnArray(0,0);
  for (int i=0;i<GobanNode::height;i++,location+=2)
    for (int j=0;j<GobanNode::width;j++,location++) {/**assert(0);**/
      double val2=evaluateMoveByHeuristicOnFastBoard(board, location, saveMove, numThread);
      long long representation=goban.locationPattern5x5One(location);
      double elo5x5=0.5;
      if (patternElos.count(representation)) {/**assert(0);**/
        // elo5x5=(patternElos[representation]+400.)/2400.;
        elo5x5=1./(1.+pow(10., (1000.-elo5x5)/400.));

      }
      assert(elo5x5>=0.);
      //       elo5x5=(elo5x5-0.5)*0.5+0.5;
      //       goban.textModeShowPosition(location);
      //       GoTools::print("elo5x5 %f (%f->%f), representation %i,%i", elo5x5,val2,sqrt(val2*elo5x5),
      //                      representation>>32LL, int(representation)/*(representation&((1LL<<32LL)-1LL))*/);
      //       getchar();
      if (val2>=0.) {/**assert(0);**/
        if (val2==0.5) {/**assert(0);**/
          val2*=elo5x5; val2=sqrt(val2); if (val2<=0.) val2=0.1; else if (val2>=1.) val2=0.9;
        }
        double coef=equivalentSimulationsByDistance(distances[location]);
        //         coef=1.;
        //  GoTools::print("distance %f, coef %f, %i\n", distances[location], coef, location);
        if (val2>=valueForSaveMove) {/**assert(0);**/
          node->nodeUrgency()[location]=val2*equivalentSimulationsForInitializationForSaveMove+0.001;
          node->nodeValue()[3+location]=equivalentSimulationsForInitializationForSaveMove*coef;
        } else {/**assert(0);**/
          node->nodeUrgency()[location]=val2*equivalentSimulationsForInitialization+0.001;
          node->nodeValue()[3+location]=equivalentSimulationsForInitialization*coef;
        }
      } else {/**assert(0);**/
        node->nodeUrgency().resetNeverPlayUrgency(location);
        node->nodeValue()[3+location]=equivalentSimulationsForInitialization;
      }
    }
}




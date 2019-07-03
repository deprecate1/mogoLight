//
// C++ Implementation: moveselector_mg
//
// Description:
//
//
// Author: Yizao Wang <yizao.wang@polytechnique.edu> and Sylvain Gelly <gelly@lri.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
//
//

//#define DEBUGMPI
//#define FANHUIPATCH

#include "basic_instinct_fast_mg.h"

#include "moveselector_mg.h"
#include "treeupdator_mg.h"
#include <math.h>
#include <stdlib.h>
#include "gotools_mg.h"
#include "randomgoplayer_mg.h"
#include <algorithm>
#include "scoreevaluator_mg.h"
#include "localpatternsdatabase_mg.h"

#include "innermcgoplayer_mg.h"
#include "fastboard_mg.h"
#ifdef MOGOMPI
#include "mpi.h"
extern int MPI_NUMBER;
extern int MPI_NUMERO;
#endif

#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

#ifdef PARALLELIZED
// #define PARALLELIZED_MOVE_SELECTOR
#endif

using namespace MoGo;

//double MoveSelectorGlobal::weight_stones_quantity = 1;
//double MoveSelectorGlobal::pow_stones_quantity = 0.5f;

//double MoveSelectorGlobal::weight_stones_quantity = 1.068f;
//double MoveSelectorGlobal::pow_stones_quantity = 0.323f;

double MoveSelectorGlobal::weight_stones_quantity = 1;
double MoveSelectorGlobal::pow_stones_quantity = 0.6f;

double MoveSelectorGlobal::coef_expertise = 0.02f;

#ifdef CRITICALITY
double MoveSelectorGlobal::coef_criticality = 0.005f;
double MoveSelectorGlobal::coef_variance = 0.001f;
#endif



////////////////////////
// Class MoveSelector //
////////////////////////
double randDouble() {/**assert(0);**/
  return double(MoGo::GoTools::mogoRand(258945))/258945.;
  //    return double(MoGo::GoTools::mogoRand()%256)/255.;
}
namespace MoGo {


// TEMPORARY COMMENT: THIS FUNCTION IS USED
  MoveSelector::MoveSelector() 
  {
    distance1 = 2;
    distance2 = 1;
    treeUpdator=0;
    localDatabase=0;
    localDatabaseMode=0;
    tenukiMode=0;
    initiateGobanNodeUrgencyWhenAsked=true;
    highestValueMoveInLastNode=-1;
    highestValueInLastNode=-1.;
  }


// TEMPORARY COMMENT: THIS FUNCTION IS USED
  MoveSelector::~MoveSelector() {}}

Location MoGo::MoveSelector::selectOneMove( GobanNode * node, const Goban & goban, const int ) {/**assert(0);**/
  for (int i = 0 ; i < 50 ; i ++) {/**assert(0);**/
    Location location = selectOneMoveUnsafe(node, goban);
    if (goban.isLegal(location) && goban.isNotUseless(location))
      return location;
  }
  return goban.randomOutOneMove();
}
void MoGo::MoveSelector::showForbiddenMoves( ) {/**assert(0);**/
  for (int i=0;i<(int)forbiddenMoves.size();i++)
    GoTools::print("%i ",forbiddenMoves[i]);
  GoTools::print("\n");

}

Location MoGo::MoveSelector::selectOneMoveUnsafe( GobanNode * , const Goban & ) {/**assert(0);**/
  return PASSMOVE;
}

void MoGo::MoveSelector::neverPlayMove( GobanNode * node, const Location & location ) {/**assert(0);**/
  node->nodeUrgency().resetUrgency(location,0);
}
void MoGo::MoveSelector::firstPlayMove( GobanNode * node, const Location & location ) {/**assert(0);**/
  node->nodeUrgency().resetUrgency(location,10000);
}



void MoGo::MoveSelector::updateUrgencyByCurrentSequence( const NodeSequence & currentSequence ) {/**assert(0);**/
  for (int i=0;i<(int)currentSequence.size();i++)
    currentSequence[i]->needToUpdateUrgencyOn();
}

Vector< Location > MoGo::MoveSelector::preferedMoves( GobanNode * , const Goban & , const int , Vector<GobanNode *> *preferedChilds) {/**assert(0);**/
  Vector < Location > preferedMoves_;
  preferedMoves_.push_back(PASSMOVE);
  if (preferedChilds) preferedChilds->push_back(0);
  return preferedMoves_;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::MoveSelector::clearForbiddenMoves( ) {
  forbiddenMoves.clear();
}

void MoGo::MoveSelector::addForbiddenMove( Location move ) {/**assert(0);**/
  forbiddenMoves.push_back(move);
}
void MoGo::MoveSelector::removeForbiddenMove( Location move ) {/**assert(0);**/
  for (int i=0;i<(int)forbiddenMoves.size();i++)
    if (forbiddenMoves[i]==move) {/**assert(0);**/
      if (forbiddenMoves.size()>1)
        forbiddenMoves[i]=forbiddenMoves[forbiddenMoves.size()-1];
      forbiddenMoves.pop_back();
      break;
    }
}






// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::MoveSelector::setLastMoves( Vector< Location > & lastMoves ) {
  this->lastMoves = lastMoves;
}






















void MoGo::MoveSelector::showCurrentSequenceBitmap( const NodeSequence & currentSequence) const {/**assert(0);**/
  //if ((int)currentSequence.size()<=2) return;
  Vector < Bitmap > currentSequenceBitmaps;
  for (int i=0;i<(int)currentSequence.size();i++) {/**assert(0);**/
    if (currentSequence[i]->isNodeBitmapCreated())
      currentSequenceBitmaps.push_back(currentSequence[i]->bitmap());
    else
      GoTools::print("node %d in currentSequence has no bitmap.\n",i);
  }
  GoTools::print("\n");
  GobanBitmap::showBitmap(currentSequenceBitmaps);
}



// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::MoveSelector::setOpeningMode( int mode, int openingMoveNumber ) {
  if (mode == 2) GoTools::print("mode=2\n");
  openingMode = mode;
  this->openingMoveNumber = openingMoveNumber;
}







//////////////////
// UCTSelector1 //
//////////////////

// TEMPORARY COMMENT: THIS FUNCTION IS USED
MoGo::UCTSelector1::UCTSelector1(int selectorMode) {
  this->selectorMode=selectorMode;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
MoGo::UCTSelector1::~ UCTSelector1( ) {}

Location MoGo::UCTSelector1::selectOneMove( GobanNode * node, const Goban & goban, const int depth) {     //// it is here that moves are ultimately decided!
  // WARNING! we use depth as a flag and not depth!
  double value=10000;
  int index=-1;
  for (int i=0;i<(int)node->childNode().size();i++) {
    int k=0; for (;k<forbiddenMoves.size();k++) if (node->childMove()[i]==forbiddenMoves[k]) break;
    if (k<forbiddenMoves.size()) continue;
    double valueTmp=valueOfLocation(node->childNode()[i]);
    if (valueTmp<value) {
      value=valueTmp;
      index=i;
    }
  }



  if (index == -1) {/**assert(0);**/
    GoTools::print("UCTSelector1 warnings. The current node given to selectOneMove is not explored, or have 0 child(even no PASSMOVE, impossible!)PASSMOVE is given.\n");
    return PASSMOVE;
  }
  //if (depth == -1)
  //GoTools::print("index = %d\n",index);
  // GoTools::print("depth = %d\n",depth);

  if (node->childMove()[index]==PASSMOVE && depth!=-123) {/**assert(0);**/
    Vector < Location > preferedMoves_ = preferedMoves(node,goban,10);
    //     GoTools::print("test moves other than PASSMOVE.\n");

    //        for (int j = 0;j<(int)preferedMoves_.size();j++)
    //           GoTools::print("preferedMoves_(%i) = %i\n", j, preferedMoves_[j]);
    for (int j=1;j<(int)preferedMoves_.size();j++) {/**assert(0);**/
      //       goban.textModeShowPosition(preferedMoves_[j]);
      if (!goban.isSelfAtari(preferedMoves_[j]))
        return preferedMoves_[j];
      else if (goban.isEatingMove(preferedMoves_[j]))
        return preferedMoves_[j];
    }
    return PASSMOVE;
  } else
  {
    return node->childMove()[index];

  }
}



Location MoGo::UCTSelector1::selectOneMoveMPI( GobanNode * node, const Goban & goban, const int depth) {/**assert(0);**/     //// it is here that moves are ultimately decided!
  // WARNING! we use depth as a flag and not depth!
#ifdef MPIMOGO
  double * values = new double[500];
  for (int j=0;j<500;j++) values[j]=0.;
#endif
  double value=10000;
  int index=-1;
  for (int i=0;i<(int)node->childNode().size();i++) {/*GoTools::print("considering a move\n");*//**assert(0);**/
    int k=0; for (;k<forbiddenMoves.size();k++) if (node->childMove()[i]==forbiddenMoves[k]) {GoTools::print("forbmove\n");break;}
    if (k<forbiddenMoves.size()) continue;
    double valueTmp=valueOfLocation(node->childNode()[i]);
#ifdef MPIMOGO
    assert(node->childMove()[i]>=0);
    assert(node->childMove()[i]<500);
    
    //PATCH JBH

    if(valueTmp<=0)
      {
	values[node->childMove()[i]]=10000000.f;
      }
    else
      {
	values[node->childMove()[i]]=1/valueTmp;
	assert(valueTmp>0);
      }

    //values[node->childMove()[i]]=1/valueTmp;
    //assert(valueTmp>0);

    //END PATCH JBH


#endif
    //fprintf(stderr,"valueTmp(%d)=%g\n",node->childMove()[i],valueTmp);fflush(stderr);
    if (valueTmp<value) {/**assert(0);**/
      value=valueTmp;
      index=i;
    }
  }

#ifdef MPIMOGO
  
  int bestLocation=-1;
  if (MPI_NUMERO>0)
    {/**assert(0);**/
      assert(MPI_Send((void*)values,500,MPI_DOUBLE,0,MPI_TAG_ESTIMATE,MPI_COMM_WORLD)==MPI_SUCCESS);
      assert(MPI_Recv((int*)&bestLocation,1,MPI_INT,0,MPI_TAG_ESTIMATE,MPI_COMM_WORLD,MPI_STATUS_IGNORE)==MPI_SUCCESS);
    }
  else
    {/**assert(0);**/
      double * otherValues = new double[500];
#ifdef DEBUGMPI
      for (int j=0;j<500;j++) otherValues[j]=0.;
      printf("proc0: ");
      for (int k=50;k<70;k++) printf("%g ",values[k]);
      printf("\n");
#endif
      for (int i=1;i<MPI_NUMBER;i++)
	{/**assert(0);**/
	  assert(MPI_Recv((void*)otherValues,500,MPI_DOUBLE,i,MPI_TAG_ESTIMATE,MPI_COMM_WORLD,MPI_STATUS_IGNORE)==MPI_SUCCESS);
#ifdef DEBUGMPI
	  printf("proc%d :",i);
	  for (int k=50;k<70;k++) printf("%g ",otherValues[k]);
	  printf("\n");fflush(stdout);
#endif
	  for (int j=0;j<500;j++)
	    values[j]+=otherValues[j];
	}
      double bestValue=0;
      for (int j=0;j<500;j++)
	if (values[j]>bestValue)
	  {/**assert(0);**/
	    bestValue=values[j];
	    bestLocation=j;
	    //printf("--> i choose %d\n",j);fflush(stdout);
	  }
      delete [] otherValues;
      for (int i=1;i<MPI_NUMBER;i++)
	assert(MPI_Send((void*)&bestLocation,1,MPI_INT,i,MPI_TAG_ESTIMATE,MPI_COMM_WORLD)==MPI_SUCCESS);
    }
  assert(bestLocation>=0);
  delete [] values ;
  //  printf("decision taken (%d) \n",MPI_NUMERO);fflush(stdout);
#endif






  if (index == -1) {/**assert(0);**/
    GoTools::print("UCTSelector1 MPI warnings. The current node given to selectOneMove is not explored, or have 0 child(even no PASSMOVE, impossible!)PASSMOVE is given.\n");
    return PASSMOVE;
  }
  //if (depth == -1)
  //GoTools::print("index = %d\n",index);
  // GoTools::print("depth = %d\n",depth);

  if (node->childMove()[index]==PASSMOVE && depth!=-123) {/**assert(0);**/
    Vector < Location > preferedMoves_ = preferedMoves(node,goban,10);
    //     GoTools::print("test moves other than PASSMOVE.\n");

    //        for (int j = 0;j<(int)preferedMoves_.size();j++)
    //           GoTools::print("preferedMoves_(%i) = %i\n", j, preferedMoves_[j]);
    for (int j=1;j<(int)preferedMoves_.size();j++) {/**assert(0);**/
      //       goban.textModeShowPosition(preferedMoves_[j]);
      if (!goban.isSelfAtari(preferedMoves_[j]))
        return preferedMoves_[j];
      else if (goban.isEatingMove(preferedMoves_[j]))
        return preferedMoves_[j];
    }
    return PASSMOVE;
  } else
    {/**assert(0);**/
#ifdef MPIMOGO
#ifdef DEBUGMPI
      printf("alone (%d), I would send %d, but I send %d\n",MPI_NUMERO,node->childMove()[index],bestLocation);fflush(stdout);
#endif
      return bestLocation;
#else
      return node->childMove()[index];
#endif
    
    }
}


#ifdef JYMODIF_NEW

double MoGo::UCTSelector1::valueOfLocation( const GobanNode * childNode ) const {
  if (childNode==0)
    return -1.;
  double numberOfSimulation_i=childNode->numberOfSimulation();
  double value=1.-(childNode->nodeValue()[1])/numberOfSimulation_i;//Empirical winning ratio
  //I assume that the root node is the first father (it might happen that childNode has several fathers and that even the root node has father(s))

  GobanNode * father = childNode->fatherNode()[0];// father of the node to be evaluated (childNode) 
  double maxNk=0.,Nk=0.;//maxNk
  for (int k=0;k<(int)father->childNode().size();k++) {
    GobanNode * child_k=father->childNode()[k];
    Nk=child_k->numberOfSimulation();
    if (Nk>maxNk) maxNk=Nk;
  }
  int i;//i will denote the index of the node to be evaluated
  for (i=0;i<(int)father->childNode().size();i++) {
    if (father->childNode()[i]==childNode)
      break;
  }
  Location move=father->childMove()[i];//move is the location associated to the node to be evaluated
  double nbAllAtFirstMovesForThisChild=father->nodeValue()[move+3];
  if (nbAllAtFirstMovesForThisChild < 0.5) nbAllAtFirstMovesForThisChild=1;
  double invNbAllAtFirstMovesForThisChild=1./nbAllAtFirstMovesForThisChild;
  double valueAMAF=(father->nodeUrgency()[move])*invNbAllAtFirstMovesForThisChild;
  double biasToUse;
  if (GobanNode::height==9) biasToUse=0.0015;
	  else
  if (GobanNode::height==13) biasToUse=0.001;
	  else
	biasToUse=0.000625;
  double coef=1.-nbAllAtFirstMovesForThisChild/(nbAllAtFirstMovesForThisChild+numberOfSimulation_i+numberOfSimulation_i*nbAllAtFirstMovesForThisChild*biasToUse);
  value=valueAMAF+coef*(value-valueAMAF);//This is the urgency of playing the node to be evaluated
  
  if (selectorMode==0) {/**assert(0);**/
    if (childNode->numberOfSimulation()<=0) return 1.;
    return childNode->nodeValue()[1]/childNode->numberOfSimulation();
  } else if (selectorMode==1) {/**assert(0);**/
    if (childNode->numberOfSimulation()<=0) return 1.;
  if ((!(value>=-0.001)) || (!(value<=1.001))) {/**assert(0);**/ GoTools::print("value=%f\n",value); assert(0);}
    double variance=childNode->nodeValue()[2]/numberOfSimulation_i-(childNode->nodeValue()[1]/numberOfSimulation_i)*(childNode->nodeValue()[1]/numberOfSimulation_i);
    //FIXME here variance may be a v v v little smaller than 0.
    double confidence=1./sqrt(numberOfSimulation_i)*(variance+1/sqrt(numberOfSimulation_i));
    //         if (confidence > .25) confidence=.25;
    return value+confidence;
  } else if (selectorMode==2) {//This is the selectorMode currently used (17/01/08)
    if (childNode->numberOfSimulation()<=0) return 1.;
    double tmp = 1./numberOfSimulation_i;
    assert(maxNk>0.);
    if (numberOfSimulation_i>0.9*maxNk) {
      tmp=1.-value;//Only nodes that have been played more than 0.9*maxNk will be played


      //JBH FIXME FIXME

    //   if(tmp<=0)
// 	{
// 	  std::cerr<<"NEGATIF: "<<tmp<<" - "<<value<<" - "<<valueAMAF<<" - "<<coef*(value-valueAMAF)<<endl;

      //example NEGATIF: -5.63779e-07 - 1 - 1.00001 - -8.6852e-06


      //But in the function selectOneMoveMPI, we have
      
      //double valueTmp=valueOfLocation(node->childNode()[i]);
      //...
      //values[node->childMove()[i]]=1/valueTmp;
      //assert(valueTmp>0);  --> BUG

      // 	}
      
      //tmp can be negative
      
      //
      
      //JBH END FIXME FIXME


    } else {
      tmp=1.;
    }
    return tmp; 
  } else
    assert(0);
  return -1.;
}

#else
// TEMPORARY COMMENT: THIS FUNCTION IS USED
double MoGo::UCTSelector1::valueOfLocation( const GobanNode * childNode ) const {
  if (childNode==0)
    return -1.;
  if (selectorMode==0) {/**assert(0);**/
    if (childNode->numberOfSimulation()<=0) return 1.;
    return childNode->nodeValue()[1]/childNode->numberOfSimulation();
  } else if (selectorMode==1) {/**assert(0);**/
    if (childNode->numberOfSimulation()<=0) return 1.;
    double numberOfSimulation_i=childNode->numberOfSimulation();
    double value=childNode->nodeValue()[1]/numberOfSimulation_i;
  if ((!(value>=-0.001)) || (!(value<=1.001))) {/**assert(0);**/ GoTools::print("value=%f\n",value); assert(0);}
    double variance=childNode->nodeValue()[2]/numberOfSimulation_i-(childNode->nodeValue()[1]/numberOfSimulation_i)*(childNode->nodeValue()[1]/numberOfSimulation_i);
    //FIXME here variance may be a v v v little smaller than 0.
    double confidence=1./sqrt(numberOfSimulation_i)*(variance+1/sqrt(numberOfSimulation_i));
    //         if (confidence > .25) confidence=.25;
    return value+confidence;
  } else if (selectorMode==2) {
    if (childNode->numberOfSimulation()<=0) return 1.;
    return 1./double(childNode->numberOfSimulation());
  } else
    assert(0);
  return -1.;
}
#endif
// TEMPORARY COMMENT: THIS FUNCTION IS USED
double MoGo::UCTSelector1::valueOfLocation( const GobanNode * node, const Location location ) const {
  for (int i=0;i<(int)node->childMove().size();i++)
    if (node->childMove()[i]==location)
      return valueOfLocation(node->childNode()[i]);
  return -1.;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
Vector< Location > MoGo::UCTSelector1::preferedMoves( GobanNode * node, const Goban & , const int number, Vector<GobanNode *> *preferedChilds ) {
  Vector < Location > preferedMoves_;
  Vector < double > preferedMovesValue;
  preferedMoves_.push_back(PASSMOVE); if (preferedChilds) preferedChilds->push_back(0);
  double passmoveValue = valueOfLocation(node,PASSMOVE);
  if (passmoveValue == -1.)
    preferedMovesValue.push_back(0.);
  else preferedMovesValue.push_back(1.-passmoveValue);
 

  for (int i=0;i<(int)node->childMove().size();i++) {
    if (node->childMove()[i]==PASSMOVE)
      continue;
    int l=0; for (;l<forbiddenMoves.size();l++) if (node->childMove()[i]==forbiddenMoves[l]) break;
    if (l<forbiddenMoves.size()) continue;
    double v=1.-valueOfLocation(node->childNode()[i]);
    int k;
    for (k=0;k<(int)preferedMoves_.size();k++)
      if (v>preferedMovesValue[k]) {
        preferedMovesValue.insert(preferedMovesValue.begin()+k,v);
        preferedMoves_.insert(preferedMoves_.begin()+k,node->childMove()[i]); if (preferedChilds) preferedChilds->insert(preferedChilds->begin()+k,node->childNode()[i]);
        break;
      }
    if (k==(int)preferedMoves_.size()) {
      preferedMovesValue.push_back(v);
      preferedMoves_.push_back(node->childMove()[i]);
      if (preferedChilds) preferedChilds->push_back(node->childNode()[i]);
    }
  }
  //     for (int j = 0;j<(int)preferedMovesValue.size();j++)
  //       GoTools::print("preferedMovesValue(%i) = %f\n", j, preferedMovesValue[j]);
  //     for (int j = 0;j<(int)preferedMoves_.size();j++)
  //       GoTools::print("preferedMoves_(%i) = %i\n", j, preferedMoves_[j]);

  if (int(preferedMoves_.size())>number) {
    preferedMoves_.resize(number);
    if (preferedChilds) preferedChilds->resize(number);
  }
  assert((int)preferedMoves_.size()<=number);
  return preferedMoves_;

}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
double MoGo::UCTSelector1::valueOfLocationForDisplay( const GobanNode * node, const Location location ) const {
  for (int i=0;i<(int)node->childMove().size();i++)
    if (node->childMove()[i]==location)
      return valueOfLocationForDisplay(node->childNode()[i]);
  return -1.;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
double MoGo::UCTSelector1::valueOfLocationForDisplay( const GobanNode * childNode ) const {
  if (childNode==0)
    return -1.;
  if (childNode->numberOfSimulation()<=0) return 1.;
  return childNode->nodeValue()[1]/childNode->numberOfSimulation();
}
















// TEMPORARY COMMENT: THIS FUNCTION IS USED
//  #define DEBUG_MoveSelectorGlobal
MoGo::MoveSelectorGlobal::MoveSelectorGlobal( int useDistancesByBlock, double powStarCountDown, int nBestSize, int startCountDown, int maxRandomOptimizeSelectMove, int useBitmapNeighborhood, int useNewTrainOneGame, int avoidDeltaForRoot, double deltaPow, double delta, double nbEqEstimator2, double bias2,double coefBonusAMAF, double biasInAMAFCoef, double powTerritoriesBonus, double movingConstantSpeed, int movingConstantMode, int keepBonusNbSimulations, int historySizeForPrecomputed, int optimizeSelectMove, double bias, double limitInterestingMoveCoef, int passOnlyIfNoChoice, double powForLogReplacement, int interestingMoveMode, double minInterestingMoves, double powInterestingMoves, double coefInterestingMoves, int hardLocalFirst, int randomModeInTree, int nbSimulationsRandomInTree, int equivalentSimulationsForInitialization, ScoreEvaluator *optionalInitialization, bool initiateGobanNodeUrgencyWhenAsked, int localFirst, int maxDepth, double territoriesLimit, bool initializeByGrandFather, int stopUsingGlobal, double explorationParam, double explorationParam2, double coefParam, double coefPow, int urgencyUpdatorMode, int urgencySelectorMode, double urgencyUpdatorParameter2, double Wamaf ) {
  this->Wamaf=Wamaf;
  this->initiateGobanNodeUrgencyWhenAsked=initiateGobanNodeUrgencyWhenAsked;
  this->localFirst=localFirst;
  this->maxDepth=maxDepth;
  this->territoriesLimit=territoriesLimit;
  this->initializeByGrandFather=initializeByGrandFather;
  this->urgencyUpdatorMode = urgencyUpdatorMode ;
  this->urgencySelectorMode = urgencySelectorMode ;
  this->urgencyUpdatorParameter2 = urgencyUpdatorParameter2 ;
  this->explorationParam=explorationParam;
  this->explorationParam2=explorationParam2;
  this->coefParam=coefParam;
  this->coefPow=coefPow;
  this->stopUsingGlobal=stopUsingGlobal;
  this->optionalInitialization=optionalInitialization;
  this->equivalentSimulationsForInitialization=equivalentSimulationsForInitialization;
  this->nbSimulationsRandomInTree=nbSimulationsRandomInTree;
  this->randomModeInTree=randomModeInTree;
  this->hardLocalFirst=hardLocalFirst;
  this->coefInterestingMoves=coefInterestingMoves;
  this->powInterestingMoves=powInterestingMoves;
  this->minInterestingMoves=minInterestingMoves;
  this->interestingMoveMode=interestingMoveMode;
  this->powForLogReplacement=powForLogReplacement;
  this->passOnlyIfNoChoice=passOnlyIfNoChoice;
  this->limitInterestingMoveCoef=limitInterestingMoveCoef;
  this->bias=bias;
  updatedUrgencyNode.resize(NUM_THREADS_MAX,0);
  //   urgency.resize(1000); // this way we can handle any goban size < sqrt(1000) which is a lot.
  urgencies.resize(NUM_THREADS_MAX,Vector<Vector<double> >(historySizeForPrecomputed, Vector<double>(1000)));
  historyOfUpdatedNodes.resize(NUM_THREADS_MAX,Vector<GobanNode *>(urgencies[0].size(),0));
  kForHistoryOfUpdatedNodes.resize(NUM_THREADS_MAX,0);
  lastKForHistoryOfUpdatedNodes.resize(NUM_THREADS_MAX,-1);

  this->optimizeSelectMove=optimizeSelectMove;
  this->keepBonusNbSimulations=keepBonusNbSimulations;
  this->movingConstantMode=movingConstantMode;
  this->movingConstantSpeed=movingConstantSpeed;

  this->powTerritoriesBonus=powTerritoriesBonus;
  this->biasInAMAFCoef=biasInAMAFCoef;
  this->coefBonusAMAF=coefBonusAMAF;

  this->nbEqEstimator2=nbEqEstimator2;
  this->bias2=bias2;
  this->deltaPow=deltaPow;
  this->delta=delta;
  this->avoidDeltaForRoot=avoidDeltaForRoot;
  this->useNewTrainOneGame=useNewTrainOneGame;
  this->useBitmapNeighborhood=useBitmapNeighborhood;
  this->maxRandomOptimizeSelectMove=maxRandomOptimizeSelectMove;
  this->startCountDown=startCountDown;
  this->nBestSize=nBestSize;
  this->powStarCountDown=powStarCountDown;
  this->useDistancesByBlock=useDistancesByBlock;

  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutex_init(&nodesMutex,&attr);
  pthread_mutex_init(&blockNodeMutex,&attr);

}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
MoGo::MoveSelectorGlobal::~MoveSelectorGlobal( ) {}

Location MoGo::MoveSelectorGlobal::selectOneMove( GobanNode * node, const Goban & goban, const int /*depth*/
#ifdef JYMODIF
, int &totalSims
#endif
 ) 
{/**assert(0);**/
  int childIndex=-1;

#ifdef JYMODIF
  return selectOneMoveNew(node, goban, childIndex,0,totalSims);
#else
  return selectOneMoveNew(node, goban, childIndex,0);
#endif
}



void MoGo::MoveSelectorGlobal::updateUrgencyByCurrentSequence( const NodeSequence & currentSequence ) {/**assert(0);**/
  for (int i=0;i<(int)currentSequence.size();i++)
    currentSequence[i]->needToUpdateUrgencyOn();
}

void MoGo::MoveSelectorGlobal::firstPlayMove( GobanNode * node, const Location & location ) {/**assert(0);**/
  node->nodeUrgency().resetFirstPlayUrgency(location);
}

void MoGo::MoveSelectorGlobal::neverPlayMove( GobanNode * node, const Location & location ) {/**assert(0);**/
  node->nodeUrgency().resetNeverPlayUrgency(location);
}




 // TEMPORARY COMMENT: THIS FUNCTION IS USED
#ifdef GOLDENEYE
void MoGo::MoveSelectorGlobal::initiateGobanNodeUrgencyWithFastBoard( GobanNode * node, const Goban &gobanAtRoot, FastBoard &board, const NodeSequence & /*currentSequence*/, GoGameTree *playerTree ) 
#else
void MoGo::MoveSelectorGlobal::initiateGobanNodeUrgencyWithFastBoard( GobanNode * node, const Goban &gobanAtRoot, const FastBoard &board, const NodeSequence & /*currentSequence*/, GoGameTree *playerTree ) 
#endif
{
  if (node->nodeUrgency().size()!=(GobanNode::height+2)*(GobanNode::width+2)) 
    {/**assert(0);**/
      //     Goban(node->gobanKey()).textModeShowGoban();
      GoTools::print("%i %i\n", node->nodeUrgency().size(),(GobanNode::height+2)*(GobanNode::width+2));
      assert(node->nodeUrgency().size()==(GobanNode::height+2)*(GobanNode::width+2));
    }
  
  node->nodeUrgency().setFirstPlayUrgency(0.0001);
  node->needToUpdateUrgencyOn();
  
  
  if (optionalInitialization && equivalentSimulationsForInitialization>0) {/**assert(0);**/
    // initiateGobanNodeUrgency(node,goban);
    updator()->stopUsingInitializationDirtyHack=0.;
    SimpleHeuristicScoreEvaluator *evaluator=dynamic_cast<SimpleHeuristicScoreEvaluator *>(optionalInitialization);
    assert(evaluator);
    if (useDistancesByBlock==0
        || board.lastMove()==PASSMOVE
        || board.koMove()!=PASSMOVE
        || (tenukiMode==1 && node->fatherNode().size()==0)) {
      evaluator->initialiseNodeUrgencyByFastBoard(board, node, equivalentSimulationsForInitialization);
    } else {
      int mTI=0;
#ifdef PARALLELIZED_MOVE_SELECTOR
      mTI=ThreadsManagement::getNumThread(); // mTI=multiThreadsIdentifier
#endif
      double *_urgency=&urgencies[mTI][0][0];
      board.computeDistancesByGroup(board.lastMove(), _urgency);
      
      if (useDistancesByBlock==2)
        evaluator->initialiseNodeUrgencyByFastBoardAndDistancesAnd5x5(board, node, equivalentSimulationsForInitialization, _urgency);
      else
	{/**assert(0);**/
	  evaluator->initialiseNodeUrgencyByFastBoardAndDistances(board, node, equivalentSimulationsForInitialization, _urgency);
	  
	  
	}

    }
  } else { assert(0);
    Location location=gobanAtRoot.indexOnArray(0,0);
    for (int i=0;i<GobanNode::height;i++,location+=2)
      for (int j=0;j<GobanNode::width;j++,location++) {
        int v=board.interestingPatternValue(location);
        if (v<0)
          node->nodeUrgency().resetNeverPlayUrgency(location);
        else
          node->nodeUrgency()[location]=0.0001;
        node->nodeValue()[3+location]=0.;
      }
  }


  node->nodeUrgency()[PASSMOVE]=0.3; // DANGEROUS CHANGE
  if (InnerMCGoPlayer::shishoCheckMode && node->isShallowerThan(InnerMCGoPlayer::shishoCheckMode-2)) {/**assert(0);**///shallow nodes only!
    assert(InnerMCGoPlayer::shishoCheckMode<=2);
    gobanAtRoot.initiateNodeUrgencyForShisho(node, playerTree);
  }
  if (InnerMCGoPlayer::shishoCheckModeNew && node->isShallowerThan(InnerMCGoPlayer::shishoCheckModeNew-1)) {/**assert(0);**/// shallow nodes only!
    assert(InnerMCGoPlayer::shishoCheckModeNew<=2);
    gobanAtRoot.initiateNodeUrgencyForShishoNew(node, playerTree);
  }
  //
  double basic_instinct[500];
  for(int i = 0; i<=FBS_handler::getBigArea(); i++) basic_instinct[i] = 0;
  
  //BasicInstinctFast bif(board,PASSMOVE);
  
  BasicInstinctFast bif(board,before_last_move);
  //!!!!!!!!!!!bif.compute(&basic_instinct);
  //bif.compute(basic_instinct);
  
  int color = (board.isBlacksTurn()) ? BLACK : WHITE;
  bif.compute(basic_instinct,color
  #ifdef CRITICALITY
  , node
  #endif
  );
#ifdef GOLDENEYE
 /* node->semeaiW=board.semeaiW;
  node->semeaiB=board.semeaiB;
  node->golden=0;*/
//  fprintf(stderr," on recoit dans node: %d W %d B\n",node->semeaiW.size(),node->semeaiB.size());
#endif

  //if((node->expertise).size()<0)
  //(node->expertise).resize(FBS_handler::board_big_area,0);
  
  /*
  for(int i = 0; i<441; i++)
    {
      (node->expertise)[i] = 0;
    }
  */
  
  Location location=gobanAtRoot.indexOnArray(0,0);
  for (int i=0;i<GobanNode::height;i++,location+=2)
    for (int j=0;j<GobanNode::width;j++,location++) 
      {
	double tmp = basic_instinct[location];
        if (node->nodeUrgency()[location]>0.0001)
	  {
	    //  	node->nodeValue()[3+location]+=WEIGHT_EXPERTISE;//2*WEIGHT_EXPERTISE;
	    //node->nodeUrgency()[location]+=(tmp+1)*WEIGHT_EXPERTISE;
	    if (tmp>0) {
	      node->nodeUrgency()[location]+=tmp*WEIGHT_EXPERTISE;
	      node->nodeValue()[location+3]+=tmp*WEIGHT_EXPERTISE;
	    }
	    else node->nodeValue()[location+3]-=tmp*WEIGHT_EXPERTISE;
          }
	
	/*
	  for this moment, expertise out of amaf is not validated on 9x9
	*/
	
	if(FBS_handler::board_size<=9) continue;
	
	if((basic_instinct)[location]>=0)/*&&(node->depth()==0)*/
	  (node->expertise())[location] = basic_instinct[location];
	else (node->expertise())[location] = 0;
      }
  


  //
}




// TEMPORARY COMMENT: THIS FUNCTION IS USED
Vector< Location > MoGo::MoveSelectorGlobal::preferedMoves( GobanNode * node, const Goban & , const int number, Vector<GobanNode *> *preferedChilds ) {
  Vector < Location > preferedMoves_;
  Vector < int > preferedMovesNumberOfSimulation;
  if (node->childMove().size()==0) return preferedMoves_;
  preferedMoves_.push_back(node->childMove()[0]); if (preferedChilds) preferedChilds->push_back(node->childNode()[0]);
  preferedMovesNumberOfSimulation.push_back((int)node->childNode()[0]->numberOfSimulation());

  for (int i=1;i<(int)node->childMove().size();i++) {
    int k = 0;
    for (k=0;k<(int)preferedMoves_.size();k++)
      if (node->childNode()[i]->numberOfSimulation()>=preferedMovesNumberOfSimulation[k]) {
        if ((int)preferedMoves_.size()<number) {
          preferedMoves_.push_back(0); if (preferedChilds) preferedChilds->push_back(0);
          preferedMovesNumberOfSimulation.push_back(0);
        }
        for (int l = preferedMoves_.size()-1; l>=k+1; l--) {
          preferedMoves_[l]=preferedMoves_[l-1]; if (preferedChilds) (*preferedChilds)[l]=(*preferedChilds)[l-1];
          preferedMovesNumberOfSimulation[l]=preferedMovesNumberOfSimulation[l-1];
        }
        preferedMoves_[k]=node->childMove()[i]; if (preferedChilds) (*preferedChilds)[k]=node->childNode()[i];
        preferedMovesNumberOfSimulation[k]=(int)node->childNode()[i]->numberOfSimulation();

        break;
      }
    if ( (int)preferedMoves_.size() < number && k == (int)preferedMoves_.size() ) {
      preferedMoves_.push_back(node->childMove()[i]); if (preferedChilds) preferedChilds->push_back(node->childNode()[i]);
      preferedMovesNumberOfSimulation.push_back((int)node->childNode()[i]->numberOfSimulation());
    }
  }
  assert((int)preferedMoves_.size()<=number);
  return preferedMoves_;
}



UCTUpdatorGlobal *MoGo::MoveSelectorGlobal::updator( ) {/**assert(0);**/
  return dynamic_cast<UCTUpdatorGlobal *>(treeUpdator);
}








double MoGo::MoveSelectorGlobal::saveValueForInterestingMoveValue(GobanNode * node, const Goban & , Location move, double v) {/**assert(0);**/
  if (interestingMoveMode==1000) {/**assert(0);**/
    assert(updator()->getUseEndTerritories());
    GobanNode *referenceNode=updator()->getReferenceNode(node, 0, 10);
    if (referenceNode!=0)
      v -= territoriesLimit*pow(fabs(referenceNode->getStats().getAverage(0, move)-0.5), powTerritoriesBonus);
  }

  if (node->getBonusPrecomputed().size()>0)
    node->getBonusPrecomputed()[move]=v;
  return v;
}




// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::MoveSelectorGlobal::clearCacheHistory( ) {
  for (int i=0;i<(int)historyOfUpdatedNodes.size();i++)
    for (int j=0;j<(int)historyOfUpdatedNodes[i].size();j++)
      historyOfUpdatedNodes[i][j]=0;


  for (int i=0;i<(int)updatedUrgencyNode.size();i++) {
    updatedUrgencyNode[i]=0;
    kForHistoryOfUpdatedNodes[i]=0;
    lastKForHistoryOfUpdatedNodes[i]=-1;
  }

}



// TEMPORARY COMMENT: THIS FUNCTION IS USED
Location MoGo::MoveSelectorGlobal::selectOneMoveNewExplorator( GobanNode * node, const Goban & goban, int &nextChildIndex, int mode) {
  if (useBitmapNeighborhood)
    updateNodeBitmap(goban, node);

  return selectOneMove17NewExplorator(node, goban, nextChildIndex, mode);
}



// TEMPORARY COMMENT: THIS FUNCTION IS USED
Location MoGo::MoveSelectorGlobal::selectOneMoveNew( GobanNode * node, const Goban & goban, int &nextChildIndex, int mode) {
  if (useBitmapNeighborhood)
    updateNodeBitmap(goban, node);

  return selectOneMove17New(node, goban, nextChildIndex, mode);
}




// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::MoveSelectorGlobal::lockThisNode( GobanNode * node ) {
#ifdef PARALLELIZED
# ifdef USE_MUTEX_PER_NODE
#ifdef ANTILOCK
  node->isLocked=1;
#endif


  pthread_mutex_lock(&node->nodeMutex);
  //    while (pthread_mutex_trylock(&node->nodeMutex));
# else
#  ifdef LINUX
  while (findOrAddBlockedNode(node))  pthread_yield();
#  else
  while (findOrAddBlockedNode(node));
#  endif
# endif
#else
  node=0;
#endif

}



bool MoGo::MoveSelectorGlobal::findOrAddBlockedNode(GobanNode *node) {/**assert(0);**/
  while (pthread_mutex_trylock(&nodesMutex));
  bool found=0;
  for (int i=0;i<(int)lockedNodes.size();i++)
    if (lockedNodes[i]==node) found=true;
  if (!found)
    lockedNodes.push_back(node);
  pthread_mutex_unlock(&nodesMutex);
  return found;
}

 // TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::MoveSelectorGlobal::unlockThisNode(GobanNode *node) {
#ifdef PARALLELIZED
#ifdef USE_MUTEX_PER_NODE
  pthread_mutex_unlock(&node->nodeMutex);
#ifdef ANTILOCK
  node->isLocked=0;
#endif

#else
  while (pthread_mutex_trylock(&nodesMutex));
  int foundIndex=-1;
  for (int i=0;i<(int)lockedNodes.size();i++)
    if (lockedNodes[i]==node) foundIndex=i;
  assert(foundIndex>=0);
  lockedNodes[foundIndex]=lockedNodes[lockedNodes.size()-1];
  lockedNodes.pop_back();
  pthread_mutex_unlock(&nodesMutex);
#endif
#else
  node=0;
#endif
}


void MoGo::MoveSelectorGlobal::updateNodeBitmap( const Goban & goban, GobanNode * node ) {/**assert(0);**/
  bool fullFill=goban.lastMove()==PASSMOVE || goban.koPosition()!=PASSMOVE || (tenukiMode>0 && node->fatherNode().size()==0);

  if (node->bitmap().size()==0) {/**assert(0);**/
    node->setNodeBitmapCreated(true);

    node->clearNeighborhood();
    if (!fullFill) {/**assert(0);**/
      node->addNeighborhoodToBitmap(goban, goban.lastMove());
      while (node->getEmptyLocationsSize()<5 && node->extendBitmapNeighborhood(goban))
        ;
    } else
      GobanBitmap::fullfill(node->bitmap());

    if (goban.lastLastMove()!=PASSMOVE)
      node->addNeighborhoodToBitmap(goban, goban.lastLastMove());
  }
  if (fullFill)
    return;
  double n=node->numberOfSimulation();
  n=coefParam*pow(n,coefPow);
  if (int(n)>node->getEmptyLocationsSize()) {/**assert(0);**/
    node->extendBitmapNeighborhood(goban);

  
  }
}



// TEMPORARY COMMENT: THIS FUNCTION IS USED
Location MoGo::MoveSelectorGlobal::selectOneMove17NewExplorator(GobanNode * node, const Goban & goban, int &nextChildIndex, int /*mode*/) {
  int nextMoveInfo=node->getNextInfoForMoveToChoose();
  Location nextMove=GobanNode::getNextLocation(nextMoveInfo);
  nextChildIndex=GobanNode::getNextChildIndex(nextMoveInfo);

  bool upToDate=node->isUrgencyUpToDate();

  // super strange line
  if ( optimizeSelectMove>0 && node->numberOfSimulation()>optimizeSelectMove && (!upToDate)) upToDate=GoTools::mogoRand(
          GoTools::min((int)sqrt(node->numberOfSimulation()), maxRandomOptimizeSelectMove)
        );
  if (upToDate) { //we do not have to compute anything
    if (nextMove>PASSMOVE)
      return nextMove;
  }
  int mTI=0;
#ifdef PARALLELIZED_MOVE_SELECTOR
  mTI=ThreadsManagement::getNumThread(); // mTI=multiThreadsIdentifier
#endif
  double *_urgency=&urgencies[mTI][0][0];



  Location urgentestLocation=PASSMOVE; double bestUrgency=0.; nextChildIndex=-1;
  //_urgency[PASSMOVE]=0.3; // PASSMOVE is never in the "all at first statistic", we have to put a value. If PASSMOVE is already a child, this value will be overwritten // DANGEROUS CHANGE
  //   if (goban.lastMove()==PASSMOVE /*&& (passOnlyIfNoChoice==0)*/) // avoid double pass in the tree //FIXMEFIXME
  _urgency[PASSMOVE]=0.1; // DANGEROUS CHANGE
  assert(int(node->nodeValue().size())>=3+(GobanNode::height+1)*(GobanNode::width+1));

  double biasToUse=bias*bias/0.25;//parameter of the combination between AMAF and norm

  Location location = node->indexOnArray(0,0);

  // GoTools::print("updateUrgency\n"); goban.textModeShowGoban();

  // heuristic for "helping" some children
  FastBoard *board=0; Location savingMove=-1;
  double coefInterestingMovesModified=0.; if (coefInterestingMoves>0.) {/**assert(0);**/
    board=FastBoard::getBoardForThread();
    if (powInterestingMoves==1.) coefInterestingMovesModified=coefInterestingMoves/double(node->numberOfSimulation()); else  coefInterestingMovesModified=coefInterestingMoves/pow(node->numberOfSimulation(),powInterestingMoves);
  }
  if (coefInterestingMovesModified<limitInterestingMoveCoef) coefInterestingMovesModified=0.;

  // this is a heuristic for speeding up the argmax by removing all the sequel of this function
  node->nBestSetSize(nBestSize);
  if (node->countDownForConsideringAllMoves<=0) {
    node->nBestClear();
    node->countDownForConsideringAllMoves=int(double(startCountDown)*pow(node->numberOfSimulation(), powStarCountDown))+1;
  } else {
    node->countDownForConsideringAllMoves--;
    return chooseBest17FromNBest(node, goban, nextChildIndex, biasToUse, coefInterestingMovesModified, board, mTI, savingMove);
  }

  // _urgency iniatialized at 0
  int size=(GobanNode::height+2)*(GobanNode::width+2)+location;
  for (int i=location;i<size;i++)
    _urgency[i]=0.;
  int nbExaeco=0;
  //here we compute the value for all children-nodes (non-children later)
#ifdef JYMODIF
  totalSims=0;
#endif
  
  float stones_density = 0;

  if(FBS_handler::board_size==9)
    {
      if(not board)
	board=FastBoard::getBoardForThread();
      stones_density = board->getStonesDensity();
    }
   

  for (int i=0;i<(int)node->childNode().size();i++) {
    Location move=node->childMove()[i];
    GobanNode *child=node->childNode()[i]; if (!child) continue; //FIXME : I have a strange bug here, child could not be 0!! Let's see what happen if I do that.
    assert(child);
    double numberOfSimulation_i=child->numberOfSimulation();
#ifdef JYMODIF
//printf("from %d to \n",totalSims);
totalSims+=int(numberOfSimulation_i);
//printf("%d\n",totalSims);
#endif
    double invNumberOfSimulation_i;

    if (numberOfSimulation_i>0) {  // This node has been created but not yet explored (can happen in multithread)
      invNumberOfSimulation_i=1./numberOfSimulation_i;
      double value=1.-child->nodeValue()[1]*invNumberOfSimulation_i;
      value=value<0.?0:value;
      value=value>1.?1:value;

#ifndef PARALLELIZED
      assert(numberOfSimulation_i>0);
#endif
#ifdef DEBUG_MoveSelectorGlobal
      GoTools::print("node->childMove()[i] %i: %g/%g\n", move, node->nodeUrgency()[move]/node->nodeValue()[move+3], node->nodeValue()[move+3]);
#endif
      double nbAllAtFirstMovesForThisChild=node->nodeValue()[move+3]; if (nbAllAtFirstMovesForThisChild < 0.5) nbAllAtFirstMovesForThisChild=1.;
      double invNbAllAtFirstMovesForThisChild=1./nbAllAtFirstMovesForThisChild;

      double valueAMAF=(node->nodeUrgency()[move])*invNbAllAtFirstMovesForThisChild;
      double bonus=0.; if (coefInterestingMovesModified>0. && move!=PASSMOVE) bonus=coefInterestingMovesModified*valueForInterestingMoveFastBoard(node, goban, *board, move, mTI, savingMove)+minInterestingMoves;
      // GoTools::print("bonus %f\n", bonus);

      double coef=1.-(nbAllAtFirstMovesForThisChild)/(nbAllAtFirstMovesForThisChild+numberOfSimulation_i+numberOfSimulation_i*nbAllAtFirstMovesForThisChild*biasToUse+weight_stones_quantity*pow((float)stones_density*FBS_handler::board_area,(float)pow_stones_quantity));
      
      if (move==PASSMOVE) coef=1.;

	if ((node->nodeValue()[1])/(node->numberOfSimulation())<0.5) coef/=0.9;
	if (coef>1) coef=1.;
      
	double expertise = node->get_expertise(move);
	//coef_expertise = 0.02f/(double)(node->depth()+1);

	expertise = coef_expertise*expertise/(log(numberOfSimulation_i+2));

    #ifdef CRITICALITY
    double criticality = node->getLocationCriticalityValue(move);
    criticality = (1.-coef) * coef_criticality * criticality;
    criticality -= coef_variance * child->VarianceTerritory;

    _urgency[move]=value*coef+(1.-coef)*valueAMAF+bonus+expertise+criticality;
    #else
    _urgency[move]=value*coef+(1.-coef)*valueAMAF+bonus+expertise;
    #endif

#ifdef FANHUIPATCH
    if (_urgency[move]<0.3) _urgency[move]=double(GoTools::mogoRand2(300))/1000.; //SUPER PATCH FAN HUI
#endif
	
    node->nBestInsert(_urgency[move], move, i);

      
        if ((_urgency[move]>bestUrgency) || ((_urgency[move]==bestUrgency) && (move<urgentestLocation))) { urgentestLocation=move;nbExaeco=0; bestUrgency=_urgency[move]; nextChildIndex=i;}
       
	
      // GoTools::print("Son urgentestLocation %i, val %f\n", urgentestLocation, _urgency[urgentestLocation]);
    } else
      _urgency[move]=0.;
  }
 // printf("mon resultat est %d\n",totalSims);
  // now we compute _urgency for children which are not yet nodes
  location = node->indexOnArray(0,0);
  for (int i=0;i<GobanNode::height;i++, location+=2)
    for (int j=0;j<GobanNode::width;j++, location++) {
      //       Location location = node->indexOnArray(i,j);
      if (node->nodeUrgency()[location] < 1e-10) { // this move is illegal
        _urgency[location]=0.;
        continue;
      } else if (_urgency[location]>1e-10) // this location has already a child
        continue;

      // if the number of visits is smaller than some constant, then apply the "distance"-heuristic
      if ((localFirst>0) && (node->numberOfSimulation()<localFirst)) {/**assert(0);**/ // we want to play local if not a lot of visits
        Bitmap *bitmap=&node->bitmap();assert(bitmap);
        if (!GobanBitmap::getBitValue(*bitmap,i,j))
          continue; // we don't consider this move
      } else if ((hardLocalFirst>0) && (node->numberOfSimulation()<hardLocalFirst) && (goban.lastMove()!=PASSMOVE)) {/**assert(0);**/
        Bitmap *bitmap=&node->bitmap();
        if (bitmap && (int)bitmap->size()==goban.height() && (!GobanBitmap::getBitValue(*bitmap,i,j)))
          continue; // we don't consider this move

        //the famous geometric trick
        int distance=goban.distance(goban.lastMove(), location);
        double maxDistance=double(goban.height())*sqrt(node->numberOfSimulation())/sqrt(hardLocalFirst)+1.;
        if (distance>(int)maxDistance)
          continue; // we don't consider this move
      } else if (useBitmapNeighborhood) {/**assert(0);**/
        Bitmap *bitmap=&node->bitmap();assert(bitmap);
        if (!GobanBitmap::getBitValue(*bitmap,i,j))
          continue; // we don't consider this move
      }

      double value=0.; double tmp=node->nodeValue()[location+3];
      if (tmp < 0.5) // no information for this node by all moves at first
        value=1000.;
      else { // only interesting if no child node corresponds to this move. The moves with a child have already been seen
        double bonus=0.; if (coefInterestingMovesModified>0. && location!=PASSMOVE) bonus=coefInterestingMovesModified*valueForInterestingMoveFastBoard(node, goban, *board, location, mTI, savingMove)+minInterestingMoves;
        double tmp2=1./tmp;
        double v=(node->nodeUrgency()[location])*tmp2;
        value=v+bonus;
      }
      _urgency[location]=value+100;

      node->nBestInsert(_urgency[location], location, -1);

    if ((_urgency[location]>bestUrgency) || ((_urgency[location]==bestUrgency) && (location<urgentestLocation))) { urgentestLocation=location; bestUrgency=_urgency[location];nextChildIndex=-1;}
    }


  if (_urgency[PASSMOVE]>bestUrgency) {
    urgentestLocation=PASSMOVE; nextChildIndex=-1;
    node->nBestInsert(_urgency[urgentestLocation], urgentestLocation, -1);

  }
  //   node->nodeUrgency()[3]=bestUrgency; //FIXME hack
  //   node->nodeUrgency()[4]=urgentestLocation; //FIXME hack

  nextMoveInfo=GobanNode::getNextInfo(urgentestLocation, nextChildIndex);
  node->setNextInfoForMoveToChoose(nextMoveInfo);

  node->needToUpdateUrgencyOff();

 
  // GoTools::print("urgentestLocation = %i, bestUrgency =%f\n", urgentestLocation, bestUrgency);
  return urgentestLocation;
}


// TEMPORARY COMMENT: THIS FUNCTION IS USED
Location MoGo::MoveSelectorGlobal::selectOneMove17New(GobanNode * node, const Goban & goban, int &nextChildIndex, int /*mode*/

#ifdef JYMODIF
, int & totalSims
#endif


) {
  int nextMoveInfo=node->getNextInfoForMoveToChoose();
  Location nextMove=GobanNode::getNextLocation(nextMoveInfo);
  nextChildIndex=GobanNode::getNextChildIndex(nextMoveInfo);

  bool upToDate=node->isUrgencyUpToDate();

  // super strange line
  if ( optimizeSelectMove>0 && node->numberOfSimulation()>optimizeSelectMove && (!upToDate)) upToDate=GoTools::mogoRand(
          GoTools::min((int)sqrt(node->numberOfSimulation()), maxRandomOptimizeSelectMove)
        );
  if (upToDate) { //we do not have to compute anything
    if (nextMove>PASSMOVE)
      return nextMove;
  }
  int mTI=0;
#ifdef PARALLELIZED_MOVE_SELECTOR
  mTI=ThreadsManagement::getNumThread(); // mTI=multiThreadsIdentifier
#endif
  double *_urgency=&urgencies[mTI][0][0];



  Location urgentestLocation=PASSMOVE; double bestUrgency=0.; nextChildIndex=-1;
  //_urgency[PASSMOVE]=0.3; // PASSMOVE is never in the "all at first statistic", we have to put a value. If PASSMOVE is already a child, this value will be overwritten // DANGEROUS CHANGE
  //   if (goban.lastMove()==PASSMOVE /*&& (passOnlyIfNoChoice==0)*/) // avoid double pass in the tree //FIXMEFIXME
  _urgency[PASSMOVE]=0.1; // DANGEROUS CHANGE
  assert(int(node->nodeValue().size())>=3+(GobanNode::height+1)*(GobanNode::width+1));

  double biasToUse=bias*bias/0.25;//parameter of the combination between AMAF and norm

  Location location = node->indexOnArray(0,0);

  // GoTools::print("updateUrgency\n"); goban.textModeShowGoban();

  // heuristic for "helping" some children
  FastBoard *board=0; Location savingMove=-1;
  double coefInterestingMovesModified=0.; if (coefInterestingMoves>0.) {/**assert(0);**/
    board=FastBoard::getBoardForThread();
    if (powInterestingMoves==1.) coefInterestingMovesModified=coefInterestingMoves/double(node->numberOfSimulation()); else  coefInterestingMovesModified=coefInterestingMoves/pow(node->numberOfSimulation(),powInterestingMoves);
  }
  if (coefInterestingMovesModified<limitInterestingMoveCoef) coefInterestingMovesModified=0.;

  // this is a heuristic for speeding up the argmax by removing all the sequel of this function
  node->nBestSetSize(nBestSize);
  if (node->countDownForConsideringAllMoves<=0) {
    node->nBestClear();
    node->countDownForConsideringAllMoves=int(double(startCountDown)*pow(node->numberOfSimulation(), powStarCountDown))+1;
  } else {
    node->countDownForConsideringAllMoves--;
    return chooseBest17FromNBest(node, goban, nextChildIndex, biasToUse, coefInterestingMovesModified, board, mTI, savingMove);
  }

  // _urgency iniatialized at 0
  int size=(GobanNode::height+2)*(GobanNode::width+2)+location;
  for (int i=location;i<size;i++)
    _urgency[i]=0.;
  int nbExaeco=0;
  //here we compute the value for all children-nodes (non-children later)
#ifdef JYMODIF
  totalSims=0;
#endif


  float stones_density = 0;

  if(FBS_handler::board_size==9)
    {
      if(not board)
	board=FastBoard::getBoardForThread();
      stones_density = board->getStonesDensity();
    }


  for (int i=0;i<(int)node->childNode().size();i++) {
    Location move=node->childMove()[i];
    GobanNode *child=node->childNode()[i]; if (!child) continue; //FIXME : I have a strange bug here, child could not be 0!! Let's see what happen if I do that.
    assert(child);
    double numberOfSimulation_i=child->numberOfSimulation();
#ifdef JYMODIF
//printf("from %d to \n",totalSims);
totalSims+=int(numberOfSimulation_i);
//printf("%d\n",totalSims);
#endif
    double invNumberOfSimulation_i;

    if (numberOfSimulation_i>0) {  // This node has been created but not yet explored (can happen in multithread)
      invNumberOfSimulation_i=1./numberOfSimulation_i;
      double value=1.-child->nodeValue()[1]*invNumberOfSimulation_i;
      value=value<0.?0:value;
      value=value>1.?1:value;

#ifndef PARALLELIZED
      assert(numberOfSimulation_i>0);
#endif
#ifdef DEBUG_MoveSelectorGlobal
      GoTools::print("node->childMove()[i] %i: %g/%g\n", move, node->nodeUrgency()[move]/node->nodeValue()[move+3], node->nodeValue()[move+3]);
#endif
      double nbAllAtFirstMovesForThisChild=node->nodeValue()[move+3]; if (nbAllAtFirstMovesForThisChild < 0.5) nbAllAtFirstMovesForThisChild=1.;
      double invNbAllAtFirstMovesForThisChild=1./nbAllAtFirstMovesForThisChild;

      double valueAMAF=(node->nodeUrgency()[move])*invNbAllAtFirstMovesForThisChild;
      double bonus=0.; if (coefInterestingMovesModified>0. && move!=PASSMOVE) bonus=coefInterestingMovesModified*valueForInterestingMoveFastBoard(node, goban, *board, move, mTI, savingMove)+minInterestingMoves;
      // GoTools::print("bonus %f\n", bonus);

      double coef=1.-(nbAllAtFirstMovesForThisChild)/(nbAllAtFirstMovesForThisChild+numberOfSimulation_i+numberOfSimulation_i*nbAllAtFirstMovesForThisChild*biasToUse+weight_stones_quantity*pow((float)stones_density*FBS_handler::board_area,(float)pow_stones_quantity));
						     
      if (move==PASSMOVE) coef=1.;

	if ((node->nodeValue()[1])/(node->numberOfSimulation())<0.5) coef/=0.9;
	if (coef>1) coef=1.;
      
	double expertise = node->get_expertise(move);
	//coef_expertise = 0.02f/(double)(node->depth()+1);
	
	expertise = coef_expertise*expertise/(log(numberOfSimulation_i+2));
	
    #ifdef CRITICALITY
    double criticality = node->getLocationCriticalityValue(move);
    criticality = (1.-coef) * coef_criticality * criticality;
    _urgency[move]=value*coef+(1.-coef)*valueAMAF+bonus+expertise+criticality;
    #else
    _urgency[move]=value*coef+(1.-coef)*valueAMAF+bonus+expertise;
    #endif

      
      node->nBestInsert(_urgency[move], move, i);

      
        if ((_urgency[move]>bestUrgency) || ((_urgency[move]==bestUrgency) && (move<urgentestLocation))) { urgentestLocation=move;nbExaeco=0; bestUrgency=_urgency[move]; nextChildIndex=i;}
       
	
      // GoTools::print("Son urgentestLocation %i, val %f\n", urgentestLocation, _urgency[urgentestLocation]);
    } else
      _urgency[move]=0.;
  }
 // printf("mon resultat est %d\n",totalSims);
  // now we compute _urgency for children which are not yet nodes
  location = node->indexOnArray(0,0);
  for (int i=0;i<GobanNode::height;i++, location+=2)
    for (int j=0;j<GobanNode::width;j++, location++) {
      //       Location location = node->indexOnArray(i,j);
      if (node->nodeUrgency()[location] < 1e-10) { // this move is illegal
        _urgency[location]=0.;
        continue;
      } else if (_urgency[location]>1e-10) // this location has already a child
        continue;

      // if the number of visits is smaller than some constant, then apply the "distance"-heuristic
      if ((localFirst>0) && (node->numberOfSimulation()<localFirst)) {/**assert(0);**/ // we want to play local if not a lot of visits
        Bitmap *bitmap=&node->bitmap();assert(bitmap);
        if (!GobanBitmap::getBitValue(*bitmap,i,j))
          continue; // we don't consider this move
      } else if ((hardLocalFirst>0) && (node->numberOfSimulation()<hardLocalFirst) && (goban.lastMove()!=PASSMOVE)) {/**assert(0);**/
        Bitmap *bitmap=&node->bitmap();
        if (bitmap && (int)bitmap->size()==goban.height() && (!GobanBitmap::getBitValue(*bitmap,i,j)))
          continue; // we don't consider this move

        //the famous geometric trick
        int distance=goban.distance(goban.lastMove(), location);
        double maxDistance=double(goban.height())*sqrt(node->numberOfSimulation())/sqrt(hardLocalFirst)+1.;
        if (distance>(int)maxDistance)
          continue; // we don't consider this move
      } else if (useBitmapNeighborhood) {/**assert(0);**/
        Bitmap *bitmap=&node->bitmap();assert(bitmap);
        if (!GobanBitmap::getBitValue(*bitmap,i,j))
          continue; // we don't consider this move
      }

      double value=0.; double tmp=node->nodeValue()[location+3];
      if (tmp < 0.5) // no information for this node by all moves at first
        value=1000.;
      else { // only interesting if no child node corresponds to this move. The moves with a child have already been seen
        double bonus=0.; if (coefInterestingMovesModified>0. && location!=PASSMOVE) bonus=coefInterestingMovesModified*valueForInterestingMoveFastBoard(node, goban, *board, location, mTI, savingMove)+minInterestingMoves;
        double tmp2=1./tmp;
        double v=(node->nodeUrgency()[location])*tmp2;
        value=v+bonus;
      }
      _urgency[location]=value;

      node->nBestInsert(_urgency[location], location, -1);

    if ((_urgency[location]>bestUrgency) || ((_urgency[location]==bestUrgency) && (location<urgentestLocation))) { urgentestLocation=location; bestUrgency=_urgency[location];nextChildIndex=-1;}
    }


  if (_urgency[PASSMOVE]>bestUrgency) {
    urgentestLocation=PASSMOVE; nextChildIndex=-1;
    node->nBestInsert(_urgency[urgentestLocation], urgentestLocation, -1);

  }
  //   node->nodeUrgency()[3]=bestUrgency; //FIXME hack
  //   node->nodeUrgency()[4]=urgentestLocation; //FIXME hack

  nextMoveInfo=GobanNode::getNextInfo(urgentestLocation, nextChildIndex);
  node->setNextInfoForMoveToChoose(nextMoveInfo);

  node->needToUpdateUrgencyOff();

 
  // GoTools::print("urgentestLocation = %i, bestUrgency =%f\n", urgentestLocation, bestUrgency);
  return urgentestLocation;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
Location MoGo::MoveSelectorGlobal::chooseBest17FromNBest(GobanNode * node, const Goban & goban, int &nextChildIndex, double biasToUse, double coefInterestingMovesModified, FastBoard *board, int mTI, Location savingMove) {
  Location urgentestLocation=PASSMOVE; double bestUrgency=-1.; int bestIndex=-1;
#ifdef ANTILOCK
  int lastLock=-1;
#endif

  int currentSize=node->nBestCurrentSize();
  if (currentSize<1) {/**assert(0);**/
    return PASSMOVE;
  }


  float stones_density = 0;
  if(FBS_handler::board_size==9)
    {
      if(not board)
	board=FastBoard::getBoardForThread();
      stones_density = board->getStonesDensity();
    }
  
  for (int i=0;i<(int)currentSize;i++) {
    Location move=node->nBestMove(i);
    int index=node->nBestIndex(i);
    //     GoTools::print("move %i, index %i, i %i\n", move, index, i);
    double v;
    if (index>=0) {
      v=greedyValueForMoveWithChild(node, goban, index, biasToUse, coefInterestingMovesModified, board, mTI, savingMove,stones_density);
      if (node->childNode()[index]->isReallyLocked)
	      v=v/1000;
#ifdef ANTILOCK
       if (v>bestUrgency) {
                if (lastLock>0) node->childNode()[lastLock]->isLocked=0;
                node->childNode()[index]->isLocked=1;lastLock=index;}
#endif

    } else {
      v=greedyValueForMoveWithoutChild(node, goban, move, goban.xIndex(move), goban.yIndex(move), biasToUse, coefInterestingMovesModified, board, mTI, savingMove,stones_density);
#ifdef ANTILOCK
        if (v>bestUrgency)
        {
                if (lastLock>0) node->childNode()[lastLock]->isLocked=0;
                lastLock=-1;
        }
#endif

    }
    if (v>bestUrgency) { bestUrgency=v; urgentestLocation=move; nextChildIndex=index; bestIndex=i; }
  }
  assert(bestIndex>=0);
  //   if (nextChildIndex<0) {/**assert(0);**/
  //     node->nBest.indexes[bestIndex]=node->childNode().size();
  //   }
  int nextMoveInfo=GobanNode::getNextInfo(urgentestLocation, nextChildIndex);
  node->setNextInfoForMoveToChoose(nextMoveInfo);

  node->needToUpdateUrgencyOff();
  return urgentestLocation;
}





// TEMPORARY COMMENT: THIS FUNCTION IS USED
double MoGo::MoveSelectorGlobal::greedyValueForMoveWithChild(GobanNode * node, const Goban & goban, int index, double biasToUse, double coefInterestingMovesModified, FastBoard *board, int mTI, Location savingMove, float stones_density) {
  Location move=node->childMove()[index];
  GobanNode *child=node->childNode()[index]; if (!child) return -1.; //FIXME : I have a strange bug here, child could not be 0!! Let's see what happen if I do that.
#ifdef ANTILOCK
  if ((mTI>3)&&(child->isLocked)) {return 0.;}
#endif

  assert(child);
  double numberOfSimulation_i=child->numberOfSimulation();
  double invNumberOfSimulation_i;

  if (numberOfSimulation_i>0) {  // This node has been created but not yet explored (can happen in multithread)
    invNumberOfSimulation_i=1./numberOfSimulation_i;
    double value=1.-child->nodeValue()[1]*invNumberOfSimulation_i;
    value=value<0.?0:value;
    value=value>1.?1:value;

#ifndef PARALLELIZED
    assert(numberOfSimulation_i>0);
#endif
#ifdef DEBUG_MoveSelectorGlobal
    GoTools::print("node->childMove()[i] %i: %g/%g\n", move, node->nodeUrgency()[move]/node->nodeValue()[move+3], node->nodeValue()[move+3]);
#endif
    double nbAllAtFirstMovesForThisChild=node->nodeValue()[move+3]; if (nbAllAtFirstMovesForThisChild < 0.5) nbAllAtFirstMovesForThisChild=1.;
    double invNbAllAtFirstMovesForThisChild=1./nbAllAtFirstMovesForThisChild;

    double valueAMAF=(node->nodeUrgency()[move])*invNbAllAtFirstMovesForThisChild;
    double bonus=0.; if (coefInterestingMovesModified>0. && move!=PASSMOVE) bonus=coefInterestingMovesModified*valueForInterestingMoveFastBoard(node, goban, *board, move, mTI, savingMove)+minInterestingMoves;
    // GoTools::print("bonus %f\n", bonus);

    double coef=1.-(Wamaf*nbAllAtFirstMovesForThisChild/**(1-pow(stones_density,6.2f))*/)/(Wamaf*nbAllAtFirstMovesForThisChild+numberOfSimulation_i+numberOfSimulation_i*nbAllAtFirstMovesForThisChild*biasToUse+weight_stones_quantity*pow((float)stones_density*FBS_handler::board_area,(float)pow_stones_quantity));

    double expertise = node->get_expertise(move);
    //coef_expertise = 0.02f/(double)(node->depth()+1);
    
    expertise = coef_expertise*expertise/(log(numberOfSimulation_i+2));
    
    if (move==PASSMOVE) coef=1.;
    #ifdef CRITICALITY
    double criticality = node->getLocationCriticalityValue(move);
    criticality = (1.-coef) * coef_criticality * criticality;
    criticality -= coef_variance * child->VarianceTerritory;

    double total = value*coef+(1.-coef)*valueAMAF+bonus+expertise+criticality;

#ifdef FANHUIPATCH
    if (total<0.1) value=double(GoTools::mogoRand2(100))/500.; //SUPER PATCH FAN HUI
#endif
    return total;
    #else
    value=value*coef+(1.-coef)*valueAMAF+bonus+expertise;
#ifdef FANHUIPATCH
    if (value<0.1) value=double(GoTools::mogoRand2(100))/500.; //SUPER PATCH FAN HUI
#endif
    return value;
    #endif

    // GoTools::print("Son urgentestLocation %i, val %f\n", urgentestLocation, _urgency[urgentestLocation]);*/
  } else
    return 0.;
  //     _urgency[move]=0.;

}





// TEMPORARY COMMENT: THIS FUNCTION IS USED
double MoGo::MoveSelectorGlobal::greedyValueForMoveWithoutChild(GobanNode * node, const Goban & goban, Location location, int i, int j, double , double coefInterestingMovesModified, FastBoard *board, int mTI, Location savingMove, float stones_density) {
  if ((localFirst>0) && (node->numberOfSimulation()<localFirst)) {/**assert(0);**/ // we want to play local if not a lot of visits
    Bitmap *bitmap=&node->bitmap();assert(bitmap);
    if (!GobanBitmap::getBitValue(*bitmap,i,j))
      return -1.; // we don't consider this move
  } else if ((hardLocalFirst>0) && (node->numberOfSimulation()<hardLocalFirst) && (goban.lastMove()!=PASSMOVE)) {/**assert(0);**/
    Bitmap *bitmap=&node->bitmap();
    if (bitmap && (int)bitmap->size()==goban.height() && (!GobanBitmap::getBitValue(*bitmap,i,j)))
      return -1.; // we don't consider this move
    int distance=goban.distance(goban.lastMove(), location);
    double maxDistance=double(goban.height())*sqrt(node->numberOfSimulation())/sqrt(hardLocalFirst)+1.;
    if (distance>(int)maxDistance)
      return -1.; // we don't consider this move
  } else if (useBitmapNeighborhood) {/**assert(0);**/
    Bitmap *bitmap=&node->bitmap();assert(bitmap);
    if (!GobanBitmap::getBitValue(*bitmap,i,j))
      return -1.; // we don't consider this move
  }

  double value=0.; double tmp=node->nodeValue()[location+3];
  if (tmp < 0.5) // no information for this node by all moves at first
    value=1000.;
  else { // only interesting if no child node corresponds to this move. The moves with a child have already been seen
    double bonus=0.; if (coefInterestingMovesModified>0. && location!=PASSMOVE) bonus=coefInterestingMovesModified*valueForInterestingMoveFastBoard(node, goban, *board, location, mTI, savingMove)+minInterestingMoves;
    double tmp2=1./tmp;
    double v=(node->nodeUrgency()[location])*tmp2;
    value=v+bonus;


#ifdef FANHUIPATCH
    if (value<0.1) value=double(GoTools::mogoRand2(100))/500.; //SUPER PATCH FAN HUI
#endif
  }
  return value;
  //       _urgency[location]=value;

  // #ifdef KEEP_NBEST_PER_NODE
  //       node->nBest.insert(_urgency[location], location, -1);
  // #endif

  //     if ((_urgency[location]>bestUrgency) || ((_urgency[location]==bestUrgency) && (location<urgentestLocation))) {/**assert(0);**/ urgentestLocation=location; bestUrgency=_urgency[location];nextChildIndex=-1;}
}




double MoGo::MoveSelectorGlobal::valueForInterestingMoveFastBoard( GobanNode * node, const Goban & gobanAtRoot, const FastBoard &board, Location move, int numThread, Location &savingMove ) {/**assert(0);**/
  if (node->numberOfSimulation()>=keepBonusNbSimulations) {/**assert(0);**/
    if (node->getBonusPrecomputed().size()==0)
      node->getBonusPrecomputed().resize((gobanAtRoot.height()+2)*(gobanAtRoot.width()+2),-10);
    if (node->getBonusPrecomputed()[move]>-5) return node->getBonusPrecomputed()[move];
  }

  if (savingMove<0) savingMove=board.saveStringOnAtari(numThread);
  if (board.isSelfAtari(move, numThread))
    return saveValueForInterestingMoveValue(node, gobanAtRoot, move, -1.);
  if (move==savingMove)
    return saveValueForInterestingMoveValue(node, gobanAtRoot, move, 3.);
  if (board.isInterestingByPattern(move))
    return saveValueForInterestingMoveValue(node, gobanAtRoot, move, 1.);
  else
    return saveValueForInterestingMoveValue(node, gobanAtRoot, move, 0.);
}







//
// C++ Implementation: treeupdator_mg
//
// Description:
//
//
// Author: Yizao Wang <yizao.wang@polytechnique.edu> and Sylvain Gelly <gelly@lri.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "treeupdator_mg.h"
#include <math.h>
#include "gotools_mg.h"
#include <string>
#include "moveselector_mg.h"
#include "fastboard_mg.h"
#include "gogame_mg.h"
#include "gobannode_mg.h"

#define SUM_SCORE_INDEX (0)
#define SUM_SQUARED_SCORE_INDEX (1)


using namespace MoGo;

  double torsion=0.;

// TEMPORARY COMMENT: THIS FUNCTION IS USED
MoGo::NodeSequence::NodeSequence() {
  clear();
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::NodeSequence::clear( ) {
  blockVariable=false;
  nodeAddress.clear();
  index.clear();
  superKoKeys.clear();
  highestValueMoves.clear();
  highestValues.clear();
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::NodeSequence::push_back( GobanNode * node, int indexForChild, int keepGobanKeys) {
#ifdef PARALLELIZED
  //   while (blockVariable) pthread_yield(); blockVariable=true;
#endif
  assert(node);
  if (nodeAddress.size()==0) {
    nodeAddress.push_back(node);
  } else {
    int tmpIndex=-1;
    if (indexForChild < 0) {
      for (int i=0;i<(int)nodeAddress[nodeAddress.size()-1]->childNode().size();i++)
        if (nodeAddress[nodeAddress.size()-1]->childNode()[i]==node) tmpIndex=i;
    } else
      tmpIndex = indexForChild;
    if (tmpIndex<0) {/**assert(0);**/
      GoTools::print("node=%i %i\n", nodeAddress[nodeAddress.size()-1], node);
      for (int i=0;i<(int)nodeAddress[nodeAddress.size()-1]->childNode().size();i++)
        GoTools::print("%i ",nodeAddress[nodeAddress.size()-1]->childNode()[i]);
      GoTools::print("ENDnode=%i %i\n", nodeAddress[nodeAddress.size()-1], node);
      assert(0);
    }
    nodeAddress.push_back(node);
    index.push_back(tmpIndex);
  }
  if (keepGobanKeys) {/**assert(0);**/
    superKoKeys.push_back(node->gobanKey()); superKoKeys.back()[0]-=superKoKeys.back()[0]%(1024*1024)/(1024*512)*(1024*512); superKoKeys.back()[0]-=superKoKeys.back()[0]%(1024*512)/1024*1024;
  }
#ifdef PARALLELIZED
  //   blockVariable=false;
#endif
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
int MoGo::NodeSequence::size( ) const {
  assert((nodeAddress.size() == 0) || (nodeAddress.size()==index.size()+1));
  return nodeAddress.size();
}


Location MoGo::NodeSequence::getNextMove( const int i ) const {/**assert(0);**/
  assert(i<(int)nodeAddress.size()-1);
  return nodeAddress[i]->childMove()[index[i]];
}

Location MoGo::NodeSequence::getNextMoveIndex( const int i ) const {/**assert(0);**/
  assert(i<(int)index.size());
  return index[i];
}

void MoGo::NodeSequence::pop_back( ) {/**assert(0);**/
#ifdef PARALLELIZED
  //   while (blockVariable) pthread_yield(); blockVariable=true;
#endif
  assert(nodeAddress.size()>0);
  nodeAddress.pop_back();
  if (index.size() > 0)
    index.pop_back();
  superKoKeys.pop_back();
  highestValueMoves.pop_back();
  highestValues.pop_back();
#ifdef PARALLELIZED
  //   blockVariable=false;
#endif
}

 // TEMPORARY COMMENT: THIS FUNCTION IS USED
MoGo::NodeSequence::~ NodeSequence( ) {}
MoGo::NodeSequence MoGo::NodeSequence::subSequence( int size ) const {/**assert(0);**/
  assert(size > 0);
  assert(size <= (int)this->size());
  NodeSequence sequence;
  sequence.nodeAddress.resize(size);
  sequence.index.resize(size-1);
  sequence.superKoKeys.resize(size);
  for (int i=0;i<size;i++) {/**assert(0);**/
    sequence.nodeAddress[i]=nodeAddress[i];
    sequence.superKoKeys[i]=superKoKeys[i];
    if (i < size-1)
      sequence.index[i]=index[i];
  }
  return sequence;
}


void MoGo::NodeSequence::push_BestValue( Location move, double value ) {/**assert(0);**/
  highestValueMoves.push_back(move);  highestValues.push_back(value);
}












///////////////////
//  TreeUpdator  //
///////////////////

namespace MoGo {

// TEMPORARY COMMENT: THIS FUNCTION IS USED
  TreeUpdator::TreeUpdator() {}


// TEMPORARY COMMENT: THIS FUNCTION IS USED
  TreeUpdator::~TreeUpdator() {}}

//void MoGo::TreeUpdator::nodeValueInitiation( GobanNode *  ) {/**assert(0);**/}  //TOBEREMOVED

void MoGo::TreeUpdator::changeMode( int  ) {/**assert(0);**/
  assert(0);// i set this only the for UCTUpdator
}





/////////////////////////////////
//  NumberOfSimulationUpdator  //
/////////////////////////////////


// TEMPORARY COMMENT: THIS FUNCTION IS USED
MoGo::NumberOfSimulationUpdator::NumberOfSimulationUpdator(int mode) {
  this->mode=mode;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
MoGo::NumberOfSimulationUpdator::~ NumberOfSimulationUpdator( ) {}






//////////////////
//  UCTUpdator  //
//////////////////

// TEMPORARY COMMENT: THIS FUNCTION IS USED
MoGo::UCTUpdator::UCTUpdator( double discountBeta, const int mode, const int meanIndex, const int varianceIndex, int keepNodesOrdered )
    : numberOfSimulationUpdator(keepNodesOrdered) {
  this->meanIndex=meanIndex;
  this->varianceIndex=varianceIndex;
  assert(mode==0 || mode==1 || mode==2 || mode==-1);
  this->mode=mode;
  this->discountBeta=discountBeta;
}


// TEMPORARY COMMENT: THIS FUNCTION IS USED
MoGo::UCTUpdator::~ UCTUpdator( ) {}

//TOBEREMOVED
// void MoGo::UCTUpdator::nodeValueInitiation( GobanNode * node ) {/**assert(0);**/
//   node->nodeValue().resize(3);
// }





void MoGo::UCTUpdator::changeMode( int mode ) {/**assert(0);**/
  this->mode = mode;
}



























// TEMPORARY COMMENT: THIS FUNCTION IS USED
MoGo::UCTUpdatorGlobal::UCTUpdatorGlobal( int useNewTrainOneGame, double powUpdate, double alphaUpdate, int avoidAMAFPreventInTree, int keepSequences, double discountBeta, int useEyes, double lambdaForAMAF2, double lambdaForAMAF, int keepNodesOrdered, double coefTwice, double betaForTerritoryScore, double coefForTerritoryScore, int equivalentSimulationsForInitialization, int updateFormula, int useEndTerritories, bool initializeByGrandFather, bool globalPlayerUseMemoryEconomy, bool keepDataForCorrelations, const int treeDepthAllAtFirstLimit, const int simulationDepthAllAtFirstLimit, const int mode, const int meanIndex, const int varianceIndex ) : UCTUpdator(discountBeta, mode, meanIndex, varianceIndex, keepNodesOrdered) {
  this->updateFormula=updateFormula;
  this->useEndTerritories=useEndTerritories;
  this->initializeByGrandFather=initializeByGrandFather;
  this->treeDepthAllAtFirstLimit=treeDepthAllAtFirstLimit;
  this->simulationDepthAllAtFirstLimit=simulationDepthAllAtFirstLimit;
  this->keepDataForCorrelations=keepDataForCorrelations;
  this->globalPlayerUseMemoryEconomy=globalPlayerUseMemoryEconomy;
  this->equivalentSimulationsForInitialization=equivalentSimulationsForInitialization;
  this->coefForTerritoryScore=coefForTerritoryScore;
  this->betaForTerritoryScore=betaForTerritoryScore;
  this->stopUsingInitializationDirtyHack=0.;
  this->coefTwice=coefTwice;
  this->lambdaForAMAF=lambdaForAMAF;
  this->lambdaForAMAF2=lambdaForAMAF2;
  this->useEyes=useEyes;
  this->discountBeta=discountBeta;
  this->keepSequences=keepSequences;
  this->avoidAMAFPreventInTree=avoidAMAFPreventInTree;
  this->powUpdate=powUpdate;
  this->alphaUpdate=alphaUpdate;
  this->useNewTrainOneGame=useNewTrainOneGame;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
MoGo::UCTUpdatorGlobal::~ UCTUpdatorGlobal( ) {}



//TOBEREMOVED
// void MoGo::UCTUpdatorGlobal::nodeValueInitiationFurther(GobanNode * node ) {/**assert(0);**/
//   //    if (node->nodeValue().size()>3)
//   //      return;
//   node->nodeValue().resize(3+(GobanNode::height+2)*(GobanNode::width+2));
//   if (node->nodeUrgency().size()!=(GobanNode::height+2)*(GobanNode::width+2)) {/**assert(0);**/
//     GoTools::print("%i %i\n", node->nodeUrgency().size(),(GobanNode::height+2)*(GobanNode::width+2));
//     assert(node->nodeUrgency().size()==(GobanNode::height+2)*(GobanNode::width+2));
//   }
//   int c=0;GobanNode *root=node;
//   if (initializeByGrandFather) {/**assert(0);**/
//     for ( ; (c<2) && (root->fatherNode().size()>0); c++) root=root->fatherNode()[0];
//   }
//   if (initializeByGrandFather) {/**assert(0);**/
//     for (int i=0;i<GobanNode::height;i++)
//       for (int j=0;j<GobanNode::width;j++) {/**assert(0);**/
//         Location location = 3+node->indexOnArray(i,j);
//         assert(location>=3);
//         node->nodeValue()[location]=0.;
//         if (equivalentSimulationsForInitialization>0 && (c==2))
//           node->nodeValue()[location]=equivalentSimulationsForInitialization*(1.-stopUsingInitializationDirtyHack);
//         if (!useMemoryEco())
//           assert(node->nodeUrgency()[location-3]<0.1);
//       }
//   } else {/**assert(0);**/
//     for (int i=0;i<GobanNode::height;i++)
//       for (int j=0;j<GobanNode::width;j++) {/**assert(0);**/
//         Location location = 3+node->indexOnArray(i,j);
//         assert(location>=3);
//         node->nodeValue()[location]=0.;
//         if (equivalentSimulationsForInitialization>0)
//           node->nodeValue()[location]=equivalentSimulationsForInitialization*(1.-stopUsingInitializationDirtyHack);
//         if (!useMemoryEco())
//           assert(node->nodeUrgency()[location-3]<0.1);
//       }
//   }
// }



//TOBEREMOVED
// void MoGo::UCTUpdatorGlobal::nodeValueInitiation( GobanNode * node ) {/**assert(0);**/
//   assert(0);
//  //  node->nodeValue().resize(3);
// //   for (int i=0;i<3;i++)
// //     node->nodeValue()[i]=0.;
// //   nodeValueInitiationFurther(node);
// }



GobanNode * MoGo::UCTUpdatorGlobal::getReferenceNode( GobanNode * node, int numStat, int nbSimulationsMin ) const {/**assert(0);**/
  GobanNode *referenceNode=0;
  for ( ; referenceNode==0 && node->fatherNode().size()>0; node=node->fatherNode()[0]) {/**assert(0);**/
    if (referenceNode==0 && node->getStats().getNbStats()>numStat && node->getStats().getNbIterations(numStat)>nbSimulationsMin)
      referenceNode=node;
  }
  return referenceNode;
}




































































































































































// ./mogo --test playMultipleTournaments --player0 InnerMCGoPlayer --player1 gnugoDefault --nbRuns 100 --displayGames 0 --dontDisplay 1 --default9 --gobanSize 9 --dontDisplay 1 --thresholdSelfAtari 3 --minNumberOfSimulationsOfResign 1000 --dontDisplay 0 --displayGames 1 --playsAgainstHuman 1 --useLibEgoTest 0 --useFastBoard 3 --playNearMode 0 --numUpdateFormulaeMoveSelector 16 --delta 0.00001 --useRLInitializor 10 --useNewTrainOneGame 0 --srandOnTime 1 --optimizeSelectMove -1 --nbTotalSimulations 10000 --updateFormula 8


































double MoGo::UCTUpdatorGlobal::logistic( double x ) const {/**assert(0);**/
  double c=lambdaForAMAF; //FIXME
  return 1./(1.+exp(-c*x));
}

double MoGo::UCTUpdatorGlobal::logisticDeriv( double x ) const {/**assert(0);**/
  double c=lambdaForAMAF; //FIXME
  return c*exp(-c*x)/((1.+exp(-c*x))*(1.+exp(-c*x)));
}





















extern int handicapLevel;

double averageScore=0.;

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::UCTUpdatorGlobal::updateAllInOne( MoveSelectorGlobal * moveSelector, GoGameTree & /*tree*/, const NodeSequence & currentSequence, const Goban & goban, double _score, const double komi, const FastSmallVector< int > & allMoves, Vector< int > & allAtFirstData, Vector< int > & /*allAtFirstDataInTree*/, const Vector< Location > & currentSequenceMove,double weightSimu ) {

  
  double score;
 
#ifndef NODK
  if (ThreadsManagement::getNumThread()==1)
  {
  double maxFuturSize=goban.height()*goban.width();
  //double pastSize=2.*(goban.moveNumber()+currentSequenceMove.size())/maxFuturSize;    VERSION UNDER TEST
  double pastSize=3.*(goban.moveNumber()+currentSequenceMove.size())/maxFuturSize;
//  if (futurSize>maxFuturSize) futurSize=maxFuturSize;
  if ((handicapLevel>0)&&(pastSize<1))
  {

	torsion=(handicapLevel*7)*(1-pastSize);
	  //if (drand48()<0.01) fprintf(stderr,"handiLevel=%d, maxFuturSize=%g, allAtFirstData.size=%d, torsion=%g,averageScore=%g\n",handicapLevel,maxFuturSize,currentSequence.size(),torsion,averageScore);
  }
  else torsion=0.;
  }
#endif
  	score=_score>komi/*+torsion/3*/;
	
	//averageScore=(_score-komi)*0.01+0.99*averageScore;

  #ifdef CRITICALITY
  for (int i=0;i<currentSequence.size();i++)
  {
    GobanNode* node= currentSequence[i];
    int nbOfTerritoryBelongingToBlack = 0;
    
    //updating criticality
    int location = node->indexOnArray(0,0);
    for (int i=0;i<GobanNode::height;i++, location+=2)
        for (int j=0;j<GobanNode::width;j++, location++) {
            int proprioOfLocation = FastBoard::getBoardForThread()->owner(location);
            node->setWinsForLocationOwner(score, proprioOfLocation, location);
            if (proprioOfLocation == 1)
                nbOfTerritoryBelongingToBlack++;
        }
        
    //updating territory average
    double territoryWonInThisGame = nbOfTerritoryBelongingToBlack / (GobanNode::height * GobanNode::width);
    node->AverageTerritory = 0.95*node->AverageTerritory + 0.05 * territoryWonInThisGame;
    double diffTerritory = territoryWonInThisGame - node->AverageTerritory;
    if (diffTerritory < 0)
        diffTerritory = diffTerritory * -1.;
    node->VarianceTerritory = 0.95 * node->VarianceTerritory + 0.05 * sqrt(diffTerritory);
  }
  #endif



  assert(useMemoryEco());
  assert(updateFormula==0 || updateFormula==1 ); assert(getAvoidAMAFPreventInTree()==1);
  bool lock=false;

//   numberOfSimulationUpdator.update(tree,currentSequence,goban, score);
//   meanAndVarianceUpdate(currentSequence,score,1);


// here score is already 0 or 1
double scoreMax=1.;
int size=currentSequence.size()-1;

  
  // #if 0
  for (int i=0;i<=size;i++) {        // i = node to be updated
    GobanNode *node=currentSequence[i]; bool isBlacksTurn=node->isBlacksTurn();
    UrgencyType value; if (isBlacksTurn) value=score; else value=(scoreMax-score);

    if (lock) moveSelector->lockThisNode(node);

    node->resetNumberOfSimulation(node->numberOfSimulation()+weightSimu);
    node->nodeValue()[meanIndex]+=value*weightSimu;
    //     if (value==1. && node->numberOfSimulation()>0/*&& node->numberOfSimulation()>(int)coefForTerritoryScore*/) {/**assert(0);**/
    //       node->needToUpdateUrgencyOff();
    //     } else //FIXMEFIXME
    node->needToUpdateUrgencyOn();
    if (i==size /*&& !node->getInitiated()*/) {
      if (lock) moveSelector->unlockThisNode(node);
      continue;
    }

    node->nodeUrgency()[0]=0.5; node->nodeValue()[0+3]=1.; // For PASSMOVE
    allAtFirstData[0]+=3; int numIterationAllAtFirst=allAtFirstData[0]; // This gives the number of the iteration. 
                                                                        //ca c est du commentaire pourrave inexploitable
#ifdef KEEP_CAPTURE_MOVES_PER_NODE
    for (int j=0;j<(int)node->getCaptureMoves().size();j++)
      allAtFirstData[node->getCaptureMoves()[j]]=numIterationAllAtFirst-1;
#endif


    for (int l=i+1;l<(int)currentSequenceMove.size();l+=2) {
      Location location=currentSequenceMove[l];
#ifdef KEEP_CAPTURE_MOVES_PER_NODE
      int captured=location&65536; location=location&0xFFFF;                //manip crade sur les bits de poids fort pour savoir si c est une capture
#endif
      //       if (location==PASSMOVE) continue;
      //       if (allAtFirstData[location]>numIterationAllAtFirst) continue; // already played in this sequence
      if (location != PASSMOVE && allAtFirstData[location]<numIterationAllAtFirst
#ifdef KEEP_CAPTURE_MOVES_PER_NODE
          && (allAtFirstData[location]!=numIterationAllAtFirst-1 || (captured)) // was not a capture move, or something has been captured in AMAF
#endif
         ) {
        allAtFirstData[location]=numIterationAllAtFirst+1;

        //       assert(!(isBlacksTurn ^ currentSequence[l-1]->isBlacksTurn()));
        //       if (isBlacksTurn ^ currentSequence[l-1]->isBlacksTurn()) continue; // if it is not the same color
        //       if (!(node->nodeUrgency()[location]>0.)) continue; // not a legal move in this position...
        if (node->nodeUrgency()[location]<=0.) continue; // not a legal move in this position...
        node->nodeUrgency()[location]+=value*weightSimu;
        node->nodeValue()[location+3]+=1.*weightSimu;
      }
    }
    if (allMoves.size()==0) {
    //if ((allMoves.size()==0)||(node->numberOfSimulation()>1000)) {
      if (lock) moveSelector->unlockThisNode(node);
      continue;
    }
    int beginning=isBlacksTurn^currentSequence[size]->isBlacksTurn()?1:0;
    for (int j=beginning;j<(int)allMoves.size();j++) {
      if (updateFormula==0 && j>0 && allMoves[j]*allMoves[j-1]>0) break;
      Location location=allMoves[j]; if (!isBlacksTurn) location=-location;
#ifndef KEEP_CAPTURE_MOVES_PER_NODE
      if (location>0 && node->nodeUrgency()[location]>0. && allAtFirstData[location]<=numIterationAllAtFirst) // was not a capture move, or something has been captured in AMAF
      {
        node->nodeUrgency()[location]+=value*weightSimu;
        node->nodeValue()[location+3]+=1.*weightSimu;
      }
#else
      if (location>0) {/**assert(0);**/
        int captured=location&65536; location=location&0xFFFF;
        if ((allAtFirstData[location]!=numIterationAllAtFirst-1 || (captured)) && node->nodeUrgency()[location]>0. && allAtFirstData[location]<=numIterationAllAtFirst) // was not a capture move, or something has been captured in AMAF
        {/**assert(0);**/
          node->nodeUrgency()[location]+=value*weightSimu;
          node->nodeValue()[location+3]+=1.*weightSimu;
        }
      }
#endif

    }

    if (lock) moveSelector->unlockThisNode(node);
  }
  // #endif





}











































#if 0
#ifdef KEEP_STACK_MOVE_VECTOR_PER_NODE
if (node->stringLocationsSet) {/**assert(0);**/
  Vector<double> indicators; indicators.push_back(1.);
  for (int ind=0;ind<(int)node->allStringLocations.size();ind++) {/**assert(0);**/
    Location location=node->allStringLocations[ind];
    if (location<0) {/**assert(0);**/ location=-location; }
    int captured=board->hasBeenCaptured(location);
    if (captured==0) indicators.push_back(0.);
    else if (captured==1) {/**assert(0);**/
      if (node->isBlacksTurn()) indicators.push_back(-1.); else indicators.push_back(1.);
    } else if (captured==2) {/**assert(0);**/
      if (node->isBlacksTurn()) indicators.push_back(1.); else indicators.push_back(-1.);
    }
  }
  assert(indicators.size()==node->allStringLocations.size()+1);
  double x=0.; double xDefault=0.;
  for (int k=0;k<(int)indicators.size();k++) {/**assert(0);**/
    x+=node->getStats().getValue(4, k+2)*indicators[k];

    if (k>=1) {/**assert(0);**/
      Location location=node->allStringLocations[k-1];
      if (location<0) {/**assert(0);**/ location=-location; }
      double pwKNcap=root->getStats().getAverage(0,1,location);
      double pwKNnotcap=root->getStats().getAverage(2,3,location);
      if (board->hasBeenCaptured(location))
        xDefault+=pwKNcap;
      else
        xDefault+=pwKNnotcap;
    }
  }
  xDefault/=double(indicators.size()-1);
  double value=logistic(x); double scoreTmp=score; if (!node->isBlacksTurn()) scoreTmp=scoreMax-scoreTmp;
  if (node->fatherNode().size()==0) {/**assert(0);**/
    static double cumulativeError=0., cumulativeErrorDefault=0.; static GobanNode *oldRoot=node; static int nbErrors=0;
    if (oldRoot!=node) {/**assert(0);**/cumulativeError=0.;oldRoot=node;nbErrors=0;cumulativeErrorDefault=0.;}
    nbErrors++; cumulativeError+=(value-scoreTmp)*(value-scoreTmp); cumulativeErrorDefault+=(xDefault-scoreTmp)*(xDefault-scoreTmp);
    if (nbErrors%100==99) {/**assert(0);**/
      //           Vector<double> theta; for (int k=0;k<(int)indicators.size();k++) {/**assert(0);**/
      //             goban.textModeShowPosition(node->allStringLocations[k]>0?node->allStringLocations[k]:-node->allStringLocations[k]); theta.push_back(node->getStats().getValue(4, k+2));
      //           }
      //           GoTools::print("\n");
      //           GoTools::print(theta);
      GoTools::print("nbErrors %i, error=%f (size %i) (%f)\n", nbErrors, cumulativeError/double(nbErrors), indicators.size()-1, cumulativeErrorDefault/double(nbErrors));
      cumulativeError=0.;nbErrors=0;cumulativeErrorDefault=0.;
    }
  }

  for (int k=0;k<(int)indicators.size();k++) {/**assert(0);**/
    node->getStats().setValue(4, k+2, node->getStats().getValue(4, k+2)-alphaUpdate*indicators[k]*logisticDeriv(x)*(logistic(x)-scoreTmp));
  }

}
#endif
#endif


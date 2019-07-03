//
// C++ Implementation: gobannode_mg
//
// Description:
//
//
// Author: Yizao Wang <yizao.wang@polytechnique.edu> and Sylvain Gelly <gelly@lri.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "gobannode_mg.h"
#include "gotools_mg.h"

#ifdef CRITICALITY
#include "fastboard_mg.cpp"
#endif

using namespace MoGo;




int GobanNode::height;
int GobanNode::width;

QuickMemoryStack<GobanNode *> MoGo::QuickGobanNodeMemoryManagement::memory;





// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::NodeValue::clear( ) {
  valueArray.clear();
}



MoGo::GobanNode::~GobanNode( ) {/**assert(0);**/
#ifdef SUMTRICK
if (lastShared) {delete [] lastShared;lastShared=NULL;isShared=0;}
#endif
}



#if 0
MoGo::GobanNode::GobanNode( const GobanKey & gobanKey, GobanNode * fatherNode, const int nodeValueSize ) : nodeValue_(nodeValueSize),nodeUrgency_(-2,-2)/*,expertise_(-2,-2)*//*, nodeUrgency_(gobanKey[0]%32,gobanKey[0]%1024/32)*/, numberOfSimulation_(0), stats(((gobanKey[0]%32)+2)*((gobanKey[0]%1024/32)+2)) {/**assert(0);**/
  if (fatherNode!=NULL) {/**assert(0);**/
    fatherNode->childNodeList.push_back(this);
    fatherNodeList.push_back(fatherNode);
  }
  isNodeUrgencyUpToDate=false;
  nodeGobanKey=gobanKey;
  childNodeList.clear();
  height=gobanKey[0]%32;
  width=gobanKey[0]%1024/32;
  isBlackToPlay=1-gobanKey[0]%(1024*1024)/(1024*512);//2^20/2^19;
  isInitiated=false;
  isNodeBitmapCreated_ = false;
#ifdef USE_MUTEX_PER_NODE
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutex_init(&nodeMutex,&attr);
#endif

  resizeExpertise( );
}
#endif

// TEMPORARY COMMENT: THIS FUNCTION IS USED
MoGo::GobanNode::GobanNode( const GobanKey & gobanKey ):nodeUrgency_(-2,-2)/*,expertise_(-2,-2)*/, stats(((gobanKey[0]%32)+2)*((gobanKey[0]%1024/32)+2)) {
  this->nodeUrgency_.clear();
#ifdef GOLDENEYE
golden=0;
goldenbis=0;
#endif
  //this->expertise_.clear();
  nbMC=3.;for (int i=0;i<6;i++) {winBlack[i]=0;winWhite[i]=0;lossBlack[i]=0;lossWhite[i]=0;}
  winBlack[Goban::getThresholdSelfAtari()-1]+=3;
  winWhite[Goban::getThresholdSelfAtari()-1]+=3;
  
   // for(int i = 0; i<441; i++)
//     {
//       expertise[i] = 0;
//     }
   

  //expertise.resize(FBS_handler::board_big_area,0);

#ifdef ANTILOCK
  isLocked=0;
  isReallyLocked=0;
#endif

#ifdef SUMTRICK
  isShared=0;
  lastShared=NULL;
#endif
  clearVector(this->nodeGobanKey);
#ifdef USE_CHILD_ALLOCATED_VECTOR
  this->childNodeList.clear();
  this->childMoveList.clear();
#else
  clearVector(this->childNodeList);
  clearVector(this->childMoveList);
#endif
  this->nodeValue().clear();
  //       GoTools::print("this->nodeValue() size %i\n", this->nodeValue().getValueArray().size());
  this->setNodeBitmapCreated(false);
  clearVector(this->nodeBitmap_);
  this->bonusPrecomputed.clear();
  this->stats.clear();
  //   nextChildToKeep=0;
  nextInfoForMoveToChoose=0;

  /*  isNodeUrgencyUpToDate=false;
    nodeGobanKey=gobanKey;
    childNodeList.clear();
    height=gobanKey[0]%32;
    width=gobanKey[0]%1024/32;
    isBlackToPlay=1-gobanKey[0]%(1024*1024)/(1024*512);//2^20/2^19;
    isInitiated=false;
    isNodeBitmapCreated_ = false;
  #ifdef USE_MUTEX_PER_NODE
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutex_init(&nodeMutex,&attr);
  #endif
     deleteGobanNode(this);*/

  resizeExpertise();

  #ifdef CRITICALITY
  initializeCriticality();
  #endif

}


void MoGo::GobanNode::showGoban(const Location location ) const {/**assert(0);**/
  Goban goban(nodeGobanKey);
  assert(isBlackToPlay==goban.isBlacksTurn());
  goban.textModeShowGoban(location);
  goban.printGobanKey();
}


// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GobanNode::addChild(const Location location, GobanNode* childNode) {
  assert(childNode!=NULL);
#ifdef USE_CHILD_ALLOCATED_VECTOR
#ifdef USE_MUTEX_PER_NODE
  //  pthread_mutex_lock(&nodeMutex);
#endif
  // GoTools::print("addChild %i -> %i (%i)\n", this, childNode, location);
  childMoveList.push_backElementWithoutSize(location);
  childNodeList.push_backElementWithoutSize(childNode);
  childMoveList.incrementSize();
  childNodeList.incrementSize();

  nBestChangeIndex(location, childNodeList.size()-1);
  // #ifdef KEEP_NBEST_PER_NODE
  //   for (int i=0;i<(int)nBest.moves.size();i++) {/**assert(0);**/
  //     if (nBest.moves[i]==location)
  //       nBest.indexes[i]=childNodeList.size()-1;
  //   }
  // #endif

  // GoTools::print("addChild %i -> %i (%i): size %i\n", this, childNode, location, this->childNode().size());
#ifdef USE_MUTEX_PER_NODE
  //  pthread_mutex_unlock(&nodeMutex);
#endif

#else
  childMoveList.push_back(location);
  childNodeList.push_back(childNode);
#endif
  childNode->fatherNodeList.push_back(this);
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::GobanNode::isChild(const Location location) const {
  for (int i=0;i<(int)childMoveList.size();i++)
    if (location==childMoveList[i]) return true;
  return false;
}
MoGo::GobanNode * MoGo::GobanNode::getChild( const Location location ) const {/**assert(0);**/
  for (int i=0;i<(int)childMoveList.size();i++)
    if (location==childMoveList[i]) return childNodeList[i];
  return 0;
}
Location MoGo::GobanNode::getChild( const GobanNode * node ) const {/**assert(0);**/
  for (int i=0;i<(int)childNodeList.size();i++)
    if (node==childNodeList[i]) return childMoveList[i];
  return -1;
}

bool MoGo::GobanNode::isChild(const GobanNode * node) const {/**assert(0);**/
  for (int i=0;i<(int)childNodeList.size();i++)
    if (node==childNodeList[i]) return true;
  return false;
}

void MoGo::GobanNode::showChildren() const {/**assert(0);**/
  Cout << childMoveList.size() << " child(ren) in total: ";
  if (childMoveList.size()==0) return;
  int counter=-1;
  for (int i=0;i<(int)childMoveList.size();i++) {/**assert(0);**/
    counter++;
    if (counter%4==0) Cout << "\n";
    textModeShowPosition(childMoveList[i]);
    nodeValue_.print();
  }

  Cout << "\n";
}

void MoGo::GobanNode::showFathers() const {/**assert(0);**/
  GoTools::print("%d father nodes:",fatherNodeList.size());
  if (fatherNodeList.size()==0) return;
  for (int i=0;i<(int)fatherNodeList.size();i++)
    fatherNodeList[i]->showGoban();
}

void MoGo::GobanNode::showNodeInformation( ) const {/**assert(0);**/
  Cout << "\n";
  Cout << "--------------------------Show Node Information--------------------------\n";
  Cout << "Goban at this node: ";
  Goban tmpGoban(nodeGobanKey);
  tmpGoban.textModeShowGoban();
  showChildren();
  nodeUrgency_.textModeShowUrgency();
  Cout << "---------------------------End of Information----------------------------\n";

}




int MoGo::GobanNode::xIndex( const Location location ) const {/**assert(0);**/
  return location/(width+2)-1;
}

int MoGo::GobanNode::yIndex( const Location location ) const {/**assert(0);**/
  return location%(width+2)-1;
}

void MoGo::GobanNode::textModeShowPosition( const Location location ) const {/**assert(0);**/
  if (location==PASSMOVE) {/**assert(0);**/
    Cout << "(Pass)";
    return;
  }
  int x=0,y=0;
  x=xIndex(location);
  y=yIndex(location);

  if (y>=8)
    GoTools::print("(%c%i)",(char)('A'+y+1),height-x);
  else
    GoTools::print("(%c%i)",(char)('A'+y),height-x);
  return;
  return;

}



void MoGo::GobanNode::printGobanKey( ) const {/**assert(0);**/
  Goban goban(nodeGobanKey);
  goban.printGobanKey();
}
/////////////////////
// Class NodeValue //
/////////////////////


// TEMPORARY COMMENT: THIS FUNCTION IS USED
MoGo::NodeValue::NodeValue( int size ) : valueArray(size) {
  for (int i=0;i<size;i++)
    valueArray[i]=0;
}

MoGo::NodeValue::~ NodeValue( ) {/**assert(0);**/}

void MoGo::NodeValue::print( ) const {/**assert(0);**/
  for (int i=0;i<(int)valueArray.size();i++)
    GoTools::print("%6.2f ",valueArray[i]);
}

 // TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GobanNode::setInitiated( bool isInitiated ) {
  this->isInitiated = isInitiated;
}


// TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::GobanNode::getInitiated( ) const {
  return isInitiated;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GobanNode::removeFather( GobanNode * node ) {
  assert(fatherNodeList.size() > 0);
  int i=0;
  for (;i<(int)fatherNodeList.size();i++)
    if (fatherNodeList[i]==node)
      break;
  if (i<(int)fatherNodeList.size()) {
    fatherNodeList[i]=fatherNodeList[fatherNodeList.size()-1];
    fatherNodeList.pop_back();
  } else
    assert(0);
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GobanNode::removeChild( GobanNode * node ) {
  assert(childNodeList.size() > 0);
  int i=0;
  for (;i<(int)childNodeList.size();i++)
    if (childNodeList[i]==node)
      break;
  if (i<(int)childNodeList.size()) {
    childNodeList[i]=childNodeList[childNodeList.size()-1];
    childMoveList[i]=childMoveList[childMoveList.size()-1];
    childNodeList.pop_back();
    childMoveList.pop_back();
  } else
    assert(0);
  //   nextChildToKeep=0;
  nextInfoForMoveToChoose=0;

  countDownForConsideringAllMoves=0;
  nBestClear();

}

GobanKey MoGo::GobanNode::superKoGobanKey( ) const {/**assert(0);**/
  GobanKey tmpGobanKey = nodeGobanKey;
  tmpGobanKey[0]-=tmpGobanKey[0]%(1024*1024)/(1024*512)*(1024*512);
  tmpGobanKey[0]-=tmpGobanKey[0]%(1024*512)/1024*1024;
  return tmpGobanKey;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
GobanNode * MoGo::GobanNode::newGobanNode( const GobanKey & gobanKey ) {
  return QuickGobanNodeMemoryManagement::quickMalloc(gobanKey);
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GobanNode::deleteGobanNode( GobanNode * node ) {
  //   node->nodeUrgency_=GobanUrgency(-2,-2);
  node->nodeUrgency_.clear();
  //node->expertise_.clear();
  node->nbMC=3.;for (int i=0;i<6;i++) {node->winBlack[i]=0;node->winWhite[i]=0;node->lossBlack[i]=0;node->lossWhite[i]=0;}
  node->winBlack[Goban::getThresholdSelfAtari()-1]+=3;
  node->winWhite[Goban::getThresholdSelfAtari()-1]+=3;
  
  for(int i = 0; i<(node->expertise_).size(); i++)
    {
      (node->expertise_)[i] = 0;
    }
  
#ifdef SUMTRICK
  if (node->isShared)
	{
		assert(node->lastShared);
		delete [] node->lastShared;
		node->lastShared=NULL;
	}
  node->isShared=0;
#endif
  clearVector(node->nodeGobanKey);
#ifdef USE_CHILD_ALLOCATED_VECTOR
  node->childNodeList.clear();
  node->childMoveList.clear();
#else
  clearVector(node->childNodeList);
  clearVector(node->childMoveList);
#endif
#ifdef KEEP_STACK_MOVE_VECTOR_PER_NODE
  clearVector(node->frontLocations);
#endif
#ifdef KEEP_STRINGS_ON_NODES
  clearVector(node->allStringLocations);
#endif
  node->countDownForConsideringAllMoves=0;
  node->nBestClear();

  node->nodeValue().clear();
  //       GoTools::print("node->nodeValue() size %i\n", node->nodeValue().getValueArray().size());
  node->setNodeBitmapCreated(false);
  clearVector(node->nodeBitmap_);
  node->bonusPrecomputed.clear();
  node->stats.clear();
  //   node->nextChildToKeep=0;
  node->nextInfoForMoveToChoose=0;
  QuickGobanNodeMemoryManagement::free(node);
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GobanNode::reinitiate( const GobanKey & gobanKey ) {
  //    GoTools::print("reinitiate node %i (%i)\n", (int)this, nodeValue_.size());
  assert(nodeValue_.size() == 0);
  nodeValue_.resize(3);
  //   nodeValue_.resize(0);
  //   nodeUrgency_=GobanUrgency(gobanKey[0]%32,gobanKey[0]%1024/32);
  nodeUrgency_.clear();
  numberOfSimulation_=0;

  isNodeUrgencyUpToDate=false;
  fatherNodeList.clear();
  childNodeList.clear();
  childMoveList.clear();
  setNodeBitmapCreated(false);
  nodeBitmap_.clear();
  //   nextChildToKeep=0;
  nextInfoForMoveToChoose=0;
  
 
  nodeGobanKey=gobanKey;
  height=gobanKey[0]%32;
  width=gobanKey[0]%1024/32;
  isBlackToPlay=1-gobanKey[0]%(1024*1024)/(1024*512);//2^20/2^19;
  stats=NodeStats((height+2)*(width+2));
  isInitiated=false;
#ifdef USE_MUTEX_PER_NODE
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
  pthread_mutex_init(&nodeMutex,&attr);
#endif
#ifdef KEEP_STACK_MOVE_VECTOR_PER_NODE
  frontLocations.clear();
  emptyLocationsSize=0;
  beginningUnseenFrontLocations=0;
#endif
#ifdef KEEP_STRINGS_ON_NODES
  allStringLocations.clear();
  stringLocationsSet=false;
#endif
  countDownForConsideringAllMoves=0;
  nBestClear();
#ifndef KEEP_NBEST_PER_NODE
  nBestCurrentSize_=-1;
#endif
#ifdef KEEP_CAPTURE_MOVES_PER_NODE
  captureMoves.clear();
#endif


  resizeExpertise();
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GobanNode::resizeUrgency( ) {
  // GoTools::print("%i %i\n", height, width);
  /*static int c=0;
  if (c>0)
  assert(0);
  c++;*/
#ifdef USE_CHILD_ALLOCATED_VECTOR
  childNodeList.resize(height*width+1); childNodeList.clearInternal();
  childMoveList.resize(height*width+1); childMoveList.clearInternal();
#endif
  nodeUrgency_=GobanUrgency(height,width);

}



void MoGo::GobanNode::resizeExpertise( ) 
{
  if((width<=9)&&(height<=9))
    {
      expertise_.clear();
      assert((int)expertise_.size()==0);
    }
  else expertise_.resize((width+2)*(height+2)); 

  for(int i = 0; i<(int)expertise_.size(); i++)
    expertise_[i] = 0;
}

#ifdef CRITICALITY
void MoGo::GobanNode::initializeCriticality()
{
    int completeSize = (FBS_handler::board_size + 2) * (FBS_handler::board_size + 2);

    whiteWinsLocationIsWhite_ = new int[completeSize];
    for (int i = 0; i <completeSize; i++)
      whiteWinsLocationIsWhite_[i] = 0;
      
    whiteWinsLocationIsBlack_ = new int[completeSize];
    for (int i = 0; i <completeSize; i++)
      whiteWinsLocationIsBlack_[i] = 0;
    
    blackWinsLocationIsWhite_ = new int[completeSize];
    for (int i = 0; i <completeSize; i++)
      blackWinsLocationIsWhite_[i] = 0;
    
    blackWinsLocationIsBlack_ = new int[completeSize];
    for (int i = 0; i <completeSize; i++)
      blackWinsLocationIsBlack_[i] = 0;
    
    AverageTerritory = 0.5;
    VarianceTerritory = 0;
}

void MoGo::GobanNode::setWinsForLocationOwner(bool isBlackWin, int locIsOwnedByBlack, int loc)
{
    if (isBlackWin)
    {
        if (locIsOwnedByBlack == 1)
            blackWinsLocationIsBlack_[loc]++;
        else if (locIsOwnedByBlack == -1)
            blackWinsLocationIsWhite_[loc]++;
    }
    else
    {
        if (locIsOwnedByBlack == 1)
            whiteWinsLocationIsBlack_[loc]++;
        else if (locIsOwnedByBlack == -1)
            whiteWinsLocationIsWhite_[loc]++;
    }
}

double MoGo::GobanNode::getLocationCriticalityValue(int location)
{
    double resul;
    double countBlackWins = blackWinsLocationIsBlack_[location] + blackWinsLocationIsWhite_[location];
    double countWhiteWins = whiteWinsLocationIsBlack_[location] + whiteWinsLocationIsWhite_[location];
    double countLocationIsBlack = blackWinsLocationIsBlack_[location] + whiteWinsLocationIsWhite_[location];
    double countLocationIsWhite = blackWinsLocationIsWhite_[location] + whiteWinsLocationIsWhite_[location];
    double countLocationOwnerWonPlayout = blackWinsLocationIsBlack_[location] + whiteWinsLocationIsWhite_[location];
    double countLocationOwnerLostPlayout = blackWinsLocationIsWhite_[location] + whiteWinsLocationIsBlack_[location];

    resul = ((2*countLocationOwnerWonPlayout) / (numberOfSimulation_+1.0)) - ((countLocationIsWhite+countLocationIsBlack)/(numberOfSimulation_+1.0)) * ((countWhiteWins+countBlackWins)/(numberOfSimulation_+1.0));


    return resul;
}
#endif















































pthread_mutex_t MoGo::QuickGobanNodeMemoryManagement::saferMutex;

// TEMPORARY COMMENT: THIS FUNCTION IS USED
GobanNode * MoGo::QuickGobanNodeMemoryManagement::quickMalloc( const GobanKey & key ) {
#ifdef PARALLELIZED
  //       pthread_mutex_lock(&saferMutex);
#endif
  //     if (!toInitQuickMemoryStack) init();
  if (memory.empty()) {
#ifdef PARALLELIZED
    //       pthread_mutex_unlock(&saferMutex);
#endif
    GobanNode *tmp=new GobanNode(key);
    tmp->reinitiate(key);
#ifdef GOLDENEYE
    tmp->golden=0;tmp->goldenbis=0;
#endif
    return tmp;
  } else {
    GobanNode *tmp=memory.top();
    memory.pop();
#ifdef GOLDENEYE_MC
    tmp->semeaiW.resize(0);
    tmp->semeaiB.resize(0);
#endif
    assert(tmp);
    tmp->reinitiate(key);
#ifdef ANTILOCK
    tmp->isLocked=0;
    tmp->isReallyLocked=0;
#endif
#ifdef PARALLELIZED
    //       pthread_mutex_unlock(&saferMutex);
#endif
    return tmp;
  }

}

void MoGo::GobanNode::setNodeBitmapByNodeUrgency( ) {/**assert(0);**/
  nodeUrgency_.changeToBitmap(nodeBitmap_);
}

void MoGo::GobanNode::reorderChildsBySimulations( GobanNode * parent, GobanNode * child ) {/**assert(0);**/
  int i;
  for (i=0;i<(int)parent->childNodeList.size();i++)
    if (parent->childNodeList[i]==child)
      break;

  assert(i<(int)parent->childNodeList.size());

  int index=i;
  for (;i>0 && (parent->childNodeList[i-1]->numberOfSimulation()<child->numberOfSimulation());i--)
    ;

  Location tmp=parent->childMoveList[index];
  parent->childMoveList[index]=parent->childMoveList[i];
  parent->childNodeList[index]=parent->childNodeList[i];
  parent->childMoveList[i]=tmp;
  parent->childNodeList[i]=child;
}

bool MoGo::GobanNode::isShallowerThan( int maxDepth ) const {/**assert(0);**/
  const GobanNode *node=this;
  maxDepth=maxDepth<0?0:maxDepth;
  int depth=0;
  while (node->fatherNode().size()>0 && depth<=maxDepth) {/**assert(0);**/
    node=node->fatherNode()[0];
    depth++;
  }
  return depth<=maxDepth;
}



int MoGo::GobanNode::depth( ) const {/**assert(0);**/
  const GobanNode *node=this;
  int depth=0;
  while ((node->fatherNode().size()>0)&&(depth<100)) {/**assert(0);**/
    node=node->fatherNode()[0];
    depth++;
  }
  return depth;
}

void MoGo::GobanNode::addNeighborhoodToBitmap( const Goban &goban, Location location ) {/**assert(0);**/
  assert(location!=PASSMOVE);
#ifdef KEEP_STACK_MOVE_VECTOR_PER_NODE
  const GobanState &gobanState=goban.getGobanState();
  for (int i=0;i<4;i++) {/**assert(0);**/
    Location tmpLocation=location+goban.direction(i);
    int state=gobanState[tmpLocation];
    if (state==OUTSIDE) continue;
    if (GobanBitmap::getBitValue(nodeBitmap_, tmpLocation)) continue;
    GobanBitmap::setBitValue(nodeBitmap_, tmpLocation, true);
    if (state==EMPTY) {/**assert(0);**/
      frontLocations.push_back(tmpLocation); emptyLocationsSize++;
    } else {/**assert(0);**/
      goban.addAllClosestLiberties(tmpLocation, frontLocations, nodeBitmap_);
    }
  }
#else
  goban.textModeShowGoban();
  location=PASSMOVE; // To avoid the warning
  assert(0);
#endif
}

bool MoGo::GobanNode::extendBitmapNeighborhood(const Goban &goban) {/**assert(0);**/
#ifdef KEEP_STACK_MOVE_VECTOR_PER_NODE
  const GobanState &gobanState=goban.getGobanState();
  //     if (fatherNode().size()==0) {/**assert(0);**/
  //       GoTools::print("frontLocations %i, beginningUnseenFrontLocations %i\n", frontLocations.size(), beginningUnseenFrontLocations);
  //     }
  int size=frontLocations.size(); int diff=size-beginningUnseenFrontLocations;
  for (int i=beginningUnseenFrontLocations;i<size;i++) {/**assert(0);**/
    Location location=frontLocations[i];
  
    for (int j=0;j<4;j++) {/**assert(0);**/
      Location tmpLocation=location+goban.direction(j);
      int state=gobanState[tmpLocation];
   
      if (state==OUTSIDE) continue;
      if (GobanBitmap::getBitValue(nodeBitmap_, tmpLocation)) {/**assert(0);**/
        continue;
     
      }
      GobanBitmap::setBitValue(nodeBitmap_, tmpLocation, true);
      if (state==EMPTY) {/**assert(0);**/
        frontLocations.push_back(tmpLocation);emptyLocationsSize++;
       
      } else {/**assert(0);**/
        goban.addAllClosestLiberties(tmpLocation, frontLocations, nodeBitmap_);
      }
    }
  }
  beginningUnseenFrontLocations+=diff;
  return diff>0;
#else

  goban.textModeShowGoban();
  assert(0);
  return false;
#endif
}

void MoGo::GobanNode::clearNeighborhood( ) {/**assert(0);**/
#ifdef KEEP_STACK_MOVE_VECTOR_PER_NODE
  frontLocations.clear();
  GobanBitmap::clear(nodeBitmap_);
  emptyLocationsSize=0;
  beginningUnseenFrontLocations=0;
#endif
}

int MoGo::GobanNode::getEmptyLocationsSize() const {/**assert(0);**/
#ifdef KEEP_STACK_MOVE_VECTOR_PER_NODE
  if (beginningUnseenFrontLocations<(int)frontLocations.size())
    return emptyLocationsSize;
  else
    return 1<<30; //FIXME
#else
  return 0;
#endif
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
Location MoGo::GobanNode::getNextLocation( int nextInfoForMoveToChoose ) {
  return nextInfoForMoveToChoose&(65535);
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
int MoGo::GobanNode::getNextChildIndex( int nextInfoForMoveToChoose ) {
  return ((nextInfoForMoveToChoose>>16)&(65535))-1;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
int MoGo::GobanNode::getNextInfo( Location nextMove, int nextIndex ) {
  assert(nextMove>=0); assert(nextMove<(GobanNode::height+2)*(GobanNode::width+2));
  assert(nextIndex>=-1); assert(nextIndex<(GobanNode::height)*(GobanNode::width));
  return nextMove+((nextIndex+1)<<16);
}

void MoGo::GobanNode::printStat( int numStat ) {/**assert(0);**/
  GobanUrgency urgency(height+2, width+2, 0.);
  Location location = GobanNode::indexOnArray(0,0);
  for (int i=0;i<GobanNode::height;i++, location+=2) {/**assert(0);**/
    for (int j=0;j<GobanNode::width;j++, location++) {/**assert(0);**/

      urgency[location]=getStats().getAverage(numStat, location);
    }
  }
  urgency.textModeShowUrgency();
}

void MoGo::GobanNode::printStats( int numStat1, int numStat2 ) {/**assert(0);**/
  GobanUrgency urgency(height+2, width+2, 0.);
  Location location = GobanNode::indexOnArray(0,0);
  for (int i=0;i<GobanNode::height;i++, location+=2) {/**assert(0);**/
    for (int j=0;j<GobanNode::width;j++, location++) {/**assert(0);**/

      urgency[location]=getStats().getAverage(numStat1, numStat2, location);
    }
  }
  urgency.textModeShowUrgency();
}






















MoGo::NBest::NBest( int size ) {/**assert(0);**/
  init(size);
}
void MoGo::NBest::init( int size ) {/**assert(0);**/
  this->size=size;
  smallestValue=1e10;
  indexOfSmallestValue=-1;
}

void MoGo::NBest::insert( double v, Location move, int index ) {/**assert(0);**/
  if (int(moves.size())==size) {/**assert(0);**/
    if (v<=smallestValue) return;
    moves[indexOfSmallestValue]=move;
    indexes[indexOfSmallestValue]=index;
    moveValues[indexOfSmallestValue]=v;
    smallestValue=v;
    for (int i=0;i<size;i++) {/**assert(0);**/
      if (moveValues[i]<smallestValue) {/**assert(0);**/ smallestValue=moveValues[i]; indexOfSmallestValue=i; }
    }
  } else {/**assert(0);**/
    if (v<smallestValue) {/**assert(0);**/ smallestValue=v; indexOfSmallestValue=moves.size(); }
    moves.push_back(move);
    indexes.push_back(index);
    moveValues.push_back(v);
  }
}

void MoGo::NBest::clear( ) {/**assert(0);**/
  moves.clear();
  indexes.clear();
  moveValues.clear();
}


/** Implementation of nBest in the unused values of gobanUrgency and nodeValue. It is ugly, but it is fast (avoid other allocations) */
// nodeUrgency()[1]= size
// nBestCurrentSize_= currentSize
// nodeValue()[3+1]= smallestValue
// nodeValue()[3+2]= indexOfSmallestValue
// nodeUrgency()[3 and +]= moveValues
// nodeValue()[3+3 and +]= indexes (if <0 then no child is associated, so that is the move (Location), exactly "-move-1")

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GobanNode::nBestInsert( double v, Location move, int index ) {
#ifdef KEEP_NBEST_PER_NODE
  nBest.insert(v, move, index);
#else
  int size=int(nodeUrgency()[1]), currentSize=nBestCurrentSize_;
  double smallestValue=nodeValue()[3+1];
  int indexOfSmallestValue=int(nodeValue()[3+2]);

  if (index<0) index=-move-1;

  if (currentSize==size) {
    if (v<=smallestValue) return;
    nodeUrgency()[3+indexOfSmallestValue]=v;
    nodeValue()[3+3+indexOfSmallestValue]=index;
    nodeValue()[3+1]/*smallestValue*/=v;
    for (int i=0;i<size;i++) {
      if (nodeUrgency()[3+i]<smallestValue) {
        nodeValue()[3+1]/*smallestValue*/=nodeUrgency()[3+i];
        nodeValue()[3+2]/*indexOfSmallestValue*/=i;
      }
    }
  } else {
    if (v<smallestValue) {
      nodeValue()[3+1]/*smallestValue*/=v;
      nodeValue()[3+2]/*indexOfSmallestValue*/=currentSize;
    }
    nodeUrgency()[3+currentSize]=v;
    nodeValue()[3+3+currentSize]=index;
    nBestCurrentSize_++;
  }
#endif
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GobanNode::nBestClear( ) {
#ifdef KEEP_NBEST_PER_NODE
  nBest.clear();
#else
  nBestCurrentSize_=0;
#endif
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GobanNode::nBestSetSize( int newSize ) {
	if (newSize>=GobanNode::height) newSize=GobanNode::height-1;
#ifdef KEEP_NBEST_PER_NODE
  nBest.setSize(newSize);
#else
  if (nBestCurrentSize_<0) nBestInit(newSize);
  int currentSize=nBestCurrentSize_;
  if (currentSize>newSize)
    nBestCurrentSize_=newSize;
  nodeUrgency()[1]=newSize;
  
  assert(newSize<GobanNode::height+2-2);
#endif
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GobanNode::nBestChangeIndex( Location location, int index ) {
#ifdef KEEP_NBEST_PER_NODE
  for (int i=0;i<(int)nBest.moves.size();i++) {/**assert(0);**/
    if (nBest.moves[i]==location)
      nBest.indexes[i]=index;
  }
#else
  assert(index>=0);
  int currentSize=nBestCurrentSize_;
  for (int i=0;i<(int)currentSize;i++) {
    Location move;
    int ind=int(nodeValue()[3+3+i]);
    if (ind<0) move=-ind-1;
    else move=childMove()[ind];
    if (move==location)
      nodeValue()[3+3+i]=index;
  }
#endif
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
Location MoGo::GobanNode::nBestMove(int i) const {
#ifdef KEEP_NBEST_PER_NODE
  return nBest.moves[i];
#else
  int ind=int(nodeValue()[3+3+i]);
  Location move;
  if (ind<0) move=-ind-1;
  else move=childMove()[ind];
  return move;
#endif
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
int MoGo::GobanNode::nBestIndex(int i) const {
#ifdef KEEP_NBEST_PER_NODE
  return nBest.indexes[i];
#else
  int index=int(nodeValue()[3+3+i]);
  if (index<0) return -1;
  else return index;
#endif
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
int MoGo::GobanNode::nBestCurrentSize() const {
#ifdef KEEP_NBEST_PER_NODE
  return (int)nBest.moves.size();
#else
  return nBestCurrentSize_;
#endif
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GobanNode::nBestInit( int size ) {
#ifdef KEEP_NBEST_PER_NODE
  nBest.init(size);
#else
  nodeUrgency()[1]=size;
  // nodeValue()[3+1]= smallestValue
  // nodeValue()[3+2]= indexOfSmallestValue
  nodeValue()[3+1]=1e10;
  nodeValue()[3+2]=-1;
#endif
}

void MoGo::GobanNode::addCaptureMove( Location location ) {/**assert(0);**/
#ifdef KEEP_CAPTURE_MOVES_PER_NODE
  captureMoves.push_back(location);
#else
  location=PASSMOVE;
#endif
}

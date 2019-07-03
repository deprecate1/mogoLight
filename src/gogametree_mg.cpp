//
// C++ Implementation: gogametree_mg
//
// Description:
//
//
// Author: Yizao Wang <yizao.wang@polytechnique.edu> and Sylvain Gelly <gelly@lri.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "gogametree_mg.h"
#include "gotools_mg.h"
#include <algorithm>
#include "openingdatabase_mg.h"

using namespace std;
using namespace MoGo;

namespace MoGo {
// TEMPORARY COMMENT: THIS FUNCTION IS USED
  void clearDatabase(MoGoHashMap(GobanKey, GobanNode *, hashGobanKey, equalGobanKey) &database) {
    MoGoHashMap(GobanKey, GobanNode *, hashGobanKey, equalGobanKey)().swap(database);
  }


  GoGameTree::~GoGameTree() {/**assert(0);**/
    deleteTree();
  }
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GoGameTree::deleteTree( ) {
  for (MoGoHashMap(GobanKey, GobanNode *, hashGobanKey, equalGobanKey)::
       const_iterator i = nodeAddresses.begin(); i != nodeAddresses.end() ; ++i) {
    //     delete i->second;
    GobanNode::deleteGobanNode(i->second);
  }
  rootNode=NULL;
  //   nodeAddresses.clear();
  clearDatabase(nodeAddresses);
  sharedChildrenNumber=0;
}

Vector< MoGo::GobanNode * > MoGo::GoGameTree::sharedNodes( ) {/**assert(0);**/
  Vector < GobanNode * > sharedNodeList;
  for (MoGoHashMap(GobanKey, GobanNode *, hashGobanKey, equalGobanKey)::
       iterator i = nodeAddresses.begin(); i != nodeAddresses.end() ; ++i) {/**assert(0);**/
    if (i->second->fatherNode().size()>1)
      sharedNodeList.push_back(i->second);
  }
  return sharedNodeList;
}


// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GoGameTree::reduceTreeToNewRoot( GobanNode * node ) {
  assert(node);
  showHashMapSize();
  rootNode=node;
  //   for (MoGoHashMap(GobanKey, GobanNode *, hashGobanKey, equalGobanKey)::
  //        const_iterator it = nodeAddresses.begin(); it != nodeAddresses.end() ; ++it)
  //     it->second->needToUpdateUrgencyOff();
  //GoTools::print("???\n");
  MoGoHashMap(GobanKey, GobanNode *, hashGobanKey, equalGobanKey) newHash;
  //   GoTools::print("iterateMarkNode start\n");
  iterateMarkNode(node, newHash);
  //   GoTools::print("iterateMarkNode end\n");
  //GoTools::print("???\n");
  //   Vector < GobanKey > deleteIterator;
  //   Vector < MoGoHashMap(GobanKey, GobanNode *, hashGobanKey, equalGobanKey)::iterator > deleteIterator;

  //   GoTools::print("removeFather start\n");
  for (MoGoHashMap(GobanKey, GobanNode *, hashGobanKey, equalGobanKey)::
       const_iterator it = newHash.begin(); it != newHash.end() ;++it) {
    GobanNode *node=it->second;
    for (int i=0;i<(int)node->fatherNode().size();i++)
      if (newHash.count(node->fatherNode()[i]->gobanKey())==0) {
        node->removeFather(node->fatherNode()[i]);
        i--;
      }
  }
  //   GoTools::print("delete start\n");
  for (MoGoHashMap(GobanKey, GobanNode *, hashGobanKey, equalGobanKey)::
       const_iterator it = nodeAddresses.begin(); it != nodeAddresses.end() ;++it) {
    if (newHash.count(it->first) == 0) {
      GobanNode *node=it->second;
      sharedChildrenNumber-=node->fatherNode().size()-1;
      //       delete it->second;
      GobanNode::deleteGobanNode(it->second);
    }
  }

  //   int oldSize=nodeAddresses.bucket_count();
  //   GoTools::print("clear start (%i)\n", oldSize);
  //   nodeAddresses.clear();
  clearDatabase(nodeAddresses);
#ifndef INTEL_COMPILER
  //   nodeAddresses.resize(oldSize);
#endif
 
  // nodeAddresses=newHash;
  nodeAddresses.swap(newHash);
  //   GoTools::print("copy end (%i)\n", nodeAddresses.bucket_count());
  //   showHashMapSize();
}


MoGo::GoGameTree::GoGameTree( ) {/**assert(0);**/
  rootNode=NULL;
  //evaluateNode=rootNode;
  sharedChildrenNumber=0;
  blockVariable=false;
#ifdef PROTECT_TREE_HASHTABLE_BY_MUTEX
  pthread_mutexattr_t attr; pthread_mutexattr_init(&attr); pthread_mutex_init(&mutex,&attr);
#endif
  pthread_mutexattr_t attr2; pthread_mutexattr_init(&attr2); pthread_mutex_init(&mutex2,&attr2);

}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
MoGo::GoGameTree::GoGameTree( const MoGo::Goban &goban ) {
  //   rootNode=new GobanNode(goban.getGobanKey());
  rootNode=GobanNode::newGobanNode(goban.getGobanKey());
  nodeAddresses[goban.getGobanKey()] = rootNode;
  sharedChildrenNumber=0;
  blockVariable=false;
#ifdef PROTECT_TREE_HASHTABLE_BY_MUTEX
  pthread_mutexattr_t attr; pthread_mutexattr_init(&attr); pthread_mutex_init(&mutex,&attr);
#endif
  pthread_mutexattr_t attr2; pthread_mutexattr_init(&attr2); pthread_mutex_init(&mutex2,&attr2);
}

MoGo::GoGameTree::GoGameTree( const GobanKey & gobanKey ) {/**assert(0);**/
  //   rootNode=new GobanNode(gobanKey);
  rootNode=GobanNode::newGobanNode(gobanKey);
  nodeAddresses[gobanKey] = rootNode;
  sharedChildrenNumber=0;
  blockVariable=false;
#ifdef PROTECT_TREE_HASHTABLE_BY_MUTEX
  pthread_mutexattr_t attr; pthread_mutexattr_init(&attr); pthread_mutex_init(&mutex,&attr);
#endif
  pthread_mutexattr_t attr2; pthread_mutexattr_init(&attr2); pthread_mutex_init(&mutex2,&attr2);
}

void MoGo::GoGameTree::reInitiate( const Goban & goban ) {/**assert(0);**/
  reInitiate(goban.getGobanKey());
  blockVariable=false;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GoGameTree::reInitiate( const GobanKey & gobanKey ) {
  deleteTree();
  //   rootNode=new GobanNode(gobanKey);
  rootNode=GobanNode::newGobanNode(gobanKey);
  //evaluateNode=rootNode;
  nodeAddresses[gobanKey] = rootNode;
  sharedChildrenNumber=0;
  blockVariable=false;
}



// TEMPORARY COMMENT: THIS FUNCTION IS USED
int MoGo::GoGameTree::getSharedChildrenNumber() const {
  return sharedChildrenNumber;
}


void MoGo::GoGameTree::showTreeStructure( ) const {/**assert(0);**/
  iterateShowTree(rootNode);
}

void MoGo::GoGameTree::iterateShowTree(GobanNode *node ) const {/**assert(0);**/
  //   GoTools::print("Current Node:\n");
  //node->showGoban();
  node->showChildren();
  //   GoTools::print("Children Node:%i\n",childrenList.size());
  //   for (int i=0;i<(int)childrenList.size();i++)
  //     childrenList[i]->showGoban();
  //   GoTools::print("---\n");
  for (int i=0;i<(int)node->childNode().size();i++)
    iterateShowTree(node->childNode()[i]);

}

bool MoGo::GoGameTree::goToFirstChild( ) {/**assert(0);**/
  if (currentNode->childNode().size()==0) return false;
  currentNode=currentNode->childNode()[0];
  return true;
}

void MoGo::GoGameTree::showChildrenValue( GobanNode * node ) {/**assert(0);**/
  for (int i=0;i<(int)node->childNode().size();i++) {/**assert(0);**/
    node->childNode()[i]->showGoban();
    node->childNode()[i]->nodeValue().print();
  }
}


// TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::GoGameTree::isInTheTree( const GobanKey & gobanKey ) const {
#ifdef PARALLELIZED
  //   while (blockVariable) pthread_yield(); blockVariable=true;
#ifdef PROTECT_TREE_HASHTABLE_BY_MUTEX
  pthread_mutex_lock(&mutex);
#endif

  int c=nodeAddresses.count(gobanKey);
#ifdef PROTECT_TREE_HASHTABLE_BY_MUTEX
  pthread_mutex_unlock(&mutex);
#endif
  //   blockVariable=false;
  return c!=0;
#else
  return nodeAddresses.count(gobanKey)!=0;
#endif
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
MoGo::GobanNode * MoGo::GoGameTree::getGobanNodeAddress( const GobanKey & gobanKey ) const {
#ifdef PARALLELIZED
  //   while (blockVariable) pthread_yield(); blockVariable=true;
  MoGo::GobanNode *node=0;
#ifdef PROTECT_TREE_HASHTABLE_BY_MUTEX
  pthread_mutex_lock(&mutex);
#endif
  if (nodeAddresses.count(gobanKey)!=0)
    node=nodeAddresses.find(gobanKey)->second;
#ifdef PROTECT_TREE_HASHTABLE_BY_MUTEX
  pthread_mutex_unlock(&mutex);
#endif
  //   blockVariable=false;
  return node;
#else
  if (nodeAddresses.count(gobanKey)==0) return NULL;
  else
    return nodeAddresses.find(gobanKey)->second;
#endif
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
MoGo::GobanNode * MoGo::GoGameTree::getGobanNodeAddress( const Goban & goban ) const {
  return getGobanNodeAddress(goban.getGobanKey());
}


// TEMPORARY COMMENT: THIS FUNCTION IS USED
MoGo::GobanNode * MoGo::GoGameTree::getRootNodeAddress( ) const {
  return rootNode;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::GoGameTree::isChild( GobanNode * node, const Location location ) const {
  return node->isChild(location);
}

bool MoGo::GoGameTree::isChild( GobanNode * node, const Goban & goban ) const {/**assert(0);**/
  return isChild(node, goban.getGobanKey());
}

bool MoGo::GoGameTree::isChild( GobanNode * node, const GobanKey & gobanKey ) {/**assert(0);**/
#ifdef PROTECT_TREE_HASHTABLE_BY_MUTEX
  //   pthread_mutex_lock(&mutex);
#endif
  GobanNode *child=nodeAddresses[gobanKey];
#ifdef PROTECT_TREE_HASHTABLE_BY_MUTEX
  //   pthread_mutex_unlock(&mutex);
#endif
  return node->isChild(child);
}

void MoGo::GoGameTree::exportToDotFile(const char *name, const int n) const {/**assert(0);**/
  FILE *f=fopen(name, "w");
  if (f==NULL)
    return;
  fprintf(f, "digraph unix {/**assert(0);**/\nnode [shape=rect,color=lightblue2, style=filled, fontsize=6, fixedsize=false, fontname=Courier];\n");

  for (MoGoHashMap(GobanKey, GobanNode *, hashGobanKey, equalGobanKey)::
       const_iterator it = nodeAddresses.begin(); it != nodeAddresses.end() ; ++it) {/**assert(0);**/
    if (it->second->numberOfSimulation()<=n) continue;
    Goban goban(it->second->gobanKey());
    for (int i = 0 ; i < (int)it->second->childNode().size();i++) {/**assert(0);**/
      if (it->second->childNode()[i]->numberOfSimulation()<=n) continue;

      exportGobanToFile(f, it->second);
      fprintf(f," -> ");
      exportGobanToFile(f, it->second->childNode()[i]);
      fprintf(f,";\n");
    }
  }
  fprintf(f, "}\n");
  fclose(f);
}

void MoGo::GoGameTree::exportGobanToFile( FILE * f, GobanNode *node ) {/**assert(0);**/
  fprintf(f,"\"");
  Goban goban(node->gobanKey());
  for (int i = 0 ; i < goban.height() ; i++) {/**assert(0);**/
    for (int j = 0 ; j < goban.width() ; j++) {/**assert(0);**/
      PositionState state=goban.position(goban.indexOnArray(i,j));
      if (state == EMPTY)
        fprintf(f,".");
      else if (state == BLACK)
        fprintf(f,"@");
      else if (state == WHITE)
        fprintf(f,"O");
    }
    fprintf(f,"\\n");
  }
  for (int i=0;i<(int)node->nodeValue().size();i++)
    fprintf(f,"%6.2f\\n",node->nodeValue()[i]);

  fprintf(f,"%d \"",(int)node->numberOfSimulation());
}

void MoGo::GoGameTree::dottyOut(const int n) const {/**assert(0);**/
  exportToDotFile("tmp",n);
  system("dotty tmp");
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GoGameTree::showHashMapSize( ) const {
  GoTools::print("Current hash_map size/Max hash_map size %i/%u\n",hashMapSize(),maxHashMapSize());
  GoTools::print("Memory used: %i\n",hashMapSize()*sizeof(GobanNode));
  if (rootNode)
    GoTools::print("numberOfSimulation at root: %d\n",rootNode->numberOfSimulation());
  //GoTools::print("\nCurrent hash_map size/Max hash_map size: %d/%d\n" ,hashMapSize(),maxHashMapSize());
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
unsigned int MoGo::GoGameTree::hashMapSize( ) const {
  return nodeAddresses.size();
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
unsigned int MoGo::GoGameTree::maxHashMapSize( ) const {
  return nodeAddresses.max_size();
}

void MoGo::GoGameTree::showPreferedSequence( const Goban & goban ) {/**assert(0);**/
  GobanNode * node=getGobanNodeAddress(goban);

  assert(node!=NULL);

  GoTools::print("prefered sequence: ");
  int j=0;
  while (node->childNode().size()!=0 && j<100) {/**assert(0);**/

    double urgency=-10000;

    int index=-1;
    Location location = -1;
    for (int i=0;i<(int)node->childMove().size();i++)
      if (node->nodeUrgency()[node->childMove()[i]]>urgency) {/**assert(0);**/
        urgency=node->nodeUrgency()[node->childMove()[i]];
        location=node->childMove()[i];
        index=i;
      }

    goban.textModeShowPosition(location);
    node=node->childNode()[index];
    j++;
  }
  GoTools::print("\n");

}

int MoGo::GoGameTree::sharedNumber( ) const {/**assert(0);**/
  return sharedChildrenNumber;
}



// TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::GoGameTree::iterateMarkNode( GobanNode * node, MoGoHashMap(GobanKey, GobanNode *, hashGobanKey, equalGobanKey) &newHash, bool doNotMarkNodeWithSeveralParents, bool isFirstCall, int atFirstAvoidNbChilds) {
  if (doNotMarkNodeWithSeveralParents && (node->fatherNode().size()>1))
    return false;
  node->needToUpdateUrgencyOn();
  assert(newHash.count(node->gobanKey())==0);
  newHash[node->gobanKey()]=node;
  for (int i=0;i<(int)node->childNode().size();i++) {
    if (isFirstCall && (i < atFirstAvoidNbChilds))
      continue;
    if (newHash.count(node->childNode()[i]->gobanKey())==0)
      if (!iterateMarkNode(node->childNode()[i], newHash, doNotMarkNodeWithSeveralParents, false))
        return false;
  }
  return true;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::GoGameTree::iterateMarkNodeForCut( GobanNode * node, MoGoHashMap(GobanKey, GobanNode *, hashGobanKey, equalGobanKey) &newHash ) {
  //   if (doNotMarkNodeWithSeveralParents && /*(!isFirstCall) &&*/ (node->fatherNode().size()>1))
  //     return false;
  if (node==rootNode)
    return false;
  node->needToUpdateUrgencyOn();
  assert(newHash.count(node->gobanKey())==0);
  newHash[node->gobanKey()]=node;
  for (int i=0;i<(int)node->childNode().size();i++) {
    //     if (isFirstCall && (i < atFirstAvoidNbChilds))
    //       continue;
    if (newHash.count(node->childNode()[i]->gobanKey())==0)
      if (!iterateMarkNodeForCut(node->childNode()[i], newHash))
        return false;
  }
  return true;
}

bool MoGo::GoGameTree::deleteSubTreeAtNode( GobanNode * node, int nbChildsAlreadyDone ) {/**assert(0);**/
  if (int(node->childNode().size())<=nbChildsAlreadyDone) // nothing to do :-)
    return node->fatherNode().size()==1; // if the node has several parents we return false
  MoGoHashMap(GobanKey, GobanNode *, hashGobanKey, equalGobanKey) newHash;
  if (!iterateMarkNode(node, newHash, true, true, nbChildsAlreadyDone))
    return false;
  newHash.erase(node->gobanKey());

  for (MoGoHashMap(GobanKey, GobanNode *, hashGobanKey, equalGobanKey)::
       const_iterator it = newHash.begin(); it != newHash.end() ;++it) {/**assert(0);**/
    GobanNode *node=it->second;
    assert(node->fatherNode().size() == 1);
    for (int i=0;i<(int)node->fatherNode().size();i++)
      if (newHash.count(node->fatherNode()[i]->gobanKey())==0)
        node->fatherNode()[i]->removeChild(node);
    for (int i=0;i<(int)node->childNode().size();i++)
      if (newHash.count(node->childNode()[i]->gobanKey())==0)
        node->childNode()[i]->removeFather(node);
  }
  for (MoGoHashMap(GobanKey, GobanNode *, hashGobanKey, equalGobanKey)::
       const_iterator it = newHash.begin(); it != newHash.end() ;++it) {/**assert(0);**/
    GobanNode *node=it->second;
    sharedChildrenNumber-=node->fatherNode().size()-1;
    //       delete it->second;
    nodeAddresses.erase(node->gobanKey());
    GobanNode::deleteGobanNode(node);
  }
  return true;
}





class compareTwoNodes {
public:

// TEMPORARY COMMENT: THIS FUNCTION IS USED
  bool operator()(GobanNode *& x, GobanNode *& y) {
    if (x->fatherNode().size()==0)
      return true;
    else if (y->fatherNode().size()==0)
      return false;
    else
      return x->fatherNode()[0]->numberOfSimulation()>y->fatherNode()[0]->numberOfSimulation();
  }

};

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GoGameTree::cutTree( int newSize ) {
  if (newSize>=int(hashMapSize())-1000)
    return;
  int nbToDelete=hashMapSize()-newSize;
  Vector<GobanNode *> counts(nodeAddresses.size());
  int i=0;
  for (MoGoHashMap(GobanKey, GobanNode *, hashGobanKey, equalGobanKey)::
       const_iterator it = nodeAddresses.begin(); it != nodeAddresses.end() ; ++it) {
    counts[i]=it->second;
    i++;
  }
  MoGoHashMap(GobanKey, GobanNode *, hashGobanKey, equalGobanKey) newHash;
  make_heap(counts.begin(), counts.end(),compareTwoNodes());
  for (int i=0;(i<int(counts.size())) && (int(newHash.size())<nbToDelete);i++) {
    pop_heap(counts.begin(), counts.end()-i,compareTwoNodes());
    GobanNode *node=counts[counts.size()-i-1];
    if (node->fatherNode().size()>0 && node->fatherNode()[0]->fatherNode().size()>0 && node->fatherNode()[0]->fatherNode()[0]->fatherNode().size()>0) { // not the root and not a child child of root
      node=node->fatherNode()[0]; // take the father
      if (newHash.count(node->gobanKey())==0)
        iterateMarkNodeForCut(node, newHash);
    }
    //     GoTools::print("node sim = %i (%i)\n", node->numberOfSimulation(), i);
    //     if (i<int(counts.size()-1))
  }
  for (MoGoHashMap(GobanKey, GobanNode *, hashGobanKey, equalGobanKey)::
       const_iterator it = newHash.begin(); it != newHash.end() ;++it) {
    GobanNode *node=it->second;
    //     assert(node->fatherNode().size() == 1);
    for (int i=0;i<(int)node->fatherNode().size();i++)
      if (newHash.count(node->fatherNode()[i]->gobanKey())==0)
        node->fatherNode()[i]->removeChild(node);
    for (int i=0;i<(int)node->childNode().size();i++)
      if (newHash.count(node->childNode()[i]->gobanKey())==0)
        node->childNode()[i]->removeFather(node);
  }
  for (MoGoHashMap(GobanKey, GobanNode *, hashGobanKey, equalGobanKey)::
       const_iterator it = newHash.begin(); it != newHash.end() ;++it) {
    GobanNode *node=it->second;
    sharedChildrenNumber-=node->fatherNode().size()-1;
    nodeAddresses.erase(node->gobanKey());
    GobanNode::deleteGobanNode(node);
  }
  clearDatabase(newHash);
  clearVector(counts);
  GoTools::print("cut tree, to delete %i, now %i\n", nbToDelete, nodeAddresses.size());

}





void MoGo::GoGameTree::addToDatabase( Database3 & database, int minimumNumberForChild ) const {/**assert(0);**/
  Vector<GobanNode *> counts(nodeAddresses.size());
  int i=0;
  for (MoGoHashMap(GobanKey, GobanNode *, hashGobanKey, equalGobanKey)::
       const_iterator it = nodeAddresses.begin(); it != nodeAddresses.end() ; ++it) {/**assert(0);**/
    counts[i]=it->second;
    i++;
  }
  for (int i=0;(i<int(counts.size()));i++) {/**assert(0);**/
    GobanNode *node=counts[i];
    if (node->numberOfSimulation()<minimumNumberForChild) continue;
    for (int j=0;j<(int)node->childNode().size();j++) {/**assert(0);**/
      GobanNode *child=node->childNode()[j];
      if (child->numberOfSimulation()<minimumNumberForChild) continue;

      double v=child->nodeValue()[1]/double(child->numberOfSimulation());
      double forUs=double(child->numberOfSimulation())*(1.-v);
      double forHim=double(child->numberOfSimulation())*(v);

      database.addValues(node->gobanKey(),node->childMove()[j],forUs,forHim);

    }
  }
}







GobanNode * MoGo::GoGameTree::getNodeFromSequence( const Vector< Location > & sequence ) const {/**assert(0);**/
  GobanNode *node=getRootNodeAddress();
  int i=0;
  while (node) {/**assert(0);**/
    if (i>=(int)sequence.size()) {/**assert(0);**/
      return node;
    }
    node=node->getChild(sequence[i]);
    i++;
  }
  return 0;
}

void MoGo::GoGameTree::lock()
{
	while (pthread_mutex_lock(&mutex2));

}

void MoGo::GoGameTree::unlock()
{
 pthread_mutex_unlock(&mutex2);
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::GoGameTree::connectToTree( GobanNode * node, const GobanKey & key, Location location, int childIndex, GobanNode *& childPlayed, bool lock ) {
  // GoTools::print("grr\n"); lock=true;
  if (childIndex>=0) {/**assert(0);**/
    assert(childIndex<(int)node->childNode().size());
    childPlayed=node->childNode()[childIndex];
    return false;
  }
#ifdef PROTECT_TREE_HASHTABLE_BY_MUTEX
  assert(0);
  pthread_mutex_lock(&mutex);
#endif
#ifdef PARALLELIZED
  //   if (lock) while (pthread_mutex_trylock(&mutex2));
  if (lock) while (pthread_mutex_lock(&mutex2));
#else
  lock=false; // to avoid a warning at compilation time
#endif

  GobanNode *child=nodeAddresses[key];
  childPlayed=child;

  if (child==0)
    //the playerGoban is not saved in the game tree then should be a new node,
    //for the sequence and for the tree. thus, a reset of all the urgency is required.
  {
    MoGo::GobanNode *newNode=GobanNode::newGobanNode(key);


    nodeAddresses[key] = newNode;
    //  GoTools::print("node childs %i %i\n", node->childMove().size(), node->childNode().size());
    node->addChild(location,newNode);
    childPlayed=newNode;


#ifdef PARALLELIZED
    if (lock) pthread_mutex_unlock(&mutex2);
#endif
#ifdef PROTECT_TREE_HASHTABLE_BY_MUTEX
    pthread_mutex_unlock(&mutex);
#endif
    return true;
  } else if (!isChild( node , location )) {
    assert(child==nodeAddresses[key]);
    //FIXME: was child=nodeAddresses[key];
    node->addChild(location,child);
    childPlayed=child;
    sharedChildrenNumber++;
  }



#ifdef PARALLELIZED
  if (lock) pthread_mutex_unlock(&mutex2);
#endif
#ifdef PROTECT_TREE_HASHTABLE_BY_MUTEX
  pthread_mutex_unlock(&mutex);
#endif
  return false;
}

bool MoGo::GoGameTree::connectToTree( GobanNode * node, Goban & currentGoban, Location location, int childIndex, GobanNode *&childPlayed, bool lock) {/**assert(0);**/
  return connectToTree(node, currentGoban.getGobanKey(), location, childIndex, childPlayed, lock);
}



void MoGo::GoGameTree::removeChild( GobanNode * node, Location location ) {/**assert(0);**/
  int index=-1;
  for (int i=0;i<(int)node->childMove().size();i++) {/**assert(0);**/
    if (node->childMove()[i]==location) {/**assert(0);**/
      index=i;
      break;
    }
  }
  if (index<0) return; // no child corresponding to that move

  MoGoHashMap(GobanKey, GobanNode *, hashGobanKey, equalGobanKey) newHash;
  GobanNode *child=node->childNode()[index];
  iterateMarkNodeForCut(child, newHash);

  for (MoGoHashMap(GobanKey, GobanNode *, hashGobanKey, equalGobanKey)::
       const_iterator it = newHash.begin(); it != newHash.end() ;++it) {/**assert(0);**/
    GobanNode *node=it->second;
    //     assert(node->fatherNode().size() == 1);
    for (int i=0;i<(int)node->fatherNode().size();i++)
      if (newHash.count(node->fatherNode()[i]->gobanKey())==0)
        node->fatherNode()[i]->removeChild(node);
    for (int i=0;i<(int)node->childNode().size();i++)
      if (newHash.count(node->childNode()[i]->gobanKey())==0)
        node->childNode()[i]->removeFather(node);
  }
  for (MoGoHashMap(GobanKey, GobanNode *, hashGobanKey, equalGobanKey)::
       const_iterator it = newHash.begin(); it != newHash.end() ;++it) {/**assert(0);**/
    GobanNode *node=it->second;
    sharedChildrenNumber-=node->fatherNode().size()-1;
    nodeAddresses.erase(node->gobanKey());
    GobanNode::deleteGobanNode(node);
  }
  clearDatabase(newHash);
}


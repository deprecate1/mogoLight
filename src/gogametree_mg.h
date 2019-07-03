//
// C++ Interface: gogametree_mg
//
// Description:
//
//
// Author: Yizao Wang <yizao.wang@polytechnique.edu> and Sylvain Gelly <gelly@lri.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MOGOGOGAMETREE_MG_H
#define MOGOGOGAMETREE_MG_H

#include "gobannode_mg.h"
#include <ext/hash_map>

namespace MoGo {
  class NodeValue;
  class Database3;
  /**
  The tree keeping the results of the simulations
   
  @author Yizao Wang and Sylvain Gelly
  */



  class GoGameTree {
  public:
    GoGameTree();
    /**A given goban as the root of the evaluator(tree). Not very neat yet, e.g. do we need to save all the simulations that have been ever made? (Maybe another class?)*/
    GoGameTree(const Goban &goban );
    GoGameTree(const GobanKey &gobanKey);
    ~GoGameTree();



    void reInitiate(const Goban &goban);
    void reInitiate(const GobanKey &gobanKey);
    void showTreeStructure() const;
    /** Give a position, only a very easy and stupid test.*/
    bool isInTheTree(const GobanKey &gobanKey) const;
    GobanNode* getGobanNodeAddress(const GobanKey &gobanKey) const;
    GobanNode* getGobanNodeAddress(const Goban &goban) const;
    GobanNode* getRootNodeAddress() const;

    GobanNode* addChildToNode(GobanNode *node, const Location location, const Goban &goban) ;
    /** Add the node associated to gobanKey as a child for node. Create a new node if there is no node associated to gobanKey. The added node must NOT already be a child of node. Return a pointer on the added node */
    GobanNode* addChildToNode(GobanNode *node, const Location location, const GobanKey &gobanKey);
    GobanNode* addChildToNode(GobanNode *node, const Location location);
    void addChildToNode(GobanNode *node, const Location location, GobanNode *child);

    bool isChild(GobanNode* node, const Location location) const ;
    bool isChild(GobanNode* node, const Goban &goban) const ;
    bool isChild(GobanNode* node, const GobanKey &gobanKey) ;

    /** export the tree to a file that graphviz (dotty program) can interpret and display */
    void exportToDotFile(const char *name, const int n = 0) const;
    void dottyOut( const int n = 0 ) const;
    int getSharedChildrenNumber() const;

    void showHashMapSize() const;
    unsigned int hashMapSize() const;
    unsigned int maxHashMapSize() const;

    //void updateNodeInformation( GobanNode * node, const NodeValue & value);
    //void updateNodeInformation( GobanNode * node, const double value, const int number = 1 , const double variance = 0);
    void showPreferedSequence( const Goban & goban  ) ;
    void reduceTreeToNewRoot( GobanNode * node );
    bool deleteSubTreeAtNode(GobanNode * node, int nbChildsAlreadyDone);

    int sharedNumber() const;
    Vector< MoGo::GobanNode * > sharedNodes( ) ;
    void deleteTree();
    void cutTree(int newSize);
    void addToDatabase(Database3 &database, int minimumNumberForChild) const;
    GobanNode *getNodeFromSequence(const Vector<Location> &sequence) const;

    bool connectToTree(GobanNode *node, Goban &currentGoban, Location location, int childIndex, GobanNode *&childPlayed, bool lock=false);
    bool connectToTree(GobanNode *node, const GobanKey &key, Location location, int childIndex, GobanNode *&childPlayed, bool lock=false);
    void removeChild(GobanNode *node, Location location);
    void lock();
    void unlock();
  private:
    /** Here the structure I am not very clear yet.*/
    GobanNode* rootNode;
    //GobanNode* evaluateNode;//these two lines are useless?
    GobanNode* currentNode;
    Goban testGoban;
    void iterateShowTree(GobanNode *node) const;

    bool goToFirstChild();//Just for test
    /** For the deconstructor*/
    //     void iterateDeleteNode(GobanNode* node);
    void showChildrenValue(GobanNode* node);

    MoGoHashMap(GobanKey, GobanNode *, hashGobanKey, equalGobanKey) nodeAddresses;

    static void exportGobanToFile(FILE *f, GobanNode *node);
    int sharedChildrenNumber;


    bool iterateMarkNode( GobanNode * node, MoGoHashMap(GobanKey, GobanNode *, hashGobanKey, equalGobanKey) &newHash, bool doNotMarkNodeWithSeveralParents=false, bool isFirstCall=true, int atFirstAvoidNbChilds=0 );
    bool iterateMarkNodeForCut( GobanNode * node, MoGoHashMap(GobanKey, GobanNode *, hashGobanKey, equalGobanKey) &newHash );

    mutable bool blockVariable;
#ifdef PROTECT_TREE_HASHTABLE_BY_MUTEX
    mutable pthread_mutex_t mutex;
#endif
    mutable pthread_mutex_t mutex2;

  };



}

#endif

//
// C++ Interface: treeupdator_mg
//
// Description:
//
//
// Author: Yizao Wang <yizao.wang@polytechnique.edu> and Sylvain Gelly <gelly@lri.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MOGOTREEUPDATOR_MG_H
#define MOGOTREEUPDATOR_MG_H

#include "goban_mg.h"
#include "gobannode_mg.h"
#include "fastsmallvector_mg.h"
#include "typedefs_mg.h"
#include "gogametree_mg.h"


namespace MoGo {
  class MoveSelectorGlobal;
  class FastBoard;
  /**
  Who, given a sequence and a score of one simulation, updates the node values.

  We have A LOT here, but only the UCTUpdator and UCTParallelizedUpdator are interesting. Ignore the others.

   
  @author Yizao Wang, Sylvain Gelly
  */

  class NodeSequence {
  public:
    NodeSequence();
    ~NodeSequence();
    /** Add a node in the sequence. The argument "index" is the index on the child array of its parent node. If index < 0 then it is calculated. */
    void push_back(GobanNode * node, int index=-1, int keepGobanKeys=1);
    void push_BestValue(Location move, double value);
    void pop_back();
    Location getNextMove(const int i) const;
    Location getNextMoveIndex(const int i) const;
    int size() const ;
    void clear();
    inline GobanNode* operator[](int i) const {
      assert(i>=0);
      assert(i<(int)nodeAddress.size());
      return nodeAddress[i];
    }
    inline const GobanKey & getSuperKoKey(int i) const {
      return superKoKeys[i];
    }
    inline Location getHighestValueMove(int i) const {
      return highestValueMoves[i];
    }
    inline double getHighestValue(int i) const {
      return highestValues[i];
    }
    inline int getHighestValuesSize() const {
      assert(highestValueMoves.size()==highestValues.size());
      return highestValues.size();
    }
    inline bool operator == (const NodeSequence &seq) const {
      if (nodeAddress.size() != seq.nodeAddress.size())
        return false;
      for (int i=0;i<(int)nodeAddress.size();i++)
        if (nodeAddress[i] != seq.nodeAddress[i])
          return false;
      return true;
    }
    bool isBeginOf(const NodeSequence &seq) const {
      if (nodeAddress.size() > seq.nodeAddress.size())
        return false;
      for (int i=0;i<(int)nodeAddress.size();i++)
        if (nodeAddress[i] != seq.nodeAddress[i])
          return false;
      return true;
    }
    NodeSequence subSequence(int size) const;

  private:
    //     FastSmallVector < GobanNode *> nodeAddress;
    //     FastSmallVector < int > index;
    Vector< GobanNode *> nodeAddress;
    Vector< int > index;
    Vector<GobanKey> superKoKeys;
    Vector<Location> highestValueMoves;Vector<double> highestValues;
    bool blockVariable;
  };


  class TreeUpdator {

  public:
    TreeUpdator();

    virtual ~TreeUpdator();
  
    //virtual void nodeValueInitiation(GobanNode * node);  //TOBEREMOVED
    /** Clone the object (return a copy of itself) */
    virtual TreeUpdator *clone() const =0;
    virtual void changeMode ( int mode ) ;
  
  };

  class NumberOfSimulationUpdator : public TreeUpdator {
    
  public:
    NumberOfSimulationUpdator(int mode=0);
    virtual ~NumberOfSimulationUpdator();
  
    /** Clone the object (return a copy of itself) */
    virtual NumberOfSimulationUpdator *clone() const { return new NumberOfSimulationUpdator(*this); }
  private:
    int mode;
  };


  class UCTUpdator : public TreeUpdator {
  public:
    UCTUpdator( double discountBeta, const int mode = 0, const int meanIndex = 1, const int varianceIndex = 2, int keepNodesOrdered=0);
    virtual ~UCTUpdator();
 
    //virtual void nodeValueInitiation( GobanNode * node );  //TOBEREMOVED
    /** Clone the object (return a copy of itself) */
    virtual UCTUpdator *clone() const { return new UCTUpdator(*this); }
    virtual void changeMode ( int mode ) ;
  protected:
    NumberOfSimulationUpdator numberOfSimulationUpdator;
    int meanIndex;
    int varianceIndex;
    int mode;
    double discountBeta;
  

  };

 

  class UCTUpdatorGlobal : public UCTUpdator {
    friend class MoveSelectorGlobal;
  public:
    UCTUpdatorGlobal( int useNewTrainOneGame, double powUpdate, double alphaUpdate, int avoidAMAFPreventInTree, int keepSequences, double discountBeta, int useEyes, double lambdaForAMAF2, double lambdaForAMAF, int keepNodesOrdered, double coefTwice, double betaForTerritoryScore, double coefForTerritoryScore, int equivalentSimulationsForInitialization, int updateFormula, int useEndTerritories, bool initializeByGrandFather, bool globalPlayerUseMemoryEconomy, bool keepDataForCorrelations, const int treeDepthAllAtFirstLimit, const int simulationDepthAllAtFirstLimit, const int mode = 0, const int meanIndex = 1, const int varianceIndex = 2);
    virtual ~UCTUpdatorGlobal();
  
   
   
   

    virtual void updateAllInOne(MoveSelectorGlobal *moveSelector, GoGameTree & tree, const NodeSequence & currentSequence, const Goban & goban, double score , const double komi, const FastSmallVector<int> &allMoves, Vector<int> &allAtFirstData, Vector<int> &allAtFirstDataInTree, const Vector < Location > &currentSequenceMove , double weightSimu );

   
    
    /** Clone the object (return a copy of itself) */
    virtual UCTUpdatorGlobal *clone() const { return new UCTUpdatorGlobal(*this); }
    //virtual void nodeValueInitiation( GobanNode * node );  //TOBEREMOVED
    int getTreeDepthAllAtFirstLimit() const {
      return treeDepthAllAtFirstLimit;
    }
    int getSimulationDepthAllAtFirstLimit() const {
      return simulationDepthAllAtFirstLimit;
    }
    bool useMemoryEco() const {
      return globalPlayerUseMemoryEconomy;
    }
    bool getUseEndTerritories() const { return (bool)useEndTerritories; }
    int getUseEyes() const { return useEyes; }
    //     const Vector<double> &getTerritoriesAtRoot() const { return territoriesScores;}
    //     double getNbForTerritoriesAtRoot() const { return nbTerritoriesScores; }
   
    GobanNode *getReferenceNode(GobanNode *node, int numStat, int nbSimulationsMin) const;
    int getAvoidAMAFPreventInTree() const { return avoidAMAFPreventInTree;}
    //void nodeValueInitiationFurther(GobanNode * node); //TOBEREMOVED
  private:
  
   
   
   
   
   
   
    
  
    int treeDepthAllAtFirstLimit;
    int simulationDepthAllAtFirstLimit;
    /** data to later compute then correlations between the "all at first" average and the "classical" average,  */
    Vector<Vector<double> >allAtFirstAverages1;
    Vector<Vector<double> >classicalAverages1;
    Vector<Vector<double> >otherNumber1;
    Vector<Vector<double> >allAtFirstAverages2;
    Vector<Vector<double> >classicalAverages2;
    Vector<Vector<double> >otherNumber2;
    bool keepDataForCorrelations;
  
    bool globalPlayerUseMemoryEconomy;
    bool initializeByGrandFather;
    int useEndTerritories;
    int useEyes;
    int updateFormula;
    int equivalentSimulationsForInitialization;
    double coefForTerritoryScore;
    double betaForTerritoryScore;
    double coefTwice;
    double stopUsingInitializationDirtyHack;
    double lambdaForAMAF;
    double lambdaForAMAF2;
    int keepSequences;
    int avoidAMAFPreventInTree;
    double powUpdate;
    double alphaUpdate;
    int useNewTrainOneGame;

    double logistic(double x) const;
    double logisticDeriv(double x) const;

  



  };


}
#endif

//
// C++ Interface: gobannode_mg
//
// Description:
//
//
// Author: Yizao Wang <yizao.wang@polytechnique.edu> and Sylvain Gelly <gelly@lri.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MOGOGOBANNODE_MG_H
#define MOGOGOBANNODE_MG_H

#include "typedefs_mg.h"
#include "goban_mg.h"
#include "fastsmallvector_mg.h"
#include "vectormemorypool_mg.h"

#define NUMBEROFNODEVALUE 3

typedef NODE_VALUE_TYPE NodeValueType ;

namespace MoGo {

  class NBest {
    friend class MoveSelectorGlobal;
    friend class GobanNode;
    NBest(int size=4);
    ~NBest() {}
    void insert(double v, Location move, int index);
    void clear();
    void setSize(int newSize) {
      if (newSize<size && int(moves.size())>newSize) {
        moves.resize(newSize);
        indexes.resize(newSize);
        moveValues.resize(newSize);
      }
      size=newSize;
    }
    void init(int size);
  private:
    int size;
    Vector<Location> moves;
    Vector<int> indexes;
    Vector<double> moveValues;
    double smallestValue;
    int indexOfSmallestValue;
  };


  class NodeValue {
    friend class GobanNode;
  public :
    NodeValue( int size = 1 );
    ~NodeValue();

    void print() const;
    inline int size() const { return valueArray.size();};
    inline void resize(int size) {valueArray.resize(size,0.);};

    inline NodeValueType & operator[](int i) {
      //       assert(i>=0);assert(i<(int)valueArray.size());
      return valueArray[i];
    }
    inline const NodeValueType & operator[](int i) const {
      //       assert(i>=0);assert(i<(int)valueArray.size());
      return valueArray[i];
    }
    VectorMemoryPool < NodeValueType > &getValueArray() {
      return valueArray;
    }
    const VectorMemoryPool < NodeValueType > &getValueArray() const {
      return valueArray;
    }
    void clear();
  private :
    VectorMemoryPool < NodeValueType > valueArray;
  };

  class NodeStats {
    friend class GobanNode;
  public:
    NodeStats(int size) {
      this->size=size;
    }
    inline void addOneStat(NodeValueType discount) {
      allValues.resize(allValues.size()+size);
      allValues[allValues.size()-size]=0;
      allValues[allValues.size()-size+1]=discount;
    }
    inline NodeValueType & value(int i, int j) {
      return allValues[size*i+j];
    }
    inline const NodeValueType & value(int i, int j) const {
      return allValues[size*i+j];
    }
    inline void addValue(int i, int j, const NodeValueType &v) {
      allValues[size*i+j]=allValues[size*i+j]*allValues[size*i+1]+v;
    }
    inline NodeValueType getValue(int i, int j) {
      return allValues[size*i+j];
    }
    inline void setValue(int i, int j, const NodeValueType &v) {
      allValues[size*i+j]=v;
    }
    template<class T> void addAllValues(int i, const T &v) {
      assert(int(v.size())>=size);
      int start=i*size;
      NodeValueType discount=allValues[size*i+1];
      allValues[start]=allValues[start]*discount+1;
      for (int j=2;j<size;j++)
        allValues[start+j]=allValues[start+j]*discount+v[j];
    }
    inline void increment(int i) {
      allValues[size*i]=allValues[size*i]*allValues[size*i+1]+1;
    }
    inline NodeValueType getAverage(int i, Location j) const {
      return allValues[size*i+j]/(allValues[size*i]+1e-10);
    }
    inline NodeValueType getAverage(int numStat1, int numStat2, Location location) const {
      return allValues[size*numStat1+location]/(allValues[size*numStat2+location]+1e-10);
    }
    inline void clear() {
      allValues.clear();
      //       size=0;
    }
    inline int getSize() const {
      return size;
    }
    inline int getNbStats() const {
      assert(size>0);
      return allValues.size()/size;
    }
    inline NodeValueType getNbIterations(int i) const {
      return allValues[i*size];
    }
  private:
    VectorMemoryPool<NodeValueType> allValues;
    int size;
  };


  /**
  For node information, on which is based the operation of tree.
   
  @author Yizao Wang, Sylvain Gelly
  */

  class GobanNode {
    friend class QuickGobanNodeMemoryManagement;
    friend class GobanUrgency;
    friend class NodeStats;
    friend class MoveSelectorGlobal;
    friend class UCTUpdatorGlobal;
	  public:
#ifdef GOLDENEYE
    std::vector<int> guessLocation;
    std::vector<int> guessColor;
    std::vector<double> guessWeight;
#endif
  private:
    /** for some historical reason, we keep some values like nodeUrgency_ and nodeBitmap_ in the node, which is not necessary in the point of view of optimisation. However, these are done in order to make the codes more general and flexible.*/
    GobanKey nodeGobanKey;
    NodeValue nodeValue_;
    GobanUrgency nodeUrgency_;
    bool isNodeUrgencyUpToDate;
#ifdef NUMBER_OF_SIMULATION_DOUBLE
    double numberOfSimulation_;
#else
    int numberOfSimulation_;
#endif

  public:
#ifdef GOLDENEYE
    int golden;
    int goldenbis;
    
#ifdef GOLDENEYE_MC
    std::vector<int> semeaiW;
    std::vector<int> semeaiB;
#endif  //GOLDENEYE_MC
#endif  //GOLDENEYE
    
    double winBlack[6];
    double winWhite[6];
    double lossBlack[6];
    double lossWhite[6];
    double nbMC;

    Vector<double> expertise_;
    //double expertise[441];
    //GobanUrgency expertise_;

    #ifdef CRITICALITY
      private:
        int* whiteWinsLocationIsWhite_;
        int* whiteWinsLocationIsBlack_;
        int* blackWinsLocationIsWhite_;
        int* blackWinsLocationIsBlack_;
      public:
        double AverageTerritory;
        double VarianceTerritory;
        void initializeCriticality();
        void setWinsForLocationOwner(bool isBlackWin, int locOwner, int loc);
        double getLocationCriticalityValue(int location);
        std::list<int> sortPossibleLocationsByCriticality(int* possibleLocations, int index);
        std::list<int> getLocationsWithBestCriticality(std::list<int> locations);
    #endif




#ifdef USE_MUTEX_PER_NODE
public:
    mutable pthread_mutex_t nodeMutex;
    
private:
#endif

#ifdef KEEP_STACK_MOVE_VECTOR_PER_NODE
    Vector<Location> frontLocations;
    int emptyLocationsSize;
    int beginningUnseenFrontLocations;
#endif
#ifdef KEEP_STRINGS_ON_NODES
    Vector<Location> allStringLocations;
    bool stringLocationsSet;
#endif

    bool isBlackToPlay;
    Vector< GobanNode* > fatherNodeList;
#ifdef USE_CHILD_ALLOCATED_VECTOR
    VectorMemoryPool< GobanNode* > childNodeList;
    VectorMemoryPool< Location > childMoveList;
#else
    Vector< GobanNode* > childNodeList;
    Vector< Location > childMoveList;
#endif
#ifdef KEEP_NBEST_PER_NODE
    NBest nBest;
#else
    int nBestCurrentSize_;
#endif

#ifdef KEEP_CAPTURE_MOVES_PER_NODE
    Vector<Location> captureMoves;
#endif

    int countDownForConsideringAllMoves;


    VectorMemoryPool<NodeValueType> bonusPrecomputed;
    Bitmap nodeBitmap_;
    bool isNodeBitmapCreated_;
    bool isLastMoveAnswered_;
    NodeStats stats;

    int xIndex( const Location location ) const ;
    int yIndex( const Location location ) const ;
public:
    void textModeShowPosition( const Location location ) const;
private:
    bool isInitiated;
    //     GobanNode *nextChildToKeep;
    int nextInfoForMoveToChoose;

    //     GobanNode(const Goban &goban , GobanNode* fatherNode = NULL, const int nodeValueSize = 1 );
    /** create one node which represents the Goban corresponding to gobanKey, with father node given. if father node is NULL as default, then this node will be the root node of a tree to be created later.*/
    //     GobanNode(const GobanKey &gobanKey,GobanNode* fatherNode = NULL, const int nodeValueSize = 1 );
    GobanNode(const GobanKey &gobanKey);
    ~GobanNode();

    /** this function cuts the tree existed so that only the subtree of node corresponding to gobanKey is saved. this is useful when MoGo is thinking while the opponent is thinking.*/
    void reinitiate(const GobanKey &gobanKey);

    //void initiateGobanUrgencyMode0(const Goban & goban);
    //void initiateGobanUrgencyMode1(const Goban & goban, const Bitmap * bitmap);
    //void initiateGobanUrgencyMode2(const Goban & goban, const Bitmap * bitmap);

  public:
#ifdef ANTILOCK
    int isLocked;
    int isReallyLocked;
#endif

#ifdef SUMTRICK
    int isShared;
    double *lastShared;
#endif
    /** Now this function could be replaced by GobanLocation::locationXY() */
    Location indexOnArray(const int x, const int y) const { return (x+1)*(width+2)+(y+1);};
    static int height;
    static int width;
    static GobanNode *newGobanNode(const GobanKey &gobanKey);
    static void deleteGobanNode(GobanNode *node);

    //GobanNode* father() const { return fatherNode;};
    //GobanNode* pointer() const { return this;}
#ifdef USE_CHILD_ALLOCATED_VECTOR
    const VectorMemoryPool<GobanNode*> &childNode() const { return childNodeList;}
    const VectorMemoryPool< Location > &childMove() const { return childMoveList;}
#else
    const Vector<GobanNode*> &childNode() const { return childNodeList;}
    const Vector< Location > &childMove() const { return childMoveList;}
#endif
    const Vector< GobanNode * > &fatherNode() const { return fatherNodeList;}

#ifdef NUMBER_OF_SIMULATION_DOUBLE
    inline double numberOfSimulation() const { return numberOfSimulation_;}
    void resetNumberOfSimulation( double number ) {numberOfSimulation_=number;}
#else
    inline int numberOfSimulation() const { return numberOfSimulation_;}
    void resetNumberOfSimulation( int number ) {numberOfSimulation_=number;}
#endif


    inline NodeValue & nodeValue() {return nodeValue_;}
    inline const NodeValue & nodeValue() const {return nodeValue_;}
    inline GobanUrgency & nodeUrgency() {return nodeUrgency_;}
    inline const GobanUrgency & nodeUrgency() const { return nodeUrgency_;}
    inline Vector<double> & expertise() {return expertise_;}
    inline const Vector<double> & expertise() const { return expertise_;}
    inline double get_expertise(int loc)
      {
	assert(loc>=0);
	
	if(loc<(int)expertise_.size())
	  return expertise_[loc];
	
	return 0;
      }
    inline const Bitmap& bitmap() const {return nodeBitmap_;}
    inline Bitmap& bitmap() {return nodeBitmap_;}
    inline const bool& isNodeBitmapCreated() const {return isNodeBitmapCreated_;}
    inline void setNodeBitmapCreated(bool value = true) {isNodeBitmapCreated_= value;}
    inline const GobanKey &gobanKey() const { assert(nodeGobanKey.size() > 0); return nodeGobanKey;}
    inline VectorMemoryPool<NodeValueType> &getBonusPrecomputed() { return bonusPrecomputed;}
    inline const VectorMemoryPool<NodeValueType> &getBonusPrecomputed() const { return bonusPrecomputed;}
    //     const GobanKey &gobanKey() const;
    GobanKey superKoGobanKey() const;
    void showGoban(const Location location = 0) const;
    void showChildren() const;
    void showFathers() const ;

    void addChild(const Location location, GobanNode* childNode);
    //     void foundFather(GobanNode &father);

    bool isChild(const Location location) const;
    bool isChild(const GobanNode * node) const;
    GobanNode *getChild(const Location location) const;
    /** return the move associated to this node, and -1 if the node is not found */
    Location getChild(const GobanNode * node) const;

    void showNodeInformation() const;

    void printGobanKey( ) const;

    //.....
    //int getHeight() const { return height;};
    //int getWidth() const { return width;};

    void needToUpdateUrgencyOn() { isNodeUrgencyUpToDate=false; }
    void needToUpdateUrgencyOff() { isNodeUrgencyUpToDate=true; }
    bool isUrgencyUpToDate() const { return isNodeUrgencyUpToDate; }
    int getHeight() const { return height;}
    int getWidth() const { return width;}
    void setInitiated(bool isInitiated);
    bool getInitiated() const;
    void removeFather(GobanNode *node);
    void removeChild(GobanNode *node);
    void resizeUrgency();
    void resizeExpertise();
    //     void setGobanKey(const GobanKey &gobanKey);
    void setNodeBitmapByNodeUrgency();
    static void reorderChildsBySimulations(GobanNode *parent, GobanNode *child);
    inline NodeStats &getStats()  { return stats;}
    inline const NodeStats &getStats() const {return stats;}
    int depth() const;
    bool isShallowerThan(int maxDepth) const;

    void addNeighborhoodToBitmap(const Goban &goban, Location location);
    bool extendBitmapNeighborhood(const Goban &goban);
    void clearNeighborhood();
    int getEmptyLocationsSize() const;
    void setNextInfoForMoveToChoose(int _nextInfoForMoveToChoose) { nextInfoForMoveToChoose=_nextInfoForMoveToChoose;}
    int getNextInfoForMoveToChoose() { return nextInfoForMoveToChoose;}
    static Location getNextLocation(int nextInfoForMoveToChoose);
    static int getNextChildIndex(int nextInfoForMoveToChoose);
    static int getNextInfo(Location nextMove, int nextIndex);

    void printStat(int numStat);
    void printStats(int numStat1, int numStat2);

    void nBestInsert(double v, Location move, int index);
    void nBestClear();
    void nBestSetSize(int newSize);
    void nBestChangeIndex(Location location, int index);
    Location nBestMove(int i) const;
    int nBestIndex(int i) const;
    int nBestCurrentSize() const;
    void nBestInit(int size);
    bool isBlacksTurn( ) const {
      return isBlackToPlay;
    }
    void addCaptureMove(Location location);
#ifdef KEEP_CAPTURE_MOVES_PER_NODE
    const Vector<Location> &getCaptureMoves() const {
      return captureMoves;
    }
#endif
  };

  class QuickGobanNodeMemoryManagement {
    static pthread_mutex_t saferMutex;
  public:
    /*  static bool init() {
        if (memory.size() != 2)
          memory.resize(2);
        int size=sizeof(int)*25000000;
        int *tmp=new int[size];
        for (int i=0;i<size;i+=sizeof(int)*START_SIZE_SMALL_VECTOR)
          memory[0].push(tmp+i);
        tmp=new int[size];
        for (int i=0;i<size;i+=sizeof(int)*START_SIZE_SMALL_VECTOR*STEP_SIZE_SMALL_VECTOR)
          memory[1].push(tmp+i);
        toInitQuickMemoryStack=true;
        return true;
      }*/
    static QuickMemoryStack<GobanNode *>memory; //static int nbAllocations10;
    //   static QuickMemoryStack<int> memoryDec; //static int nbAllocations10;
    //   static bool toInitQuickMemoryStack; //static int nbAllocations10;
    //     static int *memorySmall; //static int nbAllocations10;
    static GobanNode *quickMalloc(const GobanKey &key);
    inline static void free(GobanNode *p) {
#ifdef PARALLELIZED
      //       pthread_mutex_lock(&saferMutex);
#endif
      memory.push(p);
#ifdef PARALLELIZED
      //       pthread_mutex_unlock(&saferMutex);
#endif

    }
    /*    static void deleteHalfMemory() {
          for (int i=0;i<(int)memory.size();i++) {
            int s=(int)memory[i].size();
            for (int j=0;j<s/2;j++) {
              delete [] memory[i].top();
              memory[i].pop();
            }
          }
        }*/

  };

}

#endif

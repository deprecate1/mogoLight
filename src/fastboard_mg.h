//
// C++ Interface: fastboard_mg
//
// Description:
//
//
// Author: Sylvain Gelly <sylvain.gelly@lri.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MOGOFASTBOARD_MG_H
#define MOGOFASTBOARD_MG_H

#include "typedefs_mg.h"
#include "fastsmallvector_mg.h"
#include <queue>
#include <string.h>

#include "tools.h"

#define MAX_FAST_BOARD_SIZE (19)
#define MAX_FAST_BOARD_AREA ((MAX_FAST_BOARD_SIZE)*(MAX_FAST_BOARD_SIZE))
#define MAX_FAST_BOARD_TAB_SIZE ((MAX_FAST_BOARD_SIZE+2)*(MAX_FAST_BOARD_SIZE+2))

// Don't change those definitions!!!!!
#define FB_WHITE 0
#define FB_BLACK 1
#define FB_EMPTY 2
#define FB_EDGE 3
// Don't change those definitions!!!!!

#define FB_isPlayer(a) (((a)<FB_EMPTY))

#define swapFB(a, b) \
  do {\
    typeof(a) tmp_a = a;\
    typeof(b) tmp_b = b;\
    b = tmp_a;\
    a = tmp_b;\
  } while (0);


namespace MoGo {
  class Goban;
  /**
  New fast board implementation for MC simulations. This implementation is somehow inspired from some tricks and structures of libego library from Lukasz Lew
   
  @author Sylvain Gelly
  */


  class FBS_handler {
    friend class FastBoard;
    friend class GoExperiments;
    friend class MoveSelectorGlobal;
    friend class BasicInstinctFast;

  public:

    static int board_size;
    static int board_area;
    static int board_big_area;
    static int max_playout_length;
    static int dNS;
    static int dWE;
    static int mercyThresholdDivider;
    static int directions[8];

    static void initAll() {
      dNS = (FBS_handler::board_size + 2);
      directions[0]=-dWE;
      directions[1]=-dNS;
      directions[2]=+dWE;
      directions[3]=+dNS;
      directions[4]=-dNS+dWE;
      directions[5]=-dNS-dWE;
      directions[6]=+dNS-dWE;
      directions[7]=+dNS+dWE;
    }
    static void changeSize(int newSize) {
      assert(newSize<=MAX_FAST_BOARD_SIZE); assert(newSize>0);
      board_size        = newSize;

      board_area        = board_size * board_size;
      board_big_area        = (board_size+2) * (board_size+2);
      max_playout_length  = board_area * 2;
      mercy_threshold=board_area/mercyThresholdDivider;
      initAll();
    }

    static int row (int v) {
      return v / dNS - 1;
    }

    static int col (int v) {
      return v % dNS - 1;
    }

    static int toLocation(int r, int c) {
      return (r+1) * dNS + (c+1) * dWE;
    }
    static int upperLeft() {
      return dNS+dWE;
    }
    static bool isOnBoard(Location location) {
      if (location<upperLeft()) return false;
      if (location>=board_big_area) return false;
      int r=row(location);
      int c=col(location);
      return (r>=0) && (r<board_size) && (c>=0) && (c<board_size);
    }

      //JBH
    static bool isOnBorder(Location location) 
      {
	if (location<upperLeft()) return false;
	if (location>=board_big_area) return false;
	int r=row(location);
	int c=col(location);
	return (r==0) || (r==board_size-1) || (c==0) || (c==board_size-1);
      }

    static int getBigArea()
      {
	return board_big_area;
      }

    static bool getArea()
      {
	return board_area;
      } 

    //END JBH

    static int mercy_threshold;
  };













  class FastBoard {
    friend class GoExperiments;
    friend class InnerMCGoPlayer;
    friend class SimpleHeuristicScoreEvaluator;
    friend class BasicInstinctFast;

  public:
    FastBoard();

    ~FastBoard();
#ifdef GOLDENEYE_MC
    vector<int> semeaiW;
    vector<int> semeaiB;
    bool semeai;
#endif
    static void initStaticVariables();
    void clear();
    int getNeighborhood4(Location location) const {
      int v=0; int c=0;
      for (int i=0;i<4;i++, c+=2) {
        v+=(state[location+FBS_handler::directions[i]]<<c);
      }
      return v;
    }
    int getNeighborhood4Fast(Location location) const {
      return neighborhood[location]&0xFF;
    }
    int getNeighborhood8(Location location) const {
      int v=0; int c=0;
      for (int i=0;i<8;i++, c+=2) {
        v+=(state[location+FBS_handler::directions[i]]<<c);
      }
      return v;
    }
    int getNeighborhood8(int v, Location location) const {
      int c=4*2;
      for (int i=4;i<8;i++, c+=2) {
        v+=(state[location+FBS_handler::directions[i]]<<c);
      }
      return v;
    }
    int getNeighborhood8Fast(Location location) const {
      return neighborhood[location];
      /*      int v=neighborhood[location];
            int c=4*2;
            for (int i=4;i<8;i++, c+=2) {
              v+=(state[location+FBS_handler::directions[i]]<<c);
            }
            return v;*/
    }

    static int isOpponentEye4(int neighborhood4, int blackToPlay) {
      return (precomputedValues4[neighborhood4+(blackToPlay<<8)]>>3)&1;
    }
    int isOpponentEye4Slow(Location location) const {
      for (int i=0;i<4;i++) {
        int v=state[location+FBS_handler::directions[i]];
        if (v==FB_EMPTY || v==(isBlackTurn)) return false;
      }
      return true;
    }
    static int isOpponentEye4Slow(int neighborhood4, int isBlackTurn) {
      for (int i=0;i<4;i++) {
        int v=valueNeighborhood(neighborhood4, i);
        if (v==FB_EMPTY || v==(isBlackTurn)) return false;
      }
      return true;
    }
    static int getFreeNeighborhood(int neighborhood4) {
      return (precomputedValues4[neighborhood4])&7;
    }
    static int getFreeNeighborhoodSlow(int neighborhood4) {
      int nb=0;
      for (int i=0;i<4;i++) {
        int v=valueNeighborhood(neighborhood4, i);
        if (v==FB_EMPTY) nb++;
      }
      return nb;
    }
   static int getNeighborBlack(int neighborhood4){
     int nb=0;
     for (int i=0;i<4;i++) {
       int v=valueNeighborhood(neighborhood4, i);
       if ((v==FB_EMPTY)||(v==FB_BLACK)) nb++;
     }
     return nb;
   }
   static int getNeighborWhite(int neighborhood4){
     int nb=0;
     for (int i=0;i<4;i++) {
       int v=valueNeighborhood(neighborhood4, i);
       if ((v==FB_EMPTY)||(v==FB_WHITE)) nb++;
     }
     return nb;
   }
   
    /*    static int isEyeBlack4(int neighborhood4) {
            }
        static int isEyeWhite4(int neighborhood4) {
       }*/
    static int isEyeBlack4Slow(int neighborhood4) {
      for (int i=0;i<4;i++) {
        int v=valueNeighborhood(neighborhood4, i);
        if (v==FB_EMPTY || v==FB_WHITE) return false;
      }
      return true;
    }
    static int isEyeWhite4Slow(int neighborhood4) {
      for (int i=0;i<4;i++) {
        int v=valueNeighborhood(neighborhood4, i);
        if (v==FB_EMPTY || v==FB_BLACK) return false;
      }
      return true;
    }
    static int valueNeighborhood(int neighborhood, int i) {
      return (neighborhood>>(2*i)) & 3;
    }
    
    static int sameColor(int neighborhood4){
      int result=FB_EMPTY;
      for (int i=0;i<4;i++){
	  if ((valueNeighborhood(neighborhood4, i)!=FB_EMPTY)&&(valueNeighborhood(neighborhood4, i)!=FB_EDGE)){
	    if (result==FB_EMPTY) result=valueNeighborhood(neighborhood4, i);
	    else if (result!=valueNeighborhood(neighborhood4, i)) return 3;
	  }
      }
      return result;
    }//3 the neighbors belong to different colors;0 the neigbors belong to white; 1 the neighbors belong to black; 2 all are blank
    
    void addNeighborhood(int direction, Location dest) {
      direction+=2; if (direction>3) direction-=4;
      neighborhood[dest]+=(isBlackTurn-FB_EMPTY)<<(direction*2);
    }
    void removeNeighborhood(int direction, Location dest) {
      direction+=2; if (direction>3) direction-=4;
      neighborhood[dest]-=(!isBlackTurn-FB_EMPTY)<<(direction*2);
    }
    void addNeighborhoodCorner(int direction, Location dest) {
      direction+=2; if (direction>7) direction-=4;
      neighborhood[dest]+=(isBlackTurn-FB_EMPTY)<<(direction*2);
    }
    void removeNeighborhoodCorner(int direction, Location dest) {
      direction+=2; if (direction>7) direction-=4;
      neighborhood[dest]-=(!isBlackTurn-FB_EMPTY)<<(direction*2);
    }
    static int firstFreeDirectionSlow(int neighborhood4) {
      for (int i=0;i<4;i++) {
        int v=valueNeighborhood(neighborhood4, i);
        if (v==FB_EMPTY) return i;
      }
      return 4;
    }
    static int firstFreeDirection(int neighborhood4) {
      return (precomputedValues4[neighborhood4]>>4)&7;
    }



    void playPass();
    static void initAll();
    FastBoard &operator = (const FastBoard &fastBoard) {
      memcpy(this, &fastBoard, sizeof(*this));
      return *this;
    }
    void setKomi(double komi) {
      this->komi=int(komi);
    }
    void toGoban( Goban & goban) const;
    void print() const;
    void printLocation(Location location) const;
#ifdef KEEP_TRACK_OF_CAPTURES_IN_FAST_BOARD
    int hasBeenCaptured(Location location) const {
      return locationCaptured[location];
    }
#else
    int hasBeenCaptured(Location ) const {
      assert(0);
      return 0;
    }
#endif
#ifdef KEEP_CAPTURE_MOVES_PER_NODE
    int getOneCaptureOccured() const {
      return oneCaptureOccured;
    }
#endif

    static FastBoard *getBoardForThread();
    /** 0 for no one, -1 for white, 1 for black */
    int owner(Location location) const;
    bool isCaptureMove(Location location) const;
    int oneRollOut(int num, const Goban &goban, int choiceB, int choiceW, Vector<int> &allAtFirstData, FastSmallVector<int> &usedMovesAllAtFirst, double komi);
    int oneRollOut(int num, int choiceB, int choiceW, Vector<int> &allAtFirstData, FastSmallVector<int> &usedMovesAllAtFirst, double komi, int numThread);

    const GobanKey getGobanKey() const ;
    int isSelfAtari(const Location move, int numThread) const;
    int isInterestingByPattern(Location move) const;
    Location saveStringOnAtari(int numThread) const ;
    int interestingPatternValue(Location move) const;
    void addStringPositionsToFrontWithDistance( Location location, double refDistance, std::queue< Location > & fifo, double * distances ) const;
    void computeDistancesByGroup( Location startingPoint, double * distances ) const;
    Location lastMove() const {
      return lastPlayedMove;
    }
    Location koMove() const {
      return ko;
    }
    static void setFactorForSide(double _factorForSide) {
      factorForSide=_factorForSide;
    }

  private:
    int state[MAX_FAST_BOARD_TAB_SIZE];
    int neighborhood[MAX_FAST_BOARD_TAB_SIZE];
    int freeIndex[MAX_FAST_BOARD_TAB_SIZE];
    int chainNext[MAX_FAST_BOARD_TAB_SIZE];
    int chainId[MAX_FAST_BOARD_TAB_SIZE];


    int whiteEatenStoneNumber,blackEatenStoneNumber;

    int freeLocations[MAX_FAST_BOARD_AREA];
    int freeLocationsIndex;
    int freeLocationsLastIndex;

    int stonesPerPlayer[2];

    int komi;

    int lastPlayedMove;
    int lastMoveWasAtari;

    int chainTrueLib[MAX_FAST_BOARD_TAB_SIZE];
    int intersectionUpdated [MAX_FAST_BOARD_TAB_SIZE];
    int intersectionUpdatedIndex;
    int chainIdUpdatedIndex;
    int chainLength[MAX_FAST_BOARD_TAB_SIZE];

#ifdef KEEP_TRACK_OF_CAPTURES_IN_FAST_BOARD
    int locationCaptured[MAX_FAST_BOARD_TAB_SIZE];
    int captureIndex;
#endif

    int isBlackTurn;
    int ko;

    static int precomputedValues[65536*2];
    static int precomputedValues4[256*2];
    static int simplePatternDatabase[65536*2];
    static int vitalPointPatternDatabase[65536*2];
    static int precomputedValues_nb[256*2];
  
    int playNoPass(Location location);
    int playNoPassFaster(Location location);
    bool isSelfSuicideMove(int v) const;
    int playNoEye(int v);
    int playEye (int v);
    int playNoEyeFaster(int v);
    void processNewNeighborhood(Location newLocation, Location oldLocation);
    bool isOneLibertyOfChain(int v, int chain) const ;
    void removeChain (Location location);
    void placeStone (Location location);
    void removeStone (Location location);
    int approx_score () const;

    int score () const;
    int japanese_score () const;
    double score (double coefForSide) const;


    void removeChainNewNeighborhood(Location newLocation, Location oldLocation);
    void mergeChains (int baseChainLocation, int newChainLocation);



#ifdef KEEP_CAPTURE_MOVES_PER_NODE
    int oneCaptureOccured;
#endif


























    
    static FastBoard *fromGoban(const Goban &goban);
    static void fromGoban(const Goban &goban, FastBoard &board);

    static int oneRollOutThread(int num, int choiceB, int choiceW, Vector<int> &allAtFirstData, FastSmallVector<int> &usedMovesAllAtFirst, double komi);

    static void setEnvironnement(int argc, char** argv);

    bool saveLastMoveAtari(FastSmallVector<Location> &underAtariStringsFirstPosition, Location &savingMove, int numThread) const;
    bool isUnderAtariStringSavableByEatingMove(Location underAtariString, Location &savingMove) const;
    Location getOneLibertyOfString(Location location) const;
    bool isAtariSavingMoveTrue(const Location savingMove, int numThread) const;
    int isSelfAtariSmallerThan(const Location move, int thresholdSelfAtari, int numThread) const;
    int isSelfAtariSmallerThanFunction2(const Location move, int thresholdSelfAtari, int numThread) const;// Ajout Chris
    Location returnOtherLibertyWhenPlayingSelfAtari(Location location);// Ajout chris
    Location getOtherLiberty(Location firstLiberty, Location from, int nb_actuel);// Ajout Chris
    static double approachProbability;// Ajout Chris
    bool isOnGobanSide(const Location move) const;
    
    
    bool isConnectingMove(const Location move) const;
    int playOne(int num, const Goban &goban, int thresholdSelfAtari);
    int playOneOnGoban(int num, Goban &goban, int thresholdSelfAtari);
    int playOne(int num, int choiceB, int choiceW, int numThread);
    int playOne3(int choiceB, int choiceW, int numThread);
    int coreOfRandomMode44(int thresholdSelfAtari, int numThread);
    int coreOfRandomMode45(); //Ajout d'arpad
    int playCapture(); //Ajout JBH
    int nb_neighboard(Location location, Location from, int nb_actuel); //Ajout d'Arpad
    Location center3(Location location); //Ajout d'Arpad
    Location center4(Location location); //Ajout d'Arpad
    Location getVitalPoint(Location location); //Ajout d'Arpad
    bool isLastMoveAtari(FastSmallVector < Location > & underAtariStringsFirstPosition) const;
    int playOneInterestingMove(int numThread);
    int patternValue(Location move) const;
    bool isAtari(Location move) const;

    // TEMPORARY COMMENT: THIS FUNCTION IS USED
    inline int liberty( Location location ) const 
      {
	int strNum=chainId[location];
	return chainTrueLib[strNum];
      }

    static void precomputeSimplePatternDatabase();
    static void precomputeVitalPointPatternDatabase();
    int playOneAvoidBigSelfAtari (int thresholdSelfAtari, int numThread);

    static Vector<FastSmallVector<Location> >__adjacentFriendTwoLibertiesStrings;
    static Vector<FastSmallVector<Location> >__interestingMoves;
    static Vector<FastSmallVector<Location> >__underAtariStringsFirstPosition;

    bool areTwoStringsSharingTheSameTwoLiberties(int pos1, int pos2) const;
    bool playOnGoban(Location location);
    static void initiateReferenceBoard(const Goban &goban);
    static void initiateOwnBoardForThread();
    static void initiateOwnBoardForThread(const Goban &goban);
    static void initiateOwnBoardForThread(const Vector<Location> &moves);
    static bool playOwnBoardForThread(Location location);
    static Vector<FastBoard *> allBoards;
    static FastBoard *referenceBoard;

    bool isLocallyWeakerMoveForSimulation( Location location ) const;
    bool acceptThisMove(Location location) const;

    bool isMakeKo( const Location location ) const;
    void verifyNeighborhood() const ;
    static int playNearMode;
    static int avoidSuicide;
    static int stopPlayNearMode;
    static int startPlayNearMode;
    static int ignoreEndKo;
    static int finishBoard;
    static int initiateByFile;
    static int scoreMode;
    static int japanese_rules;
    static int nakade;
    static int fillboard;
    static double factorForSide;
    static double getFactorForCenter(double factorForSide) {
      double centerSize=FBS_handler::board_area-FBS_handler::board_size*4;
      return (double(FBS_handler::board_area)-factorForSide*double(FBS_handler::board_size*4))/centerSize;
    }
    
  public:
    
    //JBH
    
    int getLibertiesOfString(Location, list<Location>&, int nb_liberties = -1) const ;
    
    inline int position(Location place) const 
      {
	return state[place];
      }
    
    inline int isAtari2(Location place) const
      {
	return liberty(place)==1;
      }
    
    //get the number of neighbours having the color color_stone (connexity 4)
    inline int getNbNeighbours(Location location, int color_stone = FB_EMPTY) const
      {
	int nb = 0;
	
	for(int i = 0; i<4; i++)
	  {
	    Location neighbour = location+FBS_handler::directions[i];
	    
	    if(not (FBS_handler::isOnBoard(neighbour)))
	      continue;
	    
	    if(state[neighbour]==color_stone)
	      nb++;
	  }
	
	return nb;
      }
    
    inline bool isAlone4(Location location, int color_stone = FB_EMPTY) const
      {	
	for(int i = 0; i<4; i++)
	  {
	    Location neighbour = location+FBS_handler::directions[i];
	    
	    if(not (FBS_handler::isOnBoard(neighbour)))
	      continue;
	    
	    if(state[neighbour]==color_stone)
	      return false;
	  }
	
	return true;
      }
    
    inline bool isAlone8(Location location, int color_stone = FB_EMPTY) const
      {	
	for(int i = 0; i<8; i++)
	  {
	    Location neighbour = location+FBS_handler::directions[i];
	    
	    if(not (FBS_handler::isOnBoard(neighbour)))
	      continue;
	    
	    if(state[neighbour]==color_stone)
	      return false;
	  }
	
	return true;
      }
    
    inline bool isWithoutStone4(Location location) const
      {	
	for(int i = 0; i<4; i++)
	  {
	    Location neighbour = location+FBS_handler::directions[i];
	    
	    if(not (FBS_handler::isOnBoard(neighbour)))
	      continue;
	    
	    if(state[neighbour]!=FB_EMPTY)
	      return false;
	  }
	
	return true;
      }
    
    inline bool isWithoutStone8(Location location) const
      {	
	for(int i = 0; i<8; i++)
	  {
	    Location neighbour = location+FBS_handler::directions[i];
	    
	    if(not (FBS_handler::isOnBoard(neighbour)))
	      continue;
	    
	    if(state[neighbour]!=FB_EMPTY)
	      return false;
	  }
	
	return true;
      }
    
    
    inline bool isBlacksTurn() const
      {
	return isBlackTurn;
      }
    
    inline int getChainLength(int location) const
      {
	const int& numId = chainId[location];
        return chainLength[numId];
      }
    
    /**
     * @brief get the proportion of stones on the goban
     * @param color_stone the color of the stone
     * @return the density
     */
    inline double getStonesDensity(int color_stone = FB_EMPTY) const
      {
	switch(color_stone)
	  {
	  case FB_EMPTY : 
	    return (stonesPerPlayer[0]+stonesPerPlayer[1])
	      /(double)FBS_handler::board_area;
	  case FB_BLACK : 
	    return stonesPerPlayer[FB_BLACK]/(double)FBS_handler::board_area;
	  case FB_WHITE :
	    return stonesPerPlayer[FB_WHITE]/(double)FBS_handler::board_area;
	  default : ;
	  }
	
	assert(0);
	return 0;
      }
    
    /**
     * @brief know the number of stones around a location
     * @param location the considered intersection of the goban 
     * @param rayon the "middle size" of the selected window (width = 2*rayon+1)
     * @param color_stone the color of the stone
     * @param nb_locations the saveguard of the number locations which have visited.
     (could be different of (2*rayon+1)*(2*rayon+1), 
     for example, the case of the corner))
     * @return the number of counted stones
     */
    int getNumberStones(int, int = 1, int = FB_EMPTY, int* = 0) const;
    
    
    void print_fastboard(Location last_move = PASSMOVE) const; 
    
    //END JBH
    
    
  };

}

#endif

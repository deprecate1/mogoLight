//
// C++ Interface: goban_mg
//
// Description:
//
//
// Author: Yizao Wang <yizao.wang@polytechnique.edu> and Sylvain Gelly <gelly@lri.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MOGOGOBAN_MG_H
#define MOGOGOBAN_MG_H


#include <assert.h>
#include "typedefs_mg.h"
#include "fastsmallvector_mg.h"
#include "gobanurgency_mg.h"
#include "stringinformation_mg.h"
#include "locationinformation_mg.h"
#include "cfg_mg.h"
#include "smallpatternsdatabase_mg.h"

#include <queue>


#define GobanStateVector Vector

#define EatingLocationListType LocationList

namespace MoGo {

  /**
  This class saves the information of one goban.

  The random mode is in this class. it is related with StringInformation and LocationInformation, and CFG eventuelly.
   
  @author Yizao Wang
  */

  class CFG;
  class GobanNode;
  class InterestingMoveFinder;
  class GoGameTree;

  class GobanLocationLists {
    friend class Goban;
    friend class LocationInformation;
    friend class LocalPatternsDatabase;
    friend class AllLibEgoCode;
    friend class FastBoard;
  public:
    GobanLocationLists( int maxSize = 21 * 21 );
    ~GobanLocationLists();
    LongLocationList freeLocation;
    LocationList eyeLocation;
    LocationList blackEatingLocation;
    LocationList whiteEatingLocation;
    //     StringRelatedLocationList blackEatingLocation;
    //     StringRelatedLocationList whiteEatingLocation;
    EatingLocationListType blackSelfKillLocation;
    EatingLocationListType whiteSelfKillLocation;
    LongLocationList rangeLocation;
    LongLocationList openingRangeLocation;//FIXME OR
  private:
  };

  class Goban {
    friend class StringInformation;
    friend class LocationInformation;
    friend class GobanUrgency;
    friend class GobanLocationLists;
    friend class StupidCenterOpeningEvaluator;
    friend class MMScoreEvaluator;
    friend class MoveSelector1;
    friend class CFG;
    friend class CFGString;
    friend class CFGGroup;
    friend class GoExperiments;
    friend class GeneticProgramming;
    friend class AllLibEgoCode;
    friend class FastBoard;
  public:
//    mutable bool weAreTheBest;
    /** Constructor for a goban.*/

    Goban(int height = 9, int width = 9);
    /** Constructor of a goban by copying from another.*/
    //     Goban(const Goban &goban);
    Goban(GobanKey gobanKey);
    Goban(const Goban &goban);
    Goban &operator = (const Goban &goban);

    ~Goban();

    /** reloadByGobanKey keeps the rangeLocation and recreate the goban by gobanKey.
    The constructor of Goban(GobanKey gobanKey) is called(by default) when isOutsideStrange == false. */
    void reloadByGobanKey(const GobanKey & gobanKey, bool outrangeIsOutside = false);

    /** Test before play a move at the location*/
    bool isLegal(Location location) const ;
    bool isNotUseless(const Location location) const ;

    /** Play a move at the location, return a new goban. Maybe we don't have to give a new goban each time?*/
    Goban next(const Location location) const;
    /** Return true if it is the black turn */
    bool isBlacksTurn( ) const;
    /** The same function but return in int mode.*/
    StoneColor whoToPlay() const { return playerColor;}



    /** Only with legalmove list one can not yet lance the MC simulations. */
    Vector< Location > legalAndNoUselessMoves( ) const;
   

    /** Update the black and white as the number of eaten stones of each color. Very important when an evaluation of goban is needed.*/
    void eatenStoneNumber(int &black, int &white) const;
    /** Return the state of the location. black, white, empty...*/
    PositionState position(const Location location) const;
    /** Return the state of the location. black, white, empty... with outside checking*/
    PositionState positionWithChecking(const int i,const int j) const;
    /** (i,j) in the matrix way.*/
    PositionState position(const int i,const int j) const;

    /** Return the score of the goban. Score here refers to the end of a game.*/
    double score(bool inRangeOnly = false) const ;
    /** Return the score of the goban and modify the "positionScores" vector for each position by replacing values (if the stone is black or white at the end). Do only the "slowScore" (exact). Score here refers to the end of a game.*/
    double score(Vector<int> &positionScores) const ;
    /** Return the score of the goban and modify the "positionScores" vector for each position by adding value (if the stone is black or white at the end). Do only the "slowScore" (exact). Score here refers to the end of a game.*/
    double scoreAdding(Vector<int> &positionScores) const ;
    /** Return the score of the goban and modify the "eyeBlack, eyeWhite" vector for each position by adding value (if the position is an eye or not at the end). Do only the "slowScore" (exact). Score here refers to the end of a game.*/
    double scoreAddingEyeLocations(Vector<int> &eyeBlack, Vector<int> &eyeWhite) const ;
    /** Return the score of the goban and modify the "eyeBlackEdge, eyeWhiteEdge" vector for each position by adding value (if the position is close to an eye or not at the end). Do only the "slowScore" (exact). Score here refers to the end of a game.*/
    double scoreEyeLocationEdges(Vector<int> &eyeBlackEdge, Vector<int> &eyeWhiteEdge) const ;
    /** this is for a faster version. score only the stones in the range and the zone corresponds to it.*/
    double score(const Bitmap & zoneBitmap) const ;
   

    /** Return the score computed as kgs given the locationsOfDeadStrings */
    double scoreKGS(const Vector<Vector<Location> > &locationsOfDeadStrings) const;
    void iterateScoreKGS(Location location, bool isBlack, const Vector<bool> &isDead, Vector<bool> &alreadySeen, int &score) const ;
    /** As mentioned above, not every goban(even a goban of an end game) can't be scored.*/
    bool isScorable() const;

    /** Initiate the outside of goban.*/
    void initiateGoban();
    /** Clear Goban. Not change the outside positions.*/
    void clearGoban();

    /** Show the position in form of "(2A)", as shown beside the goban(like the matrix). Not the same format as GNUGO. */
    void textModeShowPosition(const Location location) const ;
    void textModeShowPosition( const int i, const int j ) const {textModeShowPosition(indexOnArray(i,j));}
    void textModeShowUrgency() const ;


    inline int width() const {return widthOfGoban;}
    inline int height() const { return heightOfGoban;};
    void textModeShowGoban(const Location location = 0, const bool showRangeOnly = false, const Bitmap * bitmap = 0, const bool showGroupNumber = false ) const ;
    /** Change the index(x,y) to Location.*/
    inline int indexOnArray(int x, int y) const {
      int v=(x+1)*(widthOfGoban+2)+(y+1);
      assert(v>=0);
      return v;
    }
    int xIndex(const Location location) const;
    int yIndex(const Location location) const;
    void getXYIndice(const Location & location, int & x, int & y) const;

   

    /** Resize the goban(with reinitiation)*/
    void resize(int width,int height);

    /** Play a move on this goban AND change it. This function does not give a new goban but do all the change on the current one.*/
    bool playMoveOnGoban(const Location location);
    bool playMoveOnGoban(const int i, const int j) ;



    /** locationInformation gives the information of the string that covers the given position(there must be one!). allLocationInformation searchs for all the location of the goban.*/

    void printLocationInformation(const Location location) const;
  

    /** stringInformation gives the information of the string with the given stringNumber. allStringInformation gives all.*/
    void printAllStringInformation() const;

    /** For MC simulation. Trick is used to accerlerate.*/
    Location koPosition() const;


    /** Get a GobanKey. bool gobanKeyUpdated is used to test if necessary to generateGobanKey().*/
    const GobanKey &getGobanKey() const ;
    GobanKey getGobanKeyForSuperKo() const;


    Location randomOutOneMove( const int mode = EATING_PREFERED_RANDOM_MODE_1) const;
    Location randomOutOneMove( const int mode, LocationList *&locationListIncludingThisLocation, int thresholdSelfAtari, bool andPlay=false) const;
    Location randomOutOneMoveAndPlay(const int mode = EATING_PREFERED_RANDOM_MODE_1, int thresholdSelfAtari=__thresholdSelfAtari);

    void printGobanKey() const ;

    void showFreeLocation( const GobanLocationLists & lists ) const;
    void showFreeLocation() const;
   
    int possibleMoveNumber() const;

    void showOneLibertyStringsInGTPMode() const;
    bool retrieveDeadStrings(int nbSimulations, double threshold, Vector<Vector<Location> > &locationsOfDeadStrings, double komi) const;
    bool retrieveDeadStrings(int randomMode, int nbSimulations, double threshold, Vector<double> &probabilityOfStringSurvival, Vector<Vector<Location> > &locationsOfDeadStrings, double komi) const;
   
 
    int approximateSizeOfPossibleMoves() const;

   
    inline int getStringLiberty(const Location location) const { return stringInformation.getStringLiberty(location);}
    inline int getStringLength(const Location location) const { return stringInformation.getStringLength(location); }
    inline int getStringNumber(const Location location) const { return stringInformation.getStringNumber(location); }

    void setSimulationRange(const Vector < Location > & rangeLocationList ) const;
    void setSimulationRange(const LocationList & rangeLocationList ) const;
    void setSimulationRangeGlobal() const;

    bool isInRangeLocation( const Location location ) const;

    bool isCut( const Location & location ) const;
    int isSelfAtari( const Location & location/*,int number = 5*/ ) const;
    bool isHane( const Location & location ) const;
    bool isSolid( const Location & location ) const;
    bool isOnGobanSide( const Location & location ) const;
    bool isOutsideOfGoban ( const Location & location ) const;
    bool isCrossCut( const Location & location ) const;
    bool isIsolatedMove( const Location & location ) const;
    bool isIsolatedSideMove( const Location & location ) const;
    bool isAtariSavingMove ( const Location & location ) const;
    bool isAtariSavingMoveTrue ( const Location & location ) const;

    bool isYoseOnSide( const Location & location, int side=-1 ) const;

    int nbAttach(Location location) const;
    int nbExtend(Location location) const;
    int nbDavidHane(Location location) const;
    int nbBlock(Location location) const;
    int nbConnect(Location location) const;
    int nbTurn(Location location) const;
    int nbTurnConnect(Location location) const;
    int nbDavidCrossCut(Location location) const;
    int blockCut(Location location) const;
    int nbDiagonal(Location location) const;
    int nbShoulderHit(Location location) const;
    int nbOnePointJump(Location location) const;
    int nbKnightMove(Location location) const;
    int nbEmptyTriangle(Location location) const;
    int nbDumpling(Location location) const;
    int nbPush(Location location) const;

    /** if yes, then returns the (first) location of under-atari string; otherwise 0. in order to save some speed.
    (Ahhhh, it was not what i thought... it's not here that I need this Location. but i keep these codes for possible further uses. */
    Location isAtari( const Location & location ) const;
    /** return 0 if this move is not an atari, and length of string if this move is an atari */
    int isAtariSize( const Location & location ) const;

    bool isInterestingMove ( const Location & location ) const;
    bool isInterestingMove44 ( const Location & location ) const;
    /** with certains randomModes, all eatingMoves are saved in the blackEatingLocation and whiteEatingLocation. In these cases, this method becomes not necessary. */
    bool isEatingMove ( const Location & location ) const;
    bool isEatingMoveNotLadder ( const Location & location ) const;
    /** return 0 if this move is not a capture move, and length of string if this move is a capture*/
    bool isEatingSize ( const Location & location ) const;

    /** if yes, return the move to save the atari string.(double atari is not considered.) otherwise 0.*/
    int isLastMoveAtari() const;
    bool isLastMoveAtari(Location & underAtariStringFirstPosition) const;
    bool isUnderAtariStringSavableByEatingMove(Location & underAtariStringFirstPosition, Location &savingMove) const;
    bool isUnderAtariStringSavableByEatingMoveFastMode(Location & underAtariStringFirstPosition, Location &savingMove) const;
    bool isLastMoveMenacing() const;
    bool isLastMoveMenacingMode2() const;
    bool isConnectingMove ( const Location & location ) const;
    int isConnectingTwoDifferentStringsMove ( const Location & location ) const;

    /** isStillEatingMove is called before each 'eating' move is chosen by randomMode. the check is necessary since the eating moves left in the list may be not one any longer after certain moves.*/
    bool isStillEatingMove( const Location location ) const;

    /** isStillEye returns false if location is no longer an eye. location must be in the eyeLocation.*/
    bool isStillEye( const Location location ) const;
    bool isMakeKo(const Location location) const;

 

    bool isNotGoodForOpening( const Location & location) const;
    GobanKey generateRotatedGobanKey() const ;
    GobanKey generateSymmetrizedGobanKey() const ;
    Location rotate(Location location) const ;
    Location symmetrize(Location location) const ;
    Location completeSymmetry(int numberOfSymetry, Location location) const ;
    /** here for the moment only at the v beginning of the game this number is correct. otherwise a counter should be added to the goban, which might, although very tinily, slow down the goban.*/
    int moveNumber() const;

    void putStoneOnGobanByForce( const Location location, const StoneColor stoneColor, bool replaceAlreadyStone = false );
    void recalculateGobanAfterForcedChangement();

    void showMoves(const Vector < Location > & moves) const;
    Location getLastPlayedMove() const {
      return lastPlayedMove;
    }

    void addLocationToRange( const Location location ) const;
    void deleteLocationFromRange ( const Location location ) const;
    int getBlackEatenStoneNumber() const;
    int getWhiteEatenStoneNumber() const;
    void setMaintainGobanKey(bool maintainGobanKey) {
      this->maintainGobanKey = maintainGobanKey;
    }
    static void setThresholdRandomSwitch(int t);
    static void setNotUselessMode(int notUselessMode);
    void setRecheckEyeMode(int mode);
    static int getRecheckEyeMode();
    void setOpeningRangeMode ( int openingRangeMode ) const ;
    const LongLocationList & openingRange() const;
    const Location& leftUpLocation() const;
    const Location& rightDownLocation() const;
    const Location & lastMove() const {return lastPlayedMove;}
    const Location & lastLastMove() const {return lastLastPlayedMove;}

    /** here the mode refers to the structure used inside Goban. StringInformation(= 0), CFG(= 1), or both(= 2). Change between mode 0 and mode 2 are always available, but not between 0 and the other 2 since StringInformation are not mutable. CFG is. */
    static void setCFGGlobal( int cfgMode ) { MoGo::Goban::cfgMode = cfgMode;}
    static int getCFGGlobal() { return MoGo::Goban::cfgMode;}
    void setCFG(bool) const;
    bool isCFGFunctionOn() const { return isCFGOn;}
    void setStringInformation(bool) const;



    void printCFGInformation() const;

    void checkCFGByStringInformation() const;

    const CFG & getCFG() const {assert(cfg); return *cfg;}

    Bitmap getBitmap() const;
    void getBitmap(Bitmap & bitmap) const;
    Bitmap changeLocationsToBitmap( const Vector < Location > & locations ) const;
    Bitmap addLocationsToBitmap( const Vector< Location > & locations, Bitmap & bitmap ) const;
    void getLocationsFromBitmap( Bitmap & bitmap, Vector < Location > & locations ) const;
   
    /** 
    change the bitmap as the rangeLocation. elsewhere OUTSIDE. the size of the rangeLocation is returned. a try interesting with bitmap.
     
    advantageForBlack: if locally there are more black stones than white, the difference we call advantageForBlack, then naturally we need this value in order to have a precise score. */

    int setBitmapAsSimulationRange( const Bitmap & bitmap, bool setOutside = true, double /*&*/advantageForBlack = 0 );
    /** bitmap1 is bigger than bitmap in order to protect the stones on the side of bitmap. in this case, these stones will never be eaten since their liberties are included but only in bitmap1 and thus will never be played. the result is probably much more precise than the precedent version. it's better to have CFG deal with this case since it has more informations than stringInformation.*/
    int setBitmapAsSimulationRange( const Bitmap & bitmap, const Bitmap & bitmap1, bool setOutside = true, double /*&*/advantageForBlack = 0 );
    /** this is a too simple version. it dilates once the bitmap to bitmap1, erases any stones if it meets in this operation, then calls setBitmapAsSimulationRange(bitmap,bitmap1)*/
    int setBitmapAsSimulationRangeViolentVersion( const Bitmap & bitmap, double /*&*/advantageForBlack = 0 );

    bool isTheSame( const Goban & goban ) const;



    /** updateLocationList is prepared for debug use. since the goban carried by GTPEngine is never touched by InnerMCGoPlayer. the latter does everything on copies of the playerGoban.*/
    void updateLocationList() const;

    const GobanState & getGobanState() const {return gobanState;}
    GobanLocationLists & getGobanLists() const { return gobanLists;}

    void initiateNodeUrgency( GobanNode * node, int mode, const Bitmap * bitmap = 0  ) const;

    bool isGobanOutsideStrange() const { return isOutsideStrange;}
    static void setThresholdSelfAtari(int _thresholdSelfAtari);
    static int getThresholdSelfAtari() { return __thresholdSelfAtari;};
    static int getThresholdRandomSwitch() { return thresholdRandomSwitch;};
    /** test ko. for debug use. before innerPlayMoveOnGoban returns(then after changePlayer).*/
    bool isKoCorrect() const;
    bool setHandicap( const int number );
    bool setHandicap( const int number, Vector < Location > &handicapStones);
    static bool showBigGobanStringNumber;
    bool isBitmapGoodForRangeSimulation(const Bitmap & bitmap, const Bitmap & rangeBitmap) const;
    static bool shishoLikeMoveForced;
    static int shishoReaderMode;


    /** shishoReader reads the shisho (ladder in English) before any 'thinking'. The shishoMove is the move (trying to save one string) which will be captured by shisho (i dont know exacly the explaination in English. The analysis starts when the lastMove is (shisho-like) atariMove. A copy of Goban is often necessary, then should only be used at the first depth.
    return 0 if no ladder at all, 1 if ladder can be escaped, 2 if ladder can't be espaced
    */
    int shishoReader(Location & shishoMove) const;
    int shishoReaderNew(Location oneStoneOnString, Location & shishoMove) const;
    /** shishoSimulator simulates only the shisho to see if the escaping string could be saved by
    - connecting with other 3-liberties string
    - eating enemy adjacent strings

    true is returned is it CANNOT escape.
    */
    bool shishoSimulator(Location lastLiberty, bool playerColorToPlay = true) const;
    /** test if there is new shisho move, and recheck all the shishoLocation saved before. */
    void shishoCheck(int shishoCheckMode, bool atRoot) const;

    static void setInterestingMoveFinder(InterestingMoveFinder *_finder) {
      finder1=_finder;
    }
    static void setInterestingMoveFinder2(InterestingMoveFinder *_finder2) {
      finder2=_finder2;
    }
    static int use25BlocksForGP;
    static bool useLibertiesForGP;
    static bool useLastLastMove;

    /** Returns the index on the array of the position (x,y) (0,0) is the position of the top left of goban. x is the horizental line number. y is the vertical line number.*/

    inline int direction(int numDirection) const {
      //assert(numDirection>=0);
      //assert(numDirection<8);
      return directionsArray[numDirection] ;
    }
    Vector<int> getLocationRepresentation(Location location, int patternSize) const;
    Vector<int> getLocationRepresentation2(Location location, int patternSize) const;
    Vector<int> getRepresentation3(int &numSymetry) const;
    void setMoveValues(const Vector<double> &moveValuesBlack, const Vector<double> &moveValuesWhite) const {
      this->moveValuesBlack=moveValuesBlack;
      this->moveValuesWhite=moveValuesWhite;
    }

    /** Returns the number of stones on the goban (slow) */
    int countNbStones() const;
    /** precompute the results of the patterns */
    static void precomputePatterns();
    /** Random 2015441 but return only the "non random part", not the uniform move. Instead of uniform returns PASSMOVE */
    Location randomOutNonRandomPartOf2015441(bool also2015445=false) const ;
    /** Return a save move if last move was atari and possible, and PASSMOVE else */
    Location saveStringOnAtari() const ;

    int distance(Location l1, Location l2) const;
    bool isNear8(Location l1, Location l2) const;
    FastSmallVector<Location> getSavingMovesForLastMoveAtari() const;

    /** Bad hack for the moment. Will be better used if it works. If it is still there it means that it did not work :) */
    static Vector<double> probabilityOfStringSurvival;
    static double powProbabilityOfStringSurvival;
    static double epsilonGreedy;

    int patternRepresentation(Location location) const;
    /** Bad hack for the moment. Will be better used if it works. If it is still there it means that it did not work :) */
    static Vector<Vector<int> >nbSeenPattern;
    static Vector<Vector<int> >nbSuccessPattern;
    static double successThresholdForInterestingMoves;
    static double successThresholdForNonInterestingMoves;
    static int retryForNonInterestingMoves;

    double successForLocation(Location location) const;
    void updateSuccessForLocation(Location location, int pattern, bool success) const;

    bool isLocallyWeakerMove(Location location) const;
    bool isLocallyWeakerMoveForSimulation(Location location) const;

    bool isPonnuki(Location location) const;
    int isInterestingMoveBy2015441(Location location, bool also2015445=false) const;
    int isInterestingMoveBy3015441(Location location, bool also2015445=false) const;
    int patternValueMatchedBy2015441(Location location) const;

    bool isSavingStoneMove(Location location) const;
    static int koUseMode;
    static int avoidSelfAtariMode;
    static int useGlobalPatterns;

    bool isStringOnSide(Location location) const;
    bool isOneNearFriendStringOnSide(Location location, int color) const;
    Location getLargestNearOpponentString(Location location, int color) const;
    bool isThisMoveWouldBeAGoodSacrifice(Location location, Location opponentStringLocation) const;
    Location randomOutOneMoveNearLastMove() const;
    FastSmallVector<Location> getAllMovesNearLastMove() const;
    const FastSmallVector < Location > &getErasedStones() const;

    static SmallPatternsDatabase smallDatabase;
    static int nonRandomCountW;
    static int nonRandomCountB;
    void initiateNodeUrgencyForShisho( GobanNode * node, GoGameTree *tree=0 ) const;
    void initiateNodeUrgencyForShishoNew( GobanNode * node, GoGameTree *tree=0 ) const;
    void addAllClosestLiberties(Location locationOnString, Vector<Location> &frontLocations, Bitmap &bitmap) const;

    /** Return a vector of size "number of strings of the goban", and each element is a location of the string. The location is >0 if black owns the string and <0 if white owns the string  */
    Vector<Location> getOneLocationPerString() const;

    double distanceToNearestString(Location location) const;

    double distanceByGroups(Location location, Location reference, int &nbFreeLocations) const;
    Vector<double> distanceByGroupsFromReference(Location reference) const;
    Vector<double> distanceByGroupsFromTwoReferences(Location reference1, Location reference2, double coefDistance) const;
    double distanceByGroupsToLastMove(Location location, int &nbFreeLocations) const;
    double distanceByGroupsTo2LastMoves(Location location, int &nbFreeLocations, double coefDistance) const;
    double distanceByGroupsToLastLastMove(Location location, int &nbFreeLocations) const;
    void addStringPositionsToFrontWithDistance(Location locationOnString, double refDistance, std::priority_queue<double> &fifo, Vector<double> &distances) const;
    static double makeUglyPriority(Location location, double distance);
    static double getUglyDistance(double priority);
    static Location getUglyLocation(double priority);

    /** Here is an attempt to measure the temperature of the position relative to the last move
    Returns a number in [0,1]:
    - 0 means nothing happens
    - 1 means something really important is happening, have to be careful
    The first version simply looks at the density of stones near the last played move.
    Slow method.
    */
    double temperatureOfLastMove() const;

    long long locationPattern5x5(const Location location) const;
    long long locationPattern5x5One(const Location location) const;
    long long locationPattern5x5(const Location location, Vector<long long> &allSymetries) const;
  private:
    int heightOfGoban ;
    int widthOfGoban ;
    /** Here the outgoban is considered in order to faciliate the programmation.*/
    GobanStateVector<int> gobanState;
    /** On each position, this vector contains the string number */

    /** For the moment the way of dealing with ko is not clear yet. Here save only the illegal ko move, although there may be several kos at the same time on the goban.*/
    /** The position ko-illegal for the current situation of goban*/
    int ko;
    //mutable isRealKo;
    /** The number of eaten stones of each color.*/
    int blackEatenStoneNumber;
    int whiteEatenStoneNumber;
    /** I am not sure whether to deal this as a bool is a good idea.*/
    StoneColor playerColor;
    bool is_Scorable;

    /** For the randomOutOneNotUselessMove */
    mutable GobanLocationLists gobanLists;
    //mutable LongLocationList freeLocation;

    /** Here the eyeLocation is defined by :
    one-point-space eye for one player, and
    NOT a self-kill move for that player.

    First, when a location is tested as an eye by uselessnessTest, it must not be a self-kill move.
    Second, when a string, who has such an eye, his liberty becomes to one, then this eye must be deleted from the eyeLocation list.

    Thus, obviously, whatever the color of the string, no one is allowed to play at any location of eyeLocation. We do not need to differ the color.
    */
    //mutable LocationList eyeLocation;

    /** Here for the self-kill move is the contrary. The self-kill move is not allowed to be played by one color, but not for the other. Be careful, the other player does not always has the interest to play that move, in fact to eat the enclosed opponent stones.

    If one string has a self-kill move, then for his color, the selfkilllocation is saved, and the string remember that location also. 
    For the string might have the chance later to be able to play on that move:

    either by having eaten some stones adjacent to him, 
    or by becoming able to eat some stones adjacent to that location;

    when this changement happens, the self-kill move is no longer forbidden, thus delete it from the list. BUT, sometimes several (two or three) strings may share one self-kill move(which connects them together and leaves 0 liberty), and the delete operation will leaves only a small inconvenience: the other strings keep always that self-kill move. However this case is OK, because the location remembered by each string is for the speed consideration, and that does not change the result of the move choice. Just remember to check every time before we want to delete one self-kill move from the list: since that one might be deleted already.

    Here we might have some extension since not only the self-kill move is interested in the simulation, but also those moves who make the string has but only one liberty left. But this case is not dealt yet. In fact I have not found a way clear to do. TODO FIXME
    */
    //mutable LocationList blackSelfKillLocation;
    //mutable LocationList whiteSelfKillLocation;
    //mutable StringRelatedLocationList eatingLocation;
    /** blackEatingLocation contains the location playing on which black can eat white stones. The same for the whiteEatingLocation. */
    //mutable StringRelatedLocationList blackEatingLocation;
    //mutable StringRelatedLocationList whiteEatingLocation;

    //mutable LocationList blackSelfAtariLocation;
    //mutable LocationList whiteSelfAtariLocation;

    /** informations on the strings */
    StringInformation stringInformation;
    /** Temporary informations on the location tested */
    mutable LocationInformation locationInformation;
    mutable CFG *cfg;
    static int cfgMode;
    //Bitmap gobanBitmap;
    //bool gobanBitmapUpToDate;

    /** Used for randomOutOneNotUselessMove

    Here, the location is the move right away played on goban, thus needed to be delete from the freeLocation list. Also need to deal with the erased stones.

    */
    void freeLocationUpdateAfterPlayMoveOnGoban(const Location location, LocationList *locationListIncludingThisLocation) ;
    void freeLocationUpdateAfterBasicTest(const Location, LocationList*& locationListIncludingThisLocation, int testResult) const ;
    void freeLocationUpdateAfterUselessnessTest(const Location, LocationList*& locationListIncludingThisLocation, int testResult) const ;
    /** Tell randomOutOneNotUselessMove whether to "RANDOM" from free location or test each first. (Some, maybe too many, free locations are useless. Then generate a possibleMove list first.*/

    mutable bool stopRandom;
    Vector<int> directionsArray;
    //      int directionsArray[8];
    /** create the directionsArray */
    void createDirectionsArray();

    /** gobanKey array, length: 5 for 9x9, 19 for 19x19.*/
    mutable GobanKey keyArray;

    void initiateGobanKey();

    /** After a move is played at location. Update all the below.*/
    void updateGobanKey(const Location location);
    /** After a stone is put on the location.*/
    void updatePositionInGobanKey(const Location location);
    /** After a stone is put on the location. (update "key" and not keyArray of Goban*/
    void updatePositionInGobanKey(const Location location, GobanKey &key, int playerColor) const;
    /** After the stone on the position is erased.*/
    void updateErasedPositionInGobanKey(const Location location);
    /** Update ko.*/
    void updateKoInGobanKey() const ;
    /** Update ko.*/
    void updateKoInGobanKey(GobanKey &key, int ko) const ;
    /** Update playerColor.*/
    void updatePlayerColorInGobanKey() const ;
    /** Update playerColor.*/
    void updatePlayerColorInGobanKey(GobanKey &key, int playerColor) const ;

    void generateGobanKey() const ;
    void updateFromGobanKey(const GobanKey gobanKey);

    mutable bool gobanKeyUpdated;

    //     Vector< unsigned int > threeExp;
    void initiateThreeExp();


    void playPassMoveOnGoban();
    void changePlayer();

    Location randomOutOneNotUselessMove( LocationList *& locationListIncludingThisLocation ) const ;
    Location randomOutOneNotUselessMoveSuicide( LocationList *& locationListIncludingThisLocation ) const ;
    Location randomOutOneNotUselessMoveEatingPrefered( LocationList *& locationListIncludingThisLocation ) const ;
    Location randomOutOneNotUselessMoveEatingPreferedAvoidingSelfAtari( LocationList *& locationListIncludingThisLocation, int thresholdSelfAtari ) const ;
    Location randomOutOneNotUselessAvoidingSelfAtari( LocationList *& locationListIncludingThisLocation, int thresholdSelfAtari ) const ;
    Location randomOutOneNotUselessAvoidingSelfAtariSuicide( LocationList *& locationListIncludingThisLocation, int thresholdSelfAtari ) const ;
    Location randomOutOneNotUselessAvoidingSelfAtari20154458( LocationList *& locationListIncludingThisLocation, int thresholdSelfAtari, bool andPlay ) const ;
    Location randomOutOneNotUselessAvoidingSelfAtariMovePreference( int movePreferenceMode, LocationList *& locationListIncludingThisLocation, int thresholdSelfAtari ) const ;
    Location randomOutOneNotUselessAvoidingSelfAtariBigEatsPrefered( LocationList *& locationListIncludingThisLocation, int thresholdSelfAtari ) const ;

   


    /** sub-function for randomOutOneMove. it randoms out one line and looks for any interesting moves(by patterns) in this line. if there is at least one it returns one randomly, otherwise it returns one by randomMode 1.*/
    Location randomOutOneInterestingMoveGlobal( LocationList *& locationListIncludingThisLocation ) const;

  

  
  



    /** Random 201544: mix between 10144 and 10544 */
    Location randomOutOneMoveMode201544(LocationList *& locationListIncludingThisLocation, int thresholdSelfAtari) const ;
   
    /** Random 2015441: 2015440 with some self atari allowance for interesting moves */
    Location randomOutOneMoveMode2015441(LocationList *& locationListIncludingThisLocation, int thresholdSelfAtari) const ;
   
    /** Random 2015445: 2015441 with big neighborhood */
    Location randomOutOneMoveMode2015445(LocationList *& locationListIncludingThisLocation, int thresholdSelfAtari) const ;
  
   
   
    /** Random 10000: random 2015445 + LD-fff */
    Location randomOutOneMoveMode10000(int thresholdSelfAtari) const ;

   

  
  

    void getInterestingMovesNearLastMoveWithProbas(int position1, int position2) const;
    bool randomOutOneMoveFromInterestingMovesWithProbas(Location &location) const;




    bool saveLastMoveAtari(Location &savingMove) const;
    int saveLastMoveAtariSize(Location &savingMove) const;
    void getInterestingMovesNearLastMove(int position1, int position2) const;
    void getInterestingMovesGlobal() const;
    void getInterestingMovesWithFinder1() const;
    void getInterestingMovesWithFinder2() const;
    void getInterestingMovesWithFinder(InterestingMoveFinder * finder) const;

    bool randomOutOneMoveFromInterestingMoves(Location &location) const;
    bool randomOutOneMoveFromInterestingMovesWithChecking(Location &location, FastSmallVector<Location> &interestingMoves) const;
    bool randomOutOneMoveFromInterestingMovesWithSelfAtariChecking(Location &location, FastSmallVector<Location> &interestingMoves) const;
    bool randomOutOneMoveFromInterestingMovesWithAllChecking(Location &location, FastSmallVector<Location> &interestingMoves) const;


    void getInterestingMovesNew(InterestingMoveFinder *finder, int newMode) const;
    void getInterestingMovesNew25(InterestingMoveFinder *finder, int newMode) const;
    bool isNewInterestingMoves(const Vector<int> &neighbors, int newMode) const;
    void getInterestingMovesNewWithLiberties(InterestingMoveFinder *finder, int newMode) const;
    void getInterestingMovesNew25WithLiberties(InterestingMoveFinder *finder, int newMode) const;
    /** if return a Location != PASSMOVE then this move has to be played before looking for interesting moves */
    Location coreOfRandomMode44(LocationList *& locationListIncludingThisLocation, int thresholdSelfAtari) const;
    /** if return a Location != PASSMOVE then this move has to be played before looking for interesting moves */
    Location coreOfRandomMode44Small(LocationList *& locationListIncludingThisLocation, int thresholdSelfAtari) const;
    /** if return a Location != PASSMOVE then this move has to be played before looking for interesting moves */
    Location coreOfRandomMode44_6(LocationList *& locationListIncludingThisLocation, int thresholdSelfAtari) const;
    /** if return a Location != PASSMOVE then this move has to be played before looking for interesting moves */
    Location coreOfRandomMode44_7(LocationList *& locationListIncludingThisLocation, int thresholdSelfAtari) const;
    /** sub-function for opening range */
    Location randomOutOneMoveProposedByOpeningRange() const;


    /** Usefull method for random 4. Not to be used alone. Return an eating move or PASSMOVE if none eating move is possible */
    Location randomOutMoveEatingOnly( LocationList *& locationListIncludingThisLocation ) const ;

    //Location randomOutOneMoveAccordingToUrgency() const;
    void updateEatingMove( const Location location , LocationList * locationListIncludingThisLocation) const ;
    void updateOneEatingMoveFound( const Location location ) const ;


    void locationListsClear() const;

  
    void reCheckEatingLocation( ) const;
    void reCheckEatingLocation( EatingLocationListType & eatingLocation, StoneColor toBeEatenStoneColor ) const ;

    

    LocationList* getLocationListIncludingThisLocation( const Location location ) const;
    LocationList* getLocationListIncludingThisLocationForMovePlay( const Location location ) const;


    void specialLocationUpdate( const Location location, LocationList *locationListIncludingThisLocation) const ;
    void specialLocationUpdateForOneLibertyString( const Location stringLocation ) const ;
    void stillSelfAtariUpdate() const ;

    /** Return the test result like LEGAL, SELFKILLMOVE, KO, ...*/
    int basicTest(const Location location, LocationList*& locationListIncludingThisLocation) const ;


    /** First trick. Necessary for lancing MC.*/
    int uselessnessTest(const Location location, LocationList*& locationListIncludingThisLocation) const;


    /** recheckEyeLocationsAfterPlayMoveOnGoban is called after each move is played. without this function, the eye locations are rechecked and found out of date only when there is at least one atari for one string sharing the eye location(by specialLocationUpdateForOneLibertyString). this function tests the four corner of the move just played, which is another case when one eye might be destroyed. if there is one eye found out of date, then returns true. */
    bool recheckEyeLocationsAfterPlayMoveOnGoban( const Location location ) const;



    bool innerPlayMoveOnGoban(const Location location,LocationList *locationListIncludingThisLocation);
    bool innerPlayMoveOnGobanCFG(const Location location,LocationList *locationListIncludingThisLocation);
    bool innerPlayMoveOnGobanMix(const Location location,LocationList *locationListIncludingThisLocation);
    int getSideNumber(const Location location);

    void selfKillLocationUpdate() const;

    Location lastPlayedMove;
    Location lastLastPlayedMove;
    Location lastLastLastPlayedMove;

    bool changedByForce;
    bool maintainGobanKey;
    static int thresholdRandomSwitch;
    static int __thresholdSelfAtari;
    static int __thresholdEatingMove;
    static int notUselessMode;

    mutable FastSmallVector < Location > interestingMoves;
    mutable FastSmallVector <double> interestingMovesProbabilities;
    mutable FastSmallVector < Location > atariSavingMoves;
    static Vector<double> probabilitiesOfinterestingMovesStatic;

    void openingRangeLocationInitiation( const int & sideDistance = 2, bool isEmptyGoban = false ) const;
    void openingRangeLocationUpdateAfterMove( const Location & location ) const;
    void openingRangeLocationShow() const;

    mutable int openingRangeMode;
    int moveNumber_;
    Location leftUpLocation_;
    Location rightDownLocation_;

    mutable bool isCFGOn;
    mutable bool isStringInformationOn;
    mutable bool isOutsideStrange;

    static int recheckEyeModeStatic;
    int recheckEyeMode;
    static int updateEatingLocations;
    static int slowScore;
    static bool useDistanceInGPPatterns;



    void initiateNodeUrgencyMode0( GobanNode * node ) const;
    void initiateNodeUrgencyMode1( GobanNode * node, const Bitmap * bitmap ) const;
    void initiateNodeUrgencyMode2( GobanNode * node, const Bitmap * bitmap ) const;
    void initiateNodeUrgencyMode3( GobanNode * node) const;
    void initiateNodeUrgencyMode4( GobanNode * node) const;
    /** Mode 1 using twice dilations of the bitmap of current goban. */
    void initiateNodeUrgencyMode5( GobanNode * node) const;
    /** Mode 5 - too slow moves. */
    void initiateNodeUrgencyMode6( GobanNode * node) const;
    void recheckStrings() const;


    /** A set of not interesting vectors. Not needed to be copied, just here as temporary (but are faster here than in temporary variable) */
    mutable FastSmallVector<Location> underAtariStringsFirstPosition;
    mutable FastSmallVector<Location> removedLocationFromGobanAvoidingSelfAtariFreeLocation;
    mutable FastSmallVector<Location> removedLocationFromGobanAvoidingSelfAtariEatingLocation;
    mutable FastSmallVector<Location> removedLocationFromGobanAvoidingSelfAtariSelfKillLocation;
    mutable FastSmallVector < Location > interestingMovesFurther;
    mutable FastSmallVector < Location > pastInterestingMoves;
    mutable Vector<double> moveValuesBlack;
    mutable Vector<double> moveValuesWhite;

    mutable LocationList shishoLocation;//TODO these lists moved into GobanLocationLists?
    mutable LocationList attackShishoLocation;//TODO these lists moved into GobanLocationLists?

    //void setRangeBorderUrgencyForShisho(GobanNode * node) const;
    static InterestingMoveFinder * finder1;
    static InterestingMoveFinder * finder2;
    static int random200Iterations;

    // The symetries array for 5x5
    Vector<Vector<int> > directionT25;
    void createRotationArray25();
    mutable Vector<int> smallestNeighborhood;
    static int randomModePrecomputed;
    static Vector<int> isInterestingPrecomputed;
    static Vector<int> isSelfAtariPrecomputed;
    static int moveChangedMode;
    void getAllCloseLiberties(Location location, Vector<bool> &strNumberVisited, FastSmallVector<int> &liberties) const;
    void addNonSeenLiberties(Location location, Location dontAddThisOne, FastSmallVector<int> &liberties) const;

    Location chooseThreatingMoveRandomly() const;
    Location koModeMove( LocationList *& locationListIncludingThisLocation, int thresholdSelfAtari ) const;
    mutable int koModeMoveStep;
    mutable int koModeMoveLocation;
    bool avoidThisSelfAtariInSimulations(Location move, int thresholdSelfAtari) const;
    static int eatingSizePrefered;
    int lengthOfEatString(Location move) const;
    /** As shishoReader, returns 0 if no ladder at all, 1 if ladder can be escaped (trivial test), 2 if it is not trivial
    */
    int getShishoLikeMoveNew(Location oneMoveOnString, Location & nextMove, Location & lastLiberty) const;

    static int raceForLibertiesThreshold;
    static int raceForLibertiesLengthThreshold;
    Location raceForLiberties(LocationList *&locationListIncludingThisLocation, int thresholdSelfAtari) const;
    int isLastMoveRaceFL(FastSmallVector<Location> &underAtariStringsFirstPosition) const;
    int isLastLastMoveRaceFL(FastSmallVector<Location> &underAtariStringsFirstPosition) const;
    int saveLastMoveRaceFL(Location &savingMove) const;
    int libertiesToWin(int stringColor, int strNumber, Location location) const;
    /** With redundancy */
    void addLibertiesOfString(Location oneStoneOnString, FastSmallVector<Location> &liberties) const;
    int oneStringCanBeAttacked(int stringColor, const FastSmallVector<Location> &locationOfStrings, int strNumber) const;
    bool isSimpleVitalPoint(Location location) const;
    Location getSimpleVitalPoint() const;
    void getAllAtariMoves() const;

    static int updateCloseLiberties;
    void addCloseLibertiesWhenPlaying(Location location);
    void removeCloseLibertiesByPlaying(Location location)const;
    void removeCloseLibertiesByPlayingIndex(int index)const;
    mutable FastSmallVector<Location> closeLiberties;

    bool isOneStoneNear4(Location location) const;
    static Vector<int> allPatternWeights;
    static int allowAttackInLadderWhenAdjacent;
    bool allowAttackInLadder(Location location) const;
    bool isLibertyOfFriend(Location location) const;
    bool changeLiberties(Location location, int &changeNbLiberties, int &changeNbStrings) const;
    int countNbLiberties(int &nbStrings) const;
  };

}


#endif

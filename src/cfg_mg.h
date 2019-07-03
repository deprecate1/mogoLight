//
// C++ Interface: cfg_mg
//
// Description:
//
//
// Author: Yizao Wang <yizao.wang@polytechnique.edu>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef MOGOCFG_MG_H
#define MOGOCFG_MG_H

#include"stringinformation_mg.h"
#include"locationinformation_mg.h"
#include"typedefs_mg.h"
#include"gotools_mg.h"

namespace MoGo {

  /**
  For the common fate graph. Yizao wrote most part of this file. This class is not optimized at all. In fact, only the adjacentStrNbs kept in the CFG is useful. Better to rewrite this part in the further development.
  
  The zone and block mode are very useful.
  
  GobanBitmap is a useful tool. GobanLocation is globally static for the reason of optimization. Some functions of GobanLocation are the same as in Goban. It is better to simplify Goban in the further work.
   
  @author Yizao Wang
  */
  class Goban;
  class GobanBitmap {
  public:
    GobanBitmap();
    ~GobanBitmap();
    static void initiate(const Goban & goban);
    static void clear(Bitmap & bitmap);
    static void fullfill(Bitmap & bitmap);
    static void setBitValue(Bitmap & bitmap, const int x, const int y, bool value = true);
    static void setBitValue(Bitmap & bitmap, const Location location, bool value = true);
    static void backupBitmap(const Bitmap & bitmap);
    static Bitmap logicalOROnLastBackupBitmap( const Bitmap &bitmap1 );
    static Bitmap logicalXOROnLastBackupBitmap( const Bitmap &bitmap1 );

    static void logicalOR( Bitmap &bitmap1, const Bitmap &bitmap2 );
    static void logicalOR( Bitmap & bitmap, const Vector < Bitmap > & bitmaps );
    static void logicalXOR( Bitmap &bitmap1, const Bitmap &bitmap2 );
    static void logicalAND( Bitmap &bitmap1, const Bitmap &bitmap2 );
    static int getBitValue(const Bitmap & bitmap, const int & x, const int & y);
    static int getBitValue(const Bitmap & bitmap, const Location & location);
    static void printBitmap(const Bitmap & bitmap);
    static void dilation( Bitmap & bitmap );
    static void showBitmap( const Bitmap & bitmap );
    static void showBitmap( const Vector < Bitmap > & bitmaps );
    static void showBitmap( const int bitLine );
    /** zoneBitmap are prepared for the random simulations. it is very important i believe to have the random range connected to the boardside. zoneBitmap and logicalOR deal with this work.*/
    static void initiateZoneBitmap();
    inline static const Bitmap & zoneBitmap( const int zoneNumber ) {
      return zoneBitmaps[zoneNumber];
    }

    inline static int getZoneBitmapNumber( const Location & location ) {
      return zoneNumber_[location];
    }
    inline static bool isCreated( const Bitmap & bitmap ) {return ((int)bitmap.size() == size);}
    static void showZoneBitmap();

    static bool isEqual( const Bitmap & bitmap1, const Bitmap & bitmap2);
    static void inversion( Bitmap & bitmap );
    static bool isEmpty( const Bitmap & bitmap );
    static int getSize(Bitmap & bitmap);
    static void getRectangle(const Bitmap & bitmap, int & topLine, int & bottomLine, int &leftColumn, int  &rightColumn);
    static void setRectangle(Bitmap & bitmap, int topLine, int bottomLine, int leftColumn, int  rightColumn);
    static Vector<int> removeIncludedBitmaps(Vector < Bitmap > & bitmaps, bool isRectangle=false);
    static bool isIncludedIn(const Bitmap & bitmap, const Bitmap & bitmap1, bool isRectangle=false);
    template<class T> static void removeFromVectorIndexes(Vector < T> & bitmaps, const Vector<int> &_toRemove) {
      Vector<int> toRemove=_toRemove;
      for (int i=0;i<(int)toRemove.size();i++) {
        bitmaps.erase(bitmaps.begin()+toRemove[i]);
        for (int j=i+1;j<(int)toRemove.size();j++)
          if (toRemove[j]>toRemove[i])
            toRemove[j]--;
          else if (toRemove[j]==toRemove[i]) {
            toRemove.erase(toRemove.begin()+j); j--;
          }
      }
    }
    static void changeBitmapToLocations(const Bitmap & bitmap, Vector < Location > & location);

  private:
    static int size;
    static int mask;
    //static Bitmap gobanBitmap;
    static Bitmap oneLocationMask_;
    static Bitmap tmpBitmap;
    static Bitmap backupGobanBitmap;
    static Vector < Bitmap > zoneBitmaps;
    static Vector < int > zoneNumber_;

  };


  class GobanLocation {
  public :
    GobanLocation();
    ~GobanLocation();
    inline static int xLocation(const Location & location) {return xLocation_[location];}
    inline static int yLocation(const Location & location) {return yLocation_[location];}
    inline static const Location & locationXY(const int& x, const int&y) {return locationXY_[x][y];}
    static void resize( const Goban & goban );
    static Location leftUpLocation;
    static Location rightDownLocation;
    static Vector<int> directionsArray;
    static void recreateDirectionsArray();
    static int height;
    static int width;
    static int size;
    static void textModeShowPosition(const Location location);
     static int getSideNumberFast(const Location location);
    static bool setHandicapStones( int number );
    static const Vector < Location > & getHandicapStones();

    static bool setHandicapStones( int number,  Vector< Location > &handicapStones_);
    static Location changeToLocation(const char * token);

  private:
    static Vector<int> xLocation_;
    static Vector<int> yLocation_;
    static Vector<Vector<Location> > locationXY_;
    static Vector< Location > handicapStones_;

  };


  class GobanLocationLists;
  class CFGString {
    friend class Goban;
    friend class CFG;
    friend class CFGGroup;
  public:
    CFGString();
    ~CFGString();
    void clear();
    void initiate(int size);
    void resetStringPreview( );
    void printInformation( ) const;
    void playMoveOnGoban( Goban * goban, const Location & location );
    void showOneLibertyStringsInGTPMode( const Goban * goban ) const;
    void addStringToBitmap(const StringNumber strNb, Bitmap & bitmap) const;
    void addStringLibertiesToBitmap(const StringNumber strNb, Bitmap & bitmap) const;
    void calculateTerritory(const Goban * goban) const;
    void showTerritory(const Goban * goban) const;
    void showTerritoryInformation(const Goban * goban) const;
  private:
    FastSmallVector< StringNumber > stringNumber;
    /** here, the adjacentStrings saves the stringNumber. LocationList is kind of misleading. In fact, what we have here are all enemy strings. */
    FastSmallVector< LocationList > adjacentStrNbs;
    /** Location type is saved. */
    FastSmallVector< LocationList > liberties;
//     Vector< Vector <Location> > stones;
     FastSmallVector< FastSmallVector<Location> > stones;

    int maxNumberOfString;
    mutable Location lastStringPreviewLocation;
    mutable bool lastStringPreviewResult;

    mutable LocationList tmpAdjacentFriendStrNbs;
    mutable LocationList tmpAdjacentEnemyStrNbs;
    mutable FastSmallVector< Location > tmpAdjacentFreeLocations;
    mutable FastSmallVector< Location > tmpErasedStones;
    /** when these two bools are false, that means the testLocation MIGHT be an eyeFillMove or a ko. It is clear when they are true.*/
    mutable bool testLocationIsNotEyeFillMove;
    mutable bool testLocationIsNotKo;

    mutable Location testKo;


    mutable int maxTerritoryNumber;
    mutable Vector < int > territoryNumber;
    mutable Vector < LocationList > territoryLocations;
    mutable Vector < LocationList > territoryAdjacentStrNbs;
    mutable Vector < StoneColor > territoryOwnerColor;

    void updateStringNumber( Goban *goban, const Location & location, const LocationList * adjacentFriendStrNbs, const LocationList * adjacentEnemyStrNbs,const FastSmallVector< Location > & emptyLocations );
    StringNumber getBiggestLibertiesStrNumber( const Location & , const LocationList * adjacentFriendStrNbs);
    void updateErasingString( Goban * goban, const StringNumber & strNb );

    void calculateAllFromGobanState( const Goban * goban, Vector < Location > & gobanState );
    void calculateOneFromGobanState(const Goban * goban, const Location location);
    void calculateAdjacentStringNumbers(Vector < Location > &gobanState, Vector < int > &directionsArray);

    void iterateCalculateOneFromGobanState(const Goban * goban, const Location location,const StoneColor stringColor, const Vector < Location > &gobanState );

    int basicTest( const Goban * goban, const Location location ) const ;
    int uselessnessTest(const Goban * goban, const Location location) const;
    bool stringPreview(const Location location, const Vector<Location> &gobanState, const Vector<int> &directionsArray, const int &playerColor) const ;
    void stringPreviewClear() const;

    void freeLocationUpdateAfterPlayMoveOnGoban( Goban * goban, const Location location, GobanLocationLists &gobanLists, LocationList *locationListIncludingThisLocation ) const;
    void selfKillLocationUpdate( Goban & , const Vector < Location > &gobanState, GobanLocationLists &gobanLists, const StoneColor & playerColor, Vector < int > &directionsArray ) const;
    void specialLocationUpdate( const Goban * goban, const Location location, const Vector < Location > &, GobanLocationLists &gobanLists ) const;
    void specialLocationUpdateForOneLibertyString( const Goban * goban, const StringNumber strNb, const StoneColor stringColor, GobanLocationLists &gobanLists ) const;
    void updateEatingMove( const Goban * goban, const Location location, GobanLocationLists &gobanLists) const;

    void calculateOneTerritory(const Goban * goban, const Location location) const;
    void iterateOneTerritory(const Goban * goban, const Location location, const int territoryNumber) const;

  };

  class CFGGroup {
    friend class Goban;
    friend class CFG;
    friend class CFGString;
  public:
    CFGGroup(int distance1 = 2, int distance2 = 1);
    ~CFGGroup();
    void initiate(int size);
    void clear();
    /** the distances here defined are different from the ones in getBlock of CFG. distance1 is the biggest number of free locations connecting two same color strings in one group, while distance2 is the biggest number of free locations connecting one string in group and one enemy string as the group's adjacentEnemyStrNbs.*/
    static void setGroupDistance(int distance1, int distance2);
    void printInformation() const;
  private:
    /** here, the adjacentGrNbs saves the GroupNumber. LocationList is kind of misleading. In fact, what we have here are all enemy groups. Probably useless since the update of group number could hardly be simultaneous. probably useless since we may have this information from adjacentEnemyStrNbs.*/
    //Vector< LocationList > adjacentGrNbs;

    Vector< LocationList > adjacentEnemyStrNbs;

    /** Location type is saved. */
    Vector< LocationList > liberties;
    Vector< LocationList > strNbs;
    Vector < Location > groupNumber;
    int maxNumberOfGroup;
    static int distance1;
    static int distance2;
    bool isUpToDate;

    /** here for the calculate of the group, only two simple functions are given, as the simultaneous update is left for the optimization in future.*/
    void calculateAllGroupsFromCFGString( const Vector < Location > & gobanState, const CFGString & stringCFG );
    void calculateOneGroupFromCFGString( const GobanState &gobanState, const CFGString & stringCFG, const StringNumber strNb );
    void addStringToGroup(const CFGString & stringCFG, const StringNumber strNb, const GroupNumber grNb);
    void addGroupStringsToBitmap(const CFGString & stringCFG, const GroupNumber grNb, Bitmap & bitmap) const;

  };


  class CFGFreeLocation {
    friend class CFG;
  public:
    CFGFreeLocation();
    ~CFGFreeLocation();
  private:
    /** again the stringNumber is saved here instead of location. */
    Vector < LocationList > adjacentBlackStrNbs;
    Vector < LocationList > adjacentWhiteStrNbs;
    /** Location type is saved. */
    Vector < LocationList > adjacentFreeLocations;
  };

  class CFG {
    friend class CFGString;
    friend class CFGFreeLocation;
    friend class Goban;
    friend class CFGGroup;
  public:
    CFG(Goban * goban);
    CFG();
    ~CFG();
    void initiate();
    void clear();
    StringNumber getStringNumber(const Location & location) const;
    StringNumber getMaxStringNumber() const;
    GroupNumber getGroupNumber(const Location & location) const;
    int getStringLength(const StringNumber & stringNumber) const;
    int getStringLiberty(const StringNumber & stringNumber) const;
    void printCFGInformation() const;
    void getRelatedRange( const Location & location, Vector < Location > & CFGRelatedRange ) const ;
    void textModeShowGoban( const Location location = PASSMOVE, bool showGroupNumber = true ) const;
    void showOneLibertyStringsInGTPMode( ) const;


  

    void addOneMoveToBlock( const Location & location, const int & distance1, const int & distance2, Bitmap & bitmap, LocationList *blockStrNbs, LocationList *blockFreeLocations) const;


    /** here the distance1 is not the one in the CFGGroup. it defines the distance of the freeLocations to the stones in the group block. the block is got as following:

    1. add the group including location to the block.
    2. add to the block all the groups adjacent to at least one group in the block
    3. count the freeLocations size by distance1
    4. if freeLocationSize is smaller than zoneFreeLocationSize and there is new group added, then redo 2.
    5. add freeLocations to the bitmap

    the freeLocationSize is returned.
    */
    int getGroupBlock( const Location & location, const int &distance1, const int &, Bitmap & bitmap, int zoneFreeLocationSize) const;

  

  
  
   
 
    /** territory bitmap, different from the zone territory, concentrates on the 'territory', in other words, on the free locations. getLocalTerritoryBitmap calculates one bitmap for the location territory including the location given(maybe not free). distance is somehow the size of the bitmap*/
    void getLocalTerritoryBitmap(const Location location, Bitmap & bitmap, int distance) const;
    /** getGlobalTerritoryBitmap gives several bitmaps for all the territories on the goban. these bitmaps has no commun intersection between each two. */
    void getGlobalTerritoryBitmap(Vector < Bitmap > & bitmaps, int distance) const;


    void updateErasingString( const Goban * goban, const StringNumber & strNb );

    const FastSmallVector < Location >& tmpErasedStones() const {return stringCFG.tmpErasedStones;}
    const LocationList &tmpAdjacentFriendStrNbs() const {return stringCFG.tmpAdjacentFriendStrNbs;}

    void setGroupDistance(int distance1, int distance2) const;
    bool isGroupUpToDate() const {return groupCFG.isUpToDate;}

    void groupUpdateAll() const;
    void setGroupUpToDateMode(bool mode) {groupUpToDateMode = mode;}

    /** territory is defined by string of "free locations". should be called after stringCFG is up to date.*/

    void calculateTerritory(const Goban * goban) const;
    void showTerritory(const Goban * goban) const;
    void showTerritoryInformation(const Goban * goban) const;

  private:
    CFGString stringCFG;
    //CFGFreeLocation freeLocationCFG;
    Goban *goban;
    mutable CFGGroup groupCFG;

    void playMoveOnGoban( const Location & location );

    void resetStringPreview( );
    void stringPreviewClear() const;
    //mutable LocationList tmpAdjacentFriendStrNbs;
    //mutable LocationList tmpAdjacentEnemyStrNbs;
    //mutable Vector< Location > tmpAdjacentFreeLocations;
    //mutable Vector< Location > tmpErasedStones;




    /** now for the blocks. see getBlock for the definition. */

    void addOneMoveToBlock( const Location & location, const int &x, const int &y, const int & distance1, const int & distance2, Bitmap & bitmap, LocationList * blockStrNbs, LocationList * blockFreeLocations ) const;
    void blockUpdateAddAdjacentStrNbs( LocationList & blockStrNbs, const StringNumber strNb, Bitmap & bitmap, bool bitmapUpdate ) const;
    void blockUpdateAddAdjacentStrNbsForAddingOneMoveCase( LocationList &blockStrNbs, const StringNumber strNb, Bitmap &bitmap, Bitmap &tmpBitmap, bool bitmapUpdate ) const;
    void blockUpdateAddAdjacentStrNbsForAddingOneMoveCase( LocationList &blockStrNbs, const StringNumber strNb, Bitmap &bitmap, Bitmap &tmpBitmap0, Bitmap &tmpBitmap, bool bitmapUpdate ) const;

    void blockUpdateAddLiberties( LocationList & blockFreeLocations, const StringNumber strNb, Bitmap & bitmap, bool bitmapUpdate ) const;
    void getNewLocationsByDilation(Bitmap &bitmap1, const Bitmap &bitmap2, int &dilationTimes, Vector < Location > newLocations);


    int basicTest( const Location location ) const ;
    int uselessnessTest( const Location location) const;
    bool stringPreview(const Location location, const Vector<Location> &gobanState, const Vector<int> &directionsArray, const int &playerColor) const ;


    void freeLocationUpdateAfterPlayMoveOnGoban( const Location location, GobanLocationLists &gobanLists, LocationList *locationListIncludingThisLocation ) const;
    void selfKillLocationUpdate( Goban & , const Vector < Location > &gobanState, GobanLocationLists &gobanLists, const StoneColor & playerColor, Vector < int > &directionsArray ) const;
    void specialLocationUpdate( const Location location, const Vector < Location > &, GobanLocationLists &gobanLists ) const;
    void specialLocationUpdateForOneLibertyString( const StringNumber strNb, const StoneColor stringColor, GobanLocationLists &gobanLists ) const;
    void updateEatingMove( const Location location, GobanLocationLists &gobanLists) const;


    void calculateAllFromGobanState( Vector < Location > & gobanState );

    Location testKo() const {return stringCFG.testKo;}

    /** not implemented yet. it seems complicated. */
    bool groupUpToDateMode;

    bool isStringDependentOfTerritory(const StringNumber & strNb, Bitmap & bitmap, int distanceLeft) const;
    bool isExtendableBorder(const Bitmap & bitmap, const Bitmap & newBorder, int x, int y, int distanceLeft) const;
    bool isExtendableBorder(const Bitmap & bitmap, const Bitmap & newBorder, const Location location, int distanceLeft) const;

    void rangeBorderModifier(Bitmap & rangeBorderBitmap) const;

    bool isStringInterestingForBlockMode3(const StringNumber strNb) const;
    /** if string strNb has no adjacentStrNbs with different color, then isStringIsolated return true.*/
    bool isStringIsolated(const StringNumber strNb) const;


  };

}

#endif

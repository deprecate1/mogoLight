//
// C++ Interface: locationinformation_mg
//
// Description:
//
//
// Author: Yizao Wang <yizao.wang@polytechnique.edu> and Sylvain Gelly <gelly@lri.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MOGOLOCATIONINFORMATION_MG_H
#define MOGOLOCATIONINFORMATION_MG_H
#include "typedefs_mg.h"
#include "fastsmallvector_mg.h"
#include "stringinformation_mg.h"

namespace MoGo {
  class Goban;
  class StringInformation;

  /**
  Keep the temporary informations usefull to make some computation locally on a location

  The patterns are also in the class. the functions begninng with "is" are mostly for this use.
   
  @author Yizao Wang, Sylvain Gelly
  */
  class LocationInformation {
    friend class Goban;
    friend class GobanLocationLists;
    friend class StringInformation;
    friend class StupidCenterOpeningEvaluator;
  public:
    // static bool initLocationInformationstringPreviewFunction();
    static void setIsSelfAtariMode(int mode);
  private:
    LocationInformation(Goban *goban);
    LocationInformation();
    /** Useful in stringPreview.*/
    FastSmallVector< Location > friendAdjacentStringLocation;
    FastSmallVector< Location > enemyAdjacentStringLocation;
    FastSmallVector< Location > deadAdjacentStringLocation;
    FastSmallVector< Location > freeAdjacentLocation;

    /** Return the test result like LEGAL, SELFKILLMOVE, KO, ...*/
    int basicTest(const Location location);
    void showGobanOnBug19x19(const Location location, const Vector<Location> &gobanState, const Vector<int> &directionsArray, const int &playerColor, int tmp);
    /** The core of the basicTest (and then playMoveOnGoban).*/
    inline bool stringPreview(const Location location, const Vector<Location> &gobanState, const Vector<int> &directionsArray, const int &playerColor) {
      if (location == lastStringPreviewLocation)
        return lastStringPreviewResult;
      lastStringPreviewLocation=location;
      int liberty1=0,liberty2=0,liberty3=0;//liberty_i: number of friend string having i liberties
      clear();
      //allLocationInformation();
      for (int i=0;i<4;i++) {
        Location tmp=location+directionsArray[i];
        //     Location tmp=locationAndDirection[(location<<2)+i];
        //      Location tmp=location+locationAndDirection[i];
        if (gobanState[tmp]==EMPTY) // empty
          freeAdjacentLocation.push_back(tmp);
        else if (gobanState[tmp]==playerColor) // friend
        {
          friendAdjacentStringLocation.push_back(tmp);
          //       stringLiberty[stringNumber[LocationDirectionI]]
          if (stringInformation->getStringLiberty(tmp)>2)//Very tricky~~
            liberty3++;
          else if (stringInformation->getStringLiberty(tmp)==2)
            liberty2++;
          else liberty1++;

        } else if (gobanState[tmp]==3-playerColor) // enemy
        {
          enemyAdjacentStringLocation.push_back(tmp);
          if(stringInformation->getStringLiberty(tmp)==1)
            deadAdjacentStringLocation.push_back(tmp);
        } else if (gobanState[tmp]==OUTSIDE) // outside
          continue;
        else //error for gobanState
        {
          showGobanOnBug19x19(location, gobanState, directionsArray, playerColor, tmp);
          assert(0);
          return false;
        }
      }
      if (deadAdjacentStringLocation.size()==0 && liberty2==0 && liberty3==0 && freeAdjacentLocation.size()==0) {
        lastStringPreviewResult=false;
        return false;//self-killing move, illegal!
      }


      /*if (selfAtariTest(location,liberty1,liberty2,liberty3))//here, no chance for location being a selfkillmove.
        isLocationSelfAtari=true;
      else
        isLocationSelfAtari=false;
        */

      lastStringPreviewResult=true;
      return true;
    }
    /** Test ko is used when test(for ko) is executed. Useful when this test doens't make a copy of goban(otherwise one can use always ko).*/
    int testKo;
    void resetStringPreview();
    /** First trick. Necessary for lancing MC.*/
    int uselessnessTest(const Location location) const;
    bool selfAtariTest( const Location location,const int liberty1, const int liberty2, const int liberty3 ) const ;

    inline void clear() {
      friendAdjacentStringLocation.clear();
      enemyAdjacentStringLocation.clear();
      deadAdjacentStringLocation.clear();
      freeAdjacentLocation.clear();
    }

    Goban *goban;
    StringInformation *stringInformation;
    Location lastStringPreviewLocation;
    bool lastStringPreviewResult;
    mutable bool isLocationSelfAtari;

    double simpleSmallPattern(const Location & location) const;

    /** Several booleans giving some knowledge to the program. all these functions dont demand the call of stringPreview. */

    /**normally these two functions should be called before.*/
    bool isTaken( const Location & location ) const;
    bool isOnGobanSide( const Location & location ) const;
    bool isOutsideOfGoban( const Location & location ) const;
    /** cut, crosscut, hane, are moves locally prefered. */
    bool isCut( const Location & location/*, double & blackInterest, double & whiteInterest*/ ) const;
    bool isHane( const Location & location/*, double blackInterest, double whiteInterest*/ ) const;
    bool isCrossCut ( const Location & location/*, double & blackInterest, double & whiteInterest */) const;
    bool isSolid( const Location & location ) const;
    /** ugly move is strongly detested, especially at the opening period.*/
    bool isUgly( const Location & location/*, double & blackInterest, double & whiteInterest*/ ) const;
    /** if no stones around then isolated. */
    bool isIsolatedMove ( const Location & location ) const;
    /** when the goban is big, the the side situation is delicate.*/
    bool isIsolatedSideMove ( const Location & location ) const;
    /** as its name says, this is for the opening pruning.
    moves whose 8 neighbors have at least one friend stone and no enemy stone.
    Attention: don't pass moves near the board (especially the corner!) here. They should probably be always allowed most of the time.*/
    bool isTooSlow( const Location & location/*, double & blackInterest, double & whiteInterest */) const;
    /** a simple self-atari test, no need to call stringpreview. */
    int isSelfAtari( const Location & location/*, int number = 5*//*, double & blackInterest, double & whiteInterest */) const;


    /** useless if location is still a self-atari move */
    bool isAtariSavingMove ( const Location & location ) const;
    bool isAtariSavingMoveTrue ( const Location & location ) const;
    Location isAtari( const Location & location ) const;

    bool isNotGoodForOpening( const Location & location/*, double & blackInterest, double & whiteInterest*/) const;
    bool isNotGoodForOpening9( const Location & location/*, double & blackInterest, double & whiteInterest*/) const;
    bool isNotGoodForOpening13( const Location & location/*, double & blackInterest, double & whiteInterest*/) const;

    bool isInterestingMove( const Location & location/*, double & blackInterest, double & whiteInterest*/) const;

    bool isEatingMove( const Location & location ) const;
    /** if yes, return the last liberty of the under-atari string.(double atari is not considered.) otherwise 0.*/
    Location isLastMoveAtari( ) const;

    static int isSelfAtariMode;

    /** this function gives the number of the string under-atari by last played move. the saving move is not calculated immediately since one might save it by eating some stones. */
    bool isLastMoveAtari(Location & underAtariStringFirstPosition) const;
    /** for the case where several strings are under atari. */
    bool isLastMoveAtari(FastSmallVector < Location > & underAtariStringsFirstPosition) const;
    bool isUnderAtariStringSavableByEatingMove(Location & underAtariStringFirstPosition, Location &savingMove) const;
    int isUnderBeforeAtariStringSavableByEatingMove(Location & underAtariStringFirstPosition, Location &savingMove) const;
    bool isUnderAtariStringSavableByEatingMoveFastMode( Location & underAtariStringFirstPosition, Location & savingMove ) const ;

    /** is there are any friend stones near the last played enemy stone.*/
    bool isLastMoveMenacing() const;
    /** is there are any stones near the last played enemy stone.*/
    bool isLastMoveMenacingMode2() const;

    bool isConnectingMove( const Location & location ) const;
    int isConnectingTwoDifferentStringsMove( const Location & location ) const;

    bool isStillEye( const Location location ) const;
    /** patterns on the goban side. side: 0 = N, 1 = E, 2 = S, 3 = W*/
    // N
    //W E
    // S
    bool isYoseOnSide( const Location & location, int side=-1 ) const;



    int directionT[8][9];
    int rotation[4];
    //     Vector<Vector<int> >directionT;
    //     Vector<int> rotation;
    void createRotationArray() ;
    void showStringPreviewResult() const;

    bool getUnderAtariStringNumber(const Location & location, const StoneColor underAtariStringColor, StringNumber & strNb) const;

    /** temporary variable */
    mutable FastSmallVector<StringNumber> adjacentFriendTwoLibertiesStrings;
    /** by last move.*/
    bool getShishoLikeMove(Location & nextMove,Location & lastLiberty) const;
    bool isAtariNear( Location location, FastSmallVector<Location> & underAtariStringsFirstPosition ) const;
    bool isBeforeAtariNear( Location location, FastSmallVector<Location> & underAtariStringsFirstPosition ) const;

  };

}

#endif

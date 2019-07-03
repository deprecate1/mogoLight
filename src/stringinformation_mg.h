//
// C++ Interface: stringinformation_mg
//
// Description:
//
//
// Author: Yizao Wang <yizao.wang@polytechnique.edu> and Sylvain Gelly <gelly@lri.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MOGOSTRINGINFORMATION_MG_H
#define MOGOSTRINGINFORMATION_MG_H

#include "typedefs_mg.h"
#include "fastsmallvector_mg.h"

#define TYPE_OF_VECTOR_STRING_LIST MoGo::FastSmallVector<Location>
namespace MoGo {
  class Goban;
  class LocationInformation;

  /**
  Grouping all the information for the goban string
   
  @author Yizao Wang, Sylvain Gelly.
  */

  class LocationList {
  public:
    LocationList ( const int maxSize = 21 * 21 ) ;
    virtual ~LocationList () ;
    inline const Location &operator[] (int number) const {
      return locationList[number];
    }
    virtual void deleteLocation(const Location location);
    /** when using this version, we suppose that the location might be in the LocationList already, so before addition there always one check, which is expensive while LocationList is too long. */
    virtual void addLocation(const Location location);
    /** when using this version, we suppose that the location is never in the LocationList. */
    virtual void addLocation(const Location location, bool);
    void resize(const int size);
    virtual void clear();

    /*  virtual inline void deleteLocation( const Location location ) {
        //  int index = indexForLocation[location];
        int index = indexForLocation(location);
        //  indexForLocation[location]=-1;
        //assert(index>=0);FIXME
        if (index<0) return;
        locationList[index]=locationList[locationList.size()-1];
        //   if (index==(int)locationList.size()-1)
        //     indexForLocation[locationList[index]]=-1;
        //   else
        //     indexForLocation[locationList[index]]=index;

        locationList.pop_back();
      }

      virtual inline void addLocation( const Location location ) {
        assert(location>=0);
        //  assert(location<=(int)indexForLocation.size());
        //  if (indexForLocation[location]!=-1) return;
        if (indexForLocation(location)!=-1) return;
        //  indexForLocation[location]=locationList.size();
        locationList.push_back(location);
      }

      virtual inline void MoGo::LocationList::clear( ) {
        //  for (int i=0;i<(int)locationList.size();i++)
        //    indexForLocation[locationList[i]]=-1;
        locationList.clear();
      }



      void resize(const int ) {}*/
    inline int size() const {
      return locationList.size();
    }
    virtual inline bool isLocationIn(const Location location) const {
      return indexForLocation(location)+1;
    }
    inline int index(const Location location) const {   return indexForLocation(location);}

    inline int indexForLocation( const Location location ) const {
      for (int i=0;i<(int)locationList.size();i++)
        if (location==locationList[i]) return i;
      return -1;
    }
  protected:
    //     Vector < Location > locationList;
    FastSmallVector<Location> locationList;
    //Vector < int > indexForLocation;
  };


  class LongLocationList : public LocationList {
  public:
    LongLocationList ( const int maxSize = 21 * 21 ) ;
    virtual ~LongLocationList () ;
    inline void deleteLocation( const Location location ) {
      int index = indexForLocation[location];
      //int index = indexForLocation(location);
      indexForLocation[location]=-1;
      //assert(index>=0);FIXME
      if (index<0) return;
      locationList[index]=locationList[locationList.size()-1];
      if (index==(int)locationList.size()-1)
        indexForLocation[locationList[index]]=-1;
      else
        indexForLocation[locationList[index]]=index;

      locationList.pop_back();
    }

    inline void addLocation( const Location location ) {
      assert(location>=0);
      //  assert(location<=(int)indexForLocation.size());
      if (indexForLocation[location]!=-1) return;//FIXME
      //if (indexForLocation(location)!=-1) return;
      indexForLocation[location]=locationList.size();
      locationList.push_back(location);
    }

    inline void clear( ) {
      for (int i=0;i<(int)locationList.size();i++)
        indexForLocation[locationList[i]]=-1;
      locationList.clear();
    }

    void testShow() const;
    inline bool isLocationIn( const Location location ) const {
      return indexForLocation[location]+1;
    }
  private:
    //     Vector < Location > locationList;
    //  FastSmallVector<int> indexForLocation;
    FastSmallVector < int > indexForLocation;
    int maxSize;
  };


  /*
    class WeightedLocationList : public LocationList {
    public:
      WeightedLocationList ( const int maxSize = 21 * 21 ) ;
      virtual ~WeightedLocationList () ;
      virtual const Location operator[] (int number) const;
      virtual void deleteLocation(const Location location);
      virtual void addLocation(const Location location, const double weight = 1.);
      virtual void resize(const int size);
      virtual int size() const;
      virtual bool isLocationIn(const Location location) const;
      virtual int index(const Location location) const {return indexForLocation[location];}
      virtual void clear();
      Location weightestLocation() const ;
      Location randomOutOneAccordingToWeight() const;
      double weight( const Location location ) const {return weightList[indexForLocation[location]];}
    private:
      Vector < Location > locationList;
      Vector < double > weightList;
      Vector < int > indexForLocation;
   
    };
  */
  class StringRelatedLocationList : public LocationList {
  public:
    StringRelatedLocationList ( const int maxSize = 21 * 21 ) ;
    virtual ~StringRelatedLocationList () ;
    virtual void deleteLocation(const Location location);
    virtual void addLocation(const Location location, const Location stringLocation);
    virtual void clear();
    const TYPE_OF_VECTOR_STRING_LIST & relatedStrings( const Location location ) const;
    void deleteRelatedString( const Location location, const StringNumber strNumber);

  private:
    //     Vector < Location > locationList;
    FastSmallVector < TYPE_OF_VECTOR_STRING_LIST > stringsList;
    //Vector < LocationList > stringsList;
    //Vector < int > indexForLocation;

  };



  /** not used */
  class GoString {
  public:
    GoString();
    ~GoString();
    int liberty() const { return liberty_;}
    int length() const { return length_;}
    Location selfKillLocation() const {return selfKillLocation_;}
  private:
    int liberty_;
    int length_;
    Location selfKillLocation_;

  };


  /** not used */
  class StringInformation {
    friend class Goban;
    friend class LocationInformation;
    friend class StupidCenterOpeningEvaluator;

  public:
    StringInformation(Goban *goban);
    StringInformation();

    ~StringInformation();
    inline int getStringLength( const Location location ) const {
      return stringLength[stringNumber[location]];
    }

    inline int getStringLiberty( const Location location ) const {
      return stringLiberty[stringNumber[location]];
    }

    inline int getStringNumber( const Location location ) const {
      return stringNumber[location];
    }
    /** stringInformation gives the information of the string with the given stringNumber. allStringInformation gives all.*/

    void printAllStringInformation() const;
    void printStringInformation( const StringNumber strNumber ) const;





  private:
    void clear();
    inline void addOneEmptyString() {
      ++maxNumberOfString;
      stringLiberty.push_back(0);
      stringLength.push_back(0);
      int s=stringPositions.size(); stringPositions.resize(s+1);stringPositions[s].resize(50);stringPositions[s].clear();
    }
    void clearStrings();

    /** The stringNumber of each location.*/
    FastSmallVector< StringNumber > stringNumber;
    /** The liberty of string.*/
    //     Vector< StringLibertyNumber > stringLiberty;
    FastSmallVector< StringLibertyNumber > stringLiberty;
    /** The lengh(number of stones) of string.*/
    FastSmallVector< StringLength > stringLength; //FIXME
    /** The set of locations of strings.*/

    //mutable Vector< Location > stringSelfKillLocation;
    //mutable Vector< int > stringEyeNumber;

    //     Vector<Vector<Location> >stringPositions;
    FastSmallVector<FastSmallVector<Location> >stringPositions;
    /** The set of locations of strings.*/
    //     Vector<IntegerSet>libertyPositions;
    /** Locations of the erasedStones. Useful in analysis.*/
    //      Vector< Location> erasedStones;
    FastSmallVector < Location > erasedStones;

    /** We can not use stringLiberty.size() instead of this.*/
    int maxNumberOfString;
    void playMoveOnGoban(const Location location);

    void eraseEatenStones();
    void iterateEraseStone(const Location location);
    /** Update only those strings who were adjacent to those eaten stones.*/
    void updateLibertyAfterEraseStone();
    /** Update the enemyAdjacentStringLocation. In fact -1 for each.*/
    void updateEnemyStringLiberty(const Location &location);
    /** Update the friend string. Trick is used to accerlerate.*/
    void updateFriendString(const Location location);
    /** Used in the updateFriendString.*/
    inline StringNumber stringNumberOfLongestFriendString(const FastSmallVector< Location > &stringLocation) const {
      StringLength strLength=0;
      StringNumber strNum=maxNumberOfString+1;
      for (int i=0;i<(int)stringLocation.size();i++)
        if (stringLength[stringNumber[stringLocation[i]]]>strLength) {
          strNum=stringNumber[stringLocation[i]];
          strLength=stringLength[strNum];
        }
      return strNum;
    }
    /** Used in the updateFriendString.*/
    void iterateFriendStringUpdate(const Location location, const StringNumber strNumber);
    /** Used in the updateFriendString.*/
    inline bool isOneLibertyOfString(const Location location, const StringNumber strNumber) const ;
    bool isOneLibertyOfStringNonInline(const Location location, const StringNumber strNumber) const ;

    //void freeLocationUpdate( const Location location ) const;
    //void specialLocationUpdate( const Location location ) const ;
    void freeLocationUpdateBeta( const Location location ) const;



    Goban *goban;
    LocationInformation *locationInformation;

    void calculateAllFromGobanState();
    void calculateOneFromGobanState(const Location location);
    void iterateCalculateOneFromGobanState(const Location location,const StoneColor playerColor);
    void updateNumberOfEatenStones( );

    Location iterateSearchForLibertyWithMark( const Location location, const StoneColor stringColor ) ;
    void iterateEraseMark( const Location location );

    Location getOneLibertyOfString( const Location location ) const ;
    Location getHeadOfOneLibertyString( const Location location ) const ;
    bool doesThisStringAtariOneOther( const Location location ) const ;
    Location iterateSearchForLiberty( const Location location, LocationList &searchedStones ) const;

    Vector < StringNumber > getAdjacentStringsOfString( const Location location) const;
    void iterateSearchForAdjacentStrings( const Location location, LocationList &searchedStones, Vector < StringNumber > &adjacentStrings ) const ;

    bool isStillSelfAtari( const Location location, const StoneColor playerColor ) const;
    bool isStillSelfKill( const Location location, const StoneColor playerColor ) const ;

    void recalculateGobanAfterForcedChangement();
    void clearStringNumber();


    bool areTwoStringsAdjacent( const StringNumber strNb1, const StringNumber strNb2 ) const;
    /** areTwoStringsSharingTheSameTwoLiberties tests two strings of two liberties, which should be tested elsewhere, are they sharing the same two liberties.*/
    bool areTwoStringsSharingTheSameTwoLiberties( const StringNumber strNb1, const StringNumber strNb2) const;

  };




}

#endif

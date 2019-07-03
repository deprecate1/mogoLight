

//
// C++ Implementation: stringinformation_mg
//
// Description:
//
//
// Author: Yizao Wang <yizao.wang@polytechnique.edu> and Sylvain Gelly <gelly@lri.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "stringinformation_mg.h"
#include "goban_mg.h"
#include "gotools_mg.h"

using namespace MoGo;



//#define TEST_SPECIAL_URGENCY_CASE_ON

//I feel these two defines dangerous so I change them all. I think there is no one left.
//#define gobanState (goban->gobanState)
//#define direction(i) (goban->direction(i))
namespace MoGo {
  // #define updateFriendString_METHOD2
  // #define updateFriendString_METHOD3

  
  // TEMPORARY COMMENT: THIS FUNCTION IS USED
  StringInformation::StringInformation(Goban *goban) : locationInformation(&goban->locationInformation) {
    this->goban = goban;
    stringNumber.resize((goban->widthOfGoban+2)*(goban->heightOfGoban+2),0);
    //     stringLiberty.resize(widthOfGoban*heightOfGoban*2);
    //     stringLength.resize(widthOfGoban*heightOfGoban*2);
  }
  StringInformation::StringInformation() : locationInformation(0) {/**assert(0);**/
    this->goban = 0;
  }

  // TEMPORARY COMMENT: THIS FUNCTION IS USED
  void MoGo::StringInformation::clear( ) {
    Location location = goban->leftUpLocation_;
    for(int i=0;i<goban->heightOfGoban;i++,location+=2)
      for(int j=0;j<goban->widthOfGoban;j++,location++) {
        assert(location>=0);
        assert(location<(int)stringNumber.size());
        stringNumber[location]=0;
      }
    clearStrings();
    //stringSelfKillLocation.resize(1);
    //stringEyeNumber.resize(1);
    //stringPositions.resize(1);
    //     libertyPositions.resize(1);
  }

  // TEMPORARY COMMENT: THIS FUNCTION IS USED
  void StringInformation::clearStrings( ) {
    maxNumberOfString=0;
    stringLiberty.resize(50);stringLiberty.resize(1);stringLiberty[0]=0;
    stringLength.resize(50);stringLength.resize(1);stringLength[0]=0;
    stringPositions.resize(50);stringPositions.resize(1);stringPositions[0].resize(50);stringPositions[0].clear();

  }



// TEMPORARY COMMENT: THIS FUNCTION IS USED
  StringInformation::~StringInformation() {}}




// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::StringInformation::playMoveOnGoban( const Location location ) {
  updateEnemyStringLiberty(location);

  //   printAllStringInformation();
  // goban->textModeShowGoban();
  // goban->textModeShowPosition(location);
  eraseEatenStones();
  if (erasedStones.size()>0) {
    updateLibertyAfterEraseStone();
    updateNumberOfEatenStones();
  }
  updateFriendString(location);

}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::StringInformation::updateNumberOfEatenStones( ) {
  if (goban->playerColor==BLACK)
    goban->whiteEatenStoneNumber+=erasedStones.size();
  else
    goban->blackEatenStoneNumber+=erasedStones.size();
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::StringInformation::eraseEatenStones( ) {
  //Here, for each enemy string we begin from one location the erase-iteration. The clean of stringLiberty and stringLength is then done once(at most 4 times in rare case) for each string, which is economic. Meanwhile, in iterateEraseStone, we clean stringNumber and gobanState at each location covered by the enemystring.
  FastSmallVector<Location> &deadAdjacentStringLocation = locationInformation->deadAdjacentStringLocation;
  erasedStones.clear();
  for(int i=0;i<(int)deadAdjacentStringLocation.size();i++)
    if (goban->gobanState[deadAdjacentStringLocation[i]]!=0) {
      stringLiberty[stringNumber[deadAdjacentStringLocation[i]]]=0;
      stringLength[stringNumber[deadAdjacentStringLocation[i]]]=0;
      iterateEraseStone(deadAdjacentStringLocation[i]);
      stringPositions[stringNumber[deadAdjacentStringLocation[i]]].clear();
    }
  return ;

}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::StringInformation::updateLibertyAfterEraseStone( ) {
  //Cout << "number of erasedStones: " << erasedStones.size() ;

  for (int i=0;i<(int)erasedStones.size();i++)
    for (int j=0;j<4;j++) {
      //         assert(erasedStones[i]+goban->direction(j)<gobanState.size());
      //assert(erasedStones[i]+goban->direction(j)>=0);
      if (goban->gobanState[erasedStones[i]+goban->direction(j)]==BLACK || goban->gobanState[erasedStones[i]+goban->direction(j)]==WHITE) {
        int test=0;
        for (int k=0;k<j;k++) {
          //assert(erasedStones[i]+goban->direction(k)<(int)stringNumber.size());
          //assert(erasedStones[i]+goban->direction(k)>=0);
          //assert(erasedStones[i]+goban->direction(j)<(int)stringNumber.size());
          //assert(erasedStones[i]+goban->direction(j)>=0);
          if (stringNumber[erasedStones[i]+goban->direction(k)]==stringNumber[erasedStones[i]+goban->direction(j)]) {
	    // TEMPORARY COMMENT: THIS FUNCTION IS USED
	    // TEMPORARY COMMENT: BLACK PLAYS 2 TIMES
            test=1;
            break;
          }
        }
        if (test==0) {
          Location location=erasedStones[i]+goban->direction(j);//FIXME on the goban?
          stringLiberty[stringNumber[location]]++;

        }

      }
    }
  if (Goban::updateCloseLiberties) {/**assert(0);**/
    for (int i=0;i<(int)erasedStones.size();i++)
      goban->closeLiberties.push_back(erasedStones[i]);
  }
}


// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::StringInformation::iterateEraseStone( const Location location ) {
 

  const FastSmallVector<int> &positions=stringPositions[stringNumber[location]];
  assert(positions.size() > 0);
  for (int p=0;p<positions.size();p++) {
    Location location=positions[p];
    goban->gobanState[location]=0;
    stringNumber[location]=0;
    erasedStones.push_back(location);
  }
 
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::StringInformation::updateFriendString(const Location location ) {
  FastSmallVector<Location> &friendAdjacentStringLocation = locationInformation->friendAdjacentStringLocation;
  StringNumber strNumber;//the maximum string Number
  if (friendAdjacentStringLocation.size()>0) {
    strNumber=stringNumberOfLongestFriendString(locationInformation->friendAdjacentStringLocation);
    if (strNumber>maxNumberOfString) {/**assert(0);**/
      goban->textModeShowGoban(PASSMOVE,false,0,false);
      goban->textModeShowPosition(location);
      printAllStringInformation();
      locationInformation->showStringPreviewResult();
      GoTools::print("(%d,%d)",goban->isCFGFunctionOn(),goban->isCFGOn);

      for (int i=0;i<(int)locationInformation->friendAdjacentStringLocation.size();i++) {/**assert(0);**/
        goban->textModeShowPosition(locationInformation->friendAdjacentStringLocation[i]);
        GoTools::print("gobanState color %d,%d\n",goban->gobanState[locationInformation->friendAdjacentStringLocation[i]],stringNumber[locationInformation->friendAdjacentStringLocation[i]]);
      }
      GoTools::print("%d,%d\n",strNumber,maxNumberOfString);
      assert(strNumber<=maxNumberOfString);
    }
    assert(stringLength[strNumber]==(int)stringPositions[strNumber].size());
    stringLiberty[strNumber]--;
    for(int i=0;i<(int)friendAdjacentStringLocation.size();i++)
      if (stringNumber[friendAdjacentStringLocation[i]]!=strNumber) {
        Location friendStringNumber=stringNumber[friendAdjacentStringLocation[i]];
        assert(stringPositions.size()==stringLength.size());assert(strNumber<(int)stringPositions.size());
        //         int oldStringSize=stringPositions[strNumber].size();

        assert(stringLength[strNumber]==(int)stringPositions[strNumber].size());
        concat(stringPositions[strNumber],stringPositions[friendStringNumber]);
        stringLength[strNumber] += stringLength[friendStringNumber];
        assert(stringLength[strNumber]==(int)stringPositions[strNumber].size());

        stringLiberty[friendStringNumber]=0;
        stringLength[friendStringNumber]=0;

        const FastSmallVector<Location> &tmp=stringPositions[friendStringNumber];
        for (int j=0;j<(int)tmp.size();j++) {
          for (int k=0;k<4;k++) {
            int a=tmp[j]+goban->direction(k);
            if (goban->gobanState[a]==EMPTY)
              if (!isOneLibertyOfString(a,strNumber))
                stringLiberty[strNumber]++;
          }
          stringNumber[tmp[j]]=strNumber;
          assert(goban->gobanState[tmp[j]]==goban->playerColor);
        }
        stringPositions[friendStringNumber].clear();
      }
  } else {
    addOneEmptyString();
    strNumber=maxNumberOfString;
    // stringPositions.push_back(FastSmallVector<Location>());
    //stringSelfKillLocation.push_back(0);
    //stringEyeNumber.push_back(0);

  }
  assert(stringLength[strNumber]==(int)stringPositions[strNumber].size());
  for (int k=0;k<4;k++)
    if (goban->gobanState[location+goban->direction(k)]==EMPTY)
      if (!isOneLibertyOfString(location+goban->direction(k),strNumber))
        stringLiberty[strNumber]++;
  goban->gobanState[location]=goban->playerColor;
  stringNumber[location]=strNumber;
  stringPositions[strNumber].push_back(location);
  stringLength[strNumber] ++;
  assert(stringLength[strNumber]==(int)stringPositions[strNumber].size());

  //  Cout << "current move stringNumber:" << stringNumber[location];
  return;
  
}



/** Non recursive version */
void MoGo::StringInformation::iterateFriendStringUpdate( const Location _location, const StringNumber strNumber ) {/**assert(0);**/
  //   assert(strNumber >= 0);
  //   assert(strNumber < (int)stringLiberty.size());

  // FastSmallVector<Location> front;
  MoGo::FastSmallVector<Location> frontGlobalStringInformation(100);
  frontGlobalStringInformation.clear();frontGlobalStringInformation.push_back(_location);
  while (frontGlobalStringInformation.size()>0) {/**assert(0);**/
    Location location=frontGlobalStringInformation[frontGlobalStringInformation.size()-1];
    frontGlobalStringInformation.pop_back();
    //     if (goban->gobanState[location]!=goban->playerColor)
    //       continue;
    if (stringNumber[location]==strNumber)
      continue;
    for (int i=0;i<4;i++) {/**assert(0);**/
      const Location tmp=location+goban->direction(i);
      if (goban->gobanState[tmp]==EMPTY) {/**assert(0);**/
        int j;
        for (j=0;j<4;j++)
          if (stringNumber[tmp+goban->direction(i)]==strNumber)
            break;
        if (j==4)
          stringLiberty[strNumber]++;
        //         if (!isOneLibertyOfString(tmp,strNumber))
        //           stringLiberty[strNumber]++;
      }
    }
    stringNumber[location]=strNumber;
    stringLength[strNumber]++;
    for (int i=0;i<4;i++) {/**assert(0);**/
      const Location tmp=location+goban->direction(i);
      if (goban->gobanState[tmp]==goban->playerColor)
        frontGlobalStringInformation.push_back(tmp);
    }
  }

  return;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::StringInformation::isOneLibertyOfString( const Location location, const StringNumber strNumber ) const {
  int i;
  for (i=0;i<4;i++)
    if (stringNumber[location+goban->direction(i)]==strNumber) return true;
  return false;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::StringInformation::updateEnemyStringLiberty(const Location & ) {
  int test;
  const FastSmallVector<Location> &enemyAdjacentStringLocation = locationInformation->enemyAdjacentStringLocation;
  for(int i=0;i<(int)enemyAdjacentStringLocation.size();i++) {
#ifdef updateFriendString_METHOD3
    libertyPositions[stringNumber[enemyAdjacentStringLocation[i]]].remove(location);
#endif
    test=0;
    for(int j=0;j<i;j++)//stupid here. how to optimize?
      if (stringNumber[enemyAdjacentStringLocation[j]]==stringNumber[enemyAdjacentStringLocation[i]]) {
	// TEMPORARY COMMENT: THIS FUNCTION IS USED
	// TEMPORARY COMMENT: BLACK PLAYS 2 TIMES
	
        test=1;
        break;
      }
    if (test==0) stringLiberty[stringNumber[enemyAdjacentStringLocation[i]]]--;
  }
}



void MoGo::StringInformation::printStringInformation( const StringNumber strNumber ) const {/**assert(0);**/
  GoTools::print("stringNumber:%3d || stringLiberty:%3d | stringLength:%3d ",strNumber,stringLiberty[strNumber],stringLength[strNumber]);
  //goban->textModeShowPosition(stringSelfKillLocation[strNumber]);
  GoTools::print("\n");
  //GoTools::print("stringEyeNumber:%d",stringEyeNumber[strNumber]);
#ifdef updateFriendString_METHOD2
  for (int i = 0 ; i < (int)stringPositions[strNumber].size() ; i++) {/**assert(0);**/
    Cout << stringPositions[strNumber][i] << " ";
  }
  Cout << "\n";
#endif
  return ;
}


void MoGo::StringInformation::printAllStringInformation( ) const {/**assert(0);**/
  for (int i=0;i<=maxNumberOfString;i++)
    if (stringLength[i]>0)
      printStringInformation(i);
  return;
}


// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::StringInformation::calculateAllFromGobanState( ) {
  clearStrings();
  bool patched = false;
  Location location = goban->leftUpLocation_;
  for (int i=0;i<goban->heightOfGoban;i++,location+=2)
    for (int j=0;j<goban->widthOfGoban;j++,location++)
      if (stringNumber[location]==0 &&
          goban->gobanState[location]!=EMPTY &&
          goban->gobanState[location]!=OUTSIDE )//for the location simulation.
      {
        calculateOneFromGobanState(location);
        //patch for zoneBitmapCase
        if (stringLiberty[maxNumberOfString] == 0) {/**assert(0);**/
          assert(maxNumberOfString == (int)stringLiberty.size()-1);
          //goban->textModeShowPosition(location);
          //goban->textModeShowGoban(PASSMOVE,true);
          //GoTools::print("!");
          assert(goban->isOutsideStrange);
          for (int k=0;k<(int)stringPositions[maxNumberOfString].size();k++) {/**assert(0);**/
            Location tmpLocation = stringPositions[maxNumberOfString][k];
            //GoTools::print("dead stone cleared by patch for zoneBitmap\n");
            patched = true;
            //getchar();
            goban->gobanState[tmpLocation] = 0;
            for (int l=0;l<4;l++)
              if (stringNumber[tmpLocation+goban->direction(l)]>0 &&
                  stringNumber[tmpLocation+goban->direction(l)] != maxNumberOfString) {/**assert(0);**/
                int m = 0;
                for (m=0;m<l;m++)
                  if (stringNumber[tmpLocation+goban->direction(m)] ==
                      stringNumber[tmpLocation+goban->direction(l)])
                    break;
                if (m==l)
                  stringLiberty[stringNumber[tmpLocation+goban->direction(l)]]++;
              }
            stringNumber[tmpLocation] = 0;
            if (goban->isInRangeLocation(tmpLocation))
              goban->gobanLists.freeLocation.addLocation(tmpLocation);
          }
          stringPositions[maxNumberOfString].clear();
          stringLiberty[maxNumberOfString] = 0;
          stringLength[maxNumberOfString] = 0;
        }
      }
 
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::StringInformation::calculateOneFromGobanState(const Location location) {
  addOneEmptyString();
 
  iterateCalculateOneFromGobanState(location,goban->gobanState[location]);
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::StringInformation::iterateCalculateOneFromGobanState(const Location location,const StoneColor stringColor) {
  assert(maxNumberOfString >= 0);
  assert(maxNumberOfString == (int)stringLiberty.size()-1);
  if (goban->gobanState[location]!=stringColor) return;// other player's stone or outside. iteration never falls on an empty location.
  if (stringNumber[location]==maxNumberOfString) return;
  for (int i=0;i<4;i++)
    if (goban->gobanState[location+goban->direction(i)]==EMPTY)
      if (!isOneLibertyOfString(location+goban->direction(i),maxNumberOfString))
        stringLiberty[maxNumberOfString]++;
  stringNumber[location]=maxNumberOfString;
  stringLength[maxNumberOfString]++;
  stringPositions[maxNumberOfString].push_back(location);
  for (int i=0;i<4;i++)
    if (goban->gobanState[location+goban->direction(i)]==stringColor)
      iterateCalculateOneFromGobanState(location+goban->direction(i),stringColor);
  return;
}


// TEMPORARY COMMENT: THIS FUNCTION IS USED
Location MoGo::StringInformation::getOneLibertyOfString( const Location location) const {
  //   MoGo::LocationList searchedStonesGobalTmp;
  //   searchedStonesGobalTmp.clear();
  //   Location tmpLocation=iterateSearchForLiberty( location, searchedStonesGobalTmp );
  const FastSmallVector<int> &positions=stringPositions[stringNumber[location]];
  for (int p=0;p<positions.size();p++)
    for (int i=0;i<4;i++)
      if (goban->gobanState[positions[p]+goban->direction(i)]==EMPTY)
        return positions[p]+goban->direction(i);

  //   assert(tmpLocation!=NOTFOUND);
  goban->textModeShowGoban();
  goban->textModeShowPosition(location);
  assert(0);
  return NOTFOUND;
}
Location MoGo::StringInformation::getHeadOfOneLibertyString( const Location location ) const {/**assert(0);**/
  //   MoGo::LocationList searchedStonesGobalTmp;
  //   searchedStonesGobalTmp.clear();
  //   Location tmpLocation=iterateSearchForLiberty( location, searchedStonesGobalTmp );
  const FastSmallVector<int> &positions=stringPositions[stringNumber[location]];
  for (int p=0;p<positions.size();p++)
    for (int i=0;i<4;i++) {/**assert(0);**/
      if (goban->gobanState[positions[p]+goban->direction(i)]==EMPTY)
        return positions[p];
    }

  //   assert(tmpLocation!=NOTFOUND);
  assert(0);
  return NOTFOUND;
}
bool MoGo::StringInformation::doesThisStringAtariOneOther( const Location location ) const {/**assert(0);**/
  int strNumber=stringNumber[location];
  const FastSmallVector<int> &positions=stringPositions[strNumber];
  for (int p=0;p<positions.size();p++)
    for (int i=0;i<4;i++) {/**assert(0);**/
      Location tmp=positions[p]+goban->direction(i);
      if ((goban->gobanState[tmp]==BLACK || goban->gobanState[tmp]==WHITE) && (getStringNumber(tmp)!=strNumber) && getStringLiberty(tmp)==1)
        return true;
    }
  return false;
}


/** Non recursive version */
// MoGo::IntegerSet globalSetSearchedStones;
Location MoGo::StringInformation::iterateSearchForLiberty( const Location location, LocationList &searchedStones ) const  {/**assert(0);**/
  MoGo::FastSmallVector<Location> frontGlobalStringInformation(100);
  frontGlobalStringInformation.clear();frontGlobalStringInformation.push_back(location);
  // globalSetSearchedStones.clear();
  while (frontGlobalStringInformation.size()>0) {/**assert(0);**/
    Location locationTmp=frontGlobalStringInformation[frontGlobalStringInformation.size()-1];
    frontGlobalStringInformation.pop_back();
    //     if (globalSetSearchedStones.contains(locationTmp))
    if (searchedStones.isLocationIn(locationTmp))
      continue;
    searchedStones.addLocation(locationTmp);
    //     globalSetSearchedStones.add(locationTmp);
    for (int i=0;i<4;i++) {/**assert(0);**/
      Location tmp=locationTmp+goban->direction(i);
      if (goban->gobanState[tmp]==EMPTY)
        return tmp;
      else if (goban->gobanState[tmp]==goban->gobanState[locationTmp])
        frontGlobalStringInformation.push_back(tmp);
    }
  }
  return NOTFOUND;
}

Vector < StringNumber > MoGo::StringInformation::getAdjacentStringsOfString( const Location location) const {/**assert(0);**/
  MoGo::LocationList searchedStonesGobalTmp;
  searchedStonesGobalTmp.clear();
  Vector < StringNumber > adjacentStrings;
  iterateSearchForAdjacentStrings( location, searchedStonesGobalTmp, adjacentStrings );
  return adjacentStrings;
}

void MoGo::StringInformation::iterateSearchForAdjacentStrings( const Location location, LocationList &searchedStones, Vector < StringNumber > &adjacentStrings ) const  {/**assert(0);**/
  if (searchedStones.isLocationIn(location)) return ;
  searchedStones.addLocation(location);
  for (int i=0;i<4;i++)
    if (goban->gobanState[location+goban->direction(i)]==goban->gobanState[location])
      iterateSearchForAdjacentStrings(location+goban->direction(i),searchedStones,adjacentStrings);
    else if (goban->gobanState[location+goban->direction(i)]==WHITE || goban->gobanState[location+goban->direction(i)]==BLACK) {/**assert(0);**/
      for (int j=0;j<(int)adjacentStrings.size();j++)
        if (adjacentStrings[j]==stringNumber[location+goban->direction(i)]) continue;
      adjacentStrings.push_back(stringNumber[location+goban->direction(i)]);
    }
  return ;
}


Location MoGo::StringInformation::iterateSearchForLibertyWithMark( const Location location, const StoneColor stringColor ) {/**assert(0);**/
  //with mark is a trick to get some efficiency. it's somehow dangerous but i like it:)

  if (goban->gobanState[location]==EMPTY) return location;
  if (goban->gobanState[location]!=stringColor) return NOTFOUND;
  if (stringNumber[location]>maxNumberOfString) return NOTFOUND;
  stringNumber[location]+=maxNumberOfString;
  for (int i=0;i<4;i++) {/**assert(0);**/
    Location tmpLocation=iterateSearchForLibertyWithMark(location+goban->direction(i),stringColor);
    if (tmpLocation!=NOTFOUND) return tmpLocation;
  }
  return NOTFOUND;
}

void MoGo::StringInformation::iterateEraseMark( const Location location ) {/**assert(0);**/
  if (stringNumber[location]<=maxNumberOfString) return;
  stringNumber[location]-=maxNumberOfString;
  for (int i=0;i<4;i++)
    iterateEraseMark(location+goban->direction(i));
}

bool MoGo::StringInformation::isStillSelfAtari( const Location location, const StoneColor playerColor ) const {/**assert(0);**/
  int test = 0;
  for (int i=0;i<4;i++)
    if (goban->gobanState[location+goban->direction(i)]==playerColor) {/**assert(0);**/
      if (stringLiberty[stringNumber[location+goban->direction(i)]]>2) return false;
      if (stringLiberty[stringNumber[location+goban->direction(i)]]==2) test = 1;
    }

  return test;
}

bool MoGo::StringInformation::isStillSelfKill( const Location location, const StoneColor playerColor ) const {/**assert(0);**/

  for (int i=0;i<4;i++) {/**assert(0);**/
    if (goban->gobanState[location+goban->direction(i)]==EMPTY) return false;
    if (goban->gobanState[location+goban->direction(i)]==playerColor) {/**assert(0);**/
      if (stringLiberty[stringNumber[location+goban->direction(i)]]>=2) return false;
    }
  }
  return true;
}

////////////////////////
// class LocationList //
////////////////////////


// TEMPORARY COMMENT: THIS FUNCTION IS USED
MoGo::LocationList::LocationList( const int /*maxSize*/ ) /*: indexForLocation(maxSize)*/ {
  //  for (int i=0;i<maxSize;i++)
  //    indexForLocation[i]=-1;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
MoGo::LocationList::~ LocationList( ) {}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::LocationList::deleteLocation( const Location location ) {
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

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::LocationList::addLocation( const Location location ) {
  assert(location>=0);
  //  assert(location<=(int)indexForLocation.size());
  //  if (indexForLocation[location]!=-1) return;
  if (indexForLocation(location)!=-1) return;//FIXME OPTIMIZATION here we lost too much!
  //  indexForLocation[location]=locationList.size();
  locationList.push_back(location);
}

void MoGo::LocationList::addLocation( const Location location, bool  ) {/**assert(0);**/
  assert(location>=0);
  locationList.push_back(location);
}


void MoGo::LocationList::resize( const int /*size*/ ) {/**assert(0);**/
  //  indexForLocation.resize(size);
}


// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::LocationList::clear( ) {
  //  for (int i=0;i<(int)locationList.size();i++)
  //    indexForLocation[locationList[i]]=-1;
  locationList.clear();
}



////////////////////////////
// class LongLocationList //
////////////////////////////


// TEMPORARY COMMENT: THIS FUNCTION IS USED
MoGo::LongLocationList::LongLocationList( const int maxSize ) : indexForLocation(maxSize) {
  for (int i=0;i<maxSize;i++)
    indexForLocation[i]=-1;
  this->maxSize = maxSize;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
MoGo::LongLocationList::~ LongLocationList( ) {}





void MoGo::LongLocationList::testShow( ) const {/**assert(0);**/
  int j=0;
  for (int i=0;i<(int)indexForLocation.size();i++)
    if (indexForLocation[i]!=-1) {/**assert(0);**/
      GoTools::print("%d ",indexForLocation[i]);
      j++;
    }
  GoTools::print("%d location saved. %d index saved.\n",locationList.size(),j);
}






////////////////////////////////
// class StringRelatedLocationList //
////////////////////////////////


MoGo::StringRelatedLocationList::StringRelatedLocationList( const int /*maxSize*/ ) {/**assert(0);**/
  //  for (int i=0;i<maxSize;i++)
  //    indexForLocation[i]=-1;
}

MoGo::StringRelatedLocationList::~ StringRelatedLocationList( ) {/**assert(0);**/}


void MoGo::StringRelatedLocationList::deleteLocation( const Location location ) {/**assert(0);**/
  //  int index = indexForLocation[location];
  int index = indexForLocation(location);
  //  indexForLocation[location]=-1;
  assert(index>=0);
  assert(locationList.size()==(int)stringsList.size());
  if (index < (int)locationList.size()-1) {/**assert(0);**/
    locationList[index]=locationList[locationList.size()-1];
    stringsList[index]=stringsList[locationList.size()-1];
  }
  //   if (index==(int)locationList.size()-1)
  //     indexForLocation[locationList[index]]=-1;
  //   else
  //     indexForLocation[locationList[index]]=index;

  locationList.pop_back();
  stringsList.pop_back();

}

void MoGo::StringRelatedLocationList::addLocation( const Location location, const Location stringLocation ) {/**assert(0);**/
  assert(location>=0);
  //  assert(location<=(int)indexForLocation.size());
  //  if (indexForLocation[location]>=0) {/**assert(0);**/
  int index=indexForLocation(location);
  if (index>=0) {/**assert(0);**/
    for (int i=0;i<(int)stringsList[index].size();i++)
      if (stringsList[index][i]==stringLocation) {/**assert(0);**/
        return;
      }
    stringsList[index].push_back(stringLocation);//in fact here sometimes the same string is pushed back more than once.
    //stringsList[indexForLocation(location)].push_back(stringLocation);
  } else {/**assert(0);**/
    //    indexForLocation[location]=locationList.size();
    locationList.push_back(location);
    TYPE_OF_VECTOR_STRING_LIST newStringLocation(1);newStringLocation[0]=stringLocation;
    //     newStringLocation.push_back(stringLocation);
    stringsList.push_back(newStringLocation);
  }
}




void MoGo::StringRelatedLocationList::clear( ) {/**assert(0);**/
  //  for (int i=0;i<(int)locationList.size();i++)
  //    indexForLocation[locationList[i]]=-1;
  locationList.clear();
  stringsList.clear();
}

void MoGo::StringRelatedLocationList::deleteRelatedString( const Location location, const StringNumber strNumber ) {/**assert(0);**/
  int ind=index(location);
  for (int i=0;i<(int)stringsList[ind].size()-1;i++)
    if (stringsList[ind][i]==strNumber) {/**assert(0);**/
      stringsList[ind][i]=stringsList[ind][stringsList[ind].size()-1];
      break;
    }
  stringsList[ind].pop_back();
}

const TYPE_OF_VECTOR_STRING_LIST & MoGo::StringRelatedLocationList::relatedStrings( const Location location ) const {/**assert(0);**/
  //  return stringsList[indexForLocation[location]];
  return stringsList[indexForLocation(location)];
}


void MoGo::StringInformation::recalculateGobanAfterForcedChangement( ) {/**assert(0);**/
  clearStrings();
  clearStringNumber();
  //goban->textModeShowGoban();
  calculateAllFromGobanState();
  //goban->textModeShowGoban();


  LocationList stringsToErase;//treat stringNumber as Location
  for (int i=0;i<=maxNumberOfString;i++)
    if (stringLength[i]>0 && stringLiberty[i]==0) {/**assert(0);**/
      //Vector < stringNumber > tmpStringsToRecheck = getAdjacentStringsOfString(stringPositions[i][0]);
      //for (int j=0;j<(int)tmpStringsToRecheck.size();j++)
      //stringsToRecheck.addLocation(tmpStringsToRecheck[j]);
      stringsToErase.addLocation(i);
    }

  while (stringsToErase.size()) {/**assert(0);**/

    int index = MoGo::GoTools::mogoRand(stringsToErase.size());
    if (stringLiberty[stringsToErase[index]]>0) {/**assert(0);**/
      stringsToErase.deleteLocation(stringsToErase[index]);
      continue;
    }

    StringNumber toBeErasedNumber = stringsToErase[index];

    //GoTools::print("in recalculate. string to erase:%d\n",toBeErasedNumber);
    //goban->textModeShowGoban();
    //goban->printAllStringInformation();
    //getchar();

    StoneColor stringColor = goban->gobanState[stringPositions[toBeErasedNumber][0]];
    assert(stringColor == 1 || stringColor == 2);
    //Vector < stringNumber > tmpStringsToRecheck = getAdjacentStringsOfString(stringPositions[stringsToErase[index]][0]);
    for (int i=0;i<(int)stringPositions[toBeErasedNumber].size();i++) {/**assert(0);**/
      goban->gobanState[stringPositions[toBeErasedNumber][i]]=0;
      stringNumber[stringPositions[toBeErasedNumber][i]]=0;
      for (int j=0;j<4;j++)
        if (goban->gobanState[stringPositions[toBeErasedNumber][i]+goban->direction(j)]==3-stringColor) {/**assert(0);**/
          int test = 1;
          for (int k=0;k<j;k++)
            if (stringNumber[stringPositions[toBeErasedNumber][i]+goban->direction(j)]==stringNumber[stringPositions[toBeErasedNumber][i]+goban->direction(k)]) {/**assert(0);**/
              test = 0;
              break;
            }
          if (test)
            stringLiberty[stringNumber[stringPositions[toBeErasedNumber][i]+goban->direction(j)]]++;
        }
    }
    stringLiberty[toBeErasedNumber]=0;
    stringLength[toBeErasedNumber]=0;
    stringPositions[toBeErasedNumber].clear();
    stringsToErase.deleteLocation(toBeErasedNumber);
  }
}

void MoGo::StringInformation::clearStringNumber( ) {/**assert(0);**/
  for (int i=0;i<(int)stringNumber.size();i++)
    stringNumber[i]=0;
}

bool MoGo::StringInformation::areTwoStringsAdjacent( const StringNumber strNb1, const StringNumber strNb2 ) const {/**assert(0);**/
  StringNumber testStrNb1 = 0, testStrNb2 = 0;
  if (stringLength[strNb1] < stringLength[strNb2] ) {/**assert(0);**/
    testStrNb1 = strNb1;
    testStrNb2 = strNb2;
  } else {/**assert(0);**/
    testStrNb1 = strNb2;
    testStrNb2 = strNb1;
  }

  for (int i=0;i<(int)stringPositions[testStrNb1].size();i++)
    for (int j=0;j<4;j++)
      if (stringNumber[goban->gobanState[stringPositions[testStrNb1][i]+goban->direction(j)]] == testStrNb2) return true;
  return false;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
// TEMPORARY COMMENT: BLACK PLAYS 2 TIMES
bool MoGo::StringInformation::areTwoStringsSharingTheSameTwoLiberties( const StringNumber strNb1, const StringNumber strNb2 ) const {
  assert(stringLiberty[strNb1] == 2);
  assert(stringLiberty[strNb2] == 2);
  StringNumber testStrNb1 = 0, testStrNb2 = 0;
  if (stringLength[strNb1] < stringLength[strNb2] ) {
    testStrNb1 = strNb1;
    testStrNb2 = strNb2;
  } else {
    testStrNb1 = strNb2;
    testStrNb2 = strNb1;
  }
  int liberty = 0;
  Location testedLiberty = PASSMOVE;
  for (int i=0;i<(int)stringPositions[testStrNb1].size() && liberty<2;i++)
    for (int j=0;j<4 && liberty<2;j++)
      if (
        goban->gobanState[stringPositions[testStrNb1][i]+goban->direction(j)] == EMPTY &&
        testedLiberty != stringPositions[testStrNb1][i]+goban->direction(j)) {
        liberty ++;
        testedLiberty = stringPositions[testStrNb1][i]+goban->direction(j);
        assert(goban->gobanState[testedLiberty] == EMPTY);
        if (!isOneLibertyOfString(testedLiberty, testStrNb2)) return false;
      }
  assert(liberty == 2);
  return true;
}

bool MoGo::StringInformation::isOneLibertyOfStringNonInline( const Location location, const StringNumber strNumber ) const {/**assert(0);**/
  int i;
  for (i=0;i<4;i++)
    if (stringNumber[location+goban->direction(i)]==strNumber) return true;
  return false;
}






//
// C++ Implementation: locationinformation_mg
//
// Description:
//
//
// Author: Yizao Wang <yizao.wang@polytechnique.edu> and Sylvain Gelly <gelly@lri.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "locationinformation_mg.h"
#include "goban_mg.h"
#include "stringinformation_mg.h"
#include "gotools_mg.h"
#include <assert.h>



#define LocationDirectionI (location+goban->direction(i))

int MoGo::LocationInformation::isSelfAtariMode = 0;

// TEMPORARY COMMENT: THIS FUNCTION IS USED
MoGo::LocationInformation::LocationInformation( Goban * goban ) {
  this->goban = goban;
  this->stringInformation = &goban->stringInformation;
  resetStringPreview();

  friendAdjacentStringLocation.resize(4);
  enemyAdjacentStringLocation.resize(4);
  deadAdjacentStringLocation.resize(4);
  freeAdjacentLocation.resize(4);
  testKo=0;
  isLocationSelfAtari=false;
}
MoGo::LocationInformation::LocationInformation() {/**assert(0);**/
  this->goban = 0;
  this->stringInformation = 0;
  resetStringPreview();

  friendAdjacentStringLocation.resize(4);
  enemyAdjacentStringLocation.resize(4);
  deadAdjacentStringLocation.resize(4);
  freeAdjacentLocation.resize(4);
  testKo=0;
  isLocationSelfAtari=false;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
int MoGo::LocationInformation::basicTest( const Location location ) {
  assert(location!=PASSMOVE);
  //if (location==PASSMOVE) return LEGAL;
  testKo=0;

  //   if (location>=(int)goban->gobanState.size() || location <0) {/**assert(0);**/
  //     Cout << "Wrong position!\n";
  //     assert(0);
  //     return OUTGOBAN;
  //   }
  //assert(goban->gobanState[location]!=OUTSIDE);
  if (goban->gobanState[location]==OUTSIDE) return OUTGOBAN;//FIXME
  if (goban->gobanState[location]==WHITE || goban->gobanState[location]==BLACK) return ALREADYTAKEN;
  //  if (FreeAdjacent(Goban,loc)>=1) return LEGAL;
  if (!stringPreview(location, goban->gobanState, goban->directionsArray, goban->playerColor))
    return SELFKILLMOVE;

  if (freeAdjacentLocation.size()) return LEGAL;//has liberty. FIXME

  if (deadAdjacentStringLocation.size()!=1)//More than one string or nothing eaten. impossible to be a ko move.
    return LEGAL;
  // stringLength[stringNumber[deadAdjacentStringLocation[0]]]
  if (stringInformation->getStringLength(deadAdjacentStringLocation[0])!=1)//one string of more than one stone eaten. impossible to be a ko move.
    return LEGAL;

  if (friendAdjacentStringLocation.size()!=0)//length of this move!=1, impossible to be a ko move
    return LEGAL;
  if (location==goban->ko)
    return KOILL;
  testKo=deadAdjacentStringLocation[0];
  return LEGAL;//in the old version here return testKo. Now testKo is a private variable.
}




// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::LocationInformation::resetStringPreview( ) {
  lastStringPreviewLocation=-1;
  lastStringPreviewResult=false;
}


// TEMPORARY COMMENT: THIS FUNCTION IS USED
int MoGo::LocationInformation::uselessnessTest(const Location location)  const//executed ALWAYS after basicTest(then stringPreview) returns LEGAL, nobody except yizao understands these terrible codes... TOO BAD.
{
  assert(location!=PASSMOVE);
  if (MoGo::Goban::notUselessMode == 0) {
    //if (location==PASSMOVE) return LEGAL; //TODO where should we leave this passmove test?
    if (deadAdjacentStringLocation.size()>0) return 0;
    if (freeAdjacentLocation.size()>1) return 0;//more than one free intersection
    if (enemyAdjacentStringLocation.size()>0 && friendAdjacentStringLocation.size()>0) return 0;//no or zero free intersection adjacent, different color stones surrounded
    //now, at most one free intersection, surrounded by the SAME COLOR stones.
    for(int i=0;i<(int)enemyAdjacentStringLocation.size();i++)
      if (stringInformation->getStringLiberty(enemyAdjacentStringLocation[i])==2) return 0;//atari move FIXME here we can utilize...
    int j=0,j1=0,j2=0,h1=0;
    for(int i=4;i<8;i++) {
      int state=goban->gobanState[location+goban->direction(i)];
      if (state==EMPTY) j2++;
      else if (state==OUTSIDE) j1++;
      else if (state==goban->playerColor) j++;
      else h1++;
    }
    //Cout << "j,j1,j2,h1=" << j << " " << j1 << " " << j2 <<" " << h1 << "\n";
    //j: corner taken by friend stone, j1:corner outside goban, j2: corner free
    //h1:corner taken by different color stone
    if (freeAdjacentLocation.size()==0) //thus by the hypothese before, 4 adjacent locations are friends
    {
      if (
        (j>=3 && j1==0) //a real eye not on the boardside
        || (j==2 && j1==2) // a real eye on the boardside
        || (j==1 && j1==3) // a real eye on the board-corner
        // the next three criterias are set for the intelligent random simulation.
        || (h1<=1 && j1==0) // not necessarily a realy eye
        || (j==1 && j2==1 && j1==2) // not necessarily a real eye on the boardside//!!!here, probably not an eye!!!!!
        || (j2==1 && j1==3) // not necessarily a real eye on the boardside
      ) return EYEFILLMOVE;//in this three cases, the stones adjacent belong to the same string. This helps the stringEyeNumber calculation.
      //if ((j+j2>=3 && j1==0) || (j1==2 && j+j2==2) || (j1==3 && j2==1)) return A_S_USELESS;

      return 0;
    }
    //adjc=1 only one liberty adjacent
    if (enemyAdjacentStringLocation.size()>0) {
      if ((h1>=3 && j1==0) || (j1==2 && h1+j2==2) || (j1==3 && (h1==1 || j2==1)))
        return MOVECHANGED;//changed to the first(and in fact the only) location in freeAdjacentLocation
      else
        return 0;
    }
    // now, adjacent stones are of the same color...
    if (j1==2 && h1==0)  return MOVECHANGED;
    if (j1==0 && h1<=1) return MOVECHANGED;//not absolutely sure...
    if ((j<3 && j1==0) || j1==2 || (j1==3 && j2==1) || (j1==3 && h1==1)) return 0;
    //  cout << Goban[loc-1-WIDTH] << " " << Goban[loc+1-WIDTH] << " " << Goban[loc-1+WIDTH] << " " << Goban[loc+1+WIDTH] << "\n";
    //FIXME well, what else is left? I am puzzled too!!
    return MOVECHANGED;
  }
  ////////////////////////////////////////////////////////////
  else if (MoGo::Goban::notUselessMode == 1) {/**assert(0);**/
    if (deadAdjacentStringLocation.size()>0) return 0;
    if (freeAdjacentLocation.size()>0) return 0;
    if (enemyAdjacentStringLocation.size()>0) return 0;//only friend stones surrounded.
    //now, necessary connecting move or useless(selfkill) eyefill move.
    int j = 0;
    for(int i=4;i<8;i++)
      if ( goban->gobanState[location+goban->direction(i)] == 3-goban->playerColor ) j++;
    if (j>1) return 0;
    if (j == 0) return EYEFILLMOVE;
    else if (isOnGobanSide(location)) return 0;
    return EYEFILLMOVE;
  }
  ////////////////////////////////////////////////////////////
  else  if (MoGo::Goban::notUselessMode == 2) {/**assert(0);**/
    if (deadAdjacentStringLocation.size()>0) return 0;
    if (freeAdjacentLocation.size()>0) return 0;//more than one free intersection
    if (enemyAdjacentStringLocation.size()>0) return 0;//only friend stones surrounded.
    //now, necessary connecting move or useless(selfkill) eyefill move.
    int j = 0,j1 = 0,j2 = 0;
    for(int i=4;i<8;i++)
      if ( goban->gobanState[location+goban->direction(i)] == 3-goban->playerColor ) j++;
      else if (goban->gobanState[location+goban->direction(i)] == OUTSIDE) j1++;
      else if (goban->gobanState[location+goban->direction(i)] == goban->playerColor ) j2++;
    if (j>1) return 0;
    if (j1 >=2) {/**assert(0);**/
      if (j2!=0) return 0;

    }
    if (j == 0) return EYEFILLMOVE;
    if (j1 == 0) return EYEFILLMOVE;
    return 0;

  }
  ////////////////////////////////////////////////////////////
  else if (MoGo::Goban::notUselessMode == 3) {/**assert(0);**/
    if (deadAdjacentStringLocation.size()>0) return 0;
    if (freeAdjacentLocation.size()>0) return 0;//more than one free intersection
    if (enemyAdjacentStringLocation.size()>0) return 0;//only friend stones surrounded.
    //now, necessary connecting move or useless(selfkill) eyefill move.
    int j = 0;
    for(int i=4;i<8;i++)
      if ( goban->gobanState[location+goban->direction(i)] == 3-goban->playerColor ) j++;
    if (j>1) return 0;
    if (j == 0) return EYEFILLMOVE;
    else if (isOnGobanSide(location)) return 0;
    return EYEFILLMOVE;
  } else if (MoGo::Goban::notUselessMode == 4) {/**assert(0);**/
    //if (location==PASSMOVE) return LEGAL; //TODO where should we leave this passmove test?
    if (deadAdjacentStringLocation.size()>0) return 0;
    if (freeAdjacentLocation.size()>1) return 0;//more than one free intersection
    if (enemyAdjacentStringLocation.size()>0 && friendAdjacentStringLocation.size()>0) return 0;//no or zero free intersection adjacent, different color stones surrounded
    //now, at most one free intersection, surrounded by the SAME COLOR stones.
    for(int i=0;i<(int)enemyAdjacentStringLocation.size();i++)
      if (stringInformation->getStringLiberty(enemyAdjacentStringLocation[i])==2) return 0;//atari move FIXME here we can utilize...
    int j=0,j1=0,j2=0,h1=0;
    for(int i=4;i<8;i++) {/**assert(0);**/
      int state=goban->gobanState[location+goban->direction(i)];
      if (state==EMPTY) j2++;
      else if (state==OUTSIDE) j1++;
      else if (state==goban->playerColor) j++;
      else h1++;
    }
    //Cout << "j,j1,j2,h1=" << j << " " << j1 << " " << j2 <<" " << h1 << "\n";
    //j: corner taken by friend stone, j1:corner outside goban, j2: corner free
    //h1:corner taken by different color stone
    if (freeAdjacentLocation.size()==0) //thus by the hypothese before, 4 adjacent locations are friends
    {/**assert(0);**/
      if (
        (j>=3 && j1==0) //a real eye not on the boardside
        || (j==2 && j1==2) // a real eye on the boardside
        || (j==1 && j1==3) // a real eye on the board-corner
        // the next three criterias are set for the intelligent random simulation.
        || (h1<=1 && j1==0) // not necessarily a realy eye
        || (j==1 && j2==1 && j1==2) // not necessarily a real eye on the boardside//!!!here, probably not an eye!!!!!
        || (j2==1 && j1==3) // not necessarily a real eye on the boardside
      ) return EYEFILLMOVE;//in this three cases, the stones adjacent belong to the same string. This helps the stringEyeNumber calculation.
      //if ((j+j2>=3 && j1==0) || (j1==2 && j+j2==2) || (j1==3 && j2==1)) return A_S_USELESS;

      return 0;
    }
    //adjc=1 only one liberty adjacent
    if (enemyAdjacentStringLocation.size()>0) {/**assert(0);**/
      if ((h1>=3 && j1==0) || (j1==2 && h1+j2==2) || (j1==3 && (h1==1 || j2==1)))
        return 0;//changed to the first(and in fact the only) location in freeAdjacentLocation
      else
        return 0;
    }
    // now, adjacent stones are of the same color...
    if (j1==2 && h1==0)  return 0;
    if (j1==0 && h1<=1) return 0;//not absolutely sure...
    if ((j<3 && j1==0) || j1==2 || (j1==3 && j2==1) || (j1==3 && h1==1)) return 0;
    //  cout << Goban[loc-1-WIDTH] << " " << Goban[loc+1-WIDTH] << " " << Goban[loc-1+WIDTH] << " " << Goban[loc+1+WIDTH] << "\n";
    //FIXME well, what else is left? I am puzzled too!!
    return 0;
  }  else if (MoGo::Goban::notUselessMode == 5) {/**assert(0);**/
    //if (location==PASSMOVE) return LEGAL; //TODO where should we leave this passmove test?
    if (deadAdjacentStringLocation.size()>0) return 0;
    if (freeAdjacentLocation.size()>1) return 0;//more than one free intersection
    if (enemyAdjacentStringLocation.size()>0 && friendAdjacentStringLocation.size()>0) return 0;//no or zero free intersection adjacent, different color stones surrounded
    //now, at most one free intersection, surrounded by the SAME COLOR stones.
    for(int i=0;i<(int)enemyAdjacentStringLocation.size();i++)
      if (stringInformation->getStringLiberty(enemyAdjacentStringLocation[i])==2) return 0;//atari move FIXME here we can utilize...
    int j=0,j1=0,j2=0,h1=0;
    for(int i=4;i<8;i++) {/**assert(0);**/
      int state=goban->gobanState[location+goban->direction(i)];
      if (state==EMPTY) j2++;
      else if (state==OUTSIDE) j1++;
      else if (state==goban->playerColor) j++;
      else h1++;
    }
    //Cout << "j,j1,j2,h1=" << j << " " << j1 << " " << j2 <<" " << h1 << "\n";
    //j: corner taken by friend stone, j1:corner outside goban, j2: corner free
    //h1:corner taken by different color stone
    if (freeAdjacentLocation.size()==0) //thus by the hypothese before, 4 adjacent locations are friends
    {/**assert(0);**/
      if (
        (j>=3 && j1==0) //a real eye not on the boardside
        || (j==2 && j1==2) // a real eye on the boardside
        || (j==1 && j1==3) // a real eye on the board-corner
        // the next three criterias are set for the intelligent random simulation.
        || (h1<=1 && j1==0) // not necessarily a realy eye
        || (j==1 && j2==1 && j1==2) // not necessarily a real eye on the boardside//!!!here, probably not an eye!!!!!
        || (j2==1 && j1==3) // not necessarily a real eye on the boardside
      ) return EYEFILLMOVE;//in this three cases, the stones adjacent belong to the same string. This helps the stringEyeNumber calculation.
      //if ((j+j2>=3 && j1==0) || (j1==2 && j+j2==2) || (j1==3 && j2==1)) return A_S_USELESS;

      return 0;
    }
    //adjc=1 only one liberty adjacent
    if (enemyAdjacentStringLocation.size()>0) {/**assert(0);**/
      if ((h1>=3 && j1==0) || (j1==2 && h1+j2==2) || (j1==3 && (h1==1 || j2==1)))
        return EYEFILLMOVE;//changed to the first(and in fact the only) location in freeAdjacentLocation
      else
        return 0;
    }
    // now, adjacent stones are of the same color...
    if (j1==2 && h1==0)  return EYEFILLMOVE;
    if (j1==0 && h1<=1) return EYEFILLMOVE;//not absolutely sure...
    if ((j<3 && j1==0) || j1==2 || (j1==3 && j2==1) || (j1==3 && h1==1)) return 0;
    //  cout << Goban[loc-1-WIDTH] << " " << Goban[loc+1-WIDTH] << " " << Goban[loc-1+WIDTH] << " " << Goban[loc+1+WIDTH] << "\n";
    //FIXME well, what else is left? I am puzzled too!!
    return EYEFILLMOVE;
  } else assert(0);

  return 0;
}

bool MoGo::LocationInformation::selfAtariTest( const Location , const int , const int liberty2, const int liberty3 ) const {/**assert(0);**/
  if (deadAdjacentStringLocation.size()>0) return false;
  if (liberty3>0) return false;
  //if (friendAdjacentStringLocation.size()==0) return;
  //if (friendAdjacentStringLocation.size()==1 && stringInformation->stringLiberty[stringInformation->stringNumber[friendAdjacentStringLocation[0]]]==1) return;
  //if (freeAdjacentLocation.size()>=2) return;
  if ( liberty2>0 && freeAdjacentLocation.size()==0) {/**assert(0);**/
    int test = 0;
    int length=stringInformation->stringLength[stringInformation->stringNumber[friendAdjacentStringLocation[0]]];
    if ((int)friendAdjacentStringLocation.size()!=1) {/**assert(0);**/
      for (int i=0;i<(int)friendAdjacentStringLocation.size()-1;i++) {/**assert(0);**/
        if (stringInformation->stringNumber[friendAdjacentStringLocation[i]]!=stringInformation->stringNumber[friendAdjacentStringLocation[friendAdjacentStringLocation.size()-1]]) test = 1;
        length+=stringInformation->stringLength[stringInformation->stringNumber[friendAdjacentStringLocation[i]]];
      }
      if (test == 1) return false;
    }
    //if (length<5) return false;
    //GoTools::print("self Atari move tested!\n");
    //goban->textModeShowGoban();
    //goban->textModeShowPosition(location);
    //getchar();
    return true;
  }
  return false;

}








// TEMPORARY COMMENT: THIS FUNCTION IS USED
// TEMPORARY COMMENT: BLACK PLAYS 2 TIMES
int MoGo::LocationInformation::isSelfAtari( const Location & location) const {
  //   if (goban->gobanState[location]!=EMPTY) return 0;
  int freeL=0;//, nbOpponentStones=0;
  for (int i=0;i<4;i++) {
    //     if (goban->gobanState[location+goban->direction(i)]==0) {/**assert(0);**/
    //       if (freeL == 0) firstFreeLocation = location+goban->direction(i);
    freeL +=goban->gobanState[location+goban->direction(i)]==0;
    //     }
    //     nbOpponentStones+=goban->gobanState[location+goban->direction(i)]==3-goban->playerColor;
  }
  // GoTools::print("for location free %i\n", freeL); goban->textModeShowPosition(location);

  /*  static int totalCalls=0;
    static int startFree=0;
    totalCalls++;*/
  if (freeL>=2) {
    
    return 0;
  }
  //   if (nbOpponentStones>=3)
  //     return 1;



  int free = 0;
  int length = 1;
  Location firstFreeLocation = PASSMOVE;
  const FastSmallVector<int> &stringNumber=stringInformation->stringNumber;
  adjacentFriendTwoLibertiesStrings.clear();
  //   const FastSmallVector<int> &stringNumber=stringInformation->stringNumber;
  for (int i=0;i<4;i++) {
    Location tmp=location+goban->direction(i);
    int state=goban->gobanState[tmp]; int strNumber=stringNumber[tmp];
    if (state==EMPTY) {
      if (free == 0) firstFreeLocation = tmp;
      free++;
      if (free == 2) return 0;
    } else if (state==goban->playerColor) { // friend
      int liberties=stringInformation->stringLiberty[strNumber];//stringInformation->getStringLiberty(tmp);
      if (liberties>=3)
        return 0;
      if (liberties<=1) {
        length+=stringInformation->stringLength[strNumber];//stringInformation->getStringLength(tmp); // TODO length can be false!!!
        continue;
      }
      bool found=false;
      for (int j=0;j<(int)adjacentFriendTwoLibertiesStrings.size();j++)
        if (adjacentFriendTwoLibertiesStrings[j]==strNumber)
          found=true;
      if (found) continue;
      length+=stringInformation->stringLength[strNumber];//stringInformation->getStringLength(tmp);
      adjacentFriendTwoLibertiesStrings.push_back(strNumber);
    } else if (state == 3-goban->playerColor &&
               stringInformation->stringLiberty[strNumber] == 1) {
      if (isSelfAtariMode>=2 && freeL==0)
        return stringInformation->stringLiberty[strNumber];
      else
        return 0;// Yizao thinks it is good to do that. I trust him :-)

      /*      if (stringInformation->stringLength[strNumber]>=2) // TODO
              return 0;
            if (free == 0) firstFreeLocation = tmp;
            free++;
            if (free == 2) return 0;*/
    }
  }

  if ((free==1 && adjacentFriendTwoLibertiesStrings.size() == 0)
      || (free==0 && adjacentFriendTwoLibertiesStrings.size() == 1))
    return length;
  if (free==1) {
    assert(firstFreeLocation!=PASSMOVE);
    for (int i=0;i<(int)adjacentFriendTwoLibertiesStrings.size();i++)
      for (int j=0;j<4;j++)
        if (stringNumber[firstFreeLocation+goban->direction(j)]==adjacentFriendTwoLibertiesStrings[i])
          return length;
    return 0;

    //       if (!stringInformation->isOneLibertyOfString(firstFreeLocation, adjacentFriendTwoLibertiesStrings[i]))
    //         return 0;
    return length;
  }
  // return 0;

 
  for (int i=0;i<(int)adjacentFriendTwoLibertiesStrings.size()-1;i++) {
    if (!stringInformation->areTwoStringsSharingTheSameTwoLiberties(adjacentFriendTwoLibertiesStrings[i],adjacentFriendTwoLibertiesStrings[i+1]))
      return 0;
  }
  //   return 0;
  return length;
}


// TEMPORARY COMMENT: THIS FUNCTION IS USED
// TEMPORARY COMMENT: BLACK PLAYS 2 TIMES
bool MoGo::LocationInformation::isCut( const Location & location/*, double & blackInterest, double & whiteInterest*/ ) const {

  for (int i=0;i<4;i++) {
    if (goban->gobanState[location+goban->direction(i)]==goban->playerColor
        && goban->gobanState[location+goban->direction((i+2)%4)]==goban->playerColor
        && goban->gobanState[location+goban->direction((i+1)%4)]==3-goban->playerColor
        && goban->gobanState[location+goban->direction((i+3)%4)]==3-goban->playerColor)
      return true;
  }



  //GoTools::print("!!");
  // XO*
  // O.*
  // ***
  for (int i=0;i<4;i++) {
    //     const Vector<int> &directionTRotationI=directionT[rotation[i]];
    const int * directionTRotationI=directionT[rotation[i]];
    if (goban->gobanState[location+directionTRotationI[NORTHWEST_]]+goban->gobanState[location+directionTRotationI[NORTH_]]==3 && goban->gobanState[location+directionTRotationI[NORTH_]]==goban->gobanState[location+directionTRotationI[WEST_]])
      if (goban->getStringLiberty(location+directionTRotationI[NORTHWEST_]) != 1) {
        //GoTools::print("!");
        //XO* XO*
        //O.O O..
        //*.* *O*
        if ((goban->gobanState[location+directionTRotationI[EAST_]] == goban->gobanState[location+directionTRotationI[NORTH_]] &&  goban->gobanState[location+directionTRotationI[SOUTH_]] == EMPTY) ||
            (goban->gobanState[location+directionTRotationI[SOUTH_]] == goban->gobanState[location+directionTRotationI[NORTH_]] &&  goban->gobanState[location+directionTRotationI[EAST_]] == EMPTY))
          ;
        else
          return true;
      }
  }
  return false;
 




 

  
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
// TEMPORARY COMMENT: BLACK PLAYS 2 TIMES
bool MoGo::LocationInformation::isHane( const Location & location/*, double , double */) const {

  

  int taken = 0;
  for (int i=0;i<4;i++)
    if (goban->gobanState[location+goban->direction(i)] != EMPTY) {
      taken++;
    }
  if (taken >= 3 || taken == 0) return false;
  if (taken == 1) {
    for (int i=0;i<8;i++)
      //  XO*
      //  ...
      //  *.*
      if ( goban->gobanState[location+directionT[i][NORTH_]] != EMPTY &&
           goban->gobanState[location+directionT[i][NORTHWEST_]]+goban->gobanState[location+directionT[i][NORTH_]] == 3)
        //  XOX
        //  ...
        //  ***
        if (goban->gobanState[location+directionT[i][NORTHEAST_]] == goban->gobanState[location+directionT[i][NORTHWEST_]])
          return true;
    //  XO.
    //  ...
    //  *.*
        else if (goban->gobanState[location+directionT[i][NORTHEAST_]] == EMPTY)
          return true;
    //  XOO
    //  ...
    //  *.*
        else if (goban->playerColor == goban->gobanState[location+directionT[i][NORTH_]])
          return false;
        else
          return true;
    return false;
  } else { //taken == 2
    for (int i=0;i<8;i++)
      //  XO*
      //  *..
      //  *.*
      if ( goban->gobanState[location+directionT[i][EAST_]] == EMPTY &&
           goban->gobanState[location+directionT[i][SOUTH_]] == EMPTY &&
           goban->gobanState[location+directionT[i][NORTHWEST_]] +
           goban->gobanState[location+directionT[i][NORTH_]] == 3 )
        //  XO*
        //  O..
        //  *.*
        if (goban->gobanState[location+directionT[i][WEST_]] ==
            goban->gobanState[location+directionT[i][NORTH_]])
          return false;
    //  XO*
    //  X..
    //  *.*
        else /*if (goban->gobanState[location+directionT[i][WEST_]] == goban->gobanState[location+directionT[i][NORTHWEST_]])*/
          return true;
    return false;
  }
  /*
   
    for (int i=0;i<8;i++)
      //  XO*
      //  *..
      //  *.*
      if ( goban->gobanState[location+directionT[i][EAST_]] == EMPTY &&
           goban->gobanState[location+directionT[i][SOUTH_]] == EMPTY &&
           goban->gobanState[location+directionT[i][NORTHWEST_]] +
           goban->gobanState[location+directionT[i][NORTH_]] == 3 )
        //  XO*
        //  O..
        //  *.*
        if (goban->gobanState[location+directionT[i][WEST_]] ==
            goban->gobanState[location+directionT[i][NORTH_]])
          return false;
    //  XO*
    //  X..
    //  *.*
        else if (goban->gobanState[location+directionT[i][WEST_]] == goban->gobanState[location+directionT[i][NORTHWEST_]])
          return true;
    //  XO*
    //  ...
    //  *.*
        else if (goban->gobanState[location+directionT[i][WEST_]] == EMPTY)
          //  XOX
          //  ...
          //  ***
          if (goban->gobanState[location+directionT[i][NORTHEAST_]] == goban->gobanState[location+directionT[i][NORTHWEST_]])
            return true;
    //  XO.
    //  ...
    //  *.*
          else if (goban->gobanState[location+directionT[i][NORTHEAST_]] == EMPTY)
            return true;
    //  XOO
    //  ...
    //  *.*
          else if (goban->playerColor == goban->gobanState[location+directionT[i][NORTH_]])
            return false;
          else
            return true;
   
    return false;
  */

  /*
    for (int i=0;i<8;i++)
      //  XO*
      //  *.*
      //  ***
      if (goban->gobanState[location+directionT[i][NORTHWEST_]] +
          goban->gobanState[location+directionT[i][NORTH_]] == 3)
        //  XO*
        //  O.*
        //  ***
        if (goban->gobanState[location+directionT[i][WEST_]] ==
            goban->gobanState[location+directionT[i][NORTH_]])
          return false;
    //  XO*
    //  *.O
    //  ***
        else if (goban->gobanState[location+directionT[i][EAST_]] ==
                 goban->gobanState[location+directionT[i][NORTH_]])
          return false;
    //  XO*
    //  *.*
    //  *O*
        else if (goban->gobanState[location+directionT[i][SOUTH_]] ==
                 goban->gobanState[location+directionT[i][NORTH_]])
          return false;
   
    //  XO*
    //  ..*
    //  ***
        else if (goban->gobanState[location+directionT[i][WEST_]] == 0)
          return true;
    //  XO*
    //  X.*
    //  ***
        else if (goban->gobanState[location+directionT[i][WEST_]] ==
                 goban->gobanState[location+directionT[i][NORTHWEST_]])
          return true;
    return false;
    
    */
}

bool MoGo::LocationInformation::isCrossCut( const Location & location/*, double & blackInterest, double & whiteInterest*/ ) const {/**assert(0);**/

  if (goban->gobanState[location+directionT[0][WEST_]]+goban->gobanState[location+directionT[0][EAST_]]==3 ||
      goban->gobanState[location+directionT[0][NORTH_]]+goban->gobanState[location+directionT[0][SOUTH_]]==3)
    return false;

  // *O*
  // X.X
  // YYY (Y : not X)
  for (int i=0;i<8;i+=2)
    if (goban->gobanState[location+directionT[i][WEST_]]==goban->gobanState[location+directionT[i][EAST_]] &&
        goban->gobanState[location+directionT[i][WEST_]]+goban->gobanState[location+directionT[i][NORTH_]]==3 &&
        goban->gobanState[location+directionT[i][SOUTHWEST_]]!=goban->gobanState[location+directionT[i][WEST_]] &&
        goban->gobanState[location+directionT[i][SOUTH_]]!=goban->gobanState[location+directionT[i][WEST_]] &&
        goban->gobanState[location+directionT[i][SOUTHEAST_]]!=goban->gobanState[location+directionT[i][WEST_]]) {/**assert(0);**/
      //blackInterest += 3.;
      //whiteInterest += 3.;
      return true;
    }
  return false;
}

bool MoGo::LocationInformation::isSolid( const Location & location ) const {/**assert(0);**/
  int t = 0;
  for (int i=0;i<4;i++) {/**assert(0);**/
    t+=goban->gobanState[location+goban->direction(i)];
    if (t>3) return false;
  }
  if (t<3) return false;
  for (int i=0;i<8;i++)
    // .XY
    // O..
    // ?.?
    if (goban->gobanState[location+directionT[i][NORTH_]]+goban->gobanState[location+directionT[i][WEST_]] == 3 &&
        goban->gobanState[location+directionT[i][NORTHWEST_]] == EMPTY &&
        goban->gobanState[location+directionT[i][EAST_]] == EMPTY &&
        //goban->gobanState[location+directionT[i][NORTHEAST_]] != goban->gobanState[location+directionT[i][NORTH_]])//FIXME TODO  here a bad pattern might be introduced.
        goban->gobanState[location+directionT[i][NORTHEAST_]] == 3-goban->gobanState[location+directionT[i][NORTH_]])
      return true;
  return false;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::LocationInformation::createRotationArray( ) {
  //    directionT.resize(8); for (int i=0;i<8;i++)directionT[i].resize(9);
  //    rotation.resize(4);

  //  5 1 4
  //  0 8 2
  //  6 3 7

  directionT[0][0]=0;directionT[0][1]=1;directionT[0][2]=2; //  N
  directionT[0][3]=3;directionT[0][4]=4;directionT[0][5]=5; // W E
  directionT[0][6]=6;directionT[0][7]=7;directionT[0][8]=8; //  S

  directionT[1][0]=2;directionT[1][1]=1;directionT[1][2]=0; //  N
  directionT[1][3]=5;directionT[1][4]=4;directionT[1][5]=3; // E W
  directionT[1][6]=8;directionT[1][7]=7;directionT[1][8]=6; //  S

  directionT[2][0]=0;directionT[2][1]=3;directionT[2][2]=6; //  W
  directionT[2][3]=1;directionT[2][4]=4;directionT[2][5]=7; // N
  directionT[2][6]=2;directionT[2][7]=5;directionT[2][8]=8;

  directionT[3][0]=2;directionT[3][1]=5;directionT[3][2]=8; //  W
  directionT[3][3]=1;directionT[3][4]=4;directionT[3][5]=7; //   N
  directionT[3][6]=0;directionT[3][7]=3;directionT[3][8]=6; //

  directionT[4][0]=6;directionT[4][1]=7;directionT[4][2]=8; //
  directionT[4][3]=3;directionT[4][4]=4;directionT[4][5]=5; // W
  directionT[4][6]=0;directionT[4][7]=1;directionT[4][8]=2; //  N

  directionT[5][0]=8;directionT[5][1]=7;directionT[5][2]=6; //
  directionT[5][3]=5;directionT[5][4]=4;directionT[5][5]=3; //   W
  directionT[5][6]=2;directionT[5][7]=1;directionT[5][8]=0; //  N

  directionT[6][0]=8;directionT[6][1]=5;directionT[6][2]=2; //
  directionT[6][3]=7;directionT[6][4]=4;directionT[6][5]=1; //   N
  directionT[6][6]=6;directionT[6][7]=3;directionT[6][8]=0; //  W

  directionT[7][0]=6;directionT[7][1]=3;directionT[7][2]=0; //
  directionT[7][3]=7;directionT[7][4]=4;directionT[7][5]=1; // N
  directionT[7][6]=8;directionT[7][7]=5;directionT[7][8]=2; //  W

  //  5 1 4
  //  0 8 2
  //  6 3 7

  int *sampleArray = new int[9];
  /*
    sampleArray[0]=-(goban->width+3);sampleArray[1]=-(goban->width+2);sampleArray[2]=-(goban->width+1);
    sampleArray[3]=-1;               sampleArray[4]=0;                sampleArray[5]=+1;
    sampleArray[6]=+(goban->width+1);sampleArray[7]=+(goban->width+2);sampleArray[8]=+(goban->width+3);
  */
  sampleArray[0]=LEFTUP_;  sampleArray[1]=UP_;  sampleArray[2]=RIGHTUP_;
  sampleArray[3]=LEFT_;    sampleArray[4]=0;   sampleArray[5]=RIGHT_;
  sampleArray[6]=LEFTDOWN_;sampleArray[7]=DOWN_;sampleArray[8]=RIGHTDOWN_;

  for (int i=0;i<8;i++)
    for (int j=0;j<9;j++)
      if (j==4) directionT[i][j]=0;
      else directionT[i][j]=goban->direction(sampleArray[directionT[i][j]]);


  delete [] sampleArray ;

  rotation[0]=0;
  /*  rotation[1]=1;//Warning!!! this rotation is for the left-up-corner, especially for the cut case.
    rotation[2]=4;
    rotation[3]=5;*/
  rotation[1]=3;
  rotation[2]=5;
  rotation[3]=7;

}

bool MoGo::LocationInformation::isNotGoodForOpening( const Location & location/*, double &blackInterest , double &whiteInterest*/ ) const {/**assert(0);**/
  if (goban->heightOfGoban == 9) return isNotGoodForOpening9(location);
  else return isNotGoodForOpening13(location);
}
bool MoGo::LocationInformation::isNotGoodForOpening9( const Location & location/*, double &blackInterest , double &whiteInterest*/ ) const {/**assert(0);**/
  int friendStones = 0, enemyStones = 0;

  for (int i=0;i<8;i++)
    if (goban->gobanState[location+goban->direction(i)]==goban->playerColor)
      friendStones++;
    else if (goban->gobanState[location+goban->direction(i)]==3-goban->playerColor)
      enemyStones++;
  if (friendStones>enemyStones+1) {/**assert(0);**/
    //goban->textModeShowPosition(location);
    //GoTools::print("too many friend stones(%d,%d)!\n",friendStones,enemyStones);//FIXME RANGE
    return true;
  }
  if (friendStones && enemyStones == 0) return true;
  if (isInterestingMove(location/*,blackInterest,whiteInterest*/)) return false;
  if (isUgly(location/*,blackInterest,whiteInterest*/))
    return true;
  return false;

}

bool MoGo::LocationInformation::isNotGoodForOpening13( const Location & location/*, double &blackInterest , double &whiteInterest*/ ) const {/**assert(0);**/
  int friendStones = 0, enemyStones = 0;
  int friendStones1 = 0, enemyStones1 = 0;

  if (isAtari(location)) return false;
  for (int i=0;i<8;i++)
    if (goban->gobanState[location+goban->direction(i)]==goban->playerColor)
      friendStones++;
    else if (goban->gobanState[location+goban->direction(i)]==3-goban->playerColor)
      enemyStones++;
  //if (friendStones && enemyStones == 0) return true;
  for (int i=8;i<12;i++)
    if (goban->gobanState[location+goban->direction(i)]==goban->playerColor)
      friendStones1++;
    else if (goban->gobanState[location+goban->direction(i)]==3-goban->playerColor)
      enemyStones1++;

  //if (friendStones+friendStones1 && enemyStones+enemyStones1 == 0) return true;
  //if (friendStones+friendStones1 == 0 && enemyStones+enemyStones1 == 0) return false;
  if (enemyStones+enemyStones1 == 0) return friendStones+friendStones1;
  if (friendStones1+enemyStones1 == 0 && isInterestingMove(location/*,blackInterest,whiteInterest*/)) return false;

  if (isUgly(location/*,blackInterest,whiteInterest*/)) return true;
  return false;
  //return true;

}





bool MoGo::LocationInformation::isUgly( const Location & location/*, double & , double & */) const {/**assert(0);**/
  for (int i=0;i<8;i+=2)
    // YOY
    // ...
    // ???
    if (goban->gobanState[location+directionT[i][WEST_]]+goban->gobanState[location+directionT[i][EAST_]] == 0 &&
        goban->gobanState[location+directionT[i][NORTHWEST_]]!=3-goban->gobanState[location+directionT[i][NORTH_]] &&
        goban->gobanState[location+directionT[i][NORTHEAST_]]!=3-goban->gobanState[location+directionT[i][NORTH_]])
      if (goban->gobanState[location+directionT[i][NORTH_]] == goban->playerColor) return true;
  // YOY
  // ...
  // PXP (P : not O)
  /*    else if (goban->gobanState[location+directionT[i][NORTH_]]+goban->gobanState[location+directionT[i][SOUTH_]] == 3 &&
               goban->gobanState[location+directionT[i][SOUTHWEST_]]!=goban->gobanState[location+directionT[i][NORTH_]] &&
               goban->gobanState[location+directionT[i][SOUTHEAST_]]!=goban->gobanState[location+directionT[i][NORTH_]] )
        return true;
      // YOY                YOY
      // ...                ...
      // X.. (P : not O)    ..X              O to play
        else if
        (goban->playerColor == goban->gobanState[location+directionT[i][NORTH_]] &&
            goban->gobanState[location+directionT[i][SOUTH_]]==EMPTY &&
            (goban->gobanState[location+directionT[i][SOUTHWEST_]]==3-goban->gobanState[location+directionT[i][NORTH_]] ||
             goban->gobanState[location+directionT[i][SOUTHEAST_]]==3-goban->gobanState[location+directionT[i][NORTH_]]) &&
            goban->gobanState[location+directionT[i][SOUTHEAST_]] != goban->gobanState[location+directionT[i][SOUTHEAST_]])
          return true;
   
    // YOY
    // ...
    // ...                              O to play
  else if (goban->gobanState[location+directionT[i][NORTH_]] == goban->playerColor &&
          goban->gobanState[location+directionT[i][WEST_]]+goban->gobanState[location+directionT[i][EAST_]] == 0 &&
          goban->gobanState[location+directionT[i][SOUTHWEST_]] == EMPTY &&
          goban->gobanState[location+directionT[i][SOUTH_]] == EMPTY &&
          goban->gobanState[location+directionT[i][SOUTHEAST_]] == EMPTY)
        return true;
  */

  for (int i=0;i<8;i++)
    // .O*
    // O.*
    // ***
    if (goban->gobanState[location+directionT[i][NORTH_]]==goban->gobanState[location+directionT[i][WEST_]] &&
        goban->gobanState[location+directionT[i][NORTH_]] != EMPTY &&
        goban->gobanState[location+directionT[i][NORTHWEST_]] == 0)
      return true;
  // OOY
  // Y.Y
  // ???             X to play
  for (int i = 0;i<8;i+=2)
    if (goban->gobanState[location+directionT[i][NORTH_]] == 3-goban->playerColor &&
        (goban->gobanState[location+directionT[i][NORTHWEST_]] == 3-goban->playerColor ||
         goban->gobanState[location+directionT[i][NORTHEAST_]] == 3-goban->playerColor) &&
        goban->gobanState[location+directionT[i][NORTHWEST_]] != goban->playerColor &&
        goban->gobanState[location+directionT[i][NORTHEAST_]] != goban->playerColor &&
        goban->gobanState[location+directionT[i][EAST_]] != goban->playerColor &&
        goban->gobanState[location+directionT[i][WEST_]] != goban->playerColor )
      return true;

  return false;
}



bool MoGo::LocationInformation::isTaken( const Location & location ) const {/**assert(0);**/
  if (goban->gobanState[location]!=EMPTY) return true;
  return false;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
// TEMPORARY COMMENT: BLACK PLAYS 2 TIMES
bool MoGo::LocationInformation::isOnGobanSide( const Location & location ) const {
  if (!goban->isOutsideStrange) {
    int x = GobanLocation::xLocation(location);
    int y = GobanLocation::yLocation(location);

    if (x==0 || y==0 || x==goban->heightOfGoban-1 || y==goban->widthOfGoban-1) return true;
    return false;
  } else
    for (int i=0;i<8;i++)
      if (goban->gobanState[location+goban->direction(i)] == OUTSIDE) return true;
  return false;

}

bool MoGo::LocationInformation::isIsolatedMove( const Location & location ) const {/**assert(0);**/
  //int friendStones = 0;
  for (int i=0;i<8;i++)
    if (goban->gobanState[location+goban->direction(i)]==3-goban->playerColor)
      return false;
  //    else if(goban->gobanState[location+goban->direction(i)]==goban->playerColor)
  //      friendStones++;
  //  return !(friendStones);
  return true;
}

bool MoGo::LocationInformation::isTooSlow( const Location & location/* , double & , double &*/ ) const {/**assert(0);**/
  int friendStones = 0;
  for (int i=0;i<8;i++)
    if (goban->gobanState[location+goban->direction(i)]==goban->playerColor)
      friendStones++;
    else if (goban->gobanState[location+goban->direction(i)]==3-goban->playerColor)
      return false;
  return friendStones;
}

bool MoGo::LocationInformation::isAtariSavingMove( const Location & location ) const {/**assert(0);**/
  //goban->textModeShowPosition(location);GoTools::print("!!!!!!\n");
  if (goban->playerColor == BLACK) {/**assert(0);**/
    if (goban->gobanLists.whiteEatingLocation.isLocationIn(location))
      return true;
  } else {/**assert(0);**/
    if (goban->gobanLists.blackEatingLocation.isLocationIn(location))
      return true;
  }
  return false;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
// TEMPORARY COMMENT: BLACK PLAYS 2 TIMES
bool MoGo::LocationInformation::isAtariSavingMoveTrue( const Location & location ) const {
  if( !isSelfAtari(location) &&
      (!isOnGobanSide(location) ||
       isConnectingMove(location)))
    return true;
  return false;
}



// TEMPORARY COMMENT: THIS FUNCTION IS USED
// TEMPORARY COMMENT: BLACK PLAYS 2 TIMES
Location MoGo::LocationInformation::isAtari( const Location & location ) const {
  for (int i=0;i<4;i++)
    if ( goban->gobanState[location+goban->direction(i)] == 3-goban->playerColor )
      if (goban->getStringLiberty(location+goban->direction(i)) == 2 /*&& goban->getStringLength(location+goban->direction(i))>1*/) return location+goban->direction(i);

  return 0;
}

bool MoGo::LocationInformation::isInterestingMove( const Location & location/*, double & blackInterest, double & whiteInterest*/ ) const {/**assert(0);**/
  if (isCut(location/*,blackInterest,whiteInterest*/) ||
      isCrossCut(location/*,blackInterest,whiteInterest*/) ||
      isHane(location/*,blackInterest,whiteInterest*/) ||
      isAtari(location) ||
      isAtariSavingMove(location))
    return true;
  return false;
}

bool MoGo::LocationInformation::isEatingMove( const Location & location ) const {/**assert(0);**/
  for (int i=0;i<4;i++)
    if (goban->gobanState[location+goban->direction(i)] == 3-goban->playerColor &&
        goban->getStringLiberty(location+goban->direction(i)) == 1)
      return true;
  return false;
}

Location MoGo::LocationInformation::isLastMoveAtari( ) const {/**assert(0);**/
  if (goban->lastPlayedMove == PASSMOVE) return false;
  for (int i=0;i<4;i++)
    if (goban->gobanState[goban->lastPlayedMove+goban->direction(i)] == goban->playerColor &&
        goban->getStringLiberty(goban->lastPlayedMove+goban->direction(i)) == 1 ) {/**assert(0);**/
      //GoTools::print("last atari move:");goban->textModeShowPosition(goban->lastPlayedMove);
      //Location location =
      //GoTools::print("proposed saving move:");goban->textModeShowPosition(location);
      return stringInformation->getOneLibertyOfString(goban->lastPlayedMove+goban->direction(i));
    }
  return 0;

}

bool MoGo::LocationInformation::isLastMoveAtari( Location & underAtariStringFirstPosition ) const {/**assert(0);**/
  if (goban->lastPlayedMove == PASSMOVE) return false;
  for (int i=0;i<4;i++)
    if (goban->gobanState[goban->lastPlayedMove+goban->direction(i)] == goban->playerColor &&
        goban->getStringLiberty(goban->lastPlayedMove+goban->direction(i)) == 1 ) {/**assert(0);**/
      underAtariStringFirstPosition = goban->lastPlayedMove+goban->direction(i);
      return true;
    }
  return false;

}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
// TEMPORARY COMMENT: BLACK PLAYS 2 TIMES
bool MoGo::LocationInformation::isLastMoveAtari( FastSmallVector<Location> & underAtariStringsFirstPosition ) const {
  assert(underAtariStringsFirstPosition.size() == 0);
  if (goban->lastPlayedMove == PASSMOVE) return false;
  for (int i=0;i<4;i++)
    if (goban->gobanState[goban->lastPlayedMove+goban->direction(i)] == goban->playerColor &&
        goban->getStringLiberty(goban->lastPlayedMove+goban->direction(i)) == 1 ) {
      underAtariStringsFirstPosition.push_back(goban->lastPlayedMove+goban->direction(i));
      //return true;
    }
  return (underAtariStringsFirstPosition.size()>0);

}
bool MoGo::LocationInformation::isAtariNear( Location location, FastSmallVector<Location> & underAtariStringsFirstPosition ) const {/**assert(0);**/
  assert(underAtariStringsFirstPosition.size() == 0);
  if (location == PASSMOVE) return false;
  for (int i=0;i<4;i++)
    if (goban->gobanState[location+goban->direction(i)] == goban->playerColor &&
        goban->getStringLiberty(location+goban->direction(i)) == 1 ) {/**assert(0);**/
      underAtariStringsFirstPosition.push_back(location+goban->direction(i));
      //return true;
    }
  return (underAtariStringsFirstPosition.size()>0);

}
bool MoGo::LocationInformation::isBeforeAtariNear( Location location, FastSmallVector<Location> & underAtariStringsFirstPosition ) const {/**assert(0);**/
  assert(underAtariStringsFirstPosition.size() == 0);
  if (location == PASSMOVE) return false;
  for (int i=0;i<4;i++)
    if (goban->gobanState[location+goban->direction(i)] == goban->playerColor &&
        goban->getStringLiberty(location+goban->direction(i)) == 2 ) {/**assert(0);**/
      underAtariStringsFirstPosition.push_back(location+goban->direction(i));
      //return true;
    }
  return (underAtariStringsFirstPosition.size()>0);

}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
// TEMPORARY COMMENT: BLACK PLAYS 2 TIMES
bool MoGo::LocationInformation::isUnderAtariStringSavableByEatingMove( Location & underAtariStringFirstPosition, Location & savingMove ) const {
  /*
  if ((goban->playerColor == BLACK && !blackEatingLocation.size()) ||
      (goban->playerColor == WHITE && !whiteEatingLocation.size()))
    return false;
  */ //the used-to-be-ko locations, which might be eating locations, are not always saved in the eatingLocation. if FastMode works well, then optimize here. TODO
  StoneColor enemyColor = 3-goban->gobanState[underAtariStringFirstPosition];
  StringNumber strNb = stringInformation->stringNumber[underAtariStringFirstPosition];
  for (int i=0;i<(int)stringInformation->stringPositions[strNb].size();i++) {
    Location location = stringInformation->stringPositions[strNb][i];
    for (int j=0;j<4;j++)
      if (goban->gobanState[location+goban->direction(j)] == enemyColor &&
          stringInformation->stringLiberty[stringInformation->stringNumber[location+goban->direction(j)]] == 1) {
        savingMove = stringInformation->getOneLibertyOfString(location+goban->direction(j));
        if (savingMove == goban->ko) continue;
        return true;
      }
  }
  return false;
}

int MoGo::LocationInformation::isUnderBeforeAtariStringSavableByEatingMove( Location & underAtariStringFirstPosition, Location & savingMove ) const {/**assert(0);**/
  StoneColor enemyColor = 3-goban->gobanState[underAtariStringFirstPosition];
  StringNumber strNb = stringInformation->stringNumber[underAtariStringFirstPosition];
  for (int i=0;i<(int)stringInformation->stringPositions[strNb].size();i++) {/**assert(0);**/
    Location location = stringInformation->stringPositions[strNb][i];
    for (int j=0;j<4;j++)
      if (goban->gobanState[location+goban->direction(j)] == enemyColor &&
          stringInformation->stringLiberty[stringInformation->stringNumber[location+goban->direction(j)]] == 1) {/**assert(0);**/
        savingMove = stringInformation->getOneLibertyOfString(location+goban->direction(j));
        if (savingMove == goban->ko) continue;
        return goban->getStringLength(location+goban->direction(j));
      }
  }
  return 0;
}

bool MoGo::LocationInformation::isUnderAtariStringSavableByEatingMoveFastMode( Location & underAtariStringFirstPosition, Location & savingMove ) const {/**assert(0);**/

  LocationList *eatingLocation = 0;
  if (goban->playerColor == BLACK)
    eatingLocation = &goban->gobanLists.blackEatingLocation;
  else
    eatingLocation = &goban->gobanLists.whiteEatingLocation;
  if (!eatingLocation->size()) return false;
  StringNumber strNb = stringInformation->stringNumber[underAtariStringFirstPosition];
  if(stringInformation->stringLength[strNb] <= 2)
    return isUnderAtariStringSavableByEatingMove(underAtariStringFirstPosition,savingMove);

  for(int n=0;n<(int)eatingLocation->size();n++) {/**assert(0);**/
    StringNumber underAtariStrNb = 0 ;
    if (!getUnderAtariStringNumber((*eatingLocation)[n],3-goban->playerColor,underAtariStrNb)) {/**assert(0);**/
      eatingLocation->deleteLocation((*eatingLocation)[n]);
      n--;
      continue;
    }
    if (2*stringInformation->stringLength[underAtariStrNb] < stringInformation->stringLiberty[strNb] &&
        stringInformation->areTwoStringsAdjacent(underAtariStrNb,strNb))
      return (*eatingLocation)[n];
  }
  return isUnderAtariStringSavableByEatingMove(underAtariStringFirstPosition,savingMove);

}

bool MoGo::LocationInformation::getUnderAtariStringNumber( const Location & location, const StoneColor underAtariStringColor, StringNumber & strNb ) const {/**assert(0);**/
  if (goban->gobanState[location]!=EMPTY) return false;
  if (goban->ko==location) return false;
  for (int i=0;i<4;i++)
    if (goban->gobanState[location+goban->direction(i)]==underAtariStringColor)
      if ((goban->isStringInformationOn &&
           stringInformation->stringLiberty[stringInformation->stringNumber[location+goban->direction(i)]]==1) ||
          (goban->isCFGOn &&
           goban->cfg->getStringLiberty(goban->cfg->getStringNumber(location+goban->direction(i))) == 1)) {/**assert(0);**/
        if (goban->isStringInformationOn)
          strNb = stringInformation->stringNumber[location+goban->direction(i)];
        else if (goban->isCFGOn) strNb = goban->cfg->getStringNumber(location+goban->direction(i));
        else assert(0);
        return true;
      }
  return false;
}



// TEMPORARY COMMENT: THIS FUNCTION IS USED
// TEMPORARY COMMENT: BLACK PLAYS 2 TIMES
bool MoGo::LocationInformation::isConnectingMove( const Location & location ) const {
  int friendStringNumber = 0;
  for (int i=0;i<4;i++)
    if (goban->gobanState[location+goban->direction(i)] == goban->playerColor)
      if (friendStringNumber == 0) {
        friendStringNumber = (goban->getStringLiberty(location+goban->direction(i))>1) ? goban->getStringNumber(location+goban->direction(i)) : 0;
        if (isSelfAtariMode >= 1 && friendStringNumber>0) return true;//TODO should be changed...
      } else
        if (goban->getStringNumber(location+goban->direction(i)) != friendStringNumber) return true;
  return false;

}
int MoGo::LocationInformation::isConnectingTwoDifferentStringsMove( const Location & location ) const {/**assert(0);**/
  int friendStringNumber = 0; Location friendLocation=PASSMOVE;
  for (int i=0;i<4;i++)
    if (goban->gobanState[location+goban->direction(i)] == goban->playerColor)
      if (friendStringNumber == 0) {/**assert(0);**/
        friendStringNumber = goban->getStringNumber(location+goban->direction(i));
        friendLocation=location+goban->direction(i);
      } else
        if (goban->getStringNumber(location+goban->direction(i)) != friendStringNumber)
          return goban->getStringLength(friendLocation)+goban->getStringLength(location+goban->direction(i));
  return 0;
}

bool MoGo::LocationInformation::isLastMoveMenacing( ) const {/**assert(0);**/
  if (goban->lastPlayedMove==PASSMOVE)
    return false;
  for (int i=0;i<20;i++) {/**assert(0);**/
    Location location=goban->lastPlayedMove+goban->direction(i);
    if (location<goban->leftUpLocation_ || location>goban->rightDownLocation_) continue;
    if (goban->gobanState[location] == goban->playerColor) return true;
  }
  return false;
}

bool MoGo::LocationInformation::isLastMoveMenacingMode2( ) const {/**assert(0);**/
  if (goban->lastPlayedMove==PASSMOVE)
    return false;
  for (int i=0;i<20;i++) {/**assert(0);**/
    Location location=goban->lastPlayedMove+goban->direction(i);
    if (location<goban->leftUpLocation_ || location>goban->rightDownLocation_) continue;
    if (goban->gobanState[location] == goban->playerColor || goban->gobanState[location] == 3-goban->playerColor) return true;
  }
  return false;
}


bool MoGo::LocationInformation::isIsolatedSideMove( const Location & location ) const {/**assert(0);**/
  for (int i=0;i<20;i++)
    if (location+goban->direction(i)>=goban->leftUpLocation_ &&
        location+goban->direction(i)<=goban->rightDownLocation_ &&
        (goban->gobanState[location+goban->direction(i)] == BLACK ||
         goban->gobanState[location+goban->direction(i)] == WHITE))
      return false;
  return true;
}

void MoGo::LocationInformation::showStringPreviewResult( ) const {/**assert(0);**/
  GoTools::print("\nlastStringPreviewLocation:");goban->textModeShowPosition(lastStringPreviewLocation);
  GoTools::print("\n%d friendAdjacentStrings:",friendAdjacentStringLocation.size());
  for (int i=0;i<(int)friendAdjacentStringLocation.size();i++)
    goban->textModeShowPosition(friendAdjacentStringLocation[i]);
  GoTools::print("\n%d enemyAdjacentStrings:",enemyAdjacentStringLocation.size());
  for (int i=0;i<(int)enemyAdjacentStringLocation.size();i++)
    goban->textModeShowPosition(enemyAdjacentStringLocation[i]);
  GoTools::print("\n%d deadAdjacentStrings:",deadAdjacentStringLocation.size());
  for (int i=0;i<(int)deadAdjacentStringLocation.size();i++)
    goban->textModeShowPosition(deadAdjacentStringLocation[i]);
  GoTools::print("\n%d freeAdjacentStrings:",freeAdjacentLocation.size());
  for (int i=0;i<(int)freeAdjacentLocation.size();i++)
    goban->textModeShowPosition(freeAdjacentLocation[i]);
  GoTools::print("\n");


}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
// TEMPORARY COMMENT: BLACK PLAYS 2 TIMES
bool MoGo::LocationInformation::isStillEye( const Location location ) const {
  // must be called before change of player.
  if (goban->gobanState[location] != EMPTY) {/**assert(0);**/
    goban->textModeShowGoban();
    goban->textModeShowPosition(location);
    goban->showFreeLocation();
    GoTools::print("is Still Eye BUG!!!\n");
    //     assert(0);
    goban->gobanLists.eyeLocation.deleteLocation(location);
    return true;
  }
  assert(goban->gobanState[location] == EMPTY);
  assert(goban->gobanLists.eyeLocation.isLocationIn(location));

  for (int i=0;i<4;i++)
    if (goban->gobanState[location+goban->direction(i)] == goban->playerColor) return true;
    else if (goban->gobanState[location+goban->direction(i)] == 3-goban->playerColor) break;

  int enemyNumber = 0, outsideNumber = 0;//eye color is different from playerColor.
  for (int i=4;i<8;i++) {
    StoneColor stoneColor = goban->gobanState[location+goban->direction(i)];
    if (stoneColor == 3-goban->playerColor ) ;
    else if (stoneColor == EMPTY) ;
    else if (stoneColor == OUTSIDE) {
      outsideNumber++;
      if (outsideNumber>=2 && enemyNumber>=1) return false;
    } else if (stoneColor == goban->playerColor) {
      enemyNumber++;
      if (outsideNumber>=2) return false;
      if (enemyNumber>=2) return false;
    } else assert(0);
  }
  return true;
}

bool MoGo::LocationInformation::isOutsideOfGoban( const Location & location ) const {/**assert(0);**/
  if (GobanLocation::xLocation(location) == -1 ||
      GobanLocation::yLocation(location) == -1) return true;
  return false;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::LocationInformation::setIsSelfAtariMode( int mode ) {
  isSelfAtariMode = mode;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
// TEMPORARY COMMENT: BLACK PLAYS 2 TIMES
bool MoGo::LocationInformation::isYoseOnSide( const Location & location, int side ) const {
  if (side==-1)
    side=goban->getSideNumber(location);
  if (side>=4) return false;
  assert(side>=0);
  const int * directionTRotationI=directionT[rotation[side]];
  if (goban->gobanState[location+directionTRotationI[NORTH_]] != OUTSIDE) {/**assert(0);**/
    goban->textModeShowGoban();
    goban->textModeShowPosition(location);
    GoTools::print("side number:%d\n",side);
  }//TODO debug lines
  assert(goban->gobanState[location+directionTRotationI[NORTH_]] == OUTSIDE);
  // %%% // % = OUTSIDE
  // ?.?
  // ?.?
  if (goban->gobanState[location+directionTRotationI[SOUTH_]] == EMPTY)
    // %%%
    // ...
    // X.O
  {
    if (goban->gobanState[location+directionTRotationI[WEST_]]
        +goban->gobanState[location+directionTRotationI[EAST_]] == 0 &&
        goban->gobanState[location+directionTRotationI[SOUTHWEST_]]
        +goban->gobanState[location+directionTRotationI[SOUTHEAST_]] == 3)
      return true;
    // %%% %%%
    // X.? ?.X
    // O.? ?.O
    else if(goban->gobanState[location+directionTRotationI[WEST_]] +goban->gobanState[location+directionTRotationI[SOUTHWEST_]] == 3 ||
            goban->gobanState[location+directionTRotationI[EAST_]] +goban->gobanState[location+directionTRotationI[SOUTHEAST_]] == 3)
      return true;
  }
  // %%%
  // ?.?
  // ?X?
  // %%%
  // Y.O
  // ?X?
  else if ((goban->gobanState[location+directionTRotationI[SOUTH_]]
            +goban->gobanState[location+directionTRotationI[EAST_]] == 3 ||
            goban->gobanState[location+directionTRotationI[SOUTH_]]
            +goban->gobanState[location+directionTRotationI[WEST_]] == 3) &&
           goban->gobanState[location+directionTRotationI[WEST_]]
           +goban->gobanState[location+directionTRotationI[EAST_]] != 3)
    return true;
  // %%%                           %%% %%%
  // ?.?                           ?.Y O.X
  // ?XO  playerColor X            ?XO ?XO
  else if (goban->gobanState[location+directionTRotationI[SOUTHEAST_]]
           +goban->gobanState[location+directionTRotationI[SOUTH_]] == 3 &&
           (goban->playerColor == goban->gobanState[location+directionTRotationI[SOUTH_]] ||
            goban->gobanState[location+directionTRotationI[EAST_]] != 3-goban->playerColor ||
            goban->gobanState[location+directionTRotationI[WEST_]] == goban->playerColor))
    return true;
  // %%%
  // ?.?
  // OX?
  else if (goban->gobanState[location+directionTRotationI[SOUTHWEST_]]
           +goban->gobanState[location+directionTRotationI[SOUTH_]] == 3 &&
           (goban->playerColor == goban->gobanState[location+directionTRotationI[SOUTH_]] ||
            goban->gobanState[location+directionTRotationI[WEST_]] != 3-goban->playerColor ||
            goban->gobanState[location+directionTRotationI[EAST_]] == goban->playerColor))
    return true;

  return false;

}
// #define DEBUG_SHISHO

bool MoGo::LocationInformation::getShishoLikeMove( Location & nextMove, Location & lastLiberty ) const {/**assert(0);**/
  //GoTools::print("getShishoLikeMove called. last move: ");
  //goban->textModeShowPosition(goban->lastPlayedMove);
  //GoTools::print("\n");
  Location lastMove = goban->lastMove();
  if (lastMove == PASSMOVE) return false;
  if (goban->getStringLiberty(lastMove) != 2) return false;

  Vector<Location> freeLiberty;
  for (int i=0;i<4;i++)
    if (goban->gobanState[lastMove+goban->direction(i)] == EMPTY)
      freeLiberty.push_back(lastMove+goban->direction(i));
  if ((int)freeLiberty.size() != 2) return false;


  Vector<Location> shishoLikeMove;
  for (int i=0;i<2;i++) {/**assert(0);**/
    int nextLiberty = 0;//if leave freeLiberty[i] as the last liberty (by playing on the other liberty), then when the under-atari string extends at freeLiberty[i], the liberty it will get.
    for (int j=0;j<4;j++) {/**assert(0);**/
      Location tmpLocation = freeLiberty[i]+goban->direction(j);
      if (tmpLocation == lastMove) continue;
      if (goban->gobanState[tmpLocation] == EMPTY)
        nextLiberty++;
      else if (goban->gobanState[tmpLocation] == 3-goban->playerColor)
        nextLiberty+=goban->getStringLiberty(tmpLocation)-1;
      else if (goban->gobanState[tmpLocation] == goban->playerColor &&
               goban->getStringLiberty(tmpLocation) == 1)
        nextLiberty++;
    }
    if (nextLiberty <= 2)
      shishoLikeMove.push_back(freeLiberty[1-i]);
  }
  //   GoTools::print("shishoLikeMove size %d\n",shishoLikeMove.size());
  if ((int)shishoLikeMove.size() == 0)
    return false;
  else if (shishoLikeMove.size() == 1) {/**assert(0);**/
    nextMove = shishoLikeMove[0];
    for (int i=0;i<2;i++)
      if (freeLiberty[i]!=nextMove)
        lastLiberty = freeLiberty[i];
  } else {/**assert(0);**/
    Location firstPossibility=shishoLikeMove[0]; int nbLiberties1=0; int nbLibertiesOwnStone1=100;
    for (int i=0;i<4;i++) {/**assert(0);**/
      Location tmpLocation=firstPossibility+goban->direction(i);
      if (goban->gobanState[tmpLocation]==EMPTY) nbLiberties1++;
      else if (goban->gobanState[tmpLocation]==goban->playerColor) nbLibertiesOwnStone1=goban->getStringLiberty(tmpLocation);
    }
    Location secondPossibility=shishoLikeMove[1]; int nbLiberties2=0;int nbLibertiesOwnStone2=100;
    for (int i=0;i<4;i++) {/**assert(0);**/
      Location tmpLocation=secondPossibility+goban->direction(i);
      if (goban->gobanState[tmpLocation]==EMPTY) nbLiberties2++;
      else if (goban->gobanState[tmpLocation]==goban->playerColor) nbLibertiesOwnStone2=goban->getStringLiberty(tmpLocation);
    }
#ifdef DEBUG_SHISHO
    GoTools::print("choose between %i and %i (%i,%i) (%i, %i)\n", firstPossibility, secondPossibility, nbLiberties1, nbLiberties2, nbLibertiesOwnStone1, nbLibertiesOwnStone2);
#endif
    if (nbLiberties1==nbLiberties2) {/**assert(0);**/
      if (nbLibertiesOwnStone1==nbLibertiesOwnStone2) {/**assert(0);**/
        int index = GoTools::mogoRand(2);
        nextMove = shishoLikeMove[index];
        lastLiberty= shishoLikeMove[1-index];
      } else if (nbLibertiesOwnStone1<nbLibertiesOwnStone2) {/**assert(0);**/
        nextMove = shishoLikeMove[0];
        lastLiberty= shishoLikeMove[1];
      } else {/**assert(0);**/
        nextMove = shishoLikeMove[1];
        lastLiberty= shishoLikeMove[0];
      }
    } else if (nbLiberties1<nbLiberties2) {/**assert(0);**/
      nextMove = shishoLikeMove[1];
      lastLiberty= shishoLikeMove[0];
    } else {/**assert(0);**/
      nextMove = shishoLikeMove[0];
      lastLiberty= shishoLikeMove[1];
    }
  }
  return true;

}








void MoGo::LocationInformation::showGobanOnBug19x19( const Location location, const Vector< Location > & gobanState, const Vector< int > & , const int & , int tmp ) {/**assert(0);**/
  goban->textModeShowPosition(location);
  goban->textModeShowPosition(tmp);
  GoTools::print("%d %d\n",tmp,location);
  GoTools::print("%d %d\n",goban->gobanState[tmp],stringInformation->getStringNumber(tmp));
  goban->textModeShowGoban();
  for (int i=0;i<(int)gobanState.size();i++)
    GoTools::print("%i\n", gobanState[i]);

}





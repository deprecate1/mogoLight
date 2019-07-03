//
// C++ Implementation: goban_mg
//
// Description:
//
//
// Author: Yizao Wang <yizao.wang@polytechnique.edu> and Sylvain Gelly <gelly@lri.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "goban_mg.h"
#include "gotools_mg.h"
#include "randomgoplayer_mg.h"
#include "gobannode_mg.h"
#include "goexperiments_mg.h"
#include "innermcgoplayer_mg.h"
#include "yadtinterface_mg.h"
#include "gogametree_mg.h"

using namespace MoGo;

extern int handicapLevel;

Vector< unsigned int > threeExp;

int MoGo::Goban::notUselessMode = 0;
int MoGo::Goban::cfgMode = 0;
int MoGo::Goban::recheckEyeModeStatic = 0;
int MoGo::Goban::__thresholdSelfAtari=2;
int MoGo::Goban::__thresholdEatingMove=0;
bool MoGo::Goban::showBigGobanStringNumber = false;
bool MoGo::Goban::shishoLikeMoveForced = 0;
int MoGo::Goban::shishoReaderMode = 0;
MoGo::InterestingMoveFinder * MoGo::Goban::finder1=0;
MoGo::InterestingMoveFinder * MoGo::Goban::finder2=0;
int MoGo::Goban::slowScore=0;
int MoGo::Goban::use25BlocksForGP=0;
bool MoGo::Goban::useLibertiesForGP=false;
bool MoGo::Goban::useLastLastMove=false;
bool MoGo::Goban::useDistanceInGPPatterns=false;
Vector<double> MoGo::Goban::probabilitiesOfinterestingMovesStatic;
int MoGo::Goban::random200Iterations;
int MoGo::Goban::randomModePrecomputed;
Vector<int> MoGo::Goban::isInterestingPrecomputed;
Vector<int> MoGo::Goban::isSelfAtariPrecomputed;
int MoGo::Goban::updateEatingLocations=1;
int MoGo::Goban::moveChangedMode=0;
Vector<double> MoGo::Goban::probabilityOfStringSurvival;
double MoGo::Goban::powProbabilityOfStringSurvival=1.;
double MoGo::Goban::epsilonGreedy=0.;

Vector<Vector<int> >MoGo::Goban::nbSeenPattern;
Vector<Vector<int> >MoGo::Goban::nbSuccessPattern;
double MoGo::Goban::successThresholdForInterestingMoves=0.7;
double MoGo::Goban::successThresholdForNonInterestingMoves=0.5;
int MoGo::Goban::retryForNonInterestingMoves=1;

int MoGo::Goban::koUseMode=0;
int MoGo::Goban::avoidSelfAtariMode=0;
int MoGo::Goban::eatingSizePrefered=3;

int MoGo::Goban::raceForLibertiesThreshold=3;
int MoGo::Goban::raceForLibertiesLengthThreshold=3;
int MoGo::Goban::useGlobalPatterns=0;

int MoGo::Goban::updateCloseLiberties=0;
SmallPatternsDatabase MoGo::Goban::smallDatabase;
int MoGo::Goban::nonRandomCountW=0;
int MoGo::Goban::nonRandomCountB=0;

Vector<int> MoGo::Goban::allPatternWeights;

int MoGo::Goban::allowAttackInLadderWhenAdjacent=false;

////////////////////////
// GobanLocationLists //
////////////////////////

// TEMPORARY COMMENT: THIS FUNCTION IS USED
MoGo::GobanLocationLists::GobanLocationLists( int maxSize ) :
    freeLocation(maxSize),
    eyeLocation(maxSize),
    blackEatingLocation(maxSize),
    whiteEatingLocation(maxSize),
    blackSelfKillLocation(maxSize),
    whiteSelfKillLocation(maxSize),
    rangeLocation(maxSize),
openingRangeLocation(maxSize) {}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
MoGo::GobanLocationLists::~ GobanLocationLists( ) {}




namespace MoGo {
  int Goban::thresholdRandomSwitch=30;

  // TEMPORARY COMMENT: THIS FUNCTION IS USED
  Goban::Goban(int height, int width) :
      heightOfGoban(height), widthOfGoban(width),
      gobanState((height+2)*(width+2),0),
      gobanLists((height+2)*(width+2)),
      //freeLocation((height+2)*(width+2)),
      //eyeLocation((height+2)*(width+2)),
      //blackSelfKillLocation((height+2)*(width+2)),
      //whiteSelfKillLocation((height+2)*(width+2)),
      //blackEatingLocation((height+2)*(width+2)),
      //whiteEatingLocation((height+2)*(width+2)),
      stringInformation(this),
  locationInformation(this) {
//    weAreTheBest=false;
    cfg=0;
    if (cfgMode>0)
      cfg=new CFG(this);
    initiateGoban();
    //     GobanUrgency::height=height;
    //     GobanUrgency::width=width;
  }

  MoGo::Goban::Goban( GobanKey gobanKey ) :
      heightOfGoban(gobanKey[0]%32),
      widthOfGoban(gobanKey[0]%1024/32),
      gobanState((gobanKey[0]%32+2)*(gobanKey[0]%1024/32+2),0),
      gobanLists((gobanKey[0]%32+2)*(gobanKey[0]%1024/32+2)),
      //freeLocation((gobanKey[0]%32+2)*(gobanKey[0]%1024/32+2)),
      //eyeLocation((gobanKey[0]%32+2)*(gobanKey[0]%1024/32+2)),
      //blackSelfKillLocation((gobanKey[0]%32+2)*(gobanKey[0]%1024/32+2)),
      //whiteSelfKillLocation((gobanKey[0]%32+2)*(gobanKey[0]%1024/32+2)),
      //blackEatingLocation((gobanKey[0]%32+2)*(gobanKey[0]%1024/32+2)),
      //whiteEatingLocation((gobanKey[0]%32+2)*(gobanKey[0]%1024/32+2)),
      stringInformation(this),
  locationInformation(this) {
//    weAreTheBest=false;
    cfg=0;//TODO here we lost the memory?
    if (cfgMode>0)
      cfg=new CFG(this);

    //     GobanUrgency::height=heightOfGoban;
    //     GobanUrgency::width=widthOfGoban;

    assert((gobanKey[0]%32+2)*(gobanKey[0]%1024/32+2)<500);
    //     heightOfGoban=gobanKey[0]%32;//2^5 I am not sure that I could use the "<" operator here to accelerate.
    //     widthOfGoban=gobanKey[0]%1024/32;//2^10 2^5
    //freeLocation.testShow();
    initiateGoban();
    assert(gobanKey.size()>0);
    //freeLocation.testShow();
    ko=gobanKey[0]%(1024*512)/1024;//2^19/2^10;
    playerColor=1+gobanKey[0]%(1024*1024)/(1024*512);//2^20/2^19;
    gobanState[indexOnArray(0,0)]=gobanKey[0]/(1024*1024);//2^20;

  if (gobanState[indexOnArray(0,0)]==BLACK || gobanState[indexOnArray(0,0)]==WHITE) {gobanLists.freeLocation.deleteLocation(indexOnArray(0,0));moveNumber_++;}

    int c=-1,index=1;
    assert(gobanKey.size()>1);
    unsigned int copyKey=gobanKey[1];
    for (int i=1;i<widthOfGoban;i++) {
      c++;
      if (c==20) {/**assert(0);**/
        c=0;
        index++;
        copyKey=gobanKey[index];
      }
      gobanState[indexOnArray(0,i)]=copyKey%3;
      if (gobanState[indexOnArray(0,i)]==BLACK || gobanState[indexOnArray(0,i)]==WHITE) {
        assert(gobanLists.freeLocation.isLocationIn(indexOnArray(0,i)));
        gobanLists.freeLocation.deleteLocation(indexOnArray(0,i));
        moveNumber_++;
      }
      copyKey/=3;
    }

    //freeLocation.testShow();
    for (int i=1;i<heightOfGoban;i++)
      for (int j=0;j<widthOfGoban;j++) {
        c++;
        if (c==20) {
          c=0;
          index++;
          copyKey=gobanKey[index];
        }
        gobanState[indexOnArray(i,j)]=copyKey%3;
        if (gobanState[indexOnArray(i,j)]==BLACK || gobanState[indexOnArray(i,j)]==WHITE) {
          assert(gobanLists.freeLocation.isLocationIn(indexOnArray(i,j)));
          gobanLists.freeLocation.deleteLocation(indexOnArray(i,j));
          moveNumber_++;
        }
        copyKey/=3;
      }
    //if (!cfgOptionEnabled)//FIXME CFG
    if (isStringInformationOn)
      stringInformation.calculateAllFromGobanState();
    if (isCFGOn)
      cfg->calculateAllFromGobanState(gobanState);

    keyArray.resize(gobanKey.size());
    for (int i=0;i<(int)gobanKey.size();i++)
      keyArray[i]=gobanKey[i];
    gobanKeyUpdated=true;

    //GoTools::print("end of copy of goban by gobankey.\n");
    //textModeShowGoban();
    //showFreeLocation();

  }

 
  MoGo::Goban::Goban( const Goban & goban) : gobanState(goban.gobanState), gobanLists(goban.gobanLists),
      stringInformation(goban.stringInformation), locationInformation(goban.locationInformation), directionsArray(goban.directionsArray), keyArray(goban.keyArray), moveValuesBlack(goban.moveValuesBlack), moveValuesWhite(goban.moveValuesWhite),
  directionT25(goban.directionT25) {
//    weAreTheBest=false;
    heightOfGoban = goban.heightOfGoban;
    widthOfGoban = goban.widthOfGoban ;
    //     assert(goban.gobanState.size()<500);
    //     assert(goban.gobanState.capacity()<1000);
    //     assert(gobanState.capacity()<1000);
    //     gobanState = goban.gobanState;
    ko = goban.ko;
    blackEatenStoneNumber = goban.blackEatenStoneNumber;
    whiteEatenStoneNumber = goban.whiteEatenStoneNumber;
    playerColor = goban.playerColor;
    is_Scorable = goban.is_Scorable;
    //     gobanLists = goban.gobanLists;
    //freeLocation = goban.freeLocation;
    //eyeLocation = goban.eyeLocation;
    //blackSelfKillLocation = goban.blackSelfKillLocation;
    //whiteSelfKillLocation = goban.whiteSelfKillLocation;
    //blackEatingLocation = goban.blackEatingLocation;
    //whiteEatingLocation = goban.whiteEatingLocation;

    //     stringInformation = goban.stringInformation;
    //     locationInformation = goban.locationInformation;
    cfg=0;
    if (cfgMode > 0 && (goban.cfg!=0)) {/**assert(0);**/
      cfg = new CFG(*goban.cfg);
      cfg->goban = this;
    }


    locationInformation.goban=this;locationInformation.stringInformation=&stringInformation;
    stringInformation.goban=this;stringInformation.locationInformation = &locationInformation;


    stopRandom = goban.stopRandom;
    //     directionsArray = goban.directionsArray;
    //     createDirectionsArray();
    //     keyArray = goban.keyArray;
    gobanKeyUpdated = goban.gobanKeyUpdated;
    //     threeExp = goban.threeExp;
    lastPlayedMove = goban.lastPlayedMove;
    lastLastPlayedMove = goban.lastLastPlayedMove;
    lastLastLastPlayedMove = goban.lastLastLastPlayedMove;
    this->maintainGobanKey=goban.maintainGobanKey;

    changedByForce = goban.changedByForce;
    openingRangeMode = goban.openingRangeMode;
    leftUpLocation_ = goban.leftUpLocation_;
    rightDownLocation_ = goban.rightDownLocation_;
    moveNumber_ = goban.moveNumber_;

    isCFGOn = goban.isCFGOn;
    isStringInformationOn = goban.isStringInformationOn;
    isOutsideStrange = goban.isOutsideStrange;
    shishoLocation = goban.shishoLocation;
    attackShishoLocation = goban.attackShishoLocation;
    pastInterestingMoves = goban.pastInterestingMoves;
    //     directionT25=goban.directionT25;
    //     moveValuesBlack=goban.moveValuesBlack;
    //     moveValuesWhite=goban.moveValuesWhite;
    recheckEyeMode=goban.recheckEyeMode;
    koModeMoveStep=goban.koModeMoveStep;
    koModeMoveLocation=goban.koModeMoveLocation;
    if (updateCloseLiberties)
      closeLiberties=goban.closeLiberties;


  }

  // TEMPORARY COMMENT: THIS FUNCTION IS USED
  Goban & MoGo::Goban::operator =( const Goban & goban ) {
    heightOfGoban = goban.heightOfGoban;
//    weAreTheBest = goban.weAreTheBest;
    widthOfGoban = goban.widthOfGoban ;
    //     assert(goban.gobanState.size()<500);
    //     assert(goban.gobanState.capacity()<1000);
    //     assert(gobanState.capacity()<1000);
    gobanState = goban.gobanState;
    ko = goban.ko;
    blackEatenStoneNumber = goban.blackEatenStoneNumber;
    whiteEatenStoneNumber = goban.whiteEatenStoneNumber;
    playerColor = goban.playerColor;
    is_Scorable = goban.is_Scorable;
    gobanLists = goban.gobanLists;
    //freeLocation = goban.freeLocation;
    //eyeLocation = goban.eyeLocation;
    //blackSelfKillLocation = goban.blackSelfKillLocation;
    //whiteSelfKillLocation = goban.whiteSelfKillLocation;
    //blackEatingLocation = goban.blackEatingLocation;
    //whiteEatingLocation = goban.whiteEatingLocation;

    stringInformation = goban.stringInformation;
    locationInformation = goban.locationInformation;
    if (cfg) delete cfg;
    cfg=0;
    if (cfgMode > 0 && (goban.cfg!=0)) {/**assert(0);**/
      cfg = new CFG(*goban.cfg);
      cfg->goban = this;
    }


    locationInformation.goban=this;locationInformation.stringInformation=&stringInformation;
    stringInformation.goban=this;stringInformation.locationInformation = &locationInformation;


    stopRandom = goban.stopRandom;
    directionsArray = goban.directionsArray;
    //     createDirectionsArray();
    keyArray = goban.keyArray;
    gobanKeyUpdated = goban.gobanKeyUpdated;
    //     threeExp = goban.threeExp;
    lastPlayedMove = goban.lastPlayedMove;
    lastLastPlayedMove = goban.lastLastPlayedMove;
    lastLastLastPlayedMove = goban.lastLastLastPlayedMove;
    this->maintainGobanKey=goban.maintainGobanKey;

    changedByForce = goban.changedByForce;
    openingRangeMode = goban.openingRangeMode;
    leftUpLocation_ = goban.leftUpLocation_;
    rightDownLocation_ = goban.rightDownLocation_;
    moveNumber_ = goban.moveNumber_;

    isCFGOn = goban.isCFGOn;
    isStringInformationOn = goban.isStringInformationOn;
    isOutsideStrange = goban.isOutsideStrange;
    shishoLocation = goban.shishoLocation;
    attackShishoLocation = goban.attackShishoLocation;
    pastInterestingMoves = goban.pastInterestingMoves;
    directionT25=goban.directionT25;
    moveValuesBlack=goban.moveValuesBlack;
    moveValuesWhite=goban.moveValuesWhite;
    recheckEyeMode=goban.recheckEyeMode;
    koModeMoveStep=goban.koModeMoveStep;
    koModeMoveLocation=goban.koModeMoveLocation;
    if (updateCloseLiberties)
      closeLiberties=goban.closeLiberties;
    return *this;
  }

  // TEMPORARY COMMENT: THIS FUNCTION IS USED
  Goban::~Goban() {
    if (cfg) delete cfg;
  }

  void MoGo::Goban::reloadByGobanKey( const GobanKey & gobanKey, bool outrangeIsOutside ) {/**assert(0);**/
    if (!isOutsideStrange) {/**assert(0);**/
      int moveNumberTmp=moveNumber_;
      *this = Goban(gobanKey);
      moveNumber_=moveNumberTmp;
      //assert(0);//FIXME DEBUG DELETE
      return;
    }
    //     assert(0);//FIXME DEBUG DELETE
    locationInformation.resetStringPreview();
    locationListsClear();

    ko=gobanKey[0]%(1024*512)/1024;//2^19/2^10;
    playerColor=1+gobanKey[0]%(1024*1024)/(1024*512);//2^20/2^19;


    Location tmpLocation = GobanLocation::locationXY(0,0);
    if (outrangeIsOutside && !isInRangeLocation(tmpLocation))
      ;
    //gobanState[tmpLocation] = OUTSIDE;
    else {/**assert(0);**/
      gobanState[tmpLocation]=gobanKey[0]/(1024*1024);//2^20;
      if (gobanState[tmpLocation]==EMPTY &&
          isInRangeLocation(tmpLocation))
        gobanLists.freeLocation.addLocation(tmpLocation);
    }
    int c=-1,index=1;
    assert(gobanKey.size()>1);
    unsigned int copyKey=gobanKey[1];
    for (int i=1;i<widthOfGoban;i++) {/**assert(0);**/
      c++;
      if (c==20) {/**assert(0);**/
        c=0;
        index++;
        copyKey=gobanKey[index];
      }
      tmpLocation = GobanLocation::locationXY(0,i);
      if (outrangeIsOutside && !isInRangeLocation(tmpLocation))
        ;
      //gobanState[tmpLocation] = OUTSIDE;
      else {/**assert(0);**/
        gobanState[tmpLocation]=copyKey%3;
        if (gobanState[tmpLocation]==EMPTY && isInRangeLocation(tmpLocation))
          gobanLists.freeLocation.addLocation(tmpLocation);
      }
      copyKey/=3;
    }

    //freeLocation.testShow();
    for (int i=1;i<heightOfGoban;i++)
      for (int j=0;j<widthOfGoban;j++) {/**assert(0);**/
        c++;
        if (c==20) {/**assert(0);**/
          c=0;
          index++;
          copyKey=gobanKey[index];
        }
        Location tmpLocation = GobanLocation::locationXY(i,j);
        if (outrangeIsOutside && !isInRangeLocation(tmpLocation))
          ;
        //           gobanState[tmpLocation] = OUTSIDE;
        else {/**assert(0);**/
          gobanState[tmpLocation]=copyKey%3;
          if (gobanState[tmpLocation]==EMPTY && isInRangeLocation(tmpLocation))
            gobanLists.freeLocation.addLocation(tmpLocation);
        }
        copyKey/=3;
      }
    //if (!cfgOptionEnabled)//FIXME CFG
    if (isStringInformationOn) {/**assert(0);**/
      stringInformation.clear();
      stringInformation.calculateAllFromGobanState();
      locationInformation.resetStringPreview();
    }
    if (isCFGOn) {/**assert(0);**/
      cfg->calculateAllFromGobanState(gobanState);
      cfg->resetStringPreview();
    }

    keyArray.resize(gobanKey.size());
    for (int i=0;i<(int)gobanKey.size();i++)
      keyArray[i]=gobanKey[i];
    gobanKeyUpdated=true;

    koModeMoveStep=0;koModeMoveLocation=0;


    if (updateCloseLiberties) {/**assert(0);**/
      for (int i=0;i<(int)gobanLists.freeLocation.size();i++)
        if (isOneStoneNear4(gobanLists.freeLocation[i]))
          closeLiberties.push_back(gobanLists.freeLocation[i]);
    }

  }


  // TEMPORARY COMMENT: THIS FUNCTION IS USED
  void Goban::initiateGoban( ) {
    maintainGobanKey = true;
    openingRangeMode = 0;
    isOutsideStrange = false;
    if (cfgMode == 0) {
      isCFGOn = false;
      isStringInformationOn = true;
    } else if (cfgMode == 1) {/**assert(0);**/
      isCFGOn = true;
      cfg->initiate();
      isStringInformationOn = false;
    } else if (cfgMode == 2) {/**assert(0);**/
      isCFGOn = true;
      cfg->initiate();
      isStringInformationOn = true;
    } else assert(0);

    leftUpLocation_ = indexOnArray(0,0);
    rightDownLocation_ = indexOnArray(heightOfGoban-1,widthOfGoban-1);

    assert(widthOfGoban+2>=0);
    assert(heightOfGoban+2>=0);
    for (int i=0;i<widthOfGoban+2;i++) {
      gobanState[i]=OUTSIDE;
      gobanState[(widthOfGoban+2)*(heightOfGoban+1)+i]=OUTSIDE;
    }
    for (int i=0;i<heightOfGoban;i++) {
      gobanState[(i+1)*(widthOfGoban+2)]=OUTSIDE;
      gobanState[(i+2)*(widthOfGoban+2)-1]=OUTSIDE;
    }
    ko=0;
    for (int i=0;i<heightOfGoban;i++)
      for (int j=0;j<widthOfGoban;j++) {
        gobanState[indexOnArray(i,j)]=0;
        gobanLists.rangeLocation.addLocation(indexOnArray(i,j));
      }
    createDirectionsArray();
    clearGoban();
    initiateGobanKey();
    initiateThreeExp();
    createRotationArray25();
    locationInformation.createRotationArray();//FIXME
    recheckEyeMode=recheckEyeModeStatic;
    koModeMoveStep=0;koModeMoveLocation=0;
  }

  // TEMPORARY COMMENT: THIS FUNCTION IS USED
  void Goban::clearGoban() {
    blackEatenStoneNumber = 0;
    whiteEatenStoneNumber = 0;
    moveNumber_ = 0;
    playerColor = BLACK;
    ko = 0;

    isOutsideStrange = false;
    shishoLocation.clear();
    attackShishoLocation.clear();
    //isRealKo=false;
    is_Scorable = false;
    stopRandom = false;
    int i,j;

    stringInformation.clear();
    locationInformation.clear();
    locationListsClear();

    for(i=0;i<heightOfGoban;i++)
      for(j=0;j<widthOfGoban;j++) {
        Location location=indexOnArray(i,j);
        gobanState[location]=0;
        gobanLists.freeLocation.addLocation(location);
      }
    gobanKeyUpdated=false; // for the moment the update of gobanKey is not set for every move, so it is always false.
    lastPlayedMove = PASSMOVE;
    lastLastPlayedMove=PASSMOVE;
    lastLastLastPlayedMove=PASSMOVE;
    if (openingRangeMode >= 1)
      openingRangeLocationInitiation(2,true);

    if (isCFGOn)
      cfg->clear();
    pastInterestingMoves.clear();
    koModeMoveStep=0;koModeMoveLocation=0;
    closeLiberties.clear();
  }

// TEMPORARY COMMENT: THIS FUNCTION IS USED
  bool Goban::isBlacksTurn() const {
    if (playerColor==BLACK) return true;
    return false;

  }

  // TEMPORARY COMMENT: THIS FUNCTION IS USED
  void Goban::locationListsClear() const {
    gobanLists.freeLocation.clear();
    gobanLists.eyeLocation.clear();
    gobanLists.blackSelfKillLocation.clear();
    gobanLists.whiteSelfKillLocation.clear();
    gobanLists.blackEatingLocation.clear();
    gobanLists.whiteEatingLocation.clear();
  }

// TEMPORARY COMMENT: THIS FUNCTION IS USED
  bool Goban::isScorable() const {
    Vector<Location> legalAndNoUselessMoveList;
    for (int i=0;i<heightOfGoban;i++)
      for (int j=0;j<widthOfGoban;j++) {
        Location location=indexOnArray(i,j);
        if (isNotUseless(location))
          return false;
      }
    return true;
  }


// TEMPORARY COMMENT: THIS FUNCTION IS USED
  int Goban::basicTest(const Location location, LocationList*& locationListIncludingThisLocation) const {
    //assert(location!=PASSMOVE);
    //textModeShowPosition(location);
    if (locationListIncludingThisLocation==0) {/**assert(0);**/
      textModeShowGoban(PASSMOVE,true);
      textModeShowPosition(location);
      showFreeLocation();
    } //FIXME OR
    if (!locationListIncludingThisLocation) //FIXME SLOW
      showFreeLocation();
    assert(locationListIncludingThisLocation);
    int testResult=0;//locationInformation.basicTest(location);
    if (isStringInformationOn)
      testResult = locationInformation.basicTest(location);
    if (isCFGOn)
      if (!isStringInformationOn)
        testResult = cfg->basicTest(location);
      else cfg->basicTest(location);

 
    freeLocationUpdateAfterBasicTest(location,locationListIncludingThisLocation,testResult);

    return testResult;
  }
  
  // TEMPORARY COMMENT: THIS FUNCTION IS USED
  bool Goban::isLegal(const Location location) const {
    if (location==PASSMOVE) return true;
    int testResult = 0;//locationInformation.basicTest(location);
    if (isStringInformationOn)
      testResult = locationInformation.basicTest(location);
    else if (isCFGOn)
      testResult = cfg->basicTest(location);
    else assert(0);
 
    if (testResult==LEGAL) return true;
    else return false;
  }

  // TEMPORARY COMMENT: THIS FUNCTION IS USED
  bool Goban::isNotUseless(const Location location) const {
    if (location==PASSMOVE) return true;
    if (!isLegal(location)) return false;
    int testResult = 0;//locationInformation.uselessnessTest(location);
    if (isStringInformationOn)
      testResult = locationInformation.uselessnessTest(location);
    else if (isCFGOn)
      testResult = cfg->uselessnessTest(location);
    else assert(0);
   
    if (testResult==OKMOVE) return true;
    else return false;
  }


}


// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::Goban::createDirectionsArray( ) {
  directionsArray.resize(20);
  //
  //    12 9  13
  // 19 5  1  4 14
  // 8  0 [*] 2 10
  // 18 6  3  7 15
  //    17 11 16
  //
  directionsArray[0] = -1;//LEFT
  directionsArray[1] = -(widthOfGoban+2);//UP
  directionsArray[2] = 1;//RIGHT
  directionsArray[3] = widthOfGoban+2 ;//DOWN
  directionsArray[4] = -(widthOfGoban+1);//RIGHTUP
  directionsArray[5] = -(widthOfGoban+3);//LEFTUP
  directionsArray[6] = widthOfGoban+1;//LEFTDOWN
  directionsArray[7] = widthOfGoban+3;//RIGHTDOWN

  directionsArray[8] = directionsArray[LEFT_]+directionsArray[LEFT_];
  directionsArray[9] = directionsArray[UP_]+directionsArray[UP_];
  directionsArray[10] = directionsArray[RIGHT_]+directionsArray[RIGHT_];
  directionsArray[11] = directionsArray[DOWN_]+directionsArray[DOWN_];
  directionsArray[12] = directionsArray[LEFT_]+directionsArray[UP_]+directionsArray[UP_];
  directionsArray[13] = directionsArray[RIGHT_]+directionsArray[UP_]+directionsArray[UP_];
  directionsArray[14] = directionsArray[UP_]+directionsArray[RIGHT_]+directionsArray[RIGHT_];
  directionsArray[15] = directionsArray[DOWN_]+directionsArray[RIGHT_]+directionsArray[RIGHT_];
  directionsArray[16] = directionsArray[RIGHT_]+directionsArray[DOWN_]+directionsArray[DOWN_];
  directionsArray[17] = directionsArray[LEFT_]+directionsArray[DOWN_]+directionsArray[DOWN_];
  directionsArray[18] = directionsArray[DOWN_]+directionsArray[LEFT_]+directionsArray[LEFT_];
  directionsArray[19] = directionsArray[UP_]+directionsArray[LEFT_]+directionsArray[LEFT_];


}


// TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::Goban::playMoveOnGoban(const Location location) {
  if (location==PASSMOVE) {
    playPassMoveOnGoban();
    return true;
  }

  moveNumber_++;
  if (location==ko) {/**assert(0);**/
    GoTools::print("forbidden ko move!");//FIXME
    textModeShowGoban(location);
    showFreeLocation();
    printAllStringInformation();
    assert(0);
  }
  LocationList * locationListIncludingThisLocation=getLocationListIncludingThisLocationForMovePlay(location);
  int testResult=basicTest(location,locationListIncludingThisLocation);

  if (testResult!=LEGAL) //here could be optimized when a preselection of move is executed. In this case every move entered here is qualified already  by basicTest then no need to redo it. But the optimization way is not evident.
  {/**assert(0);**/
    GoTools::print("Illegal move occurs at ");textModeShowPosition(location);
    GoTools::print("\nIllegal reason(number): %d\n",testResult);
    showBigGobanStringNumber = true;
    textModeShowGoban();
    stringInformation.printAllStringInformation();
    showFreeLocation();

    assert(0);
  }

  if (updateCloseLiberties) {/**assert(0);**/
    removeCloseLibertiesByPlaying(location);
    addCloseLibertiesWhenPlaying(location);
  }

  if (isStringInformationOn)
    if (!isCFGOn)
      return innerPlayMoveOnGoban(location,locationListIncludingThisLocation);
    else return innerPlayMoveOnGobanMix(location,locationListIncludingThisLocation);
  else if (isCFGOn)
    return innerPlayMoveOnGobanCFG(location,locationListIncludingThisLocation);
  else assert(0);
  return PASSMOVE;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::Goban::innerPlayMoveOnGoban(const Location location,LocationList *locationListIncludingThisLocation) {

  stringInformation.playMoveOnGoban(location);//until now there is not yet any update for the freeLocation/selfKillLocation/eyeLocation (in the stringInformation). BUT the move has been played, with the dead stones taken away from the board.

  //here in the freeLocationUpdateAfterPlayMoveOnGoban, 'this' location is deleted,
  //if there are stones eaten, then these locations are added to the freeLocation.
  freeLocationUpdateAfterPlayMoveOnGoban(location,locationListIncludingThisLocation);

  if (stringInformation.erasedStones.size()) {
    selfKillLocationUpdate();
  }

  //for the special locations: in fact one liberty locations:
  //enemy strings has but one liberty left
  //this move has but one liberty after being played.
  specialLocationUpdate(location,locationListIncludingThisLocation);

  if (ko>0)
    if (gobanState[ko]==EMPTY) {
      // TEMPORARY COMMENT: THIS FUNCTION IS USED
      // TEMPORARY COMMENT: BLACK PLAYS 2 TIMES
      /*LocationList * locationListIncludingKo = getLocationListIncludingThisLocation(ko);
      if (!locationListIncludingKo)
        gobanLists.freeLocation.addLocation(ko);
        */ //FIXME before change for randomMode 9
      if (!gobanLists.freeLocation.isLocationIn(ko))
        gobanLists.freeLocation.addLocation(ko);
      //showFreeLocation();
      //getchar();
    } else if (isOutsideStrange && gobanState[ko] == OUTSIDE)
      ko = 0;
    else {/**assert(0);**/
      //GoTools::print("----------------------BUG in innerPlayMoveOnGoban--------------------\n");
      //textModeShowGoban(location);
      //showFreeLocation();
      GoTools::print("problem with ko. ");
      //if (isOutsideStrange)
      //GoTools::print("caused by strange outside shape. yizao will resolve it.\n");
      //else GoTools::print("tell yizao!\n");
      //GoTools::print("---------------------------------------------------------------------\n");
    }
  ko=locationInformation.testKo;


  //for the eating move just created.
  if (updateEatingLocations)
    updateEatingMove(location,locationListIncludingThisLocation);

  if (recheckEyeMode)
    recheckEyeLocationsAfterPlayMoveOnGoban(location);


  changePlayer();
  lastLastLastPlayedMove=lastLastPlayedMove;
  lastLastPlayedMove=lastPlayedMove;
  lastPlayedMove = location;
  if (!isKoCorrect()) {/**assert(0);**/
    showBigGobanStringNumber = true;
    textModeShowGoban();
    textModeShowPosition(location);
    stringInformation.printAllStringInformation();
    showFreeLocation();
    assert(isKoCorrect());//FIXME KO SLOW
  }
  //GoTools::print("updateEatingMove done.\n");
  locationInformation.resetStringPreview();
  //if (cfgOptionEnabled) //FIXME CFG
  //cfg->resetStringPreview();
  if (maintainGobanKey) {
    updateGobanKey(location);
    gobanKeyUpdated=true;
  } else
    gobanKeyUpdated=false;


  if (openingRangeMode >= 1 && gobanLists.openingRangeLocation.size())
    openingRangeLocationUpdateAfterMove(location);//FIXME OR


  return true;
}

bool MoGo::Goban::innerPlayMoveOnGobanCFG(const Location location,LocationList *locationListIncludingThisLocation) {/**assert(0);**/

  cfg->playMoveOnGoban(location);
  cfg->freeLocationUpdateAfterPlayMoveOnGoban(location,gobanLists,locationListIncludingThisLocation);

  if (cfg->tmpErasedStones().size()) {/**assert(0);**/
    cfg->selfKillLocationUpdate(*this,gobanState,gobanLists,playerColor,directionsArray);
  }

  //for the special locations: in fact one liberty locations:
  //enemy strings has but one liberty left
  //this move has but one liberty after being played.
  cfg->specialLocationUpdate(location,gobanState,gobanLists);

  if (ko>0)
    if (gobanState[ko]==EMPTY) {/**assert(0);**/
      if (!gobanLists.freeLocation.isLocationIn(ko))
        gobanLists.freeLocation.addLocation(ko);
    } else if (isOutsideStrange && gobanState[ko] == OUTSIDE)
      ko = 0;
    else {/**assert(0);**/
      GoTools::print("----------------------BUG in innerPlayMoveOnGobanCFG--------------------\n");
      GoTools::print("problem with ko.\n");
      GoTools::print("------------------------------------------------------------------------\n");
    }
  ko=cfg->testKo();

  //for the eating move just created.
  cfg->updateEatingMove(location,gobanLists);
  if (recheckEyeMode)
    recheckEyeLocationsAfterPlayMoveOnGoban(location);
  changePlayer();
  cfg->resetStringPreview();
  if (maintainGobanKey) {/**assert(0);**/
    updateGobanKey(location);
    gobanKeyUpdated=true;
  } else
    gobanKeyUpdated=false;
  lastLastLastPlayedMove=lastLastPlayedMove;
  lastLastPlayedMove=lastPlayedMove;
  lastPlayedMove = location;


  if (openingRangeMode >= 1 && gobanLists.openingRangeLocation.size())
    openingRangeLocationUpdateAfterMove(location);//FIXME OR

  return true;
}

bool MoGo::Goban::innerPlayMoveOnGobanMix(const Location location,LocationList *locationListIncludingThisLocation) {/**assert(0);**/
  assert(isStringInformationOn || isCFGOn);
  if (isStringInformationOn) {/**assert(0);**/
    //GoTools::print("stringInformation!");
    stringInformation.playMoveOnGoban(location);
  }
  if (isCFGOn) {/**assert(0);**/
    //GoTools::print("cfg!");
    if (isStringInformationOn)
      gobanState[location] = 0;
    cfg->playMoveOnGoban(location);
  }


  if (isStringInformationOn)
    freeLocationUpdateAfterPlayMoveOnGoban(location,locationListIncludingThisLocation);
  else if (isCFGOn)
    cfg->freeLocationUpdateAfterPlayMoveOnGoban(location,gobanLists,locationListIncludingThisLocation);
  else assert(0);

  if (isStringInformationOn) {/**assert(0);**/
    if (stringInformation.erasedStones.size())
      selfKillLocationUpdate();
  } else if (isCFGOn) {/**assert(0);**/
    if (cfg->tmpErasedStones().size())
      cfg->selfKillLocationUpdate(*this,gobanState,gobanLists,playerColor,directionsArray);
  }

  //for the special locations: in fact one liberty locations:
  //enemy strings has but one liberty left
  //this move has but one liberty after being played.
  if (isStringInformationOn)
    specialLocationUpdate(location,locationListIncludingThisLocation);
  else if (isCFGOn)
    cfg->specialLocationUpdate(location,gobanState,gobanLists);//why the parameters are different? because locationListIncludingThisLocation is never used! some historical reason.

  if (ko>0)
    if (gobanState[ko]==EMPTY) {/**assert(0);**/
      if (!gobanLists.freeLocation.isLocationIn(ko))
        gobanLists.freeLocation.addLocation(ko);
    } else if (isOutsideStrange && gobanState[ko] == OUTSIDE)
      ko = 0;
    else {/**assert(0);**/
      GoTools::print("---------------BUG in innerPlayMoveOnGobanMix----------------\n");
      GoTools::print("problem with ko.\n");
      textModeShowGoban(location);
      showFreeLocation();
      GoTools::print("-------------------------------------------------------------\n");
    }
  if (isStringInformationOn)
    ko = locationInformation.testKo;
  else if (isCFGOn)
    ko=cfg->testKo();
  //for the eating move just created.
  if (isStringInformationOn)
    updateEatingMove(location,locationListIncludingThisLocation);
  else if (isCFGOn)
    cfg->updateEatingMove(location,gobanLists);

  if (recheckEyeMode)
    recheckEyeLocationsAfterPlayMoveOnGoban(location);

  changePlayer();
  assert(isKoCorrect());//FIXME DEBUG
  if (isStringInformationOn)
    locationInformation.resetStringPreview();
  if (isCFGOn)
    cfg->resetStringPreview();


  if (maintainGobanKey) {/**assert(0);**/
    updateGobanKey(location);
    gobanKeyUpdated=true;
  } else
    gobanKeyUpdated=false;
  lastLastLastPlayedMove=lastLastPlayedMove;
  lastLastPlayedMove=lastPlayedMove;
  lastPlayedMove = location;


  if (openingRangeMode >= 1 && gobanLists.openingRangeLocation.size())
    openingRangeLocationUpdateAfterMove(location);//FIXME OR
  if (GoExperiments::debugMode)
    recheckStrings();//FIXME SLOW DEBUG DELETE
  return true;
}


// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::Goban::playPassMoveOnGoban() {

  if (ko>0)
    if (gobanState[ko]==EMPTY) {
      /*
      LocationList * locationListIncludingKo = getLocationListIncludingThisLocation(ko);
      if (!locationListIncludingKo)
        gobanLists.freeLocation.addLocation(ko);
        */ // FIXME change for the randomMode 9/10
      if (!gobanLists.freeLocation.isLocationIn(ko))
        gobanLists.freeLocation.addLocation(ko);
      //showFreeLocation();
      //getchar();
    } else {/**assert(0);**/
      GoTools::print("-----------------BUG in playPassMoveOnGoban------------------\n");
      GoTools::print("problem with ko.\n");
      GoTools::print("-------------------------------------------------------------\n");
    }


  if (playerColor==BLACK) playerColor=WHITE;
  else if (playerColor==WHITE) playerColor=BLACK;
  else assert(0);
  ko=0;
  if (isStringInformationOn)
    locationInformation.resetStringPreview();
  if (isCFGOn)
    cfg->resetStringPreview();
  updateKoInGobanKey();
  updatePlayerColorInGobanKey();
  gobanKeyUpdated=true;
  lastLastLastPlayedMove=lastLastPlayedMove;
  lastLastPlayedMove=lastPlayedMove;
  lastPlayedMove = PASSMOVE;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::Goban::changePlayer() {
  if (playerColor==BLACK) playerColor=WHITE;
  else if (playerColor==WHITE) playerColor=BLACK;
  else assert(0);
}


// int MoGo::Goban::width( ) const {/**assert(0);**/
//   assert(widthOfGoban>=0);
//   return widthOfGoban;
// }
//
// int MoGo::Goban::height( ) const {/**assert(0);**/
//   assert(heightOfGoban>=0);
//   return heightOfGoban;
// }

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::Goban::textModeShowGoban(const Location location, const bool showRangeOnly, const Bitmap * bitmap, const bool showGroupNumber) const {

  int HorSideWidth=6;
  //textModeShowPosition(location);
  GoTools::print("\n");
  int mode;
  if (widthOfGoban>13) mode=0;
  else mode=1;

  for(int i=0;i<HorSideWidth;i++) GoTools::print(" ");
  for(int i=0;i<widthOfGoban;i++)
    if (i>=8)
      GoTools::print(" %c ",(char)('A'+i+1));
    else
      GoTools::print(" %c ",(char)('A'+i));
  if (mode==1) {
    GoTools::print(" ");
    for(int i=0;i<HorSideWidth;i++) GoTools::print(" ");
    for(int i=0;i<widthOfGoban;i++)
      if (i>=8)
        GoTools::print(" %c ",(char)('A'+i+1));
      else
        GoTools::print(" %c ",(char)('A'+i));
  }
  GoTools::print("\n");
  for(int i=0;i<HorSideWidth-1;i++) GoTools::print(" "); GoTools::print("+");
  for(int i=0;i<widthOfGoban;i++) GoTools::print("---"); GoTools::print("+");
  if (mode==1) {
    for(int i=0;i<HorSideWidth-1;i++) GoTools::print(" "); GoTools::print("+");
    for(int i=0;i<widthOfGoban;i++) GoTools::print("---"); GoTools::print("+");
  }
  for(int i=0;i<heightOfGoban;i++) {
    GoTools::print("\n%2d",heightOfGoban-i);
    //if (i<9) GoTools::print(" ");
    for(int j=2;j<HorSideWidth-1;j++) GoTools::print(" ");
    GoTools::print("|");
    for(int j=0;j<widthOfGoban;j++) {
      int llocation=indexOnArray(i,j);
  

      if (0 && showRangeOnly /*&& rangeBorderLocation.isLocationIn(llocation)*/)
        GoTools::print("{/**assert(0);**/");
      else {


        if (bitmap && GobanBitmap::getBitValue(*bitmap,i,j))
          if (location == llocation)
            GoTools::print("<");
          else
            GoTools::print("[");
        else if (location == llocation)
          GoTools::print("(");
        else
          GoTools::print(" ");
      }
      /////////////
      if (gobanState[llocation]==WHITE) GoTools::print("O");
      else if (gobanState[llocation]==BLACK) GoTools::print("@");
      else if (gobanState[llocation]==EMPTY)
        if (showRangeOnly && !gobanLists.rangeLocation.isLocationIn(llocation))
          GoTools::print(" ");
        else
          GoTools::print(".");
      else if (gobanState[llocation] == OUTSIDE) //now possible with location simulation!
        GoTools::print("#");
      ///////////
      if (0 && showRangeOnly /*&& rangeBorderLocation.isLocationIn(llocation)*/)
        GoTools::print("}");
      else {
        if (bitmap && GobanBitmap::getBitValue(*bitmap,i,j))
          if (location == llocation)
            GoTools::print(">");
          else
            GoTools::print("]");
        else if (location == llocation)
          GoTools::print(")");
        else
          GoTools::print(" ");
      }



    }
    GoTools::print("| ");

    if (mode==1) {
      for(int j=2;j<HorSideWidth-1;j++) GoTools::print(" ");
      GoTools::print(" |");
      for(int j=0;j<widthOfGoban;j++) {
        int llocation=indexOnArray(i,j);
        //GoTools::print("[%d %d %d]",showGroupNumber,isCFGOn,cfg->isGroupUpToDate());
        if (showGroupNumber && isCFGOn && cfg->isGroupUpToDate()) {/**assert(0);**/
          if (cfg->getGroupNumber(llocation)<100) GoTools::print(" ");
          GoTools::print("%d",cfg->getGroupNumber(llocation));
          if (cfg->getGroupNumber(llocation)<10) GoTools::print(" ");
        } else if (isStringInformationOn) {
          if (stringInformation.getStringNumber(llocation)<100) GoTools::print(" ");
          GoTools::print("%d",stringInformation.getStringNumber(llocation));
          if (stringInformation.getStringNumber(llocation)<10) GoTools::print(" ");
        } else if (isCFGOn) {/**assert(0);**/
          if (cfg->getStringNumber(llocation)<100) GoTools::print(" ");
          GoTools::print("%d",cfg->getStringNumber(llocation));
          if (cfg->getStringNumber(llocation)<10) GoTools::print(" ");
        } else assert(0);

      }
      GoTools::print("|");
    }
    if (i == heightOfGoban/2-1) {
      GoTools::print("  Move %d: ",moveNumber_);
      textModeShowPosition(lastPlayedMove);
    } else if (i==heightOfGoban/2) {
      if (isBlacksTurn()) GoTools::print("  Black");
      else GoTools::print("  White");
      GoTools::print(" to play");
    } else if (i==heightOfGoban/2+1)
      GoTools::print("  Black eaten stone(s): %d",blackEatenStoneNumber);
    else if (i==heightOfGoban/2+2)
      GoTools::print("  White eaten stone(s): %d",whiteEatenStoneNumber);
  }
  GoTools::print("\n");
  for(int i=0;i<HorSideWidth-1;i++) GoTools::print(" "); GoTools::print("+");
  for(int i=0;i<widthOfGoban;i++) GoTools::print("---");
  GoTools::print("+");

  if (mode==1) {
    for(int i=0;i<HorSideWidth-1;i++) GoTools::print(" "); GoTools::print("+");
    for(int i=0;i<widthOfGoban;i++) GoTools::print("---");
    GoTools::print("+");
  }
  GoTools::print("\n");

  if (mode == 0 && showBigGobanStringNumber) {/**assert(0);**/
    for(int j=0;j<HorSideWidth;j++) GoTools::print(" ");
    for(int j=0;j<widthOfGoban;j++)
      if (j>=8)
        GoTools::print(" %c ",(char)('A'+j+1));
      else
        GoTools::print(" %c ",(char)('A'+j));
    GoTools::print("\n");
    for(int i=0;i<HorSideWidth-1;i++) GoTools::print(" "); GoTools::print("+");
    for(int i=0;i<widthOfGoban;i++) GoTools::print("---");
    GoTools::print("+\n");

    for (int i=0;i<heightOfGoban;i++) {/**assert(0);**/

      for(int j=0;j<HorSideWidth-2;j++) GoTools::print(" ");GoTools::print(" |");
      for(int j=0;j<widthOfGoban;j++) {/**assert(0);**/
        int llocation=indexOnArray(i,j);
        //GoTools::print("[%d %d %d]",showGroupNumber,isCFGOn,cfg->isGroupUpToDate());
        if (showGroupNumber && isCFGOn && cfg->isGroupUpToDate()) {/**assert(0);**/
          if (cfg->getGroupNumber(llocation)<100) GoTools::print(" ");
          GoTools::print("%d",cfg->getGroupNumber(llocation));
          if (cfg->getGroupNumber(llocation)<10) GoTools::print(" ");
        } else if (isStringInformationOn) {/**assert(0);**/
          if (stringInformation.getStringNumber(llocation)<100) GoTools::print(" ");
          GoTools::print("%d",stringInformation.getStringNumber(llocation));
          if (stringInformation.getStringNumber(llocation)<10) GoTools::print(" ");
        } else if (isCFGOn) {/**assert(0);**/
          if (cfg->getStringNumber(llocation)<100) GoTools::print(" ");
          GoTools::print("%d",cfg->getStringNumber(llocation));
          if (cfg->getStringNumber(llocation)<10) GoTools::print(" ");
        } else assert(0);

      }
      GoTools::print("|\n");
    }
    for(int i=0;i<HorSideWidth-1;i++) GoTools::print(" "); GoTools::print("+");
    for(int i=0;i<widthOfGoban;i++) GoTools::print("---");
    GoTools::print("+\n");
  }

}





Vector< Location > MoGo::Goban::legalAndNoUselessMoves( ) const {/**assert(0);**/
  Vector<Location> legalAndNoUselessMoveList;
  for (int i=0;i<heightOfGoban;i++)
    for (int j=0;j<widthOfGoban;j++) {/**assert(0);**/
      Location location=indexOnArray(i,j);
      if (isNotUseless(location))
        legalAndNoUselessMoveList.push_back(location);
     
    }
  return legalAndNoUselessMoveList;
}


// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::Goban::textModeShowPosition( const Location location ) const {
  if (location==PASSMOVE) {
    GoTools::print("pass ");
    return;
  }
  int x,y;
  x=xIndex(location);
  y=yIndex(location);

  if (y>=8)
    GoTools::print("%c%i ",(char)('A'+y+1),heightOfGoban-x);
  else
    GoTools::print("%c%i ",(char)('A'+y),heightOfGoban-x);
  if (heightOfGoban>9 && heightOfGoban-x<10) GoTools::print(" ");
  return;

}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
int MoGo::Goban::xIndex( const Location location ) const {
  return location/(widthOfGoban+2)-1;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
int MoGo::Goban::yIndex( const Location location ) const {
  return location%(widthOfGoban+2)-1;
}
void MoGo::Goban::getXYIndice(const Location & location, int & x, int & y) const {/**assert(0);**/

  x=location/(widthOfGoban+2)-1;
  y=location%(widthOfGoban+2)-1;
}

void MoGo::Goban::resize( int width, int height ) {/**assert(0);**/
  if (widthOfGoban == width && heightOfGoban == height) return;
  widthOfGoban=width; heightOfGoban=height;
  Goban tmpGoban(width,height);
  leftUpLocation_ = indexOnArray(0,0);
  rightDownLocation_ = indexOnArray(width-1,height-1);
  *this = tmpGoban;
  GobanLocation::resize(*this);
  GobanBitmap::initiate(*this);
  //initiateGoban();
  //clearGoban();
  return;
}


void MoGo::Goban::printLocationInformation(const Location location ) const {/**assert(0);**/
  GoTools::print("Location:");
  textModeShowPosition(location);
  GoTools::print(": stringNumber: %d stringLiberty: %d stringLength: %d \n",
                 stringInformation.getStringNumber(location),
                 stringInformation.getStringLiberty(location),
                 stringInformation.getStringLength(location));
  return;
}



double MoGo::Goban::score( bool inRangeOnly ) const {/**assert(0);**/
  if (inRangeOnly) return score();
  int score=0;
  Location location=leftUpLocation_;
  if (!slowScore) {/**assert(0);**/
    for (int i=0;i<heightOfGoban;i++,location += 2) {/**assert(0);**/
      for (int j=0;j<widthOfGoban;j++,location++)
        if (!inRangeOnly ||
            isInRangeLocation(location)) {/**assert(0);**/
          if (gobanState[location]==BLACK) score++;
          else if (gobanState[location]==WHITE) score--;
          else if (gobanState[location]==EMPTY)
            for (int k=0;k<4;k++) {/**assert(0);**/
              //assert(location+direction(k)<gobanState.size());
              //assert(location+direction(k)>=0);
              if (gobanState[location+direction(k)]==BLACK) {/**assert(0);**/
                score++;
                break;
              } else if (gobanState[location+direction(k)]==WHITE) {/**assert(0);**/
                score--;
                break;
              }
            }
        }
    }
  } else if (slowScore==1) {/**assert(0);**/
    for (int i=0;i<heightOfGoban;i++,location += 2) {/**assert(0);**/
      for (int j=0;j<widthOfGoban;j++,location++)
        if (!inRangeOnly ||
            isInRangeLocation(location)) {/**assert(0);**/
          if (gobanState[location]==BLACK) score++;
          else if (gobanState[location]==WHITE) score--;
          else if (gobanState[location]==EMPTY) {/**assert(0);**/
            int state=0;
            for (int k=0;k<4;k++)
              if (gobanState[location+direction(k)]==WHITE) {/**assert(0);**/ // The point can't go to BLACK
                if (state==1) // was black
                {/**assert(0);**/ state=0; break; }
                state=2;
              } else if (gobanState[location+direction(k)]==BLACK) {/**assert(0);**/ // The point can't go to WHITE
                if (state==2) // was white
                {/**assert(0);**/ state=0; break; }
                state=1;
              }
            if (state==1) score++; else if (state==2) score--;

          }
        }
    }
  } else {/**assert(0);**/ // attempt of japanese score!
    for (int i=0;i<heightOfGoban;i++,location += 2) {/**assert(0);**/
      for (int j=0;j<widthOfGoban;j++,location++)
        if (!inRangeOnly ||
            isInRangeLocation(location)) {/**assert(0);**/
          if (gobanState[location]==EMPTY) {/**assert(0);**/
            int state=0;
            for (int k=0;k<4;k++)
              if (gobanState[location+direction(k)]==WHITE) {/**assert(0);**/ // The point can't go to BLACK
                if (state==1) // was black
                {/**assert(0);**/ state=0; break; }
                state=2;
              } else if (gobanState[location+direction(k)]==BLACK) {/**assert(0);**/ // The point can't go to WHITE
                if (state==2) // was white
                {/**assert(0);**/ state=0; break; }
                state=1;
              }
            if (state==1) score++; else if (state==2) score--;

          }
        }
    }
    //     GoTools::print("score before %i\n", score);
    score+=whiteEatenStoneNumber-blackEatenStoneNumber;
    //     GoTools::print("score before %i\n", score);
  }
  //   this->textModeShowGoban();
  //   GoTools::print("score = %i\n", score);
  return (double)score;
}

double MoGo::Goban::score( const Bitmap & zoneBitmap ) const {
  assert((int)zoneBitmap.size() == heightOfGoban);
  int score=0;
  for (int i=0;i<heightOfGoban;i++)
    if (zoneBitmap[i])
      for (int j=0;j<widthOfGoban;j++) {/**assert(0);**/
        Location location = GobanLocation::locationXY(i,j);
        if (isInRangeLocation(location)) {/**assert(0);**/
          if (gobanState[location]==BLACK) score++;
          else if (gobanState[location]==WHITE) score--;
          else if (gobanState[location]==EMPTY)
            for (int k=0;k<4;k++) {/**assert(0);**/
              //assert(location+direction(k)<gobanState.size());
              //assert(location+direction(k)>=0);
              if (gobanState[location+direction(k)]==BLACK) {/**assert(0);**/
                score++;
                break;
              } else if (gobanState[location+direction(k)]==WHITE) {/**assert(0);**/
                score--;
                break;
              }
            }
        }
      }

  return (double)score;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
// TEMPORARY COMMENT: BLACK PLAYS 2 TIMES
int MoGo::Goban::uselessnessTest(const Location location, LocationList*& locationListIncludingThisLocation)  const//executed ALWAYS after basicTest(then stringPreview) returns LEGAL, nobody except yizao understands these terrible codes... TOO BAD.
{
  //assert(location!=PASSMOVE);
  assert(locationListIncludingThisLocation);
  //if (!locationListIncludingThisLocation) locationListIncludingThisLocation=getLocationListIncludingThisLocation(location);
  int testResult=0;//locationInformation.uselessnessTest(location);
  if (isStringInformationOn)
    testResult = locationInformation.uselessnessTest(location);
  else if (isCFGOn)
    testResult = cfg->uselessnessTest(location);
  else assert(0);

  //if (locationInformation.isLocationSelfAtari)    updateSelfAtari(location,locationListIncludingThisLocation,testResult);

  freeLocationUpdateAfterUselessnessTest(location,locationListIncludingThisLocation,testResult);//no call for  stringInformation and locationInformation.
  return testResult;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::Goban::freeLocationUpdateAfterBasicTest( const Location location , LocationList*& locationListIncludingThisLocation, int testResult ) const {
  assert(locationListIncludingThisLocation);
  if (testResult==SELFKILLMOVE) {
    // TEMPORARY COMMENT: THIS FUNCTION IS USED
    // TEMPORARY COMMENT: BLACK PLAYS 2 TIMES

    //deleteLocationFromList(location);
    locationListIncludingThisLocation->deleteLocation(location);
    if ((isStringInformationOn &&
         locationInformation.friendAdjacentStringLocation.size()) ||
        (isCFGOn &&
         cfg->tmpAdjacentFriendStrNbs().size()))
      if (playerColor==BLACK) {
        gobanLists.blackSelfKillLocation.addLocation(location);
        locationListIncludingThisLocation=&gobanLists.blackSelfKillLocation;
      } else {
        gobanLists.whiteSelfKillLocation.addLocation(location);
        locationListIncludingThisLocation=&gobanLists.whiteSelfKillLocation;
      }
    else {
      gobanLists.eyeLocation.addLocation(location);
      // i changed here yesterday before yesterday night (30/08 morning on CVS)
      //if (notUselessMode == 1 && !isStillEye(location)) {/**assert(0);**/
      //the infinit loop is caused by the line before.
      if (recheckEyeMode && !isStillEye(location)) {//FIXME RECHECKMODEhere too slow. isStillEye asserts the location is in the eyeLocation.
        gobanLists.eyeLocation.deleteLocation(location);
        if (gobanLists.freeLocation.isLocationIn(location))
          gobanLists.freeLocation.deleteLocation(location);
        if (playerColor==BLACK) {
          gobanLists.blackSelfKillLocation.addLocation(location);
          locationListIncludingThisLocation=&gobanLists.blackSelfKillLocation;
        } else {
          gobanLists.whiteSelfKillLocation.addLocation(location);
          locationListIncludingThisLocation=&gobanLists.whiteSelfKillLocation;
        }
        return;
      }//FIXME RECHECKMODE between these two recheckEyeMode, undertest.

      locationListIncludingThisLocation=&gobanLists.eyeLocation;
    }
    return;
  }
  if (testResult==KOILL)
    //deleteLocationFromList(ko);
    locationListIncludingThisLocation->deleteLocation(location);

}


// TEMPORARY COMMENT: THIS FUNCTION IS USED
// TEMPORARY COMMENT: BLACK PLAYS 2 TIMES
void MoGo::Goban::freeLocationUpdateAfterUselessnessTest( const Location location , LocationList *& locationListIncludingThisLocation, int testResult ) const {
  if (testResult==EYEFILLMOVE) {
    locationListIncludingThisLocation->deleteLocation(location);
    gobanLists.eyeLocation.addLocation(location);
    locationListIncludingThisLocation=&gobanLists.eyeLocation;
    return;
  }
 
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::Goban::freeLocationUpdateAfterPlayMoveOnGoban( const Location location, LocationList *locationListIncludingThisLocation ) {

  assert(locationListIncludingThisLocation);
  //deleteLocationFromList(location);
  //for the location played.
  //textModeShowPosition(location);GoTools::print("erased...");
  locationListIncludingThisLocation->deleteLocation(location);
  for (int i=0;i<(int)stringInformation.erasedStones.size();i++)
    if (gobanLists.rangeLocation.isLocationIn(stringInformation.erasedStones[i]))
      gobanLists.freeLocation.addLocation(stringInformation.erasedStones[i]);

}


// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::Goban::specialLocationUpdate( const Location location, LocationList * ) const {
  //When a string has but one liberty left, then his eye, if he has one, is no longer meaningful.
  //also the self atari move. this two situation leads to the delete of the previous and the add to the //new selfKillLocation list.
  for (int i=0;i<(int)locationInformation.enemyAdjacentStringLocation.size();i++)
    if (stringInformation.stringLiberty[stringInformation.stringNumber[locationInformation.enemyAdjacentStringLocation[i]]]==1) {
      int test = 1;
      for (int j=0;j<i;j++)
        if (stringInformation.stringNumber[locationInformation.enemyAdjacentStringLocation[i]]==stringInformation.stringNumber[locationInformation.enemyAdjacentStringLocation[j]]) {
          test = 0;
          break;
        }
      if (test)
        specialLocationUpdateForOneLibertyString(locationInformation.enemyAdjacentStringLocation[i]);

    }
  if (stringInformation.stringLiberty[stringInformation.stringNumber[location]]==1)
    specialLocationUpdateForOneLibertyString(location);
}


// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::Goban::specialLocationUpdateForOneLibertyString( const Location stringLocation ) const {
  //here we have a string, his color given, knowing that he has only one liberty.
  //if one eye tested before belongs to this string...

  Location lastLiberty=stringInformation.getOneLibertyOfString(stringLocation);
  if (!gobanLists.rangeLocation.isLocationIn(lastLiberty)) return;

  if (gobanLists.eyeLocation.isLocationIn(lastLiberty))
    gobanLists.eyeLocation.deleteLocation(lastLiberty);
  gobanLists.freeLocation.addLocation(lastLiberty);

  if (gobanState[stringLocation]==BLACK) {
    if (gobanLists.whiteSelfKillLocation.isLocationIn(lastLiberty))
      gobanLists.whiteSelfKillLocation.deleteLocation(lastLiberty);
    //gobanLists.whiteEatingLocation.addLocation(lastLiberty,stringLocation);//FIXME SPEED
    gobanLists.whiteEatingLocation.addLocation(lastLiberty);
  } else {
    if (gobanLists.blackSelfKillLocation.isLocationIn(lastLiberty))
      gobanLists.blackSelfKillLocation.deleteLocation(lastLiberty);
    //gobanLists.blackEatingLocation.addLocation(lastLiberty,stringLocation);//FIXME SPEED
    gobanLists.blackEatingLocation.addLocation(lastLiberty);
  }

}

Location MoGo::Goban::randomOutOneMove( const int mode ) const {/**assert(0);**/
  LocationList *locationListIncludingThisLocation=0;
  return randomOutOneMove(mode, locationListIncludingThisLocation, __thresholdSelfAtari,false);
}


// TEMPORARY COMMENT: THIS FUNCTION IS USED
// TEMPORARY COMMENT: BLACK PLAYS 2 TIMES
Location MoGo::Goban::randomOutOneMove( const int mode, LocationList *&locationListIncludingThisLocation, int thresholdSelfAtari, bool andPlay) const {
  locationListIncludingThisLocation=0;
  
  switch(mode)
    {
    case NORMAL_RANDOM_MODE : 
      return randomOutOneNotUselessMove(locationListIncludingThisLocation);
      
    case  EATING_PREFERED_RANDOM_MODE_1 :
      return randomOutOneNotUselessMoveEatingPrefered(locationListIncludingThisLocation);
      
    case  201544 :
      return randomOutOneMoveMode201544(locationListIncludingThisLocation, thresholdSelfAtari);
      
    case 2015441 :
      return randomOutOneMoveMode2015441(locationListIncludingThisLocation, thresholdSelfAtari);
      
    case  2015445 :
      return randomOutOneMoveMode2015445(locationListIncludingThisLocation, thresholdSelfAtari);
      
    case 10000 :
      return randomOutOneMoveMode10000(thresholdSelfAtari);
      
    default :
      assert(0);
    }
  
  //  if (mode == NORMAL_RANDOM_MODE)
  //     return randomOutOneNotUselessMove(locationListIncludingThisLocation);
  //   else if (mode == EATING_PREFERED_RANDOM_MODE_1)
  //     return randomOutOneNotUselessMoveEatingPrefered(locationListIncludingThisLocation);
  //   else 
  //     if (mode == 201544)
  //       return randomOutOneMoveMode201544(locationListIncludingThisLocation, thresholdSelfAtari);
  //     else if (mode == 2015441)
  //       return randomOutOneMoveMode2015441(locationListIncludingThisLocation, thresholdSelfAtari);
  //     else if (mode == 2015445)
  //       return randomOutOneMoveMode2015445(locationListIncludingThisLocation, thresholdSelfAtari);
  //     else if (mode == 10000)
  //       return randomOutOneMoveMode10000(thresholdSelfAtari);
  
  //   return randomOutOneNotUselessMoveEatingPrefered(locationListIncludingThisLocation);
  
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
// TEMPORARY COMMENT: BLACK PLAYS 2 TIMES
Location MoGo::Goban::randomOutOneMoveAndPlay(const int mode, int thresholdSelfAtari ) {
  LocationList *locationListIncludingThisLocation=0;
  Location location = randomOutOneMove(mode, locationListIncludingThisLocation, thresholdSelfAtari,true);
  if (locationListIncludingThisLocation==0)
    playMoveOnGoban(location);
  else {
    //textModeShowPosition(location);
    if (location==PASSMOVE) {
      playPassMoveOnGoban();
      return PASSMOVE;
    }
    //GoTools::print("randomOutOneMove:");
    //textModeShowPosition(location);
    if (updateCloseLiberties) {/**assert(0);**/
      addCloseLibertiesWhenPlaying(location);
    }
    moveNumber_++;
    if (isStringInformationOn)
      if (!isCFGOn)
        innerPlayMoveOnGoban(location,locationListIncludingThisLocation);
      else
        innerPlayMoveOnGobanMix(location,locationListIncludingThisLocation); //FIXME BOTHON
    else
      innerPlayMoveOnGobanCFG(location,locationListIncludingThisLocation);
  }
  return location;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
// TEMPORARY COMMENT: BLACK PLAYS 2 TIMES
Location MoGo::Goban::randomOutOneNotUselessMove( LocationList*& locationListIncludingThisLocation ) const {
  LocationList *opponentSelfKillLocation = 0;
  if (playerColor==BLACK) opponentSelfKillLocation=&gobanLists.whiteSelfKillLocation;
  else opponentSelfKillLocation=&gobanLists.blackSelfKillLocation;
  int counter = 0;

  while(gobanLists.freeLocation.size()+opponentSelfKillLocation->size()>0) {
    counter++;
    if (counter>heightOfGoban*widthOfGoban*2) {/**assert(0);**/
      GoTools::print("infinite loop occurs during randomOutOneNotUselessMove. Save the log and tell yizao!!!\n");
      textModeShowGoban();
      showFreeLocation();
      getchar();
      return PASSMOVE;
    }
    //showFreeLocation();
    int i=MoGo::GoTools::mogoRand(gobanLists.freeLocation.size()+opponentSelfKillLocation->size());
    Location location=0;
    if (i<gobanLists.freeLocation.size()) {
      location=gobanLists.freeLocation[i];
      if (gobanState[location]!=EMPTY) {/**assert(0);**/
        GoTools::print("during randomOutOneNotUselessMove the famous bug of having taken position in freeLocation list occured!\n");
        textModeShowGoban(location);
        showFreeLocation();
        GoTools::print("position ");
        textModeShowPosition(location);
        GoTools::print("removed from freeLocation.\n");
        gobanLists.freeLocation.deleteLocation(location);
      }
      locationListIncludingThisLocation = &gobanLists.freeLocation;
    } else {
      location=(*opponentSelfKillLocation)[i-gobanLists.freeLocation.size()];
      locationListIncludingThisLocation = opponentSelfKillLocation;
    }

    if (basicTest(location,locationListIncludingThisLocation)==LEGAL) {
      int testResult=uselessnessTest(location,locationListIncludingThisLocation);
      if (testResult==0)
        return location;
      if (moveChangedMode==0) {
        if (testResult==MOVECHANGED)
          return location;
      } else if (moveChangedMode==1) {/**assert(0);**/
        if (testResult==MOVECHANGED) {/**assert(0);**/
          if (locationListIncludingThisLocation == &gobanLists.freeLocation)
            gobanLists.freeLocation.deleteLocation(location);

          //       locationListIncludingThisLocation = opponentSelfKillLocation;

          continue;
        }
      } else if (moveChangedMode==2 && locationInformation.freeAdjacentLocation.size()>0 && (testResult==MOVECHANGED)) {/**assert(0);**/
        //         textModeShowGoban();
        Location changedMove=locationInformation.freeAdjacentLocation[0];
        //         GoTools::print("Changed from %i to %i\n", location, changedMove);
        //         textModeShowPosition(location); textModeShowPosition(changedMove);
        //   locationListIncludingThisLocation=getLocationListIncludingThisLocationForMovePlay(locationInformation.freeAdjacentLocation[0]);
        locationListIncludingThisLocation=0;
        if ((gobanLists.freeLocation.isLocationIn(changedMove) || opponentSelfKillLocation->isLocationIn(changedMove))&& isLegal(changedMove)) {/**assert(0);**/
          int r=locationInformation.uselessnessTest(changedMove);
          if (r==0 || r==MOVECHANGED)
            return changedMove;
        }
        if (testResult==MOVECHANGED) {/**assert(0);**/
          //         GoTools::print("cancel MOVECHANGED\n");
          return location;
        }
        //         GoTools::print("changed try again...\n");
      }
    }
  }
  return PASSMOVE;
}
Location MoGo::Goban::randomOutOneNotUselessMoveSuicide( LocationList*& locationListIncludingThisLocation ) const {/**assert(0);**/
  LocationList *opponentSelfKillLocation = 0;
  if (playerColor==BLACK) opponentSelfKillLocation=&gobanLists.whiteSelfKillLocation;
  else opponentSelfKillLocation=&gobanLists.blackSelfKillLocation;
  int counter = 0;

  while(gobanLists.freeLocation.size()+opponentSelfKillLocation->size()>0) {/**assert(0);**/
    counter++;
    if (counter>heightOfGoban*widthOfGoban*2) {/**assert(0);**/
      GoTools::print("infinite loop occurs during randomOutOneNotUselessMove. Save the log and tell yizao!!!\n");
      textModeShowGoban();
      showFreeLocation();
      getchar();
      return PASSMOVE;
    }
    //showFreeLocation();
    int i=MoGo::GoTools::mogoRand(gobanLists.freeLocation.size()+opponentSelfKillLocation->size());
    Location location=0;
    if (i<gobanLists.freeLocation.size()) {/**assert(0);**/
      location=gobanLists.freeLocation[i];
      if (gobanState[location]!=EMPTY) {/**assert(0);**/
        GoTools::print("during randomOutOneNotUselessMove the famous bug of having taken position in freeLocation list occured!\n");
        textModeShowGoban(location);
        showFreeLocation();
        GoTools::print("position ");
        textModeShowPosition(location);
        GoTools::print("removed from freeLocation.\n");
        gobanLists.freeLocation.deleteLocation(location);
      }
      locationListIncludingThisLocation = &gobanLists.freeLocation;
    } else {/**assert(0);**/
      location=(*opponentSelfKillLocation)[i-gobanLists.freeLocation.size()];
      locationListIncludingThisLocation = opponentSelfKillLocation;
    }

    if (basicTest(location,locationListIncludingThisLocation)==LEGAL) {/**assert(0);**/
      int testResult=uselessnessTest(location,locationListIncludingThisLocation);
      if (testResult==0)
        return location;
      if (moveChangedMode==0) {/**assert(0);**/
        if (testResult==MOVECHANGED)
          return location;
      } else if (moveChangedMode==1) {/**assert(0);**/
        if (testResult==MOVECHANGED) {/**assert(0);**/
          if (locationListIncludingThisLocation == &gobanLists.freeLocation)
            gobanLists.freeLocation.deleteLocation(location);

          //       locationListIncludingThisLocation = opponentSelfKillLocation;

          continue;
        }
      } else if (moveChangedMode==2 && locationInformation.freeAdjacentLocation.size()>0 && (testResult==MOVECHANGED)) {/**assert(0);**/
        //         textModeShowGoban();
        Location changedMove=locationInformation.freeAdjacentLocation[0];
        //         GoTools::print("Changed from %i to %i\n", location, changedMove);
        //         textModeShowPosition(location); textModeShowPosition(changedMove);
        //   locationListIncludingThisLocation=getLocationListIncludingThisLocationForMovePlay(locationInformation.freeAdjacentLocation[0]);
        locationListIncludingThisLocation=0;
        if ((gobanLists.freeLocation.isLocationIn(changedMove) || opponentSelfKillLocation->isLocationIn(changedMove))&& isLegal(changedMove)) {/**assert(0);**/
          int r=locationInformation.uselessnessTest(changedMove);
          if (r==0 || r==MOVECHANGED)
            return changedMove;
        }
        if (testResult==MOVECHANGED) {/**assert(0);**/
          //         GoTools::print("cancel MOVECHANGED\n");
          return location;
        }
        //         GoTools::print("changed try again...\n");
      }
    }
  }
  return PASSMOVE;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
int MoGo::Goban::approximateSizeOfPossibleMoves( ) const {
  //LocationList* locationListIncludingThisLocation;
  LocationList *opponentSelfKillLocation = 0;
  EatingLocationListType *playerEatingLocation = 0;
  if (playerColor==BLACK) {
    opponentSelfKillLocation=&gobanLists.whiteSelfKillLocation;
    playerEatingLocation=&gobanLists.blackEatingLocation;
  } else {
    opponentSelfKillLocation=&gobanLists.blackSelfKillLocation;
    playerEatingLocation=&gobanLists.whiteEatingLocation;
  }

  //int counter = 0 ;

  return gobanLists.freeLocation.size()+opponentSelfKillLocation->size();
}

Location MoGo::Goban::randomOutOneNotUselessMoveEatingPrefered(LocationList*& locationListIncludingThisLocation) const {/**assert(0);**/
  //Since I have used eatingLocation at the first version, so in order not the mix the name and make some misleading stories, I set playerEatingLocation as the name, meaning that player to play and eat opponent's stones.
  //FIXME be careful, dont mix opponentSelfKillLocation at opponentSelfAtariLocation.
  LocationList *opponentSelfKillLocation = 0;
  EatingLocationListType *playerEatingLocation = 0;
  if (playerColor==BLACK) {/**assert(0);**/
    opponentSelfKillLocation=&gobanLists.whiteSelfKillLocation;
    playerEatingLocation=&gobanLists.blackEatingLocation;
  } else {/**assert(0);**/
    opponentSelfKillLocation=&gobanLists.blackSelfKillLocation;
    playerEatingLocation=&gobanLists.whiteEatingLocation;
  }

  int counter = 0 ;

  while(gobanLists.freeLocation.size()+opponentSelfKillLocation->size()>0) {/**assert(0);**/
    //FIXME these lines below are probably useless and slowing MoGo. delete them soon.
    counter++;
    if (counter>gobanLists.freeLocation.size()+opponentSelfKillLocation->size()+100) {/**assert(0);**/
      GoTools::print("-----------------------BUG--------------------\nAn infinite loop occurs during the randomOutOneNotUselessMoveEatingPrefered. Stop obliged and passmove is returned.\ntell yizao please.\n");
      textModeShowGoban();
      showFreeLocation();
      GoTools::print("----------------------------------------------\n");
      return PASSMOVE;
    }

    Location location=0;
    if (playerEatingLocation->size()) {/**assert(0);**/
      //GoTools::print("!!!!!!!!playerEatingLocation size = %d\n",playerEatingLocation->size());
      //location=getBiggestEatingLocation(*playerEatingLocation);
      int index = 0;
      if (playerEatingLocation->size() > 1)
        index=MoGo::GoTools::mogoRand(playerEatingLocation->size());// here this trick helps to resolve the superko situation(the sequence is not determinist any more, and most of the superko disappear after this. However, 2% speed low-down.S
      //if (playerEatingLocation->size()>10)
      //GoTools::print("%d/%d|",index,playerEatingLocation->size());
      location=(*playerEatingLocation)[index];
      if (!isStillEatingMove(location)) {/**assert(0);**/
        playerEatingLocation->deleteLocation(location);
        continue;
      }
      //GoTools::print("move to test:");
      //textModeShowPosition(location);
      //GoTools::print("\n");
      if (gobanLists.freeLocation.isLocationIn(location)) {/**assert(0);**/
        if (gobanState[location]!=EMPTY) {/**assert(0);**/
          GoTools::print("during randomOutOneNotUselessMoveEatingPrefered, the famous bug of having taken position in freeLocation list occured!\n");
          textModeShowGoban(location);
          showFreeLocation();
          GoTools::print("position ");
          textModeShowPosition(location);
          GoTools::print("removed from freeLocation.\n");

          gobanLists.freeLocation.deleteLocation(location);
        }
        locationListIncludingThisLocation = &gobanLists.freeLocation;
      } else if (opponentSelfKillLocation->isLocationIn(location))
        locationListIncludingThisLocation = opponentSelfKillLocation;//FIXME the order is good?
      else {/**assert(0);**/
        if (!isOutsideStrange) {/**assert(0);**/
          textModeShowPosition(location);
          textModeShowGoban();
          showFreeLocation();
          assert(0);//FIXME
        }
        locationListIncludingThisLocation =  playerEatingLocation;
      }
    } else {/**assert(0);**/
      int i=MoGo::GoTools::mogoRand(gobanLists.freeLocation.size()+opponentSelfKillLocation->size());
      //if (gobanLists.freeLocation.size()+opponentSelfKillLocation->size()==70)
      //GoTools::print("%d/%d|",i,gobanLists.freeLocation.size()+opponentSelfKillLocation->size());
      if (i<gobanLists.freeLocation.size()) {/**assert(0);**/
        location=gobanLists.freeLocation[i];
        if (gobanState[location]!=EMPTY) {/**assert(0);**/
          GoTools::print("during randomOutOneNotUselessMoveEatingPrefered, the famous bug of having taken position in freeLocation list occured!\n");
          textModeShowGoban(location);
          showFreeLocation();
          GoTools::print("position ");
          textModeShowPosition(location);
          GoTools::print("removed from freeLocation.\n");

          gobanLists.freeLocation.deleteLocation(location);
        }
        locationListIncludingThisLocation=&gobanLists.freeLocation;
      } else {/**assert(0);**/
        location=(*opponentSelfKillLocation)[i-gobanLists.freeLocation.size()];
        locationListIncludingThisLocation=opponentSelfKillLocation;
      }
    }

    if (basicTest(location,locationListIncludingThisLocation)==LEGAL) {/**assert(0);**/
      //if (basicResult==LEGAL) {/**assert(0);**/
      int testResult=uselessnessTest(location,locationListIncludingThisLocation);
      //GoTools::print("testResult %d\n",testResult);
      if (testResult==0) {/**assert(0);**/
        //GoTools::print("after test\n");
        //showFreeLocation();
        return location;
      }
      if (testResult==MOVECHANGED) return location;
      //       if (testResult==MOVECHANGED) {/**assert(0);**/
      //         //locationInformation.resetStringPreview();
      //         basicTest(location,locationListIncludingThisLocation);
      //         locationInformation.stringPreview(locationInformation.freeAdjacentLocation[0]);
      //         return locationInformation.freeAdjacentLocation[0];
      //       }
    }
  }
  return PASSMOVE;
}




// TEMPORARY COMMENT: THIS FUNCTION IS USED
Location MoGo::Goban::koPosition( ) const {
  return ko;
}

PositionState MoGo::Goban::position( const Location location ) const {/**assert(0);**/
  return gobanState[location];
}
PositionState MoGo::Goban::positionWithChecking(const int i,const int j) const {/**assert(0);**/
  int v=(i+1)*(widthOfGoban+2)+(j+1);
  if (v<=0) return OUTSIDE;
  else if (v>=(int)gobanState.size()) return OUTSIDE;
  else return gobanState[v];
}

PositionState MoGo::Goban::position( const int i, const int j ) const {/**assert(0);**/
  return position(indexOnArray(i,j));
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::Goban::generateGobanKey( ) const {

  int size=heightOfGoban*widthOfGoban;
  if (size%20<=1) keyArray.resize(size/20+1);  //3^20<2^32<3^21, 81 = 361 mod 20
  else keyArray.resize(size/20+2);
  //GoTools::print("size of gobankey : %d\n",keyArray.size());
  keyArray[0]=(unsigned int)heightOfGoban+(unsigned int)widthOfGoban*32+(unsigned int)ko*1024; //height/width(probably we don't have this two size, what is the interest to have a rectangle goban?)/ko
  keyArray[0]+=(unsigned int)(playerColor-1)*(1024*512);//2^19;//Color of Player
  int state=gobanState[indexOnArray(0,0)]; if (state==OUTSIDE) state=EMPTY;
  keyArray[0]+=state*(1024*1024);//2^20; //trick here. For 9x9 and 19x19 goban, the situation of 20(no nore) positions could be saved in a 32bits type.

  //GoTools::print("%d\n",keyArray[0]);

  int c=-1,index=1;keyArray[1]=0;
  unsigned int three_exp=1;
  for (int i=1;i<widthOfGoban;i++) {
    c++;
    if (c==20) {/**assert(0);**/
      c=0;
      index++;
      keyArray[index]=0;
      three_exp=1;
    }
    state=gobanState[indexOnArray(0,i)]; if (state==OUTSIDE) state=EMPTY;
    keyArray[index]+=state*three_exp;
    three_exp*=3;
  }
  // changement of c before keyArray update and changement of three_exp after, is because, ... hard to say but rather tricky.
  for (int i=1;i<heightOfGoban;i++)
    for (int j=0;j<widthOfGoban;j++) {
      c++;
      if (c==20) {
        c=0;
        index++;
        keyArray[index]=0;
        three_exp=1;
      }
      state=gobanState[indexOnArray(i,j)]; if (state==OUTSIDE) state=EMPTY;
      keyArray[index]+=state*three_exp;
      three_exp*=3;
    }
  gobanKeyUpdated=true;
}

GobanKey MoGo::Goban::generateRotatedGobanKey( ) const {/**assert(0);**/
  GobanKey keyArray;
  int size=heightOfGoban*widthOfGoban;
  if (size%20<=1) keyArray.resize(size/20+1);  //3^20<2^32<3^21, 81 = 361 mod 20
  else keyArray.resize(size/20+2);
  //GoTools::print("size of gobankey : %d\n",keyArray.size());
  keyArray[0]=(unsigned int)heightOfGoban+(unsigned int)widthOfGoban*32+(unsigned int)rotate(ko)*1024; //height/width(probably we don't have this two size, what is the interest to have a rectangle goban?)/ko
  keyArray[0]+=(unsigned int)(playerColor-1)*(1024*512);//2^19;//Color of Player
  keyArray[0]+=gobanState[indexOnArray(0,heightOfGoban-1)]*(1024*1024);//2^20; //trick here. For 9x9 and 19x19 goban, the situation of 20(no nore) positions could be saved in a 32bits type.

  //GoTools::print("%d\n",keyArray[0]);

  int c=-1,index=1;keyArray[1]=0;
  unsigned int three_exp=1;
  for (int i=1;i<widthOfGoban;i++) {/**assert(0);**/
    c++;
    if (c==20) {/**assert(0);**/
      c=0;
      index++;
      keyArray[index]=0;
      three_exp=1;
    }
    keyArray[index]+=gobanState[indexOnArray(i,heightOfGoban-0-1)]*three_exp;
    three_exp*=3;

  }
  // changement of c before keyArray update and changement of three_exp after, is because, ... hard to say but rather tricky.
  for (int i=1;i<heightOfGoban;i++)
    for (int j=0;j<widthOfGoban;j++) {/**assert(0);**/
      c++;
      if (c==20) {/**assert(0);**/
        c=0;
        index++;
        keyArray[index]=0;
        three_exp=1;
      }
      keyArray[index]+=gobanState[indexOnArray(j,heightOfGoban-i-1)]*three_exp;
      three_exp*=3;
    }
  return keyArray;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
Location MoGo::Goban::rotate( Location location ) const {
  if (location == PASSMOVE)
    return PASSMOVE;
  int i=xIndex(location);int j=yIndex(location);
  return indexOnArray(heightOfGoban-j-1,i);
}

GobanKey MoGo::Goban::generateSymmetrizedGobanKey( ) const {/**assert(0);**/
  GobanKey keyArray;

  int size=heightOfGoban*widthOfGoban;
  if (size%20<=1) keyArray.resize(size/20+1);  //3^20<2^32<3^21, 81 = 361 mod 20
  else keyArray.resize(size/20+2);
  //GoTools::print("size of gobankey : %d\n",keyArray.size());
  keyArray[0]=(unsigned int)heightOfGoban+(unsigned int)widthOfGoban*32+(unsigned int)symmetrize(ko)*1024; //height/width(probably we don't have this two size, what is the interest to have a rectangle goban?)/ko
  keyArray[0]+=(unsigned int)(playerColor-1)*(1024*512);//2^19;//Color of Player
  keyArray[0]+=gobanState[indexOnArray(heightOfGoban-1,0)]*(1024*1024);//2^20; //trick here. For 9x9 and 19x19 goban, the situation of 20(no nore) positions could be saved in a 32bits type.

  //GoTools::print("%d\n",keyArray[0]);

  int c=-1,index=1;keyArray[1]=0;
  unsigned int three_exp=1;
  for (int i=1;i<widthOfGoban;i++) {/**assert(0);**/
    c++;
    if (c==20) {/**assert(0);**/
      c=0;
      index++;
      keyArray[index]=0;
      three_exp=1;
    }
    keyArray[index]+=gobanState[indexOnArray(heightOfGoban-0-1,i)]*three_exp;
    three_exp*=3;

  }
  // changement of c before keyArray update and changement of three_exp after, is because, ... hard to say but rather tricky.
  for (int i=1;i<heightOfGoban;i++)
    for (int j=0;j<widthOfGoban;j++) {/**assert(0);**/
      c++;
      if (c==20) {/**assert(0);**/
        c=0;
        index++;
        keyArray[index]=0;
        three_exp=1;
      }
      keyArray[index]+=gobanState[indexOnArray(heightOfGoban-i-1,j)]*three_exp;
      three_exp*=3;
    }
  return keyArray;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
Location MoGo::Goban::symmetrize( Location location ) const {
  if (location == PASSMOVE)
    return PASSMOVE;
  int i=xIndex(location);int j=yIndex(location);
  return indexOnArray(heightOfGoban-i-1,j);
}


// TEMPORARY COMMENT: THIS FUNCTION IS USED
const GobanKey &MoGo::Goban::getGobanKey( ) const {
  if (!gobanKeyUpdated)
    generateGobanKey();
  return keyArray;
}

GobanKey MoGo::Goban::getGobanKeyForSuperKo( ) const {/**assert(0);**/
  if (!gobanKeyUpdated)
    generateGobanKey();
  GobanKey tmpKeyArray = keyArray;
  tmpKeyArray[0]-=tmpKeyArray[0]%(1024*1024)/(1024*512)*(1024*512);
  tmpKeyArray[0]-=tmpKeyArray[0]%(1024*512)/1024*1024;
  return tmpKeyArray;
}



// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::Goban::initiateGobanKey( ) {
  keyArray.clear();
  generateGobanKey();
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::Goban::updateGobanKey( const Location location ) {
  updatePositionInGobanKey(location);
  updateKoInGobanKey();
  updatePlayerColorInGobanKey();
  if (isStringInformationOn)//FIXME CFG
    for (int i=0;i<(int)stringInformation.erasedStones.size();i++)
      updateErasedPositionInGobanKey(stringInformation.erasedStones[i]);
  else if (isCFGOn)
    for (int i=0;i<(int)cfg->tmpErasedStones().size();i++)
      updateErasedPositionInGobanKey(cfg->tmpErasedStones()[i]);
  else assert(0);

}

void MoGo::Goban::updatePositionInGobanKey( const Location location, GobanKey & key, int playerColor ) const {/**assert(0);**/
  // here we suppose that every update on a given position is from 0 to 1/2. Otherwise use updateErasedPositionInGobanKey.
  int i,j;
  i=xIndex(location);
  j=yIndex(location);
  if (i==0 && j==0)
    key[0]+=playerColor*1024*1024;
  else {/**assert(0);**/

    int c=i*widthOfGoban+j;
    //unsigned int three_exp=fastPow(3,(c-1)%20+1);
    //                  assert(threeExp[(c-1)%20+1]==three_exp);
    //keyArray[(c-1)/20+1]+=(unsigned int) gobanState[location]*three_exp;
    key[(c-1)/20+1]+=(unsigned int) playerColor*threeExp[(c-1)%20];

  }
}
void MoGo::Goban::updateKoInGobanKey( GobanKey & key, int ko ) const {/**assert(0);**/
  key[0]=key[0]-key[0]%(1024*512)/1024*1024+(unsigned int)ko*1024;
}

void MoGo::Goban::updatePlayerColorInGobanKey( GobanKey & key, int playerColor ) const {/**assert(0);**/
  key[0]=key[0]-key[0]%(1024*1024)/(1024*512)*(1024*512)+(unsigned int)(playerColor-1)*(1024*512);//2^19;//Color of Player
}



// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::Goban::updatePositionInGobanKey( const Location location ) {
  // here we suppose that every update on a given position is from 0 to 1/2. Otherwise use updateErasedPositionInGobanKey.
  int i,j;
  i=xIndex(location);
  j=yIndex(location);
  if (i==0 && j==0)
    keyArray[0]+=gobanState[indexOnArray(0,0)]*1024*1024;
  else {

    int c=i*widthOfGoban+j;
    //unsigned int three_exp=fastPow(3,(c-1)%20+1);
    //                  assert(threeExp[(c-1)%20+1]==three_exp);
    //keyArray[(c-1)/20+1]+=(unsigned int) gobanState[location]*three_exp;
    keyArray[(c-1)/20+1]+=(unsigned int) gobanState[location]*threeExp[(c-1)%20];

  }
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::Goban::initiateThreeExp() {
  if (threeExp.size() == 22)
    return;
  threeExp.resize(22);
  threeExp[0]=1;
  for (int i=1;i<22;i++)
    threeExp[i]=threeExp[i-1]*3;

}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::Goban::updateErasedPositionInGobanKey( const Location location ) {
  //This is supposed to be executed after the erase operation. However, might be faster if it is before, but the structure would be not very clear.
  int i,j;
  i=xIndex(location);
  j=yIndex(location);
  if (i==0 && j==0)
    keyArray[0]%=1024*1024;
  else {
    int c=i*widthOfGoban+j;
    int c1=(c-1)/20+1;int c2=(c-1)%20;
    //unsigned int three_exp=1;
    //for (int k=0;k<(c-1)%20+1;k++)
    //  three_exp*=3;
    //assert(threeExp[(c-1)%20+1]==three_exp);
    //keyArray[(c-1)/20+1]=keyArray[(c-1)/20+1]-keyArray[(c-1)/20+1]%(three_exp*3)/three_exp*three_exp+gobanState[location]*three_exp;
    keyArray[c1]=keyArray[c1]-keyArray[c1]%(threeExp[c2]*3)/threeExp[c2]*threeExp[c2]+gobanState[location]*threeExp[c2];
    //     unsigned int a=keyArray[c1]-keyArray[c1]%(threeExp[c2+1]*3)/threeExp[c2+1]*threeExp[c2+1]+gobanState[location]*threeExp[c2+1];
    //     assert(a==keyArray[c1]);

  }
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::Goban::updateKoInGobanKey( ) const {
  keyArray[0]=keyArray[0]-keyArray[0]%(1024*512)/1024*1024+(unsigned int)ko*1024;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::Goban::updatePlayerColorInGobanKey( ) const {
  keyArray[0]=keyArray[0]-keyArray[0]%(1024*1024)/(1024*512)*(1024*512)+(unsigned int)(playerColor-1)*(1024*512);//2^19;//Color of Player
}

void MoGo::Goban::printAllStringInformation( ) const {/**assert(0);**/
  stringInformation.printAllStringInformation();
}




void MoGo::Goban::printGobanKey( ) const {/**assert(0);**/
  for (int i=0;i<(int)keyArray.size();i++)
    GoTools::print("%d ",keyArray[i]);
  GoTools::print("\n");
}

void MoGo::Goban::showFreeLocation() const {/**assert(0);**/
  showFreeLocation(gobanLists);
}


void MoGo::Goban::showFreeLocation( const GobanLocationLists & lists ) const {/**assert(0);**/
  GoTools::print("%d range locations: ",lists.rangeLocation.size());
  for (int i=0;i<(int)lists.rangeLocation.size();i++)
    textModeShowPosition(lists.rangeLocation[i]);
  for (int location=0;location<(heightOfGoban+2)*(widthOfGoban+2);location++)
    if (lists.rangeLocation.index(location)>-1)
      if (location!=lists.rangeLocation[lists.rangeLocation.index(location)]) {/**assert(0);**/
        GoTools::print("Warning!");
        textModeShowPosition(location);
        textModeShowPosition(lists.rangeLocation[lists.rangeLocation.index(location)]);
        GoTools::print("|");
      }


  GoTools::print("\n%d free locations:  ",lists.freeLocation.size());
  for (int i=0;i<(int)lists.freeLocation.size();i++)
    textModeShowPosition(lists.freeLocation[i]);
  for (int location=0;location<(heightOfGoban+2)*(widthOfGoban+2);location++)
    if (lists.freeLocation.index(location)>-1)
      if (location!=lists.freeLocation[lists.freeLocation.index(location)]) {/**assert(0);**/
        GoTools::print("Warning!");
        textModeShowPosition(location);
        textModeShowPosition(lists.freeLocation[lists.freeLocation.index(location)]);
        GoTools::print("|");
      }

  GoTools::print("\n%d black eating locations: ",lists.blackEatingLocation.size());
  for (int i=0;i<(int)lists.blackEatingLocation.size();i++) {/**assert(0);**/
    //GoTools::print("|");
    textModeShowPosition(lists.blackEatingLocation[i]);
    GoTools::print("( ");

    //     for (int j=0;j<(int)lists.blackEatingLocation.relatedStrings(lists.blackEatingLocation[i]).size();j++)
    //       textModeShowPosition(lists.blackEatingLocation.relatedStrings(lists.blackEatingLocation[i])[j]);
    //GoTools::print("(%d)",blackEatingLocation.relatedStrings(blackEatingLocation[i])[j]);
    GoTools::print(")   ");//(%d)",eatingLocation.sumOfLengthOfRelatedStrings(i));
  }
  for (int location=0;location<(heightOfGoban+2)*(widthOfGoban+2);location++)
    if (lists.blackEatingLocation.index(location)>-1)
      if (location!=lists.blackEatingLocation[lists.blackEatingLocation.index(location)]) {/**assert(0);**/
        GoTools::print("Warning!");
        textModeShowPosition(location);
        textModeShowPosition(lists.blackEatingLocation[lists.blackEatingLocation.index(location)]);
        GoTools::print("|");
      }


  GoTools::print("\n%d white eating locations: ",lists.whiteEatingLocation.size());
  for (int i=0;i<(int)lists.whiteEatingLocation.size();i++) {/**assert(0);**/
    //GoTools::print("|");
    textModeShowPosition(lists.whiteEatingLocation[i]);
    GoTools::print("( ");

    //     for (int j=0;j<(int)lists.whiteEatingLocation.relatedStrings(lists.whiteEatingLocation[i]).size();j++)
    //       textModeShowPosition(lists.whiteEatingLocation.relatedStrings(lists.whiteEatingLocation[i])[j]);
    //GoTools::print("(%d)",whiteEatingLocation.relatedStrings(whiteEatingLocation[i])[j]);
    GoTools::print(")   ");//(%2d)",eatingLocation.sumOfLengthOfRelatedStrings(i));
  }
  for (int location=0;location<(heightOfGoban+2)*(widthOfGoban+2);location++)
    if (lists.whiteEatingLocation.index(location)>-1)
      if (location!=lists.whiteEatingLocation[lists.whiteEatingLocation.index(location)]) {/**assert(0);**/
        GoTools::print("Warning!");
        textModeShowPosition(location);
        textModeShowPosition(lists.whiteEatingLocation[lists.whiteEatingLocation.index(location)]);
        GoTools::print("|");
      }




  GoTools::print("\n%d eye locations: ",lists.eyeLocation.size());
  for(int i=0;i<(int)lists.eyeLocation.size();i++)
    textModeShowPosition(lists.eyeLocation[i]);
  for (int location=0;location<(heightOfGoban+2)*(widthOfGoban+2);location++)
    if (lists.eyeLocation.index(location)>-1)
      if (location!=lists.eyeLocation[lists.eyeLocation.index(location)]) {/**assert(0);**/
        GoTools::print("Warning!");
        textModeShowPosition(location);
        textModeShowPosition(lists.eyeLocation[lists.eyeLocation.index(location)]);
        GoTools::print("|");
      }


  GoTools::print("\n%d black self-kill locations: ",lists.blackSelfKillLocation.size());
  for (int i=0;i<(int)lists.blackSelfKillLocation.size();i++)
    textModeShowPosition(lists.blackSelfKillLocation[i]);
  for (int location=0;location<(heightOfGoban+2)*(widthOfGoban+2);location++)
    if (lists.blackSelfKillLocation.index(location)>-1)
      if (location!=lists.blackSelfKillLocation[lists.blackSelfKillLocation.index(location)]) {/**assert(0);**/
        GoTools::print("Warning!");
        textModeShowPosition(location);
        textModeShowPosition(lists.blackSelfKillLocation[lists.blackSelfKillLocation.index(location)]);
        GoTools::print("|");
      }


  GoTools::print("\n%d white self-kill locations: ",lists.whiteSelfKillLocation.size());
  for (int i=0;i<(int)lists.whiteSelfKillLocation.size();i++)
    textModeShowPosition(lists.whiteSelfKillLocation[i]);
  for (int location=0;location<(heightOfGoban+2)*(widthOfGoban+2);location++)
    if (lists.whiteSelfKillLocation.index(location)>-1)
      if (location!=lists.whiteSelfKillLocation[lists.whiteSelfKillLocation.index(location)]) {/**assert(0);**/
        GoTools::print("Warning!");
        textModeShowPosition(location);
        textModeShowPosition(lists.whiteSelfKillLocation[lists.whiteSelfKillLocation.index(location)]);
        GoTools::print("|");
      }
 


  GoTools::print("\nko ");
  textModeShowPosition(ko);
  GoTools::print("\n");


}



void MoGo::Goban::updateEatingMove( const Location location, LocationList *) const {/**assert(0);**/
  //This function is executed before the changement of player.
  EatingLocationListType *playerEatingLocation, *opponentEatingLocation;
  if (isBlacksTurn()) {/**assert(0);**/
    playerEatingLocation=&gobanLists.blackEatingLocation;
    opponentEatingLocation=&gobanLists.whiteEatingLocation;
  } else {/**assert(0);**/
    playerEatingLocation=&gobanLists.whiteEatingLocation;
    opponentEatingLocation=&gobanLists.blackEatingLocation;
  }
  int selfAtari=0;
  Location selfEatingLocation=-1;
  if (stringInformation.stringLiberty[stringInformation.stringNumber[location]]==1)
    //the move played right away is a self-atari
  {/**assert(0);**/
    selfEatingLocation=stringInformation.getOneLibertyOfString(location);

    //LocationList * locationListIncludingThisSelfEatingLocation = getLocationListIncludingThisLocation(selfEatingLocation);
    //if (locationListIncludingThisSelfEatingLocation)
    //locationListIncludingThisSelfEatingLocation->deleteLocation(selfEatingLocation);
    if (gobanLists.rangeLocation.isLocationIn(selfEatingLocation)) {/**assert(0);**/
      //       opponentEatingLocation->addLocation(selfEatingLocation,location);
      opponentEatingLocation->addLocation(selfEatingLocation);
      selfAtari=1;
    }
  }
  for (int i=0;i<(int)locationInformation.enemyAdjacentStringLocation.size();i++) {/**assert(0);**/
    if (stringInformation.stringLiberty[stringInformation.stringNumber[locationInformation.enemyAdjacentStringLocation[i]]]==1) {/**assert(0);**/
      int test = 1;
      for (int j=0;j<i;j++)
        if (stringInformation.stringNumber[locationInformation.enemyAdjacentStringLocation[i]]==stringInformation.stringNumber[locationInformation.enemyAdjacentStringLocation[j]]) {/**assert(0);**/
          test = 0;
          break;
        }
      if (test) {/**assert(0);**/
        Location eatingEnemyLocation = stringInformation.getOneLibertyOfString(locationInformation.enemyAdjacentStringLocation[i]);
        if (gobanLists.rangeLocation.isLocationIn(eatingEnemyLocation))
          playerEatingLocation->addLocation(eatingEnemyLocation);
        //           playerEatingLocation->addLocation(eatingEnemyLocation,locationInformation.enemyAdjacentStringLocation[i]);

        //         if (selfAtari) {/**assert(0);**/
        //           opponentEatingLocation->addLocation(selfEatingLocation,locationInformation.enemyAdjacentStringLocation[i]);
        //           playerEatingLocation->addLocation(eatingEnemyLocation,locationInformation.enemyAdjacentStringLocation[i]);
        //         }

      }
    }
  }
}


void MoGo::Goban::reCheckEatingLocation( ) const {/**assert(0);**/
  if (playerColor==BLACK)
    reCheckEatingLocation(gobanLists.whiteEatingLocation,BLACK);
  else
    reCheckEatingLocation(gobanLists.blackEatingLocation,WHITE);
}

void MoGo::Goban::reCheckEatingLocation( EatingLocationListType & eatingLocation , StoneColor  ) const {/**assert(0);**/
  assert(0);

  for (int i=0;i<(int)eatingLocation.size();i++) {/**assert(0);**/
    if (gobanState[eatingLocation[i]]==WHITE || gobanState[eatingLocation[i]]==BLACK) {/**assert(0);**/
      eatingLocation.deleteLocation(eatingLocation[i]);
      i--;
      continue;
      //FIXME
      showFreeLocation();
      textModeShowPosition(eatingLocation[i]);
      assert(gobanState[eatingLocation[i]]==EMPTY);
    }
    bool stillEatingMove = false;
 

    /*
        Vector < StringNumber > relatedStrings = eatingLocation.relatedStrings( eatingLocation[i] );
        for (int j=0;j<(int)relatedStrings.size();j++)
          if (stringInformation.stringLiberty[relatedStrings[j]]>1) eatingLocation.deleteRelatedString(eatingLocation[i],relatedStrings[j]);
        if (eatingLocation.relatedStrings(eatingLocation[i] ).size()==0) stillEatingMove=false;
    */
    if (!stillEatingMove) {/**assert(0);**/
      //Location location=eatingLocation[i];
      eatingLocation.deleteLocation(eatingLocation[i]);i--;
      //if (!getLocationListIncludingThisLocation(location))
      //freeLocation.addLocation(location);
    }
  }
  //showFreeLocation();
}



MoGo::LocationList* MoGo::Goban::getLocationListIncludingThisLocation( const Location location ) const {/**assert(0);**/
  if (gobanLists.freeLocation.isLocationIn(location))
    return &gobanLists.freeLocation;
  if (gobanLists.blackEatingLocation.isLocationIn(location))
    return &gobanLists.blackEatingLocation;
  if (gobanLists.whiteEatingLocation.isLocationIn(location))
    return &gobanLists.whiteEatingLocation;
  if (gobanLists.blackSelfKillLocation.isLocationIn(location)) return &gobanLists.blackSelfKillLocation;
  if (gobanLists.whiteSelfKillLocation.isLocationIn(location)) return &gobanLists.whiteSelfKillLocation;
  if (gobanLists.eyeLocation.isLocationIn(location))
    return &gobanLists.eyeLocation;
  //assert(0);
  return 0;//illegal move
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
MoGo::LocationList* MoGo::Goban::getLocationListIncludingThisLocationForMovePlay( const Location location ) const {
  if (gobanLists.freeLocation.isLocationIn(location)) {
    //GoTools::print("!");//FIXME
    if (gobanLists.blackEatingLocation.isLocationIn(location)) gobanLists.blackEatingLocation.deleteLocation(location);
    if (gobanLists.whiteEatingLocation.isLocationIn(location)) gobanLists.whiteEatingLocation.deleteLocation(location);
    //if (blackSelfAtariLocation.isLocationIn(location)) blackSelfAtariLocation.deleteLocation(location);
    //if (whiteSelfAtariLocation.isLocationIn(location)) whiteSelfAtariLocation.deleteLocation(location);
    return &gobanLists.freeLocation;
  }
  if (gobanLists.blackSelfKillLocation.isLocationIn(location)) return &gobanLists.blackSelfKillLocation;
  if (gobanLists.whiteSelfKillLocation.isLocationIn(location)) return &gobanLists.whiteSelfKillLocation;
  if (gobanLists.blackEatingLocation.isLocationIn(location)) {/**assert(0);**/
    //    if (whiteSelfKillLocation.isLocationIn(location))
    //      whiteSelfKillLocation.deleteLocation(location);
    //    if (whiteEatingLocation.isLocationIn(location))
    //      whiteEatingLocation.deleteLocation(location);
    return &gobanLists.blackEatingLocation;
  }
  if (gobanLists.whiteEatingLocation.isLocationIn(location)) {/**assert(0);**/
    //GoTools::print("whiteEatingLocation!");//FIXME

    //    if (blackSelfKillLocation.isLocationIn(location))
    //      blackSelfKillLocation.deleteLocation(location);
    //    if (blackEatingLocation.isLocationIn(location))
    //      blackEatingLocation.deleteLocation(location);
    return &gobanLists.whiteEatingLocation;
  }
  //if (blackSelfAtariLocation.isLocationIn(location)) return &blackSelfAtariLocation;
  //if (whiteSelfAtariLocation.isLocationIn(location)) return &whiteSelfAtariLocation;
  if (gobanLists.eyeLocation.isLocationIn(location)) {/**assert(0);**/
    gobanLists.freeLocation.addLocation(location);
    gobanLists.eyeLocation.deleteLocation(location);
    return &gobanLists.freeLocation;
  }
  
  //assert(0);
  return 0;//illegal move
}

bool MoGo::Goban::playMoveOnGoban( const int i, const int j ) {/**assert(0);**/
  return playMoveOnGoban(indexOnArray(i,j));
}

int MoGo::Goban::possibleMoveNumber( ) const {/**assert(0);**/
  int number=gobanLists.freeLocation.size();
  //if (playerColor==BLACK) number+=gobanLists.blackEatingLocation.size();
  //else number+=gobanLists.whiteEatingLocation.size();
  if (playerColor==BLACK) number+=gobanLists.whiteSelfKillLocation.size();
  else number+=gobanLists.blackEatingLocation.size();
  return number;
}

bool MoGo::Goban::isStillEatingMove( const Location location ) const {/**assert(0);**/
  //FIXME
  //if (locationInformation.basicTest(location)!=LEGAL) return false;
  if (gobanState[location]!=EMPTY) return false;
  if (ko==location) return false;
  for (int i=0;i<4;i++)
    if (gobanState[location+direction(i)]==3-playerColor)
      if ((isStringInformationOn &&
           stringInformation.stringLiberty[stringInformation.stringNumber[location+direction(i)]]==1) ||
          (isCFGOn &&
           cfg->getStringLiberty(cfg->getStringNumber(location+direction(i))) == 1)) return true;
  return false;
}

void MoGo::Goban::showOneLibertyStringsInGTPMode( ) const {/**assert(0);**/
  if (!isStringInformationOn) {/**assert(0);**/
    assert(isCFGOn);
    cfg->showOneLibertyStringsInGTPMode();
  } else {/**assert(0);**/
    int totalStringNumber=stringInformation.stringLiberty.size();
    int test = 1;
    for (int i=0;i<totalStringNumber;i++)
      if (stringInformation.stringLiberty[i]==1) {/**assert(0);**/
        for (int j=0;j<heightOfGoban;j++)
          for(int k=0;k<widthOfGoban;k++)
            if (stringInformation.stringNumber[indexOnArray(j,k)]==i) {/**assert(0);**/
              textModeShowPosition(indexOnArray(j,k));
              test = 0;
            }
        GoTools::print("\n");
      }
    if (test) GoTools::print("\n");
  }
}



// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::Goban::selfKillLocationUpdate( ) const {
  LocationList *locationList = 0;
  if (playerColor==BLACK)
    locationList = &gobanLists.blackSelfKillLocation;
  else if (playerColor==WHITE)
    locationList = &gobanLists.whiteSelfKillLocation;
  else assert(0);
  for (int i=0;i<(int)locationList->size();i++) {
    for (int j=0;j<4;j++)
      if (gobanState[(*locationList)[i]+direction(j)]==playerColor)
        if (stringInformation.stringLiberty[stringInformation.stringNumber[(*locationList)[i]+direction(j)]]>1) {
          assert(!gobanLists.freeLocation.isLocationIn((*locationList)[i]));
          if (gobanLists.rangeLocation.isLocationIn((*locationList)[i])) {
            gobanLists.freeLocation.addLocation((*locationList)[i]);
            locationList->deleteLocation((*locationList)[i]);
            i--;
            break;
          }
        }
  }
}



void MoGo::Goban::setSimulationRange( const Vector< Location > & rangeLocationList ) const {/**assert(0);**/
  gobanLists.rangeLocation.clear();
  gobanLists.freeLocation.clear();
  gobanLists.eyeLocation.clear();
  gobanLists.blackEatingLocation.clear();
  gobanLists.whiteEatingLocation.clear();
  gobanLists.blackSelfKillLocation.clear();
  gobanLists.whiteSelfKillLocation.clear();

  for (int i=0;i<(int)rangeLocationList.size();i++) {/**assert(0);**/
    if (rangeLocationList[i]==PASSMOVE)
      GoTools::print("warning! PASSMOVE in rangeLocation lists.\n");
    gobanLists.rangeLocation.addLocation(rangeLocationList[i]);

    if (gobanState[rangeLocationList[i]]==EMPTY)
      gobanLists.freeLocation.addLocation(rangeLocationList[i]);
  }
}

void MoGo::Goban::setSimulationRange( const LocationList & rangeLocationList ) const {/**assert(0);**/
  gobanLists.rangeLocation.clear();
  gobanLists.freeLocation.clear();
  gobanLists.eyeLocation.clear();
  gobanLists.blackEatingLocation.clear();
  gobanLists.whiteEatingLocation.clear();
  gobanLists.blackSelfKillLocation.clear();
  gobanLists.whiteSelfKillLocation.clear();

  for (int i=0;i<(int)rangeLocationList.size();i++) {/**assert(0);**/
    if (rangeLocationList[i]==PASSMOVE)
      GoTools::print("warning! PASSMOVE in rangeLocation lists.\n");
    gobanLists.rangeLocation.addLocation(rangeLocationList[i]);
    if (gobanState[rangeLocationList[i]]==EMPTY)
      gobanLists.freeLocation.addLocation(rangeLocationList[i]);
  }
}


void MoGo::Goban::setSimulationRangeGlobal( ) const {/**assert(0);**/
  //gobanLists.rangeLocation.clear();
  if (heightOfGoban*widthOfGoban == gobanLists.rangeLocation.size()) return;
  for (int i=0;i<heightOfGoban;i++)
    for (int j=0;j<widthOfGoban;j++) {/**assert(0);**/
      Location location = indexOnArray(i,j);
      if (!gobanLists.rangeLocation.isLocationIn(location)) {/**assert(0);**/
        gobanLists.rangeLocation.addLocation(location);
        assert(!gobanLists.freeLocation.isLocationIn(location));
        if (gobanState[location]==EMPTY)
          gobanLists.freeLocation.addLocation(location);
      }
    }

}



bool MoGo::Goban::isInRangeLocation( const Location location ) const {/**assert(0);**/
  return gobanLists.rangeLocation.isLocationIn(location);
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
// TEMPORARY COMMENT: BLACK PLAYS 2 TIMES
bool MoGo::Goban::isCut( const Location & location ) const {
  //double blackInterest = 0., whiteInterest = 0.;
  return locationInformation.isCut(location/*,blackInterest,whiteInterest*/);
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
// TEMPORARY COMMENT: BLACK PLAYS 2 TIMES
int MoGo::Goban::isSelfAtari( const Location & location/*, int number */) const {
  //double blackInterest = 0., whiteInterest = 0.;
  if (location==PASSMOVE)
    return 0;

  int l=locationInformation.isSelfAtari(location/*,number*//*,blackInterest,whiteInterest*/);
 
  return l;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
// TEMPORARY COMMENT: BLACK PLAYS 2 TIMES
bool MoGo::Goban::isHane( const Location & location ) const {
  //double blackInterest = 0., whiteInterest = 0.;
  return locationInformation.isHane(location/*,blackInterest,whiteInterest*/);
}

bool MoGo::Goban::isSolid( const Location & location ) const {/**assert(0);**/
  //double blackInterest = 0., whiteInterest = 0.;
  return locationInformation.isSolid(location/*,blackInterest,whiteInterest*/);
}








Location MoGo::Goban::randomOutMoveEatingOnly( LocationList *& locationListIncludingThisLocation ) const {/**assert(0);**/
  //Since I have used eatingLocation at the first version, so in order not the mix the name and make some misleading stories, I set playerEatingLocation as the name, meaning that player to play and eat opponent's stones.
  //FIXME be careful, dont mix opponentSelfKillLocation at opponentSelfAtariLocation.
  LocationList *opponentSelfKillLocation = 0;
  EatingLocationListType *playerEatingLocation = 0;
  if (playerColor==BLACK) {/**assert(0);**/
    opponentSelfKillLocation=&gobanLists.whiteSelfKillLocation;
    playerEatingLocation=&gobanLists.blackEatingLocation;
  } else {/**assert(0);**/
    opponentSelfKillLocation=&gobanLists.blackSelfKillLocation;
    playerEatingLocation=&gobanLists.whiteEatingLocation;
  }

  int counter = 0 ;

  while(playerEatingLocation->size()>0) {/**assert(0);**/
    //FIXME these lines below are probably useless and slowing MoGo. delete them soon.
    counter++;
    if (counter>gobanLists.freeLocation.size()+opponentSelfKillLocation->size()+100) {/**assert(0);**/
      GoTools::print("-----------------------BUG--------------------\nAn infinite loop occurs during the randomOutOneNotUselessMoveEatingPrefered. Stop obliged and passmove is returned.\ntell yizao please.\n");
      textModeShowGoban();
      showFreeLocation();
      GoTools::print("----------------------------------------------\n");
      return PASSMOVE;
    }

    Location location=0;
    //GoTools::print("!!!!!!!!playerEatingLocation size = %d\n",playerEatingLocation->size());
    //location=getBiggestEatingLocation(*playerEatingLocation);
    int index = 0;
    if (playerEatingLocation->size()>1)
      index=MoGo::GoTools::mogoRand(playerEatingLocation->size());// here this trick helps to resolve the superko situation(the sequence is not determinist any more, and most of the superko disappear after this. However, 2% speed low-down.S
    //if (playerEatingLocation->size()>10)
    //GoTools::print("%d/%d|",index,playerEatingLocation->size());
    location=(*playerEatingLocation)[index];
    if (!isStillEatingMove(location)) {/**assert(0);**/
      playerEatingLocation->deleteLocation(location);
      continue;
    }
    //GoTools::print("move to test:");
    //textModeShowPosition(location);
    //GoTools::print("\n");
    if (gobanLists.freeLocation.isLocationIn(location)) {/**assert(0);**/
      if (gobanState[location]!=EMPTY) {/**assert(0);**/
        GoTools::print("during randomOutOneNotUselessMoveEatingPrefered, the famous bug of having taken position in freeLocation list occured!\n");
        textModeShowGoban(location);
        showFreeLocation();
        GoTools::print("position ");
        textModeShowPosition(location);
        GoTools::print("removed from freeLocation.\n");

        gobanLists.freeLocation.deleteLocation(location);
      }
      locationListIncludingThisLocation = &gobanLists.freeLocation;
    } else if (opponentSelfKillLocation->isLocationIn(location))
      locationListIncludingThisLocation = opponentSelfKillLocation;
    else {/**assert(0);**/
      if (isOutsideStrange) {/**assert(0);**/
        assert(gobanLists.eyeLocation.isLocationIn(location));
        gobanLists.eyeLocation.deleteLocation(location);
        gobanLists.freeLocation.addLocation(location);
        locationListIncludingThisLocation = &gobanLists.freeLocation;
      } else {/**assert(0);**/
        textModeShowGoban();
        textModeShowPosition(location);
        showFreeLocation();
        GoTools::print("BUG!!! eating move is not in freeLocation neither in selfKillLocation.\n");
        //FIXME TODO
        locationListIncludingThisLocation = playerEatingLocation;
      }
    }

    if (basicTest(location,locationListIncludingThisLocation)==LEGAL) {/**assert(0);**/
      //if (basicResult==LEGAL) {/**assert(0);**/
      int testResult=uselessnessTest(location,locationListIncludingThisLocation);
      //GoTools::print("testResult %d\n",testResult);
      if (testResult==0) {/**assert(0);**/
        //GoTools::print("after test\n");
        //showFreeLocation();
        return location;
      }
      if (testResult==MOVECHANGED) return location;
      //       if (testResult==MOVECHANGED) {/**assert(0);**/
      //         //locationInformation.resetStringPreview();
      //         basicTest(location,locationListIncludingThisLocation);
      //         locationInformation.stringPreview(locationInformation.freeAdjacentLocation[0]);
      //         return locationInformation.freeAdjacentLocation[0];
      //       }
    }
  }
  return PASSMOVE;
}






Location MoGo::Goban::randomOutOneMoveProposedByOpeningRange( ) const {/**assert(0);**/
  //GoTools::print("one move proposed by openingRange!\n"); //FIXME OR
  //openingRangeLocationShow();
  int index = GoTools::mogoRand(gobanLists.openingRangeLocation.size());
  //textModeShowPosition(gobanLists.openingRangeLocation[index]);
  return gobanLists.openingRangeLocation[index];
}





Location MoGo::Goban::randomOutOneNotUselessMoveEatingPreferedAvoidingSelfAtari( LocationList *& locationListIncludingThisLocation, int thresholdSelfAtari ) const {/**assert(0);**/
  //  GoTools::print("randomOutOneNotUselessMoveEatingPreferedAvoidingSelfAtari\n");
  LocationList *opponentSelfKillLocation = 0;
  EatingLocationListType *playerEatingLocation = 0;
  if (playerColor==BLACK) {/**assert(0);**/
    opponentSelfKillLocation=&gobanLists.whiteSelfKillLocation;
    playerEatingLocation=&gobanLists.blackEatingLocation;
  } else {/**assert(0);**/
    opponentSelfKillLocation=&gobanLists.blackSelfKillLocation;
    playerEatingLocation=&gobanLists.whiteEatingLocation;
  }

  Location l=randomOutOneNotUselessMoveEatingPrefered(locationListIncludingThisLocation);
  removedLocationFromGobanAvoidingSelfAtariFreeLocation.clear();
  removedLocationFromGobanAvoidingSelfAtariEatingLocation.clear();
  removedLocationFromGobanAvoidingSelfAtariSelfKillLocation.clear();
  while (l!=PASSMOVE && avoidThisSelfAtariInSimulations(l, thresholdSelfAtari)/*(isSelfAtari(l)>=thresholdSelfAtari)*//* || (getStringLength(l)<__thresholdEatingMove && playerEatingLocation->isLocationIn(l))*/) {/**assert(0);**/
    //  GoTools::print("proposed move eating but self atari\n"); textModeShowPosition(l);
    //     if (getStringLength(l)<__thresholdEatingMove && playerEatingLocation->isLocationIn(l)) {/**assert(0);**/
    //     } else {/**assert(0);**/
    if (gobanLists.freeLocation.isLocationIn(l)) {/**assert(0);**/
      gobanLists.freeLocation.deleteLocation(l);
      removedLocationFromGobanAvoidingSelfAtariFreeLocation.push_back(l);
    } else {/**assert(0);**/
      assert(opponentSelfKillLocation->isLocationIn(l));
      opponentSelfKillLocation->deleteLocation(l);
      removedLocationFromGobanAvoidingSelfAtariSelfKillLocation.push_back(l);
    }
    //     }
    if (playerEatingLocation->isLocationIn(l)) {/**assert(0);**/
      playerEatingLocation->deleteLocation(l);
      removedLocationFromGobanAvoidingSelfAtariEatingLocation.push_back(l);
    }
    //     const_cast<MoGo::Goban *>(this)->gobanState[l]=OUTSIDE;
    l=randomOutOneNotUselessMoveEatingPrefered(locationListIncludingThisLocation);
  }
  for (int i=0;i<(int)removedLocationFromGobanAvoidingSelfAtariFreeLocation.size();i++)
    gobanLists.freeLocation.addLocation(removedLocationFromGobanAvoidingSelfAtariFreeLocation[i]);
  for (int i=0;i<(int)removedLocationFromGobanAvoidingSelfAtariEatingLocation.size();i++)
    playerEatingLocation->addLocation(removedLocationFromGobanAvoidingSelfAtariEatingLocation[i]);
  for (int i=0;i<(int)removedLocationFromGobanAvoidingSelfAtariSelfKillLocation.size();i++)
    opponentSelfKillLocation->addLocation(removedLocationFromGobanAvoidingSelfAtariSelfKillLocation[i]);

  return l;
}



Location MoGo::Goban::randomOutOneInterestingMoveGlobal( LocationList *& locationListIncludingThisLocation ) const {/**assert(0);**/
  int lineNb = GoTools::mogoRand(heightOfGoban-2)+1;
  interestingMoves.clear();
  Location location = indexOnArray(lineNb,1);
  for (int i=1;i<widthOfGoban-2;i++,location++)

    if ((gobanState[location] == EMPTY)) {/**assert(0);**/
      int isa = isSelfAtari(location);
      if (isa) continue;
      if (isAtari(location))
        interestingMoves.push_back(location);
      if (isEatingMove(location))
        interestingMoves.push_back(location);
      if (isHane(location))
        interestingMoves.push_back(location);
      else if (isCut(location))
        interestingMoves.push_back(location);
      else if (isCrossCut(location))
        interestingMoves.push_back(location);
    }

  while (interestingMoves.size() > 0) {/**assert(0);**/
    int index=0;
    if (interestingMoves.size() > 1)
      index=MoGo::GoTools::mogoRand(interestingMoves.size());
    if (isNotUseless(interestingMoves[index]))
      return interestingMoves[index];
    else {/**assert(0);**/
      interestingMoves[index]=interestingMoves[interestingMoves.size()-1];
      interestingMoves.pop_back();
    }
  }
  return randomOutOneNotUselessMove(locationListIncludingThisLocation);
}




bool MoGo::Goban::isNotGoodForOpening( const Location & location ) const {/**assert(0);**/
  //double blackInterest = 0.;
  //double whiteInterest = 0.;
  return locationInformation.isNotGoodForOpening(location/*,blackInterest,whiteInterest*/);

}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
// TEMPORARY COMMENT: BLACK PLAYS 2 TIMES
bool MoGo::Goban::isOnGobanSide( const Location & location ) const {
  return locationInformation.isOnGobanSide(location);
}

bool MoGo::Goban::isOutsideOfGoban( const Location & location ) const {/**assert(0);**/
  return locationInformation.isOutsideOfGoban(location);
}



bool MoGo::Goban::isCrossCut( const Location & location ) const {/**assert(0);**/
  //double blackInterest = 0.;
  //double whiteInterest = 0.;
  return locationInformation.isCrossCut(location/*,blackInterest,whiteInterest*/);

}

bool MoGo::Goban::isIsolatedMove( const Location & location ) const {/**assert(0);**/
  return locationInformation.isIsolatedMove(location);
}

bool MoGo::Goban::isAtariSavingMove( const Location & location ) const {/**assert(0);**/
  return locationInformation.isAtariSavingMove(location);
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
// TEMPORARY COMMENT: BLACK PLAYS 2 TIMES
bool MoGo::Goban::isAtariSavingMoveTrue( const Location & location ) const {
  return locationInformation.isAtariSavingMoveTrue(location);
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
int MoGo::Goban::moveNumber( ) const {
  return moveNumber_;
}

void MoGo::Goban::putStoneOnGobanByForce( const Location location, const StoneColor stoneColor, bool replaceAlreadyStone ) {/**assert(0);**/
  changedByForce = true;
  if (((gobanState[location]==BLACK || gobanState[location]==WHITE) && replaceAlreadyStone) || gobanState[location]==EMPTY)
    gobanState[location] = stoneColor;
}

void MoGo::Goban::recalculateGobanAfterForcedChangement( ) {/**assert(0);**/
  stringInformation.recalculateGobanAfterForcedChangement();
  locationListsClear();
  ko=0;
  generateGobanKey();
  for (int i=0;i<(int)gobanState.size();i++)
    if (gobanState[i]==EMPTY && gobanLists.rangeLocation.isLocationIn(i))
      gobanLists.freeLocation.addLocation(i);
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::Goban::showMoves( const Vector< Location > & moves ) const {
  for (int i=0;i<(int)moves.size();i++)
    textModeShowPosition(moves[i]);
}

void MoGo::Goban::addLocationToRange( const Location location ) const {/**assert(0);**/
  if (!gobanLists.rangeLocation.isLocationIn(location)) {/**assert(0);**/
    gobanLists.rangeLocation.addLocation(location);
    if (gobanState[location]==EMPTY)
      gobanLists.freeLocation.addLocation(location);
  }
}

void MoGo::Goban::deleteLocationFromRange( const Location location ) const {/**assert(0);**/
  if (gobanLists.rangeLocation.isLocationIn(location)) {/**assert(0);**/
    gobanLists.rangeLocation.deleteLocation(location);

    if (gobanLists.freeLocation.isLocationIn(location))
      gobanLists.freeLocation.deleteLocation(location);

    if (gobanLists.eyeLocation.isLocationIn(location))
      gobanLists.eyeLocation.deleteLocation(location);

    if (gobanLists.blackEatingLocation.isLocationIn(location))
      gobanLists.blackEatingLocation.deleteLocation(location);

    if (gobanLists.blackSelfKillLocation.isLocationIn(location))
      gobanLists.blackSelfKillLocation.deleteLocation(location);

    if (gobanLists.whiteEatingLocation.isLocationIn(location))
      gobanLists.whiteEatingLocation.deleteLocation(location);

    if (gobanLists.whiteSelfKillLocation.isLocationIn(location))
      gobanLists.whiteSelfKillLocation.deleteLocation(location);
  }
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
// TEMPORARY COMMENT: BLACK PLAYS 2 TIMES
Location MoGo::Goban::isAtari( const Location & location ) const {
  return locationInformation.isAtari(location);
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
int MoGo::Goban::getBlackEatenStoneNumber( ) const {
  return blackEatenStoneNumber;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
int MoGo::Goban::getWhiteEatenStoneNumber( ) const {
  return whiteEatenStoneNumber;
}

bool MoGo::Goban::isInterestingMove( const Location & location ) const {/**assert(0);**/
  //double blackInterest = 0., whiteInterest = 0.;
  return locationInformation.isInterestingMove(location/*,blackInterest,whiteInterest*/);
}

bool MoGo::Goban::isEatingMove( const Location & location ) const {/**assert(0);**/
  if (location==PASSMOVE)
    return false;
  return locationInformation.isEatingMove(location);
}
bool MoGo::Goban::isEatingMoveNotLadder( const Location & location ) const {/**assert(0);**/
  if (location==PASSMOVE)
    return false;
  int nb=0;
  for (int i=0;i<4;i++) {/**assert(0);**/
    if (gobanState[location+direction(i)]==EMPTY) nb++;
  }
  if (nb>2)
    return locationInformation.isEatingMove(location);
  else
    return false;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::Goban::setThresholdRandomSwitch( int t ) {
  MoGo::Goban::thresholdRandomSwitch=t;
}

int MoGo::Goban::isLastMoveAtari( ) const {/**assert(0);**/
  return locationInformation.isLastMoveAtari();
}

bool MoGo::Goban::isConnectingMove( const Location & location ) const {/**assert(0);**/
  return locationInformation.isConnectingMove(location);
}
int MoGo::Goban::isConnectingTwoDifferentStringsMove( const Location & location ) const {/**assert(0);**/
  return locationInformation.isConnectingTwoDifferentStringsMove(location);
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::Goban::setNotUselessMode( int notUselessMode ) {
  MoGo::Goban::notUselessMode = notUselessMode;
}

void MoGo::Goban::openingRangeLocationUpdateAfterMove( const Location & location ) const {/**assert(0);**/ //TODOSPEED
  //assert(0);//FIXME OR
  int distance = 2;
  if (heightOfGoban>13) distance = 3;
  Location tmpLocation;
  int x = 0, y = 0;
  getXYIndice(location,x,y);
  for (int i = -distance; i <= distance; i++)
    for (int j = -distance; j <= distance; j++) {/**assert(0);**/
      if (i+x <=1 || i+x >= heightOfGoban-2 || j+y <=1 || j+y >=widthOfGoban-2) continue;
      tmpLocation = indexOnArray(i+x,j+y);
      if (gobanLists.openingRangeLocation.isLocationIn(tmpLocation)) {/**assert(0);**/
        gobanLists.openingRangeLocation.deleteLocation(tmpLocation);
        if (!gobanLists.openingRangeLocation.size()) return;
      }
    }
}

void MoGo::Goban::openingRangeLocationInitiation( const int & sideDistance, bool isEmptyGoban ) const {/**assert(0);**/
  //assert(0);//FIXME OR
  gobanLists.openingRangeLocation.clear();
  for ( int i=sideDistance;i<heightOfGoban-sideDistance;i++)
    for ( int j=sideDistance;j<widthOfGoban-sideDistance;j++)
      gobanLists.openingRangeLocation.addLocation(indexOnArray(i,j));
  if (isEmptyGoban) return;
  Location location = leftUpLocation_;
  for (int i=0;i<heightOfGoban;i++, location+=2)
    for (int j=0;j<widthOfGoban;j++, location++)
      if (gobanState[location] != EMPTY) {/**assert(0);**/
        assert(gobanState[location] == BLACK || gobanState[location] == WHITE);
        openingRangeLocationUpdateAfterMove(location);
      }
}

void MoGo::Goban::setOpeningRangeMode( int openingRangeMode ) const {/**assert(0);**/
  if (openingRangeMode >= 1 && this->openingRangeMode == 0)
    openingRangeLocationInitiation();
  this->openingRangeMode = openingRangeMode;

}

void MoGo::Goban::openingRangeLocationShow( ) const {/**assert(0);**/
  setSimulationRange(gobanLists.openingRangeLocation);
  textModeShowGoban(PASSMOVE,true);
  setSimulationRangeGlobal();
}

const MoGo::LongLocationList & MoGo::Goban::openingRange( ) const {/**assert(0);**/
  return gobanLists.openingRangeLocation;
}

bool MoGo::Goban::isLastMoveMenacing( ) const {/**assert(0);**/
  return locationInformation.isLastMoveMenacing();
}

bool MoGo::Goban::isLastMoveMenacingMode2( ) const {/**assert(0);**/
  return locationInformation.isLastMoveMenacingMode2();
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
const Location & MoGo::Goban::leftUpLocation( ) const {
  return leftUpLocation_;
}

const Location & MoGo::Goban::rightDownLocation( ) const {/**assert(0);**/
  return rightDownLocation_;
}

bool MoGo::Goban::isIsolatedSideMove( const Location & location ) const {/**assert(0);**/
  return locationInformation.isIsolatedSideMove(location);
}

void MoGo::Goban::printCFGInformation( ) const {/**assert(0);**/
  cfg->printCFGInformation();
}

void MoGo::Goban::checkCFGByStringInformation( ) const {/**assert(0);**/
  if (!isCFGOn | !isStringInformationOn) {/**assert(0);**/
    GoTools::print("this function works only when CFG and StringInformation are both on.\n");
    return;
  }
  if (cfg->getMaxStringNumber() != stringInformation.stringLiberty.size()) {/**assert(0);**/
    GoTools::print("string numbers are not the same!\n");
    return;
  }
  for (int i=0;i<cfg->getMaxStringNumber();i++)
    if (cfg->getStringLiberty(i)) {/**assert(0);**/
      if (cfg->getStringLiberty(i)!=stringInformation.stringLiberty[i])
        GoTools::print("string liberties for string %d are not the same!\n",i);
      if (cfg->getStringLength(i)!=stringInformation.stringPositions[i].size())
        GoTools::print("string lengths for string %d are not the same!\n",i);
    }

}

Bitmap MoGo::Goban::getBitmap( ) const {/**assert(0);**/
  Bitmap tmpBitmap(heightOfGoban);
  Location location = leftUpLocation_;
  for (int i=0;i<heightOfGoban;i++,location+=2) {/**assert(0);**/
    tmpBitmap[i] = 0;
    for (int j=0;j<widthOfGoban;j++,location++)
      if (gobanState[location]!=EMPTY)
        GobanBitmap::setBitValue(tmpBitmap,i,j);
  }
  return tmpBitmap;
}

void MoGo::Goban::getBitmap( Bitmap & bitmap ) const {/**assert(0);**/
  GobanBitmap::clear(bitmap);
  Location location = leftUpLocation_;
  for (int i=0;i<heightOfGoban;i++,location+=2) {/**assert(0);**/
    bitmap[i] = 0;
    for (int j=0;j<widthOfGoban;j++,location++)
      if (gobanState[location]!=EMPTY)
        GobanBitmap::setBitValue(bitmap,i,j);
  }
}


Bitmap MoGo::Goban::changeLocationsToBitmap( const Vector< Location > & locations ) const {/**assert(0);**/
  Bitmap tmpBitmap(heightOfGoban);
  for (int i=0;i<(int)locations.size();i++) {/**assert(0);**/
    int x=0,y=0;
    getXYIndice(locations[i],x,y);
    GobanBitmap::setBitValue(tmpBitmap,x,y);
  }
  return tmpBitmap;
}

Bitmap MoGo::Goban::addLocationsToBitmap( const Vector< Location > & locations, Bitmap & bitmap ) const {/**assert(0);**/
  for (int i=0;i<(int)locations.size();i++) {/**assert(0);**/
    int x=0,y=0;
    getXYIndice(locations[i],x,y);
    GobanBitmap::setBitValue(bitmap,x,y);
  }
  return bitmap;
}


void MoGo::Goban::getLocationsFromBitmap( Bitmap & bitmap, Vector< Location > & locations ) const {/**assert(0);**/
  Location location = leftUpLocation_;
  for (int i=0;i<heightOfGoban;i++,location+=2)
    if (bitmap[i]) {/**assert(0);**/
      for (int j=0;j<widthOfGoban;j++,location++)
        if (GobanBitmap::getBitValue(bitmap,i,j))
          locations.push_back(location);
    } else
      location+=widthOfGoban;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
// TEMPORARY COMMENT: BLACK PLAYS 2 TIMES
void MoGo::Goban::setCFG( bool mode ) const {
  if (mode && !cfgMode) assert(0);
  if (!isCFGOn && mode)
    assert(0);// here I don't know how to do it cleverly. if i dont want to set cfg as mutable.
  isCFGOn = mode;
}

void MoGo::Goban::setStringInformation( bool mode) const {/**assert(0);**/
  isStringInformationOn = mode;
}

int MoGo::Goban::setBitmapAsSimulationRange( const Bitmap & bitmap, bool setOutside, double /*&*/ ) {/**assert(0);**/
  int size = 0;
  Location location = leftUpLocation_;
  for (int i=0;i<heightOfGoban;i++,location+=2)
    for (int j=0;j<widthOfGoban;j++,location++)
      if (!GobanBitmap::getBitValue(bitmap,i,j)) {/**assert(0);**/
        if (setOutside)
          gobanState[location] = OUTSIDE;
        deleteLocationFromRange(location);
      } else size++;
  isOutsideStrange = true;
  if (isStringInformationOn) {/**assert(0);**/
    stringInformation.clear();
    stringInformation.calculateAllFromGobanState();
    locationInformation.resetStringPreview();
  }
  if (isCFGOn) {/**assert(0);**/
    cfg->calculateAllFromGobanState(gobanState);
    cfg->resetStringPreview();
  }
 
  if (gobanState[ko] == OUTSIDE) ko = 0;
  return size;
}


int MoGo::Goban::setBitmapAsSimulationRange( const Bitmap & bitmap, const Bitmap & bitmap1, bool setOutside, double /*&*/ ) {/**assert(0);**/
  int size = 0;
  Location location = leftUpLocation_;
  for (int i=0;i<heightOfGoban;i++,location+=2)
    for (int j=0;j<widthOfGoban;j++,location++)
      if (!GobanBitmap::getBitValue(bitmap1,i,j)) {/**assert(0);**/
        if (setOutside)
          gobanState[location] = OUTSIDE;
        deleteLocationFromRange(location);
      } else if (!GobanBitmap::getBitValue(bitmap,i,j))
        deleteLocationFromRange(location);
      else size++;
  isOutsideStrange = true;

  if (isStringInformationOn) {/**assert(0);**/
    stringInformation.clear();
    stringInformation.calculateAllFromGobanState();
    locationInformation.resetStringPreview();
  }
  if (isCFGOn) {/**assert(0);**/
    cfg->calculateAllFromGobanState(gobanState);
    cfg->resetStringPreview();
  }
  /*
    assert(isStringInformationOn);
    double localAdvantageForBlack = 0.;
    for (int i=1;i<=stringInformation.maxNumberOfString;i++)
      localAdvantageForBlack+= gobanState[stringInformation.stringPositions[i][0]] == BLACK ? stringInformation.stringLength[i] : -stringInformation.stringLength[i];
    advantageForBlack += localAdvantageForBlack;
  */
  if (gobanState[ko] == OUTSIDE) ko = 0;
  return size;


}

int MoGo::Goban::setBitmapAsSimulationRangeViolentVersion( const Bitmap & bitmap, double /*&*/advantageForBlack ) {/**assert(0);**/
  Bitmap tmpBitmap = bitmap;
  GobanBitmap::dilation(tmpBitmap);
  GobanBitmap::logicalXOR(tmpBitmap,bitmap);

  Location location = leftUpLocation_;
  for (int i=0;i<heightOfGoban;i++,location+=2)
    if (tmpBitmap[i] == 0)
      location+=widthOfGoban;
    else
      for (int j=0;j<widthOfGoban;j++,location++)
        if (GobanBitmap::getBitValue(tmpBitmap,i,j))
          if ( i == 0 || i == heightOfGoban-1 || j == 0 || j == widthOfGoban-1 )
            GobanBitmap::setBitValue(tmpBitmap,i,j,false);
          else
            gobanState[location] = EMPTY;


  GobanBitmap::logicalOR(tmpBitmap,bitmap);
  //GoTools::print("ok\n");
  return setBitmapAsSimulationRange(bitmap,tmpBitmap,advantageForBlack);

}

bool MoGo::Goban::isTheSame( const Goban & goban ) const {/**assert(0);**/
  if (heightOfGoban != goban.height()) {/**assert(0);**/
    GoTools::print("height is not the same!(%d %d)\n",heightOfGoban,goban.height());
    return false;
  }
  if (widthOfGoban != goban.width()) {/**assert(0);**/
    GoTools::print("height is not the same!(%d %d)\n",widthOfGoban,goban.width());
    return false;
  }
  if (playerColor != goban.playerColor) {/**assert(0);**/
    GoTools::print("playerColor is not the same(%d %d)\n",playerColor, goban.playerColor);
    return false;
  }
  for (int i=0;i<heightOfGoban;i++)
    for (int j=0;j<widthOfGoban;j++)
      if (gobanState[GobanLocation::locationXY(i,j)] != goban.gobanState[GobanLocation::locationXY(i,j)]) {/**assert(0);**/
        GoTools::print("gobanState is not the same at");
        textModeShowPosition(GobanLocation::locationXY(i,j));
        GoTools::print("\n");
        textModeShowGoban();
        goban.textModeShowGoban();
        return false;
      }
  return true;
}


// TEMPORARY COMMENT: THIS FUNCTION IS USED
// TEMPORARY COMMENT: BLACK PLAYS 2 TIMES
bool MoGo::Goban::isStillEye( const Location location ) const {
  if (isStringInformationOn) {
    if (!locationInformation.isStillEye(location)) {
      gobanLists.eyeLocation.deleteLocation(location);
      gobanLists.freeLocation.addLocation(location);
      return false;
    }
    return true;
  } else if (isCFGOn) ;//TODO
  return true;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::Goban::recheckEyeLocationsAfterPlayMoveOnGoban( const Location location ) const {
  bool value = true;
  for (int i=4;i<8;i++)
    if (gobanLists.eyeLocation.isLocationIn(location+directionsArray[i]) &&
        !isStillEye(location+directionsArray[i]))
      value = false;
  return value;
}

void MoGo::Goban::setRecheckEyeMode( int mode ) {/**assert(0);**/
  recheckEyeMode = mode;
}
int MoGo::Goban::getRecheckEyeMode( ) {/**assert(0);**/
  return recheckEyeModeStatic;
}

void MoGo::Goban::updateLocationList( ) const {/**assert(0);**/
  locationListsClear();
  for (int i=0;i<heightOfGoban;i++)
    for (int j=0;j<widthOfGoban;j++)
      if (gobanState[GobanLocation::locationXY(i,j)] == EMPTY)
        gobanLists.freeLocation.addLocation(GobanLocation::locationXY(i,j));
  LocationList * locationListIncludingThisLocation = 0;
  locationListIncludingThisLocation = &gobanLists.freeLocation;
  for (int i=0;i<(int)gobanLists.freeLocation.size();i++) {/**assert(0);**/
    locationListIncludingThisLocation = &gobanLists.freeLocation;
    basicTest(gobanLists.freeLocation[i],locationListIncludingThisLocation);
    uselessnessTest(gobanLists.freeLocation[i],locationListIncludingThisLocation);
  }

  
}

bool MoGo::Goban::isLastMoveAtari( Location & underAtariStringFirstPosition ) const {/**assert(0);**/
  return locationInformation.isLastMoveAtari(underAtariStringFirstPosition);
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
// TEMPORARY COMMENT: BLACK PLAYS 2 TIMES
bool MoGo::Goban::isUnderAtariStringSavableByEatingMove( Location & underAtariStringFirstPosition, Location & savingMove ) const {
  
  return locationInformation.isUnderAtariStringSavableByEatingMove(underAtariStringFirstPosition,savingMove);
}

bool MoGo::Goban::isUnderAtariStringSavableByEatingMoveFastMode( Location & underAtariStringFirstPosition, Location & savingMove ) const {/**assert(0);**/
  return locationInformation.isUnderAtariStringSavableByEatingMoveFastMode(underAtariStringFirstPosition,savingMove);
}







bool MoGo::Goban::saveLastMoveAtari(Location &savingMove) const {/**assert(0);**/
  for (int i=0;i<(int)underAtariStringsFirstPosition.size()-1;i++)
    for (int j=i+1;j<(int)underAtariStringsFirstPosition.size();j++)
      if (stringInformation.getStringLength(underAtariStringsFirstPosition[i]) < stringInformation.getStringLength(underAtariStringsFirstPosition[j])) {/**assert(0);**/
        Location tmp = underAtariStringsFirstPosition[i];
        underAtariStringsFirstPosition[i] = underAtariStringsFirstPosition[j];
        underAtariStringsFirstPosition[j] = tmp;
      }

  for (int i=0;i<(int)underAtariStringsFirstPosition.size();i++) {/**assert(0);**/
    savingMove = PASSMOVE;
    if (isUnderAtariStringSavableByEatingMove(underAtariStringsFirstPosition[i],savingMove)) {/**assert(0);**/
      //         textModeShowPosition(savingMove);
      if (!isLegal(savingMove)) {/**assert(0);**/
        textModeShowGoban();
        textModeShowPosition(savingMove);
        assert(0);
      }
      if (isInRangeLocation(savingMove))
        return true;
    }
    savingMove = stringInformation.getOneLibertyOfString(underAtariStringsFirstPosition[i]);
    //       textModeShowPosition(savingMove);
    //       GoTools::print("savingMove isLegal %d isAtariSavingMoveTrue %d\n",isLegal(savingMove),isAtariSavingMoveTrue(savingMove));

    if (shishoLikeMoveForced) {/**assert(0);**///FIXME SHISHO
      for (int i=0;i<4;i++)
        if (!isInRangeLocation(savingMove+directionsArray[i]))
          if (gobanState[savingMove+directionsArray[i]] == EMPTY) return false;
    }

    if( isInRangeLocation(savingMove) &&
        isLegal(savingMove) &&
        isAtariSavingMoveTrue(savingMove))
      return true;
  }
  return false;
}
int MoGo::Goban::saveLastMoveAtariSize(Location &savingMove) const {/**assert(0);**/
  for (int i=0;i<(int)underAtariStringsFirstPosition.size()-1;i++)
    for (int j=i+1;j<(int)underAtariStringsFirstPosition.size();j++)
      if (stringInformation.getStringLength(underAtariStringsFirstPosition[i]) < stringInformation.getStringLength(underAtariStringsFirstPosition[j])) {/**assert(0);**/
        Location tmp = underAtariStringsFirstPosition[i];
        underAtariStringsFirstPosition[i] = underAtariStringsFirstPosition[j];
        underAtariStringsFirstPosition[j] = tmp;
      }

  for (int i=0;i<(int)underAtariStringsFirstPosition.size();i++) {/**assert(0);**/
    savingMove = PASSMOVE;
    if (isUnderAtariStringSavableByEatingMove(underAtariStringsFirstPosition[i],savingMove)) {/**assert(0);**/
      //         textModeShowPosition(savingMove);
      assert(isLegal(savingMove));
      if (isInRangeLocation(savingMove))
        return getStringLength(underAtariStringsFirstPosition[i]);
    }
    savingMove = stringInformation.getOneLibertyOfString(underAtariStringsFirstPosition[i]);
    //       textModeShowPosition(savingMove);
    //       GoTools::print("savingMove isLegal %d isAtariSavingMoveTrue %d\n",isLegal(savingMove),isAtariSavingMoveTrue(savingMove));

    if (shishoLikeMoveForced) {/**assert(0);**///FIXME SHISHO
      for (int i=0;i<4;i++)
        if (!isInRangeLocation(savingMove+directionsArray[i]))
          if (gobanState[savingMove+directionsArray[i]] == EMPTY) return 0;
    }

    if( isInRangeLocation(savingMove) &&
        isLegal(savingMove) &&
        isAtariSavingMoveTrue(savingMove))
      return getStringLength(underAtariStringsFirstPosition[i]);

  }
  return 0;
}


void MoGo::Goban::getInterestingMovesNearLastMove(int position1, int position2) const {/**assert(0);**/
  for (int i=position1;i<position2;i++) {/**assert(0);**/
    Location location = lastPlayedMove + direction(i);
    if (i>=8 && location<leftUpLocation_ || location>rightDownLocation_) continue;
    if (gobanState[location] == EMPTY &&
        isInRangeLocation(location)) {/**assert(0);**/
      int isa = isSelfAtari(location);
      if (isa) continue;
      if (isAtari(location))
        interestingMoves.push_back(location);
      if (isEatingMove(location))
        interestingMoves.push_back(location);
      if (isOnGobanSide(location)) {/**assert(0);**/
        if (locationInformation.isYoseOnSide(location))
          interestingMoves.push_back(location);
        continue;
      }
      //         else if (!(int)atariSavingMoves.size()) {/**assert(0);**/
      if (isHane(location))
        interestingMoves.push_back(location);
      else if (isCut(location))
        interestingMoves.push_back(location);
      else if (isCrossCut(location))
        interestingMoves.push_back(location);
      //       else if (isSolid(location))
      //         interestingMoves.push_back(location);
    }
  }
}
bool MoGo::Goban::randomOutOneMoveFromInterestingMoves(Location &location) const {/**assert(0);**/
  while (interestingMoves.size() > 0) {/**assert(0);**/
    int index=0;
    if (interestingMoves.size() > 1)
      index=MoGo::GoTools::mogoRand(interestingMoves.size());
    if (isNotUseless(interestingMoves[index])) {/**assert(0);**/
      location=interestingMoves[index];
      return true;
    } else {/**assert(0);**/
      interestingMoves[index]=interestingMoves[interestingMoves.size()-1];
      interestingMoves.pop_back();
    }
  }
  return false;
}

bool MoGo::Goban::randomOutOneMoveFromInterestingMovesWithChecking(Location &location, FastSmallVector<Location> &interestingMoves) const {/**assert(0);**/
  while (interestingMoves.size() > 0) {/**assert(0);**/
    int index=0;
    if (interestingMoves.size() > 1)
      index=MoGo::GoTools::mogoRand(interestingMoves.size());
    if (isNotUseless(interestingMoves[index]) && isInterestingMove44(interestingMoves[index])) {/**assert(0);**/
      location=interestingMoves[index];
      return true;
    } else {/**assert(0);**/
      interestingMoves[index]=interestingMoves[interestingMoves.size()-1];
      interestingMoves.pop_back();
    }
  }
  return false;
}


bool MoGo::Goban::randomOutOneMoveFromInterestingMovesWithSelfAtariChecking( Location & location, FastSmallVector< Location > & interestingMoves ) const {/**assert(0);**/
  while (interestingMoves.size() > 0) {/**assert(0);**/
    int index=0;
    if (interestingMoves.size() > 1)
      index=MoGo::GoTools::mogoRand(interestingMoves.size());
    if (isNotUseless(interestingMoves[index]) && (!isSelfAtari(interestingMoves[index]))) {/**assert(0);**/
      location=interestingMoves[index];
      return true;
    } else {/**assert(0);**/
      interestingMoves[index]=interestingMoves[interestingMoves.size()-1];
      interestingMoves.pop_back();
    }
  }
  return false;
}




































void MoGo::Goban::initiateNodeUrgency( GobanNode * node, int mode, const Bitmap * bitmap ) const {/**assert(0);**/
  if (mode == 0) initiateNodeUrgencyMode0(node);
  else if (mode == 1) initiateNodeUrgencyMode1(node,bitmap);
  else if (mode == 2) initiateNodeUrgencyMode2(node,bitmap);
  else if (mode == 3) initiateNodeUrgencyMode3(node);
  else if (mode == 4) initiateNodeUrgencyMode4(node);
  else if (mode == 5) initiateNodeUrgencyMode5(node);
  else if (mode == 6) initiateNodeUrgencyMode6(node);
  else assert(0);
}

void MoGo::Goban::initiateNodeUrgencyMode0( GobanNode * node ) const {/**assert(0);**/
  Location location = leftUpLocation_;
  for (int i=0;i<heightOfGoban;i++,location+=2)
    for (int j=0;j<widthOfGoban;j++,location++)
      if (isInRangeLocation(location) && isNotUseless(location)) //FIXME SLOW range test slows down.
        node->nodeUrgency().resetFirstPlayUrgency(location);
      else node->nodeUrgency().resetNeverPlayUrgency(location);
  if (InnerMCGoPlayer::shishoCheckMode && node->isShallowerThan(InnerMCGoPlayer::shishoCheckMode-2))//shallow nodes only!
    initiateNodeUrgencyForShisho(node);
  if (InnerMCGoPlayer::shishoCheckModeNew && node->isShallowerThan(InnerMCGoPlayer::shishoCheckModeNew-1)) // shallow nodes only!
    initiateNodeUrgencyForShishoNew(node);
  //node->nodeUrgency().resetFirstPlayUrgency(PASSMOVE);
}

void MoGo::Goban::initiateNodeUrgencyMode1( GobanNode * node, const Bitmap * bitmap ) const {/**assert(0);**/
  assert(bitmap);
  Location location = leftUpLocation_;
  for (int i=0;i<heightOfGoban;i++,location+=2)
    for (int j=0;j<widthOfGoban;j++,location++)
      if (isInRangeLocation(location) &&
          GobanBitmap::getBitValue(*bitmap,i,j) &&
          isNotUseless(location))
        node->nodeUrgency().resetFirstPlayUrgency(location);
      else
        node->nodeUrgency().resetNeverPlayUrgency(location);

  //if (shishoLikeMoveForced)
  //setRangeBorderUrgencyForShisho(node);
  //if (shishoReaderMode && isShishoMove(location))
  //node->nodeUrgency().resetNeverPlayUrgency(location);

  if (InnerMCGoPlayer::shishoCheckMode && node->isShallowerThan(InnerMCGoPlayer::shishoCheckMode-2))//shallow nodes only!
    initiateNodeUrgencyForShisho(node);
  if (InnerMCGoPlayer::shishoCheckModeNew && node->isShallowerThan(InnerMCGoPlayer::shishoCheckModeNew-1)) // shallow nodes only!
    initiateNodeUrgencyForShishoNew(node);
  //node->nodeUrgency().resetNeverPlayUrgency(PASSMOVE);//FIXME BLOCK here attention.
}

void MoGo::Goban::initiateNodeUrgencyMode2( GobanNode * node, const Bitmap * bitmap ) const {/**assert(0);**/
  assert(bitmap);
  Location location = leftUpLocation_;
  for (int i=0;i<heightOfGoban;i++,location+=2)
    for (int j=0;j<widthOfGoban;j++,location++)
      if (isInRangeLocation(location) &&
          isNotUseless(location))
        if (GobanBitmap::getBitValue(*bitmap,i,j))
          node->nodeUrgency().resetFirstPlayUrgency(location);
        else
          node->nodeUrgency().resetSecondPlayUrgency(location);
      else
        node->nodeUrgency().resetNeverPlayUrgency(location);
  //node->nodeUrgency().resetNeverPlayUrgency(PASSMOVE);//FIXME BLOCK here attention.

}

//void MoGo::MoveSelector1::initiateGobanNodeUrgencyForOpening( GobanNode * node, const Goban & goban) {/**assert(0);**/
void MoGo::Goban::initiateNodeUrgencyMode3( GobanNode * node) const {/**assert(0);**/
  //GoTools::print("opening!\n");
  Location location = leftUpLocation_;
  for (int i=0;i<heightOfGoban;i++,location+=2)
    for (int j=0;j<widthOfGoban;j++,location++) {/**assert(0);**/
      if (!isInRangeLocation(location) ||
          !isNotUseless(location)) {/**assert(0);**/
        node->nodeUrgency().resetNeverPlayUrgency(location);
        continue;
      }
      if ( (i<1 || j<1 || i>heightOfGoban-2 || j>width()-2) ||
           (heightOfGoban == 9 && (i == 1 || j == 1 || i == heightOfGoban-2 || j == widthOfGoban-2)) ) {/**assert(0);**/
        if (isIsolatedSideMove(location)) {/**assert(0);**/
          node->nodeUrgency().resetNeverPlayUrgency(location);
          continue;
        } else {/**assert(0);**/
          node->nodeUrgency().resetFirstPlayUrgency(location);
          continue;
        }
      }

      if (isNotGoodForOpening(location))
        node->nodeUrgency().resetNeverPlayUrgency(location);
      else
        node->nodeUrgency().resetFirstPlayUrgency(location);
    }
}


//void MoGo::MoveSelector1::initiateGobanNodeUrgencyForOpeningMode2( GobanNode * node, const Goban & goban ) {/**assert(0);**/
void MoGo::Goban::initiateNodeUrgencyMode4( GobanNode * node ) const {/**assert(0);**/

  //GoTools::print("openingMode = 2; %d,%d\n",moveNumber(),openingMoveNumber);
  setOpeningRangeMode(2);
  if (openingRange().size() < 3)
    initiateNodeUrgencyMode3(node);
  else {/**assert(0);**/
    Location location = leftUpLocation_;
    for (int i=0;i<heightOfGoban;i++,location+=2)
      for (int j=0;j<widthOfGoban;j++,location++)
        if (isInRangeLocation(location) &&
            gobanLists.openingRangeLocation.isLocationIn(location))
          node->nodeUrgency().resetFirstPlayUrgency(location);
        else
          node->nodeUrgency().resetNeverPlayUrgency(location);
  }
  if (isLastMoveMenacing()) {/**assert(0);**/
    //if (openingRange().size())
    for (int i=0;i<20;i++)
      if (lastPlayedMove+direction(i) >= leftUpLocation_ &&
          lastPlayedMove+direction(i) <= rightDownLocation() &&
          isNotUseless(lastPlayedMove+direction(i)))
        node->nodeUrgency().resetFirstPlayUrgency(lastPlayedMove+direction(i));
  }
}



void MoGo::Goban::initiateNodeUrgencyMode5( GobanNode * node ) const {/**assert(0);**/
  assert(!node->isNodeBitmapCreated());
  getBitmap(node->bitmap());
  for (int i=0;i<2;i++)
    GobanBitmap::dilation(node->bitmap());
  initiateNodeUrgencyMode1(node, &node->bitmap());
}

void MoGo::Goban::initiateNodeUrgencyMode6( GobanNode * node ) const {/**assert(0);**/
  assert(!node->isNodeBitmapCreated());
  getBitmap(node->bitmap());
  Bitmap tmpBitmap = node->bitmap();
  for (int i=0;i<2;i++)
    GobanBitmap::dilation(node->bitmap());
  GobanBitmap::logicalXOR(tmpBitmap,node->bitmap());
  for (int i=2;i<heightOfGoban-2;i++)
    for (int j=2;j<widthOfGoban-2;j++)
      if (GobanBitmap::getBitValue(tmpBitmap,i,j))
        if (locationInformation.isTooSlow(GobanLocation::locationXY(i,j)))
          GobanBitmap::setBitValue(node->bitmap(),i,j,false);
  initiateNodeUrgencyMode1(node, &node->bitmap());
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::Goban::setThresholdSelfAtari( int _thresholdSelfAtari ) {
  __thresholdSelfAtari=_thresholdSelfAtari;
}

bool MoGo::Goban::isYoseOnSide( const Location & location, int side ) const {/**assert(0);**/
  return locationInformation.isYoseOnSide(location, side);
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::Goban::isKoCorrect( ) const {
  //after changePlayer
  bool value = true;
  if (ko == PASSMOVE) return true;
  int deadStringNumber = 0;
  for (int i=0;i<4;i++) {
    Location location = ko+direction(i);
    if (gobanState[location] == EMPTY) {/**assert(0);**/value = false;break;}
    if (gobanState[location] == playerColor) {/**assert(0);**/value = false;break;}
    if (gobanState[location] == 3-playerColor &&
        getStringLiberty(location) == 1) {
      deadStringNumber++;
      if (getStringLength(location) > 1) {/**assert(0);**/value = false;break;}
      if (deadStringNumber>1) {/**assert(0);**/value = false;break;}
    }
  }
  if (value) return true;
  //textModeShowGoban();
  //printAllStringInformation();
  return false;
}


bool MoGo::Goban::setHandicap( const int number ) {/**assert(0);**/
  if (number<=0)
    return false;
  handicapLevel=number;
  Vector < Location > handicapStones;
  if (!GobanLocation::setHandicapStones(number, handicapStones)) return false;
  clearGoban();
  for (int i=0;i<(int)handicapStones.size()-1;i++) {/**assert(0);**/
    assert(playerColor == BLACK);
    playMoveOnGoban(handicapStones[i]);
    playMoveOnGoban(PASSMOVE);
  }
  playMoveOnGoban(handicapStones[handicapStones.size()-1]);
  assert(playerColor == WHITE);
  return true;
}

bool MoGo::Goban::setHandicap( const int number, Vector< Location > & handicapStones ) {/**assert(0);**/
  if (number<=0)
    return false;
  if (playerColor!=BLACK)
    return false;
  handicapStones.clear();
  if (!GobanLocation::setHandicapStones(number, handicapStones)) return false;
  clearGoban();
  for (int i=0;i<(int)handicapStones.size()-1;i++) {/**assert(0);**/
    assert(playerColor == BLACK);
    playMoveOnGoban(handicapStones[i]);
    playMoveOnGoban(PASSMOVE);
  }
  playMoveOnGoban(handicapStones[handicapStones.size()-1]);
  assert(playerColor == WHITE);
  return true;
}


void MoGo::Goban::recheckStrings( ) const {/**assert(0);**/
  bool value = true;
  if (isStringInformationOn) {/**assert(0);**/
    for (int nb=1;nb<=stringInformation.maxNumberOfString;nb++)
      if (stringInformation.stringLength[nb] > 0) {/**assert(0);**/
        if (stringInformation.stringLength[nb] != (int)stringInformation.stringPositions[nb].size()) {/**assert(0);**/
          GoTools::print("stringInformation error at stringNumber:%d!\n",nb);
          value = false;
        }
        if (stringInformation.stringLiberty[nb] == 0) {/**assert(0);**/
          GoTools::print("stringInformation error at stringNumber:%d!\n",nb);
          value = false;
        }
      }
    if (value == false) {/**assert(0);**/
      textModeShowGoban();showBigGobanStringNumber = true;
      stringInformation.printAllStringInformation();
      showFreeLocation();
    }
  }
  if (isCFGOn) {/**assert(0);**/
    int nb = 0;
    value = true;
    for (nb=1;nb<=cfg->stringCFG.maxNumberOfString;nb++)
      if ((int)cfg->stringCFG.stones[nb].size() > 0) {/**assert(0);**/
        if ((int)cfg->stringCFG.liberties[nb].size() == 0) {/**assert(0);**/
          GoTools::print("cfg error at stringNumber:%d!\n",nb);
          value = false;
          break;
        }
      }
    if (value == false) {/**assert(0);**/
      textModeShowGoban();showBigGobanStringNumber = true;
      cfg->stringCFG.printInformation();
      showFreeLocation();
    }

  }
}

bool MoGo::Goban::isBitmapGoodForRangeSimulation( const Bitmap & bitmap, const Bitmap & rangeBitmap ) const {/**assert(0);**/
  bool isBitmapGood = true;
  Bitmap tmp = bitmap;
  GobanBitmap::inversion(tmp);
  GobanBitmap::dilation(tmp);
  GobanBitmap::logicalAND(tmp,bitmap);
  for (int i=0;i<heightOfGoban;i++)
    if (tmp[i])
      for (int j=0;j<widthOfGoban;j++)
        if (GobanBitmap::getBitValue(tmp,i,j)) {/**assert(0);**/
          Location location = GobanLocation::locationXY(i,j);
          if (gobanState[location] == BLACK || gobanState[location] == WHITE)
            if (getStringLiberty(location) == 0) {/**assert(0);**/
              textModeShowPosition(location);
              GoTools::print("liberty errors!\n");
              isBitmapGood = false;
            } else {/**assert(0);**/
              StringNumber strNb = cfg->stringCFG.stringNumber[location];
              bool noProblem = false;
              for (int k=0;k<(int)cfg->stringCFG.liberties[strNb].size();k++) {/**assert(0);**/
                Location liberty = cfg->stringCFG.liberties[strNb][k];
                if (GobanBitmap::getBitValue(bitmap,liberty)) {/**assert(0);**/
                  noProblem = true;
                  break;
                } else if (GobanBitmap::getBitValue(rangeBitmap,liberty)) {/**assert(0);**/
                  noProblem = true;
                  break;
                }
              }
              if (!noProblem) {/**assert(0);**/
                textModeShowPosition(location);
                GoTools::print("liberty errors!\n");
                isBitmapGood = false;
              }
            }
        }
  if (!isBitmapGood) {/**assert(0);**/
    //textModeShowGoban(PASSMOVE,false,&bitmap);
    GobanBitmap::showBitmap(bitmap);
    GobanBitmap::showBitmap(rangeBitmap);
  }
  return isBitmapGood;
}

//  #define DEBUG_SHISHO
int MoGo::Goban::shishoReader( Location & shishoMove ) const {/**assert(0);**/
  if (lastPlayedMove == PASSMOVE) return 0;
#ifdef DEBUG_SHISHO
  GoTools::print("shishoReader\n");textModeShowPosition(lastPlayedMove);
  textModeShowGoban();
#endif
  StoneColor escapingStringColor = 3 - gobanState[lastPlayedMove];
  Location escapingStringHead__ = PASSMOVE;
  for (int i=0;i<4;i++) {/**assert(0);**/
    Location tmpLocation = lastPlayedMove+directionsArray[i];
    if (gobanState[tmpLocation] == escapingStringColor &&
        getStringLiberty(tmpLocation) == 1)
      if (escapingStringHead__ == PASSMOVE) escapingStringHead__ = tmpLocation;
      else return 0;
  }//lastPlayedMove is single atari.
  if (escapingStringHead__==PASSMOVE)
    return 0;
  Location lastLiberty = PASSMOVE;

  Location escapingStringHead=stringInformation.getHeadOfOneLibertyString(escapingStringHead__);
#ifdef DEBUG_SHISHO
  GoTools::print("escapingStringHead found\n");textModeShowPosition(escapingStringHead);
  GoTools::print("escapingStringHead = %i\n", escapingStringHead);
#endif
  for (int i=0;i<4;i++)
    if (gobanState[escapingStringHead+directionsArray[i]] == EMPTY) {/**assert(0);**/
      lastLiberty = escapingStringHead+directionsArray[i];
      break;
    }//get the lastLiberty for the under-atari string. only consider to liberty near.//TODO
#ifdef DEBUG_SHISHO
  GoTools::print("\nlastLiberty found\n");textModeShowPosition(lastLiberty);
#endif

  Location savingMove;
  if (isUnderAtariStringSavableByEatingMove(escapingStringHead__,savingMove))
    /*if (saveLastMoveAtari(savingMove) &&
        savingMove != lastLiberty)*/ return 1;//then can be saved by eatingstones. //bug FIX??,

#ifdef DEBUG_SHISHO
  GoTools::print("lastLiberty = %i\n", lastLiberty);
#endif
  int liberty = 0;
  for (int i=0;i<4;i++)
    if (gobanState[lastLiberty+directionsArray[i]] == EMPTY) liberty++;
    else if (gobanState[lastLiberty+directionsArray[i]] == escapingStringColor &&
             lastLiberty+directionsArray[i]!=escapingStringHead)
      return 0;

  if (liberty != 2)
    return 0;
#ifdef DEBUG_SHISHO
  GoTools::print("shishoSimulator to be lanced!\n");
#endif

  if (shishoSimulator(lastLiberty)) {/**assert(0);**/
    shishoMove = lastLiberty;
#ifdef DEBUG_SHISHO
    GoTools::print("shishoSimulator returns 2!\n");
#endif
    return 2;
  }
  return 1;
}

bool MoGo::Goban::shishoSimulator( Location lastLiberty, bool playerColorToPlay ) const {/**assert(0);**/
  {/**assert(0);**/
    StoneColor toPlayColor = EMPTY;
    Location locationString=PASSMOVE;
    for (int i=0;i<4;i++) {/**assert(0);**/
      Location tmpLocation = lastLiberty+directionsArray[i];
      if (gobanState[tmpLocation] == BLACK || gobanState[tmpLocation] == WHITE)
        if (getStringLiberty(tmpLocation) == 1) {/**assert(0);**/
          locationString=tmpLocation;
          if (toPlayColor == EMPTY)
            toPlayColor = gobanState[tmpLocation];
          else if (toPlayColor != gobanState[tmpLocation])
            return false;
        }
    }
    if (toPlayColor == EMPTY) return false;
    assert(locationString!=PASSMOVE);
    if (stringInformation.doesThisStringAtariOneOther(locationString))
      return false; // one enemy string is under atari, so it is not a ladder
  }

  Goban tmpGoban(*this);
  bool couldBeCaptured = false;
  int counter = 0;

  if (!playerColorToPlay) {/**assert(0);**/
    StoneColor toPlayColor = EMPTY;
    for (int i=0;i<4;i++) {/**assert(0);**/
      Location tmpLocation = lastLiberty+directionsArray[i];
      if (gobanState[tmpLocation] == BLACK ||
          gobanState[tmpLocation] == WHITE)
        if (getStringLiberty(tmpLocation) == 1)
          if (toPlayColor == EMPTY)
            toPlayColor = gobanState[tmpLocation];
          else if (toPlayColor != gobanState[tmpLocation])
            return false;
    }
    if (toPlayColor == EMPTY) return false;
    if (playerColor != toPlayColor)
      tmpGoban.playMoveOnGoban(PASSMOVE);
  }

  tmpGoban.setSimulationRangeGlobal();

  while(1) {/**assert(0);**/
#ifdef DEBUG_SHISHO
    GoTools::print("lastLiberty\n");textModeShowPosition(lastLiberty);
#endif
    counter ++;
    if (!tmpGoban.isLegal(lastLiberty)) {/**assert(0);**/
      couldBeCaptured = true;
      break;
    }
    bool willBeAtari=tmpGoban.isAtari(lastLiberty);
    tmpGoban.playMoveOnGoban(lastLiberty);
    if (tmpGoban.getStringLiberty(lastLiberty)>2) {/**assert(0);**/
#ifdef DEBUG_SHISHO
      GoTools::print("saved by liberty>2\n");
#endif
      couldBeCaptured = false;//saved!
      break;
    } else if (tmpGoban.getStringLiberty(lastLiberty) == 1) {/**assert(0);**/
#ifdef DEBUG_SHISHO
      GoTools::print("captured\n");
#endif
      couldBeCaptured = true;//captured!
      break;
    }
    if (willBeAtari) {/**assert(0);**/
#ifdef DEBUG_SHISHO
      GoTools::print("saved by atari\n");
#endif
      couldBeCaptured = false;//saved!
      break;
    }
    bool stop=false;
    for (int i=0;i<4;i++)
      if (tmpGoban.gobanState[lastLiberty+directionsArray[i]] == OUTSIDE) {/**assert(0);**/
#ifdef DEBUG_SHISHO
        GoTools::print("captured by side\n");
#endif
        couldBeCaptured = true;//captured!
        stop=true;
        break;
      }
    if (stop) break;

#ifdef DEBUG_SHISHO
    tmpGoban.textModeShowGoban(tmpGoban.lastMove());
#endif
    Location nextMove = PASSMOVE;
    if (tmpGoban.locationInformation.getShishoLikeMove(nextMove,lastLiberty))
      if (tmpGoban.isLegal(nextMove) && (!tmpGoban.isSelfAtari(nextMove)))
        tmpGoban.playMoveOnGoban(nextMove);
      else {/**assert(0);**/
        couldBeCaptured = false;
        break;
      }
    else break;//escaped!
  }
  //if (counter>5) {/**assert(0);**/
  //GoTools::print("shishoSimulator found something!\n");
  //tmpGoban.textModeShowGoban();
  //getchar();
  //}
  return couldBeCaptured;

}


void MoGo::Goban::shishoCheck(int shishoCheckMode, bool atRoot) const {/**assert(0);**/
  // GoTools::print("shishoCheck\n");
  for (int i=0;i<(int)shishoLocation.size();i++) {/**assert(0);**/
    // GoTools::print("shishoLocations %i\n");
    if (!isLegal(shishoLocation[i]) ||
        !shishoSimulator(shishoLocation[i],false)) {/**assert(0);**/
      shishoLocation.deleteLocation(shishoLocation[i]);
      i--;
    }
  }
  for (int i=0;i<(int)attackShishoLocation.size();i++) {/**assert(0);**/
    // GoTools::print("shishoLocations %i\n");
    if ((!isLegal(attackShishoLocation[i])) || allowAttackInLadder(attackShishoLocation[i])) {/**assert(0);**/
      attackShishoLocation.deleteLocation(attackShishoLocation[i]);
      i--;
    } else {/**assert(0);**/
      Goban tmpGoban(*this);
      tmpGoban.playMoveOnGoban(attackShishoLocation[i]);
      bool remove=true;
      for (int j=0;j<4;j++) {/**assert(0);**/
        Location tmpLocation = attackShishoLocation[i]+direction(j);
        if ((gobanState[tmpLocation] == WHITE || gobanState[tmpLocation] == BLACK) && getStringLiberty(tmpLocation) == 2)
          remove=false;
      }
      // GoTools::print("remove %i\n", (int)remove);
      Location location = PASSMOVE;
      if (remove || (tmpGoban.shishoReader(location)!=1)) {/**assert(0);**/
        attackShishoLocation.deleteLocation(attackShishoLocation[i]);
        i--;
      }
    }
  }

  Location location = PASSMOVE;
  if (shishoReader(location)==2)
    shishoLocation.addLocation(location);

  if (shishoCheckMode>1) {/**assert(0);**/
    Location nextMove=PASSMOVE, lastLiberty=PASSMOVE;
    if (locationInformation.getShishoLikeMove(nextMove,lastLiberty))
      if (isLegal(nextMove) && (!allowAttackInLadder(nextMove)) && (!isSelfAtari(nextMove))) {/**assert(0);**/
        Goban tmpGoban(*this);
        tmpGoban.playMoveOnGoban(nextMove);
        if (tmpGoban.shishoReader(nextMove)==1)
          attackShishoLocation.addLocation(nextMove);
      }
  }

  //textModeShowGoban();
  if (atRoot && (!InnerMCGoPlayer::dontDisplay)) {/**assert(0);**/
    GoTools::print("shishoCheck is called. Now we have %d shishoLocations: and %d attackShishoLocation",
                   shishoLocation.size(), attackShishoLocation.size());
    for (int i=0;i<(int)shishoLocation.size();i++)
      textModeShowPosition(shishoLocation[i]);
    GoTools::print("\n");
    for (int i=0;i<(int)attackShishoLocation.size();i++)
      textModeShowPosition(attackShishoLocation[i]);
    GoTools::print("\n");
  }
}

void MoGo::Goban::initiateNodeUrgencyForShisho( GobanNode * node, GoGameTree *tree ) const {/**assert(0);**/
  if (ko==PASSMOVE) {/**assert(0);**/ // in ko, we don't forbid any move
    if (node->fatherNode().size()==0 && (!InnerMCGoPlayer::dontDisplay))
      GoTools::print("initiateNodeUrgencyForShisho at root node!\n");
    for (int i=0;i<(int)shishoLocation.size();i++)
      if (!isEatingMove(shishoLocation[i])) {/**assert(0);**/
// GoTools::print("shishoLocation[%i], tree %i\n", shishoLocation[i], int(tree)); textModeShowPosition(shishoLocation[i]);
        node->nodeUrgency().resetNeverPlayUrgency(shishoLocation[i]);
        if (tree) tree->removeChild(node, shishoLocation[i]);
      }
    for (int i=0;i<(int)attackShishoLocation.size();i++)
      if (!isEatingMove(attackShishoLocation[i])) {/**assert(0);**/
        node->nodeUrgency().resetNeverPlayUrgency(attackShishoLocation[i]);
        if (tree) tree->removeChild(node, attackShishoLocation[i]);
      }
  }
}





Location MoGo::Goban::randomOutOneMoveMode201544( LocationList *& locationListIncludingThisLocation, int thresholdSelfAtari ) const {/**assert(0);**/
  locationListIncludingThisLocation=0;
  // GoTools::print("random\n");
  if ( lastPlayedMove == PASSMOVE) {/**assert(0);**/
    // GoTools::print("eating pref with PASSMOVE\n");
    //     showFreeLocation();
    //     printAllStringInformation();
    return randomOutOneNotUselessMoveEatingPreferedAvoidingSelfAtari(locationListIncludingThisLocation,thresholdSelfAtari);
  }

  underAtariStringsFirstPosition.clear();
  if (locationInformation.isLastMoveAtari(underAtariStringsFirstPosition)) {/**assert(0);**/
    //          GoTools::print("last move atari found!\n");

    for (int i=0;i<(int)underAtariStringsFirstPosition.size()-1;i++)
      for (int j=i+1;j<(int)underAtariStringsFirstPosition.size();j++)
        if (stringInformation.getStringLength(underAtariStringsFirstPosition[i]) < stringInformation.getStringLength(underAtariStringsFirstPosition[j])) {/**assert(0);**/
          Location tmp = underAtariStringsFirstPosition[i];
          underAtariStringsFirstPosition[i] = underAtariStringsFirstPosition[j];
          underAtariStringsFirstPosition[j] = tmp;
        }

    for (int i=0;i<(int)underAtariStringsFirstPosition.size();i++) {/**assert(0);**/
      Location savingMove = PASSMOVE;
      if (isUnderAtariStringSavableByEatingMove(underAtariStringsFirstPosition[i],savingMove)) {/**assert(0);**/
        //         textModeShowPosition(savingMove);
        assert(isLegal(savingMove));
        if (isInRangeLocation(savingMove)) {/**assert(0);**/
          locationListIncludingThisLocation=getLocationListIncludingThisLocationForMovePlay(savingMove);
          return savingMove;
        }
      }
      savingMove = stringInformation.getOneLibertyOfString(underAtariStringsFirstPosition[i]);
      //       textModeShowPosition(savingMove);
      //       GoTools::print("savingMove isLegal %d isAtariSavingMoveTrue %d\n",isLegal(savingMove),isAtariSavingMoveTrue(savingMove));

      if( isInRangeLocation(savingMove) &&
          isLegal(savingMove) &&
          isAtariSavingMoveTrue(savingMove)) {/**assert(0);**/
        locationListIncludingThisLocation=getLocationListIncludingThisLocationForMovePlay(savingMove);
        return savingMove;
      }
    }
    return randomOutOneNotUselessMoveEatingPreferedAvoidingSelfAtari(locationListIncludingThisLocation,thresholdSelfAtari);
  }
  // textModeShowPosition(lastPlayedMove);
  // GoTools::print(" lastPlayedMove \n");
  assert(lastPlayedMove != PASSMOVE);
  interestingMoves.clear();
  Location location = PASSMOVE;
  for (int i=0;i<8;i++) {/**assert(0);**/
    location = lastPlayedMove + direction(i);
    if (isInRangeLocation(location) && gobanState[location] == EMPTY) {/**assert(0);**/
      int isa = isSelfAtari(location);
      if (isa) continue;
      if (isAtari(location))
        interestingMoves.push_back(location);
      //       if (isEatingMove(location))
      //         interestingMoves.push_back(location);
      if (isOnGobanSide(location)) {/**assert(0);**///FIXME hey, i changed only two lines here.
        if (locationInformation.isYoseOnSide(location))
          interestingMoves.push_back(location);
        continue;
      }
      //         else if (!(int)atariSavingMoves.size()) {/**assert(0);**/
      if (isHane(location))
        interestingMoves.push_back(location);
      else if (isCut(location))
        interestingMoves.push_back(location);
      //       else if (isCrossCut(location))
      //         interestingMoves.push_back(location);
      //       else if (isSolid(location))
      //         interestingMoves.push_back(location);
    }
  }
  while (interestingMoves.size() > 0) {/**assert(0);**/
    int index=0;
    if (interestingMoves.size() > 1)
      index=MoGo::GoTools::mogoRand(interestingMoves.size());
    if (isNotUseless(interestingMoves[index])) {/**assert(0);**/
      locationListIncludingThisLocation=getLocationListIncludingThisLocationForMovePlay(interestingMoves[index]);
      return interestingMoves[index];
    } else {/**assert(0);**/
      interestingMoves[index]=interestingMoves[interestingMoves.size()-1];
      interestingMoves.pop_back();
    }
  }

  // GoTools::print("eating pref\n");
  //     showFreeLocation();
  //     printAllStringInformation();

  return randomOutOneNotUselessMoveEatingPreferedAvoidingSelfAtari(locationListIncludingThisLocation,thresholdSelfAtari);
}



void MoGo::Goban::getInterestingMovesGlobal() const {/**assert(0);**/
  LocationList *opponentSelfKillLocation = 0;
  if (playerColor==BLACK) {/**assert(0);**/
    opponentSelfKillLocation=&gobanLists.whiteSelfKillLocation;
  } else {/**assert(0);**/
    opponentSelfKillLocation=&gobanLists.blackSelfKillLocation;
  }
  for (int i=0;i<(int)opponentSelfKillLocation->size();i++) {/**assert(0);**/
    Location location = (*opponentSelfKillLocation)[i];
    if (gobanState[location] == EMPTY &&
        isInRangeLocation(location)) {/**assert(0);**/
      int isa = isSelfAtari(location);
      if (isa) continue;
      if (isAtari(location))
        interestingMoves.push_back(location);
      if (isEatingMove(location))
        interestingMoves.push_back(location);
      if (isOnGobanSide(location)) {/**assert(0);**/
        if (locationInformation.isYoseOnSide(location))
          interestingMoves.push_back(location);
        continue;
      }
      //         else if (!(int)atariSavingMoves.size()) {/**assert(0);**/
      if (isHane(location))
        interestingMoves.push_back(location);
      else if (isCut(location))
        interestingMoves.push_back(location);
      else if (isCrossCut(location))
        interestingMoves.push_back(location);
      //       else if (isSolid(location))
      //         interestingMoves.push_back(location);
    }
  }
  for (int i=0;i<gobanLists.freeLocation.size();i++) {/**assert(0);**/
    Location location = gobanLists.freeLocation[i];
    if (gobanState[location] == EMPTY &&
        isInRangeLocation(location)) {/**assert(0);**/
      int isa = isSelfAtari(location);
      if (isa) continue;
      if (isAtari(location))
        interestingMoves.push_back(location);
      if (isEatingMove(location))
        interestingMoves.push_back(location);
      if (isOnGobanSide(location)) {/**assert(0);**/
        if (locationInformation.isYoseOnSide(location))
          interestingMoves.push_back(location);
        continue;
      }
      //         else if (!(int)atariSavingMoves.size()) {/**assert(0);**/
      if (isHane(location))
        interestingMoves.push_back(location);
      else if (isCut(location))
        interestingMoves.push_back(location);
      else if (isCrossCut(location))
        interestingMoves.push_back(location);
      //       else if (isSolid(location))
      //         interestingMoves.push_back(location);
    }
  }

}
















bool MoGo::Goban::isInterestingMove44( const Location & location ) const {/**assert(0);**/
  if (gobanState[location] == EMPTY &&
      isInRangeLocation(location)) {/**assert(0);**/
    int isa = isSelfAtari(location);
    if (isa) return false;
    if (isAtari(location))
      return true;
    if (isEatingMove(location))
      return true;
    if (isOnGobanSide(location)) {/**assert(0);**/
      if (locationInformation.isYoseOnSide(location))
        return true;
      return false;
    }
    //         else if (!(int)atariSavingMoves.size()) {/**assert(0);**/
    if (isHane(location))
      return true;
    else if (isCut(location))
      return true;
    else if (isCrossCut(location))
      return true;
    //       else if (isSolid(location))
    //         interestingMoves.push_back(location);
  }
  return false;
}




// TEMPORARY COMMENT: THIS FUNCTION IS USED
// TEMPORARY COMMENT: BLACK PLAYS 2 TIMES
Location MoGo::Goban::randomOutOneNotUselessAvoidingSelfAtari( LocationList *& locationListIncludingThisLocation, int thresholdSelfAtari ) const {
  //     GoTools::print("randomOutOneNotUselessAvoidingSelfAtari\n");
  LocationList *opponentSelfKillLocation = 0;
  EatingLocationListType *playerEatingLocation = 0;
  if (playerColor==BLACK) {
    opponentSelfKillLocation=&gobanLists.whiteSelfKillLocation;
    playerEatingLocation=&gobanLists.blackEatingLocation;
  } else {
    opponentSelfKillLocation=&gobanLists.blackSelfKillLocation;
    playerEatingLocation=&gobanLists.whiteEatingLocation;
  }

  Location l=randomOutOneNotUselessMove(locationListIncludingThisLocation);
  removedLocationFromGobanAvoidingSelfAtariFreeLocation.clear();
  removedLocationFromGobanAvoidingSelfAtariEatingLocation.clear();
  removedLocationFromGobanAvoidingSelfAtariSelfKillLocation.clear();
  while (l!=PASSMOVE && avoidThisSelfAtariInSimulations(l, thresholdSelfAtari)/*(isSelfAtari(l)>=thresholdSelfAtari)*/) {
    //       GoTools::print("proposed move eating but self atari\n"); textModeShowPosition(l);
    if (gobanLists.freeLocation.isLocationIn(l)) {
      gobanLists.freeLocation.deleteLocation(l);
      removedLocationFromGobanAvoidingSelfAtariFreeLocation.push_back(l);
    } else {
      if (!opponentSelfKillLocation->isLocationIn(l)) {/**assert(0);**/ textModeShowGoban(); GoTools::print("l %i\n", l); textModeShowPosition(l);assert(0);}

      opponentSelfKillLocation->deleteLocation(l);
      removedLocationFromGobanAvoidingSelfAtariSelfKillLocation.push_back(l);
    }
    //     }
    if (playerEatingLocation->isLocationIn(l)) {
      playerEatingLocation->deleteLocation(l);
      removedLocationFromGobanAvoidingSelfAtariEatingLocation.push_back(l);
    }
    //     const_cast<MoGo::Goban *>(this)->gobanState[l]=OUTSIDE;
    l=randomOutOneNotUselessMove(locationListIncludingThisLocation);
  }
  for (int i=0;i<(int)removedLocationFromGobanAvoidingSelfAtariFreeLocation.size();i++)
    gobanLists.freeLocation.addLocation(removedLocationFromGobanAvoidingSelfAtariFreeLocation[i]);
  for (int i=0;i<(int)removedLocationFromGobanAvoidingSelfAtariEatingLocation.size();i++)
    playerEatingLocation->addLocation(removedLocationFromGobanAvoidingSelfAtariEatingLocation[i]);
  for (int i=0;i<(int)removedLocationFromGobanAvoidingSelfAtariSelfKillLocation.size();i++)
    opponentSelfKillLocation->addLocation(removedLocationFromGobanAvoidingSelfAtariSelfKillLocation[i]);

  return l;
}
Location MoGo::Goban::randomOutOneNotUselessAvoidingSelfAtariSuicide( LocationList *& locationListIncludingThisLocation, int thresholdSelfAtari ) const {/**assert(0);**/
  //     GoTools::print("randomOutOneNotUselessAvoidingSelfAtari\n");
  LocationList *opponentSelfKillLocation = 0;
  EatingLocationListType *playerEatingLocation = 0;
  if (playerColor==BLACK) {/**assert(0);**/
    opponentSelfKillLocation=&gobanLists.whiteSelfKillLocation;
    playerEatingLocation=&gobanLists.blackEatingLocation;
  } else {/**assert(0);**/
    opponentSelfKillLocation=&gobanLists.blackSelfKillLocation;
    playerEatingLocation=&gobanLists.whiteEatingLocation;
  }

  Location l=randomOutOneNotUselessMoveSuicide(locationListIncludingThisLocation);
  removedLocationFromGobanAvoidingSelfAtariFreeLocation.clear();
  removedLocationFromGobanAvoidingSelfAtariEatingLocation.clear();
  removedLocationFromGobanAvoidingSelfAtariSelfKillLocation.clear();
  while (l!=PASSMOVE && avoidThisSelfAtariInSimulations(l, thresholdSelfAtari)/*(isSelfAtari(l)>=thresholdSelfAtari)*/) {/**assert(0);**/
    //       GoTools::print("proposed move eating but self atari\n"); textModeShowPosition(l);
    if (gobanLists.freeLocation.isLocationIn(l)) {/**assert(0);**/
      gobanLists.freeLocation.deleteLocation(l);
      removedLocationFromGobanAvoidingSelfAtariFreeLocation.push_back(l);
    } else {/**assert(0);**/
      if (!opponentSelfKillLocation->isLocationIn(l)) {/**assert(0);**/ textModeShowGoban(); GoTools::print("l %i\n", l); textModeShowPosition(l);assert(0);}

      opponentSelfKillLocation->deleteLocation(l);
      removedLocationFromGobanAvoidingSelfAtariSelfKillLocation.push_back(l);
    }
    //     }
    if (playerEatingLocation->isLocationIn(l)) {/**assert(0);**/
      playerEatingLocation->deleteLocation(l);
      removedLocationFromGobanAvoidingSelfAtariEatingLocation.push_back(l);
    }
    //     const_cast<MoGo::Goban *>(this)->gobanState[l]=OUTSIDE;
    l=randomOutOneNotUselessMoveSuicide(locationListIncludingThisLocation);
  }
  for (int i=0;i<(int)removedLocationFromGobanAvoidingSelfAtariFreeLocation.size();i++)
    gobanLists.freeLocation.addLocation(removedLocationFromGobanAvoidingSelfAtariFreeLocation[i]);
  for (int i=0;i<(int)removedLocationFromGobanAvoidingSelfAtariEatingLocation.size();i++)
    playerEatingLocation->addLocation(removedLocationFromGobanAvoidingSelfAtariEatingLocation[i]);
  for (int i=0;i<(int)removedLocationFromGobanAvoidingSelfAtariSelfKillLocation.size();i++)
    opponentSelfKillLocation->addLocation(removedLocationFromGobanAvoidingSelfAtariSelfKillLocation[i]);

  return l;
}

int MoGo::Goban::countNbStones( ) const {/**assert(0);**/
  int count=0;
  for (int i = 0 ; i < height() ; i++)
    for (int j = 0 ; j < width() ; j++) {/**assert(0);**/
      PositionState s=position(i,j);
      if (s == EMPTY)
        continue;
      else if (s == BLACK) {/**assert(0);**/
        count ++;
      } else if (s == WHITE) {/**assert(0);**/
        count ++;
      }

    }
  return count;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::Goban::precomputePatterns( ) {
  randomModePrecomputed=2015441;
  isInterestingPrecomputed.clear();
  isInterestingPrecomputed.resize(65536*2,-1);
  isSelfAtariPrecomputed.clear();
  isSelfAtariPrecomputed.resize(65536*2,-1);
 
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
// TEMPORARY COMMENT: BLACK PLAYS 2 TIMES
int MoGo::Goban::getSideNumber( const Location location ) {
  if (GobanLocation::height==heightOfGoban)
    return GobanLocation::getSideNumberFast(location);
  int x = xIndex(location);
  int y = yIndex(location);

  if (x == 0 && y!=0 && y!=widthOfGoban-1) return 0;
  if (y == widthOfGoban-1 && x!=0 && x!=heightOfGoban-1) return 1;
  if (x == heightOfGoban-1 && y!=0 && y!=widthOfGoban-1) return 2;
  if (y == 0 && x!=0 && x!=heightOfGoban-1) return 3;
  return 4;//TODO 4 is might be a corner, might be the center, not defined yet.
}

FastSmallVector< Location > MoGo::Goban::getSavingMovesForLastMoveAtari( ) const {/**assert(0);**/
  FastSmallVector< Location > res;

  if (lastPlayedMove!=PASSMOVE) {/**assert(0);**/
    underAtariStringsFirstPosition.clear();
    if (locationInformation.isLastMoveAtari(underAtariStringsFirstPosition)) {/**assert(0);**/
      for (int i=0;i<(int)underAtariStringsFirstPosition.size();i++) {/**assert(0);**/
        Location savingMove = PASSMOVE;
        if (isUnderAtariStringSavableByEatingMove(underAtariStringsFirstPosition[i],savingMove)) {/**assert(0);**/
          res.push_back(savingMove);
          continue;
        }
        savingMove = stringInformation.getOneLibertyOfString(underAtariStringsFirstPosition[i]);
        if(
          isLegal(savingMove) &&
          isAtariSavingMoveTrue(savingMove)) {/**assert(0);**/
          res.push_back(savingMove);
          continue;
        }
      }
    }
  }

  return res;
}


void MoGo::Goban::addNonSeenLiberties( Location location, Location dontAddThisOne, FastSmallVector< int > & liberties ) const {/**assert(0);**/
  int strNumber=stringInformation.stringNumber[location];

  for (int i=0;i<(int)stringInformation.stringPositions[strNumber].size();i++) {/**assert(0);**/
    for (int j=0;j<4;j++) {/**assert(0);**/
      Location tmpLocation=stringInformation.stringPositions[strNumber][i]+direction(j);
      if (gobanState[tmpLocation]==EMPTY && tmpLocation != dontAddThisOne) {/**assert(0);**/
        bool alreadySeen=false;
        for (int k=0;k<(int)liberties.size() && (!alreadySeen);k++) {/**assert(0);**/
          if (liberties[k]==tmpLocation)
            alreadySeen=true;
        }
        if (!alreadySeen)
          liberties.push_back(tmpLocation);
      }
    }
  }
}




bool MoGo::Goban::isPonnuki( Location location ) const {/**assert(0);**/
  if (gobanState[location]!=EMPTY)
    return false;
  for (int i=0;i<4;i++) {/**assert(0);**/
    Location tmpLocation=location+direction(i);
    if (gobanState[tmpLocation]==3-playerColor && getStringLiberty(tmpLocation)==1 && getStringLength(tmpLocation)==1) {/**assert(0);**/
      bool ponnukiPossible=true;
      for (int j=0;j<4 && ponnukiPossible;j++) {/**assert(0);**/
        Location tmpLocation2=tmpLocation+direction(j);
        if (gobanState[tmpLocation2]==3-playerColor || gobanState[tmpLocation2]==OUTSIDE)
          ponnukiPossible=false;
      }
      if (ponnukiPossible) return true;

    }
  }
  return false;
}

bool MoGo::Goban::isSavingStoneMove( Location location ) const {/**assert(0);**/
  underAtariStringsFirstPosition.clear();
  locationInformation.isAtariNear(location, underAtariStringsFirstPosition);


  for (int i=0;i<(int)underAtariStringsFirstPosition.size();i++) {/**assert(0);**/
    Location savingMove = PASSMOVE;
    if (isUnderAtariStringSavableByEatingMove(underAtariStringsFirstPosition[i],savingMove)) {/**assert(0);**/
      //         textModeShowPosition(savingMove);
      assert(isLegal(savingMove));
      if (isInRangeLocation(savingMove) && savingMove==location)
        return true;
    }
    savingMove = stringInformation.getOneLibertyOfString(underAtariStringsFirstPosition[i]);
    assert(savingMove==location);
    //       textModeShowPosition(savingMove);
    //       GoTools::print("savingMove isLegal %d isAtariSavingMoveTrue %d\n",isLegal(savingMove),isAtariSavingMoveTrue(savingMove));

    if( isInRangeLocation(savingMove) &&
        isLegal(savingMove) &&
        isAtariSavingMoveTrue(savingMove))
      return true;
  }
  return false;
}






Location MoGo::Goban::chooseThreatingMoveRandomly( ) const {/**assert(0);**/
  FastSmallVector<Location> allThreatingMoves;
  for (int i=0;i<(int)stringInformation.stringPositions.size();i++) {/**assert(0);**/
    if (stringInformation.stringPositions[i].size()>0) {/**assert(0);**/
      const FastSmallVector<int> &positions=stringInformation.stringPositions[i];
      Location location=positions[0];
      if (gobanState[location]==3-playerColor && getStringLiberty(location)==2) {/**assert(0);**/
        for (int p=0;p<positions.size();p++)
          for (int j=0;j<4;j++) {/**assert(0);**/
            Location tmpLocation=positions[p]+direction(j);
            if (gobanState[tmpLocation]==EMPTY && isNotUseless(tmpLocation) && (!isSelfAtari(tmpLocation)))
              allThreatingMoves.push_back(tmpLocation);
          }
      }
    }
  }
  if (allThreatingMoves.size()==0)
    return PASSMOVE;
  return allThreatingMoves[GoTools::mogoRand(allThreatingMoves.size())];
}






int MoGo::Goban::lengthOfEatString( Location move ) const {/**assert(0);**/
  int length=0;
  for (int i=0;i<4;i++) {/**assert(0);**/
    Location tmpLocation=move+direction(i);
    if (gobanState[tmpLocation]==3-playerColor && getStringLiberty(tmpLocation)==1) {/**assert(0);**/
      length+=getStringLength(tmpLocation);
    }
  }
  return length;
}







bool MoGo::Goban::isStringOnSide( Location location ) const {/**assert(0);**/
  const FastSmallVector<int> &positions=stringInformation.stringPositions[getStringNumber(location)];
  for (int p=0;p<positions.size();p++)
    for (int j=0;j<4;j++) {/**assert(0);**/
      if (gobanState[positions[p]+direction(j)]==OUTSIDE)
        return true;
    }
  return false;
}

bool MoGo::Goban::isOneNearFriendStringOnSide( Location location, int color ) const {/**assert(0);**/
  for (int i=0;i<4;i++) {/**assert(0);**/
    if (gobanState[location+direction(i)]==color)
      if (isStringOnSide(location+direction(i)))
        return true;
  }
  return false;
}

Location MoGo::Goban::getLargestNearOpponentString( Location location, int color ) const {/**assert(0);**/
  color=3-color;
  int max=0;Location l=PASSMOVE;
  if (gobanState[location]!=EMPTY) {/**assert(0);**/
    const FastSmallVector<int> &positions=stringInformation.stringPositions[getStringNumber(location)];
    for (int p=0;p<positions.size();p++)
      for (int i=0;i<4;i++) {/**assert(0);**/
        Location tmpLocation=positions[p]+direction(i);
        if (gobanState[tmpLocation]==color) {/**assert(0);**/
          int length=getStringLength(tmpLocation);
          if (length>max) {/**assert(0);**/
            max=length;
            l=tmpLocation;
          }
        }
      }
  } else {/**assert(0);**/
    for (int i=0;i<4;i++) {/**assert(0);**/
      Location tmpLocation=location+direction(i);
      if (gobanState[tmpLocation]==color) {/**assert(0);**/
        int length=getStringLength(tmpLocation);
        if (length>max) {/**assert(0);**/
          max=length;
          l=tmpLocation;
        }
      }
    }
  }
  return l;
}

bool MoGo::Goban::isThisMoveWouldBeAGoodSacrifice( Location location, Location opponentStringLocation ) const {/**assert(0);**/
  if (getStringLiberty(opponentStringLocation)>2) return false;
  FastSmallVector<int> friendAdjacentStringLocation;
  for (int i=0;i<4;i++) {/**assert(0);**/
    Location tmpLocation=location+direction(i);
    if (gobanState[tmpLocation]==playerColor)
      friendAdjacentStringLocation.push_back(getStringNumber(tmpLocation));
  }

  const FastSmallVector<int> &positions=stringInformation.stringPositions[getStringNumber(opponentStringLocation)];
  bool ok=true;
  for (int p=0;p<positions.size();p++)
    for (int j=0;j<4;j++) {/**assert(0);**/
      Location tmpLocation=positions[p]+direction(j);
      if (gobanState[tmpLocation]==EMPTY && (tmpLocation!=location)) {/**assert(0);**/
        bool foundLocation=false;
        for (int k=0;k<4;k++) {/**assert(0);**/
          Location tmpTmpLocation=tmpLocation+direction(k);
          if (tmpTmpLocation==location)
            foundLocation=true;
          else
            for (int l=0;l<(int)friendAdjacentStringLocation.size();l++)
              if (gobanState[tmpTmpLocation]==playerColor && getStringNumber(tmpTmpLocation)==friendAdjacentStringLocation[l])
                foundLocation=true;
        }
        if (!foundLocation) {/**assert(0);**/
          // GoTools::print("don't found :'(\n");
          // textModeShowPosition(tmpLocation);
          ok=false;
        }
      }
    }
  return ok;
}

bool MoGo::Goban::isNear8( Location l1, Location l2 ) const {/**assert(0);**/
  int d1=abs(xIndex(l1)-xIndex(l2));
  int d2=abs(yIndex(l1)-yIndex(l2));
  return (d1<=1 && d2<=1);
}

Location MoGo::Goban::randomOutOneMoveNearLastMove( ) const {/**assert(0);**/
  if (lastPlayedMove==PASSMOVE) return PASSMOVE;
  interestingMoves.clear();
  for (int i=0;i<8;i++)
    if (gobanState[lastPlayedMove+direction(i)]==EMPTY)
      interestingMoves.push_back(lastPlayedMove+direction(i));
  Location location=PASSMOVE;
  if (randomOutOneMoveFromInterestingMovesWithSelfAtariChecking(location,interestingMoves)) return location;
  else return PASSMOVE;
}

FastSmallVector< Location > MoGo::Goban::getAllMovesNearLastMove( ) const {/**assert(0);**/
  FastSmallVector<Location>res;
  if (lastPlayedMove==PASSMOVE) return res;
  for (int i=0;i<8;i++) {/**assert(0);**/
    Location location=lastPlayedMove+direction(i);
    if (gobanState[location]==EMPTY)
      if (isNotUseless(location) && (!isSelfAtari(location)))
        res.push_back(location);
  }
  return res;
}


void MoGo::Goban::addCloseLibertiesWhenPlaying(Location location) {/**assert(0);**/
  // GoTools::print("addCloseLibertiesWhenPlaying \n"); textModeShowPosition(location);
  for (int i=0;i<4;i++) {/**assert(0);**/
    Location tmpLocation=location+direction(i);
    if (gobanState[tmpLocation]==EMPTY) {/**assert(0);**/
      bool alreadyNear=false;


      for (int j=0;j<4;j++) {/**assert(0);**/
        Location tmpTmpLocation=tmpLocation+direction(j);
        if (gobanState[tmpTmpLocation]==BLACK || gobanState[tmpTmpLocation]==WHITE) {/**assert(0);**/
          alreadyNear=true;
          break;
        }
      }
      if (!alreadyNear)
        closeLiberties.push_back(tmpLocation);
    }
  }
}

void MoGo::Goban::removeCloseLibertiesByPlaying( Location location ) const {/**assert(0);**/
  int index=-1;
  for (int i=0;i<(int)closeLiberties.size();i++) {/**assert(0);**/
    if (closeLiberties[i]==location) {/**assert(0);**/
      index=i;
      break;
    }
  }
  if (index>=0) removeCloseLibertiesByPlayingIndex(index);
}

void MoGo::Goban::removeCloseLibertiesByPlayingIndex( int index ) const {/**assert(0);**/
  closeLiberties[index]=closeLiberties[closeLiberties.size()-1];
  closeLiberties.pop_back();
}


bool MoGo::Goban::isOneStoneNear4(Location move) const {/**assert(0);**/
  bool stoneNear=false;
  for (int i=0;i<4;i++)
    if (gobanState[move+direction(i)]==BLACK ||gobanState[move+direction(i)]==WHITE) {/**assert(0);**/
      stoneNear=true;
      break;
    }
  return stoneNear;
}

const FastSmallVector< Location > & MoGo::Goban::getErasedStones( ) const {/**assert(0);**/
  return stringInformation.erasedStones;
}



int MoGo::Goban::patternValueMatchedBy2015441( Location location ) const {/**assert(0);**/
  if (gobanState[location] == EMPTY) {/**assert(0);**/
    int numPattern=0;

    for (int j=0;j<8;j++) {/**assert(0);**/ numPattern+=gobanState[location+direction(j)];numPattern=numPattern<<2; }
    numPattern=numPattern>>1; if (playerColor==2) numPattern++;

    bool isAtariPossible=false;
    int answer=0;

    for (int j=0;j<4;j++)
      if ( gobanState[location+direction(j)] == 3-playerColor )
        isAtariPossible=true;

    if (isOnGobanSide(location)) {/**assert(0);**///FIXME hey, i changed only two lines here.
      if (locationInformation.isYoseOnSide(location)) {/**assert(0);**/
        answer++;
        // GoTools::print("isYose\n");
      }
    }
    if (isHane(location))
      answer++;
    else if (isCut(location))
      answer++;
    //  GoTools::print("yose %i, hane %i, cut %i (answer %i)\n", locationInformation.isYoseOnSide(location), isHane(location), isCut(location), answer);
    if (answer>0 || isAtariPossible) {/**assert(0);**/
      if (answer>0 && isAtariPossible)
        return 3;
      else if (isAtariPossible)
        return 2;
      else
        return 1;
    } else
      return 0;
  }
  return -1;
}

int MoGo::Goban::isAtariSize( const Location & location ) const {/**assert(0);**/
  for (int i=0;i<4;i++)
    if ( gobanState[location+direction(i)] == 3-playerColor )
      if (getStringLiberty(location+direction(i)) == 2) return getStringLength(location+direction(i));

  return 0;
}

bool MoGo::Goban::isEatingSize( const Location & location ) const {/**assert(0);**/
  int size=0;
  for (int i=0;i<4;i++)
    if (gobanState[location+direction(i)] == 3-playerColor &&
        getStringLiberty(location+direction(i)) == 1)
      size+=getStringLength(location+direction(i));
  return size;
}

int MoGo::Goban::nbAttach( Location location ) const {/**assert(0);**/
  // ATTACH (+10)
  // oX

  int nb=0;
  for (int i=0;i<8;i++) {/**assert(0);**/
    const int * dTR=locationInformation.directionT[i];
    // GoTools::print("dTR[WEST_] %i \n", gobanState[location+dTR[WEST_]]==3-playerColor);
    if (gobanState[location+dTR[WEST_]]==3-playerColor)
      nb=1;
  }
  return nb;
}

int MoGo::Goban::nbExtend( Location location ) const {/**assert(0);**/
  // EXTEND (+20)
  // xX

  int nb=0;
  for (int i=0;i<8;i++) {/**assert(0);**/
    const int * dTR=locationInformation.directionT[i];

    if (gobanState[location+dTR[WEST_]]==playerColor)
      nb=1;
  }
  return nb;
}
int MoGo::Goban::nbDavidHane( Location location ) const {/**assert(0);**/
  // HANE (+100)
  // xo
  // .X

  int nb=0;
  for (int i=0;i<8;i++) {/**assert(0);**/
    const int * dTR=locationInformation.directionT[i];
    if (gobanState[location+dTR[WEST_]]==EMPTY)
      if (gobanState[location+dTR[NORTH_]]==3-playerColor)
        if (gobanState[location+dTR[NORTHWEST_]]==playerColor)
          nb=1;
  }
  return nb;
}
int MoGo::Goban::nbBlock( Location location ) const {/**assert(0);**/
  // BLOCK (+100)
  // xX
  // .o

  int nb=0;
  for (int i=0;i<8;i++) {/**assert(0);**/
    const int * dTR=locationInformation.directionT[i];
    if (gobanState[location+dTR[WEST_]]==playerColor)
      if (gobanState[location+dTR[SOUTH_]]==3-playerColor)
        if (gobanState[location+dTR[SOUTHWEST_]]==EMPTY)
          nb=1;
  }
  return nb;
}




int MoGo::Goban::nbConnect( Location location ) const {/**assert(0);**/
  // CONNECT (+20)
  // xXx

  int nb=0;
  for (int i=0;i<8;i++) {/**assert(0);**/
    const int * dTR=locationInformation.directionT[i];
    if (gobanState[location+dTR[WEST_]]==playerColor)
      if (gobanState[location+dTR[EAST_]]==playerColor)
        nb=1;
  }
  return nb;
}
int MoGo::Goban::nbTurn( Location location ) const {/**assert(0);**/
  // TURN (+150)
  // xx
  // oX

  int nb=0;
  for (int i=0;i<8;i++) {/**assert(0);**/
    const int * dTR=locationInformation.directionT[i];
    if (gobanState[location+dTR[WEST_]]==3-playerColor)
      if (gobanState[location+dTR[NORTH_]]==playerColor)
        if (gobanState[location+dTR[NORTHWEST_]]==playerColor)
          nb=1;
  }
  return nb;
}

int MoGo::Goban::nbTurnConnect( Location location ) const {/**assert(0);**/
  // TURN-CONNECT (+100)
  // xX
  // ox

  int nb=0;
  for (int i=0;i<8;i++) {/**assert(0);**/
    const int * dTR=locationInformation.directionT[i];
    if (gobanState[location+dTR[WEST_]]==playerColor)
      if (gobanState[location+dTR[SOUTH_]]==playerColor)
        if (gobanState[location+dTR[SOUTHWEST_]]==3-playerColor)
          nb=1;
  }
  return nb;
}

int MoGo::Goban::nbDavidCrossCut( Location location ) const {/**assert(0);**/
  // CROSS-CUT (+50)
  // oX
  // xo

  int nb=0;
  for (int i=0;i<8;i++) {/**assert(0);**/
    const int * dTR=locationInformation.directionT[i];
    if (gobanState[location+dTR[WEST_]]==3-playerColor)
      if (gobanState[location+dTR[SOUTH_]]==3-playerColor)
        if (gobanState[location+dTR[SOUTHWEST_]]==playerColor)
          nb=1;
  }
  return nb;
}
int MoGo::Goban::blockCut( Location location ) const {/**assert(0);**/
  // BLOCK-CUT (+20)
  // oXo

  int nb=0;
  for (int i=0;i<8;i++) {/**assert(0);**/
    const int * dTR=locationInformation.directionT[i];
    if (gobanState[location+dTR[WEST_]]==3-playerColor)
      if (gobanState[location+dTR[EAST_]]==3-playerColor)
        nb=1;
  }
  return nb;
}
int MoGo::Goban::nbDiagonal( Location location ) const {/**assert(0);**/
  // DIAGONAL MOVE (+30)
  // x.
  // .X

  int nb=0;
  for (int i=0;i<8;i++) {/**assert(0);**/
    const int * dTR=locationInformation.directionT[i];
    if (gobanState[location+dTR[NORTHWEST_]]==playerColor)
      nb=1;
  }
  return nb;
}
int MoGo::Goban::nbShoulderHit( Location location ) const {/**assert(0);**/
  //FIXME
  int nb=0;
  for (int i=0;i<8;i++) {/**assert(0);**/
    const int * dTR=locationInformation.directionT[i];
    if (gobanState[location+dTR[NORTHWEST_]]==3-playerColor)
      if (gobanState[location+dTR[SOUTHEAST_]]==3-playerColor)
        nb=1;
  }
  return nb;

}
int MoGo::Goban::nbOnePointJump( Location  ) const {/**assert(0);**/
  // ONE-POINT JUMP (+50)
  // x.X
  return false;
}

int MoGo::Goban::nbKnightMove( Location  ) const {/**assert(0);**/
  // KNIGHT'S-MOVE (+10)
  // x..
  // ..X
  return false;
}
int MoGo::Goban::nbEmptyTriangle( Location location ) const {/**assert(0);**/
  // EMPTY TRIANGLE (-100)
  // xx
  // .X

  int nb=0;
  for (int i=0;i<8;i++) {/**assert(0);**/
    const int * dTR=locationInformation.directionT[i];
    if (gobanState[location+dTR[NORTH_]]==playerColor)
      if (gobanState[location+dTR[NORTHWEST_]]==playerColor)
        if (gobanState[location+dTR[WEST_]]==EMPTY)
          nb=1;
  }
  return nb;
}
int MoGo::Goban::nbDumpling( Location location ) const {/**assert(0);**/
  // DUMPLING (-200)
  // xx
  // xX

  int nb=0;
  for (int i=0;i<8;i++) {/**assert(0);**/
    const int * dTR=locationInformation.directionT[i];
    if (gobanState[location+dTR[WEST_]]==playerColor)
      if (gobanState[location+dTR[NORTH_]]==playerColor)
        if (gobanState[location+dTR[NORTHWEST_]]==playerColor)
          nb=1;
  }
  return nb;
}

int MoGo::Goban::nbPush( Location location ) const {/**assert(0);**/
  // PUSH (70)
  // ox
  // oX

  int nb=0;
  for (int i=0;i<8;i++) {/**assert(0);**/
    const int * dTR=locationInformation.directionT[i];
    if (gobanState[location+dTR[WEST_]]==3-playerColor)
      if (gobanState[location+dTR[NORTH_]]==playerColor)
        if (gobanState[location+dTR[NORTHWEST_]]==3-playerColor)
          nb=1;
  }
  return nb;
}

bool MoGo::Goban::allowAttackInLadder( Location location ) const {/**assert(0);**/
  if (!allowAttackInLadderWhenAdjacent) return false;
  if (location==PASSMOVE) return false;
  for (int i=0;i<4;i++) {/**assert(0);**/
    Location tmpLocation=location+direction(i);
    if ((gobanState[tmpLocation]==BLACK && isBlacksTurn()) || (gobanState[tmpLocation]==WHITE && (!isBlacksTurn())))
      return true;
  }
  return false;
}

bool MoGo::Goban::isMakeKo( const Location location ) const {/**assert(0);**/
  int size=0;
  for (int i=0;i<4;i++) {/**assert(0);**/
    Location tmpLocation=location+direction(i);
    if (gobanState[tmpLocation] == 3-playerColor) {/**assert(0);**/
      if (getStringLiberty(tmpLocation) == 1)
        size+=getStringLength(tmpLocation);
    } else if (gobanState[tmpLocation] == playerColor || gobanState[tmpLocation] == EMPTY)
      return false;
  }
  return size==1;
}

Vector< Location > MoGo::Goban::getOneLocationPerString( ) const {/**assert(0);**/
  Vector<Location> res;
  for (int strNumber=0;strNumber < stringInformation.stringPositions.size();strNumber++)
    if (stringInformation.stringPositions[strNumber].size()>0 && stringInformation.stringNumber[stringInformation.stringPositions[strNumber][0]]==strNumber) {/**assert(0);**/
      Location tmpLocation=stringInformation.stringPositions[strNumber][0];
      if (gobanState[tmpLocation]==BLACK)
        res.push_back(tmpLocation);
      else if (gobanState[tmpLocation]==WHITE)
        res.push_back(-tmpLocation);
      else assert(0);
    }
  return res;
}

double MoGo::Goban::distanceToNearestString( Location location ) const {/**assert(0);**/
  double min=100000.;
  for (int strNumber=0;strNumber < stringInformation.stringPositions.size();strNumber++)
    if (stringInformation.stringPositions[strNumber].size()>0) {/**assert(0);**/
      Location tmpLocation=stringInformation.stringPositions[strNumber][0];

      //   double d=abs(xIndex(location)-xIndex(tmpLocation))+abs(yIndex(location)-yIndex(tmpLocation));
      double d=sqrt((xIndex(location)-xIndex(tmpLocation))*(xIndex(location)-xIndex(tmpLocation))
                    +(yIndex(location)-yIndex(tmpLocation))*(yIndex(location)-yIndex(tmpLocation)))-2;
      min=GoTools::min(min, d);
    }
  // textModeShowPosition(location);
  // GoTools::print("distanceToNearestString %f\n", min);

  double d=sqrt((yIndex(location)-0)*(yIndex(location)-0));
  min=GoTools::min(min, d);
  d=sqrt((yIndex(location)-heightOfGoban)*(yIndex(location)-heightOfGoban));
  min=GoTools::min(min, d);
  d=sqrt((xIndex(location)-0)*(xIndex(location)-0));
  min=GoTools::min(min, d);
  d=sqrt((xIndex(location)-widthOfGoban)*(xIndex(location)-widthOfGoban));
  min=GoTools::min(min, d);


  return min;
}












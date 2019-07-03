//
// C++ Implementation: goplayer_mg
//
// Description:
//
//
// Author: Yizao Wang <yizao.wang@polytechnique.edu> and Sylvain Gelly <gelly@lri.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "goplayer_mg.h"
#include "gotools_mg.h"

namespace MoGo {
  std::string GoPlayer::databaseName=std::string("openingValues_merged");

  // TEMPORARY COMMENT: THIS FUNCTION IS USED
  GoPlayer::GoPlayer() {
    openingHoshiStone.clear();
    if (GobanLocation::height == 19 && GobanLocation::width == 19) {/**assert(0);**/
      openingHoshiStone.push_back(GobanLocation::locationXY(3,3));
      openingHoshiStone.push_back(GobanLocation::locationXY(3,15));
      openingHoshiStone.push_back(GobanLocation::locationXY(15,3));
      openingHoshiStone.push_back(GobanLocation::locationXY(15,15)); 
      openingHoshiStone.push_back(GobanLocation::locationXY(3,9));
      openingHoshiStone.push_back(GobanLocation::locationXY(9,3));
      openingHoshiStone.push_back(GobanLocation::locationXY(9,15));
      openingHoshiStone.push_back(GobanLocation::locationXY(15,9));
      openingHoshiStone.push_back(GobanLocation::locationXY(15,9)); //I remove the center and put twice the same
//       openingHoshiStone.push_back(GobanLocation::locationXY(9,9)); //I remove the center and put twice the same
    } else if (GobanLocation::height == 13 && GobanLocation::width == 13) {/**assert(0);**/
      openingHoshiStone.push_back(GobanLocation::locationXY(3,3));
      openingHoshiStone.push_back(GobanLocation::locationXY(3,9));
      openingHoshiStone.push_back(GobanLocation::locationXY(9,3));
      openingHoshiStone.push_back(GobanLocation::locationXY(9,9));
      openingHoshiStone.push_back(GobanLocation::locationXY(3,6));
      openingHoshiStone.push_back(GobanLocation::locationXY(6,3));
      openingHoshiStone.push_back(GobanLocation::locationXY(9,6));
      openingHoshiStone.push_back(GobanLocation::locationXY(6,9));
      openingHoshiStone.push_back(GobanLocation::locationXY(6,6));
    } else if (GobanLocation::height == 9 && GobanLocation::width == 9) {
      openingHoshiStone.push_back(GobanLocation::locationXY(2,2));
      openingHoshiStone.push_back(GobanLocation::locationXY(2,6));
      openingHoshiStone.push_back(GobanLocation::locationXY(6,2));
      openingHoshiStone.push_back(GobanLocation::locationXY(6,6));
      openingHoshiStone.push_back(GobanLocation::locationXY(4,2));
      openingHoshiStone.push_back(GobanLocation::locationXY(2,4));
      openingHoshiStone.push_back(GobanLocation::locationXY(6,4));
      openingHoshiStone.push_back(GobanLocation::locationXY(4,6));
      openingHoshiStone.push_back(GobanLocation::locationXY(4,4));
    }


  }


  GoPlayer::~GoPlayer() {/**assert(0);**/}}

Location MoGo::GoPlayer::generateOneMove( const Location , const Goban & goban, double time ) {/**assert(0);**/
  return generateOneMove(goban,time);
}

void MoGo::GoPlayer::reset( ) {/**assert(0);**/}

// TEMPORARY COMMENT: GOGUI
// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GoPlayer::start( const Goban &, const Location ) {
  clearHistoryGoban();
}

void MoGo::GoPlayer::finish( double ) {/**assert(0);**/}

Location MoGo::GoPlayer::generateAndPlay( const Location otherPlayerLocation, Goban & goban, double time ) {/**assert(0);**/
  Location location=generateOneMove(otherPlayerLocation, goban, time);
  goban.playMoveOnGoban(location);
  return location;
}

double MoGo::GoPlayer::getScore( ) {/**assert(0);**/
  return 0.;
}

void MoGo::GoPlayer::setGobanSize(int ) {/**assert(0);**/}

void MoGo::GoPlayer::showName( ) const {/**assert(0);**/
  MoGo::GoTools::print((getName()+"\n").c_str());
}

void MoGo::GoPlayer::setKomi( const double ) {/**assert(0);**/}

bool MoGo::GoPlayer::hasResigned( ) const {/**assert(0);**/
  return false;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::GoPlayer::openingMove( const Goban & goban, Location & location, double *prediction ) {
  bool r=openingDatabase.chooseMove(goban, location, prediction);
  // GoTools::print("try openingMove (%i)\n",r);
  return r;
  // here is the old method
  //   if (openingDatabase.count(goban.getGobanKey())==0)
  //     return false;
  //   location=openingDatabase[goban.getGobanKey()];
  //   return true;
}

bool MoGo::GoPlayer::loadOpeningDatabase( const std::string & fileName, MoGoHashMap( GobanKey, int, hashGobanKey, equalGobanKey ) & openingDatabase ) {/**assert(0);**/
  FILE *f=fopen(fileName.c_str(),"r");
  if (!f)
    return false;
  fclose(f);
  openingDatabase.clear();
  return addToOpeningDatabase(fileName, openingDatabase);
}
bool MoGo::GoPlayer::addToOpeningDatabase( const std::string & fileName, MoGoHashMap( GobanKey, int, hashGobanKey, equalGobanKey ) & openingDatabase ) {/**assert(0);**/
  FILE *f=fopen(fileName.c_str(),"r");
  if (!f)
    return false;

  int gobanSize=0;
  fscanf(f, "%i", &gobanSize);
  if (gobanSize <= 0) {/**assert(0);**/
    fclose(f);
    return false;
  }

  Goban goban(gobanSize, gobanSize);
  GobanKey key=goban.getGobanKey();
  bool eof=false; int nbCollisions=0; int nbEntries=0;
  for ( ; !eof ; ) {/**assert(0);**/
    int nb=0;int v;
    for (int i = 0 ; i < (int)key.size(); i++) {/**assert(0);**/
      nb=0;
      nb=fscanf(f, "%i", &v);
      if ((i == 0) && (nb != 1)) {/**assert(0);**/ eof = true; break; }
      assert(nb == 1);
      key[i]=v;
    }
    if (!eof) {/**assert(0);**/
      nb=0;
      nb=fscanf(f, "%i", &v);
      assert(nb == 1);
      Location move=v;
      Goban tmp(key);
      if (tmp.approximateSizeOfPossibleMoves() < tmp.height()*tmp.width()-15)
        continue;
    
      nbEntries++;
      if (openingDatabase.count(tmp.getGobanKey())==0)
        openingDatabase[tmp.getGobanKey()] = move;
      else
        nbCollisions++;
      //       tmp.textModeShowGoban();
      //       tmp.textModeShowPosition(move);
    }
  }


  fclose(f);
  GoTools::print("Load opening database %s succeed (nbCollisions=%i/%i)\n", fileName.c_str(), nbCollisions, nbEntries);
  return true;
}

bool MoGo::GoPlayer::addToOpeningDatabase( const std::string & fileName ) {/**assert(0);**/
  //   return addToOpeningDatabase(fileName, openingDatabase);
  return openingDatabase.addFromFile(fileName);
}

 // TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::GoPlayer::loadOpeningDatabase( const std::string & fileName ) {
  return openingDatabase.loadFromFile(fileName);
  //   return loadOpeningDatabase(fileName, openingDatabase);
}

 // TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::GoPlayer::loadOpeningDatabase(/*bool newDatabase*/) {
  //   if (!newDatabase) {/**assert(0);**/
  //     if (!loadOpeningDatabase("opening"))
  //       return loadOpeningDatabase("opening2");
  //   } else {/**assert(0);**/
  //  Here is the "old" database
  /*  if (!loadOpeningDatabase("opening"))
      return false;
    //   if (!addToOpeningDatabase("opening3"))
    //     return false;
    if (!addToOpeningDatabase("opening2"))
      return false;
    //   }
    return false;
  */
  bool success=loadOpeningDatabase(databaseName);
  GoTools::print("tried to open %s, success %i\n", databaseName.c_str(), success);
  return success;
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GoPlayer::addHistoryGoban( const Goban & goban ) {
  GobanKey gobanKey = goban.getGobanKey();
  gobanKey[0]-=gobanKey[0]%(1024*1024)/(1024*512)*(1024*512);
  gobanKey[0]-=gobanKey[0]%(1024*512)/1024*1024;
  historyGoban[gobanKey] = 1;
  if (lastMoves.size()) {
    //     assert(goban.lastMove()==PASSMOVE || lastMoves.back() != goban.lastMove());
    if (!(goban.lastMove()==PASSMOVE || lastMoves.back() != goban.lastMove()))
      GoTools::print("Warning GoPlayer::addHistoryGoban: lastMove %i, lastMoves.back %i\n", goban.lastMove(), lastMoves.back());
  }
  lastMoves.push_back(goban.lastMove());
}

// TEMPORARY COMMENT: GOGUI
// TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::GoPlayer::clearHistoryGoban( ) {
  historyGoban.clear();
  lastMoves.clear();
  lastMoves.push_back(PASSMOVE);
}

void MoGo::GoPlayer::restart( ) {/**assert(0);**/
  clearHistoryGoban();
}

void MoGo::GoPlayer::showPreferedMoves( const Goban & , const int  ) {/**assert(0);**/}


void MoGo::GoPlayer::undoForHistory(const Goban & goban) {/**assert(0);**/
  GobanKey gobanKey = goban.getGobanKeyForSuperKo();
  if (historyGoban.count(gobanKey) > 0) {/**assert(0);**/
    //GoTools::print("delete one gobanKey\n");
    historyGoban.erase(gobanKey);
  }
  lastMoves.pop_back();
}

void MoGo::GoPlayer::showPreferedSequence( const Goban & , const Location , const int) {/**assert(0);**/}

void MoGo::GoPlayer::setRandomModeByGTPEngine( const int , const Goban &  ) {/**assert(0);**/}

int MoGo::GoPlayer::getRandomMode( ) const {/**assert(0);**/
  assert(0);
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
// Fonction jouant les 4 coins en premier suivant les modes
// Quelque soit le mode, le premier coup de la partie est joué dans un coin (sauf mode -1) 
bool MoGo::GoPlayer::firstMove( int mode, const Goban & goban, Location & location ) const {

  if (mode==-1) {/**assert(0);**/
     return false;
  }


  if (goban.moveNumber() == 0)
    if ((goban.height() == 13 && goban.width() == 13) ||
        (goban.height() == 19 && goban.width() == 19)) {/**assert(0);**/
      location = goban.indexOnArray(3,goban.width()-4);
      return true;
    }

  if ( mode == 1) {
    if (goban.moveNumber() == 0)
      if ((goban.height() == 13 && goban.width() == 13) ||
          (goban.height() == 19 && goban.width() == 19)) {/**assert(0);**/
        location = goban.indexOnArray(3,goban.width()-4);
        return true;
      } else if (goban.height() == 9 && goban.width() == 9) {
        location = goban.indexOnArray(4,4);
        return true;
      }
    return false;
  }
  if (mode == 2) {/**assert(0);**/
    if (goban.moveNumber() <= 10 && goban.height() == 19 && goban.width() == 19) {/**assert(0);**/
      Vector < Location > hoshiStone;
      GoTools::print(openingHoshiStone);
      for (int i=0;i<(int)openingHoshiStone.size();i++)
        if (goban.isLegal(openingHoshiStone[i]) &&
            goban.isIsolatedMove(openingHoshiStone[i]))
          hoshiStone.push_back(openingHoshiStone[i]);
      if (hoshiStone.size() == 0) return false;
      int index = GoTools::mogoRand(hoshiStone.size());
      location = hoshiStone[index];
      return true;
    } else return false;
  }

  if (mode == 3) {/**assert(0);**/
    if (goban.moveNumber() <= 10 && goban.height() == 19 && goban.width() == 19) {/**assert(0);**/
      Vector < Location > hoshiStone;
      assert((int)openingHoshiStone.size() == 9);
      for (int i=0;i<4;i++)
        if (goban.isLegal(openingHoshiStone[i]) &&
            goban.isIsolatedMove(openingHoshiStone[i]))
          hoshiStone.push_back(openingHoshiStone[i]);
      if (hoshiStone.size() == 0)
        for (int i=4;i<(int)openingHoshiStone.size();i++)
          if (goban.isLegal(openingHoshiStone[i]) &&
              goban.isIsolatedMove(openingHoshiStone[i]))
            hoshiStone.push_back(openingHoshiStone[i]);
      if (hoshiStone.size() == 0) return false;
      int index = GoTools::mogoRand(hoshiStone.size());
      location = hoshiStone[index];
      return true;
    } else return false;
  }

  if (mode == 4) {/**assert(0);**/
    if (goban.moveNumber() <= 10 &&
        ((goban.height() == 19 && goban.width() == 19) ||
         (goban.height() == 13 && goban.width() == 13))) {/**assert(0);**/
      Vector < Location > hoshiStone;
      assert((int)openingHoshiStone.size() == 9);
      for (int i=0;i<4;i++)
        if (goban.isLegal(openingHoshiStone[i]) &&
            goban.isIsolatedMove(openingHoshiStone[i]))
          hoshiStone.push_back(openingHoshiStone[i]);
      if (hoshiStone.size() == 0)
        if (goban.isLegal(openingHoshiStone.back())) {/**assert(0);**/
          location = openingHoshiStone.back();
          return true;
        } else {/**assert(0);**/
          for (int i=4;i<(int)openingHoshiStone.size();i++)
            if (goban.isLegal(openingHoshiStone[i]) &&
                goban.isIsolatedMove(openingHoshiStone[i]))
              hoshiStone.push_back(openingHoshiStone[i]);
        }
      if (hoshiStone.size() == 0) return false;
      int index = GoTools::mogoRand(hoshiStone.size());
      location = hoshiStone[index];
      return true;
    } else return false;
  }

  if (mode == 5) {/**assert(0);**/
    if (goban.moveNumber() <= 10 && goban.height() == 19 && goban.width() == 19) {/**assert(0);**/
      Vector < Location > hoshiStone;
      GoTools::print(openingHoshiStone);
      for (int i=0;i<4;i++)
        if (goban.isLegal(openingHoshiStone[i]) &&
            goban.isIsolatedMove(openingHoshiStone[i]))
          hoshiStone.push_back(openingHoshiStone[i]);
      if (hoshiStone.size() == 0) {/**assert(0);**/
        if (goban.isLegal(GobanLocation::locationXY(9,9))) {/**assert(0);**/
          location = GobanLocation::locationXY(9,9);
          return true;
        }
        return false;
      }
      int index = GoTools::mogoRand(hoshiStone.size());
      location = hoshiStone[index];
      return true;
    } else return false;
  }
  if (mode == 6 && goban.height() == 9 && goban.width() == 9)
    if (openingMoveForcedMode1(goban,location))
      return true;
    else return false;

  if (mode==7) {/**assert(0);**/
    if (goban.moveNumber() <= 10 && ( (goban.height() == 19 && goban.width() == 19) || (goban.height() == 13 && goban.width() == 13))) {/**assert(0);**/
      Vector < Location > hoshiStone;
//       GoTools::print(openingHoshiStone);
      for (int i=0;i<4;i++)
        if (goban.isLegal(openingHoshiStone[i]) &&
            goban.isIsolatedSideMove(openingHoshiStone[i]))
          hoshiStone.push_back(openingHoshiStone[i]);
      if (hoshiStone.size() == 0) {/**assert(0);**/

        return false;
      }
      int index = GoTools::mogoRand(hoshiStone.size());
      location = hoshiStone[index];
      return true;
    } else return false;
  }



  return false;

}

bool MoGo::GoPlayer::openingMoveForcedMode1( const Goban & goban, Location & location ) const {/**assert(0);**/
  if (goban.moveNumber()>5) return false;
  if (goban.lastMove() != PASSMOVE && !goban.isIsolatedMove(goban.lastMove())) return false;
  Vector < Location > openingMoves;
  for (int i=0;i<4;i++)
    if (goban.isLegal(openingHoshiStone[i]) && goban.isIsolatedMove(openingHoshiStone[i]))
      openingMoves.push_back(openingHoshiStone[i]);
  if (openingMoves.size() > 0) {/**assert(0);**/
    int index = GoTools::mogoRand(openingMoves.size());
    location = openingMoves[index];
    return true;
  }
  for (int i=4;i<8;i++)
    if (goban.isLegal(openingHoshiStone[i]) && goban.isIsolatedMove(openingHoshiStone[i]))
      openingMoves.push_back(openingHoshiStone[i]);
  if (openingMoves.size() > 0) {/**assert(0);**/
    int index = GoTools::mogoRand(openingMoves.size());
    location = openingMoves[index];
    return true;
  }
  return false;
}




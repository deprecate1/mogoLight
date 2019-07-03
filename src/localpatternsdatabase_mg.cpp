//
// C++ Implementation: localpatternsdatabase_mg
//
// Description:
//
//
// Author: Sylvain Gelly and Yizao Wang <sylvain.gelly@lri.fr and yizao.wang@polytechnique.edu>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "localpatternsdatabase_mg.h"
#include "goban_mg.h"
#include "locationinformation_mg.h"

using namespace MoGo;





namespace MoGo {


  LocalPatternsDatabase::~LocalPatternsDatabase() {/**assert(0);**/}}

MoGo::LocalPatternsDatabase::LocalPatternsDatabase( int gobanSize, int cornerSize ) {/**assert(0);**/
  this->gobanSize=gobanSize;
  patternToLocation.resize(4);
  patternToLocationInside.resize(4);
  Goban goban(gobanSize, gobanSize);
  assert(cornerSize<=gobanSize);
  for (int i=0;i<4;i++) {/**assert(0);**/
    patternToLocation[i].resize(cornerSize*cornerSize);
    patternToLocationInside[i].resize((cornerSize-1)*(cornerSize-1));
  }
  int c=0;
  for (int i=0;i<cornerSize;i++) {/**assert(0);**/
    for (int j=0;j<cornerSize;j++) {/**assert(0);**/
      patternToLocation[0][c]=goban.indexOnArray(i,j);
      patternToLocation[1][c]=goban.indexOnArray(i,gobanSize-j-1);
      patternToLocation[2][c]=goban.indexOnArray(gobanSize-i-1,j);
      patternToLocation[3][c]=goban.indexOnArray(gobanSize-i-1,gobanSize-j-1);
      c++;
    }
  }
  c=0;
  for (int i=0;i<cornerSize-1;i++) {/**assert(0);**/
    for (int j=0;j<cornerSize-1;j++) {/**assert(0);**/
      patternToLocationInside[0][c]=goban.indexOnArray(i,j);
      patternToLocationInside[1][c]=goban.indexOnArray(i,gobanSize-j-1);
      patternToLocationInside[2][c]=goban.indexOnArray(gobanSize-i-1,j);
      patternToLocationInside[3][c]=goban.indexOnArray(gobanSize-i-1,gobanSize-j-1);
      c++;
    }
  }
  createLocationToVectors();
}
MoGo::LocalPatternsDatabase::LocalPatternsDatabase( ) {/**assert(0);**/
  gobanSize=0;
}

void MoGo::LocalPatternsDatabase::createLocationToVectors( ) {/**assert(0);**/
  locationToPatternNumber.resize((gobanSize+2)*(gobanSize+2));
  locationToPositionInPattern.resize((gobanSize+2)*(gobanSize+2));
  for (int i=0;i<(int)patternToLocationInside.size();i++) {/**assert(0);**/
    for (int j=0;j<(int)patternToLocationInside[i].size();j++) {/**assert(0);**/
      locationToPatternNumber[patternToLocationInside[i][j]].push_back(i);
      locationToPositionInPattern[patternToLocationInside[i][j]].resize(patternToLocationInside.size(),-1);
      locationToPositionInPattern[patternToLocationInside[i][j]][i]=j;
    }
  }
}




Vector< int > MoGo::LocalPatternsDatabase::getAllMovesInPosition( const Goban & goban, Vector< double > & _movesImportance ) {/**assert(0);**/
  _movesImportance.clear();
  Vector<int> _allMoves;
  if (gobanSize!=goban.height())
    return Vector<int>();
  int nbPatterns=patternToLocation.size();
  for (int i=0;i<nbPatterns;i++) {/**assert(0);**/
    Vector<int> representation=getRepresentation(goban, i);
    if (allGobans.count(representation)>0) {/**assert(0);**/
      int index=allGobans[representation];
      assert(int(moves[index].size())==nbPatterns);
      concat(_allMoves,moves[index][i]);
      concat(_movesImportance,movesImportance[index]);
    }
  }
  return _allMoves;
}

void MoGo::LocalPatternsDatabase::addMoveInPosition( const Goban & goban, Location move, double moveImportance ) {/**assert(0);**/
  if (gobanSize!=goban.height())
    return;
  if (move==PASSMOVE)
    return;
  const Vector<int> &patterns=locationToPatternNumber[move];
  if (patterns.size()==0)
    return;

  for (int i=0;i<(int)patterns.size();i++) {/**assert(0);**/
    int numPattern=patterns[i];
    Vector<int> representation=getRepresentation(goban, numPattern);
    if (allGobans.count(representation)==0) {/**assert(0);**/
      allGobans[representation]=movesImportance.size();
      movesImportance.push_back(Vector<double>());
      moves.push_back(Vector<Vector<int> >(patternToLocation.size()));
    }
    assert(allGobans.count(representation)>0);
    int index=allGobans[representation];
    assert(locationToPositionInPattern[move].size()==patternToLocation.size());
    int positionInPattern=locationToPositionInPattern[move][numPattern];assert(positionInPattern>=0);
    addMove(index, positionInPattern, moveImportance);
  }
}


bool MoGo::LocalPatternsDatabase::saveToFile( const char * name ) {/**assert(0);**/
  FILE *f=fopen(name,"w");
  if (!f)
    return false;

  fprintf(f, "%i\n", gobanSize);
  printOnFile(patternToLocation, f);
  fprintf(f, "\n");
  printOnFile(patternToLocationInside, f);
  fprintf(f, "\n");
  printOnFile(locationToPatternNumber, f);
  fprintf(f, "\n");
  printOnFile(locationToPositionInPattern, f);
  fprintf(f, "\n");
  printOnFile(movesImportance, f);
  fprintf(f, "\n");
  printOnFile(moves, f);
  fprintf(f, "\n");

  Vector<Vector<int> >keyTmp; Vector<int> tmp;
  for (MoGoHashMap(Vector<int>, int, hashVectorInt, equalVectorInt)::const_iterator it = allGobans.begin() ; it != allGobans.end() ; ++it) {/**assert(0);**/
    keyTmp.push_back(it->first);
    tmp.push_back(it->second);
  }
  printOnFile(keyTmp, f);
  fprintf(f, "\n");
  printOnFile(tmp, f);
  fprintf(f, "\n");

  fclose(f);
  return true;
}

LocalPatternsDatabase * MoGo::LocalPatternsDatabase::loadFromFile( const char * name ) {/**assert(0);**/
  FILE *f=fopen(name,"r");
  if (!f)
    return 0;

  LocalPatternsDatabase *database=new LocalPatternsDatabase();

  fscanf(f, "%i\n", &database->gobanSize);

  readOnFile(database->patternToLocation, f);
  fprintf(f, "\n");
  readOnFile(database->patternToLocationInside, f);
  fprintf(f, "\n");
  readOnFile(database->locationToPatternNumber, f);
  fprintf(f, "\n");
  readOnFile(database->locationToPositionInPattern, f);
  fprintf(f, "\n");
  readOnFile(database->movesImportance, f);
  fprintf(f, "\n");
  readOnFile(database->moves, f);
  fprintf(f, "\n");

  Vector<Vector<int> >keyTmp; Vector<int> tmp;
  readOnFile(keyTmp, f);
  readOnFile(tmp, f);
  if (keyTmp.size()!=tmp.size()) {/**assert(0);**/
    GoTools::print("keyTmp %, tmp %i\n", keyTmp.size(), tmp.size());
    assert(0);
  }
  //   database->allGobans.resize(100000);
  GoTools::print("loadFromFile allGobans (%i)\n", tmp.size());
  for (int i=0;i<(int)tmp.size();i++)
    //     database->allGobans.insert(std::pair<const Vector<int>, int>(keyTmp[i],tmp[i]));
    database->allGobans[keyTmp[i]]=tmp[i];
  GoTools::print("loadFromFile allGobans (%i) END\n", tmp.size());

  fclose(f);
  return database;
}

Vector< int > MoGo::LocalPatternsDatabase::getRepresentation( const Goban & goban, int numPattern ) {/**assert(0);**/
  assert(numPattern<(int)patternToLocation.size()); assert(numPattern>=0);
  Vector<Location> &ptl=patternToLocation[numPattern];
  Vector<int> representation((ptl.size()+15)/16);
  int playerColor=goban.whoToPlay();
  int numInt=0, numBit=0;
  for (int i=0;i<(int)ptl.size();i++) {/**assert(0);**/
    int state=goban.getGobanState()[ptl[i]];
    assert(state!=OUTSIDE);
    if (state==playerColor)
      state=1;
    else if (state==3-playerColor)
      state=2;
    // GoTools::print("numInt %i, numBit %i\n", numInt, numBit);
    assert(numInt<(int)representation.size());
    representation[numInt]+=state<<numBit;
  numBit+=2; if (numBit>=32) {/**assert(0);**/ numInt++; numBit=0; }

  }
  return representation;
}

int MoGo::LocalPatternsDatabase::addMove( int index, int positionInPattern, double importance ) {/**assert(0);**/
  //   assert(movesImportance[index].size()==moves[index].size());
  int nbPatterns=patternToLocationInside.size();
  assert(int(moves[index].size())==nbPatterns);
  int pos=-1; bool addedMove=false;
  for (int num=0;num<nbPatterns;num++) {/**assert(0);**/
    // GoTools::print("num %i\n", num);
    if (num==0)
      assert(movesImportance[index].size()==moves[index][num].size());
    else if (addedMove)
      assert(movesImportance[index].size()==moves[index][num].size()+1);
    Location move=patternToLocationInside[num][positionInPattern];
    if (num==0) {/**assert(0);**/
      for (int i=0;i<(int)movesImportance[index].size();i++) {/**assert(0);**/
        if (moves[index][num][i]==move) {/**assert(0);**/
          pos=i;
          break;
        }
      }
      if (pos<0) {/**assert(0);**/
        pos=moves[index][num].size();
        moves[index][num].push_back(move);
        movesImportance[index].push_back(importance);
        addedMove=true;
      } else {/**assert(0);**/
        if (num==0)
          movesImportance[index][pos]+=importance;
      }
    } else {/**assert(0);**/
      assert(pos>=0);
      if (addedMove)
        moves[index][num].push_back(move);
    }
  }
  return 0;
}

Location MoGo::LocalPatternsDatabase::getOneRandomMoveInPosition( const Goban & goban ) {/**assert(0);**/
  Vector<int> _allMoves;
  if (gobanSize!=goban.height())
    return PASSMOVE;
  int nbPatterns=patternToLocation.size();
  for (int i=0;i<nbPatterns;i++) {/**assert(0);**/
    Vector<int> representation=getRepresentation(goban, i);
    if (allGobans.count(representation)>0) {/**assert(0);**/
      int index=allGobans[representation];
      assert(int(moves[index].size())==nbPatterns);
      concat(_allMoves,moves[index][i]);
    }
  }
  while (_allMoves.size()>0) {/**assert(0);**/
    int index=GoTools::mogoRand(_allMoves.size());
    Location location=_allMoves[index];
    if (goban.isNotUseless(location))
      return location;
    _allMoves[index]=_allMoves.back();
    _allMoves.pop_back();
  }
  return PASSMOVE;
}



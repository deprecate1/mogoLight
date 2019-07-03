//
// C++ Implementation: openingdatabase_mg
//
// Description:
//
//
// Author: Yizao Wang <yizao.wang@polytechnique.edu>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "openingdatabase_mg.h"
#include "goban_mg.h"
#include "gotools_mg.h"
#include <math.h>
#include "gogame_mg.h"

using namespace MoGo;

namespace MoGo {
  int Database3::minimumDatabaseUse=20;
  OpeningDatabase::OpeningDatabase() {/**assert(0);**/}


  OpeningDatabase::~OpeningDatabase() {/**assert(0);**/}}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
MoGo::Database3::Database3(int gobanSize, bool useOldFormat) : referenceGoban(gobanSize,gobanSize) {
  nbGobans=0;
  this->useOldFormat=useOldFormat;
}
bool MoGo::Database3::isGobanAndMoveInDatabase( const GobanKey & key, int move ) {/**assert(0);**/
  if (allGobans.count(key)==0)
    return false;
  if (allGobans.find(key)->second.count(move)==0)
    return false;

  return true;
}

int MoGo::Database3::indexOfGobanAndMove( const GobanKey & key, int move ) {/**assert(0);**/
  if (allGobans.count(key)==0) {/**assert(0);**/
    allGobans[key]=MoGoHashMap(int, int, hashInt, equalInt)();
    nbGobans++;
  }
  if (allGobans[key].count(move)==0) {/**assert(0);**/
    allGobans[key][move]=values.size();
    values.push_back(Vector<double>(2));
  }
  return allGobans[key][move];
}


bool MoGo::Database3::addValues( const Goban & _goban, const Location move, double value0, double value1, bool discardOneSimulationGobans, bool testIfLegal ) {/**assert(0);**/
  GobanKey minKey=_goban.getGobanKey(); Location minKeyMove=move;
  Goban goban(_goban);Location currentMove=move;
  for (int j=0;j<4;j++) {/**assert(0);**/
    currentMove=goban.rotate(currentMove); goban=Goban(goban.generateRotatedGobanKey());
    if (testIfLegal)
      if (!goban.isLegal(currentMove)) {/**assert(0);**/
        _goban.textModeShowGoban();_goban.textModeShowPosition(move); goban.textModeShowGoban(); goban.textModeShowPosition(currentMove);assert(0);
      }
    if ((goban.getGobanKey() < minKey) || ((goban.getGobanKey() == minKey) && (currentMove<minKeyMove))) {/**assert(0);**/
      minKey=goban.getGobanKey(); minKeyMove=currentMove;
    }
    Goban sym(goban.generateSymmetrizedGobanKey());
    if (testIfLegal)
      if (!sym.isLegal(sym.symmetrize(currentMove))) {/**assert(0);**/
        _goban.textModeShowGoban();_goban.textModeShowPosition(move); sym.textModeShowGoban(); sym.textModeShowPosition(sym.symmetrize(currentMove));  assert(0);
      }
    if ((sym.getGobanKey() < minKey) || ((sym.getGobanKey() == minKey) && (sym.symmetrize(currentMove)<minKeyMove))) {/**assert(0);**/
      minKey=sym.getGobanKey(); minKeyMove=sym.symmetrize(currentMove);
    }
  }


  return addValues(minKey,minKeyMove,value0,value1, _goban.height(),_goban.approximateSizeOfPossibleMoves(),discardOneSimulationGobans);
}
bool MoGo::Database3::addValues( const GobanKey & key, const Location move, double value0, double value1, int /*size*/, int /*approximateSizeOfPossibleMoves*/, bool discardOneSimulationGobans ) {/**assert(0);**/
  // goban.textModeShowGoban();
  // printf("value : %i\n", goban.approximateSizeOfPossibleMoves() - goban.height()*goban.width()+20);
  // getchar();
  //if (approximateSizeOfPossibleMoves < size*size-15) // We throw away all the goban with more than 15 moves played //TODO Yizao disabled this line.
  //return false;
  if (discardOneSimulationGobans)
    if (value0+value1<2)
      return false;
  int index=indexOfGobanAndMove(key,move);
  assert(index>=0); assert(index<(int)values.size());
  values[index][0]+=value0;
  values[index][1]+=value1;
  return true;
}


bool MoGo::Database3::saveToFile( const std::string & fileName, bool newFile ) {/**assert(0);**/
  if (nbGobans==0) {/**assert(0);**/
    GoTools::print("nbGobans == 0!\n");
    return false;
  }
  FILE *f;
  if (newFile)
    f=fopen(fileName.c_str(), "w");
  else
    f=fopen(fileName.c_str(), "a");
  if (!f)
    return false;

  int count=0;
  for (MoGoHashMap(GobanKey, MoGoHashMap(int, int, hashInt, equalInt), hashGobanKey, equalGobanKey)::const_iterator it = allGobans.begin()
       ; it != allGobans.end() ; ++it) {/**assert(0);**/
    GobanKey key = it->first;
    if (newFile) {/**assert(0);**/
      if (count == 0)
        fprintf(f,"%i\n", Goban(key).height());fflush(f);
    }
    const MoGoHashMap(int, int, hashInt, equalInt) &movesToIndex=it->second;
    for (MoGoHashMap(int, int, hashInt, equalInt)::const_iterator jt = movesToIndex.begin()
         ; jt != movesToIndex.end() ; ++jt) {/**assert(0);**/
      for (int k = 0 ; k < (int)key.size() ; k++) {/**assert(0);**/
        fprintf(f, "%i ", key[k]);
      }
      fprintf(f, "%i ", jt->first);
      fprintf(f, "%f %f\n", values[jt->second][0], values[jt->second][1]);
      fflush(f);
    }
    count ++;
  }
  fclose(f);
  return true;
}

 // TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::Database3::loadFromFile( const std::string & fileName, bool discardNonBeginningGobans ) {
  clear();
  return addFromFile(fileName, discardNonBeginningGobans);
}

 // TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::Database3::addFromFile( const std::string & fileName, bool discardNonBeginningGobans) {
  FILE *f=fopen(fileName.c_str(),"r");
  if (!f)
    return false;


  int gobanSize=0;
  fscanf(f, "%i", &gobanSize);
  if (gobanSize <= 0) {/**assert(0);**/
    fclose(f);
    return false;
  }

  int nbIllegalMoves=0;
  Goban goban(gobanSize, gobanSize); referenceGoban=goban;
  GobanKey key=goban.getGobanKey();
  bool eof=false; int nbEntries=0;
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

      float value0, value1;
      nb=0;nb=fscanf(f, "%f", &value0); assert(nb == 1);
      nb=0;nb=fscanf(f, "%f", &value1); assert(nb == 1);

      if (useOldFormat)
        if (!Goban(key).isLegal(move)) {/**assert(0);**/
          nbIllegalMoves++;
          continue;
        }
      int approximateSizeOfPossibleMoves=1000;
      if (discardNonBeginningGobans)
        approximateSizeOfPossibleMoves=Goban(key).approximateSizeOfPossibleMoves();
      // GoTools::print(key);
      // GoTools::print("\nkey \n, move %i, v0 %f, v1 %f\n", move, value0, value1);
      if (useOldFormat) {/**assert(0);**/
        if (addValues(Goban(key), move, value0, value1, true))
          nbEntries++;
      } else {/**assert(0);**/
        if (addValues(key, move, value0, value1, gobanSize, approximateSizeOfPossibleMoves, true))
          nbEntries++;
      }
    }
  }


  fclose(f);
  GoTools::print("Load opening database %s succeed (nbEntries=%i) (nbIllegalMoves removed %i)\n", fileName.c_str(), nbEntries, nbIllegalMoves);
  return true;
}

 // TEMPORARY COMMENT: THIS FUNCTION IS USED
void MoGo::Database3::clear( ) {
  allGobans.clear(); values.clear();
  nbGobans=0;
}

double MoGo::Database3::valueForThisMove( const Goban & _goban, const Location move, double & value0, double & value1 ) {/**assert(0);**/

  GobanKey minKey=_goban.getGobanKey(); Location minKeyMove=move;
  Goban goban(_goban);Location currentMove=move;
  for (int j=0;j<4;j++) {/**assert(0);**/
    currentMove=goban.rotate(currentMove); goban=Goban(goban.generateRotatedGobanKey());
    if ((goban.getGobanKey() < minKey) || ((goban.getGobanKey() == minKey) && (currentMove<minKeyMove))) {/**assert(0);**/
      minKey=goban.getGobanKey(); minKeyMove=currentMove;
    }
    Goban sym(goban.generateSymmetrizedGobanKey());
    if ((sym.getGobanKey() < minKey) || ((sym.getGobanKey() == minKey) && (sym.symmetrize(currentMove)<minKeyMove))) {/**assert(0);**/
      minKey=sym.getGobanKey(); minKeyMove=sym.symmetrize(currentMove);
    }
  }

  return valueForThisMove(minKey, minKeyMove, value0, value1);

}
double MoGo::Database3::valueForThisMove( const GobanKey & minKey, const Location minKeyMove, double & value0, double & value1 ) {/**assert(0);**/
  if (!isGobanAndMoveInDatabase(minKey, minKeyMove)) {/**assert(0);**/
    value0=0.; value1=0.;
    return 0.;
  } else {/**assert(0);**/
    int index=indexOfGobanAndMove(minKey,minKeyMove);
    assert(index < (int)values.size());
    value0=values[index][0]; value1=values[index][1];
    if (values[index][0]==0. && values[index][1]==0.)
      return 0.;
    return value0/(value0+value1);
  }
}

// TEMPORARY COMMENT: THIS FUNCTION IS USED
bool MoGo::Database3::chooseMove( const Goban & _goban, Location & location, double *prediction, bool display, bool penalizeUncertainty ) {
  int nbRotations=0;bool useSymetry=false;
  if (allGobans.size()==0)
    return false;
  GobanKey representative=getRepresentative(_goban, nbRotations, useSymetry);
  //   Location representativeMove=transformMove(move, nbRotations, useSymetry);

  if (allGobans.count(representative)==0)
    return false;

  const MoGoHashMap(int, int, hashInt, equalInt) &movesToIndex=allGobans[representative];
  double totalValues=0.;
  Location bestMove=PASSMOVE; double bestOdd=-1.; double bestWin=0.; double bestLoss=0.;
  for (MoGoHashMap(int, int, hashInt, equalInt)::const_iterator jt = movesToIndex.begin()
       ; jt != movesToIndex.end() ; ++jt) {/**assert(0);**/
    Location l=jt->first;
    if (l == PASSMOVE)
      continue;
    double tot=values[jt->second][0]+values[jt->second][1];
    assert(tot>0.);
    totalValues += tot;
    double v=values[jt->second][0]/tot;
    if (penalizeUncertainty)
      v=v-10./sqrt(tot+1.); // To handle roughly the uncertainty
    else
      v=v+1./sqrt(tot+1.); // To handle roughly the uncertainty
    if (bestOdd < v) {/**assert(0);**/
      bestOdd=v;bestMove=l; bestWin=values[jt->second][0]; bestLoss=values[jt->second][1];
    }
  }
  if (totalValues > double(minimumDatabaseUse)) {/**assert(0);**/
    if (display) {/**assert(0);**/
      GoTools::print("the database predicts this move with urgency %4.2f (odd %4.2f) (%4.2f/%4.2f) (over %4.2f total moves on this position)\n", bestOdd, bestWin/(bestWin+bestLoss), bestWin,(bestWin+bestLoss), totalValues);
      GoTools::print(_goban.getGobanKey());
    }
    if (prediction)
      *prediction=bestOdd;
    location=untransformMove(bestMove, nbRotations, useSymetry);
    if (location==PASSMOVE) return false;
    return true;
  } else
    return false;


}

bool MoGo::Database3::chooseAMoveRandomly( const Goban & _goban, Location & location, bool moreDeterministic ) {/**assert(0);**/
  int nbRotations=0;bool useSymetry=false;
  GobanKey representative=getRepresentative(_goban, nbRotations, useSymetry);

  if (allGobans.count(representative)==0)
    return false;

  const MoGoHashMap(int, int, hashInt, equalInt) &movesToIndex=allGobans[representative];
  double totalValues=0.;
  double totalUrgency=0.;
  Vector<double> urgencies;
  Vector<Location> possibleMoves;
  for (MoGoHashMap(int, int, hashInt, equalInt)::const_iterator jt = movesToIndex.begin()
       ; jt != movesToIndex.end() ; ++jt) {/**assert(0);**/
    Location l=jt->first;
    if (l == PASSMOVE)
      continue;
    double tot=values[jt->second][0]+values[jt->second][1];
    assert(tot>0.);
    totalValues += tot;
    double v=values[jt->second][0]/tot;
    v=v+1./sqrt(tot+1.); // To handle roughly the uncertainty. Indeed we want to play more often the moves not so played
    urgencies.push_back(v); totalUrgency+=v;
    possibleMoves.push_back(l);
  }
  double playInDatabase=randDouble()*200.;
  double urgency;
  if (totalValues > playInDatabase) {/**assert(0);**/
    assert(totalUrgency>0.); location=PASSMOVE;
    if (!moreDeterministic) {/**assert(0);**/
      double r=randDouble()*totalUrgency; int chosenI=-1;
      for (int i=0;(i<(int)possibleMoves.size()) && (r >= 0.);i++) {/**assert(0);**/
        r -= urgencies[i];
        if (r<0.) {/**assert(0);**/
          location = possibleMoves[i];
          chosenI = i;
        }
      }
      location=untransformMove(location, nbRotations, useSymetry);
      urgency=urgencies[chosenI];
    } else {/**assert(0);**/
      if (!chooseMove(_goban, location, &urgency, true, false))
        return false;
    }
    assert(location != PASSMOVE);
    GoTools::print("RandomPlayed Move. The database predicts this move with urgency %4.2f (over %4.2f total moves on this position)\n", urgency, totalValues);
    return true;
  } else
    return false;
}
bool MoGo::Database3::chooseAMoveWithUCT( const Goban & _goban, Location & location, double p2Meta ) {/**assert(0);**/
  int nbRotations=0;bool useSymetry=false;
  GobanKey representative=getRepresentative(_goban, nbRotations, useSymetry);

  if (allGobans.count(representative)==0)
    return false;

  const MoGoHashMap(int, int, hashInt, equalInt) &movesToIndex=allGobans[representative];
  double totalValues=0.;
  Vector<double> urgencies; Vector<Location> possibleMoves; Vector<double>nbSimulationsforThisMove;
  for (MoGoHashMap(int, int, hashInt, equalInt)::const_iterator jt = movesToIndex.begin()
       ; jt != movesToIndex.end() ; ++jt) {/**assert(0);**/
    Location l=jt->first;
    if (l == PASSMOVE)
      continue;
    double tot=values[jt->second][0]+values[jt->second][1];
    assert(tot>0.);
    totalValues += tot;
    double v=values[jt->second][0]/tot;
    v=v;
    nbSimulationsforThisMove.push_back(tot);assert(nbSimulationsforThisMove[nbSimulationsforThisMove.size()-1]>0.);
    urgencies.push_back(v); possibleMoves.push_back(l);
  }
  double playInDatabase=randDouble()*200.;
  double urgency;
  if (totalValues > playInDatabase) {/**assert(0);**/
    location=PASSMOVE;
    double logNbS=log(totalValues);
    double maxUrgency=-1e100;int chosenI=-1;
    double p2=p2Meta;
    for (int i=0;i<(int)urgencies.size();i++) {/**assert(0);**/
      double variance=urgencies[i]-urgencies[i]*urgencies[i];
      double confidence=variance+
                        1.*sqrt(2.*logNbS/nbSimulationsforThisMove[i]);
      if (confidence > .25) confidence=.25;
      //       double tmp=urgencies[i]+p2*sqrt(logNbS*logNbS/nbSimulationsforThisMove[i]*confidence); //FIXME I have put logNbS*logNbS in place of logNbS alone
      double tmp=urgencies[i]+p2*sqrt(logNbS/nbSimulationsforThisMove[i]/5.);
      if (tmp>maxUrgency) {/**assert(0);**/
        maxUrgency=tmp;chosenI=i;location=possibleMoves[i];
      }
    }

    assert(chosenI>=0);
    location=untransformMove(location, nbRotations, useSymetry);
    urgency=urgencies[chosenI];
    assert(location != PASSMOVE);
    GoTools::print("UCT Move played. The database predicts this move with urgency %4.2f (odd %4.2f) (%4.2f/%4.2f total moves on this position)\n", maxUrgency, urgencies[chosenI], nbSimulationsforThisMove[chosenI],totalValues);
    return true;
  } else
    return false;
}



void MoGo::Database3::getAllGobanKeyAndMoves( Vector< GobanKey > & keys, Vector< Location > & moves ) {/**assert(0);**/
  keys.clear(); moves.clear();
  if (nbGobans==0)
    return ;

  for (MoGoHashMap(GobanKey, MoGoHashMap(int, int, hashInt, equalInt), hashGobanKey, equalGobanKey)::const_iterator it = allGobans.begin()
       ; it != allGobans.end() ; ++it) {/**assert(0);**/
    GobanKey key = it->first;
    const MoGoHashMap(int, int, hashInt, equalInt) &movesToIndex=it->second;
    for (MoGoHashMap(int, int, hashInt, equalInt)::const_iterator jt = movesToIndex.begin()
         ; jt != movesToIndex.end() ; ++jt) {/**assert(0);**/

      keys.push_back(key); moves.push_back(jt->first);
    }
  }
}

GobanKey MoGo::Database3::getRepresentative( const Goban & _goban, int & nbRotations, bool & useSymetry ) {/**assert(0);**/
  GobanKey minKey=_goban.getGobanKey(); nbRotations=0;useSymetry=false;
  Goban goban(_goban);
  Goban symTmp(_goban.generateSymmetrizedGobanKey());
  if (symTmp.getGobanKey() < minKey) {/**assert(0);**/
    minKey=symTmp.getGobanKey(); nbRotations=0;useSymetry=true;
  }

  for (int j=1;j<4;j++) {/**assert(0);**/
    goban=Goban(goban.generateRotatedGobanKey());
    if (goban.getGobanKey() < minKey) {/**assert(0);**/
      minKey=goban.getGobanKey(); nbRotations=j; useSymetry=false;
    }
    Goban sym(goban.generateSymmetrizedGobanKey());
    if (sym.getGobanKey() < minKey) {/**assert(0);**/
      minKey=sym.getGobanKey(); nbRotations=j;useSymetry=true;
    }
  }
  // _goban.textModeShowGoban();
  return minKey;
}

Location MoGo::Database3::transformMove( const Location move, int nbRotations, bool useSymetry ) {/**assert(0);**/
  Location result=move;
  for (int i=0;i<nbRotations;i++) {/**assert(0);**/
    result=referenceGoban.rotate(result);
  }
  if (useSymetry)
    result=referenceGoban.symmetrize(result);

  return result;
}

Location MoGo::Database3::untransformMove( const Location move, int nbRotations, bool useSymetry ) {/**assert(0);**/
  Location result=move;
  if (useSymetry)
    result=referenceGoban.symmetrize(result);

  if (nbRotations >0) {/**assert(0);**/
    nbRotations=4-nbRotations;
    for (int i=0;i<nbRotations;i++) {/**assert(0);**/
      result=referenceGoban.rotate(result);
    }
  }

  return result;
}

bool MoGo::Database3::isOneRepresentativeOfGobanInDatabase( const Goban & _goban ) {/**assert(0);**/
  int nbRotations=0;bool useSymetry=false;
  GobanKey representative=getRepresentative(_goban, nbRotations, useSymetry);

  if (allGobans.count(representative)==0)
    return false;
  else
    return true;
}
int MoGo::Database3::nbRepresentativesOfGobanInDatabase( const Goban & _goban ) {/**assert(0);**/
  int nbRotations=0;bool useSymetry=false;
  GobanKey representative=getRepresentative(_goban, nbRotations, useSymetry);
  if (allGobans.count(representative)>0) {/**assert(0);**/
    const MoGoHashMap(int, int, hashInt, equalInt) &movesToIndex=allGobans[representative];
    double totalValues=0.;
    for (MoGoHashMap(int, int, hashInt, equalInt)::const_iterator jt = movesToIndex.begin()
         ; jt != movesToIndex.end() ; ++jt) {/**assert(0);**/
      Location l=jt->first;
      if (l == PASSMOVE)
        continue;
      double tot=values[jt->second][0]+values[jt->second][1];
      assert(tot>0.);
      totalValues += tot;
    }
    return (int)totalValues;
  } else
    return 0;
}

void MoGo::Database3::showAllEntries( ) {/**assert(0);**/

  for (MoGoHashMap(GobanKey, MoGoHashMap(int, int, hashInt, equalInt), hashGobanKey, equalGobanKey)::const_iterator it = allGobans.begin()
       ; it != allGobans.end() ; ++it) {/**assert(0);**/
    GobanKey key = it->first;
    Goban tmpGoban(key);
    Location location = 0;
    chooseMove(tmpGoban,location);
    GoTools::print("Goban:\n");
    tmpGoban.textModeShowGoban();
    GoTools::print("proposed move:");
    tmpGoban.textModeShowPosition(location);
    getchar();
  }
}

void MoGo::Database3::saveToSGF( const char * destinationFileName, double lambda ) {/**assert(0);**/
  MoGoHashMap(GobanKey, int, hashGobanKey, equalGobanKey) visitedNodes;
  for (MoGoHashMap(GobanKey, MoGoHashMap(int, int, hashInt, equalInt), hashGobanKey, equalGobanKey)::const_iterator it = allGobans.begin()
       ; it != allGobans.end() ; ++it) {/**assert(0);**/
    GobanKey key=it->first;
    if (visitedNodes.count(key)>0)
      continue;
    //     Goban(key).textModeShowGoban();
    assert(allGobans.count(key)>0);
    iterativeMarkPositions(key, visitedNodes);
  }
  MoGoHashMap(GobanKey, double, hashGobanKey, equalGobanKey) allPositionsValues;
  MoGoHashMap(GobanKey, double, hashGobanKey, equalGobanKey) allPositionsWeights;
  for (MoGoHashMap(GobanKey, MoGoHashMap(int, int, hashInt, equalInt), hashGobanKey, equalGobanKey)::const_iterator it = allGobans.begin()
       ; it != allGobans.end() ; ++it) {/**assert(0);**/
    if (visitedNodes.count(it->first)>0)
      continue;
    //      Goban(it->first).textModeShowGoban();
    computePositionEstimates(lambda, 0.5, it->first, allPositionsValues, allPositionsWeights);
  }

  std::string saveFileNameBase(destinationFileName);
  std::string saveFileNameScores(saveFileNameBase+".scores");
  FILE *f=fopen(saveFileNameScores.c_str(),"w");
  if (!f) return;
  int k=0;
  for (MoGoHashMap(GobanKey, MoGoHashMap(int, int, hashInt, equalInt), hashGobanKey, equalGobanKey)::const_iterator it = allGobans.begin()
       ; it != allGobans.end() ; ++it) {/**assert(0);**/
    assert(allPositionsValues.count(it->first)>0);
    assert(allPositionsWeights.count(it->first)>0);
    Goban goban(it->first);
    double value=allPositionsValues[it->first];
    int nbStones=goban.countNbStones();
    std::string r="value="+GoTools::toString(value)+";nbStones="+GoTools::toString(nbStones);

    std::string tmp;
    GoTools::sprint(tmp, "%05i", k);

    GoGame::saveGobanPosition(goban, saveFileNameBase+tmp,r);
    fprintf(f,"%f %i\n", value, nbStones);
    k++;
  }
  fclose(f);
}

void MoGo::Database3::iterativeMarkPositions(const GobanKey &key, MoGoHashMap(GobanKey, int, hashGobanKey, equalGobanKey) &visitedNodes) {/**assert(0);**/
  assert(allGobans.count(key)>0);
  const MoGoHashMap(int, int, hashInt, equalInt) &movesToIndex=allGobans[key];
  for (MoGoHashMap(int, int, hashInt, equalInt)::const_iterator jt = movesToIndex.begin()
       ; jt != movesToIndex.end() ; ++jt) {/**assert(0);**/
    Location move=jt->first;

    Goban tmp(key); tmp.playMoveOnGoban(move); int nbRotations; bool useSymetry;
    GobanKey tmpKey=getRepresentative(tmp, nbRotations, useSymetry);
    if (visitedNodes.count(tmpKey)>0)
      continue;
    //     tmp.textModeShowGoban();
    visitedNodes[tmpKey]=1;
    if (allGobans.count(tmpKey)==0)
      continue;
    iterativeMarkPositions(tmpKey, visitedNodes);
  }
}


double MoGo::Database3::computePositionEstimates(double lambda, double priorPositionEstimate, const GobanKey &key,
    MoGoHashMap(GobanKey, double, hashGobanKey, equalGobanKey) &allPositionValues,
    MoGoHashMap(GobanKey, double, hashGobanKey, equalGobanKey) &allPositionWeights) {/**assert(0);**/
  assert(allGobans.count(key)>0);
  if (allPositionValues.count(key)>0) {/**assert(0);**/
    assert(allPositionWeights.count(key)>0);
    return allPositionValues[key];
  }
  assert(allPositionWeights.count(key)==0);

  const MoGoHashMap(int, int, hashInt, equalInt) &movesToIndex=allGobans[key];
  double sum=0.;
  double trajectoryWeightsSum=0.;
  double positionWeight=0.;
  int count=0;
  for (MoGoHashMap(int, int, hashInt, equalInt)::const_iterator jt = movesToIndex.begin()
       ; jt != movesToIndex.end() ; ++jt) {/**assert(0);**/
    Location move=jt->first;
    double value0, value1;  double positionValue=valueForThisMove(key, move, value0, value1);

    double trajectoryWeight=value0+value1;
    trajectoryWeightsSum+=trajectoryWeight;

    Goban tmp(key); tmp.playMoveOnGoban(move); int nbRotations; bool useSymetry;
    GobanKey tmpKey=getRepresentative(tmp, nbRotations, useSymetry);
    if (allGobans.count(tmpKey)==0)
      continue;
    double v=computePositionEstimates(lambda, positionValue, tmpKey, allPositionValues, allPositionWeights);
    assert(allPositionValues.count(tmpKey)>0); assert(allPositionWeights.count(tmpKey)>0);
    double childPositionWeight=allPositionWeights[tmpKey];
    double trajectoryValue=(priorPositionEstimate+lambda*v*childPositionWeight)/(1.+lambda*childPositionWeight);
    positionWeight+=trajectoryWeight*(1.+lambda*childPositionWeight);
    sum+=trajectoryWeight*trajectoryValue;
    count++;
  }
  if (count==0) {/**assert(0);**/ // end position
    allPositionWeights[key]=1.;
    allPositionValues[key]=priorPositionEstimate;
  } else {/**assert(0);**/
    positionWeight/=trajectoryWeightsSum; sum/=trajectoryWeightsSum;
    allPositionWeights[key]=positionWeight;
    allPositionValues[key]=sum;
  }

  return allPositionValues[key];
}


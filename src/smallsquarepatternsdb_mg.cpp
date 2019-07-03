//
// C++ Implementation: smallsquarepatternsdb_mg
//
// Description:
//
//
// Author: Sylvain Gelly <sylvain.gelly@lri.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "smallsquarepatternsdb_mg.h"
#include "goban_mg.h"
#include "gotools_mg.h"
#include <math.h>

namespace MoGo {

  SmallSquarePatternsDB::SmallSquarePatternsDB() {/**assert(0);**/}


  SmallSquarePatternsDB::~SmallSquarePatternsDB() {/**assert(0);**/}}

void MoGo::SmallSquarePatternsDB::addPattern( const Goban & goban, Location location, bool isPlayed ) {/**assert(0);**/
  if (goban.getGobanState()[location]!=EMPTY) return;
  long long representation=getLocationPattern5x5(goban,location);
  if (patternToIndex.count(representation)==0) {/**assert(0);**/
    patternToIndex[representation]=nbOccurence.size();
    nbPlayed.push_back(0.);
    nbOccurence.push_back(0.);
    elo.push_back(1000.);
    if (nbPlayed.size()%100000==0) {/**assert(0);**/ //FIXME
      GoTools::print("database size %i\n", nbPlayed.size());
    }
  }
  int index=patternToIndex[representation];
  if (isPlayed)
    nbPlayed[index]++;
  nbOccurence[index]++;
}


double MoGo::SmallSquarePatternsDB::probabilityOfThisMove( const Goban & goban, Location location, const Vector<double> &distances ) {/**assert(0);**/
  long long representation=getLocationPattern5x5(goban,location, distances);
  if (patternToIndex.count(representation)==0) {/**assert(0);**/
    return -1.;
  }
  int index=patternToIndex[representation];
  //      return nbPlayed[index]/nbOccurence[index];
  return elo[index]; //FIXME
}




void MoGo::SmallSquarePatternsDB::reduceDB( ) {/**assert(0);**/}

double MoGo::SmallSquarePatternsDB::getNbPlayed( const Goban & goban, Location location ) {/**assert(0);**/
  long long representation=getLocationPattern5x5(goban,location);
  if (patternToIndex.count(representation)==0) {/**assert(0);**/
    return -1.;
  }
  int index=patternToIndex[representation];
  return nbPlayed[index];
}

double MoGo::SmallSquarePatternsDB::getNbOccurence( const Goban & goban, Location location ) {/**assert(0);**/
  long long representation=getLocationPattern5x5(goban,location);


  if (patternToIndex.count(representation)==0) {/**assert(0);**/
    return -1.;
  }
  int index=patternToIndex[representation];
  return nbOccurence[index];
}


Location MoGo::SmallSquarePatternsDB::chooseMove(const Goban &goban) {/**assert(0);**/
  Location location=PASSMOVE; double maxValue=-1.;
  Vector<double> distances=goban.distanceByGroupsFromReference(goban.lastMove());


  for (int i=0;i<goban.height();i++) {/**assert(0);**/
    for (int j=0;j<goban.width();j++) {/**assert(0);**/
      Location tmpLocation=goban.indexOnArray(i,j);
      if (goban.getGobanState()[tmpLocation]!=EMPTY) continue;
      double prob=probabilityOfThisMove(goban, tmpLocation, distances);
      //       prob=winningProba(prob, 1000.);
      prob/=((distances[tmpLocation]+4.)*0.2);
      if (prob>maxValue) {/**assert(0);**/
        maxValue=prob;
        location=tmpLocation;
      }
    }
  }
  return location;
}






double MoGo::SmallSquarePatternsDB::winningProba(double elo1, double elo2) {/**assert(0);**/
  return 1./(1.+pow(10., (elo2-elo1)/400.));
}
double MoGo::SmallSquarePatternsDB::newElo(double elo1, double elo2, double k, bool win) {/**assert(0);**/
  double prob=winningProba(elo1, elo2);
  //   GoTools::print("prob %f, diff %f\n", prob, double(win)-prob);
  return elo1+k*(double(win)-prob);
}






void MoGo::SmallSquarePatternsDB::addAllPatterns( const Goban & goban, Location location ) {/**assert(0);**/
 
  updateAllElos(goban, location);
}





void MoGo::SmallSquarePatternsDB::updateAllElos(const Goban &goban, Location played) {/**assert(0);**/
  Vector<double> distances=goban.distanceByGroupsFromReference(goban.lastMove());

  long long playedRepresentation=getLocationPattern5x5(goban, played, distances);
  insert(goban, played, distances);

  int index=getIndex(playedRepresentation);
  nbOccurence[index]++; nbPlayed[index]++;


  for (int i=0;i<goban.height();i++) {/**assert(0);**/
    for (int j=0;j<goban.width();j++) {/**assert(0);**/
      Location tmpLocation=goban.indexOnArray(i,j);
      long long representation=getLocationPattern5x5(goban, tmpLocation, distances);
      if (goban.getGobanState()[tmpLocation]!=EMPTY) continue;
      if (representation==playedRepresentation) continue;
      //       GoTools::print("played %i, tmpLocation %i\n", played, tmpLocation);
      insert(goban, tmpLocation, distances);
      updateTwoElos(playedRepresentation, representation);
    }
  }
}


void MoGo::SmallSquarePatternsDB::updateTwoElos(long long playedRepresentation, long long representation) {/**assert(0);**/
  int index1=getIndex(playedRepresentation);
  int index2=getIndex(representation);
  double elo1=elo[index1];
  double elo2=elo[index2];
  double k1=getK(index1,index2);
  double k2=getK(index2,index1);

  //   GoTools::print("elos before: %f, %f\n", elo[index1], elo[index2]);

  elo[index1]=newElo(elo1, elo2, k1, true);
  elo[index2]=newElo(elo2, elo1, k2, false);

  //   GoTools::print("elos after: %f, %f\n", elo[index1], elo[index2]);
  //   getchar();

  nbOccurence[index2]++;
}


int MoGo::SmallSquarePatternsDB::getIndex(long long representation) {/**assert(0);**/
 
  assert(patternToIndex.count(representation)!=0);

  return patternToIndex[representation];
}


double MoGo::SmallSquarePatternsDB::getK(int index) {/**assert(0);**/
  return 1.+20./sqrt(nbOccurence[index]+1);
  //    return 32.;
}

double MoGo::SmallSquarePatternsDB::getK( int index1, int /*index2*/ ) {/**assert(0);**/
  //   double k1=getK(index1);
  //   double k2=getK(index2);
  return getK(index1);
  //   double sum=sqrt(nbOccurence[index1]+nbOccurence[index2]);
  //   double k=7.+GoTools::min(0.2*sum/(sqrt(nbOccurence[index1]+1)),30.);

  //   GoTools::print("k %i->%i = %f (%f, %f)\n", index1, index2, k, nbOccurence[index1], nbOccurence[index2]);

  //   return k;
}

long long MoGo::SmallSquarePatternsDB::getLocationPattern5x5( const Goban & goban, Location location ) const {/**assert(0);**/
  return goban.locationPattern5x5(location);
}

long long MoGo::SmallSquarePatternsDB::getLocationPattern5x5( const Goban & goban, Location location, const Vector< double > & /*distances*/ ) const {/**assert(0);**/
  long long representation=goban.locationPattern5x5(location)/*<<2LL*/;
  //    if (distances[location]==1.) representation+=0LL;
  //    else if (distances[location]==2.) representation+=1LL;
  //    else representation+=2LL;

  return representation;
}

void MoGo::SmallSquarePatternsDB::save( const char * name ) const {/**assert(0);**/
  FILE *f=fopen(name, "wb"); assert(f);
  int size=patternToIndex.size();
  fwrite(&size, sizeof(int), 1, f);
  GoTools::print("saving a %i elements database\n", size);
  for (MoGoHashMap(long long, int, hashLongLong, equalLongLong)::const_iterator it = patternToIndex.begin(); it != patternToIndex.end() ;++it) {/**assert(0);**/
    //     long long representation=it->first;
    int index=it->second;
    fwrite(&elo[index], sizeof(double), 1, f);
    assert(allRepresentations[index].size()==8);
    for (int i=0;i<(int)allRepresentations[index].size();i++)
      fwrite(&allRepresentations[index][i], sizeof(long long), 1, f);
  }
  fclose(f);
}

void MoGo::SmallSquarePatternsDB::load( const char *name, MoGoHashMap(long long, float, hashLongLong, equalLongLong) &patternElos ) {/**assert(0);**/
  patternElos.clear();
  FILE *f=fopen(name, "rb"); assert(f);
  int size=0;
  fread(&size, sizeof(int), 1, f);
  GoTools::print("loading a %i elements database\n", size);
  double max=-1e10; double min=1e10;
  for (int i=0;i<size;i++) {/**assert(0);**/
    long long representation=0;
    double elo=0.;
    fread(&elo, sizeof(double), 1, f);
    max=GoTools::max(max, elo);min=GoTools::min(min, elo);
    for (int j=0;j<8;j++) {/**assert(0);**/
      fread(&representation, sizeof(long long), 1, f);
      patternElos[representation]=elo;
    }
  }
  GoTools::print("max %f, min %f\n", max, min);
  fclose(f);
}

void MoGo::SmallSquarePatternsDB::insert( const Goban & goban, Location location, const Vector< double > & /*distances*/ ) {/**assert(0);**/
  Vector<long long> allRep;
  long long representation=goban.locationPattern5x5(location, allRep);
  if (patternToIndex.count(representation)==0) {/**assert(0);**/
    patternToIndex[representation]=nbOccurence.size();
    nbPlayed.push_back(0.);
    nbOccurence.push_back(0.);
    elo.push_back(1000.);
    allRepresentations.push_back(allRep);
    if (nbPlayed.size()%100000==0) {/**assert(0);**/ //FIXME
      GoTools::print("database size %i\n", nbPlayed.size());
    }
  }
}

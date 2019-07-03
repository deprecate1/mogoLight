//
// C++ Interface: smallsquarepatternsdb_mg
//
// Description:
//
//
// Author: Sylvain Gelly <sylvain.gelly@lri.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MOGOSMALLSQUAREPATTERNSDB_MG_H
#define MOGOSMALLSQUAREPATTERNSDB_MG_H

#include "typedefs_mg.h"

namespace MoGo {
  class Goban;
  /**
  A database containing small patterns, centered on the move to play. Hopefuly there are learned by professional games
   
  @author Sylvain Gelly
  */
  class SmallSquarePatternsDB {
  public:
    SmallSquarePatternsDB();

    ~SmallSquarePatternsDB();

    void addPattern(const Goban &goban, Location location, bool isPlayed);
    void addAllPatterns(const Goban &goban, Location played);
    double probabilityOfThisMove(const Goban &goban, Location location, const Vector<double> &distances);
    double getNbPlayed(const Goban &goban, Location location);
    double getNbOccurence(const Goban &goban, Location location);
    Location chooseMove(const Goban &goban);
    void save(const char *name) const;
    static void load(const char *name, MoGoHashMap(long long, float, hashLongLong, equalLongLong) &patternElos);
  private:
    void reduceDB();

    MoGoHashMap(long long, int, hashLongLong, equalLongLong) patternToIndex;
    static double winningProba(double elo1, double elo2);
    static double newElo(double elo1, double elo2, double k, bool win);
    void updateAllElos(const Goban &goban, Location played);
    void updateTwoElos(long long playedRepresentation, long long representation);
    long long getLocationPattern5x5(const Goban &goban, Location location) const;
    long long getLocationPattern5x5(const Goban &goban, Location location, const Vector<double> &distances) const;
    int getIndex(long long representation);
    void insert(const Goban &goban, Location location, const Vector<double> &distances);
    double getK(int index);
    double getK(int index1, int index2);
    Vector<double> nbPlayed;
    Vector<double> nbOccurence;
    Vector<double> elo;
    Vector<Vector<long long> > allRepresentations;
  };

}

#endif

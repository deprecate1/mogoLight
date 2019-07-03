//
// C++ Interface: localpatternsdatabase_mg
//
// Description:
//
//
// Author: Sylvain Gelly and Yizao Wang <sylvain.gelly@lri.fr and yizao.wang@polytechnique.edu>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MOGOLOCALPATTERNSDATABASE_MG_H
#define MOGOLOCALPATTERNSDATABASE_MG_H

#include "typedefs_mg.h"

namespace MoGo {

  /**
  Database representation of local patterns
   
  @author Sylvain Gelly
  */
  class LocalPatternsDatabase {
  public:
    LocalPatternsDatabase();
    /** A database constructor for the corners */
    LocalPatternsDatabase(int gobanSize, int cornerSize);

    ~LocalPatternsDatabase();

    Vector<int> getAllMovesInPosition(const Goban &goban, Vector<double> &movesImportance);
    Location getOneRandomMoveInPosition(const Goban &goban);
    void addMoveInPosition(const Goban &goban, Location move, double moveImportance);
    bool saveToFile(const char *name);
    static LocalPatternsDatabase *loadFromFile(const char *name);
    Vector<int> getRepresentation(const Goban &goban, int numPattern);
  private:
    int addMove(int index, int positionInPattern, double importance);
    void createLocationToVectors();
    int gobanSize;
    Vector<Vector<Location> >patternToLocation;
    Vector<Vector<Location> >patternToLocationInside;
    Vector<Vector<int> >locationToPatternNumber;
    Vector<Vector<int> >locationToPositionInPattern;
    MoGoHashMap(Vector<int>, int, hashVectorInt, equalVectorInt) allGobans;
    Vector<Vector<double> >movesImportance;
    Vector<Vector<Vector<int> > >moves;
  };

}

#endif

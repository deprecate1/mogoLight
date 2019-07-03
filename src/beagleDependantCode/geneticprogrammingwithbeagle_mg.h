//
// C++ Interface: geneticprogrammingwithbeagle_mg
//
// Description:
//
//
// Author: Yizao Wang and Sylvain Gelly <yizao.wang@polytechnique.edu and sylvain.gelly@lri.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MOGOGENETICPROGRAMMINGWITHBEAGLE_MG_H
#define MOGOGENETICPROGRAMMINGWITHBEAGLE_MG_H

#include "../geneticprogramming_mg.h"
namespace Beagle {
  namespace GP {
    class Individual;
  }
}

namespace MoGo {

  /**
  Genetic programming using Beagle to find good patterns
   
  @author Yizao Wang and Sylvain Gelly
  */
  class GeneticProgrammingWithBeagle {
  public:
    GeneticProgrammingWithBeagle();

    ~GeneticProgrammingWithBeagle();

  };
/*  class InterestingMoveFinderBeagle : public InterestingMoveFinder {
  public:
    InterestingMoveFinderBeagle(Beagle::GP::Individual *inIndividual);
    bool isInteresting(const Vector<int> &neighborhood) const;
  protected:
    Beagle::GP::Individual *inIndividual;
  };*/

}










#endif

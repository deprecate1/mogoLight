//
// C++ Interface: initiatefastboard_mg
//
// Description:
//
//
// Author: Sylvain Gelly <sylvain.gelly@lri.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MOGOINITIATEFASTBOARD_MG_H
#define MOGOINITIATEFASTBOARD_MG_H

namespace MoGo {

  /**
  Just a precomputed list of pattern values
   
  @author Sylvain Gelly
  */
  class InitiateFastBoard {
  public:
    InitiateFastBoard();

    ~InitiateFastBoard();
    static void initiatePrecomputedValues( int *precomputedValues, int *precomputedValues4,  int *simplePatternDatabase);
    static void initiatePrecomputedValuesb( int *precomputedValues, int *precomputedValues4,  int *simplePatternDatabase);
    static void initiatePrecomputedValues2( int *precomputedValues, int *precomputedValues4,  int *simplePatternDatabase);
    static void initiatePrecomputedValues2b( int *precomputedValues, int *precomputedValues4,  int *simplePatternDatabase);
    static void initiatePrecomputedValues3( int *precomputedValues, int *precomputedValues4,  int *simplePatternDatabase);
    static void initiatePrecomputedValues3b( int *precomputedValues, int *precomputedValues4,  int *simplePatternDatabase);
    static void initiatePrecomputedValues4( int *precomputedValues, int *precomputedValues4,  int *simplePatternDatabase);
    static void initiatePrecomputedValues4b( int *precomputedValues, int *precomputedValues4,  int *simplePatternDatabase);
    
  };

}

#endif

//
// C++ Interface: gobanurgency_mg
//
// Description:
//
//
// Author: Yizao Wang <yizao.wang@polytechnique.edu> and Sylvain Gelly <gelly@lri.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MOGOGOBANURGENCY_MG_H
#define MOGOGOBANURGENCY_MG_H

#include "typedefs_mg.h"
#include "fastsmallvector_mg.h"
#include "vectormemorypool_mg.h"
//#include "goban_mg.h"
//Normally the recent MoGo version does not demand the lineUrgency and totalUrgency.
//#define TOTAL_AND_LINE_URGENCY_DEMANDED

namespace MoGo {

  class Goban;
  /**
  This class deals with the urgency of each position of goban. In fact in UCT we don't have to keep the urgency table in the memory (which costs a lot!).
   
  @author Yizao Wang, Sylvain Gelly
  */
  class GobanUrgency {
    friend class Goban;
  public:
    GobanUrgency( int h, int w, double firstPlayUrgency = 10000 );
    ~GobanUrgency();

#ifdef TOTAL_AND_LINE_URGENCY_DEMANDED
    UrgencyType getTotalUrgency() const { return totalUrgency; } ;
    UrgencyType lineUrgency(int i) const { return urgencyByLine[i];};
#else
    UrgencyType lineUrgency(int ) const { return 0.;};
#endif
    void resetUrgency(double resetValue = 0.);
    inline void resetUrgency( const Location location, UrgencyType newUrgency ) {
      //if (location==PASSMOVE) return;
      if (location>=(int)urgency.size()) {
        fprintf(stderr, "location =%i, size =%i\n\n", location, urgency.size());
        assert(0);
      }
      assert(location>=0);
      //assert(location != PASSMOVE);
      if ( location == PASSMOVE ) {
#ifdef TOTAL_AND_LINE_URGENCY_DEMANDED
        totalUrgency-=urgency[PASSMOVE];
        totalUrgency+=newUrgency;
#endif
        urgency[PASSMOVE]=newUrgency;
        return;
      }
#ifdef TOTAL_AND_LINE_URGENCY_DEMANDED
      totalUrgency-=urgency[location];
      totalUrgency+=newUrgency;
      urgencyByLine[xIndex(location)]-=urgency[location];
      urgencyByLine[xIndex(location)]+=newUrgency;
#endif
      urgency[location]=newUrgency;
    }
    void clear();

    void textModeShowUrgency() const;

    inline const UrgencyType & operator[] (const int i) const {return urgency[i];};
    inline  UrgencyType & operator[] (const int i)  {return urgency[i];};
    //UrgencyType & operator[] (const int i) {return urgency[i];}; // DONT active this line.

    /** mainly we use the reset functions during the urgency update.*/
    void resetNeverPlayUrgency(Location location);
    void resetFirstPlayUrgency(Location location);
    void resetSecondPlayUrgency(Location location);
    /** set functions are only for the never/first/second urgencies (only three values!)*/
    void setNeverPlayUrgency(const double neverPlayUrgency);
    void setFirstPlayUrgency(const double firstPlayUrgency);
    void setSecondPlayUrgency(const double secondPlayUrgency);
    int possibleMoveNumber() const;
    bool isNeverPlayMove(const Location location) const { return urgency[location]==0.;};
    double highestUrgency() const ;
    bool isIntestingToContinue() const;
    inline int size() const {   return urgency.size(); }

    void textModeShowUrgencyRange(const Goban & goban) const ;
    /** change urgency distribution to bitmap */
    void changeToBitmap ( Bitmap & bitmap ) const;
    int indexOnArray( const int x, const int y ) const;

  private:
    VectorMemoryPool< UrgencyType > urgency;
    //      FastSmallVector< UrgencyType > urgency;
#ifdef TOTAL_AND_LINE_URGENCY_DEMANDED
    Vector< UrgencyType > urgencyByLine;
#endif
#ifdef TOTAL_AND_LINE_URGENCY_DEMANDED
    UrgencyType totalUrgency;
#endif
    //FIXME be CAREFUL here if the definition of location changed.
    //     int xIndex( const Location location ) const { return location/(width+2)-1; }
    ;//dangerous
    static UrgencyType neverPlayUrgency;
    static UrgencyType firstPlayUrgency;
    static UrgencyType secondPlayUrgency;
    int possibleMoveNumber_;


  };

}

#endif

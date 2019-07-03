//
// C++ Interface: cputimechronometer_mg
//
// Description:
//
//
// Author: Sylvain Gelly <sylvain.gelly@polytechnique.org>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MOGOCPUTIMECHRONOMETER_MG_H
#define MOGOCPUTIMECHRONOMETER_MG_H
#include <time.h>
// #include <datetimesgl.h>
#ifdef MPIMOGO
// #define QT_CHRONOMETER
#endif

namespace MoGo {

  /**
  A chronometer of cpu time
   
  @author Yizao Wang
  */
  class CPUTimeChronometer {
  public:
    CPUTimeChronometer();

    ~CPUTimeChronometer();
    double getTime() const ;
    void stop() ;
    void restart() ;
    void pause() ;
    void resume() ;

  private:
#ifndef QT_CHRONOMETER
#ifdef MPIMOGO
    double start, end;
    double durationOfPauses ;
    double dateOfPause ;
#else
    clock_t start, end;
    clock_t durationOfPauses ;
    clock_t dateOfPause ;
#endif

#else
    DateTime start, end;
    double durationOfPauses ;
    DateTime dateOfPause ;
#endif

    int stateOfChronometer ;

  };

}

#endif

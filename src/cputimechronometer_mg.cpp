//
// C++ Implementation: cputimechronometer_mg
//
// Description:
//
//
// Author: Sylvain Gelly <sylvain.gelly@polytechnique.org>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "cputimechronometer_mg.h"

#include <stdio.h>
#include "typedefs_mg.h"

#ifdef MPIMOGO
#include <mpi.h>
#undef QT_CHRONOMETER //JBH : for this moment, I prefer avoid to use Qt with MPI.
#endif



namespace MoGo {




  CPUTimeChronometer::CPUTimeChronometer() {
    restart() ;
  }


  CPUTimeChronometer::~CPUTimeChronometer() {}



  double CPUTimeChronometer::getTime( ) const {
#ifndef QT_CHRONOMETER

#ifdef MPIMOGO
    //MPI_Wtime gives the time in seconds
    
    if (stateOfChronometer == 1)
      return ((double) (MPI_Wtime() - start-durationOfPauses));
    else
      return ((double) (end - start-durationOfPauses));
#else
    if (stateOfChronometer == 1)
      return ((double) (clock() - start-durationOfPauses)) / CLOCKS_PER_SEC;
    else
      return ((double) (end - start-durationOfPauses)) / CLOCKS_PER_SEC;
#endif

#else
    DateTime tmp ;
    if (stateOfChronometer == 1)
      tmp = QDateTime::currentDateTime() ;
    else
      tmp = end ;
    //   tmp = tmp- start-durationOfPauses ;
    // printf("for %i : %f/%f/%f\n", (int)this, tmp.toDouble(),start.toDouble(),durationOfPauses) ;
    return tmp.toDouble()-start.toDouble()-durationOfPauses ;
#endif
  }

  void CPUTimeChronometer::stop( ) {
    if (stateOfChronometer != 1)
      return ;
#ifndef QT_CHRONOMETER

#ifdef MPIMOGO
    end = MPI_Wtime();
#else
    end = clock() ;
#endif


#else
    end = QDateTime::currentDateTime() ;
#endif
    stateOfChronometer = 0 ;
  }

  void CPUTimeChronometer::restart( ) {
#ifndef QT_CHRONOMETER
    
#ifdef MPIMOGO
    start = MPI_Wtime();
#else
    start = clock();
#endif
    
    end = -1 ;
    durationOfPauses = 0 ;
#else
    start = QDateTime::currentDateTime() ;
    end = DateTime() ;
    durationOfPauses = 0. ;
#endif
    stateOfChronometer = 1 ;
  }

  void CPUTimeChronometer::pause( ) {
    if (stateOfChronometer != 1)
      return ;
    stop() ;
#ifndef QT_CHRONOMETER
  
#ifdef MPIMOGO
    dateOfPause = MPI_Wtime();
#else
    dateOfPause = clock() ;
#endif

#else
    dateOfPause = QDateTime::currentDateTime() ;
#endif
    // printf("pause (%i) (%f)\n", (int)this, dateOfPause.toDouble()) ;
    stateOfChronometer = 2 ;
  }

  void CPUTimeChronometer::resume( ) {
    if (stateOfChronometer == 1)
      return ;
    end = -1 ;
    stateOfChronometer = 1 ;
#ifndef QT_CHRONOMETER

#ifdef MPIMOGO
    durationOfPauses += MPI_Wtime()-dateOfPause ;
#else
    durationOfPauses += clock()-dateOfPause ;
#endif

#else
    durationOfPauses += (DateTime(QDateTime::currentDateTime())-dateOfPause).toDouble() ;
#endif
    // printf("resume (%i) (%f)-(%f) => (%f)\n", (int)this, DateTime(QDateTime::currentDateTime()).toDouble(), dateOfPause.toDouble(), durationOfPauses.toDouble()) ;
  }

}


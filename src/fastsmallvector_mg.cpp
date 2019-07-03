//
// C++ Implementation: fastsmallvector_mg
//
// Description:
//
//
// Author: Yizao Wang <yizao.wang@polytechnique.edu> and Sylvain Gelly <gelly@lri.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "fastsmallvector_mg.h"
#include <pthread.h>


#ifdef UseQuickMemoryManagementForFastVector
// int MoGo::QuickMemoryManagement::nbAllocations10=0;
// int MoGo::QuickMemoryManagement::nbAllocations100=0;
// int MoGo::QuickMemoryManagement::nbAllocations1000=0;
namespace MoGo {
#ifndef PARALLELIZED
  std::vector<QuickMemoryStack<int *> >MoGo::QuickMemoryManagement<int>::memory(1);
  bool MoGo::QuickMemoryManagement<int>::toInitQuickMemoryStack=false;
#else
  std::vector<std::vector<QuickMemoryStack<int *> > >MoGo::QuickMemoryManagement<int>::memory(NUM_THREADS_MAX); // We allow up to NUM_THREADS_MAX threads!
#ifdef UseQuickMemoryManagementForFastVector  
  int MoGo::ThreadsManagement::numberOfThreads[NUM_THREADS_MAX];
  int MoGo::ThreadsManagement::nbThreads=0;
  pthread_mutex_t MoGo::ThreadsManagement::threadsManagementMutex;
  bool MoGo::ThreadsManagement::threadsManagementValue=false;
#endif /* UseQuickMemM... */
  std::vector<bool> MoGo::QuickMemoryManagement<int>::toInitQuickMemoryStack(NUM_THREADS_MAX,false);
#endif /* notdef PARALLELIZED */
  //   template<> int *MoGo::QuickMemoryManagement<int>::memorySmall;
  //   bool MoGo::QuickMemoryManagement<int>::blockVariable2=false;
#ifdef MOGO_USE_MUTEX_FOR_MALLOC
  pthread_mutex_t MoGo::QuickMemoryManagement<int>::saferMutex;
#endif /* USE_MUTEX */

    /*
#ifdef PARALLELIZED
    ThreadsManagement::ThreadsManagement() {
        pthread_mutex_init(&threadsManagementMutex, NULL);
    }
    ThreadsManagement::~ThreadsManagement() {
        pthread_mutex_destroy(&threadsManagementMutex);
    }
#endif
    */
}
#endif

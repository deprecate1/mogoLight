#ifndef MOGOMUTEX_H
#define MOGOMUTEX_H

// Les locks
//
// En version non parallèle, on a une version inline qui ne fait rien
//
#include "typedefs_mg.h"

#ifdef PARALLELIZED
#  include <pthread.h>
#  define PAR_INLINE
#  define PAR_CODE ;
#  ifdef INSTR_LOCKS
#    include <stdint.h>
#  endif
#else
#  define PAR_INLINE inline
#  define PAR_CODE { return 0; }
#endif

class mogoMutex {
  public:
  	PAR_INLINE int init(void) PAR_CODE
  	PAR_INLINE int lock(long info, long info2) PAR_CODE
  	inline int lock(long info) { return lock(info, -1); }
  	inline int lock() { return lock(-1, -1); }
  	PAR_INLINE int trylock(long info, long info2) PAR_CODE
  	inline int trylock(long info) { return trylock(info, -1); }
  	inline int trylock() { return trylock(-1, -1); }
	PAR_INLINE int unlock(void) PAR_CODE
  private:
#ifdef PARALLELIZED
	pthread_mutex_t mutex;
#  ifdef INSTR_LOCKS
	uint64_t locked_date;
#  endif
#endif
};

void dump_traces(void);

#endif

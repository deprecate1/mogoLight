#ifndef TIMING_H
#define TIMING_H

#include <sys/types.h>
#include <sys/time.h>

#if defined(__i386) || defined(__x86_64)

typedef unsigned long long tick_t, *p_tick_t;

#ifdef __i386
#define GET_TICK(t) \
   __asm__ volatile("rdtsc" : "=A" (t))
#else
#define GET_TICK(t) do { \
     unsigned int __a,__d; \
     asm volatile("rdtsc" : "=a" (__a), "=d" (__d)); \
     (t) = ((unsigned long)__a) | (((unsigned long)__d)<<32); \
} while(0)
#endif
#define TICK_RAW_DIFF(t1, t2) \
   ((t2) - (t1))

#else

/* fall back to imprecise but portable way */
#define TIMING_GETTIMEOFDAY

typedef struct timeval tick_t, *p_tick_t;

#define GET_TICK(t) \
   gettimeofday(&(t), NULL)
#define TICK_RAW_DIFF(t1, t2) \
   ((t2.tv_sec * 1000000L + t2.tv_usec) - \
    (t1.tv_sec * 1000000L + t1.tv_usec))

#endif

#define TICK_DIFF(t1, t2) (TICK_RAW_DIFF(t1, t2) - residual)
#define TIMING_DELAY(t1, t2) tick2usec(TICK_DIFF(t1, t2))

extern unsigned long long residual;
extern tick_t         new_event;
extern tick_t         last_event;

char *tick2str(long long t);


#endif /* TIMING_H */

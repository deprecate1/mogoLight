
#include "mogomutex.h"

#ifdef PARALLELIZED

#ifdef INSTR_LOCKS

#include <stdio.h>
#include "timing.h"
#include <stdlib.h>

#define NB_INFOS 4096000
#define NB_MAX_THREADS 68

struct infos_thread {
	 unsigned long *events;
	 unsigned long used;
	 unsigned long skipped_events;
	 pthread_t tid;
};
typedef struct infos_thread *p_infos_thread;

static __thread p_infos_thread evs;

static p_infos_thread infos_threads[NB_MAX_THREADS];
static int nb_threads=0;

static inline unsigned long* getstampedbuffer(unsigned long code, 
             	                              int size)
{
#define EV_H_SZ (1+(sizeof(tick_t)+sizeof(long)-1)/sizeof(long))
	struct infos_thread *infos=evs;
	if (infos == NULL) {
		infos=new struct infos_thread;
		evs=infos;
		infos->events=new unsigned long[NB_INFOS];
		infos->used=0;
		infos->skipped_events=0;
		infos->tid=pthread_self();
		int id_thread=__sync_fetch_and_add(&nb_threads, 1);
		if (id_thread >= NB_MAX_THREADS) {
			fprintf(stderr,"Too many threads\n");
			exit(1);
		}
		infos_threads[id_thread]=infos;
	}
	unsigned long idx=__sync_fetch_and_add(&infos->used,
			(size+EV_H_SZ));
	if (idx>=NB_INFOS) {
		static __thread int first=1;
		if (first) {
			fprintf(stderr,"Too many events for thread %li:"
				" skipping the next ones (idx=%li)\n",
				pthread_self(), idx);
			first=0;
		}
		infos->skipped_events++;
		infos->used=NB_INFOS;
		return NULL;
	} else if (idx==0) {
	}

	unsigned long *slot=&(infos->events[idx]);
	slot[0]=code<<4 | size;
	GET_TICK(*(tick_t*)(slot+1));
        return slot+EV_H_SZ;
}

static inline void record_event3(unsigned long code,
		unsigned long i1,
		unsigned long i2,
		unsigned long i3) {
	unsigned long *buff=getstampedbuffer(code, 3);
	if (buff) {
		buff[0]=i1;
		buff[1]=i2;
		buff[2]=i3;
	}
}

static inline void record_event2(unsigned long code,
		unsigned long i1,
		unsigned long i2) {
	unsigned long *buff=getstampedbuffer(code, 2);
	if (buff) {
		buff[0]=i1;
		buff[1]=i2;
	}
}

static inline void record_event1(unsigned long code,
		unsigned long i1) {
	unsigned long *buff=getstampedbuffer(code, 1);
	if (buff) {
		buff[0]=i1;
	}
}

static inline void record_event0(unsigned long code) {
	unsigned long *buff=getstampedbuffer(code, 0);
}

#endif

int mogoMutex::init(void) {
	pthread_mutex_init(&mutex, NULL);
}

int mogoMutex::trylock(long info, long info2) {
	int ret;
	if((ret=pthread_mutex_trylock(&mutex)) == 0) {
#ifdef INSTR_LOCKS
		record_event3(0, (unsigned long)&mutex, info, info2);
		record_event1(1, 0);
#endif
	} else {
#ifdef INSTR_LOCKS
		record_event3(3, (unsigned long)&mutex, info, info2);
#endif
	}
	return ret;
}

int mogoMutex::lock(long info, long info2) {
	int ret;
#ifdef INSTR_LOCKS
	record_event3(0, (unsigned long)&mutex, info, info2);
	if(pthread_mutex_trylock(&mutex) == 0) {
		record_event1(1, 0);
		return 0;
	}
#endif
	ret=pthread_mutex_lock(&mutex);
#ifdef INSTR_LOCKS
	record_event1(1, 1);
#endif
	return ret;
}

int mogoMutex::unlock(void) {
	int ret;
#ifdef INSTR_LOCKS
	record_event0(2);
#endif
	ret=pthread_mutex_unlock(&mutex);
	return ret;
}

#ifdef INSTR_LOCKS
struct minf {
	unsigned long nb_wait;
	unsigned long nb_lock;
	unsigned long nb_badtry;
	tick_t wait;
	tick_t use;
};

#  define NB_KINDS 2
#  define NB_LEVELS 100
#endif

void dump_traces(void) {
#ifdef INSTR_LOCKS
	fprintf(stderr, "\nBEGIN TRACES STATS (%i threads)\n", nb_threads);
	for (int id_th=0; id_th<nb_threads; id_th++) {
		fprintf(stderr, "Thread [%i]: begin\n", id_th);
		struct infos_thread *infos=infos_threads[id_th];
		if (infos==NULL) {
			fprintf(stderr, "Thread [%i]: no info\n", id_th);
			continue;
		}
		fprintf(stderr, "Thread [%i] %li : %li/%li used\n", id_th, 
			infos->tid, infos->used, NB_INFOS);
		if (! infos->events) {
			continue;
		}
		unsigned long idx=0;
		unsigned long nb_events=0;
		unsigned long last_code=2;
		unsigned long kind, level;
		struct minf *cur_m=NULL;
		struct minf m_info[NB_KINDS+1][NB_LEVELS+1]={{0,0,0,0,0}, };
		tick_t start;
		tick_t end;

		while(idx < infos->used) {
			unsigned long *slot=&(infos->events[idx]);
			unsigned long code=slot[0]>>4;
			unsigned long size=slot[0]&0xF;
#if 1
			if (code < 3) {
				last_code++;
				last_code=last_code%3;
				if (code!= last_code) {
					fprintf(stderr, "Argh: code=%i, %i expected\n",
						code, last_code);
				}
			}
#endif
#if 0
			fprintf(stderr, "Ev[%i/%li]: %li/%li at %lli (",
				id_th, nb_events,code, last_code, *((p_tick_t)&slot[1]));
			for (unsigned long arg=0; arg<size; arg++) {
				fprintf(stderr, "%li, ", slot[arg+EV_H_SZ]);
			}
			fprintf(stderr, ")\n");
#endif
			switch (code) {
			case 0: // Début prise de lock
				kind= slot[EV_H_SZ+1];
				level= (kind<2)?slot[EV_H_SZ+2]:0;
				cur_m=& m_info[kind][level];
				cur_m->nb_lock++;
				start=*(p_tick_t)(&slot[1]);
				break;
			case 1: // Fin prise de lock
			    {
				unsigned long val=slot[EV_H_SZ+0];
				if (val) {
					cur_m->nb_wait++;
					end=*(p_tick_t)(&slot[1]);
					cur_m->wait += (end-start);
					start=end;
				}
				break;
			    }
			case 2:
				end=*(p_tick_t)(&slot[1]);
				cur_m->use += (end-start);
				cur_m=NULL;
				break;
			case 3:
				kind= slot[EV_H_SZ+1];
				level= (kind<2)?slot[EV_H_SZ+2]:0;
				cur_m=& m_info[kind][level];
				cur_m->nb_badtry++;
				break;
			}


			nb_events++;
			idx+=size+EV_H_SZ;
		}
		for (kind=0; kind<=NB_KINDS; kind++) {
			tick_t use=0, wait=0;
			unsigned long nb_lock=0, nb_wait=0, nb_badtry=0;
			for (level=0; level<=NB_LEVELS; level++) {
				cur_m=& m_info[kind][level];
				if (!cur_m->nb_lock) continue;
				nb_lock+=cur_m->nb_lock;
				nb_wait+=cur_m->nb_wait;
				nb_badtry+=cur_m->nb_badtry;
				use+=cur_m->use;
				wait+=cur_m->wait;
				fprintf(stderr, "mutex %li (level) %2li: "
					"lock: %6li (dont wait: %6li (%2li\%)), skipped=%6li"
					" tot_used=%02.2fms (around %li/lock) wait %02.2fms (around %li/wait) delta: %li\%\n"
					,
					kind, level, cur_m->nb_lock,
					cur_m->nb_wait, 
					cur_m->nb_wait*100/cur_m->nb_lock,
					cur_m->nb_badtry,
					cur_m->use/2200000.0,
					cur_m->use/cur_m->nb_lock,
					cur_m->wait/2200000.0,
					cur_m->nb_wait?cur_m->wait/cur_m->nb_wait:0,
					100*(cur_m->wait)/cur_m->use


					);
			}
			if (!nb_lock) continue;

			fprintf(stderr, "mutex %li (all level): "
				"lock: %6li (dont wait: %6li (%2li\%)), skipped=%6li"
				" tot_used=%02.2fms (around %li/lock) wait %02.2fms (around %li/wait) delta: %li\%\n"
				,
				kind, nb_lock,
				nb_wait, 
				nb_wait*100/nb_lock,
				nb_badtry,
				use/2200000.0,
				use/nb_lock,
				wait/2200000.0,
				nb_wait?wait/nb_wait:0,
				100*(wait)/use


				);
		}
		fprintf(stderr, "Thread [%i] %li : %li events (and %li ignored)\n",
			id_th, infos->tid, nb_events, infos->skipped_events);
	}
	fprintf(stderr, "END TRACES STATS\n");
	fflush(stderr);
	return ;

#endif
}
#endif

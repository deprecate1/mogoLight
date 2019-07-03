//
// C++ Interface: typedefs_mg
//
// Description:
//
//
// Author: Yizao Wang <yizao.wang@polytechnique.edu> and Sylvain Gelly <gelly@lri.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//

#define NODK
#ifndef TYPEDEFS_MG_H
#define TYPEDEFS_MG_H
#define ANTICOIN
#define NOPEDAGO
#include <list>
#include <iostream>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//int handicapLevel;

// do not uncomment the two following lines: this is now the job of the "make" to define these!
//#define MOGOMPI    // states that mogo is in mpi mode
//#define MPIMOGO    // the same, for convenience

/*
#ifdef MPIMOGO

#warning "c'est bon"
#include "mpi.h"
static int mylock(pthread_mutex_t *l, int LINE, char *FILE) {
  double t1 = MPI_Wtime(); 
  pthread_mutex_lock(l); 
  double t2 = MPI_Wtime();
  if( t2-t1 > 0.01 ) 
    printf("Ligne %d du fichier %s: lock tres long (%g)\n", LINE, FILE, t2-t1);
  return 0;
}

#define pthread_mutex_lock(l) mylock(l, __LINE__, __FILE__)
*/

#define MODE_PANIC
//#define MOGOTW    
// for the moment mogotw is weaker than mogo, be careful

//#define AUTOTUNE
//#define LEARNING
#ifdef LEARNING
#define FUSEKI_RATIO
#endif

#ifdef MPIMOGO
/*#undef SEEK_SET
#undef SEEK_CUR
#undef SEEK_END*/

#define MPI_INTENSIVE


#define DELTATIME_SHARING 1.5
#define DEPTH_SHARING 1     //3
#define PROBA_SHARING (0.05)
#define SUMTRICK
//#define CREATE_OPENINGS

#define MPI_TAG_GTP 1
#define MPI_TAG_GTP_PARAM 2
#define MPI_TAG_DECISION 3
#define MPI_TAG_ESTIMATE 4
#define MPI_TAG_GNUGO	5
#define MPI_TAG_SHARING 6
#define MPI_TAG_ENDTHINKING 7
#define MPI_TAG_PONDERING 8

#endif

#define WEIGHT_EXPERTISE 5

#define MAX_GTP_SIZE 200

#define NB_ITER_BEFORE_DISPLAY 10000
#define NB_ITER_BEFORE_ISINTERESTING 10000
#define NB_ITER_BEFORE_CHECKING_SLAVE 1

#define NORMAL_RANDOM_MODE 0
#define EATING_PREFERED_RANDOM_MODE_1 1


#define STUPID_CENTER_OPENING_MODE 1

#define MAX_DEPTH_TREE_AMAF (100)

#define LINUX
#ifdef MAC
#undef LINUX
#endif
#ifdef WINDOWS
#undef LINUX
#endif




#define NUMBER_OF_SIMULATION_DOUBLE

// #define PARALLELIZED
#define USE_MUTEX_PER_NODE
 #define USE_CHILD_ALLOCATED_VECTOR
// #define PROTECT_TREE_HASHTABLE_BY_MUTEX
// #define KEEP_STACK_MOVE_VECTOR_PER_NODE
//  #define KEEP_CAPTURE_MOVES_PER_NODE

//  #define KEEP_TRACK_OF_CAPTURES_IN_FAST_BOARD

// #define KEEP_STRINGS_ON_NODES
//  #define

// #define WITH_LIBEGO_TEST



#ifdef USE_SGL
#include <vectorgelly.h>
#else
#include <vector>
#define Vector std::vector
// #include "fastsmallvector_mg.h"
// #define Vector MoGo::FastSmallVector
#endif
typedef int Location;
typedef int StringNumber;
typedef int GroupNumber;
typedef int StringLibertyNumber;
typedef int StringLength;
typedef Location Move;
typedef int PositionState;
typedef int StoneColor;
typedef int GoGameInfo;
typedef int Mode;
typedef Vector<int> Bitmap;
typedef Vector<Location> GobanState;

#define GobanKey Vector< unsigned int >

#define FASTCONTENTION
#define ANTILOCK
#define JYMODIF_NEW
//#define BIZARMC

#ifdef FASTCONTENTION
#define UrgencyType float
#define NODE_VALUE_TYPE float
#else
#define UrgencyType double
#define NODE_VALUE_TYPE double
#endif

#define EMPTY 0
#define BLACK 1
#define WHITE 2
#define OUTSIDE 3
#define UNCERTAIN 4

#define PASSMOVE 0
#define RESIGN -1

#define NOTFOUND -1


#define GUILLAUME
//#define FUSEKI_RATIO
//#define GOLDENEYE
//#define GOLDENEYE_MC

//#define CRITICALITY
//#define CRITICALITY_MC


//#define TEST_RANDOM_PATTERN
//#define RANDOM_PATTERN

#ifdef TEST_RANDOM_PATTERN
#undef MODULE_RANDOM_PATTERN
#define MODULE_RANDOM_PATTERN
#endif

#ifdef RANDOM_PATTERN
#undef MODULE_RANDOM_PATTERN
#define MODULE_RANDOM_PATTERN
#endif



#define GSIZE (WIDTH+2)*(HEIGHT+2)
/*#define POS(i,j) i*(WIDTH+2)+j+1
#define POSI(loc) (loc-1)/(WIDTH+2)
#define POSJ(loc) (loc-1)%(WIDTH+2)*/
#define GAMELENGTHMAX GSIZE*2
/*
#define UP(loc) loc-WIDTH-2
#define DOWN(loc) loc+WIDTH+2
#define LEFT(loc) loc-1
#define RIGHT(loc) loc+1
*/
#define KOLISTLENGTH 10
#define LEGAL 0
#define ALREADYTAKEN -1
#define KOILL -2
#define SELFKILLMOVE -3
#define MOVECHANGED -4
#define DEADSTRINGERROR -5
#define EYEFILLMOVE -6
#define OKMOVE 0
#define A_S_USELESS -7
#define OUTGOBAN -8
#define SELFATARIMOVE -9

#define LEFT_ 0
#define UP_ 1
#define RIGHT_ 2
#define DOWN_ 3
#define RIGHTUP_ 4
#define LEFTUP_ 5
#define LEFTDOWN_ 6
#define RIGHTDOWN_ 7

#define NORTHWEST_ 0
#define NORTH_ 1
#define NORTHEAST_ 2
#define WEST_ 3
#define CENTER_ 4
#define EAST_ 5
#define SOUTHWEST_ 6
#define SOUTH_ 7
#define SOUTHEAST_ 8

double randDouble() ;



#include <ext/hash_map>
#ifdef INTEL_COMPILER
#define MoGoHashMap(a,b,c,d) std::hash_map<a,b,c>
typedef std::string String;
#define Cout std::cout
#define Cin std::cin
#else
typedef std::string String;
#define Cout std::cout
#define Cin std::cin
#define MoGoHashMap(a,b,c,d) __gnu_cxx::hash_map<a,b,c,d>
#endif





#ifndef INTEL_COMPILER
/** Used for the hash_map.*/
struct hashGobanKey {
  size_t operator()(const GobanKey& gobanKey) const {
    /*      int s=gobanKey.size();
          unsigned int n=0,N=(unsigned int)(1024*1024*1024)*(unsigned int)2;//each unsigned int 2^32 bits
          for (int i=1;i<s;i++)//the first array is of little interest.
            n+=gobanKey[i]%(N/((s-2)/2+1));*/
    unsigned int n = 0 ;
    for (int i=0;i<(int)gobanKey.size();i++)//the first array is of little interest.
      n^=gobanKey[i];
    return n;
  }
};
struct hashVectorInt {
  size_t operator()(const Vector<int>& v) const {
    /*      int s=gobanKey.size();
          unsigned int n=0,N=(unsigned int)(1024*1024*1024)*(unsigned int)2;//each unsigned int 2^32 bits
          for (int i=1;i<s;i++)//the first array is of little interest.
            n+=gobanKey[i]%(N/((s-2)/2+1));*/
    int n = 0 ;
    for (int i=0;i<(int)v.size();i++)//the first array is of little interest.
      n^=v[i];
    return n;
  }
};
struct hashInt {
  size_t operator()(const int &a) const {
    return a;
  }
};
struct hashLongLong {
  size_t operator()(const long long &a) const {
    return (size_t)a;
  }
};
template <class T> class hashPointer {
public:
  size_t operator()(T * const &a) const {
    return size_t(a);
  }
};
#else
using namespace std;
class hashGobanKey {
private:
  typedef GobanKey Key;
  hash_compare<unsigned int> THC;
public:
  static const size_t bucket_size = 4;
  static const size_t min_buckets = 8;
  size_t operator()(const Key & k) const {
    size_t h=0;
    for(Key::size_type i=0; i<k.size(); i++) h ^= THC(k[i]);
    return h;
  };
  bool operator( )(const Key & k1, const Key & k2) const {
    if(k1.size()!=k2.size()) return true;
    for(Key::size_type i=0; i<k1.size(); i++) {
      if(THC(k1[i], k2[i])) return true;
    }
    return false;
  };
};
class hashVectorInt {
private:
  typedef hashVectorInt Key;
  hash_compare<int> THC;
public:
  static const size_t bucket_size = 4;
  static const size_t min_buckets = 8;
  size_t operator()(const Key & k) const {
    int h=0;
    for(Key::size_type i=0; i<k.size(); i++) h ^= THC(k[i]);
    return h;
  };
  bool operator( )(const Key & k1, const Key & k2) const {
    if(k1.size()!=k2.size()) return true;
    for(Key::size_type i=0; i<k1.size(); i++) {
      if(THC(k1[i], k2[i])) return true;
    }
    return false;
  };
};
class hashInt {
public:
  static const size_t bucket_size = 4;
  static const size_t min_buckets = 8;
  size_t operator()(const int & k) const {
    return (size_t) k;
  };
  bool operator( )(const int & k1, const int & k2) const {
    return k1==k2;
  };
};
class hashLongLong {
public:
  static const size_t bucket_size = 4;
  static const size_t min_buckets = 8;
  size_t operator()(const long long & k) const {
    return (size_t) k;
  };
  bool operator( )(const long & k1, const long & k2) const {
    return k1==k2;
  };
};
#endif

struct equalGobanKey {
  bool operator()(const GobanKey& gobanKey1, const GobanKey& gobanKey2) const {
    if (gobanKey1.size()!=gobanKey2.size()) return false;
    for (int i=0;i<(int)gobanKey1.size();i++)
      if (gobanKey1[i]!=gobanKey2[i]) return false;
    return true;
  }
};
struct equalVectorInt {
  bool operator()(const Vector<int>& gobanKey1, const Vector<int>& gobanKey2) const {
    if (gobanKey1.size()!=gobanKey2.size()) return false;
    //     if (gobanKey1[0]!=gobanKey2[0]) return false; // WARNING we assume that the vector are of size > 0 to improve speed
    for (int i=0;i<(int)gobanKey1.size();i++)
      if (gobanKey1[i]!=gobanKey2[i]) return false;
    return true;
  }
};
struct equalInt {
  bool operator()(const int &a, const int &b) const {
    return a==b;
  }
};
struct equalLongLong {
  bool operator()(const long long &a, const long long &b) const {
    return a==b;
  }
};
template <class T> class equalPointer {
public:
  bool operator()(T *const &a, T *const&b) const {
    return a==b;
  }
};

















namespace MoGo {
  class Goban;
  class IntegerSet {
  public:
    void add(int p) {
      if (map.count(p) == 0)
        map[p] = 1;
    }
    void add(const IntegerSet &set) {
      for (MoGoHashMap(int, int, hashInt, equalInt)::const_iterator i = set.map.begin(); i != set.map.end() ; ++i)
        add(i->second);
    }
    void remove(int p) {
      map.erase(p);
    }
    void remove(const IntegerSet &set) {
      for (MoGoHashMap(int, int, hashInt, equalInt)::const_iterator i = set.map.begin(); i != set.map.end() ; ++i)
        remove(i->second);
    }
    void clear() {
      map.clear();
    }
    size_t size() const {
      return map.size();
    }
    bool contains(int v) const {
      return map.count(v)==1;
    }
  private:
    MoGoHashMap(int,int,hashInt, equalInt) map;
  };
}








#ifdef RL_SCORE

#else
class RlMogoWrapper {
public:
  RlMogoWrapper(const std::string& ,
                const std::vector<std::pair<std::string, std::string> > &) {}
  int Simulate(const Vector<int> &, int) {
    return 0;
  }
  int SelectMove(const std::vector<int>& , int ) {
    return 0;
  }
  float GetValue(const std::vector<int>& , int ) {
    return 0;
  }
  void NewGame(const std::vector<int>& , int ) {}
  void Play(int , int , int ) {}

  /** Get the value of the current position */
  float GetCurrentValue() {
    return 0.5;
  }
  void GetAllValues(int , std::vector<float>& ) {}

  /** Update weights */
  void Update(float , bool ) {}

  /** Reset weights */
  void Reset() {}
  void Think(const std::vector<int>& , int , int ) {}
  void Test() {}
  void Print() {}
  void Save(const std::string& ) {}
}
;



#endif

template <class T> void clearVector(Vector<T> &v) {
  std::vector<T>().swap(v);
}

template <class T> void concat(Vector<T> &v, const Vector<T> &v2) {
  // GoTools::print("concat : \n");
  //   for (int i=0;i<(int)v.size();i++)
  // GoTools::print("v1[%i]=%i\n",i,v[i]);
  //   for (int i=0;i<(int)v2.size();i++)
  // GoTools::print("v2[%i]=%i\n",i,v2[i]);
  int oldSize=v.size();
  v.resize(v.size()+v2.size());
  for (int i=0;i<(int)v2.size();i++)
    v[i+oldSize]=v2[i];
  //   for (int i=0;i<(int)v.size();i++)
  // GoTools::print("vfinal[%i]=%i\n",i,v[i]);
}
void printOnFile(const int &v, FILE *f);
void printOnFile(const double &v, FILE *f);
template <class T> void printOnFile(const Vector<T> &v, FILE *f) {
  fprintf(f, "%i ", v.size());
  for (int i=0;i<(int)v.size();i++) {
    printOnFile(v[i],f);
  }
}
void readOnFile(int &v, FILE *f) ;
void readOnFile(double &v, FILE *f) ;
template <class T> void readOnFile(Vector<T> &v, FILE *f) {
  v.clear();
  int tmp; fscanf(f, "%i ", &tmp); v.resize(tmp);
  for (int i=0;i<(int)v.size();i++) {
    readOnFile(v[i],f);
  }
}



#endif


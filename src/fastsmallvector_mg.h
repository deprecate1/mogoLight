//
// C++ Interface: fastsmallvector_mg
//
// Description:
//
//
// Author: Yizao Wang <yizao.wang@polytechnique.edu> and Sylvain Gelly <gelly@lri.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MOGOFASTSMALLVECTOR_MG_H
#define MOGOFASTSMALLVECTOR_MG_H


// #define MOGO_USE_MUTEX_FOR_MALLOC
#define UseQuickMemoryManagementForFastVector

// #include "typedefs_mg.h"
#include <vector>
#include <assert.h>
#include <string.h>

#ifndef NUM_THREADS_MAX
#define NUM_THREADS_MAX 17
#endif
#ifdef PARALLELIZED
#define PREALLOCATED_SIZE_TAB (25000000)
#else
#define PREALLOCATED_SIZE_TAB (START_SIZE_SMALL_VECTOR*STEP_SIZE_SMALL_VECTOR)
#endif

#ifdef PARALLELIZED
#include <pthread.h>
namespace MoGo {
  class ThreadsManagement {
  private:
    static int numberOfThreads[NUM_THREADS_MAX];
    static int nbThreads;
  public:
    static pthread_mutex_t threadsManagementMutex;
    static bool threadsManagementValue;
    static int getNumThread() {
      if (nbThreads==0) return 0;
      int numTh=(int)pthread_self();
      // printf("size = %i\n", numberOfThreads.size());
      for (int i=0;i<nbThreads;i++)
        if (numberOfThreads[i]==numTh)
          return i;
      assert(0);
      return -1;
    }
/*
    ThreadsManagement();
    ~ThreadsManagement();
*/
    static void clearNumThreads() {
      nbThreads=0;
      addThread();
    }
    static void addThread() {
      assert(nbThreads<NUM_THREADS_MAX);
      //       if (int(numberOfThreads.size())<=multiThreadsIdentifier) numberOfThreads.resize(multiThreadsIdentifier+1);
      //       numberOfThreads[multiThreadsIdentifier]=pthread_self();
      numberOfThreads[nbThreads]=(int)pthread_self();
      nbThreads++;
    }

  };
}
#endif


// #define PARALLELIZED_SECOND_MUTEX_WAIT

#ifndef UseQuickMemoryManagementForFastVector
namespace MoGo {

  /**
  Like a vector, but internally optimized for small sizes and often resizes
   
  @author Sylvain Gelly
  */
  template <class T, int maxSize=10> class FastSmallVector {
  public:
  inline FastSmallVector(int size=0) :v(maxSize>size?maxSize:size) {
      //       assert(size<maxSize);
      currentSize=size ;
    }
  inline FastSmallVector(int size, const T &value) :v(maxSize>size?maxSize:size, value) {
      //       assert(size<maxSize);
      currentSize=size ;
    }
    inline ~FastSmallVector() {}

    inline void resize(int s) {
      if (s > (int)v.size())
        v.resize(s);
      currentSize=s;
    }
    inline void resize(int s, const T &value) {
      if (s > (int)v.size())
        v.resize(s, value);
      currentSize=s;
    }
    inline int size() const {
      return currentSize;
    }
    inline void push_back(const T &t) {
      //       assert(currentSize < maxSize);
      //assert(currentSize<(int)v.size());
      if (currentSize < (int)v.size()) {
        v[currentSize++]=t;
      } else {
        currentSize++;
        v.push_back(t);
      }
    }
    inline void pop_back() {
      assert(currentSize>0);
      currentSize --;
    }
    inline void clear() {
      currentSize=0;
    }
    inline T& operator[](int i) {
      //       assert(i>=0);assert(i<(int)v.size());
      return v[i];
    }
    inline const T& operator[](int i) const {
      //       assert(i>=0);assert(i<(int)v.size());
      return v[i];
    }

  private:
    std::vector<T> v;
    int currentSize;
  };
}

#endif
namespace MoGo {
  template<class TYPE_OF_SFV> class QuickMemoryStack {
  public:
    QuickMemoryStack() {
      sizeM=0;
    }
    inline bool empty() const {
      return sizeM==0;
    }
    inline TYPE_OF_SFV top() {
      assert(sizeM<=(int)v.size());
      assert(sizeM>0);
      return v[sizeM-1];
    }
    inline void push(const TYPE_OF_SFV &t) {
      if (sizeM >= (int)v.size())
        v.push_back(t);
      else
        v[sizeM]=t;
      sizeM++;
    }
    inline void pop() {
      assert(sizeM>0);
      sizeM--;
    }
    inline int size() const {
      return sizeM;
    }
  private:
    int sizeM;
    std::vector<TYPE_OF_SFV> v;
  };
}

#ifdef UseQuickMemoryManagementForFastVector
#include <stack>


namespace MoGo {







#define START_SIZE_SMALL_VECTOR 10
#define STEP_SIZE_SMALL_VECTOR 5

#ifndef PARALLELIZED

  template<class TYPE_OF_SFV> class QuickMemoryManagement {
  public:
    static void init() {}
    static std::vector<QuickMemoryStack<TYPE_OF_SFV *> >memory; //static int nbAllocations10;
    static bool blockVariable;
    //     static pthread_mutex_t saferMutex;

    inline static TYPE_OF_SFV *quickMalloc(int &size) {
      if (memory.size() == 0)
        memory.push_back(QuickMemoryStack<TYPE_OF_SFV *>());

      TYPE_OF_SFV *tmp=0;
      int i=0;int sizeTmp=START_SIZE_SMALL_VECTOR;
      for ( ; sizeTmp  < size ; ) {
        sizeTmp *= STEP_SIZE_SMALL_VECTOR;
        i++;
        if (i >= (int)memory.size())
          memory.push_back(QuickMemoryStack<TYPE_OF_SFV *>());
      }
      size=sizeTmp;
      if (memory[i].empty()) {
        memory[i].push(new TYPE_OF_SFV[size]);
        //                    static int nbAllocations1000=0;nbAllocations1000++; if (nbAllocations1000 % 100 ==0) printf("nbAllocations1000 %i, %i\n", nbAllocations1000, sizeof(TYPE_OF_SFV));
      }
      tmp=memory[i].top();
      memory[i].pop();
      assert(tmp);
      return tmp;
    }
    inline static void free(TYPE_OF_SFV *p, int size) {
      int i=0;int sizeTmp=START_SIZE_SMALL_VECTOR;
      for ( ; (sizeTmp  < size) ; i++)
        sizeTmp *= STEP_SIZE_SMALL_VECTOR;
      assert(i < (int)memory.size());
      memory[i].push(p);
    }
  };
  template<class TYPE_OF_SFV> std::vector<QuickMemoryStack<TYPE_OF_SFV *> >MoGo::QuickMemoryManagement<TYPE_OF_SFV>::memory;
  template<class TYPE_OF_SFV> bool MoGo::QuickMemoryManagement<TYPE_OF_SFV>::blockVariable=false;
  //   template<class TYPE_OF_SFV> pthread_mutex_t MoGo::QuickMemoryManagement<TYPE_OF_SFV>::saferMutex;


  template<> class QuickMemoryManagement<int> {
    //     static pthread_mutex_t saferMutex;
  public:
    static bool init() {
      if (memory.size() != 2)
        memory.resize(2);
      int size=sizeof(int)*PREALLOCATED_SIZE_TAB;
      int *tmp=new int[size];
      for (int i=0;i<size;i+=sizeof(int)*START_SIZE_SMALL_VECTOR)
        memory[0].push(tmp+i);
      tmp=new int[size];
      for (int i=0;i<size;i+=sizeof(int)*START_SIZE_SMALL_VECTOR*STEP_SIZE_SMALL_VECTOR)
        memory[1].push(tmp+i);
      toInitQuickMemoryStack=true;
      //       blockVariable2=false;
      return true;
    }
    static std::vector<QuickMemoryStack<int *> >memory; //static int nbAllocations10;
    //     static QuickMemoryStack<int> memoryDec; //static int nbAllocations10;
    static bool toInitQuickMemoryStack; //static int nbAllocations10;
    //     static bool blockVariable2; //static int nbAllocations10;
    //     static bool blockVariable;
    //     static int *memorySmall; //static int nbAllocations10;
    inline static int *quickMalloc(int &size) {
      if (!toInitQuickMemoryStack)
        init();
      int *tmp=0;
      int i=0;int sizeTmp=START_SIZE_SMALL_VECTOR;
      for ( ; sizeTmp  < size ; ) {
        sizeTmp *= STEP_SIZE_SMALL_VECTOR;
        i++;
        if (i >= (int)memory.size())
          memory.push_back(QuickMemoryStack<int *>());
      }
      size=sizeTmp;
      if (memory[i].empty()) {
        memory[i].push(new int[size]);
        //           nbAllocations1000++; if (nbAllocations1000 % 100 ==0) printf("nbAllocations1000 %i, %i\n", nbAllocations1000, sizeof(TYPE_OF_SFV));
      }
      tmp=memory[i].top();
      memory[i].pop();
      assert(tmp);
      return tmp;
    }
    inline static void free(int *p, int size) {
      int i=0;int sizeTmp=START_SIZE_SMALL_VECTOR;
      for ( ; sizeTmp  < size ; i++)
        sizeTmp *= STEP_SIZE_SMALL_VECTOR;
      assert(i<(int)memory.size());
      memory[i].push(p);
    }
    /*    static void deleteHalfMemory() {
          for (int i=0;i<(int)memory.size();i++) {
            int s=(int)memory[i].size();
            for (int j=0;j<s/2;j++) {
              delete [] memory[i].top();
              memory[i].pop();
            }
          }
        }*/

  };









#else










  template<class TYPE_OF_SFV> class QuickMemoryManagement {
  public:
    static void init() {}
    static std::vector< std::vector<QuickMemoryStack<TYPE_OF_SFV *> > >memory; //static int nbAllocations10;
    static bool blockVariable;
#ifdef MOGO_USE_MUTEX_FOR_MALLOC
    static pthread_mutex_t saferMutex;
#endif

    inline static TYPE_OF_SFV *quickMalloc(int &size) {
#ifdef MOGO_USE_MUTEX_FOR_MALLOC
      pthread_mutex_lock(&saferMutex);
#endif
      int num=ThreadsManagement::getNumThread();
      //       while (blockVariable) pthread_yield(); blockVariable=true;
      // printf("num = %i\n", num);
      if (memory[num].size() == 0)
        memory[num].push_back(QuickMemoryStack<TYPE_OF_SFV *>());

      TYPE_OF_SFV *tmp=0;
      int i=0;int sizeTmp=START_SIZE_SMALL_VECTOR;
      for ( ; sizeTmp  < size ; ) {
        sizeTmp *= STEP_SIZE_SMALL_VECTOR;
        i++;
        if (i >= (int)memory[num].size())
          memory[num].push_back(QuickMemoryStack<TYPE_OF_SFV *>());
      }
      size=sizeTmp;
      if (memory[num][i].empty()) {
        memory[num][i].push(new TYPE_OF_SFV[size]);
        //           nbAllocations1000++; if (nbAllocations1000 % 100 ==0) printf("nbAllocations1000 %i, %i\n", nbAllocations1000, sizeof(TYPE_OF_SFV));
      }
      tmp=memory[num][i].top();
      memory[num][i].pop();
      assert(tmp);
      //       blockVariable=false;
#ifdef MOGO_USE_MUTEX_FOR_MALLOC
      pthread_mutex_unlock(&saferMutex);
#endif
      return tmp;
    }
    inline static void free(TYPE_OF_SFV *p, int size) {
#ifdef MOGO_USE_MUTEX_FOR_MALLOC
      pthread_mutex_lock(&saferMutex);
#endif
      int num=ThreadsManagement::getNumThread();
      //       while (blockVariable) pthread_yield(); blockVariable=true;
      int i=0;int sizeTmp=START_SIZE_SMALL_VECTOR;
      for ( ; (sizeTmp  < size) ; i++)
        sizeTmp *= STEP_SIZE_SMALL_VECTOR;
      assert(i < (int)memory[num].size());
      memory[num][i].push(p);
#ifdef MOGO_USE_MUTEX_FOR_MALLOC
      pthread_mutex_unlock(&saferMutex);
#endif
      //       blockVariable=false;
    }
  };
  template<class TYPE_OF_SFV> std::vector<std::vector<QuickMemoryStack<TYPE_OF_SFV *> > >MoGo::QuickMemoryManagement<TYPE_OF_SFV>::memory(NUM_THREADS_MAX);
  //   template<class TYPE_OF_SFV> bool MoGo::QuickMemoryManagement<TYPE_OF_SFV>::blockVariable=false;
#ifdef MOGO_USE_MUTEX_FOR_MALLOC
  template<class TYPE_OF_SFV> pthread_mutex_t MoGo::QuickMemoryManagement<TYPE_OF_SFV>::saferMutex;
#endif


  template<> class QuickMemoryManagement<int> {
#ifdef MOGO_USE_MUTEX_FOR_MALLOC
    static pthread_mutex_t saferMutex;
#endif
  public:
    static bool init(int num) {
      if (memory[num].size() != 2)
        memory[num].resize(2);
      int size=sizeof(int)*(PREALLOCATED_SIZE_TAB/1000+START_SIZE_SMALL_VECTOR*STEP_SIZE_SMALL_VECTOR)/*000*/;
      int *tmp=new int[size];
      for (int i=0;i<size;i+=sizeof(int)*START_SIZE_SMALL_VECTOR)
        memory[num][0].push(tmp+i);
      tmp=new int[size];
      for (int i=0;i<size;i+=sizeof(int)*START_SIZE_SMALL_VECTOR*STEP_SIZE_SMALL_VECTOR)
        memory[num][1].push(tmp+i);
      toInitQuickMemoryStack[num]=true;
      return true;
    }
    static std::vector<std::vector<QuickMemoryStack<int *> > >memory; //static int nbAllocations10;
    //     static QuickMemoryStack<int> memoryDec; //static int nbAllocations10;
    static std::vector<bool>  toInitQuickMemoryStack; //static int nbAllocations10;
    //     static bool blockVariable2; //static int nbAllocations10;
    //     static bool blockVariable;
    //     static int *memorySmall; //static int nbAllocations10;
    inline static int *quickMalloc(int &size) {
#ifdef MOGO_USE_MUTEX_FOR_MALLOC
      pthread_mutex_lock(&saferMutex);
#endif
      int num=ThreadsManagement::getNumThread();
      if (!toInitQuickMemoryStack[num])
        init(num);
      int *tmp=0;
      int i=0;int sizeTmp=START_SIZE_SMALL_VECTOR;
      for ( ; sizeTmp  < size ; ) {
        sizeTmp *= STEP_SIZE_SMALL_VECTOR;
        i++;
        if (i >= (int)memory[num].size())
          memory[num].push_back(QuickMemoryStack<int *>());
      }
      size=sizeTmp;
      if (memory[num][i].empty()) {
        memory[num][i].push(new int[size]);
        //           nbAllocations1000++; if (nbAllocations1000 % 100 ==0) printf("nbAllocations1000 %i, %i\n", nbAllocations1000, sizeof(TYPE_OF_SFV));
      }
      tmp=memory[num][i].top();
      memory[num][i].pop();
      assert(tmp);
#ifdef MOGO_USE_MUTEX_FOR_MALLOC
      pthread_mutex_unlock(&saferMutex);
#endif
      return tmp;
    }
    inline static void free(int *p, int size) {
#ifdef MOGO_USE_MUTEX_FOR_MALLOC
      pthread_mutex_lock(&saferMutex);
#endif
      int num=ThreadsManagement::getNumThread();
      int i=0;int sizeTmp=START_SIZE_SMALL_VECTOR;
      for ( ; sizeTmp  < size ; i++)
        sizeTmp *= STEP_SIZE_SMALL_VECTOR;
      assert(i<(int)memory[num].size());
      memory[num][i].push(p);
      //       blockVariable2=false;
#ifdef MOGO_USE_MUTEX_FOR_MALLOC
      pthread_mutex_unlock(&saferMutex);
#endif

    }
    /*    static void deleteHalfMemory() {
    for (int i=0;i<(int)memory.size();i++) {
    int s=(int)memory[i].size();
    for (int j=0;j<s/2;j++) {
    delete [] memory[i].top();
    memory[i].pop();
    }
    }
    }*/

  };
#endif

































template <class T> struct funda { static const bool isfunda = false; };
  template <> struct funda<int> { static const bool isfunda = true; };
  template <> struct funda<bool> { static const bool isfunda = true; };
  template <> struct funda<double> { static const bool isfunda = true; };
  template <> struct funda<unsigned int> { static const bool isfunda = true; };
  template <> struct funda<float> { static const bool isfunda = true; };
  template <class T> struct funda<T *> { static const bool isfunda = true; };

  template <class TYPE_OF_SFV, bool Fundamental = /*is_fundamental<TYPE_OF_SFV>()*/funda<TYPE_OF_SFV>::isfunda > class FastSmallVector {
  public:
    explicit inline FastSmallVector(int size=0) {
      //       assert(size<maxSize);
      currentSize=size ;
      allocatedSize=START_SIZE_SMALL_VECTOR;
      while (allocatedSize < currentSize)
        allocatedSize *= STEP_SIZE_SMALL_VECTOR;
      //       assert(allocatedSize <= 1000);
      v=QuickMemoryManagement<TYPE_OF_SFV>::quickMalloc(allocatedSize);
      //       TYPE_OF_SFV a; a++;
    }
    inline FastSmallVector(const FastSmallVector<TYPE_OF_SFV> &fsv) {
      this->currentSize=fsv.currentSize;
      this->allocatedSize=fsv.allocatedSize;
      this->v=QuickMemoryManagement<TYPE_OF_SFV>::quickMalloc(allocatedSize);
      //       memcpy(this->v, fsv.v, currentSize*sizeof(TYPE_OF_SFV));
      for (int i=0;i<currentSize;i++)
        this->v[i]=fsv.v[i];
    }
    inline FastSmallVector &operator = (const FastSmallVector<TYPE_OF_SFV> &fsv) {
      if (this == &fsv)
        return *this;
      if (this->allocatedSize < fsv.allocatedSize) {
        QuickMemoryManagement<TYPE_OF_SFV>::free(this->v, this->allocatedSize);
        this->allocatedSize=fsv.allocatedSize;
        this->v=QuickMemoryManagement<TYPE_OF_SFV>::quickMalloc(allocatedSize);
      }
      this->currentSize=fsv.currentSize;
      //       memcpy(this->v, fsv.v, currentSize*sizeof(TYPE_OF_SFV));
      for (int i=0;i<currentSize;i++)
        this->v[i]=fsv.v[i];
      return *this;
    }
    inline ~FastSmallVector() {
      QuickMemoryManagement<TYPE_OF_SFV>::free(v, allocatedSize);
    }

    inline void resize(int s) {
      while (s > allocatedSize)
        growInternal();
      currentSize=s;
    }
    inline void growInternal() {
      int oldAllocatedSize=allocatedSize;allocatedSize*=STEP_SIZE_SMALL_VECTOR;
      TYPE_OF_SFV *tmp=QuickMemoryManagement<TYPE_OF_SFV>::quickMalloc(allocatedSize);
      //       memcpy(tmp, v, currentSize*sizeof(TYPE_OF_SFV));
      for (int i=0;i<currentSize;i++)
        tmp[i]=v[i];
      QuickMemoryManagement<TYPE_OF_SFV>::free(v,oldAllocatedSize);
      v=tmp;
    }
    inline int size() const {
      return currentSize;
    }
    inline void push_back(const TYPE_OF_SFV &t) {
      //       assert(currentSize < maxSize);
      //assert(currentSize<(int)v.size());
      //       assert(currentSize<allocatedSize);
      if (currentSize<allocatedSize)
        v[currentSize++]=t;
      else {
        growInternal();
        v[currentSize++]=t;
      }
    }
    inline void pop_back() {
      assert(currentSize>0);
      currentSize --;
    }
    inline void clear() {
      currentSize=0;
    }
    inline TYPE_OF_SFV& operator[](int i) {
      //       assert(i>=0);assert(i<allocatedSize);
      return v[i];
    }
    inline const TYPE_OF_SFV& operator[](int i) const {
      //       assert(i>=0);assert(i<allocatedSize);
      return v[i];
    }

  private:
    //     Vector<T> v;
    TYPE_OF_SFV *v;
    int currentSize;
    int allocatedSize;
  };

  template <class T> class FastSmallVector<T, true> {
  public:
    explicit inline FastSmallVector(int size=0) {
      //       assert(size<maxSize);
      currentSize=size ;
      allocatedSize=START_SIZE_SMALL_VECTOR;
      while (allocatedSize < currentSize)
        allocatedSize *= STEP_SIZE_SMALL_VECTOR;
      //       assert(allocatedSize <= 1000);
      v=QuickMemoryManagement<T>::quickMalloc(allocatedSize);
    }
    explicit inline FastSmallVector(int size, const T&value) {
      //       assert(size<maxSize);
      currentSize=size ;
      allocatedSize=START_SIZE_SMALL_VECTOR;
      while (allocatedSize < currentSize)
        allocatedSize *= STEP_SIZE_SMALL_VECTOR;
      //       assert(allocatedSize <= 1000);
      v=QuickMemoryManagement<T>::quickMalloc(allocatedSize);
      for (int i=0;i<size;i++)
        v[i]=value;
    }
    inline FastSmallVector(const FastSmallVector<T, true> &fsv) {
      this->currentSize=fsv.currentSize;
      this->allocatedSize=fsv.allocatedSize;
      this->v=QuickMemoryManagement<T>::quickMalloc(allocatedSize);
      memcpy(this->v, fsv.v, currentSize*sizeof(T));
    }
    inline FastSmallVector &operator = (const FastSmallVector<T, true> &fsv) {
      if (this == &fsv)
        return *this;
      if (this->allocatedSize < fsv.allocatedSize) {
        QuickMemoryManagement<T>::free(this->v, this->allocatedSize);
        this->allocatedSize=fsv.allocatedSize;
        this->v=QuickMemoryManagement<T>::quickMalloc(allocatedSize);
      }
      this->currentSize=fsv.currentSize;
      memcpy(this->v, fsv.v, currentSize*sizeof(T));
      return *this;
    }
    inline ~FastSmallVector() {
      QuickMemoryManagement<T>::free(v, allocatedSize);
    }

    inline void resize(int s) {
      while (s > allocatedSize)
        growInternal();
      currentSize=s;
    }
    inline void resize(int s, const T &value) {
      while (s > allocatedSize)
        growInternal();
      for (int i=currentSize;i<s;i++)
        v[i]=value;
      currentSize=s;
    }
    inline void growInternal() {
      int oldAllocatedSize=allocatedSize;allocatedSize*=STEP_SIZE_SMALL_VECTOR;
      T *tmp=QuickMemoryManagement<T>::quickMalloc(allocatedSize);
      memcpy(tmp, v, currentSize*sizeof(T));
      QuickMemoryManagement<T>::free(v,oldAllocatedSize);
      v=tmp;
    }
    inline int size() const {
      return currentSize;
    }
    inline void push_back(const T &t) {
      //       assert(currentSize < maxSize);
      //assert(currentSize<(int)v.size());
      //       assert(currentSize<allocatedSize);
      if (currentSize<allocatedSize)
        v[currentSize++]=t;
      else {
        growInternal();
        v[currentSize++]=t;
      }
      /*    if (currentSize>=allocatedSize) {
              growInternal();
              assert(currentSize<allocatedSize);
            }
            v[currentSize++]=t;*/
    }
    inline void pop_back() {
      assert(currentSize>0);
      currentSize --;
    }
    inline void clear() {
      currentSize=0;
    }
    inline T& operator[](int i) {
      //       assert(i>=0);assert(i<allocatedSize);
      return v[i];
    }
    inline const T& operator[](int i) const {
      //       assert(i>=0);assert(i<allocatedSize);
      return v[i];
    }

  private:
    //     Vector<T> v;
    T *v;
    int currentSize;
    int allocatedSize;
  };


  template <> class FastSmallVector<FastSmallVector<int>, false > {
  public:
    explicit inline FastSmallVector(int size=0) {
      //       assert(size<maxSize);
      currentSize=size ;
      allocatedSize=START_SIZE_SMALL_VECTOR;
      while (allocatedSize < currentSize)
        allocatedSize *= STEP_SIZE_SMALL_VECTOR;
      //       assert(allocatedSize <= 1000);
      v=QuickMemoryManagement<FastSmallVector<int> >::quickMalloc(allocatedSize);
    }
    inline FastSmallVector(const FastSmallVector<FastSmallVector<int> > &fsv) {
      this->currentSize=fsv.currentSize;
      this->allocatedSize=fsv.allocatedSize;
      this->v=QuickMemoryManagement<FastSmallVector<int> >::quickMalloc(allocatedSize);
      for (int i=0;i<currentSize;i++)
        v[i]=fsv.v[i];
    }
    inline FastSmallVector &operator = (const FastSmallVector<FastSmallVector<int> > &fsv) {
      if (this->allocatedSize < fsv.allocatedSize) {
        QuickMemoryManagement<FastSmallVector<int> >::free(this->v, this->allocatedSize);
        this->allocatedSize=fsv.allocatedSize;
        this->v=QuickMemoryManagement<FastSmallVector<int> >::quickMalloc(allocatedSize);
      }
      this->currentSize=fsv.currentSize;
      //       memcpy(this->v, fsv.v, currentSize*sizeof(int));
      for (int i=0;i<currentSize;i++)
        v[i]=fsv.v[i];
      return *this;
    }
    inline ~FastSmallVector() {
      QuickMemoryManagement<FastSmallVector<int> >::free(v, allocatedSize);
    }

    inline void resize(int s) {
      while (s > allocatedSize)
        growInternal();
      currentSize=s;
    }
    inline void resize(int s, const FastSmallVector<int> &value) {
      while (s > allocatedSize)
        growInternal();
      for (int i=currentSize;i<s;i++)
        v[i]=value;
      currentSize=s;
    }
    inline void growInternal() {
      int oldAllocatedSize=allocatedSize;allocatedSize*=STEP_SIZE_SMALL_VECTOR;
      FastSmallVector<int> *tmp=QuickMemoryManagement<FastSmallVector<int> >::quickMalloc(allocatedSize);
      //       memcpy(tmp, v, currentSize*sizeof(int));
      for (int i=0;i<currentSize;i++)
        tmp[i]=v[i];
      QuickMemoryManagement<FastSmallVector<int> >::free(v,oldAllocatedSize);
      v=tmp;
    }
    inline int size() const {
      return currentSize;
    }
    inline void push_back(const FastSmallVector<int> &t) {
      //       assert(currentSize < maxSize);
      //assert(currentSize<(int)v.size());
      //       assert(currentSize<allocatedSize);
      if (currentSize>=allocatedSize) {
        growInternal();
        assert(currentSize<allocatedSize);
      }
      v[currentSize++]=t;
    }
    inline void pop_back() {
      assert(currentSize>0);
      currentSize --;
    }
    inline void clear() {
      currentSize=0;
    }
    inline FastSmallVector<int>& operator[](int i) {
      //       assert(i>=0);assert(i<allocatedSize);
      return v[i];
    }
    inline const FastSmallVector<int>& operator[](int i) const {
      //       assert(i>=0);assert(i<allocatedSize);
      return v[i];
    }

  private:
    //     Vector<T> v;
    FastSmallVector<int> *v;
    int currentSize;
    int allocatedSize;
  };



}


template <class T> void concat(MoGo::FastSmallVector<T> &v, const MoGo::FastSmallVector<T> &v2) {
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







#endif



#endif

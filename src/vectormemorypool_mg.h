//
// C++ Interface: vectormemorypool_mg
//
// Description:
//
//
// Author: Sylvain Gelly and Yizao Wang <sylvain.gelly@lri.fr and yizao.wang@polytechnique.edu>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MOGOVECTORMEMORYPOOL_MG_H
#define MOGOVECTORMEMORYPOOL_MG_H

#include "fastsmallvector_mg.h"

#include <stack>

#ifdef PARALLELIZED
#include <pthread.h>
#endif



namespace MoGo {
  template<class TYPE_OF_SFV> class QuickMemoryManagementAllSizes {
  public:
    static std::vector<QuickMemoryStack<TYPE_OF_SFV *> >memory; //static int nbAllocations10;
    static pthread_mutex_t saferMutex;

    inline static TYPE_OF_SFV *quickMalloc(const int &size) {
      if (size==0)
        return 0;
#ifdef PARALLELIZED
      pthread_mutex_lock(&saferMutex);
#endif
      if (int(memory.size()) <= size)
        memory.resize(size+1,QuickMemoryStack<TYPE_OF_SFV *>());
      assert(int(memory.size())>size);
      TYPE_OF_SFV *tmp=0;
      if (memory[size].empty()) {
        memory[size].push(new TYPE_OF_SFV[size]);
        //           nbAllocations1000++; if (nbAllocations1000 % 100 ==0) printf("nbAllocations1000 %i, %i\n", nbAllocations1000, sizeof(TYPE_OF_SFV));
      }
      tmp=memory[size].top();
      memory[size].pop();
      assert(tmp);
      //       blockVariable=false;
#ifdef PARALLELIZED
      pthread_mutex_unlock(&saferMutex);
#endif
      return tmp;
    }
    inline static void free(TYPE_OF_SFV *p, int size) {
      if (p==0)
        return;
#ifdef PARALLELIZED
      pthread_mutex_lock(&saferMutex);
#endif
      memory[size].push(p);
#ifdef PARALLELIZED
      pthread_mutex_unlock(&saferMutex);
#endif
      //       blockVariable=false;
    }
  };
  template<class TYPE_OF_SFV> std::vector<QuickMemoryStack<TYPE_OF_SFV *> > MoGo::QuickMemoryManagementAllSizes<TYPE_OF_SFV>::memory;
  //   template<class TYPE_OF_SFV> bool MoGo::QuickMemoryManagement<TYPE_OF_SFV>::blockVariable=false;
  template<class TYPE_OF_SFV> pthread_mutex_t MoGo::QuickMemoryManagementAllSizes<TYPE_OF_SFV>::saferMutex;



  template <class TYPE_OF_SFV/*, bool Fundamental = funda<TYPE_OF_SFV>::isfunda */> class VectorMemoryPool {
  public:
    explicit inline VectorMemoryPool(int size=0) {
      //       assert(size<maxSize);
      currentSize=size ;
      allocatedSize=size;
      v=QuickMemoryManagementAllSizes<TYPE_OF_SFV>::quickMalloc(currentSize);
    }
    inline VectorMemoryPool(const VectorMemoryPool<TYPE_OF_SFV> &fsv) {
      this->currentSize=fsv.currentSize;
      this->allocatedSize=fsv.allocatedSize;
      this->v=QuickMemoryManagementAllSizes<TYPE_OF_SFV>::quickMalloc(allocatedSize);
      //       memcpy(this->v, fsv.v, currentSize*sizeof(TYPE_OF_SFV));
      for (int i=0;i<currentSize;i++)
        this->v[i]=fsv.v[i];
    }
    inline VectorMemoryPool &operator = (const VectorMemoryPool<TYPE_OF_SFV> &fsv) {
      if (this == &fsv)
        return *this;
      if (this->allocatedSize != fsv.allocatedSize) {
        QuickMemoryManagementAllSizes<TYPE_OF_SFV>::free(this->v, this->allocatedSize);
        this->allocatedSize=fsv.allocatedSize;
        this->v=QuickMemoryManagementAllSizes<TYPE_OF_SFV>::quickMalloc(allocatedSize);
      }
      this->allocatedSize=fsv.allocatedSize;
      this->currentSize=fsv.currentSize;
      for (int i=0;i<currentSize;i++)
        this->v[i]=fsv.v[i];
      return *this;
    }
    inline ~VectorMemoryPool() {
      QuickMemoryManagementAllSizes<TYPE_OF_SFV>::free(v, allocatedSize);
    }

    inline void resize(int s, const TYPE_OF_SFV &val=TYPE_OF_SFV()) {
      if (s != allocatedSize) {
        TYPE_OF_SFV *tmp=QuickMemoryManagementAllSizes<TYPE_OF_SFV>::quickMalloc(s);
        if (s>currentSize) {
          for (int i=0;i<currentSize;i++)
            tmp[i]=v[i];
          for (int i=currentSize;i<s;i++)
            tmp[i]=val;
        } else
          for (int i=0;i<s;i++)
            tmp[i]=v[i];
        QuickMemoryManagementAllSizes<TYPE_OF_SFV>::free(v,allocatedSize);
        v=tmp;
      }
      currentSize=s;
      allocatedSize=s;
    }
    inline int size() const {
      return currentSize;
    }
    inline void clearInternal() {
      currentSize=0;
    }
    inline void push_back(const TYPE_OF_SFV &t) {
      assert(currentSize<allocatedSize);
      v[currentSize++]=t;
    }
    inline void push_backElementWithoutSize(const TYPE_OF_SFV &t) {
      assert(currentSize<allocatedSize);
      v[currentSize]=t;
    }
    inline void incrementSize() {
      currentSize++;
    }

    inline void pop_back() {
      assert(currentSize>0);
      currentSize --;
    }
    inline void clear() {
      QuickMemoryManagementAllSizes<TYPE_OF_SFV>::free(v,allocatedSize);
      v=0;
      currentSize=0;
      allocatedSize=0;
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
    TYPE_OF_SFV *v;
    int currentSize;
    int allocatedSize;
  };
}









#endif

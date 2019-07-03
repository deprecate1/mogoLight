//
// C++ Interface: gotools_mg
//
// Description:
//
//
// Author: Yizao Wang <yizao.wang@polytechnique.edu> and Sylvain Gelly <gelly@lri.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MOGOGOTOOLS_MG_H
#define MOGOGOTOOLS_MG_H
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <string>
#include <stdarg.h>
#include "typedefs_mg.h"
#include "fastsmallvector_mg.h"

#define STDERR_MODE (0)
#define FILE_MODE (1)
#define IGNORE_MODE (2)
#define STDOUT_MODE (3)
#define GOGUI_MODE (4)
/** STDERR_MODE + FILE_MODE */
#define LOG_MODE (5)


namespace MoGo {

  /**
  Gather usefull methods
   
  @author Sylvain Gelly
  */
  class GoTools {
  public:
    GoTools();

    ~GoTools();
    static void print(const char* msg, ...);
    static void printWithColorScaled(double nb,const char* msg, ...);
    static void print(const Vector<int> &v);
    static void print(const Vector<Vector<int> >&v);
    static void print(const Vector<double> &v);
    static void print(const Vector<float> &v);
    static void print(const Vector<unsigned int> &v);

    static void print(const Vector<std::string> &v);
    static std::string sprint(std::string &s, const char* msg, ...);
    static std::string toString(double a, bool smallPrecision=false);
    static std::string toString(int a);
    static Vector<std::string> split(const char *s, char sep);
    /** choose the mode for printing message. the "mode" argument can be :
       STDERR_MODE, FILE_MODE (output to file named "outputMoGo"), IGNORE_MODE, STDOUT_MODE, GOGUI_MODE
     */
    static void setOutputMode(int mode);
    static void changeFileName(const std::string &name, bool appendToFile=false);
    static void saveOutputMode();
    static void restoreOutputMode();
    template <typename T> static Vector<T> subVector( const Vector<T> & array, int start, int end ) {
      end=end<int(array.size())?end:array.size();
      Vector<T> res(end-start) ;
      for (int i = start ; i < end ; i++)
        res[i-start]=array[i] ;

      return res ;
    }
    /** convert the string s into a double. Return true iff success */
    static bool toDouble(const char *s, double &v);
    /** convert the string s into an int. Return true iff success */
    static bool toInt(const char *s, int &v);
    static double max(const Vector<double> &v, int *index=0);
    static double max(double a, double b) {
      if (a>b)
        return a;
      else
        return b;
    }
    static double min(double a, double b) {
      if (a>b)
        return b;
      else
        return a;
    }
    static int max(int a, int b) {
      if (a>b)
        return a;
      else
        return b;
    }
    static int min(int a, int b) {
      if (a>b)
        return b;
      else
        return a;
    }
    /** Return a random int in [0 and max-1] */
    static int mogoRand(const int &max);
    /** Return a random int */
    static int mogoRand();
    static int mogoRand2();
    /** Return a random int in [0 and max-1] */
    static int mogoRand2(const int &max);
    /** Initialise the random seed */
    static void mogoSRand(int seed);
    /** Initialise the random seed using time*/
    static void mogoSRand();
    static void setFastRandMode(bool fastRand);
    static void displayWithColor(bool useColor);
    static const Vector<int> &oneRandom8Permutations();
    static double std( const Vector< double > & x );
    static double sum( const Vector< double > & x );
    static Vector< double > product( const Vector< double > & x, const Vector< double > & y );
    static Vector< double > centered( const Vector< double > & x );
    static double mean( const Vector< double > & x );
    static double correlation( const Vector< double > & x, const Vector< double > & y );
    static double correlationDifferentSizes( const Vector< double > & x, const Vector< double > & y );
    static Vector< double > productDifferentSizes( const Vector< double > & x, const Vector< double > & y );
    static Vector<std::string> toVector(const std::string &s);
    template<class T> static void remove(FastSmallVector<T> &v, const T &e) {
      int foundIndex=-1;
      for (int i=0;i<(int)v.size();i++)
        if (v[i]==e) {
          foundIndex=i;
          break;
        }
      if (foundIndex>=0) {
        v[foundIndex]=v[v.size()-1];
        v.pop_back();
      }
    }
    template<class T> static int find(FastSmallVector<T> &v, const T &e) {
      int foundIndex=-1;
      for (int i=0;i<(int)v.size();i++)
        if (v[i]==e) {
          foundIndex=i;
          break;
        }
      return foundIndex;
    }
    /** in MB */
    static double memoryUsed();
    /** in MB */
    static double additionalMemoryUsed();
    static void initMemoryUsed();
  private:
    static double startMemoryUsed;
    static int outputMode;
    static int savedOutputMode;
    static FILE *outputFile;
    static std::string fileName;
    static std::string savedFileName;
    static bool appendToFile;
    static bool savedAppendToFile;
    static int randDec[NUM_THREADS_MAX];
    static int iInRandArray[NUM_THREADS_MAX*1024];
    static bool fastRand;
    static bool useColor;
    static Vector<Vector<int> >all8Permutations;
    static void initializePermutations();
    static bool add1(Vector<int> &v, int base);
    static bool isPermutation(const Vector<int> &v);
  };

}

#endif

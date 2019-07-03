#ifndef GE_TOOLS_H
#define GE_TOOLS_H

#include <iostream>
using namespace std;

#include <list>
#include <vector>
#include "const_tools_ge.h"

class GE_Allocator
{
 public:
  GE_Allocator() {}
  ~GE_Allocator() {}
  
  static bool allocate(int***, int = -1, int = -1, int = -1, const list<int>* = 0);
  static bool desallocate(int***, int, int = -1, bool = true);
  static bool init(int***, int = -1, int = -1, int = 0, const list<int>* = 0);

  static bool allocate(float****, int, int, int);
  static bool desallocate(float****, int, int);
  static bool allocate(float***, int, int);
  static bool desallocate(float***, int);
  
};

class GE_ListOperators
{
 public:
  GE_ListOperators() {}
  ~GE_ListOperators() {}
  
  static bool is_in(const list<int>&, int);
  static bool push_if_not(list<int>&, int);
  static void erase(list<int>&, int);
  static bool unique(list<int>&);
  static bool merge(const list<int>&, list<int>&, bool = false);
  static bool intersection(const list<int>&, const list<int>&, list<int>&);
  static void sort(list<int>&);
  static int random(const list<int>&);
  static int random(list<int>&, list<int>::iterator&);
  static int select(list<int>&, int (&) (list<int>&, list<int>::iterator&), 
		    bool = true);
  static void print(const list<int>&);
  static void print(const list<int>&, const string (&) (const int));
  static void print(const list<int>&, const string (&) (const int, const int), const int);
  static void print_error(const list<int>&);
  static void print_error(const list<int>&, const string (&) (const int));
  static void print_error(const list<int>&, const string (&) (const int, const int), const int);


  static list<int> get_one_coordinate(const list<pair<int, int> >&, int = 0);
};



class GE_VectorOperators
{
 public:
  GE_VectorOperators() {}
  ~GE_VectorOperators() {}
  
  static bool is_in(const vector<int>&, int);
  static bool push_if_not(vector<int>&, int);
  static void intersection(const vector<int>&, const vector<int>&, 
			   vector<int>&);
  static int random(const vector<int>&);
  static int random(vector<int>&, int&);
  static int select(vector<int>&, int (&) (vector<int>&, int&), bool = true);
  static void print_error(const vector<int>&);
  static void print_error(const vector<int>&, 
			  const string (&)(const int, const int), 
			  const int); 
};



#endif

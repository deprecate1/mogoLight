#ifndef TOOLS_H
#define TOOLS_H

#include <iostream>
using namespace std;

#include <list>
#include "const_tools.h"

class Allocator
{
 public:
  Allocator() {}
  ~Allocator() {}
  
  static bool allocate(int***, int = -1, int = -1, int = -1, const list<int>* = 0);
  static bool desallocate(int***, int, int = -1, bool = true);
  static bool init(int***, int = -1, int = -1, int = 0, const list<int>* = 0);

  static bool allocate(float****, int, int, int);
  static bool desallocate(float****, int, int);
  static bool allocate(float***, int, int);
  static bool desallocate(float***, int);
  
};

class ListOperators
{
 public:
  ListOperators() {}
  ~ListOperators() {}
  
  static bool is_in(const list<int>&, int);
  static bool push_if_not(list<int>&, int);
  static void erase(list<int>&, int);
  static bool unique(list<int>&);
  static bool intersection(const list<int>&, const list<int>&, list<int>&);
  static void sort(list<int>&);
  static int random(const list<int>&);
  static int random(list<int>&, list<int>::iterator&);
  static int select(list<int>&, int (&) (list<int>&, list<int>::iterator&));
  static void print(const list<int>&);
  static void print(const list<int>&, const string (&) (const int));
  static void print(const list<int>&, const string (&) (const int, const int), const int);


  static list<int> get_one_coordinate(const list<pair<int,int> >&, int = 0);
};

#endif

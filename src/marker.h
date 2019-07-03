#ifndef GE_MARKER_H
#define GE_MARKER_H

#include <iostream>
using namespace std;

#include <vector>

#include <assert.h>

struct GE_Marker
{
  vector<int> seen;
  int num_treatment;
  int max_treatment;
  
  GE_Marker(int = 0);
  GE_Marker(const GE_Marker&);
  
  
  bool is_treated(int) const;
  int set_treated(int);
  int set_no_treated(int);
  void reinit(int = -1);
  void update_treatment();
  

  //WARNING WARNING WARNING
  //it's for an usage of memory. 
  //Here, it's not used as marker
  int memorize(int, int);
  int increment(int);
  int decrement(int);
  int get_memory(int) const;
  
};


#endif

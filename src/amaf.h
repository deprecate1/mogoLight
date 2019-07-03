#ifndef GE_AMAF_H
#define GE_AMAF_H

#include "tools_ge.h"
#include "const_goban.h"
#include "const_move.h"
#include "move.h"

struct GE_Amaf
{
  int* values;
  int* black_values;
  int* white_values;
  int* score;
  int* nb_simulations;
  int size;
  
  int* sum_places;
  
  GE_Amaf();
  GE_Amaf(int);
  ~GE_Amaf();
  
  void init_sum_places(const int); 
  
  void clear(int**);
  void clear();
  int get_max(const int*, bool = false) const;
  int print_place(const int*, int, int, team_color = GE_NEUTRAL_COLOR, bool = true, 
		  bool = true) const;
  void print_place(int, int) const;
  void preferred_moves(int*, int, list<int>* = 0, 
		       int = 10, bool = true) const;
  void print_preferred_moves(int, int = 10) const;
  int normalize(const int*, int) const;
};


#endif

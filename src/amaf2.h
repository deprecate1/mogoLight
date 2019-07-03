#ifndef GE_AMAF2_H
#define GE_AMAF2_H

#include <cmath>

#include "tools_ge.h"
#include "const_goban.h"
#include "const_move.h"
#include "move.h"

#include "game_go.h"

#define GE_DEFAULT_BEST_MAX_VALUE 100000000 //TODO in const_amaf2.h
#define GE_DEFAULT_BEST_MIN_VALUE -100000000 //TODO in const_amaf2.h

struct GE_Amaf2
{
  int* values;
  int* black_values;
  int* white_values;
  int* score;
  int* nb_simulations;
  int size;
  int* sum_places;
  
  int** sequences;
  int** black_sequences;
  int** white_sequences;
  //int** score_sequences;
  //int** simulations_sequences;
  int** places_sequences;
  
 

  GE_Amaf2();
  GE_Amaf2(int);
  ~GE_Amaf2();
  
  void init_sum_places(const int); 
  
  void clear(int**);
  void clear(int***);
  void clear();
  int get_max(const int*, bool = false) const;
  int get_max(const int**, bool = false) const;
  int print_place(const int*, int, int, team_color = GE_NEUTRAL_COLOR, bool = true, 
		  bool = true) const;
  void print_place(int, int) const;
  void preferred_moves(int*, int, list<int>* = 0, 
		       int = 10, bool = true) const;
  void print_preferred_moves(int, int = 10) const;
  int get_best_sequence(int**, list<int>* = 0, GE_Game* = 0, 
			bool = true, bool = true, int = GE_DEFAULT_SIZE_GOBAN) const;
  void print_best_sequence(GE_Game* = 0, int = GE_DEFAULT_SIZE_GOBAN, bool = true) const;
  int normalize(const int*, int) const;


  void update(int, list<int>&, int, team_color, int = 5);
};


#endif

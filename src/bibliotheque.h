#ifndef GE_BIBLIOTHEQUE_H
#define GE_BIBLIOTHEQUE_H

#include <iostream>
using namespace std;

#include <vector>

#include "move.h"
#include "const_geography.h"

struct GE_Directions
{
  static int board_area;
  static int board_size;
  static int big_board_area;
  static int big_board_size;
  static int directions[GE_NB_DIRECTIONS];
  static int limit_higher_location;
  
  
  static void init(int, int);
  static int to_move(int);
  static int to_big_move(int);
  static string to_move_string(int);
};


struct GE_Bibliotheque
{
  static vector<int> to_goban;
  static vector<int> to_big_goban;
  static vector<vector<int> > distance_pion;
  
  GE_Bibliotheque() {}
  ~GE_Bibliotheque() {}
  
  static void init(int, int);
  static void init_distance_pion();
  static bool is_in_border(int);  
};

#endif

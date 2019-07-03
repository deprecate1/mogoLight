#ifndef GE_ALGORITHM_H
#define GE_ALGORITHM_H

#include "tactic.h"
#include "strategy.h"
#include "game_go.h"
#include "const_tactic.h"

#include "location_information.h"

struct GE_Algorithm
{
  
  static void test(int = -1, bool = true, bool = true, 
		   bool = false);
  
  static bool solutions0(GE_Game&, list<int>&);
  static bool solutions1(GE_Game&, list<int>&);


  static bool can_be_useless(GE_Game&, int, int = GE_BLACK_STONE);
  static bool accept_move(GE_Game&, int, string* = 0);
  static void test_accept(int, bool = true);

  static int reflex_move(GE_Game&, GE_Informations* = 0);

};


#endif

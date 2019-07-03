#ifndef GE_MC_LOCATIONS_INFORMATIONS_H
#define GE_MC_LOCATIONS_INFORMATIONS_H

#include <cmath>
#include <assert.h>

#include "tools_ge.h"
#include "const_goban.h"
#include "const_move.h"
#include "move.h"

#include "game_go.h"
#include "goban.h"

struct GE_MCLocationsInformations
{
  int size;
  int color_win;
  int* black_times;
  int* black_wins;
  int* white_times;
  int* white_wins;
  int* empty_times;
  int* empty_wins;
  int nb_simulations;
  
  GE_MCLocationsInformations();
  GE_MCLocationsInformations(int, int = GE_BLACK);
  ~GE_MCLocationsInformations();
  
  void update(const GE_Goban&, team_color);

  double criterion_bw_on_bt(int);
  double criterion_bw_on_sim(int);
  double criterion_bt_on_sim(int);
  double criterion_ww_on_wt(int);
  double criterion_ww_on_sim(int);
  double criterion_wt_on_sim(int);
  double criterion_ew_on_et(int);
  double criterion_ew_on_sim(int);
  double criterion_et_on_sim(int);
  double criterion(int, int = 1);
  
  int pour_cent_mille(double = 1.f);

  int get_place(int, int, int&, GE_Game* = 0);
  void print_place_criteria(int, int, GE_Game* = 0, int = GE_DEFAULT_SIZE_GOBAN);
};


#endif

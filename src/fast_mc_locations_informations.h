#ifndef GE_FAST_MC_LOCATIONS_INFORMATIONS_H
#define GE_FAST_MC_LOCATIONS_INFORMATIONS_H

#include <iostream>
using namespace std;

#include <vector>

#include "fast_goban.h"
#include "game_go.h"
#include "referee.h"
#include "bibliotheque.h"

struct GE_FastMCLocationsInformations
{
  int color_win;
  vector<vector<int> > order;
  vector<vector<int> > order_before_enemy;
  
  vector<int> black_times;
  vector<int> black_wins;
  vector<int> white_times;
  vector<int> white_wins;
  vector<int> empty_times;
  vector<int> empty_wins;
  vector<int> black_frontier_times;
  vector<int> black_frontier_wins;
  vector<int> white_frontier_times;
  vector<int> white_frontier_wins;
  
  vector<int> values;
  vector<int> black_values;
  vector<int> white_values;
  vector<int> score;
  vector<int> nb_begins;
  
  int nb_simulations;
  int nb_wins;

  
  GE_FastMCLocationsInformations(int = GE_BLACK_STONE);
  ~GE_FastMCLocationsInformations() {}
  
  void clear(int = GE_BLACK_STONE);

  void update(const GE_FastGoban&, float);

  double criterion_bw_on_bt(int) const;
  double criterion_bw_on_sim(int) const;
  double criterion_bt_on_sim(int) const;
  double criterion_ww_on_wt(int) const;
  double criterion_ww_on_sim(int) const;
  double criterion_wt_on_sim(int) const;
  double criterion_ew_on_et(int) const;
  double criterion_ew_on_sim(int) const;
  double criterion_et_on_sim(int) const;
  double criterion_bfw_on_bft(int) const;
  double criterion_bfw_on_sim(int) const;
  double criterion_bft_on_sim(int) const;
  double criterion_wfw_on_wft(int) const;
  double criterion_wfw_on_sim(int) const;
  double criterion_wft_on_sim(int) const;
  double criterion_black_values(int) const;
  double criterion_white_values(int) const;
  double criterion_score(int) const;
  double criterion_mean_points(int) const;
  double criterion(int, int = 1) const;

  int pour_cent_mille(double = 1.f) const;
  
  int get_place(int, int, int&, GE_Game* = 0) const;
  void print_place_criteria(int, int, GE_Game* = 0, int = GE_DEFAULT_SIZE_GOBAN);
  void print_places(int, GE_Game* = 0, int = GE_DEFAULT_SIZE_GOBAN, int = GE_ILLEGAL_MOVE);

  void get_best_moves(list<int>&, int = 0, GE_Game* = 0, const GE_RefereeGo* = 0, bool = true) const;

  static void update_stats_semeai(const GE_FastGoban&, vector<vector<int> >&);
  static void update_stats_semeai(const GE_FastGoban&, const vector<int>&, 
				  const vector<int>&, vector<vector<int> >&);

  static void update_stats_group(const GE_FastGoban&, const GE_FastGoban&, 
				 vector<int>&);


  void print_stats(int = GE_ILLEGAL_MOVE) const;
  void print_one_stats(int big_location, int = -1) const;

  //GE_MCLocationsInformations();
  //GE_MCLocationsInformations(int, int = GE_BLACK);
  //~GE_MCLocationsInformations();
  
  //void update(const GE_Goban&, team_color);



  
  //double criterion_bw_on_bt(int);
  //double criterion_bw_on_sim(int);
  //double criterion_bt_on_sim(int);
  
  //double criterion(int, int = 1);
  
  //int pour_cent_mille(double = 1.f);

  //int get_place(int, int, int&, GE_Game* = 0);
  //void print_place_criteria(int, int, GE_Game* = 0, int = GE_DEFAULT_SIZE_GOBAN);

  
  double mean_of_neighbours(const vector<int>&, int) const;
};


#endif

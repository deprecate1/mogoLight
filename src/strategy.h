#ifndef GE_STRATEGY_H
#define GE_STRATEGY_H

#include "goban.h"
#include "tools_ge.h"
#include "const_goban.h"
#include "const_manager_game.h"
#include "param_strategy.h"
#include "game_go.h"
#include "database_games_go.h"
#include "file_sgf.h"
#include "filter.h"
#include "block.h"


struct GE_Morphology
{
  void dilatation(const pair<int, int>&, GE_Goban&, list<int>&);

  private:
  void extend(const int, const int, const int, GE_Goban&, list<int>&);
  
};

struct GE_Strategy
{
  void prediction_territories(const GE_Goban&, GE_Goban&) const;
  int get_area(const GE_Goban&, const team_color&, int* = 0, int* = 0, int* = 0) const;
  int resemblance(const GE_Goban&, const GE_Goban&) const;
  float resemblance(const GE_Goban&, const GE_Goban&, const GE_Filter&, int) const;
  int suffocation(const GE_Game&, int&) const;
  int greediness(const GE_Game&, const int, int* = 0) const;
  int survivor(const GE_Game&) const;
  int maximize_territory(GE_Game&, list<int>* = 0) const;
  int maximize_resemblance(GE_Game&, const GE_DbGamesGo&, list<int>* = 0) const;
  float maximize_resemblance(GE_Game&, const GE_DbGamesGo&, const GE_Filter&, list<int>*) const;
  int maximize_suffocation(GE_Game&, list<int>* = 0, const bool = true, int* = 0) const;
  int maximize_greediness(GE_Game&, list<int>* = 0, const bool = true, bool* = 0) const;
  int maximize_survivor(GE_Game& g, list<int>* = 0) const;
  
  bool is_stupid(GE_Game&, const int) const;
  bool update(const int, const int, int&, list<int>* = 0, GE_Game* = 0) const;
  bool update(const int, const float, float&, list<int>* = 0, GE_Game* = 0) const;


  static void wall0(GE_Game&, list<int>&);
  static void jam0(GE_Game&, list<int>&);
};

#endif

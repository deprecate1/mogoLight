#ifndef GE_MANAGER_MOVES_H
#define GE_MANAGER_MOVES_H

#include "tools_ge.h"
#include "param_manager_moves.h"
#include "const_goban.h"
#include "game_go.h"
#include "database_games_go.h"
#include "statistic.h"
#include "strategy.h"
#include <assert.h>

class GE_ManagerMoves
{
 public:
  int** eval_moves;
  int height;
  int width;
  
  GE_ManagerMoves(int = GE_DEFAULT_SIZE_GOBAN, int = GE_DEFAULT_SIZE_GOBAN);
  ~GE_ManagerMoves();
  
  void illegal_moves(GE_Game&);
  void avoid_border();
  void avoid_corner();
  void bonus_stat_moves(const GE_Game&, const GE_DbGamesGo&);
  void bonus_stat_distance(const GE_Game&, const GE_DbGamesGo&);
  void bonus_territories(GE_Game&);
  void eval(GE_Game&, const GE_DbGamesGo&);
  void print() const;
};

#endif

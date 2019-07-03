#ifndef GE_STATISTIC_H
#define GE_STATISTIC_H

#include "database_games_go.h"
#include "const_statistic.h"
#include "const_goban.h"
#include "move.h"
#include "goban.h"

#include <fstream>
#include <cmath>

struct GE_Statistic
{
  static void stat_moves(const GE_DbGamesGo&, float**** = 0, 
			 const bool = true, 
			 const int = GE_DEFAULT_SIZE_GOBAN, 
			 const int = GE_DEFAULT_SIZE_GOBAN);

  static void study_stat_moves(const GE_DbGamesGo&, const int = GE_DEFAULT_SIZE_GOBAN, 
			       const int = GE_DEFAULT_SIZE_GOBAN);
  static void stat_distances(const GE_DbGamesGo&, float*** = 0, 
			     const bool = true, 
			     const int = GE_DEFAULT_SIZE_GOBAN, 
			     const int = GE_DEFAULT_SIZE_GOBAN);

};

#endif

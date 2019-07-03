#ifndef GE_SUPER_MANAGER_GAME_H
#define GE_SUPER_MANAGER_GAME_H

#include "referee.h"
#include "manager_game.h"
#include "go_player.h"
#include "game_go.h"

class GE_SuperManagerGame
{
 public:
  GE_RefereeGo* referee;
  GE_ManagerGame* manager;
  GE_GoPlayer* black;
  GE_GoPlayer* white;
  GE_Game* game;
  
  GE_SuperManagerGame() {}
  GE_SuperManagerGame(GE_RefereeGo&, GE_ManagerGame&, GE_GoPlayer&, GE_GoPlayer&, GE_Game&);
  ~GE_SuperManagerGame() {}

  int ask_move() const;
  void update(const int, const bool = true);
  bool end_game(const bool = true) const;
  void manage(const int = 1, const bool = true);
};



#endif

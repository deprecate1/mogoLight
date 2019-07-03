#ifndef GE_MANAGER_GAME_H
#define GE_MANAGER_GAME_H

#include "const_manager_game.h"
#include "goban.h"
#include "chain.h"

class GE_ManagerGame
{
 public:

  GE_Goban* gob;
  team_color tc;
  int nb_black_stones_taken;
  int nb_white_stones_taken;
  pair<int, int> last_move;
  int num_move;

  GE_ManagerGame();
  GE_ManagerGame(GE_Goban&);
  GE_ManagerGame(GE_Goban&, const team_color&);
  ~GE_ManagerGame() {}
  
  void update(const int);
  void get_stones_taken(list<list<int> >&);

  void print_stones_taken() const;
  void print_last_move() const;
};


#endif

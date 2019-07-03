#ifndef GE_REFEREE_H
#define GE_REFEREE_H

#include "goban.h"
#include "manager_game.h"
#include "chain.h"


class GE_RefereeGo
{
 public: 
  GE_Goban last_goban;
  int before_last_move;
  int last_move;
  list<GE_Goban> positions;


  GE_RefereeGo();
  GE_RefereeGo(const GE_RefereeGo&);
  ~GE_RefereeGo() {}
  
  bool is_allowed(const GE_Goban&, const int, const team_color&) const;
  void update(GE_Goban&, const int);
  bool is_end_game() const;
  void clear();
  void undo(int);
};


#endif

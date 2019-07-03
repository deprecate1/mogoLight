#include "referee.h"


GE_RefereeGo::GE_RefereeGo() : before_last_move(GE_ILLEGAL_MOVE) , last_move(GE_ILLEGAL_MOVE) {}

GE_RefereeGo::GE_RefereeGo(const GE_RefereeGo& rg)
{
  last_goban = rg.last_goban;
  before_last_move = rg.before_last_move;
  last_move = rg.last_move;
  positions = rg.positions;
}

bool GE_RefereeGo::is_allowed(const GE_Goban& gob, const int mv, const team_color& col) const 
{
  pair<int, int> mv2 = int_to_pair(gob.height, mv);
  
  if(mv==GE_PASS_MOVE) return true;
  
  if(not GE_IS_IN_GOBAN(mv2.first, mv2.second, gob.height, gob.width))
    return false;
  
  if(gob.board[mv2.first][mv2.second]!=GE_WITHOUT_STONE)
    return false;
  
  GE_Goban copy_goban(gob);
  
  GE_ManagerGame mg(copy_goban, col);
  mg.update(mv);
  
  
  //ko
  if(copy_goban==last_goban)
    return false;

  //there is no chain without liberty (else it is an illegal move)
  GE_Chains ch(copy_goban);
  while(not ch.sets.empty())
    {
      if((ch.sets.front()).liberties==0)
	return false;
      ch.sets.pop_front();
    }

  //superko
  list<GE_Goban>::const_iterator i_g = positions.begin();

  while(i_g!=positions.end())
    {
      if(*i_g==copy_goban) return false;

      i_g++;
    }
  

  
  return true;
}


void GE_RefereeGo::update(GE_Goban& gob, int mv)
{
  last_goban = gob;
  before_last_move = last_move;
  last_move = mv;
  positions.push_back(gob);
}

bool GE_RefereeGo::is_end_game() const
{
  if(last_move==GE_RESIGN) return true;
  return ((before_last_move==GE_PASS_MOVE)&&(last_move==GE_PASS_MOVE));
}


void GE_RefereeGo::clear()
{
  before_last_move = GE_ILLEGAL_MOVE; 
  last_move = GE_ILLEGAL_MOVE;
  last_goban = GE_Goban();
  positions.clear();
}


void GE_RefereeGo::undo(int before_last_mv)
{
  last_move = before_last_move;
  before_last_move = before_last_mv;
  
  int height = positions.back().height;
  int width = positions.back().width;
  if(not positions.empty()) positions.pop_back();
  
  if(not positions.empty())
    last_goban = positions.back();
  else
    last_goban = GE_Goban(height, width);
}

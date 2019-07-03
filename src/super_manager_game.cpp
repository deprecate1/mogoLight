#include "super_manager_game.h"

GE_SuperManagerGame::GE_SuperManagerGame(GE_RefereeGo& rg, GE_ManagerGame& mg, 
				   GE_GoPlayer& b, GE_GoPlayer& w, GE_Game& g)
{
  referee = &rg;
  manager = &mg;
  black = &b;
  white = &w;
  game = &g;
}

int GE_SuperManagerGame::ask_move() const
{
  if(manager->tc==GE_WHITE)
    return white->get_move(*(manager->gob));
  return black->get_move(*(manager->gob));
}

void GE_SuperManagerGame::update(int mv, bool mode_print)
{
  referee->update(*(manager->gob), mv);
  if(mv==GE_RESIGN)
    {
      game->update(GE_PASS_MOVE);
      manager->update(GE_PASS_MOVE);
    }
  else
    {
      game->update(mv);
      manager->update(mv);
    }

  if(mode_print)
    {
      (manager->gob)->print(manager->last_move);
      manager->print_last_move();
      manager->print_stones_taken();
      cout<<endl;
    }
}

bool GE_SuperManagerGame::end_game(const bool mode_print) const
{
  if(referee->is_end_game())
    {
      if(mode_print)
	cout<<"end of the game"<<endl;
      return true;
    }
  
  return false;
} 

void GE_SuperManagerGame::manage(int sleeping, const bool mode_print)
{
  while(not this->end_game(mode_print))
    {
      int mv = this->ask_move();
      this->update(mv, mode_print);
      if(sleeping>0)
	sleep(sleeping);
    }
}



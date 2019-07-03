#include "manager_game.h"

GE_ManagerGame::GE_ManagerGame() : tc(GE_BLACK), nb_black_stones_taken(0), nb_white_stones_taken(0) , 
		num_move(0) 
{
  last_move = make_pair(GE_ILLEGAL_MOVE, GE_ILLEGAL_MOVE);
}

GE_ManagerGame::GE_ManagerGame(GE_Goban& goban)
{
  gob = &goban;
  tc = GE_BLACK;
  nb_black_stones_taken = 0;
  nb_white_stones_taken = 0;
  num_move = 0;
  last_move = make_pair(GE_ILLEGAL_MOVE, GE_ILLEGAL_MOVE);
}

GE_ManagerGame::GE_ManagerGame(GE_Goban& goban, const team_color& col)
{
  gob = &goban;
  tc = col;
  nb_black_stones_taken = 0;
  nb_white_stones_taken = 0;
  num_move = 0;
  last_move = make_pair(GE_ILLEGAL_MOVE, GE_ILLEGAL_MOVE);
}

//the legality of the movement is assumed.
void GE_ManagerGame::update(const int mv)
{
  gob->play(mv, tc);
  last_move = int_to_pair(gob->height, mv);
  num_move++;

  list<list<int> > stones_taken;
  this->get_stones_taken(stones_taken);
  gob->update(stones_taken);
  GE_NEXT_PLAYER(tc);
}

void GE_ManagerGame::get_stones_taken(list<list<int> >& stones_taken)
{
  GE_Chains ch(*gob);
  
  list<GE_Chain>::const_iterator i_c = ch.sets.begin();
  while(i_c!=ch.sets.end())
    {
      if(i_c->liberties==0)
	{
	  if(((tc==GE_WHITE)&&(i_c->stone==GE_BLACK_STONE))
	     ||((tc==GE_BLACK)&&(i_c->stone==GE_WHITE_STONE)))
	    {
	      stones_taken.push_back(i_c->links);
	      if(i_c->stone==GE_BLACK_STONE)
		nb_black_stones_taken += (i_c->links).size();
	      else nb_white_stones_taken += (i_c->links).size();
	    }
	}
      
      i_c++;
    }
}

void GE_ManagerGame::print_stones_taken() const
{
  cout<<"black stones taken: "<<nb_black_stones_taken<<endl;
  cout<<"white stones taken: "<<nb_white_stones_taken<<endl;
}

void GE_ManagerGame::print_last_move() const
{
  cout<<num_move<<". "<<move_to_string(last_move)<<endl;
}


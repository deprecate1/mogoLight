#ifndef GE_GAME_GO
#define GE_GAME_GO

#include <iostream>
using namespace std;

#include <list>
#include <fstream>

#include "goban.h"
#include "const_manager_game.h"
#include "chain.h"

struct GE_Game
{
  GE_Goban* goban;
  team_color tc;
  mutable list<int> game;
  list<list<int> > stones_taken;

  GE_Game() {}
  GE_Game(GE_Goban&);
  GE_Game(GE_Goban&, const team_color&);
  ~GE_Game() {}

  void init();

  bool stones_eaten(const int);
  bool real_play(int);
  bool play(const int);
  inline void play2(const int);
  void update(const int); //doesn't update the goban 
  void backward();
  bool is_allowed(const int);
  bool is_valid(const list<int>&);
  bool is_self_kill_group0(const int);
  bool is_stupid(const int) const;
  bool is_stupid3(const int) const;
  bool has_taken_ko();
  void clear();
  inline bool is_stupid2(const int) const;
  inline void get_last_stones_taken(list<int>&) const;
  inline int get_last_move() const;
  inline int get_before_last_move() const;

  
  void save(ofstream&) const;
  void print_error_goban() const;
};

void GE_Game::get_last_stones_taken(list<int>& last_eaten) const
{
  if(not stones_taken.empty())
    last_eaten = stones_taken.back();
}


void GE_Game::play2(const int mv)
{
  if(mv==GE_PASS_MOVE)
    {
      list<int> stone;
      stones_taken.push_back(stone);
      game.push_back(mv);
      GE_NEXT_PLAYER(tc);
      return;
    }
  
 
  goban->play(mv, tc);
  this->stones_eaten(mv);
  game.push_back(mv);
  goban->update(stones_taken.back());
  GE_NEXT_PLAYER(tc);
}

bool GE_Game::is_stupid2(const int stone) const
{
  if(stone==GE_PASS_MOVE) return false;
  
  int& height = goban->height;
  int& width = goban->width;
  pair<int, int> coord = int_to_pair(goban->height, stone);
  int nb_without_stone = 0;
  if(GE_IS_IN_GOBAN(coord.first+1, coord.second, height, width))
    {
      if(goban->board[coord.first+1][coord.second]==GE_WITHOUT_STONE)
	nb_without_stone++;
    }

  if(GE_IS_IN_GOBAN(coord.first-1, coord.second, height, width))
    {
      if(goban->board[coord.first-1][coord.second]==GE_WITHOUT_STONE)
	nb_without_stone++;
    }
  
  if(GE_IS_IN_GOBAN(coord.first, coord.second+1, height, width))
    {
      if(goban->board[coord.first][coord.second+1]==GE_WITHOUT_STONE)
	nb_without_stone++;
    }
  
  if(GE_IS_IN_GOBAN(coord.first, coord.second-1, height, width))
    {
      if(goban->board[coord.first][coord.second-1]==GE_WITHOUT_STONE)
	nb_without_stone++;
    }
  
  if(nb_without_stone>=2)
    return false;
  
  int color_eye = goban->eye(stone);
  if(color_eye!=GE_NEUTRAL_COLOR)
    {
      if(tc==color_eye)
	return true;
    }
  
  int nb_stones_taken = 0;
  
  goban->play(stone, tc);
  GE_Chains l_ch;
  l_ch.neighbour_chains(*goban, stone);
  
  int color_stone = goban->board[coord.first][coord.second];
  goban->back_play(stone);

  list<GE_Chain>::iterator i_c = l_ch.sets.begin();
  while(i_c!=l_ch.sets.end())
    {
      if(i_c->liberties==0)
	{
	  if(((tc==GE_WHITE)&&(i_c->stone==GE_BLACK_STONE))
	     ||((tc==GE_BLACK)&&(i_c->stone==GE_WHITE_STONE)))
	    {
	      nb_stones_taken += (i_c->links).size();
	      return false;
	      /* 
		 if we could take stones, 
		 then the move is not considered as stupid 
	      */
	    }
	}
      
      i_c++;
    }
  
  //remark: the value of nb_stones_taken is 0 because of "return false" 
  
  i_c = l_ch.sets.begin();
  while(i_c!=l_ch.sets.end())
    {
      if((i_c->links).empty())
	{
	  i_c++;
	  continue;
	}

      if(i_c->is_atari())
	{
	  pair<int, int> coord2 = int_to_pair(goban->height, (i_c->links).front());
	  

	  /*
	    //BUG
	    the back_play has been made; so the color of the stone is now 
	    without stone. (goban->board[coord2.first][coord2.second]==color_stone)
	    //CORRECTION
	    I add a test on the coordinate of the move (--> stone)
	    (i_c->links).front()==stone)
	  */
	  if(((i_c->links).front()==stone)
	     ||(goban->board[coord2.first][coord2.second]==color_stone))
	    {
	      
	      if((i_c->links).size()==1)
		{
		  //is it a sacrifice ?
		  if(l_ch.sets.size()==2)
		    {
		      if(i_c==l_ch.sets.begin())
			i_c++;
		      else i_c = l_ch.sets.begin(); //remember l_ch.sets.size()==2 !
		      
		      //Now, i_c points on the adversary chain
		      if(i_c->liberties==2)
			return false;
		      else
			{
			  if(nb_stones_taken < 1)
			    return true;
			  else return false;
			}		      
		    }
		}
	      
	      if((int)(i_c->links).size() > nb_stones_taken)
		return true;
	      else return false;
	    }
	}
      
      i_c++;
    }
  
  return false;
}


inline int GE_Game::get_last_move() const
{
  if(game.empty())
    return GE_ILLEGAL_MOVE;
  else return game.back();
}


inline int GE_Game::get_before_last_move() const
{
  if(game.size()<=1)
    return GE_ILLEGAL_MOVE;

  int last_move = game.back();
  game.pop_back();
  int before_last_move = game.back();
  game.push_back(last_move);
  return before_last_move;
}


 
#endif

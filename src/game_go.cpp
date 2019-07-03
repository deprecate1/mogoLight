#include "game_go.h"

GE_Game::GE_Game(GE_Goban& gob)
{
  goban = &gob;
  tc = GE_BLACK;
  this->init();
}

GE_Game::GE_Game(GE_Goban& gob, const team_color& col)
{
  goban = &gob;
  tc = col;
  this->init();
}

void GE_Game::init()
{
  //game.push_back(GE_PASS_MOVE);
  list<int> stones;
  stones_taken.push_back(stones);
}

bool GE_Game::stones_eaten(const int stone)
{
  bool temp_is_allowed = false;
  bool temp_is_not_allowed = false;
  
  list<int> stones;
  stones_taken.push_back(stones);
  
  GE_Chains ch;
  ch.neighbour_chains(*goban, stone);
 
  //ch.print();

  list<GE_Chain>::iterator i_c = ch.sets.begin();
  while(i_c!=ch.sets.end())
    {
      if(i_c->liberties==0)
	{
	  if(((tc==GE_WHITE)&&(i_c->stone==GE_BLACK_STONE))
	     ||((tc==GE_BLACK)&&(i_c->stone==GE_WHITE_STONE)))
	    {
	      (stones_taken.back()).splice((stones_taken.back()).begin(), 
					   i_c->links);
	      temp_is_allowed = true;
	    }
	  else temp_is_not_allowed = true;
	}
      i_c++;
    }
  
  if((not temp_is_allowed)&&(temp_is_not_allowed))
    {
      stones_taken.pop_back();
      return false;
    }
  return true;
}

bool GE_Game::real_play(const int mv)
{
  if(mv==GE_RESIGN)
    return play(GE_PASS_MOVE);
  
  return play(mv);
}


bool GE_Game::play(const int mv)
{
  if(mv==GE_PASS_MOVE)
    {
      list<int> stone;
      stones_taken.push_back(stone);
      game.push_back(mv);
      GE_NEXT_PLAYER(tc);
      return true;
    }
 
  pair<int, int> mv2 = int_to_pair(goban->height, mv);
  if((goban->board)[mv2.first][mv2.second]!=GE_WITHOUT_STONE)
    return false;
  
  //ko
  if((stones_taken.back().size()==1)
     &&(mv==(stones_taken.back()).back())
     &&(goban->is_alone(this->get_last_move())))
    return false;
  
  goban->play(mv, tc);
  if(not this->stones_eaten(mv))
    {
      //chain without liberty
      (goban->board)[mv2.first][mv2.second] = GE_WITHOUT_STONE;
      return false;
    }
  
  game.push_back(mv);
  goban->update(stones_taken.back());
  GE_NEXT_PLAYER(tc);
  return true;
}

void GE_Game::update(const int mv)
{
  if(mv==GE_PASS_MOVE)
    {
      list<int> stones;
      stones_taken.push_back(stones);
      game.push_back(mv);
      GE_NEXT_PLAYER(tc);
      return;
    }
  
  pair<int, int> mv2 = int_to_pair(goban->height, mv);
  goban->play(mv, tc);
  this->stones_eaten(mv);
  (goban->board)[mv2.first][mv2.second] = GE_WITHOUT_STONE;
  //it is not the role of this update. Use play!

  game.push_back(mv);
  GE_NEXT_PLAYER(tc);
}


void GE_Game::backward()
{
  pair<int, int> mv2 = int_to_pair(goban->height, game.back());
  game.pop_back();
  
  if(mv2.first!=GE_PASS_MOVE)
    (goban->board)[mv2.first][mv2.second] = GE_WITHOUT_STONE;
  
  int stone = ((tc==GE_WHITE) ? GE_WHITE_STONE : GE_BLACK_STONE);
  
  list<int>::const_iterator i_s = (stones_taken.back()).begin();
  while(i_s!=(stones_taken.back()).end())
    {
      mv2 = int_to_pair(goban->height, *i_s);
      (goban->board)[mv2.first][mv2.second] = stone;
      i_s++;
    }
  
  stones_taken.pop_back();
  GE_NEXT_PLAYER(tc);
}



bool GE_Game::is_allowed(const int mv)
{
  if(this->play(mv))
    {
      this->backward();
      return true;
    }

  return false;
}


bool GE_Game::is_valid(const list<int>& seq)
{
  bool is_correct = true;

  list<int>::const_iterator i_seq = seq.begin();

  int nb_backward = 0;

  while(i_seq!=seq.end())
    {
      if(this->play(*i_seq))
	nb_backward++;
      else is_correct = false;
      
      i_seq++;
    }
  
  while(nb_backward>0)
    {
      this->backward();
      nb_backward--;
    }

  return is_correct;
}

bool GE_Game::is_self_kill_group0(const int stone)
{
  if(stone<0) return false;

  int color_eye0 = goban->eye0(stone);
  if(color_eye0!=GE_NEUTRAL_COLOR)
    {
      if(color_eye0==tc)
	{
	  GE_Chains chs;
	  chs.neighbour_chains(*goban, stone);
	  
	  list<GE_Chain>::iterator i_ch = chs.sets.begin();
	  while(i_ch!=chs.sets.end())
	    {
	      if(i_ch->is_atari())
		break;
	      
	      i_ch++;
	    }
	  
	  if(i_ch==chs.sets.end())
	    {
	      this->play(stone);
	      GE_Chains chs2;
	      chs2.neighbour_chains(*goban, stone);
	      this->backward();
	      
	      list<GE_Chain>::iterator i_ch2 = chs2.sets.begin();
	      while(i_ch2!=chs2.sets.end())
		{
		  if(i_ch2->is_atari())
		    return true;
		  
		  i_ch2++;
		}
	    }
	}
    }
  
  return false;
}



bool GE_Game::is_stupid(const int stone) const
{
  if(stone==GE_PASS_MOVE) return false;

  //BE CAREFUL A threat of 2 ataris in the same time
  //TO IMPLEMENT in is_stupid2 ? 
  int color_eye0 = goban->eye0(stone);
  if(color_eye0!=GE_NEUTRAL_COLOR)
    {
      if(color_eye0==tc)
	{
	  GE_Chains chs;
	  chs.neighbour_chains(*goban, stone);
	  
	  list<GE_Chain>::iterator i_ch = chs.sets.begin();
	  while(i_ch!=chs.sets.end())
	    {
	      if(i_ch->is_atari())
		break;
	      
	      i_ch++;
	    }
	  
	  if(i_ch==chs.sets.end())
	    return true;
	}
    }
  

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
  
  pair<int, int> coord = int_to_pair(goban->height, stone);
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



bool GE_Game::is_stupid3(const int stone) const
{
  if(stone<=0) return false;


  int color_eye0 = goban->eye0(stone);
  if(color_eye0!=GE_NEUTRAL_COLOR)
    {
      if(color_eye0==tc)
	{
	  GE_Chains chs;
	  chs.neighbour_chains(*goban, stone);
	  
	  list<GE_Chain>::iterator i_ch = chs.sets.begin();
	  while(i_ch!=chs.sets.end())
	    {
	      if(i_ch->is_atari())
		break;
	      
	      i_ch++;
	    }
	  
	  if(i_ch==chs.sets.end())
	    return true;
	}
    }

  return false;
}


bool GE_Game::has_taken_ko()
{
  if(stones_taken.back().size()!=1) return false;
  
  if(not goban->is_alone(this->get_last_move())) return false;
  
  int ko_location = (stones_taken.back()).back();
  
  play(GE_PASS_MOVE);
  play(GE_PASS_MOVE);

  bool is_ko = is_allowed(ko_location);
  
  backward();
  backward();
  
  return is_ko;
}


void GE_Game::clear()
{
  goban->desallocate_goban();
  tc = GE_BLACK;
  game.clear();
  stones_taken.clear();
  this->init();
  //an attribute for superko ?
}

void GE_Game::save(ofstream& flux) const
{
  team_color temp_color = GE_BLACK;

  if(((tc==GE_BLACK)&&(((int)(game.size()))%2==1))
     ||((tc==GE_WHITE)&&(((int)(game.size()))%2==0)))
    temp_color = GE_WHITE;
     
  
  list<int>::const_iterator i_game = game.begin();

  while(i_game!=game.end())
    {
      
      if(temp_color==GE_BLACK)
	flux<<";B";
      else flux<<";W";
      
      flux<<"["<<move_to_string(GE_Directions::board_size, *i_game)<<"]";
      
      GE_NEXT_PLAYER(temp_color);
      
      i_game++;
    }
  
}


void GE_Game::print_error_goban() const
{
  goban->print_error(get_last_move()); 
}


// bool GE_Game::is_accepted(const int stone) 
// {
//   return not is_self_kill_group0(stone)

//     /*
//   goban->play(stone, tc);
//   GE_Chain ch;
//   ch.init(*goban, stone);
//   goban->back_play(stone);
  
//   return ((ch.size()<6)||(ch.liberties>1));
//     */
// }

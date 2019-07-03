#include "strategy.h"

void GE_Morphology::extend(const int i_stone, const int j_stone, 
			const int potential_stone, GE_Goban& gob, 
			list<int>& new_stones)
{
  if(GE_IS_IN_GOBAN(i_stone, j_stone, gob.height, gob.width))
    {
      if(gob.board[i_stone][j_stone]==GE_WITHOUT_STONE)
	{
	  gob.board[i_stone][j_stone] = potential_stone;
	  new_stones.push_back(pair_to_int(gob.height, make_pair(i_stone, j_stone)));
	}
    }
}

void GE_Morphology::dilatation(const pair<int, int>& stone,  
			    GE_Goban& gob, list<int>& new_stones)
{
  int i_stone = stone.first;
  int j_stone = stone.second;
  
  int potential_stone; 
  if((gob.board[i_stone][j_stone]==GE_WHITE_STONE)
     ||(gob.board[i_stone][j_stone]==GE_WHITE_STONE_POTENTIAL)
     ||(gob.board[i_stone][j_stone]==GE_STRONG_WHITE_STONE_POTENTIAL))
    potential_stone = GE_WHITE_STONE_POTENTIAL;
  else potential_stone = GE_BLACK_STONE_POTENTIAL;
  
  this->extend(i_stone-1, j_stone, potential_stone, gob, new_stones);
  this->extend(i_stone+1, j_stone, potential_stone, gob, new_stones);
  this->extend(i_stone, j_stone-1, potential_stone, gob, new_stones);
  this->extend(i_stone, j_stone+1, potential_stone, gob, new_stones);

  //GE_ListOperators lo; lo.print(new_stones); cout<<endl;
}


void GE_Strategy::prediction_territories(const GE_Goban& gob, GE_Goban& territories) const
{
  GE_ListOperators lo;

  territories = gob;

  GE_Goban black_goban(gob, GE_BLACK);
  GE_Goban white_goban(gob, GE_WHITE);
  GE_Morphology morph;
  list<int> white_stones;
  list<int> black_stones;
  
  for(int i = 0; i<gob.height; i++)
    for(int j = 0; j<gob.width; j++)
      {
	if(gob.board[i][j]==GE_WHITE_STONE)
	  white_stones.push_back(pair_to_int(gob.height, make_pair(i, j)));
	
	if(gob.board[i][j]==GE_BLACK_STONE)
	  black_stones.push_back(pair_to_int(gob.height, make_pair(i, j)));
      }
  
  while(true)
    {
      list<int> copy_black_stones(black_stones);
      black_stones.clear();
      
      while(not copy_black_stones.empty())
	{
	  morph.dilatation(int_to_pair(black_goban.height, copy_black_stones.front()), 
			   black_goban, black_stones);
	  copy_black_stones.pop_front();
	}
      
       //cout<<"GE_BLACK STONE "; lo.print(black_stone); cout<<endl;

      list<int> copy_white_stones(white_stones);
      white_stones.clear(); 
      
      while(not copy_white_stones.empty())
	{
	  morph.dilatation(int_to_pair(white_goban.height, copy_white_stones.front()), 
			   white_goban, white_stones);
	  copy_white_stones.pop_front();
	}
     
      //cout<<"GE_WHITE STONE "; lo.print(white_stones); cout<<endl;

      list<int>::iterator i_s = black_stones.begin();
      
      while(i_s!=black_stones.end())
	{
	  pair<int, int> stone2 = int_to_pair(territories.height, *i_s);

	  if((territories.board[stone2.first][stone2.second]==GE_WHITE_STONE)
	     ||(territories.board[stone2.first][stone2.second]==GE_BLACK_STONE))
	    {
	      i_s = black_stones.erase(i_s);
	      continue;
	    }


	  if(lo.is_in(white_stones, *i_s))
	    {
	      territories.board[stone2.first][stone2.second] = GE_NEUTRAL_STONE;
	      lo.erase(white_stones, *i_s);
	      i_s = black_stones.erase(i_s);
	      continue;
	    }
	  
	  if(territories.board[stone2.first][stone2.second]==GE_STRONG_WHITE_STONE_POTENTIAL)
	    {
	      territories.board[stone2.first][stone2.second] = GE_WHITE_STONE_POTENTIAL;
	      i_s = black_stones.erase(i_s);
	      continue;
	    }
  
	  territories.board[stone2.first][stone2.second] = GE_STRONG_BLACK_STONE_POTENTIAL;
	  i_s++;
	}
      
      i_s = white_stones.begin();
      
      while(i_s!=white_stones.end())
	{
	  pair<int, int> stone2 = int_to_pair(territories.height, *i_s);
	  
	  if((territories.board[stone2.first][stone2.second]==GE_WHITE_STONE)
	     ||(territories.board[stone2.first][stone2.second]==GE_BLACK_STONE))
	    {
	      i_s = black_stones.erase(i_s);
	      continue;
	    }
	  
	  if(territories.board[stone2.first][stone2.second]==GE_STRONG_BLACK_STONE_POTENTIAL)
	    {
	      territories.board[stone2.first][stone2.second] = GE_BLACK_STONE_POTENTIAL;
	      i_s = white_stones.erase(i_s);
	      continue;
	    }
	  
	  territories.board[stone2.first][stone2.second] = GE_STRONG_WHITE_STONE_POTENTIAL;
	  i_s++;
	}
      
      if((black_stones.empty())&&(white_stones.empty()))
	return;
    }
}

int GE_Strategy::get_area(const GE_Goban& gob, const team_color& col, 
		       int* ab, int* aw, int* diff) const
{
  int a_att = 0; 
  int a_def = 0; 
  
  GE_Goban territories;
  this->prediction_territories(gob, territories);
  
  for(int i = 0; i<territories.height; i++)
    for(int j = 0; j<territories.width; j++)
      {
	bool is_neutral = false;
	if(territories.is_color(col, i, j, is_neutral))
	  a_att++;
	else
	  {
	    if(not is_neutral)
	      a_def++;
	  }
      }
  
  if(diff)
    *diff = a_att - a_def;
  
  if(col==GE_WHITE)
    {
      if(ab) *ab = a_def;
      if(aw) *aw = a_att;
    }
  else
    {
      if(ab) *ab = a_att;
      if(aw) *aw = a_def;
    }
  
  return a_att;
}

int GE_Strategy::resemblance(const GE_Goban& gob1, const GE_Goban& gob2) const
{
  int degree = 0;
  
  for(int i = 0; i<gob1.height; i++)
    for(int j = 0; j<gob1.width; j++)
      {
	if(gob1.board[i][j]==gob2.board[i][j])
	  {
	    degree+=GE_PARAM_MAX_BONUS_RESEMBLANCE;
	    continue;
	  }
	
	if((gob1.board[i][j]==GE_WITHOUT_STONE)||(gob2.board[i][j]==GE_WITHOUT_STONE))
	  degree+=GE_PARAM_BONUS_RESEMBLANCE;
      }

  return degree;
}

float GE_Strategy::resemblance(const GE_Goban& gob1, const GE_Goban& gob2, 
			    const GE_Filter& filt, int mv) const
{
  if(mv==GE_PASS_MOVE) return 0.f;
  
  pair<int, int> mv2 = int_to_pair(gob1.height, mv);
  float degree = 0.f;
  
  for(int i = 0; i<gob1.height; i++)
    for(int j = 0; j<gob1.width; j++)
      {
	pair<int, int> coord = make_pair(i, j);

	if(mv2==coord) continue;
	
	pair<int, int> dist = distance_goban(coord, mv2);
	
	if(gob1.board[i][j]==gob2.board[i][j])
	  {
	    if(gob1.board[i][j]==GE_WITHOUT_STONE)
	      degree+=(filt.filter[dist.first][dist.second]*GE_PARAM_BONUS_RESEMBLANCE_EMPTY);
	    else
	      degree+=(filt.filter[dist.first][dist.second]*GE_PARAM_MAX_BONUS_RESEMBLANCE);
	    
	    continue;
	  }
	
	if((gob1.board[i][j]==GE_WITHOUT_STONE)||(gob2.board[i][j]==GE_WITHOUT_STONE))
	  degree+=(filt.filter[dist.first][dist.second]*GE_PARAM_BONUS_RESEMBLANCE);
      }
  
  return degree;
}


//global goban
int GE_Strategy::suffocation(const GE_Game& g, int& max_size) const
{
  int min_liberties = (g.goban)->size();
  
  GE_Chains ch(*(g.goban));
  
  list<GE_Chain>::const_iterator i_c = ch.sets.begin();
  while(i_c!=ch.sets.end())
    {
      int next_color = GE_NEXT_COLOR(g.tc);
      if(GE_IS_COLOR(i_c->stone, next_color)) 
	{
	  if(i_c->liberties==min_liberties)
	    {
	      if(max_size<(int)(i_c->links).size())
		max_size = (i_c->links).size();
	    }
	  
	  if(i_c->liberties<min_liberties)
	    {
	      max_size = (i_c->links).size();
	      min_liberties = i_c->liberties;
	    }
	}
      
      i_c++;
    }

  return min_liberties;
}

//global goban
//the stone is assumed virtually put down (the color has not been modified)
int GE_Strategy::greediness(const GE_Game& g, const int stone, int* threaten) const
{
  GE_Chains ch;
  ch.neighbour_chains(*(g.goban), stone); //local goban
  
  int val_potential = 0;
  ch.get_nb_dead(g.tc, &val_potential);

  if(val_potential>0)
    return val_potential;

  if(not threaten)
    return val_potential;

  int val_threaten = 0;
  ch.get_nb_atari(g.tc, &val_threaten);
  
  *threaten = val_threaten;

  return val_potential;
}

int GE_Strategy::survivor(const GE_Game& g) const
{
  GE_Blocks bls(*(g.goban));
  int next_color = GE_NEXT_COLOR(g.tc);
  
  while(true)
    {
      GE_Blocks temp_bls(&bls);
      bls = temp_bls;
      
      //CAREFULLY with another type of block: certainly, it is unapplicable
      if((bls.height==1)||(bls.width==1))
	{
	  if(not (bls.blocks).empty())
	    {
	      if(((bls.blocks).front()).color_stone==GE_WITHOUT_STONE)
		return 0; //unclear

	      if(GE_IS_COLOR(((bls.blocks).front()).color_stone, next_color))
		return 1; //victory
	      else return -1; //defeat
	    }
	  
	  break;
	}
    }
  
  return -1;
}


int GE_Strategy::maximize_territory(GE_Game& game, list<int>* p_l) const
{
  int max_area = -1;
  
  for(int mv = 0; mv<(game.goban)->size(); mv++)
    {
      if(not game.play(mv))
	continue;

      int next_color = (game.tc==GE_WHITE) ? GE_BLACK : GE_WHITE;
      int area = this->get_area(*(game.goban), next_color);
      game.backward();
   
      this->update(mv, area, max_area, p_l, &game);
    }

  return max_area;
}


int GE_Strategy::maximize_resemblance(GE_Game& g, const GE_DbGamesGo& dgg, 
				   list<int>* l_moves) const
{
  //amelioration: counting repeating game and take the maximum.

  int max_resemblance = 0;
  
  list<GE_FileSGF>::const_iterator i_sgf = dgg.games.begin();
  
  while(i_sgf!=dgg.games.end())
    {
      if((i_sgf->size_goban!=(g.goban)->height)
	 ||(i_sgf->size_goban!=(g.goban)->width))
	{
	  i_sgf++;
	  continue;
	}
      
      //i_sgf->print();
      list<int>::const_iterator i_g = (i_sgf->game).begin();
      
      GE_Goban gob((g.goban)->height, (g.goban)->width);
      GE_Game game(gob);
      
      while(i_g!=(i_sgf->game).end())
	{
	  //cout<<max_resemblance<<endl;
	
	  if(game.tc==g.tc)
	    {
	      int mv = *i_g;
	      if(g.is_allowed(mv))
		{
		  int res = this->resemblance(*(g.goban), *(game.goban));
		  this->update(mv, res, max_resemblance, l_moves, &g);
		}
	    }
	  
	  game.play(*i_g);
	  i_g++;
	}

      i_sgf++;
    }

  return max_resemblance;
}

float GE_Strategy::maximize_resemblance(GE_Game& g, const GE_DbGamesGo& dgg, 
				     const GE_Filter& filt, 
				     list<int>* l_moves) const
{
  //amelioration: counting repeating game and take the maximum.
  
  float max_resemblance = 0;
  
  list<GE_FileSGF>::const_iterator i_sgf = dgg.games.begin();
  
  while(i_sgf!=dgg.games.end())
    {
      //i_sgf->print();
      
      if((i_sgf->size_goban!=(g.goban)->height)
	 ||(i_sgf->size_goban!=(g.goban)->width))
	{
	  i_sgf++;
	  continue;
	}
      
      list<int>::const_iterator i_g = (i_sgf->game).begin();
      
      GE_Goban gob((g.goban)->height, (g.goban)->width);
      GE_Game game(gob);
      
      while(i_g!=(i_sgf->game).end())
	{
	  //cout<<max_resemblance<<endl;
	
	  if(game.tc==g.tc)
	    {
	      int mv = *i_g;
	      if(g.is_allowed(mv))
		{
		  float res = this->resemblance(*(g.goban), *(game.goban), filt, mv);
		  this->update(mv, res, max_resemblance, l_moves, &g);
		}
	    }
	  
	  game.play(*i_g);
	  i_g++;
	}
      
      i_sgf++;
    }

  return max_resemblance;
}



//return the size of the adversary chain that we hope to capture
//or the min of liberties
int GE_Strategy::maximize_suffocation(GE_Game& g, list<int>* l_moves, 
				   const bool is_advanced, 
				   int* min_free) const
{
  int min_liberties = (g.goban)->size();
  int max_size = 0;
  
  for(int mv = 0; mv<(g.goban)->size(); mv++)
    {
      if(not g.is_allowed(mv))
	continue;

      (g.goban)->play(mv, g.tc);
      int temp_max_size = 0;
      int temp_min_liberties = this->suffocation(g, temp_max_size);
      (g.goban)->back_play(mv);
      
      if(temp_min_liberties==min_liberties)
	{
	  if(is_advanced)
	    this->update(mv, temp_max_size, max_size, l_moves, &g);
	  else
	    {
	      if(l_moves)
		if(not this->is_stupid(g, mv))
		  l_moves->push_back(mv);
	    }
	  
	  continue;
	}
      
      if(temp_min_liberties<min_liberties)
	{
	  if(not this->is_stupid(g, mv))
	    {
	      min_liberties = temp_min_liberties;
	      max_size = temp_max_size;
	      if(l_moves)
		{
		  l_moves->clear();
		  l_moves->push_back(mv);
		}
	    }
	}
    }
  
  if(min_free)
    {
      *min_free = min_liberties;
      return max_size;
    }
  
  return min_liberties;
}


//return the maximum value of the captures.
//or the maximum value of threaten
int GE_Strategy::maximize_greediness(GE_Game& g, list<int>* l_moves, 
				  const bool is_advanced, 
				  bool* is_threaten) const
{
  if(is_threaten) *is_threaten = false; 
  //CAREFULLY: it is a pointer on a boolean (not a boolean)
  
  int max_taken = 0;
  int max_threaten = 0;
  
  list<int> l_taken;
  list<int> l_threaten;

  for(int mv = 0; mv<(g.goban)->size(); mv++)
    {
      if(not g.is_allowed(mv))
	continue;
      
      (g.goban)->play(mv, g.tc);
      int temp_max_threaten = 0;
      int temp_max_taken = this->greediness(g, mv, &temp_max_threaten);
      (g.goban)->back_play(mv);
      
      this->update(mv, temp_max_taken, max_taken, &l_taken, &g);
     
      if(is_advanced)
	this->update(mv, temp_max_threaten, max_threaten, &l_threaten, &g); 
    }
  
  if(is_advanced)
    {
      if(l_moves)
	{
	  if(GE_ListOperators::intersection(l_taken, l_threaten, *l_moves))
	    return max_taken;
	}
     
      if(not l_taken.empty())
	{
	  if(l_moves) *l_moves = l_taken;
	  return max_taken;
	}
      
      if(is_threaten) *is_threaten = true; //CAREFULLY between pointer and boolean      
      if(l_moves) *l_moves = l_threaten;
      return max_threaten;
    }
  
  if(l_moves) *l_moves = l_taken;
  return max_taken;
}


int GE_Strategy::maximize_survivor(GE_Game& g, list<int>* l_moves) const
{
  int max_victory = -2;
  
  for(int mv = 0; mv<(g.goban)->size(); mv++)
    {
      if(not g.play(mv))
	continue;
      
      int is_victory = this->survivor(g);
      g.backward();
      
      this->update(mv, is_victory, max_victory, l_moves, &g);
    }

  return max_victory;
}




bool GE_Strategy::is_stupid(GE_Game& g, const int mv) const
{
  return (g.is_stupid(mv));
}


bool GE_Strategy::update(const int mv, const int temp_val, 
		      int& maximum, list<int>* l_moves, 
		      GE_Game* g) const
{
  if(g)
    if(this->is_stupid(*g, mv))
      return false;
  
  if(temp_val==maximum)
    {
      if(l_moves)
	l_moves->push_back(mv);
      
      return true;
    }
  
  if(temp_val>maximum)
    {
      maximum = temp_val;
      if(l_moves)
	{
	  l_moves->clear();
	  l_moves->push_back(mv);
	}

      return true;
    }

  return false;
}


bool GE_Strategy::update(const int mv, const float temp_val, 
		      float& maximum, list<int>* l_moves, 
		      GE_Game* g) const
{
  if(g)
    if(this->is_stupid(*g, mv))
      return false;
  
  if(temp_val==maximum)
    {
      if(l_moves)
	l_moves->push_back(mv);
      
      return true;
    }
  
  if(temp_val>maximum)
    {
      maximum = temp_val;
      if(l_moves)
	{
	  l_moves->clear();
	  l_moves->push_back(mv);
	}

      return true;
    }

  return false;
}



void GE_Strategy::wall0(GE_Game& g, list<int>& l_moves)
{
  int last_move = g.get_last_move();
  int before_last_move = g.get_before_last_move();
  int& height = (g.goban)->height;
 

  if(before_last_move==GE_ILLEGAL_MOVE)
    return;

  pair<int, int> dist2 = distance_goban(last_move, before_last_move, height); 
  
  if((dist2.first!=1)||(dist2.second!=1))
    return;

  pair<int, int> lmv2 = int_to_pair(height, last_move);
  pair<int, int> blmv2 = int_to_pair(height, before_last_move);
  
  int rep1 = pair_to_int(height, make_pair(lmv2.first, blmv2.second));
  int rep2 = pair_to_int(height, make_pair(blmv2.first, lmv2.second));
  
  if(g.is_allowed(rep1))
    l_moves.push_back(rep1);

  if(g.is_allowed(rep2))
    l_moves.push_back(rep2); 
}


void GE_Strategy::jam0(GE_Game& g, list<int>& l_moves)
{
  int last_move = g.get_last_move();
  int before_last_move = g.get_before_last_move();
  int& height = (g.goban)->height;
  int& width = (g.goban)->width;

  if(before_last_move==GE_ILLEGAL_MOVE)
    return;
  
  pair<int, int> dist2 = distance_goban(last_move, before_last_move, height); 
  
  if(dist2.first+dist2.second!=1)
    return;

  /*
    //the same
    if(((dist2.first==0)&&(dist2.second!=1))
    ||(dist2.first==1)&&(dist2.second!=0))
    return;
  */
 
  pair<int, int> lmv2 = int_to_pair(height, last_move);
 
  pair<int, int> mv1 = make_pair(lmv2.first+dist2.first, lmv2.second+dist2.second);
  pair<int, int> mv2 = make_pair(lmv2.first-dist2.first, lmv2.second-dist2.second);
  
  if(GE_IS_IN_GOBAN(mv1.first, mv1.second, height, width))
    {
      int rep1 = pair_to_int(height, mv1);
      if(g.is_allowed(rep1))
	l_moves.push_back(rep1);
    }

  if(GE_IS_IN_GOBAN(mv2.first, mv2.second, height, width))
    {
      int rep2 = pair_to_int(height, mv2);
      if(g.is_allowed(rep2))
	l_moves.push_back(rep2);
    }

}



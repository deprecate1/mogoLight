#include "tactic.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>











// Threaten::Threaten(int type, int mv, int val_threat, list<int>* l_ans)
// {
//   this->type = type;
//   this->mv = mv;
//   this->val = val_threat;
//   if(l_ans) answers = *l_ans;
// }

// void Threaten::buid_answers(GE_Game& g)
// {
//   this->buils_answers_threaten_capture(g);
  
  
// }

// void Threaten::build_answers_threaten_capture_slow(GE_Game& g)
// {
//   this->buils_answers_threaten_capture(g);
// }

// void Threaten::build_answer_threaten_capture_fast()
// {
//   if(mv!=GE_PASS_MOVE)
//     answers.push_back(mv);
// }



// Threaten::build_threaten_capture(const GE_Chains& l_ch, int mv)
// {
//   int val_thr = -1;

//   threatens.push_backThreaten();


// }


// void kill_group();
// void save_group();


int GE_Tactic::max_depth = 1000;



void GE_Tactic::eat_or_to_be_eaten(GE_Game& g, GE_Chains& neighbours_last_move, 
				list<pair<int, int> >& solutions, 
				int min_size, int min_size2) const
{
  int col_stone = (g.tc==GE_WHITE) ? GE_WHITE_STONE : GE_BLACK_STONE;
  
  list<GE_Chain>::const_iterator i_ch = neighbours_last_move.sets.begin();
  
  while(i_ch!=neighbours_last_move.sets.end())
    {
      if((not i_ch->is_atari())||(i_ch->stone!=col_stone))
	{
	  i_ch++;
	  continue;
	}
      
      if(i_ch->size()<=min_size)
	{
	  i_ch++;
	  continue;
	}

      GE_Chains neighbours;
      neighbours.neighbour_chains(*(g.goban), *i_ch);
      
      list<int> free_places;
      int val_threat = 0;
      neighbours.get_nb_atari(GE_NEXT_COLOR(g.tc), &val_threat, &free_places);
      
      if(val_threat<=min_size2)
	{
	  i_ch++;
	  continue;
	}

      //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      //if we combine eat or to be eaten with flee0

      //In princip, this thing is made later, in reality
      int temp_loop = free_places.size();
      while(temp_loop>0)
	{
	  if(g.is_allowed(free_places.front()))
	    free_places.push_back(free_places.front());
	  
	  free_places.pop_front();
	  
	  temp_loop--;
	}

      
      //  if(not free_places.empty())
      // 	if(not (i_ch->free_intersections).empty())
      // 	  solutions.push_back(make_pair((i_ch->free_intersections).back(), i_ch->size()));
      
      //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      
      while(not free_places.empty())
	{
	  solutions.push_back(make_pair(free_places.front(), val_threat));
	  
	  free_places.pop_front();
	}
      
      i_ch++;
    }
}

//min_size = 1 is good
void GE_Tactic::flee0(GE_Game& g, GE_Chains& neighbours_last_move, 
		   list<pair<int, int> >& solutions, int min_size) const
{
  int col_stone = (g.tc==GE_WHITE) ? GE_WHITE_STONE : GE_BLACK_STONE;
  
  list<GE_Chain>::const_iterator i_ch = neighbours_last_move.sets.begin();
  
  while(i_ch!=neighbours_last_move.sets.end())
    {
      if((not i_ch->is_atari())||(i_ch->stone!=col_stone))
	{
	  i_ch++;
	  continue;
	}
      
      if((int)(i_ch->links).size()<=min_size) 
	{
	  i_ch++;
	  continue;
	}


      if(not (i_ch->free_intersections).empty())
	solutions.push_back(make_pair((i_ch->free_intersections).back(), i_ch->size()));
      
      i_ch++;
    }
}


//min_size = 1 is good
void GE_Tactic::flee1(GE_Game& g, GE_Chains& neighbours_last_move, 
		   list<pair<int, int> >& solutions, int min_size, 
		   bool anti_stupid, bool anti_shisho) const
{
  int col_stone = (g.tc==GE_WHITE) ? GE_WHITE_STONE : GE_BLACK_STONE;
  
  list<GE_Chain>::const_iterator i_ch = neighbours_last_move.sets.begin();
  
  while(i_ch!=neighbours_last_move.sets.end())
    {
      if((not i_ch->is_atari())||(i_ch->stone!=col_stone))
	{
	  i_ch++;
	  continue;
	}
      
      if((int)(i_ch->links).size()<=min_size) 
	{
	  i_ch++;
	  continue;
	}
      
      int good_move = (i_ch->free_intersections).back();
      if(not (i_ch->free_intersections).empty())
	if((not anti_stupid)||(not g.is_stupid(good_move)))
	  if((not anti_shisho)||(not study_shisho_after_move2(g, good_move)))
	    solutions.push_back(make_pair(good_move, i_ch->size()));
      
      i_ch++;
    }
}


void GE_Tactic::threat0(GE_Game& g, GE_Chain& ch_last_move, list<pair<int, int> >& l_moves, 
		     bool anti_stupid) const
{
  if(ch_last_move.liberties==2) //(ch_last_move.free_intersections.size()==2)
    {
      int& candidate1 = ch_last_move.free_intersections.front();
      int& candidate2 = ch_last_move.free_intersections.back();
      int eval = ch_last_move.links.size();
      
      if((not anti_stupid)||(not g.is_stupid(candidate1)))
	l_moves.push_back(make_pair(candidate1, eval));
      
      if((not anti_stupid)||(not g.is_stupid(candidate2)))
	l_moves.push_back(make_pair(candidate2, eval));
      
    }
}

void GE_Tactic::attacks_double_atari0(GE_Game& g, int last_move, GE_Chain& ch_last_move, list<int>& l_moves, 
				   bool anti_stupid) const
{
  if(ch_last_move.liberties==2) //(ch_last_move.free_intersections.size()==2)
    {
      int& candidate1 = ch_last_move.free_intersections.front();
      int& candidate2 = ch_last_move.free_intersections.back();
     
      if((not anti_stupid)||(not g.is_stupid(candidate1)))
	{
	  GE_Chains chs;
	  chs.neighbour_chains(*(g.goban), candidate1);
	  
	  list<GE_Chain>::const_iterator i_ch = chs.sets.begin();
	  
	  while(i_ch!=chs.sets.end())
	    {
	      if(i_ch->belongs(last_move))
		{
		  i_ch++;
		  continue;
		}
	      
	      if((i_ch->liberties==2)&&(i_ch->stone==(g.goban)->get_stone(last_move)))
		{ 
		  l_moves.push_back(candidate1);
		  break;
		}
	      
	      i_ch++;
	    }	  
	}
      
      
      if((not anti_stupid)||(not g.is_stupid(candidate2)))
	{
	  GE_Chains chs;
	  chs.neighbour_chains(*(g.goban), candidate2);
	  
	  list<GE_Chain>::const_iterator i_ch = chs.sets.begin();
	  
	  while(i_ch!=chs.sets.end())
	    {
	      if(i_ch->belongs(last_move))
		{
		  i_ch++;
		  continue;
		}
	      
	      if((i_ch->liberties==2)&&(i_ch->stone==(g.goban)->get_stone(last_move)))
		{ 
		  l_moves.push_back(candidate2);
		  break;
		}
 
	      i_ch++;
	    }	  
	}
    }
}

void GE_Tactic::attacks_two_groups0(GE_Game& g, int last_move, GE_Chain& ch_last_move, 
				 list<int>& l_moves, int nb_liberties, bool anti_stupid) const
{
  if(ch_last_move.liberties==2) //(ch_last_move.free_intersections.size()==2)
    {
      int& candidate1 = ch_last_move.free_intersections.front();
      int& candidate2 = ch_last_move.free_intersections.back();
     
      if((not anti_stupid)||(not g.is_stupid(candidate1)))
	{
	  GE_Chains chs;
	  chs.neighbour_chains(*(g.goban), candidate1);
	  
	  list<GE_Chain>::const_iterator i_ch = chs.sets.begin();
	  
	  while(i_ch!=chs.sets.end())
	    {
	      if(i_ch->belongs(last_move))
		{
		  i_ch++;
		  continue;
		}
	      
	      if((i_ch->liberties==nb_liberties)&&(i_ch->stone==(g.goban)->get_stone(last_move)))
		{ 
		  l_moves.push_back(candidate1);
		  break;
		}
	      
	      i_ch++;
	    }	  
	}
      
      
      if((not anti_stupid)||(not g.is_stupid(candidate2)))
	{
	  GE_Chains chs;
	  chs.neighbour_chains(*(g.goban), candidate2);
	  
	  list<GE_Chain>::const_iterator i_ch = chs.sets.begin();
	  
	  while(i_ch!=chs.sets.end())
	    {
	      if(i_ch->belongs(last_move))
		{
		  i_ch++;
		  continue;
		}
	      
	      if((i_ch->liberties==nb_liberties)&&(i_ch->stone==(g.goban)->get_stone(last_move)))
		{ 
		  l_moves.push_back(candidate2);
		  break;
		}
 
	      i_ch++;
	    }	  
	}
    }
}



void GE_Tactic::reduce_liberties0(GE_Game& g, GE_Chain& ch_last_move, list<pair<int, int> >& l_moves, 
			       int nb_liberties, bool anti_stupid)
{
  if((nb_liberties<0)
     ||(ch_last_move.liberties==nb_liberties)) //(ch_last_move.free_intersections.size()==2)
    {
      list<int>::const_iterator i_fi = ch_last_move.free_intersections.begin();
      
      int eval = ch_last_move.links.size();
      while(i_fi!=ch_last_move.free_intersections.end())
	{
	  if((not anti_stupid)||(not g.is_stupid(*i_fi)))
	    l_moves.push_back(make_pair(*i_fi, eval));
	  
	  i_fi++;
	}
    }
}


void GE_Tactic::attacks_by_reducing_liberties0(GE_Game& g, GE_Chain& ch_last_move, 
					    GE_Chain& ch_before_last_move, 
					    list<pair<int, int> >& l_moves, bool anti_stupid)
{
  int eval = ch_last_move.links.size();
  list<int> inter; 
  
  GE_ListOperators::intersection(ch_before_last_move.free_intersections, 
			      ch_last_move.free_intersections, 
			      inter);
  
  list<int>::const_iterator i_inter = inter.begin();      
  
  while(i_inter!=inter.end())
    {
      if((not anti_stupid)||(not g.is_stupid(*i_inter)))
	l_moves.push_back(make_pair(*i_inter, eval));
      
      i_inter++;
    }
}

void GE_Tactic::prevent_connection0(GE_Game& g, int last_move, pair<int, int>& lmv2, int dir,  
				 list<pair<int, int> >& l_moves, int nb_liberties, 
				 GE_Chain* ch_last_move, bool anti_stupid) const
{
  int& height = (g.goban)->height;
  int& width = (g.goban)->width;
  
  pair<int, int> neighbour2;
  int neighbour;

  switch(dir)
    {
    case GE_NORTH : neighbour2 = make_pair(lmv2.first-1, lmv2.second); break;
    case GE_SOUTH : neighbour2 = make_pair(lmv2.first+1, lmv2.second); break;
    case GE_WEST : neighbour2 = make_pair(lmv2.first, lmv2.second-1); break;
    case GE_EAST : neighbour2 = make_pair(lmv2.first, lmv2.second+1); break;
    default : assert(0);
    }
  
  if(not GE_IS_IN_GOBAN(neighbour2.first, neighbour2.second, height, width))
    return;

  if((g.goban)->get_stone(neighbour2)!=GE_WITHOUT_STONE)
    return;

  neighbour = pair_to_int(height, neighbour2);
  
  GE_Chains chs;
  chs.neighbour_chains(*(g.goban), neighbour);
  
  list<GE_Chain>::const_iterator i_ch = chs.sets.begin();
  while(i_ch!=chs.sets.end())
    {
      if((i_ch->stone!=(g.goban)->get_stone(lmv2))||(i_ch->belongs(last_move)))
	{
	  i_ch++;
	  continue;
	}
      
      int eval = i_ch->size();
      
      
      if((nb_liberties<0)||(i_ch->liberties==nb_liberties))
	if((not ch_last_move)||(not i_ch->is_virtually_connected(*ch_last_move)))
	  if((not anti_stupid)||(not g.is_stupid(neighbour)))
	    l_moves.push_back(make_pair(neighbour, eval));
      
      i_ch++;
    } 
}
  
void GE_Tactic::prevent_connection0(GE_Game& g, int last_move, list<pair<int, int> >& l_moves, 
				 int nb_liberties, GE_Chain* ch_last_move, 
				 bool anti_stupid) const
{
  if(last_move<0) return;
  
  pair<int, int> lmv2 = int_to_pair((g.goban)->height, last_move);
 
  this->prevent_connection0(g, last_move, lmv2, GE_NORTH, l_moves, nb_liberties, ch_last_move, anti_stupid);
  this->prevent_connection0(g, last_move, lmv2, GE_SOUTH, l_moves, nb_liberties, ch_last_move, anti_stupid);
  this->prevent_connection0(g, last_move, lmv2, GE_EAST, l_moves, nb_liberties, ch_last_move, anti_stupid);
  this->prevent_connection0(g, last_move, lmv2, GE_WEST, l_moves, nb_liberties, ch_last_move, anti_stupid);
}





//peep connect ?
void GE_Tactic::connect0(GE_Game& g, pair<int, int>& lmv2, int dir, list<int>& l_moves, 
		      bool anti_stupid) const
{
  int last_color_stone = (g.goban)->get_stone(lmv2);
  int future_color_stone = GE_COLOR_STONE_ENEMY(last_color_stone);

  int& height = (g.goban)->height;
  int& width = (g.goban)->width;
  
  pair<int, int> place;
  pair<int, int> place_left;
  pair<int, int> disconnection_left;
  
  pair<int, int> place_right;
  pair<int, int> disconnection_right;

  pair<int, int> good_move;
  
  switch(dir)
    {
    case GE_NORTH : 
      place = make_pair(lmv2.first-2, lmv2.second);
      place_left = make_pair(lmv2.first-1, lmv2.second-1);
      disconnection_left = make_pair(lmv2.first-2, lmv2.second-1);

      place_right = make_pair(lmv2.first-1, lmv2.second+1);
      disconnection_right = make_pair(lmv2.first-2, lmv2.second+1);
      
      good_move = make_pair(lmv2.first-1, lmv2.second);
      break;
      
    case GE_SOUTH : 
      place = make_pair(lmv2.first+2, lmv2.second);
      place_left = make_pair(lmv2.first+1, lmv2.second-1);
      disconnection_left = make_pair(lmv2.first+2, lmv2.second-1);

      place_right = make_pair(lmv2.first+1, lmv2.second+1);
      disconnection_right = make_pair(lmv2.first+2, lmv2.second+1);
      
      good_move = make_pair(lmv2.first+1, lmv2.second);
      break;
         
    case GE_WEST : 
      place = make_pair(lmv2.first, lmv2.second-2);
      place_left = make_pair(lmv2.first-1, lmv2.second-1);
      disconnection_left = make_pair(lmv2.first-1, lmv2.second-2);

      place_right = make_pair(lmv2.first+1, lmv2.second-1);
      disconnection_right = make_pair(lmv2.first+1, lmv2.second-2);
      
      good_move = make_pair(lmv2.first, lmv2.second-1);
      break;

    case GE_EAST :
      place = make_pair(lmv2.first, lmv2.second+2);
      place_left = make_pair(lmv2.first-1, lmv2.second+1);
      disconnection_left = make_pair(lmv2.first-1, lmv2.second+2);

      place_right = make_pair(lmv2.first+1, lmv2.second+1);
      disconnection_right = make_pair(lmv2.first+1, lmv2.second+2);
      
      good_move = make_pair(lmv2.first, lmv2.second+1);
      break;

    default: return;     
      
    }
  
  if(GE_IS_IN_GOBAN(disconnection_left.first, disconnection_left.second, height, width))
    {
      if((g.goban)->get_stone(place)==future_color_stone)
	if((g.goban)->get_stone(place_left)==future_color_stone)
	  if((g.goban)->get_stone(disconnection_left)==last_color_stone)
	    if((g.goban)->get_stone(good_move)==GE_WITHOUT_STONE)
	      {	
		int gm = pair_to_int(height, good_move);
		if((not anti_stupid)||(not g.is_stupid(gm)))
		  l_moves.push_back(gm);
		return;
	      }    
    }
  
  if(GE_IS_IN_GOBAN(disconnection_right.first, disconnection_right.second, height, width))
    {
      if((g.goban)->get_stone(place)==future_color_stone)
	if((g.goban)->get_stone(place_right)==future_color_stone)
	  if((g.goban)->get_stone(disconnection_right)==last_color_stone)
	    if((g.goban)->get_stone(good_move)==GE_WITHOUT_STONE)
	      {	
		int gm = pair_to_int(height, good_move);
		if((not anti_stupid)||(not g.is_stupid(gm)))
		  l_moves.push_back(gm);
		return;
	      }    
    }
}


void GE_Tactic::connect0(GE_Game& g, int last_move, list<int>& l_moves, bool anti_stupid) const
{
  if(last_move<0) return;
  
  pair<int, int> lmv2 = int_to_pair((g.goban)->height, last_move);
 
  this->connect0(g, lmv2, GE_NORTH, l_moves, anti_stupid);
  this->connect0(g, lmv2, GE_SOUTH, l_moves, anti_stupid);
  this->connect0(g, lmv2, GE_EAST, l_moves, anti_stupid);
  this->connect0(g, lmv2, GE_WEST, l_moves, anti_stupid);
}



void GE_Tactic::connect_if_dead_virtual_connection0(GE_Game& g, int last_move, 
						 list<pair<int, int> >& l_moves, 
						 bool anti_stupid)
{
  if(g.game.empty()) return;
  if(last_move==GE_PASS_MOVE) return;
  
  int color_stone = (g.goban)->get_stone(last_move);

  g.backward();
  
  GE_Chains chs;
  chs.neighbour_chains(*(g.goban), last_move);
  
  list<GE_Chain>::const_iterator i_ch1 = chs.sets.begin();
  while(i_ch1!=chs.sets.end())
    {
      if(i_ch1->stone==color_stone)
	{
	  i_ch1++;
	  continue;
	}
      
      list<GE_Chain>::const_iterator i_ch2 = i_ch1;
      
      i_ch2++;
      
      while(i_ch2!=chs.sets.end())
	{
	  if(i_ch2->stone==color_stone)
	    { 
	      i_ch2++;
	      continue;
	    }
	  
	  assert(i_ch1->stone==i_ch2->stone);

	  //so i_ch1->stone == i_ch2->stone is true


	  list<int> connections;
	  if(i_ch1->is_virtually_connected(*i_ch2, &connections, true))
	    if(connections.size()==2)
	      {
		GE_ListOperators::erase(connections, last_move);
		if(connections.size()!=1)
		  {
		    cout<<"*** ERROR *** connect_if_dead_virtual_connection"<<endl;
		    exit(-1);
		  }
		
		if((not anti_stupid)||(not g.is_stupid(connections.front())))
		  l_moves.push_back(make_pair(connections.front(), 
					      i_ch1->size()+i_ch2->size()));
	      }
	  
	  i_ch2++;
	}
      
      i_ch1++;
    }
  

  g.play(last_move);  
}



//symetric ?
void GE_Tactic::increase_liberties0(GE_Game& g, pair<int, int>& lmv2, int dir, list<int>& l_moves, 
				 bool anti_stupid) const
{
  int last_color_stone = (g.goban)->get_stone(lmv2);
  int future_color_stone = GE_COLOR_STONE_ENEMY(last_color_stone);

  int& height = (g.goban)->height;
  int& width = (g.goban)->width;
  
  pair<int, int> place;
  pair<int, int> place_left;
  pair<int, int> place_right;
  pair<int, int> good_move;
  
  switch(dir)
    {
    case GE_NORTH : 
      place = make_pair(lmv2.first-1, lmv2.second);
      place_left = make_pair(lmv2.first-1, lmv2.second-1);
      place_right = make_pair(lmv2.first-1, lmv2.second+1); 
      good_move = make_pair(lmv2.first-2, lmv2.second);
      break;
      
    case GE_SOUTH : 
      place = make_pair(lmv2.first+1, lmv2.second);
      place_left = make_pair(lmv2.first+1, lmv2.second-1);
      place_right = make_pair(lmv2.first+1, lmv2.second+1); 
      good_move = make_pair(lmv2.first+2, lmv2.second);
      break;
      
    case GE_WEST : 
      place = make_pair(lmv2.first, lmv2.second-1);
      place_left = make_pair(lmv2.first-1, lmv2.second-1);
      place_right = make_pair(lmv2.first+1, lmv2.second-1); 
      good_move = make_pair(lmv2.first, lmv2.second-2);
      break;

    case GE_EAST :
      place = make_pair(lmv2.first, lmv2.second+1);
      place_left = make_pair(lmv2.first-1, lmv2.second+1);
      place_right = make_pair(lmv2.first+1, lmv2.second+1); 
      good_move = make_pair(lmv2.first, lmv2.second+2);
      break;

    default: return;     
      
    }
  
  if(GE_IS_IN_GOBAN(good_move.first, good_move.second, height, width))
    {
      if(GE_IS_IN_GOBAN(place_left.first, place_left.second, height, width))
	if((g.goban)->get_stone(place_left)==last_color_stone)
	  if((g.goban)->get_stone(place)==future_color_stone)
	    if((g.goban)->get_stone(good_move)==GE_WITHOUT_STONE)
	      {	
		int gm = pair_to_int(height, good_move);
		if((not anti_stupid)||(not g.is_stupid(gm)))
		  l_moves.push_back(gm);
		return;
	      }    

      if(GE_IS_IN_GOBAN(place_right.first, place_right.second, height, width))
	if((g.goban)->get_stone(place_right)==last_color_stone)
	  if((g.goban)->get_stone(place)==future_color_stone)
	    if((g.goban)->get_stone(good_move)==GE_WITHOUT_STONE)
	      {	
		int gm = pair_to_int(height, good_move);
		if((not anti_stupid)||(not g.is_stupid(gm)))
		  l_moves.push_back(gm);
		return;
	      }   
    }
}


void GE_Tactic::increase_liberties0(GE_Game& g, int last_move, list<int>& l_moves, bool anti_stupid) const
{
  if(last_move<0) return;
  
  pair<int, int> lmv2 = int_to_pair((g.goban)->height, last_move);
  
  this->increase_liberties0(g, lmv2, GE_NORTH, l_moves, anti_stupid);
  this->increase_liberties0(g, lmv2, GE_SOUTH, l_moves, anti_stupid);
  this->increase_liberties0(g, lmv2, GE_EAST, l_moves, anti_stupid);
  this->increase_liberties0(g, lmv2, GE_WEST, l_moves, anti_stupid);
}



void GE_Tactic::one_point_jump0(GE_Game& g, const pair<int, int>& blmv2, int dir, 
			     list<int>& l_moves, bool anti_stupid) const
{
  int& height = (g.goban)->height;
  int& width = (g.goban)->width;

  pair<int, int> good_move;
  pair<int, int> place_center;
  pair<int, int> place_left;
  pair<int, int> place_right;
  
  switch(dir)
    {
    case GE_NORTH :
      good_move = make_pair(blmv2.first-2, blmv2.second);
      place_center = make_pair(blmv2.first-1, blmv2.second);
      place_left = make_pair(blmv2.first-1, blmv2.second-1);
      place_right = make_pair(blmv2.first-1, blmv2.second+1);
      break;
      
    case GE_SOUTH :
      good_move = make_pair(blmv2.first+2, blmv2.second);
      place_center = make_pair(blmv2.first+1, blmv2.second);
      place_left = make_pair(blmv2.first+1, blmv2.second-1);
      place_right = make_pair(blmv2.first+1, blmv2.second+1);
      break;

    case GE_EAST :
      good_move = make_pair(blmv2.first, blmv2.second+2);
      place_center = make_pair(blmv2.first, blmv2.second+1);
      place_left = make_pair(blmv2.first-1, blmv2.second+1);
      place_right = make_pair(blmv2.first+1, blmv2.second+1);
      break;
      
    case GE_WEST :
      good_move = make_pair(blmv2.first, blmv2.second-2);
      place_center = make_pair(blmv2.first, blmv2.second-1);
      place_left = make_pair(blmv2.first-1, blmv2.second-1);
      place_right = make_pair(blmv2.first+1, blmv2.second-1);
      break;

    default : assert(0);
    }
  
  if(GE_IS_IN_GOBAN(good_move.first, good_move.second, height, width))
    if(not GE_IS_IN_BORDER_GOBAN(good_move.first, good_move.second, height, width))
      if((g.goban)->get_stone(good_move)==GE_WITHOUT_STONE)
	if((g.goban)->get_stone(place_center)==GE_WITHOUT_STONE)
	  if((g.goban)->get_stone(place_right)==GE_WITHOUT_STONE)
	    if((g.goban)->get_stone(place_left)==GE_WITHOUT_STONE)
	      {
		int gm = pair_to_int(height, good_move);
		if((not anti_stupid)||(not g.is_stupid(gm)))
		  l_moves.push_back(gm);
	      }
}


void GE_Tactic::one_point_jump0(GE_Game& g, int before_last_move, 
			     list<int>& l_moves, bool anti_stupid) const
{
  if(before_last_move<0) return;
  
  pair<int, int> blmv2 = int_to_pair((g.goban)->height, before_last_move);
  
  this->one_point_jump0(g, blmv2, GE_NORTH, l_moves, anti_stupid);
  this->one_point_jump0(g, blmv2, GE_SOUTH, l_moves, anti_stupid);
  this->one_point_jump0(g, blmv2, GE_EAST, l_moves, anti_stupid);
  this->one_point_jump0(g, blmv2, GE_WEST, l_moves, anti_stupid);
}


bool GE_Tactic::study_shisho_after_move(GE_Game& g, int mv, list<int>* l_moves)
{
  if(not g.play(mv))
    return true;
  
  bool rep = study_shisho(g, mv, l_moves);
  g.backward();
  return rep;
}

//TO DEBUG this function must be tested
bool GE_Tactic::study_shisho(GE_Game& g, int stone, list<int>* l_moves)
{
  if(stone==GE_PASS_MOVE) return false;
  
  GE_Chain ch;
  ch.init(*(g.goban), stone);
  return study_shisho(g, stone, ch, l_moves);
}

//WARNING Color of the stone and the player
//TO IMPLEMENT: another possibility to save is to take (EAT OR TO BE EATEN)
bool GE_Tactic::study_shisho(GE_Game& g, int stone, GE_Chain& ch, list<int>* l_moves)
{
  if(ch.free_intersections.size()>2) return false;
  if(ch.free_intersections.size()<=1) return true;
  
  list<int> l_rep = ch.free_intersections;

  while(not l_rep.empty())
    {
      int att = l_rep.front();
      l_rep.pop_front();
      
      if(not g.play(att)) continue;
      GE_ListOperators::erase(ch.free_intersections, att);
      
      if(ch.free_intersections.size()!=1)
	{
	  cout<<"*** ERROR *** study_shisho(GE_Game&, int, GE_Chain&, list<int>*)"<<endl;
	  cout<<"   --> the size of ch.free_intersections is different of 1"<<endl;
	  exit(-1);
	}
      
      int def = ch.free_intersections.back();
      
      if(not g.play(def))
	{
	  g.backward();
	  ch.free_intersections.push_back(att);

	  if(l_moves)
	    {
	      l_moves->push_back(att);
	      continue;
	    }
	  
	  return true;
	}
      
      GE_ListOperators::erase(ch.free_intersections, def);
      
      list<int> liberties;
      int nb_liberties = (g.goban)->get_nb_liberties(def, &liberties);
      
      if(nb_liberties>2)
	{
	  g.backward();
	  g.backward();
	  ch.free_intersections.push_back(att);
	  ch.free_intersections.push_back(def);
	  continue;
	}
      
      /*
      if(nb_liberties<2)
	{
	  g.backward();
	  g.backward();
	  ch.free_intersections.push_back(att);
	  ch.free_intersections.push_back(def);
	  if(l_moves)
	    {
	      l_moves->push_back(att);
	      continue;
	    }
	  
	  return true;
	} 
      */
      //nb_liberties==2

      
      //ch.free_intersections = liberties; //with the same color of stone, it is a mistake
      //because we have new connection
      GE_Chain ch2; ch2.init(*(g.goban), stone);
      bool is_good_shisho = GE_Tactic::study_shisho(g, stone, ch2);
      ch.free_intersections.clear();
      g.backward();
      g.backward();
      ch.free_intersections.push_back(att);
      ch.free_intersections.push_back(def);

      if(is_good_shisho)
	{
	  if(l_moves)
	    {
	      l_moves->push_back(att);
	      continue;
	    }

	  return true;
	}

    }
  
  if(l_moves) return (not l_moves->empty());
  
  return false;  
}




// void eat();
// void atari();
// void double_atari();
// void make_eye();


//TODO THIS FUNCTION MUST BE DEPLACED  
bool append_move(GE_Game& g, int x, int y, list<int>& l_moves)
{
  GE_Goban& gob = *(g.goban);
  
  if(GE_IS_IN_GOBAN(x, y, gob.height, gob.width))
    {
      int place = pair_to_int(gob.height, make_pair(x, y));
      
      if(g.is_allowed(place))
	{
	  l_moves.push_back(place);
	  return true;
	}
      
    }
  
  return false;
}


//TODO THIS FUNCTION MUST BE DEPLACED  
void get_moves_by_random(GE_Game& g, list<int>& l_moves, 
			 int length)
{  
  int max_loops = 10*(g.goban)->size();
  
  while(true)
    {
      int mv = rand()%(g.goban)->size();
      
      if(g.is_allowed(mv))
	{
	  if(GE_ListOperators::push_if_not(l_moves, mv))
	    if((int)l_moves.size()>=length)
	      break;
	}
      
      max_loops--;
      if(max_loops==0) break;
    }
}

//TODO THIS FUNCTION MUST BE DEPLACED  
void get_moves_by_shape(GE_Game& g, int place, list<int>& l_moves, 
			int mode)
{
  if(place==GE_PASS_MOVE) return;
  GE_Goban& gob = *(g.goban);
  
  const pair<int, int> place2 = int_to_pair(gob.height, place);
  const int& i = place2.first;
  const int& j = place2.second;
  
  switch(mode)
    {
      
    case GE_SUN_1 :
      append_move(g, i, j+3, l_moves);
      append_move(g, i, j-3, l_moves);
      append_move(g, i+3, j, l_moves);
      append_move(g, i-3, j, l_moves);
      //no break
      
    case GE_SHAPE_PYRAMID :
      append_move(g, i, j+2, l_moves);
      append_move(g, i, j-2, l_moves);
      append_move(g, i+2, j, l_moves);
      append_move(g, i-2, j, l_moves);
      //no break

    case GE_CONNEXITY_8 :
      append_move(g, i+1, j+1, l_moves);
      append_move(g, i-1, j+1, l_moves);
      append_move(g, i+1, j-1, l_moves);
      append_move(g, i-1, j-1, l_moves);
      //no break
      
    case GE_CONNEXITY_4 :
      append_move(g, i, j+1, l_moves);
      append_move(g, i, j-1, l_moves);
      append_move(g, i+1, j, l_moves);
      append_move(g, i-1, j, l_moves);
      break;
      
      
    case GE_CROSS_2 :
      append_move(g, i, j+1, l_moves);
      append_move(g, i, j-1, l_moves);
      append_move(g, i+1, j, l_moves);
      append_move(g, i-1, j, l_moves);
      append_move(g, i, j+2, l_moves);
      append_move(g, i, j-2, l_moves);
      append_move(g, i+2, j, l_moves);
      append_move(g, i-2, j, l_moves);
      break;

      
      
      
    default: ;
    }
  
}


void get_moves_by_amaf0(GE_Game& g, list<int>& l_moves, 
			int max_sim, int max_length)
{
  int num_sim = 0;
  
  GE_Amaf am((g.goban)->size());
  
  bool was_updated = GE_Simulation::update_komi_on();
  
  while(num_sim<max_sim)
    {
      num_sim++;
      
      //cout<<"simulation n°"<<nb_sim<<"   "<<endl;
      
      GE_Simulation::launch(g);
      GE_Simulation::update_amaf(g, am);
      
    }
  
  GE_Simulation::reset_update_komi(was_updated);
  
  int* v = am.values;

  if(g.tc==GE_BLACK)
    v = am.black_values;
  else v = am.white_values;
  
  am.preferred_moves(v, (g.goban)->height, &l_moves, -1, false);
  
  int nb_loop = l_moves.size();
  while(nb_loop>0)
    {
      nb_loop--;
      if(g.is_allowed(l_moves.front()))
	l_moves.push_back(l_moves.front());
      
      l_moves.pop_back();
    }
  
  
  while((int)l_moves.size()>max_length)
    {    
      l_moves.pop_back();
    }
}




/******************
 * Basic instinct *
 ******************/

void GE_Tactic::peep_connect0(GE_Game& g, 
			   pair<int, int>& lmv2, 
			   int dir, 
			   list<int>& l_moves, 
			   bool anti_stupid)
{
  int& height = (g.goban)->height;
  int& width = (g.goban)->width;
  
  pair<int, int> place1;
  pair<int, int> place2;
  pair<int, int> good_move;
  
  switch(dir)
    {
    case GE_NORTH : 
      place1.first = lmv2.first-1;
      place1.second = lmv2.second-1;
      place2.first = lmv2.first-1;
      place2.second = lmv2.second+1;
      good_move = make_pair(lmv2.first-1, lmv2.second);
      break;
      
    case GE_SOUTH : 
      place1.first = lmv2.first+1;
      place1.second = lmv2.second-1;
      place2.first = lmv2.first+1;
      place2.second = lmv2.second+1;
      good_move = make_pair(lmv2.first+1, lmv2.second);
      break;
      
    case GE_WEST : 
      place1.first = lmv2.first-1;
      place1.second = lmv2.second-1;
      place2.first = lmv2.first+1;
      place2.second = lmv2.second-1;
      good_move = make_pair(lmv2.first, lmv2.second-1);
      break;
      
    case GE_EAST : 
      place1.first = lmv2.first-1;
      place1.second = lmv2.second+1;
      place2.first = lmv2.first+1;
      place2.second = lmv2.second+1;
      good_move = make_pair(lmv2.first, lmv2.second+1);
      break;
      
    default: return;     
      
    }
  
  
  if((GE_IS_IN_GOBAN(place1.first, place1.second, height, width))
     &&(GE_IS_IN_GOBAN(place2.first, place2.second, height, width)))
    {
      if((g.goban)->get_stone(place1)==(g.goban)->get_stone(place2))
	if((g.goban)->get_stone(place1)!=GE_WITHOUT_STONE)
	  if((g.goban)->get_stone(place1)!=(g.goban)->get_stone(lmv2))
	    if((g.goban)->get_stone(good_move)==GE_WITHOUT_STONE)
	      {	
		int gm = pair_to_int(height, good_move);
		if((not anti_stupid)||(not g.is_stupid(gm)))
		  l_moves.push_back(gm);
	      }
    } 
}




void GE_Tactic::peep_connect0(GE_Game& g, int last_move, list<int>& l_moves, bool anti_stupid)
{
  if(last_move<0) return;
  
  pair<int, int> lmv2 = int_to_pair((g.goban)->height, last_move);
  
  this->peep_connect0(g, lmv2, GE_NORTH, l_moves, anti_stupid);
  this->peep_connect0(g, lmv2, GE_SOUTH, l_moves, anti_stupid);
  this->peep_connect0(g, lmv2, GE_EAST, l_moves, anti_stupid);
  this->peep_connect0(g, lmv2, GE_WEST, l_moves, anti_stupid);
}


void GE_Tactic::tsuke_hane0(GE_Game& g, const pair<int, int>& lmv2, int dir, list<int>& l_moves, 
			 bool anti_stupid) const
{
  int& height = (g.goban)->height;
  int& width = (g.goban)->width;
  
  if(not (g.goban)->is_alone(lmv2)) return;

  int last_color_stone = (g.goban)->get_stone(lmv2);
  int future_color_stone = GE_COLOR_STONE_ENEMY(last_color_stone);
  
  pair<int, int> place_left;
  pair<int, int> place_right;
  pair<int, int> good_move;
  
  switch(dir)
    {
    case GE_NORTH :
      place_left = make_pair(lmv2.first-1, lmv2.second);
      place_right = make_pair(lmv2.first+1, lmv2.second);
      good_move = make_pair(lmv2.first, lmv2.second-1);
      break;

    case GE_SOUTH :
      place_left = make_pair(lmv2.first-1, lmv2.second);
      place_right = make_pair(lmv2.first+1, lmv2.second);
      good_move = make_pair(lmv2.first, lmv2.second+1);
      break;

    case GE_EAST :
      place_left = make_pair(lmv2.first, lmv2.second-1);
      place_right = make_pair(lmv2.first, lmv2.second+1);
      good_move = make_pair(lmv2.first+1, lmv2.second);
      break;

    case GE_WEST :
      place_left = make_pair(lmv2.first, lmv2.second-1);
      place_right = make_pair(lmv2.first, lmv2.second+1);
      good_move = make_pair(lmv2.first-1, lmv2.second);
      break;

    default:
      assert(0);
    }
  
  if(not GE_IS_IN_GOBAN(good_move.first, good_move.second, height, width)) return;
  if((g.goban)->get_stone(good_move)!=GE_WITHOUT_STONE) return;


  if(not GE_IS_IN_GOBAN(place_left.first, place_left.second, height, width))
    {
      if((g.goban)->get_stone(place_right)==future_color_stone)
	{
	  int gm = pair_to_int(height, good_move);
	  if((not anti_stupid)||(not g.is_stupid(gm)))
	    l_moves.push_back(gm);
	}
      return;
    }
  
  if(not GE_IS_IN_GOBAN(place_right.first, place_right.second, height, width))
    {
      if((g.goban)->get_stone(place_left)==future_color_stone)
	{
	  int gm = pair_to_int(height, good_move);
	  if((not anti_stupid)||(not g.is_stupid(gm)))
	    l_moves.push_back(gm);
	}
      return;
    }
  
  if((((g.goban)->get_stone(place_left)==future_color_stone)
      &&((g.goban)->get_stone(place_right)!=future_color_stone))     
     ||(((g.goban)->get_stone(place_right)==future_color_stone)
	&&((g.goban)->get_stone(place_left)!=future_color_stone)))
    {
      /*
	if((g.goban)->get_stone(place_left)==future_color_stone)
	if(not (g.goban)->is_alone(place_left))
	return;
	
	if((g.goban)->get_stone(place_right)==future_color_stone)
	if(not (g.goban)->is_alone(place_right))
	return;
      */

      int gm = pair_to_int(height, good_move);
      if((not anti_stupid)||(not g.is_stupid(gm)))
	l_moves.push_back(gm);
    } 
}

void GE_Tactic::tsuke_hane0(GE_Game& g, int last_move, list<int>& l_moves, bool anti_stupid) const
{
  if(last_move<0) return;
  
  pair<int, int> lmv2 = int_to_pair((g.goban)->height, last_move);
  
  this->tsuke_hane0(g, lmv2, GE_NORTH, l_moves, anti_stupid);
  this->tsuke_hane0(g, lmv2, GE_SOUTH, l_moves, anti_stupid);
  this->tsuke_hane0(g, lmv2, GE_EAST, l_moves, anti_stupid);
  this->tsuke_hane0(g, lmv2, GE_WEST, l_moves, anti_stupid);
}


void GE_Tactic::kosumi_tsuke_stretch0(GE_Game& g, const pair<int, int>& lmv2, int dir, 
				   bool is_right, list<int>& l_moves, bool anti_stupid) const
{
  int& height = (g.goban)->height;
  int& width = (g.goban)->width;

  pair<int, int> support;
  pair<int, int> place_right;
  pair<int, int> good_move;
  
  int last_color_stone = (g.goban)->get_stone(lmv2);
  int future_color_stone = GE_COLOR_STONE_ENEMY(last_color_stone);
  
  switch(dir)
    {
    case GE_NORTH :
      if(is_right)
	{
	  support = make_pair(lmv2.first-1, lmv2.second-1);
	  place_right = make_pair(lmv2.first, lmv2.second+1);
	  good_move = make_pair(lmv2.first-1, lmv2.second+1);
	}
      else
	{
	  support = make_pair(lmv2.first-1, lmv2.second+1);
	  place_right = make_pair(lmv2.first, lmv2.second-1);
	  good_move = make_pair(lmv2.first-1, lmv2.second-1);
	}
      break;
      
    case GE_SOUTH :
      if(is_right)
	{
	  support = make_pair(lmv2.first+1, lmv2.second-1);
	  place_right = make_pair(lmv2.first, lmv2.second+1);
	  good_move = make_pair(lmv2.first+1, lmv2.second+1);
	}
      else
	{
	  support = make_pair(lmv2.first+1, lmv2.second+1);
	  place_right = make_pair(lmv2.first, lmv2.second-1);
	  good_move = make_pair(lmv2.first+1, lmv2.second-1);
	}
      break;

    case GE_EAST :
      if(is_right)
	{
	  support = make_pair(lmv2.first-1, lmv2.second+1);
	  place_right = make_pair(lmv2.first+1, lmv2.second);
	  good_move = make_pair(lmv2.first+1, lmv2.second+1);
	}
      else
	{
	  support = make_pair(lmv2.first+1, lmv2.second+1);
	  place_right = make_pair(lmv2.first-1, lmv2.second);
	  good_move = make_pair(lmv2.first-1, lmv2.second+1);
	}
      break;
      
    case GE_WEST :
      if(is_right)
	{
	  support = make_pair(lmv2.first-1, lmv2.second-1);
	  place_right = make_pair(lmv2.first+1, lmv2.second);
	  good_move = make_pair(lmv2.first+1, lmv2.second-1);
	}
      else
	{
	  support = make_pair(lmv2.first+1, lmv2.second-1);
	  place_right = make_pair(lmv2.first-1, lmv2.second);
	  good_move = make_pair(lmv2.first-1, lmv2.second-1);
	}
      break;
   
    default: assert(0);
    }
  
  if(not GE_IS_IN_GOBAN(good_move.first, good_move.second, height, width)) return;
  if((g.goban)->get_stone(good_move)!=GE_WITHOUT_STONE) return;
  if(not GE_IS_IN_GOBAN(support.first, support.second, height, width)) return;
  if((g.goban)->get_stone(support)!=last_color_stone) return;
  if(not GE_IS_IN_GOBAN(place_right.first, place_right.second, height, width)) return;
  if((g.goban)->get_stone(place_right)!=future_color_stone) return;
  
  int gm = pair_to_int(height, good_move);
  if((not anti_stupid)||(not g.is_stupid(gm)))
    l_moves.push_back(gm);
}




void GE_Tactic::kosumi_tsuke_stretch0(GE_Game& g, int last_move, list<int>& l_moves, bool anti_stupid) 
  const
{
  if(last_move<0) return;
  
  pair<int, int> lmv2 = int_to_pair((g.goban)->height, last_move);
  
  this->kosumi_tsuke_stretch0(g, lmv2, GE_NORTH, true, l_moves, anti_stupid);
  this->kosumi_tsuke_stretch0(g, lmv2, GE_NORTH, false, l_moves, anti_stupid);
  this->kosumi_tsuke_stretch0(g, lmv2, GE_SOUTH, true, l_moves, anti_stupid);
  this->kosumi_tsuke_stretch0(g, lmv2, GE_SOUTH, false, l_moves, anti_stupid);
  this->kosumi_tsuke_stretch0(g, lmv2, GE_EAST, true, l_moves, anti_stupid);
  this->kosumi_tsuke_stretch0(g, lmv2, GE_EAST, false, l_moves, anti_stupid);
  this->kosumi_tsuke_stretch0(g, lmv2, GE_WEST, true, l_moves, anti_stupid);
  this->kosumi_tsuke_stretch0(g, lmv2, GE_WEST, false, l_moves, anti_stupid);
}


void GE_Tactic::thrust_block0(GE_Game& g, const pair<int, int>& lmv2, int dir, list<int>& l_moves, 
			   bool anti_stupid) const
{
  int& height = (g.goban)->height;
  int& width = (g.goban)->width;
  
  int last_color_stone = (g.goban)->get_stone(lmv2);
  int future_color_stone = GE_COLOR_STONE_ENEMY(last_color_stone);
  
  pair<int, int> place_left;
  pair<int, int> place_right;
  pair<int, int> good_move;
  
  switch(dir)
    {
    case GE_NORTH :
      place_left = make_pair(lmv2.first, lmv2.second-1);
      place_right = make_pair(lmv2.first, lmv2.second+1);
      good_move = make_pair(lmv2.first-1, lmv2.second);
      break;
      
    case GE_SOUTH :
      place_left = make_pair(lmv2.first, lmv2.second-1);
      place_right = make_pair(lmv2.first, lmv2.second+1);
      good_move = make_pair(lmv2.first+1, lmv2.second);
      break;
      
    case GE_EAST :
      place_left = make_pair(lmv2.first-1, lmv2.second);
      place_right = make_pair(lmv2.first+1, lmv2.second);
      good_move = make_pair(lmv2.first, lmv2.second+1);
      break;

    case GE_WEST :
      place_left = make_pair(lmv2.first-1, lmv2.second);
      place_right = make_pair(lmv2.first+1, lmv2.second);
      good_move = make_pair(lmv2.first, lmv2.second-1);
      break;

    default : assert(0);
    }
  
  if(not GE_IS_IN_GOBAN(good_move.first, good_move.second, height, width)) return;
  if((g.goban)->get_stone(good_move)!=GE_WITHOUT_STONE) return;
  
  if(((not GE_IS_IN_GOBAN(place_left.first, place_left.second, height, width))
      ||((g.goban)->get_stone(place_left)==future_color_stone))
     &&((not GE_IS_IN_GOBAN(place_right.first, place_right.second, height, width))
	||((g.goban)->get_stone(place_right)==future_color_stone)))
    {
      int gm = pair_to_int(height, good_move);
      if((not anti_stupid)||(not g.is_stupid(gm)))
	l_moves.push_back(gm);
    }
}



void GE_Tactic::thrust_block0(GE_Game& g, int last_move, list<int>& l_moves, bool anti_stupid) const
{
  if(last_move<0) return;
  
  pair<int, int> lmv2 = int_to_pair((g.goban)->height, last_move);
  
  this->thrust_block0(g, lmv2, GE_NORTH, l_moves, anti_stupid);
  this->thrust_block0(g, lmv2, GE_SOUTH, l_moves, anti_stupid);
  this->thrust_block0(g, lmv2, GE_EAST, l_moves, anti_stupid);
  this->thrust_block0(g, lmv2, GE_WEST, l_moves, anti_stupid);
}




bool GE_Tactic::connect_kosumi(GE_Game& g, const pair<int, int>& loc2, int dir, 
			    int color_stone) const
{
  int& height = (g.goban)->height;
  int& width = (g.goban)->width;
  
  if((g.goban)->get_stone(loc2)!=GE_WITHOUT_STONE)
    return false;

  pair<int, int> place_left;
  pair<int, int> place_up;
  
  switch(dir)
    {
    case GE_NORTH :
      place_left = make_pair(loc2.first, loc2.second-1);
      place_up = make_pair(loc2.first-1, loc2.second);
      break;
      
    case GE_EAST :
      place_left = make_pair(loc2.first-1, loc2.second);
      place_up = make_pair(loc2.first, loc2.second+1);
      break;

    case GE_SOUTH :
      place_left = make_pair(loc2.first, loc2.second+1);
      place_up = make_pair(loc2.first+1, loc2.second);
      break;

    case GE_WEST :
      place_left = make_pair(loc2.first+1, loc2.second);
      place_up = make_pair(loc2.first, loc2.second-1);
      break;
      
    default : 
      ;
    }
   
   if(not GE_IS_IN_GOBAN(place_left.first, place_left.second, height, width))
     return false;

   if(not GE_IS_IN_GOBAN(place_up.first, place_up.second, height, width))
     return false;
   
   
   if((g.goban)->get_stone(place_left)==(g.goban)->get_stone(place_up))
     {
       if((g.goban)->get_stone(place_left)==color_stone)
	 { 
	   return true;
	 }
     }
   
   return false;
}




bool GE_Tactic::useless_connect_kosumi(GE_Game& g, const pair<int, int>& loc2, 
				    int dir, int color_stone, int& motif) 
  const
{
  int& height = (g.goban)->height;
  
  int enemy_color_stone = GE_COLOR_STONE_ENEMY(color_stone);
  pair<int, int> place_left_up;


  switch(dir)
    {
    case GE_NORTH :
      place_left_up = make_pair(loc2.first-1, loc2.second-1);
      break;
      
    case GE_EAST :
      place_left_up = make_pair(loc2.first-1, loc2.second+1);
      break;
      
    case GE_SOUTH :
      place_left_up = make_pair(loc2.first+1, loc2.second+1);
      break;
      
    case GE_WEST :
      place_left_up = make_pair(loc2.first+1, loc2.second-1);
      break;
      
    default : 
      ;
    }
 
  if((g.goban)->get_stone(place_left_up)==color_stone)
    {//dango
      motif = GE_MOTIF_DANGO;
      return true;
    }
   
  if((g.goban)->get_stone(place_left_up)==GE_WITHOUT_STONE)
    {//empty triangle
      motif = GE_MOTIF_EMPTY_TRIANGLE;
      return true;
    }

  if((g.goban)->get_stone(place_left_up)==enemy_color_stone)
    {
      GE_Chain ch;
      ch.init(*(g.goban), pair_to_int(height, place_left_up));
      if(ch.liberties==1)
	{
	  motif = GE_MOTIF_ATARI;
	  return true;
	}
    }

  





  return false;
}


bool GE_Tactic::connect_tobi(GE_Game& g, const pair<int, int>& loc2, int dir, 
			  int color_stone) const
{
  int& height = (g.goban)->height;
  int& width = (g.goban)->width;
  
  if((g.goban)->get_stone(loc2)!=GE_WITHOUT_STONE)
    return false;
  
  pair<int, int> place_left;
  pair<int, int> place_right;

  switch(dir)
    {
    case GE_NORTH :
      place_left = make_pair(loc2.first-1, loc2.second);
      place_right = make_pair(loc2.first+1, loc2.second);
      break;
      
    case GE_EAST :
      place_left = make_pair(loc2.first, loc2.second-1);
      place_right = make_pair(loc2.first, loc2.second+1);
      break;
      
    default : 
      ;
    }
  
  if(not GE_IS_IN_GOBAN(place_left.first, place_left.second, height, width))
    return false;
  
  if(not GE_IS_IN_GOBAN(place_right.first, place_right.second, height, width))
    return false;
   
   
  if((g.goban)->get_stone(place_left)==(g.goban)->get_stone(place_right))
    {
      if((g.goban)->get_stone(place_left)==color_stone)
	{ 
	  return true;
	}
    }
   
   return false;
}

bool GE_Tactic::useless_connect_tobi(GE_Game& g, const pair<int, int>& loc2, 
				  int dir, int color_stone, int& motif) 
  const
{
  return false;
}


void GE_Tactic::motif_to_string(int motif, string& msg) const
{
  switch(motif)
    {
    case GE_MOTIF_ATARI : 
      msg = "atari";
      break;
      
    case GE_MOTIF_EMPTY_TRIANGLE : 
      msg = "empty triangle";
      break;
      
    case GE_MOTIF_DANGO :
      msg = "dango";
      break;

    case GE_MOTIF_SELF_ATARI :
      msg = "self atari";
      break;

    default: ;
    }
}



void GE_Tactic::death_to_string(int motif_death, string& msg)
{
  switch(motif_death)
    {
    case GE_DEAD :
      msg = "dead";
      break;
      
    case GE_DEAD_BY_STATS :
      msg = "dead by stats";
      break;
      
    case GE_DEAD_BY_PRISONER :
      msg = "dead by prisoner";
      break;
      
    case GE_DEAD_BY_SNAPBACK :
      msg = "dead by snapback";
      break;
      
    case GE_DEAD_BY_IMMEDIATE_CAPTURE :
      msg = "dead by immediate capture";
      break;
      
    case GE_DEAD_BY_SHISHO :
      msg = "dead by shisho";
      break;
      
    case GE_DEAD_BY_KILL :
      msg = "dead by kill";
      break;
      
    case GE_KILL :
      msg = "kill";
      break;
      
    case GE_KILL_BY_IMMEDIATE_CAPTURE :
      msg = "kill by immediate capture";
      break;
      
    case GE_KILL_BY_SHISHO :
      msg = "kill by shisho";
      break;
      

    case GE_UNKNOWN_DEATH_OR_LIFE :
      msg = "unknown death or life";
      break;
      
    case GE_MAYBE_DEAD_OR_MAYBE_ALIVE :
      msg = "maybe alive or maybe dead";
      break;
      
    case GE_MAYBE_ALIVE_PERHAPS_SEKI :
      msg = "maybe alive perhaps seki";
      break;
      
    case GE_ALIVE_BY_STATS :
      msg = "alive by stats";
      break;
      
    case GE_SAVE :
      msg = "save";
      break;

    case GE_SAVE_BY_CONNEXION_WITH_ALIVE_GROUP :
      msg = "save by connexion with alive group";
      break;

    case GE_SAVE_BY_CONNEXION_ONE_EYE :
      msg = "save by connexion one eye";
      break;
      
    case GE_KILL_SAVE :
      msg = "kill or save";
      break;
      
    case GE_ALIVE_BUT_KO_THREAT :
      msg = "alive but ko threat";
      break;
      
    case GE_ALIVE :
      msg = "alive";
      break;
      
    case GE_ALIVE_BY_CONNEXION_UNKILLABLE_GROUP :
      msg = "alive by connexion unkillable group";
      break;
            
    case GE_ALIVE_BY_SOLID_CONNEXION_UNKILLABLE_GROUP :
      msg = "alive by solid connexion unkillable group";
      break;

    case GE_ALIVE_BY_CONNEXION_ALIVE_GROUP :
      msg = "alive by connexion alive group";
      break;
      
    case GE_ALIVE_BY_SOLID_CONNEXION_ALIVE_GROUP :
      msg = "alive by solid connexion alive group";
      break;

    case GE_ALIVE_BY_CONNEXION_WITH_DIFFERENT_ALIVE_GROUPS :
      msg = "alive by connexion with different alive groups";
      break;
      
    case GE_ALIVE_BY_DIRECT_CONNEXION_TWO_EYES :
      msg = "alive by direct connexion two eyes";
      break;

    case GE_ALIVE_BY_CONNEXION_TWO_EYES :
      msg = "alive by connexion two eyes";
      break;
      
    case GE_ALIVE_BY_CONNEXION_DIFFERENT_ONE_EYES :
      msg = "alive by connexion different one eyes";
      break;

    case GE_ALIVE_BY_TACTIC_CONNEXION_UNKILLABLE_GROUP :
      msg = "alive by tactic connexion unkillable group";
      break;

      
    case GE_ALIVE_BY_FALSE_WALL_CONNEXION_UNKILLABLE_GROUP :
      msg = "alive by false wall connexion unkillable group";
      break;

    case GE_ALIVE_BY_TACTIC_CONNEXION_ALIVE_GROUP :
      msg = "alive by tactic connexion alive group";
      break;

    case GE_ALIVE_BY_FALSE_WALL_CONNEXION_ALIVE_GROUP :
      msg = "alive by false wall connexion alive group";
      break;

    case GE_ALIVE_BY_CAPTURE_NEIGHBOUR :
      msg = "alive by capture neighbour";
      break;

    case GE_ALIVE_BY_DIRECT_TACTIC_CONNEXION_TWO_EYES :
      msg = "alive by direct tactic connexion two eyes";
      break;
      
    case GE_ALIVE_BY_DIRECT_FALSE_WALL_CONNEXION_TWO_EYES :
      msg = "alive by direct false wall connexion two eyes";
      break;
      
    case GE_ALIVE_BY_SPACE :
      msg = "alive by space";
      break;
      
    case GE_UNKILLABLE :
      msg = "unkillable";
      break;
      
    case GE_UNKILLABLE_BY_STATS :
      msg = "unkillable by stats";
      break;
      
    case GE_UNKILLABLE_BY_TWO_EYES :
      msg = "unkillable by two eyes";
      break;
      
    case GE_UNKILLABLE_WITH_ANOTHER_GROUP :
      msg = "unkillable with another group";
      break;
      
    default: assert(0);
    }
}



void GE_Tactic::hole_to_string(int motif_hole, string& msg)
{
  switch(motif_hole)
    {
    case GE_HOLE : msg = "hole"; break;
    case GE_TRUE_EYE : msg = "true eye"; break;
    case GE_NO_FALSE_EYE : msg = "no false eye"; break;
    case GE_FALSE_EYE : msg = "false eye"; break;
    case GE_BIG_HOLE : msg = "big hole"; break;
    case GE_MIX_EYE : msg = "mix eye"; break; 
    default: ;
    }
}

void GE_Tactic::mobility_to_string(int motif_mobility, string& msg)
{
  switch(motif_mobility)
    {
    case GE_UNSTATIONARY : msg = "unstationary"; break;
    case GE_STATIONARY_BY_KILL : msg = "stationary by kill"; break;
    default: ;
    }
}



void GE_Tactic::connect_to_string(int motif_connection, string& msg)
{
  switch(motif_connection)
    {
      
    case GE_CONNECT_BY_CAPTURE_COMMON_NEIGHBOUR : 
      msg = "connect by capture common neighbour"; 
      break;
      
    case GE_CONNECT_BY_CAPTURE_DISCONNECTION :
      msg = "connect by capture disconnection"; 
      break;
      
    case GE_CONNECT_BY_IMPOSSIBLE_PLAY :
      msg = "connect by impossible play"; 
      break;
      
      
    case GE_NO_CONNEXION :
      msg = "no connexion";
      break;
      
    case GE_CONNEXION :
      msg = "connexion";
      break;
      
    case GE_SOLID_VIRTUAL_CONNEXION :
      msg = "solid virtual connexion";
      break;
      
    case GE_VIRTUAL_CONNEXION :
      msg = "virtual connexion";
      break;
      
    case GE_TACTIC_CONNEXION :
      msg = "tactic connexion";
      break;
      
    case GE_FALSE_WALL_CONNEXION :
      msg = "false wall connexion";
      break;

    case GE_POSSIBLE_CONNEXION :
      msg = "possible connexion";
      break;
      
      
    default: ;
    }
  
}


void GE_Tactic::result_to_string(int result, string& msg)
{
  switch(result)
    {
    case GE_WIN : 
      msg = "win";
      break;
      
    case GE_WIN_WITH_KO : 
      msg = "win with ko";
      break;
      
    case GE_WIN_WITH_SEKI : 
      msg = "win with seki";
      break;
      
    case GE_DRAW :
      msg = "draw";
      break;
      
    case GE_LOSS_WITH_SEKI :
      msg = "loss with seki";
      break;
      
    case GE_LOSS_WITH_KO :
      msg = "loss with ko";
      break;
      
    case GE_LOSS :
      msg = "loss";
      break;
      
    case GE_NO_RESULT :
      msg = "no result";
      break;
      
    default: ;
    }
}


void GE_Tactic::problem_to_string(int pb, string& msg)
{  
  switch(pb)
    {
    case GE_GOAL_SEMEAI : 
      msg = "semeai";
      break;
      
    case GE_GOAL_DEATH_AND_LIFE : 
      msg = "death and life";
      break;
      
    case GE_GOAL_CONNECT :
      msg = "connect";
      break;
      
    default : assert(0);
    }
}


//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

bool GE_Tactic::study_shisho_after_move2(GE_Game& g, int mv, list<int>* l_moves) 
  const
{
  if(not g.play(mv))
    return true;
  
  bool rep = study_shisho2(g, mv, l_moves);
  g.backward();
  return rep;
}


/********
 * %%%% *
 * %@@@ *
 * %@.. * 
 * %@O@ *
 * %@O@ *
 * %.@O *
 * %... *
 ********/

//TO DEBUG this function must be tested
bool GE_Tactic::study_shisho2(GE_Game& g, int stone, list<int>* l_moves) const
{
  ko_for_attack = false;
  ko_for_defense = false; 
  sequence_shisho.clear();

  if(stone==GE_PASS_MOVE) return false;
  
  list<int> l_captures;
  GE_Chain ch;
  ch.init(*(g.goban), stone);
  
  GE_Chains chs;
  chs.neighbour_chains(*(g.goban), ch);
  
  
  list<GE_Chain>::const_iterator i_ch = chs.sets.begin();
  
  while(i_ch!=chs.sets.end())
    {
      if(i_ch->is_atari())
	{
	  if((g.goban)->get_stone((i_ch->links).front())
	     !=(g.goban)->get_stone(stone))
	    {
	      l_captures.push_back((i_ch->free_intersections).front());
	      
	      
	      //if(i_ch->size()>1) return false;
	    }
	}

      i_ch++;
    }
  
  //GE_ListOperators::print_error(l_captures, move_to_string, GE_Directions::board_size);
  
  return study_shisho2(g, stone, ch, l_moves, l_captures);
}


//**********************************************************************


void GE_Tactic::prepare_new_defenses_for_shisho(const GE_Game& g, 
					     const list<int>& l_captures, 
					     int def, 
					     list<int>& next_defenses, 
					     int connected) const
{
  int color_def = (g.goban)->get_stone(def);
  int nb_friends = (g.goban)->get_nb_neighbours(def, color_def);
  
  next_defenses = l_captures;
  GE_Chains chs;
  
  if(nb_friends==1)
    { 
      if(connected>=0)
	chs.neighbour_chains(*(g.goban), def);
      else 
	{
	  GE_Chain ch_def;
	  ch_def.init(*(g.goban), def);
	  chs.neighbour_chains(*(g.goban), ch_def);
	}
    }
  else
    {
      if(nb_friends==0)
	chs.neighbour_chains(*(g.goban), def);
      else
	{
	  GE_Chain ch_def;
	  ch_def.init(*(g.goban), def);
	  
	  //TODO nb_friends==2
	  
	  chs.neighbour_chains(*(g.goban), ch_def);
	}
    }
  
  //chs.neighbour_chains(*(g.goban), def);
  //chs.neighbour_chains(*(g.goban), ch_def);
  
  list<GE_Chain>::const_iterator i_ch = chs.sets.begin();
  
  while(i_ch!=chs.sets.end())
    {
      if(i_ch->is_atari())
	{
	  if((g.goban)->get_stone((i_ch->links).front())
	     !=(g.goban)->get_stone(def))
	    {
	      const int& liberty = (i_ch->free_intersections).front();
	      
	      GE_ListOperators::push_if_not(next_defenses, liberty);
	      
	      //next_defenses
	      //.push_back((i_ch->free_intersections).front());
	    }
	}
      
      i_ch++;
    }  
}



//WARNING Color of the stone and the player
//TO IMPLEMENT: another possibility to save is to take (EAT OR TO BE EATEN)
bool GE_Tactic::study_shisho2(GE_Game& g, int stone, GE_Chain& ch, list<int>* l_moves, 
			   const list<int>& l_captures, int prof) const 
{
  if(prof>max_depth)
    {
      cerr<<"too depth"<<endl;
      return false;
    }
  
  
  list<int> l_capt2 = l_captures;
  
  
  if(ch.free_intersections.size()>2) return false;
  if(ch.free_intersections.size()<=1) 
    {
      //GE_ListOperators::print_error(sequence_shisho, move_to_string, 
      //GE_Directions::board_size);
      //cerr<<endl;

      return true;
    }

  list<int> l_rep = ch.free_intersections;
  
  while(not l_rep.empty())
    {
      int att = l_rep.front();
      l_rep.pop_front();
      
      if(not g.play(att)) continue;

      if(g.has_taken_ko())
	{
	  ko_for_attack = true;
	  //cerr<<"*** shisho *** ko for attack"<<endl; 
	}
      
      sequence_shisho.push_back(att);
      GE_ListOperators::erase(ch.free_intersections, att);
      
      int self_atari = GE_PASS_MOVE;
      GE_Chain ch_att;
      ch_att.init(*(g.goban), att);
      if(ch_att.is_atari())
	{
	  self_atari = ch_att.free_intersections.back();
	  l_capt2.push_back(self_atari);
	}
      
      
      if(ch.free_intersections.size()!=1)
	{
	  cout<<"*** ERROR *** study_shisho2(GE_Game&, int, GE_Chain&, list<int>*)"<<endl;
	  cout<<"   --> the size of ch.free_intersections is different of 1"<<endl;
	  exit(-1);
	}
      
      int def = ch.free_intersections.back();
      bool is_good_shisho = true;
      
      if(g.play(def))
	{
	  sequence_shisho.push_back(def);
	  GE_ListOperators::erase(ch.free_intersections, def);
	  
	  list<int> liberties;
	  int nb_liberties = (g.goban)->get_nb_liberties(def, &liberties);
	  
	  if(nb_liberties>2)
	    {
	      g.backward();
	      g.backward();
	      sequence_shisho.pop_back();
	      sequence_shisho.pop_back();
	      ch.free_intersections.push_back(att);
	      ch.free_intersections.push_back(def);
	      continue;
	    }
	  
	  
	  list<int> next_defenses;
	  prepare_new_defenses_for_shisho(g, l_capt2, def, next_defenses, 
					  stone);
	  
	  
	  //ch.free_intersections = liberties; 
	  //with the same color of stone, it is a mistake
	  //because we have new connection
	  GE_Chain ch2; ch2.init(*(g.goban), stone);
	  is_good_shisho = GE_Tactic::study_shisho2(g, stone, ch2, 0, 
						 next_defenses, prof+1);
	  ch.free_intersections.clear();
	  g.backward();
	  g.backward();
	  sequence_shisho.pop_back();
	  sequence_shisho.pop_back();
	  ch.free_intersections.push_back(att);
	  ch.free_intersections.push_back(def);
	}
      else
	{
	  g.backward();
	  sequence_shisho.pop_back();
	  ch.free_intersections.push_back(att);
	}
      
      
      if(not is_good_shisho) continue;
      
      bool is_good_shisho_after_capture = true;
      
      g.play(att); 
      //("has taken ko" is already detected) 
      
      sequence_shisho.push_back(att);
      
      list<int>::const_iterator i_capt = l_captures.begin();
      while(i_capt!=l_captures.end())
	{
	  if(not g.play(*i_capt))
	    {
	      i_capt++;
	      continue;
	    }
	  
	  sequence_shisho.push_back(*i_capt);
	  
	  //new defenses
	  list<int> next_defenses;
	  prepare_new_defenses_for_shisho(g, l_capt2, *i_capt, next_defenses);
	  
	  
	  GE_Chain ch3; ch3.init(*(g.goban), stone);
	  is_good_shisho_after_capture = 
	    GE_Tactic::study_shisho2(g, stone, ch3, 0, next_defenses, prof+1); 
	  g.backward();
	  sequence_shisho.pop_back();
	  i_capt++;
	  
	  if(not is_good_shisho_after_capture)
	    break;
	}
      
      if((is_good_shisho_after_capture)&&(self_atari!=GE_PASS_MOVE))
	{
	  if(g.play(self_atari))
	    {
	      sequence_shisho.push_back(self_atari);
	      
	      
	      //new defenses
	      list<int> next_defenses;
	      prepare_new_defenses_for_shisho(g, l_capt2, self_atari, 
					      next_defenses);
	      
	      GE_Chain ch3; ch3.init(*(g.goban), stone);
	      is_good_shisho_after_capture = 
		GE_Tactic::study_shisho2(g, stone, ch3, 0, next_defenses, prof+1); 
	      
	      g.backward();
	      sequence_shisho.pop_back();
	    }
	  
	}
      
      g.backward();
      sequence_shisho.pop_back();
      
      
      if(is_good_shisho_after_capture)
	{
	  if(l_moves)
	    {
	      l_moves->push_back(att);
	      continue;
	    }
	  
	  return true;
	}
    }
  
  if(l_moves) return (not l_moves->empty());
  
  return false;  
  
}
  



//**********************************************************************



// //WARNING Color of the stone and the player
// //TO IMPLEMENT: another possibility to save is to take (EAT OR TO BE EATEN)
// bool GE_Tactic::study_shisho2(GE_Game& g, int stone, GE_Chain& ch, list<int>* l_moves, 
// 			   const list<int>& l_captures) const 
// {
//   list<int> l_capt2 = l_captures;
  

//   if(ch.free_intersections.size()>2) return false;
//   if(ch.free_intersections.size()<=1) 
//     {
//       //GE_ListOperators::print_error(sequence_shisho, move_to_string, 
//       //			 GE_Directions::board_size);
//       //cerr<<endl;

//       return true;
//     }

//   list<int> l_rep = ch.free_intersections;
  
//   while(not l_rep.empty())
//     {
//       int att = l_rep.front();
//       l_rep.pop_front();
      
//       if(not g.play(att)) continue;
//       sequence_shisho.push_back(att);
//       GE_ListOperators::erase(ch.free_intersections, att);
      
//       int self_atari = GE_PASS_MOVE;
//       GE_Chain ch_att;
//       ch_att.init(*(g.goban), att);
//       if(ch_att.is_atari())
// 	{
// 	  self_atari = ch_att.free_intersections.back();
// 	  l_capt2.push_back(self_atari);
// 	}
      
      
//       if(ch.free_intersections.size()!=1)
// 	{
// 	  cout<<"*** ERROR *** study_shisho2(GE_Game&, int, GE_Chain&, list<int>*)"<<endl;
// 	  cout<<"   --> the size of ch.free_intersections is different of 1"<<endl;
// 	  exit(-1);
// 	}
      
//       int def = ch.free_intersections.back();
      
//       if(not g.play(def))
// 	{
// 	  bool is_good_shisho_after_capture = true;
	  
// 	  list<int>::const_iterator i_capt = l_captures.begin();
// 	  while(i_capt!=l_captures.end())
// 	    {
// 	      if(not g.play(*i_capt))
// 		{
// 		  i_capt++;
// 		  continue;
// 		}
	      
// 	      sequence_shisho.push_back(*i_capt);
	      
	      
// 	      GE_Chain ch3; ch3.init(*(g.goban), stone);
// 	      is_good_shisho_after_capture = 
// 		GE_Tactic::study_shisho2(g, stone, ch3, 0, l_capt2); //l_capture ?????
	      
// 	      g.backward();
// 	      sequence_shisho.pop_back();
// 	      i_capt++;
	      
	      
// 	      if(not is_good_shisho_after_capture)
// 		break;
// 	    }
	  
// 	  if((is_good_shisho_after_capture)&&(self_atari!=GE_PASS_MOVE))
// 	    {
// 	      if(g.play(self_atari))
// 		{
// 		  sequence_shisho.push_back(self_atari);
		  
// 		  GE_Chain ch3; ch3.init(*(g.goban), stone);

// 		  is_good_shisho_after_capture = 
// 		    GE_Tactic::study_shisho2(g, stone, ch3, 0, l_capt2); 
		  
// 		  g.backward();
// 		  sequence_shisho.pop_back();
// 		} 
// 	    }
	  
// 	  g.backward();
// 	  sequence_shisho.pop_back();
// 	  ch.free_intersections.push_back(att);
	  
// 	  if(is_good_shisho_after_capture)
// 	    {
// 	      if(l_moves)
// 		{
// 		  l_moves->push_back(att);
// 		  continue; 
// 		}
	      
// 	      return true;
// 	    }
	  
// 	  continue;
// 	}
      
//       sequence_shisho.push_back(def);
//       GE_ListOperators::erase(ch.free_intersections, def);
      
//       list<int> liberties;
//       int nb_liberties = (g.goban)->get_nb_liberties(def, &liberties);
      
//       if(nb_liberties>2)
// 	{
// 	  g.backward();
// 	  g.backward();
// 	  sequence_shisho.pop_back();
// 	  sequence_shisho.pop_back();
// 	  ch.free_intersections.push_back(att);
// 	  ch.free_intersections.push_back(def);
// 	  continue;
// 	}
      
//       //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//       GE_Chains chs;
//       chs.neighbour_chains(*(g.goban), def);
      
      
//       list<GE_Chain>::const_iterator i_ch = chs.sets.begin();
      
//       while(i_ch!=chs.sets.end())
// 	{
// 	  if(i_ch->is_atari())
// 	    {
// 	      if((g.goban)->get_stone((i_ch->links).front())
// 		 !=(g.goban)->get_stone(def))
// 		{
// 		  l_capt2.push_back((i_ch->free_intersections).front());
		  
		  
// 		  //if(i_ch->size()>1) return false;
// 		}
// 	    }
	  
// 	  i_ch++;
// 	}
//       //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      
//       /*
// 	if(nb_liberties<2)
// 	{
// 	g.backward();
// 	g.backward();
// 	ch.free_intersections.push_back(att);
// 	ch.free_intersections.push_back(def);
// 	if(l_moves)
// 	{
// 	l_moves->push_back(att);
// 	continue;
// 	}
	
// 	return true;
// 	} 
//       */
//       //nb_liberties==2
      
      
//       //ch.free_intersections = liberties; //with the same color of stone, it is a mistake
//       //because we have new connection
//       GE_Chain ch2; ch2.init(*(g.goban), stone);
//       bool is_good_shisho = GE_Tactic::study_shisho2(g, stone, ch2, 0, l_capt2);
//       ch.free_intersections.clear();
//       g.backward();
//       g.backward();
//       sequence_shisho.pop_back();
//       sequence_shisho.pop_back();
//       ch.free_intersections.push_back(att);
//       ch.free_intersections.push_back(def);
      
//       if(is_good_shisho)
// 	{
// 	  bool is_good_shisho_after_capture = true;
	  
// 	  g.play(att);
// 	  sequence_shisho.push_back(att);

// 	  list<int>::const_iterator i_capt = l_captures.begin();
// 	  while(i_capt!=l_captures.end())
// 	    {
// 	      if(not g.play(*i_capt))
// 		{
// 		  i_capt++;
// 		  continue;
// 		}
	      
// 	      sequence_shisho.push_back(*i_capt);
	      
// 	      GE_Chain ch3; ch3.init(*(g.goban), stone);
// 	      is_good_shisho_after_capture = 
// 		GE_Tactic::study_shisho2(g, stone, ch3, 0, l_capt2); //l_capture ?????
	      
// 	      g.backward();
// 	      sequence_shisho.pop_back();
// 	      i_capt++;
	      
// 	      if(not is_good_shisho_after_capture)
// 		break;
// 	    }

// 	  if((is_good_shisho_after_capture)&&(self_atari!=GE_PASS_MOVE))
// 	    {
// 	      if(g.play(self_atari))
// 		{
// 		  sequence_shisho.push_back(self_atari);
		  
// 		  GE_Chain ch3; ch3.init(*(g.goban), stone);
// 		  is_good_shisho_after_capture = 
// 		    GE_Tactic::study_shisho2(g, stone, ch3, 0, l_capt2); 
		  
// 		  g.backward();
// 		  sequence_shisho.pop_back();
// 		}
	      
// 	    }
	  
	  

// 	  g.backward();
// 	  sequence_shisho.pop_back();
	  
	  
// 	  if(is_good_shisho_after_capture)
// 	    {
// 	      if(l_moves)
// 		{
// 		  l_moves->push_back(att);
// 		  continue;
// 		}
	      
// 	      return true;
// 	    }
// 	}
      
//     }
  
//   if(l_moves) return (not l_moves->empty());
  
//   return false;  
// }

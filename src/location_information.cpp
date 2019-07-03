#include "location_information.h"

GE_IntersectionInformation::GE_IntersectionInformation(int loc)
{
  is_alive = GE_MAYBE;
  possible_white_eye = GE_MAYBE;
  possible_black_eye = GE_MAYBE;
  intersection = loc;
}

bool GE_IntersectionInformation::is_alive_by_pass(GE_Game& game, int location, int nb_simulations, 
					       bool init)
{
  intersection = location;
  
  int color_stone = (game.goban)->get_stone(location);
  int stay_color = 0;
  
  int color_pass;
  
  if(color_stone==GE_WHITE_STONE)
    color_pass = GE_WHITE;
  else color_pass = GE_BLACK;
  
  
  int num_sim = 0;
  while(num_sim<nb_simulations)
    {
      int nb_loops = 0;
      while(true)
	{
	  if(game.tc==color_pass)
	    game.play(GE_PASS_MOVE);
	  else
	    {
	      list<int> temp_moves;
	      get_moves_by_random(game, temp_moves, 1);
	      if((temp_moves.empty())||(not game.play(temp_moves.front())))
		{
		  if((game.goban)->get_stone(location)==color_stone)
		    stay_color++;
		  
		  break;
		}
	    }
	  
	  nb_loops++;	  
	}
      
      while(nb_loops>0)
	{
	  game.backward();
	  nb_loops--;
	}
      
      (game.goban)->print(game.get_last_move());
      
      num_sim++;
    }
  
  if(init)
    {
      if(stay_color==nb_simulations)
	is_alive = GE_TRUE;
      else is_alive = GE_MAYBE;
    }
  
  if(stay_color==nb_simulations)
    return true;
  else return false;
}

bool GE_IntersectionInformation::is_fit_for_slaughter(GE_Game& g, int location, int nb_simulations)
{
  return not is_alive_by_pass(g, location, nb_simulations, false);
}

bool GE_IntersectionInformation::is_dead(GE_Game& g, int location)
{
  if((g.goban)->get_stone(location)==GE_WITHOUT_STONE) return false;
  
  return not is_alive_by_pass(g, location, 3, false);
}


void GE_IntersectionInformation::init_possible_eye(GE_Game& g)
{
  int location = intersection;
  switch((g.goban)->get_stone(location))
    {
    case GE_WHITE_STONE : 
      possible_white_eye = GE_FALSE;
      possible_black_eye = GE_TRUE; //except if the stone is alive
      return;
      
    case GE_BLACK_STONE : 
      possible_white_eye = GE_TRUE; //except if the stone is alive
      possible_black_eye = GE_FALSE; 
      return;
      
    default: ;
    }
  
  if((g.goban)->get_nb_neighbours(location, GE_WHITE_STONE)>0)
    possible_black_eye = GE_FALSE;
  else possible_black_eye = GE_TRUE;

  if((g.goban)->get_nb_neighbours(location, GE_BLACK_STONE)>0)
    possible_white_eye = GE_FALSE;
  else possible_white_eye = GE_TRUE;  
}

void 
GE_IntersectionInformation::refine_possible_eye
(GE_Game& g, const vector<GE_GroupInformation*>& groups, 
 const vector<int>& to_num_group)
{
  if((possible_black_eye==GE_TRUE)
     &&((g.goban)->get_stone(intersection)==GE_WHITE_STONE))
    {
      int num_group = to_num_group[intersection];
      if((groups[num_group])->status>=GE_ALIVE)
	{
	  possible_black_eye = GE_FALSE;
	}
    }

  if((possible_white_eye==GE_TRUE)
     &&((g.goban)->get_stone(intersection)==GE_BLACK_STONE))
    {
      int num_group = to_num_group[intersection];
      if((groups[num_group])->status>=GE_ALIVE)
	{
	  possible_white_eye = GE_FALSE;
	}
    }
  
  int big_location = GE_Bibliotheque::to_big_goban[intersection];
  
  if(possible_black_eye==GE_TRUE)
    {
      int nb_alive = 0;
      bool is_on_border = false;
      for(int dir = GE_NORTHEAST; dir<=GE_SOUTHWEST; dir++)
	{
	  int big_location_dir = big_location+GE_Directions::directions[dir]; 
	  int location_dir = GE_Bibliotheque::to_goban[big_location_dir];
  
	  if(location_dir<0)
	    {
	      is_on_border = true;
	      continue;
	    }
	  
	  if((g.goban)->get_stone(location_dir)==GE_WHITE_STONE)
	    {
	      int num_group = to_num_group[location_dir];
	      
	      if((groups[num_group])->status>=GE_ALIVE)
		nb_alive++;
	    }
	}
      
      if(nb_alive>=2)
	possible_black_eye = GE_FALSE;
      else
	{
	  if((nb_alive==1)&&(is_on_border))
	    possible_black_eye = GE_FALSE;
	}
    }

  if(possible_white_eye==GE_TRUE)
    {
      int nb_alive = 0;
      bool is_on_border = false;
      for(int dir = GE_NORTHEAST; dir<=GE_SOUTHWEST; dir++)
	{
	  int big_location_dir = big_location+GE_Directions::directions[dir]; 
	  int location_dir = GE_Bibliotheque::to_goban[big_location_dir];
	  
	  if(location_dir<0)
	    {
	      is_on_border = true;
	      continue;
	    }
	  
	  if((g.goban)->get_stone(location_dir)==GE_BLACK_STONE)
	    {
	      int num_group = to_num_group[location_dir];
	      
	      if((groups[num_group])->status>=GE_ALIVE)
		nb_alive++;
	    }
	}
      
      if(nb_alive>=2)
	possible_white_eye = GE_FALSE;
      else
	{
	  if((nb_alive==1)&&(is_on_border))
	    possible_white_eye = GE_FALSE;
	}
    }
  
}
  



void GE_IntersectionInformation::print_error() const
{
  string s_location = "";
  s_location = move_to_string(GE_Directions::board_size, intersection);
  s_location = s_location+" ";
  switch(possible_black_eye)
    {
    case GE_TRUE : s_location += "PBE "; break;
    case GE_FALSE : s_location += "NBE"; break;
    case GE_MAYBE : s_location += "MBE"; break;
    default : ;
    }
  
  s_location+=" ";
  
  switch(possible_white_eye)
    {
    case GE_TRUE : s_location += "PWE "; break;
    case GE_FALSE : s_location += "NWE"; break;
    case GE_MAYBE : s_location += "MWE"; break;
    default : ;
    }
  
  cerr<<s_location<<endl;
  
}




/****************************
 ****************************
 **                        **        
 ** Information on a group **
 **                        **
 ****************************
 ****************************/


GE_GroupInformation::GE_GroupInformation(int indice, const GE_Chain& chain)
{
  ch = chain;
  index = indice;
  stats_life = -1;
  nb_eyes = -1;
  status = GE_UNKNOWN_DEATH_OR_LIFE;
  motif_dead = GE_MAYBE_DEAD_OR_MAYBE_ALIVE;
  motif_alive = GE_MAYBE_DEAD_OR_MAYBE_ALIVE;
  mobility = GE_UNSTATIONARY;
}


int GE_GroupInformation::set_status(int status)
{
  double old_status = this->status;
  this->status = status;
  return old_status;
}


int GE_GroupInformation::get_status(int motif) const
{
  if(motif<0)
    return status;
  
  return (motif/GE_DEAD)*GE_DEAD;
}

int GE_GroupInformation::set_motif_status(int motif)
{
  int class_motif = get_status(motif);
  int old_motif = -1;
  
  switch(class_motif)
    {
    case GE_DEAD : 
      old_motif = motif_dead;
      motif_dead = motif;
      motif_alive = class_motif;
      set_status(class_motif);
      return old_motif;
      
    case GE_KILL : 
      old_motif = motif_dead;
      motif_dead = motif;
      if(status==GE_SAVE)
	set_status(GE_KILL_SAVE);
      else set_status(GE_KILL);
      return old_motif;
      
    case GE_KILL_SAVE : 
      assert(0);
      break;
      
    case GE_SAVE : 
      old_motif = motif_alive;
      motif_alive = motif;
      if(status==GE_KILL)
	set_status(GE_KILL_SAVE);
      else set_status(GE_SAVE);
      return old_motif;
      
    case GE_ALIVE :
      old_motif = motif_alive;
      motif_alive = motif;
      motif_dead = class_motif;
      set_status(class_motif);
      return old_motif;
      
    case GE_UNKILLABLE :
      old_motif = motif_alive;
      motif_alive = motif;
      motif_dead = class_motif;
      set_status(class_motif);
      return old_motif;
 
    case GE_UNKNOWN_DEATH_OR_LIFE :
      if(status!=GE_UNKNOWN_DEATH_OR_LIFE)
	assert(0);

      
      old_motif = motif_alive;
      motif_alive = motif;
      motif_dead = GE_UNKILLABLE;
      set_status(class_motif);
      return old_motif;

     
    case GE_ALIVE_BUT_KO_THREAT : break;
    case GE_DEAD_BUT_KO_THREAT : break;
    default : assert(0);
    }
  
  
  return -2;
}

double GE_GroupInformation::set_stats_life(double stats)
{
  double old_stats = stats_life;
  stats_life = stats;
  return old_stats;
}


int GE_GroupInformation::get_one_stone() const
{
  return ch.links.front();
}

int GE_GroupInformation::get_size() const
{
  return (int)ch.links.size();
}


int GE_GroupInformation::get_color_stone() const
{
  return ch.stone;
}

void GE_GroupInformation::append_neighbour_hole(int num_hole)
{
  neighbour_holes.push_back(num_hole);
}

void GE_GroupInformation::append_neighbour_group(int num_group)
{
  neighbour_groups.push_back(num_group);
}


void GE_GroupInformation::append_false_eye(int num_hole)
{
  false_eyes.push_back(num_hole);
}

void GE_GroupInformation::append_possible_connexion(int num_group, int location)
{
  possible_connexions.push_back(make_pair(num_group, location));
}

void GE_GroupInformation::append_false_wall_connexion(int num_group)
{
  false_wall_connexions.push_back(num_group);
}


void GE_GroupInformation::append_tactic_connexion(int num_group)
{
  tactic_connexions.push_back(num_group);
}

void GE_GroupInformation::append_virtual_connexion(int num_group)
{
  virtual_connexions.push_back(num_group);
}

void GE_GroupInformation::append_solid_virtual_connexion(int num_group)
{
  solid_virtual_connexions.push_back(num_group);
}


void GE_GroupInformation::get_all_connexions(vector<int>& all_connexions, 
					  int level_connexion)
  const
{
  
  list<int>::const_iterator ig = solid_virtual_connexions.begin();
  while(ig!=solid_virtual_connexions.end())
    {
      all_connexions.push_back(*ig);
      ig++;
    }
  
  
  if(level_connexion==GE_SOLID_VIRTUAL_CONNEXION) return;
  
  ig = virtual_connexions.begin();
  while(ig!=virtual_connexions.end())
    {
      all_connexions.push_back(*ig);
      ig++;
    }
  
  
  if(level_connexion==GE_VIRTUAL_CONNEXION) return;
  
   
  ig = tactic_connexions.begin();
  while(ig!=tactic_connexions.end())
    {
      all_connexions.push_back(*ig);
      ig++;
    }
  
  
  if(level_connexion==GE_TACTIC_CONNEXION) return;
  
  
  ig = false_wall_connexions.begin();
  while(ig!=false_wall_connexions.end())
    {
      all_connexions.push_back(*ig);
      ig++;
    }

  if(level_connexion==GE_FALSE_WALL_CONNEXION) return;
  
  
  list<pair<int, int> >::const_iterator ig2 = possible_connexions.begin();
  while(ig2!=possible_connexions.end())
    {
      all_connexions.push_back(ig2->first);
      ig2++;
    }
 
  if(level_connexion==GE_POSSIBLE_CONNEXION) return;
  
}

//KO ?


/*
  the question is :
  Can I play on one of my liberties of my group ?
*/
void GE_GroupInformation::init_mobility(GE_Game& g)
{
  if(ch.liberties==2)
    {
      bool generate_pass_move = false;
      if(((g.tc==GE_BLACK)&&(ch.stone!=GE_BLACK_STONE))
	 ||((g.tc==GE_WHITE)&&(ch.stone!=GE_WHITE_STONE)))
	{
	  g.play(GE_PASS_MOVE);
	  generate_pass_move = true;
	}
      
      list<int>::const_iterator i_mv = ch.free_intersections.begin();
      
      while(i_mv!=ch.free_intersections.end())
	{
	  if(not g.play(*i_mv))
	    assert(0);
	  
	  GE_Chain temp_ch;
	  temp_ch.init(*(g.goban), *i_mv);
	  g.backward();
	  
	  if(not temp_ch.is_atari())
	    {
	      if(generate_pass_move)
		g.backward();
	      return;
	    }
	  i_mv++;
	}
      
      
      if(generate_pass_move)
	g.backward();
      
      mobility = GE_STATIONARY_BY_KILL;
      return;
    }
}


//we assume the last move is not a pass move
bool GE_GroupInformation::study_alive(GE_Game& g)
{
  
  if(ch.liberties==1)
    {
      set_motif_status(GE_KILL_BY_IMMEDIATE_CAPTURE);
      kill_moves.push_back(ch.free_intersections.front());
      
      bool generate_pass_move = false;
      
      GE_Chains neighbours;
      neighbours.neighbour_chains(*(g.goban), ch);
      
      bool is_snapback = false; 
      list<GE_Chain>::const_iterator i_ch = neighbours.sets.begin();
      while(i_ch!=neighbours.sets.end())
	{
	  if((i_ch->stone!=ch.stone)&&(i_ch->is_atari()))
	    {
	      if(is_snapback)
		{
		  return true;
		}
	      
	      // snapback ?
	      
	      if(i_ch->size()>1)
		{
		  return true;
		}
	      
	      // i_ch->size==1
	      
	      if(i_ch->free_intersections.back()
		 !=ch.free_intersections.back())
		{
		  return true;
		}
	      
	      // prise en retour possible (snapback)
	      
	      is_snapback = true;
	      
	    }
	  
	  i_ch++;
	}
      
      if(((g.tc==GE_BLACK)&&(ch.stone!=GE_BLACK_STONE))
	 ||((g.tc==GE_WHITE)&&(ch.stone!=GE_WHITE_STONE)))
	{
	  g.play(GE_PASS_MOVE);
	  generate_pass_move = true;
	}
      
      if(not g.play(ch.free_intersections.back()))
	{ 
	  if(generate_pass_move)
	    g.backward();
	  
	  set_motif_status(GE_DEAD_BY_PRISONER);
	  return true;
	}
      
      
      GE_Chain temp_ch;
      temp_ch.init((*g.goban), ch.free_intersections.back());
      
      if(temp_ch.is_atari())
	{
	  if(is_snapback)
	    set_motif_status(GE_DEAD_BY_SNAPBACK);
	  else set_motif_status(GE_DEAD_BY_IMMEDIATE_CAPTURE);
	  
	  g.backward();
	  if(generate_pass_move) g.backward();
	  
	  return true;
	}
      
      GE_Tactic tact;
      int seed_chain = ch.links.front();
      bool is_shisho = tact.study_shisho2(g, seed_chain);
      g.backward();
      
      
      if(generate_pass_move)
	g.backward();
      
      if(is_shisho) set_motif_status(GE_DEAD_BY_SHISHO);
      
      return not is_shisho;
    }
  
  if(ch.liberties==2)
    {
      bool generate_pass_move = false;
      if(((g.tc==GE_BLACK)&&(ch.stone==GE_BLACK_STONE))
	 ||((g.tc==GE_WHITE)&&(ch.stone==GE_WHITE_STONE)))
	{
	  g.play(GE_PASS_MOVE);
	  generate_pass_move = true;
	}
      
      GE_Tactic tact;
      int seed_chain = ch.links.front();
      bool is_shisho = tact.study_shisho2(g, seed_chain);
      
      if(generate_pass_move)
	g.backward();
      
      if(is_shisho)
	{
	  set_motif_status(GE_KILL_BY_SHISHO);
	  return true;
	}      
    }
  
  
  return true;
}

bool GE_GroupInformation::study_alive_by_stats()
{
  if((status==GE_UNKNOWN_DEATH_OR_LIFE)
     &&(motif_alive==GE_MAYBE_DEAD_OR_MAYBE_ALIVE)
     &&(motif_dead==GE_MAYBE_DEAD_OR_MAYBE_ALIVE))
    {
      if(stats_life>=0)
	{
	  if(stats_life>=0.9)
	    {
	      set_motif_status(GE_ALIVE_BY_STATS);
	      return true;
	    }
	  
	  if(stats_life<=0.1)
	    {
	      set_motif_status(GE_DEAD_BY_STATS);
	      return false;
	    }
	}
    }
     
  return status>=GE_UNKNOWN_DEATH_OR_LIFE;
}

  
void GE_GroupInformation::print_error() const
{
  
  cerr<<"chain "<<index<<endl;
  ch.print_error();
  
  string msg_death;
  GE_Tactic::death_to_string(status, msg_death);
  cerr<<"status: "<<msg_death<<"   "<<"(";
  
  if(stats_life<0)
    cerr<<"no stats";
  else cerr<<(int)(stats_life*10000)/(double)100.<<"%";
  cerr<<")"<<endl;
  
  string msg_alive;
  GE_Tactic::death_to_string(motif_alive, msg_alive);
  cerr<<"alive: "<<msg_alive<<endl;

  string msg_dead;
  GE_Tactic::death_to_string(motif_dead, msg_dead);
  cerr<<"dead: "<<msg_dead<<endl;

  
  string msg_mobility;
  GE_Tactic::mobility_to_string(mobility, msg_mobility);
  cerr<<"mobility: "<<msg_mobility<<endl;
  
  
  cerr<<"eyes: ";
  if(nb_eyes<0) 
    cerr<<"unknown";
  else cerr<<nb_eyes;
  
  cerr<<endl;
  
  cerr<<"false eyes: ";
  if(false_eyes.empty())
    cerr<<"empty"<<endl;
  else GE_ListOperators::print_error(false_eyes);
  
  cerr<<"neighbour groups: ";
  if((int)neighbour_groups.size()==0)
    cerr<<"empty"<<endl;
  else GE_VectorOperators::print_error(neighbour_groups);
  
  cerr<<"possible connexion: ";
  if(possible_connexions.empty())
    cerr<<"empty"<<endl;
  else
    {
      list<pair<int, int> >::const_iterator i_pc = 
	possible_connexions.begin();
      while(i_pc!=possible_connexions.end())
	{
	  cerr<<i_pc->first<<" (";
	  cerr<<move_to_string(GE_Directions::board_size, i_pc->second);
	  cerr<<")  ";
	  i_pc++;
	}
      cerr<<endl;
    } 
  
  cerr<<"false wall connexion: ";
  if(false_wall_connexions.empty())
    cerr<<"empty"<<endl;
  else GE_ListOperators::print_error(false_wall_connexions);
  
  cerr<<"tactic connexion: ";
  if(tactic_connexions.empty())
    cerr<<"empty"<<endl;
  else GE_ListOperators::print_error(tactic_connexions);
  
  cerr<<"solid virtual connexion: ";
  if(solid_virtual_connexions.empty())
    cerr<<"empty"<<endl;
  else GE_ListOperators::print_error(solid_virtual_connexions);
  
  cerr<<"virtual connexion: ";
  if(virtual_connexions.empty())
    cerr<<"empty"<<endl;
  else GE_ListOperators::print_error(virtual_connexions);
  
  cerr<<endl;
  
  //list<int> virtually_connected;
  //list<int> seki;
  
  
  //  int is_alive;
  //   int nb_holes;
  //   int nb_eyes;
  
  //double stats_life;
}



/***************************
 ***************************
 **                       **        
 ** Information on a hole **
 **                       **
 ***************************
 ***************************/

void GE_HoleInformation::init(const GE_Game& g, const GE_Chain& empty_chain, 
			   const vector<GE_GroupInformation*>& groups, 
			   const vector<int>& to_num_group, int ind)
{
  index = ind;
  hole = empty_chain.links;
  
  list<int>* p_ch = 0;
  GE_Marker mk(GE_Directions::board_area);
  list<int>::const_iterator i_f = (empty_chain.free_intersections).begin();
  while(i_f!=(empty_chain.free_intersections).end())
    {
      if((g.goban)->get_stone(*i_f)==GE_BLACK_STONE)
	p_ch = &black_chains;
      else p_ch = &white_chains;
      
      if(not mk.is_treated(*i_f))
	{
	  int num_group = to_num_group[*i_f];
	  p_ch->push_back(num_group);
     
	  list<int>::const_iterator i_ch = 
	    ((groups[num_group])->ch).links.begin();
	  
	  while(i_ch!=((groups[num_group])->ch).links.end())
	    {
	      mk.set_treated(*i_ch);
	      i_ch++;
	    }
	}
      
      i_f++;
    }

  type = GE_HOLE;
}

void GE_HoleInformation::init_type_hole()
{
  if(type!=GE_HOLE) return;
  
  if(hole.size()>1)
    {
      type = GE_BIG_HOLE;
      return;
    }
  
  //size == 1
  if((not black_chains.empty())
     &&(not white_chains.empty()))
    {
      type = GE_MIX_EYE;
      return;
    }
  
  if((black_chains.empty())
     &&(white_chains.size()==1))
    {
      type = GE_TRUE_EYE;
      return;
    }
  
  if((white_chains.empty())
     &&(black_chains.size()==1))
    {
      type = GE_TRUE_EYE;
      return;
    }
}
     

int GE_HoleInformation::set_type_hole(int type_hole)
{
  int old_type = type;
  type = type_hole;
  return old_type;
}




int GE_HoleInformation::get_color_false_eye() const
{
  if(hole.size()!=1) return GE_WITHOUT_STONE;
  if(black_chains.empty()) 
    {
      if(white_chains.size()>1)
	return GE_WHITE_STONE;
      //sinon c'est considere comme un vrai oeil 
      
      return GE_WITHOUT_STONE;
    }
  
  if(white_chains.empty()) 
    {
      if(black_chains.size()>1)
	return GE_BLACK_STONE;
      //sinon c'est considere comme un vrai oeil 

      return GE_WITHOUT_STONE;
    }
  
  return GE_WITHOUT_STONE;
}

void GE_HoleInformation::print_error() const
{
  cerr<<"hole "<<index<<endl;
  GE_ListOperators::print_error(hole, move_to_string, GE_Directions::board_size);
  cerr<<endl;
  
  string msg_hole;
  GE_Tactic::hole_to_string(type, msg_hole);
  cerr<<"type: "<<msg_hole<<endl;
  
  cerr<<"neighbour black chains:"<<endl;
  if(black_chains.empty())
    cerr<<"no black chains"<<endl;
  else
    {
      list<int>::const_iterator i_ch = black_chains.begin();
      while(i_ch!=black_chains.end())
	{
	  cerr<<*i_ch<<" ";
	  i_ch++;
	}
      cerr<<endl;
    }
  
  
  cerr<<"neighbour white chains:"<<endl;
  if(white_chains.empty())
    cerr<<"no white chains"<<endl;
  else
    {
      list<int>::const_iterator i_ch = white_chains.begin();
      while(i_ch!=white_chains.end())
	{
	  cerr<<*i_ch<<" ";
	  i_ch++;
	}
      cerr<<endl;
    } 
}


/*************************************
 *************************************
 **                                 **        
 ** Information on a groups network **
 **                                 **
 *************************************
 *************************************/


void GE_GroupsNetwork::init(const GE_GroupInformation& gi, 
			 const vector<GE_GroupInformation*>& all_groups, 
			 int index_network, 
			 int level, 
			 GE_Marker& mk)
{
  index = index_network;
  level_connexion = level;  
  init(gi, all_groups, mk, level);
}


void GE_GroupsNetwork::init(const GE_GroupInformation& gi, 
			 const vector<GE_GroupInformation*>& all_groups, 
			 GE_Marker& mk, 
			 int level_connexion)
{
  if(mk.is_treated(gi.index))
    return;
  
  groups.push_back(gi.index);
  mk.set_treated(gi.index);
  
  if(level_connexion==GE_CONNEXION) return;
  
  list<int>::const_iterator i_g = gi.solid_virtual_connexions.begin();
  while(i_g!=gi.solid_virtual_connexions.end())
    {
      init(*(all_groups[*i_g]), all_groups, mk, level_connexion);
      i_g++;
    }
  
  
  if(level_connexion==GE_SOLID_VIRTUAL_CONNEXION) return;
  
  i_g = gi.virtual_connexions.begin();
  while(i_g!=gi.virtual_connexions.end())
    {
      init(*(all_groups[*i_g]), all_groups, mk, level_connexion);
      i_g++;
    }
  
  if(level_connexion==GE_VIRTUAL_CONNEXION) return;
  
  i_g = gi.tactic_connexions.begin();
  while(i_g!=gi.tactic_connexions.end())
    {
      init(*(all_groups[*i_g]), all_groups, mk, level_connexion);
      i_g++;
    }
  
  if(level_connexion==GE_TACTIC_CONNEXION) return;
  
  i_g = gi.false_wall_connexions.begin();
  while(i_g!=gi.false_wall_connexions.end())
    {
      init(*(all_groups[*i_g]), all_groups, mk, level_connexion);
      i_g++;
    }
  
  if(level_connexion==GE_FALSE_WALL_CONNEXION) return;
  

  list<pair<int, int> >::const_iterator i_pc = 
    gi.possible_connexions.begin();
  while(i_pc!=gi.possible_connexions.end())
    {
      init(*(all_groups[i_pc->first]), all_groups, mk, level_connexion);
      i_pc++;
    }

 
  if(level_connexion==GE_POSSIBLE_CONNEXION) return;
}




void GE_GroupsNetwork::print_error()
{
  cerr<<"network "<<index<<endl;
  string msg_connexion;
  GE_Tactic::connect_to_string(level_connexion, msg_connexion);
  cerr<<"connexion: "<<msg_connexion<<endl;
  cerr<<"groups: ";
  GE_VectorOperators::print_error(groups);
  cerr<<"possible eyes: ";
  GE_ListOperators::print_error(possible_eyes, move_to_string, 
			     GE_Directions::board_size);
}



GE_Informations::GE_Informations()
{
  //intersections = 0;
  game = 0;
  to_num_group.resize(GE_Directions::board_area, -1);
  //nb_holes = -1;
}

GE_Informations::GE_Informations(int size_goban)
{
  //intersections = new GE_IntersectionInformation[size_goban];
  game = 0;
  to_num_group.resize(GE_Directions::board_area, -1);
  //nb_holes = -1;
} 


GE_Informations::GE_Informations(GE_Game& g)
{
  
  game = &g;
  for(int i = 0; i<GE_Directions::board_area; i++)
    {
      intersections.push_back(new GE_IntersectionInformation(i));
    }
  
  for(int i = 0; i<GE_Directions::board_area; i++)
    (intersections[i])->init_possible_eye(g);
  
  
  to_num_group.resize(GE_Directions::board_area, -1);
  

  
  int index = 0;
  GE_Chains chs(*(g.goban));
  while(not chs.sets.empty())
    {
      
      groups.push_back(new GE_GroupInformation(index, chs.sets.front()));
      
      list<int>::const_iterator i_stone = ((chs.sets).front()).links.begin();
      while(i_stone!=((chs.sets).front()).links.end())
	{
	  to_num_group[*i_stone] = index;
	  i_stone++;
	}
      
      chs.sets.pop_front();
      index++;
    }
  
  
  init_holes(g);
  
}

GE_Informations::~GE_Informations()
{
  to_num_group.clear();
  
  for(int i = 0; i<(int)intersections.size(); i++)
    {
      delete intersections[i];
      intersections[i] = 0;
    }
  
  intersections.clear();


  for(int i = 0; i<(int)groups.size(); i++)
    {
      delete groups[i];
      groups[i] = 0;
    }
  
  groups.clear();
  
  
  for(int i = 0; i<(int)holes.size(); i++)
    {
      delete holes[i];
      holes[i] = 0;
    }
  
  holes.clear();
  
  for(int i = 0; i<(int)networks.size(); i++)
    {
      delete networks[i];
      networks[i] = 0;
    }

  networks.clear();
}

int GE_Informations::get_num_group(int location, int color_stone) const
{
  int num_group = -1;
  
  for(int i = 0; i<(int)groups.size(); i++)
    {
      if((groups[i])->get_color_stone()!=color_stone)
	continue;
      
      list<int>::const_iterator i_s = 
	(((groups[i])->ch).links).begin();
      
      while(i_s!=(((groups[i])->ch).links).end())
	{
	  if(*i_s==location)
	    {
	      num_group = i;
	      return num_group;
	    }
	  
	  i_s++;
	}
    }
  
  
  return num_group;
}

int GE_Informations::get_num_hole(int location) const
{
  int num_hole = -1;
  
  for(int i = 0; i<(int)holes.size(); i++)
    {
      list<int>::const_iterator i_h = ((holes[i])->hole).begin();

      while(i_h!=((holes[i])->hole).end())
	{
	  if(*i_h==location)
	    {
	      num_hole = i;
	      return num_hole;
	    }
	  
	  i_h++;
	}
    }
  
  return num_hole;
}

int GE_Informations::get_index(const GE_Game& g, int location) const
{
  int color_stone = (g.goban)->get_stone(location);
  
  if(color_stone!=GE_WITHOUT_STONE)
    return get_num_group(location, color_stone);
  else return get_num_hole(location);
}


int GE_Informations::get_network(const GE_Game& g, int location)
  const 
{
  int num_network = -1;
  
  int num_group = get_index(g, location);
  
  for(int i = 0; i<(int)networks.size(); i++)
    {
      const GE_GroupsNetwork& gn = *(networks[i]);
      
      if(GE_VectorOperators::is_in(gn.groups, num_group))
	{
	  num_network = i;
	  break;
	}
    }
  
  return num_network;
}



void GE_Informations::get_dead_stones(GE_Game& g, list<int>& dead_stones)
{
  dead_stones.clear();
  
  get_dead_stones_by_simulation(g, dead_stones);
  
  
  //  for(int location = 0; location<(g.goban)->size(); location++)
  //     if(intersections[location].is_dead(g, location))
  //       dead_stones.push_back(location);
}


//TO DEBUG
void GE_Informations::erase_group(int index)
{
  //delete[] groups[index];
  delete groups[index];
  groups[index] = &(*(groups[groups.size()-1]));
  (groups[index])->index = index;
  groups[groups.size()-1] = 0;
  
  list<int>::const_iterator i_stone = ((groups[index])->ch).links.begin();
  while(i_stone!=((groups[index])->ch).links.end())
    {
      to_num_group[*i_stone] = index;
      i_stone++;
    }
  
  
  groups.pop_back();
}


void GE_Informations::get_dead_stones_by_simulation(GE_Game& g, list<int>& dead_stones)
{
  GE_Goban temp_goban(*(g.goban));
  GE_Game temp_game(temp_goban);
  GE_Simulation::launch(temp_game);
  
  for(int i = 0; i<temp_goban.size(); i++)
    {
      if((g.goban)->get_stone(i)!=GE_WITHOUT_STONE)
	if((g.goban)->get_stone(i)!=(temp_goban.get_stone(i)))
	  dead_stones.push_back(i);
    }
}


void GE_Informations::init_holes(const GE_Game& g)
{
  GE_Goban without_stones((g.goban)->height, (g.goban)->width);
  (g.goban)->get_empty_intersections(without_stones);
  
  GE_Chains chs(without_stones);
  
  list<GE_Chain>::const_iterator i_ws = chs.sets.begin(); 
  while(i_ws!=chs.sets.end())
    {
      int ind = holes.size();
      holes.push_back(new GE_HoleInformation());
      (holes.back())->init(g, *i_ws, groups, to_num_group, ind);
      i_ws++;
    }
  
  
  for(int i = 0; i<(int)holes.size(); i++)
    {
      list<int>::const_iterator i_ch = ((holes[i])->black_chains).begin();
      
      while(i_ch!=((holes[i])->black_chains).end())
	{
	  (groups[*i_ch])->append_neighbour_hole(i);
	  i_ch++;
	}
      
      i_ch = ((holes[i])->white_chains).begin();
      
      while(i_ch!=((holes[i])->white_chains).end())
	{
	  (groups[*i_ch])->append_neighbour_hole(i);
	  i_ch++;
	}
    }
}


void GE_Informations::init_neighbour_groups(const GE_Game& g)
{
  GE_Marker seen((int)groups.size());
  
  for(int i = 0; i<(int)groups.size(); i++)
    {
      seen.update_treatment();
      
      int color_stone = ((groups[i])->ch).stone;
      int enemy_color_stone = GE_COLOR_STONE_ENEMY(color_stone); 
      
      list<int>::const_iterator i_ch = ((groups[i])->ch).links.begin();
      while(i_ch!=((groups[i])->ch).links.end())
	{
	  int big_location = GE_Bibliotheque::to_big_goban[*i_ch];
	  
	  for(int dir = GE_NORTH; dir<=GE_WEST; dir++)
	    {
	      int big_location_dir = 
		big_location+GE_Directions::directions[dir];
	      
	      int location_dir = 
		GE_Bibliotheque::to_goban[big_location_dir];
	      
	      if(location_dir>=0)
		{
		  if((g.goban)->get_stone(location_dir)
		     ==enemy_color_stone)
		    {
		      
		      int num_enemy_group = to_num_group[location_dir];
		      
		      if(not seen.is_treated(num_enemy_group))
			{
			  
			  (groups[i])->append_neighbour_group(num_enemy_group);
			  seen.set_treated(num_enemy_group);
			}
		      
		    }
		  
		}
	    }
	  
	  i_ch++;	      
	}
    }
}


int GE_Informations::get_nb_holes()
{
  return holes.size();
}

void GE_Informations::init_type_false_eye(GE_HoleInformation& hi)
{
  if(hi.type!=GE_HOLE) return;
  
  hi.set_type_hole(GE_FALSE_EYE);
  
  list<int>* p_chains = 0;
  if(hi.get_color_false_eye()==GE_BLACK_STONE)
    p_chains = &(hi.black_chains);
  else p_chains = &(hi.white_chains);
  
  list<int>::const_iterator i_gr = p_chains->begin();
  while(i_gr!=p_chains->end())
    {
      init_type_false_eye(*(groups[*i_gr]));
      i_gr++;
    }
}

void GE_Informations::init_type_false_eye(const GE_GroupInformation& gi)
{
  list<int>::const_iterator i_fe = gi.false_eyes.begin();
  
  while(i_fe!=gi.false_eyes.end())
    {
      if((holes[*i_fe])->type!=GE_HOLE)
	{
	  i_fe++;
	  continue;
	}
      
      (holes[*i_fe])->set_type_hole(GE_FALSE_EYE);
      
      list<int>* p_chains = 0;
      if((holes[*i_fe])->get_color_false_eye()==GE_BLACK_STONE)
	p_chains = &((holes[*i_fe])->black_chains);
      else p_chains = &((holes[*i_fe])->white_chains);
      
      list<int>::const_iterator i_gr = p_chains->begin();
      while(i_gr!=p_chains->end())
	{
	  init_type_false_eye(*(groups[*i_gr]));
	  i_gr++;
	}
      
      i_fe++;
    }
}


void GE_Informations::init_type_holes()
{
  
  for(int i = 0; i<(int)holes.size(); i++)
    {
      (holes[i])->init_type_hole();
    }
  
  
  for(int i = 0; i<(int)groups.size(); i++)
    {
      if((((groups[i])->false_eyes).size()==1)
	 &&((groups[i])->nb_eyes<1))
	{
	  int num_hole = ((groups[i])->false_eyes).back();
	  init_type_false_eye(*(holes[num_hole]));
	}
    }
  
  for(int i = 0; i<(int)holes.size(); i++)
    {
      if((holes[i])->type==GE_HOLE)
	(holes[i])->set_type_hole(GE_NO_FALSE_EYE);
    }
}

void GE_Informations::erase_possible_connection(GE_GroupInformation& gi1, 
					     GE_GroupInformation& gi2)
{
  
  list<pair<int, int> >::iterator i_p = gi1.possible_connexions.begin();
  while(i_p!=gi1.possible_connexions.end())
    {
      if(i_p->first==gi2.index)
	i_p = gi1.possible_connexions.erase(i_p);
      else i_p++;
    }
  
  i_p = gi2.possible_connexions.begin();
  while(i_p!=gi2.possible_connexions.end())
    {
      if(i_p->first==gi1.index)
	i_p = gi2.possible_connexions.erase(i_p);
      else i_p++;
    }
}


bool GE_Informations::init_virtual_connection(GE_GroupInformation& gi1, 
					   GE_GroupInformation& gi2, 
					   list<int>* connexions)
{
  if(mk_virtual_connected[gi1.index][gi2.index]==true)
    return true;
  
  if((gi1.ch).is_virtually_connected((gi2.ch), connexions, true))
    {
      if((connexions)&&(connexions->size()>2))
	{
	  if(not GE_ListOperators::is_in(gi1.solid_virtual_connexions, 
				      gi2.index))
	    {
	      gi1.append_solid_virtual_connexion(gi2.index);
	      gi2.append_solid_virtual_connexion(gi1.index);
	      
	      mk_virtual_connected[gi1.index][gi2.index] = true; 
	      mk_virtual_connected[gi2.index][gi1.index] = true; 
	    }
	}
      else
	{
	  if(not GE_ListOperators::is_in(gi1.virtual_connexions, 
				      gi2.index))
	    {
	      gi1.append_virtual_connexion(gi2.index);
	      gi2.append_virtual_connexion(gi1.index);
	      
	      mk_virtual_connected[gi1.index][gi2.index] = true; 
	      mk_virtual_connected[gi2.index][gi1.index] = true; 
	    }
	}
      
      return true;
    }
  else
    {
      if(connexions->size()==1)
	{
	  int type_connexion = -1;
	  if(is_really_connexion(*game, gi1, gi2, connexions->front(), 
				 &type_connexion))
	    {
	      gi1.append_tactic_connexion(gi2.index);
	      gi2.append_tactic_connexion(gi1.index);
	      
	      mk_virtual_connected[gi1.index][gi2.index] = true; 
	      mk_virtual_connected[gi2.index][gi1.index] = true; 
	    }
	  else
	    {
	      gi1.append_possible_connexion((gi2.index), connexions->front());
	      gi2.append_possible_connexion((gi1.index), connexions->front());
	    
	      mk_virtual_connected[gi1.index][gi2.index] = true; 
	      mk_virtual_connected[gi2.index][gi1.index] = true; 
	    }
	}
    }
  
  
  return false;
}


void GE_Informations::init_virtual_connections(const GE_HoleInformation& hi)
{
  list<int>::const_iterator i_g = hi.black_chains.begin();
  
  while(i_g!=hi.black_chains.end())
    {
      GE_GroupInformation& gi1 = *(groups[*i_g]);
      list<int>::const_iterator i_g2 = i_g;
      i_g2++;
      while(i_g2!=hi.black_chains.end())
	{
	  GE_GroupInformation& gi2 = *(groups[*i_g2]);
	  list<int> connexions;
	  init_virtual_connection(gi1, gi2, &connexions);
	  
	  i_g2++;
	}
      
      i_g++;
    }
  
  i_g = hi.white_chains.begin();
  while(i_g!=hi.white_chains.end())
    {

      GE_GroupInformation& gi1 = *(groups[*i_g]);
      list<int>::const_iterator i_g2 = i_g;
      i_g2++;
      while(i_g2!=hi.white_chains.end())
	{
	  GE_GroupInformation& gi2 = *(groups[*i_g2]);
	  list<int> connexions;
	  init_virtual_connection(gi1, gi2, &connexions);
	  
	  i_g2++;
	}
      
      i_g++;
    }
}



void GE_Informations::init_false_wall_connections
(const GE_GroupInformation& gi, vector<vector<bool> >& mk_connected)
{

  if(gi.status!=GE_DEAD) return;

  
  for(int i=0; i<(int)(gi.neighbour_groups.size()); i++) 
    {
      int num_group1 = (gi.neighbour_groups)[i];
      
      for(int j = i+1; j<(int)(gi.neighbour_groups.size()); j++)
	{
	  int num_group2 = (gi.neighbour_groups)[j];
	  
	  if(mk_connected[num_group1][num_group2]==true)
	    continue;
	  
	  (groups[num_group1])->append_false_wall_connexion(num_group2);
	  (groups[num_group2])->append_false_wall_connexion(num_group1);
	  
	  mk_connected[num_group1][num_group2] = true; 
	  mk_connected[num_group2][num_group1] = true; 

	  erase_possible_connection(*(groups[num_group1]), 
				    *(groups[num_group2]));	  
	}
    }
}

void GE_Informations::init_false_wall_connections()
{
  vector<vector<bool> > mk_connected;
  mk_connected.resize(groups.size());
  for(int i = 0; i<(int)mk_connected.size(); i++)
    {
      mk_connected[i].resize(groups.size(), false); 
    }
  
 
  vector<int> all_connexions;
  for(int i = 0; i<(int)groups.size(); i++)
    {
      (groups[i])->get_all_connexions(all_connexions, GE_FALSE_WALL_CONNEXION);
      
      for(int j = 0; j<(int)all_connexions.size(); j++)
	{
	  int num_group2 = all_connexions[j];
	  mk_connected[i][num_group2] = true;
	  
	  //mk_connected[num_group2][i] = true; 
	  //fait a un autre moment dans la boucle
	}
      
      all_connexions.clear();
    }

 
  for(int i = 0; i<(int)groups.size(); i++)
    {
      init_false_wall_connections(*(groups)[i], mk_connected);
    }
}


void GE_Informations::init_virtual_connections()
{
  if(mk_virtual_connected.size()>0)
    {
      for(int i = 0; i<(int)mk_virtual_connected.size(); i++)
	{
	  mk_virtual_connected[i].clear(); 
	}
      
      mk_virtual_connected.clear();
    }

  mk_virtual_connected.resize(groups.size());
  for(int i = 0; i<(int)mk_virtual_connected.size(); i++)
    {
      mk_virtual_connected[i].resize(groups.size(), false); 
    }

  for(int i = 0; i<(int)holes.size(); i++)
    {
      init_virtual_connections(*(holes[i]));
    }
}








void GE_Informations::init_stats_group(vector<int>& v_group, int nb_simus, 
				    team_color first_play)
{
  if(first_play==GE_NEUTRAL_COLOR)
    { 
      for(int i = 0; i<(int)groups.size(); i++)
	{
	  double nb_life = 0;
	  double proba_life = 0;
	  
	  list<int>::const_iterator i_st = (groups[i])->ch.links.begin();
	  while(i_st!=(groups[i])->ch.links.end())
	    {
	      nb_life+=v_group[*i_st];
	      i_st++;
	    }
	  
	  proba_life = (nb_life/(double)(nb_simus*(groups[i])->ch.size()));
	  (groups[i])->set_stats_life(proba_life);
	}
      
      return;
    }
  
  
  
  int first_color_stone = GE_WITHOUT_STONE;
  if(first_play==GE_BLACK)
    first_color_stone = GE_BLACK_STONE;
  else first_color_stone = GE_WHITE_STONE;
  
  for(int i = 0; i<(int)groups.size(); i++)
    {
      double nb_life = 0;
      double proba_life = 0;
      
      list<int>::const_iterator i_st = (groups[i])->ch.links.begin();
      while(i_st!=(groups[i])->ch.links.end())
	{
	  nb_life+=v_group[*i_st];
	  i_st++;
	}
      
      proba_life = (nb_life/(double)(nb_simus*(groups[i])->ch.size()));
      
      //if(((groups[i])->ch).stone==first_color_stone)
      (groups[i])->set_stats_life(proba_life);
      //else (groups[i])->set_stats_kill(proba_life);
    }
}

void GE_Informations::study_alive(GE_Game& g)
{
  for(int i = 0; i<(int)groups.size(); i++)
    {
      //if(((g.tc==GE_BLACK)&&((groups[i])->get_color_stone()==GE_BLACK_STONE))
      //||((g.tc==GE_WHITE)&&((groups[i])->get_color_stone()==GE_WHITE_STONE)))
      //if(groups[i]->is_alive==MAYBE_ALIVE)
      groups[i]->study_alive(g);
    }
}


void GE_Informations::study_alive_by_stats()
{
  for(int i = 0; i<(int)groups.size(); i++)
    {
      if(groups[i]->status==GE_UNKNOWN_DEATH_OR_LIFE)
	groups[i]->study_alive_by_stats();
    }
}


void GE_Informations::find_unkillable()
{
  for(int i = 0; i<(int)groups.size(); i++)
    {
      if(groups[i]->nb_eyes>1)
	{
	  groups[i]->set_motif_status(GE_UNKILLABLE_BY_TWO_EYES);
	}
    }
  
  for(int i = 0; i<(int)holes.size(); i++)
    {
      if(holes[i]->type==GE_NO_FALSE_EYE)
	{
	  list<int>* p_chains = 0;
	  if((holes[i])->get_color_false_eye()==GE_BLACK_STONE)
	    p_chains = &((holes[i])->black_chains);
	  else p_chains = &((holes[i])->white_chains);
	  
	  list<int>::const_iterator i_gr = p_chains->begin();
	  while(i_gr!=p_chains->end())
	    {
	      groups[*i_gr]->set_motif_status(GE_UNKILLABLE_WITH_ANOTHER_GROUP);
	      i_gr++;
	    }
	}
    }
}

void GE_Informations::init_alive_with_unkillable_group(const GE_GroupInformation& ug)
{
  if(ug.status!=GE_UNKILLABLE)
    return;
  
  list<int>::const_iterator i_g = ug.solid_virtual_connexions.begin();
  
  while(i_g!=ug.solid_virtual_connexions.end())
    {
      if(groups[*i_g]->status<GE_UNKILLABLE)
	(groups[*i_g])->
	  set_motif_status(GE_ALIVE_BY_SOLID_CONNEXION_UNKILLABLE_GROUP);
      i_g++;
    }
  
  i_g = ug.virtual_connexions.begin();
  while(i_g!=ug.virtual_connexions.end())
    {
      if(groups[*i_g]->status<GE_ALIVE)
	(groups[*i_g])->
	  set_motif_status(GE_ALIVE_BY_CONNEXION_UNKILLABLE_GROUP);
      i_g++;
    }
  
  i_g = ug.tactic_connexions.begin();
  while(i_g!=ug.tactic_connexions.end())
    {
      if(groups[*i_g]->status<GE_ALIVE)
	(groups[*i_g])->
	  set_motif_status(GE_ALIVE_BY_TACTIC_CONNEXION_UNKILLABLE_GROUP);
      i_g++;
    }

  
  i_g = ug.false_wall_connexions.begin();
  while(i_g!=ug.false_wall_connexions.end())
    {
      if(groups[*i_g]->status<GE_ALIVE)
	(groups[*i_g])->
	  set_motif_status(GE_ALIVE_BY_FALSE_WALL_CONNEXION_UNKILLABLE_GROUP);
      i_g++;
    }
}

void GE_Informations::init_alive_with_unkillable_group()
{
  for(int i = 0; i<(int)groups.size(); i++)
    {
      if(groups[i]->status==GE_UNKILLABLE)
	{
	  init_alive_with_unkillable_group(*(groups[i]));
	}
    }
}

bool GE_Informations::init_alive_with_alive_group(const GE_GroupInformation& ag)
{
  bool is_updated = false;
  
  if(ag.status!=GE_ALIVE)
    return false;
  
  list<int>::const_iterator i_g = ag.solid_virtual_connexions.begin();
  
  while(i_g!=ag.solid_virtual_connexions.end())
    {
      if(groups[*i_g]->status<GE_ALIVE)
	{
	  (groups[*i_g])->
	    set_motif_status(GE_ALIVE_BY_SOLID_CONNEXION_ALIVE_GROUP);
	  is_updated = true;
	}
      i_g++;
    }
  
  i_g = ag.virtual_connexions.begin();
  while(i_g!=ag.virtual_connexions.end())
    {
      if(groups[*i_g]->status<GE_ALIVE)
	{
	  (groups[*i_g])->
	    set_motif_status(GE_ALIVE_BY_CONNEXION_ALIVE_GROUP);
	  
	  is_updated = true;
	}
      i_g++;
    }
  
  i_g = ag.tactic_connexions.begin();
  while(i_g!=ag.tactic_connexions.end())
    {
      if(groups[*i_g]->status<GE_ALIVE)
	{
	  (groups[*i_g])->
	    set_motif_status(GE_ALIVE_BY_TACTIC_CONNEXION_ALIVE_GROUP);
	  
	  is_updated = true;
	}
      i_g++;
    }

  i_g = ag.false_wall_connexions.begin();
  while(i_g!=ag.false_wall_connexions.end())
    {
      if(groups[*i_g]->status<GE_ALIVE)
	{
	  (groups[*i_g])->
	    set_motif_status(GE_ALIVE_BY_FALSE_WALL_CONNEXION_ALIVE_GROUP);
	  
	  is_updated = true;
	}
      i_g++;
    }
  
  return is_updated;
}

void GE_Informations::init_alive_with_alive_group()
{
  while(true)
    {
      bool is_updated = false;
      for(int i = 0; i<(int)groups.size(); i++)
	{
	  if(groups[i]->status==GE_ALIVE)
	    { 
	      bool has_been_modified = 
		init_alive_with_alive_group(*(groups[i]));
	      
	      if(has_been_modified)
		is_updated = has_been_modified;
	    }
	}
      
      if(not is_updated) break;
    }
}


void
GE_Informations::init_alive_with_possible_connexion(GE_GroupInformation& gi)
{
  list<pair<int, int> >::const_iterator i_pc = gi.possible_connexions.begin();
  
  int nb_connexions_with_alive_group = 0;
  int save_move = GE_PASS_MOVE;
  
  while(i_pc!=gi.possible_connexions.end())
    {
      if((groups[i_pc->first])->status>=GE_ALIVE)
	{
	  nb_connexions_with_alive_group++;
	  save_move = i_pc->second;
	}
      i_pc++;
    }
  
  if(nb_connexions_with_alive_group>=2)
    {
      gi.set_motif_status(GE_ALIVE_BY_CONNEXION_WITH_DIFFERENT_ALIVE_GROUPS);
      return;
    }
  
  if(nb_connexions_with_alive_group==1)
    {
      gi.set_motif_status(GE_SAVE_BY_CONNEXION_WITH_ALIVE_GROUP);
      //TO DO save move a append
      return;
    }
}

void GE_Informations::init_alive_with_possible_connexion()
{
  for(int i = 0; i<(int)groups.size(); i++)
    {
      if(groups[i]->status<GE_ALIVE)
	{
	  init_alive_with_possible_connexion(*(groups[i]));
	}
    }
}

void 
GE_Informations::init_alive_by_direct_connexion_two_eyes
(GE_GroupInformation& gi1)
{
  if(gi1.nb_eyes!=1) return;
  if(gi1.status>=GE_ALIVE) 
    {
      if(gi1.motif_alive!=GE_ALIVE_BY_DIRECT_CONNEXION_TWO_EYES)
	return;
    }
  
  list<int>::const_iterator i_g = gi1.solid_virtual_connexions.begin();
  while(i_g!=gi1.solid_virtual_connexions.end())
    {
      GE_GroupInformation& gi2 = *(groups[*i_g]);
    
      if((gi2.nb_eyes==1)&&(gi2.status<GE_UNKILLABLE))
	{
	  gi1.set_motif_status(GE_ALIVE_BY_DIRECT_CONNEXION_TWO_EYES);
	  gi2.set_motif_status(GE_ALIVE_BY_DIRECT_CONNEXION_TWO_EYES);  
	  
	  return;
	}
      
      i_g++;
    }
  
  i_g = gi1.virtual_connexions.begin();
  while(i_g!=gi1.virtual_connexions.end())
    {
      GE_GroupInformation& gi2 = *(groups[*i_g]);
      
      if((gi2.nb_eyes==1)&&(gi2.status<GE_UNKILLABLE))
	{
	  gi1.set_motif_status(GE_ALIVE_BY_DIRECT_CONNEXION_TWO_EYES);
	  gi2.set_motif_status(GE_ALIVE_BY_DIRECT_CONNEXION_TWO_EYES);  
	  
	  return;
	}
      
      i_g++;
    }

  
  i_g = gi1.tactic_connexions.begin();
  while(i_g!=gi1.tactic_connexions.end())
    {
      GE_GroupInformation& gi2 = *(groups[*i_g]);
      
      if((gi2.nb_eyes==1)&&(gi2.status<GE_UNKILLABLE))
	{
	  gi1.set_motif_status(GE_ALIVE_BY_DIRECT_TACTIC_CONNEXION_TWO_EYES);
	  gi2.set_motif_status(GE_ALIVE_BY_DIRECT_TACTIC_CONNEXION_TWO_EYES);  
	  
	  return;
	}
      
      i_g++;
    }
  
  
  i_g = gi1.false_wall_connexions.begin();
  while(i_g!=gi1.false_wall_connexions.end())
    {
      GE_GroupInformation& gi2 = *(groups[*i_g]);
      
      if((gi2.nb_eyes==1)&&(gi2.status<GE_UNKILLABLE))
	{
	  gi1.set_motif_status(GE_ALIVE_BY_DIRECT_FALSE_WALL_CONNEXION_TWO_EYES);
	  gi2.set_motif_status(GE_ALIVE_BY_DIRECT_FALSE_WALL_CONNEXION_TWO_EYES); 

	  return;
	}
      
      i_g++;
    }

}


void GE_Informations::init_alive_by_direct_connexion_two_eyes()
{
  for(int i = 0; i<(int)groups.size(); i++)
    {
      init_alive_by_direct_connexion_two_eyes(*(groups[i]));
    }
}

bool
GE_Informations::init_alive_by_connexion_two_eyes
(GE_GroupInformation& gi1, int& nb_eyes, GE_Marker& mk)
{
  if((nb_eyes==0)&&(gi1.nb_eyes<=0)) return false;
  if(gi1.status>=GE_ALIVE) return false;
  if(mk.is_treated(gi1.index))
    return false;
  else
    {
      mk.set_treated(gi1.index);
    }
  
  if(gi1.nb_eyes>=0)
    nb_eyes+=gi1.nb_eyes;
  //cerr<<nb_eyes<<" "<<gi1.index<<endl; 

  if(nb_eyes==2)
    {
      gi1.set_motif_status(GE_ALIVE_BY_CONNEXION_TWO_EYES);
      return true;
    }
  
  list<int>::const_iterator i_g = gi1.solid_virtual_connexions.begin();
  while(i_g!=gi1.solid_virtual_connexions.end())
    {
      GE_GroupInformation& gi2 = *(groups[*i_g]);
      
      if(init_alive_by_connexion_two_eyes(gi2, nb_eyes, mk))
	{
	  gi1.set_motif_status(GE_ALIVE_BY_CONNEXION_TWO_EYES);
	  return true;
	}
      
      i_g++;
    }
  
  i_g = gi1.virtual_connexions.begin();
  while(i_g!=gi1.virtual_connexions.end())
    {
      GE_GroupInformation& gi2 = *(groups[*i_g]);
      
      if(init_alive_by_connexion_two_eyes(gi2, nb_eyes, mk))
	{
	  gi1.set_motif_status(GE_ALIVE_BY_CONNEXION_TWO_EYES);
	  return true;
	}
      
      i_g++;
    }


  //TODO ALIVE_BY_TACTIC_CONNEXION_TWO_EYES ?
  i_g = gi1.tactic_connexions.begin();
  while(i_g!=gi1.tactic_connexions.end())
    {
      GE_GroupInformation& gi2 = *(groups[*i_g]);
      
      if(init_alive_by_connexion_two_eyes(gi2, nb_eyes, mk))
	{
	  gi1.set_motif_status(GE_ALIVE_BY_CONNEXION_TWO_EYES);
	  return true;
	}
      
      i_g++;
    }
 

  i_g = gi1.false_wall_connexions.begin();
  while(i_g!=gi1.false_wall_connexions.end())
    {
      GE_GroupInformation& gi2 = *(groups[*i_g]);
      
      if(init_alive_by_connexion_two_eyes(gi2, nb_eyes, mk))
	{
	  gi1.set_motif_status(GE_ALIVE_BY_CONNEXION_TWO_EYES);
	  return true;
	}
      
      i_g++;
    }
  
 
  
  return false;
}


void GE_Informations::init_alive_by_connexion_two_eyes()
{
  
  GE_Marker mk(groups.size());
  for(int i = 0; i<(int)groups.size(); i++)
    {
      int nb_eyes = 0;
      mk.update_treatment();
      
      if((groups[i])->status<GE_ALIVE)
	{
	  init_alive_by_connexion_two_eyes(*(groups[i]), nb_eyes, mk);
	}
    }
  
}


void 
GE_Informations::init_alive_by_connexion_different_one_eyes
(GE_GroupInformation& gi)
{
  if(gi.status>=GE_ALIVE) return;
  if(gi.nb_eyes!=1) return;

  int nb_groups_with_eye = 0;
  
  list<pair<int, int> >::const_iterator i_pc = gi.possible_connexions.begin();
  while(i_pc!=gi.possible_connexions.end())
    {
      if((groups[i_pc->first])->nb_eyes==1)
	nb_groups_with_eye++;
      
      i_pc++;
    }

  if(nb_groups_with_eye>=2)
    {
      gi.set_motif_status(GE_ALIVE_BY_CONNEXION_DIFFERENT_ONE_EYES);
      return;
    }
  
  if(nb_groups_with_eye==1)
    {
      gi.set_motif_status(GE_SAVE_BY_CONNEXION_ONE_EYE);
      //TO DO save move a append
      return;
    }
}

void GE_Informations::init_alive_by_connexion_different_one_eyes()
{
  for(int i = 0; i<(int)groups.size(); i++)
    {
      if((groups[i])->status<GE_ALIVE)
	{
	  init_alive_by_connexion_different_one_eyes(*(groups[i]));
	}
    }
  
}



bool GE_Informations::all_groups_stationary_by_kill(const GE_HoleInformation& hi)
{
  list<int>::const_iterator i_g = hi.black_chains.begin();
  while(i_g!=hi.black_chains.end())
    {
      if((groups[(*i_g)])->mobility!=GE_STATIONARY_BY_KILL)
	return false;
      i_g++;
    }
  
  i_g = hi.white_chains.begin();
  while(i_g!=hi.white_chains.end())
    {
      if((groups[(*i_g)])->mobility!=GE_STATIONARY_BY_KILL)
	return false;
      i_g++;
    }
  
  return true;
}


bool GE_Informations::is_perhaps_seki(const GE_GroupInformation& gi)
{
  for(int i = 0; i<(int)gi.neighbour_holes.size(); i++)
    {
      int num_hole = (gi.neighbour_holes)[i];
      if(not all_groups_stationary_by_kill(*(holes[num_hole])))
	return false;
    }
  
  return true;
}


void GE_Informations::find_sekis()
{
  for(int i = 0; i<(int)groups.size(); i++)
    {
      if((groups[i])->status==GE_UNKNOWN_DEATH_OR_LIFE)
	{
	  if(is_perhaps_seki(*(groups[i])))
	    {
	      (groups[i])->set_motif_status(GE_MAYBE_ALIVE_PERHAPS_SEKI); 
	    }
	}
    }
}

/*
  the question is :
  Can I play on one of my liberties of my group when it stays 2 liberties?
*/
bool GE_Informations::is_mobile(GE_Game& g, const GE_Chain& ch) 
{
  if(ch.liberties==2)
    {
      bool generate_pass_move = false;
      if(((g.tc==GE_BLACK)&&(ch.stone!=GE_BLACK_STONE))
	 ||((g.tc==GE_WHITE)&&(ch.stone!=GE_WHITE_STONE)))
	{
	  g.play(GE_PASS_MOVE);
	  generate_pass_move = true;
	}
      
      list<int>::const_iterator i_mv = ch.free_intersections.begin();
      
      while(i_mv!=ch.free_intersections.end())
	{
	  if(not g.play(*i_mv))
	    assert(0);
	  
	  GE_Chain temp_ch;
	  temp_ch.init(*(g.goban), *i_mv);
	  g.backward();
	  
	  if(not temp_ch.is_atari())
	    {
	      if(generate_pass_move)
		g.backward();
	      return true;
	    }
	  i_mv++;
	}
      
      
      if(generate_pass_move)
	g.backward();
      
      return false;
    }
  
  return true;
}



int GE_Informations::count_eye(const GE_GroupInformation& gi, 
			    list<int>* eyes)
{

  int nb_eyes = 0;
  for(int i = 0; i<(int)gi.neighbour_holes.size(); i++)
    {
      int num_hole = (gi.neighbour_holes)[i];
      if(((holes[num_hole])->hole).size()==1)
	{
	  if((((holes[num_hole])->black_chains).empty())
	     &&(((holes[num_hole])->white_chains).size()==1))
	    {
	      nb_eyes++;
	      if(eyes)
		eyes->push_back(((holes[num_hole])->white_chains).front());
	    }
	  if((((holes[num_hole])->white_chains).empty())
	     &&(((holes[num_hole])->black_chains).size()==1))
	    {
	      nb_eyes++;
	      if(eyes)
		eyes->push_back(((holes[num_hole])->black_chains).front());
	    }
	}
    }
  
  return nb_eyes;
}

int GE_Informations::set_nb_eyes(GE_GroupInformation& gi, int nb_eyes)
{
  int old_nb_eyes = gi.nb_eyes;
  gi.nb_eyes = nb_eyes;
  return old_nb_eyes;
}


void GE_Informations::init_eyes()
{
  for(int i = 0; i<(int)groups.size(); i++)
    {
      list<int> eyes;
      int nb_eyes = count_eye(*(groups[i]), 
			      &eyes);
      
      //if(nb_eyes!=0) //BUG ???? Quel est l'etait l'interet de ce test ????
      set_nb_eyes(*(groups[i]), nb_eyes);
    }
}


void GE_Informations::init_false_eyes()
{
  for(int i = 0; i<(int)holes.size(); i++)
    {
      int color_false_eye = (holes[i])->get_color_false_eye();
      if(color_false_eye==GE_WITHOUT_STONE) continue;
      
      list<int>* neighbours;
      if(color_false_eye==GE_BLACK_STONE)
	neighbours = &(holes[i])->black_chains;
      else neighbours = &(holes[i])->white_chains;
      
      list<int>::const_iterator i_gr = neighbours->begin();
      while(i_gr!=neighbours->end())
	{
	  (groups[*i_gr])->append_false_eye(i);
	  i_gr++;
	}
    }
}


void GE_Informations::init_group_mobility(GE_Game& g)
{
  for(int i = 0; i<(int)groups.size(); i++)
    {
      (groups[i])->init_mobility(g);      
    }
}


bool GE_Informations::is_really_connexion(GE_Game& g, 
				       GE_GroupInformation& gi1, 
				       GE_GroupInformation& gi2, 
				       int connexion, 
				       int* type_connexion)
{
  vector<int> common_neighbours;
  GE_VectorOperators::intersection(gi1.neighbour_groups, 
				gi2.neighbour_groups, 
				common_neighbours);
  for(int i = 0; i<(int)common_neighbours.size(); i++)
    {
      int num_neighbour = common_neighbours[i];
      if((groups[num_neighbour])->status==GE_DEAD)
	{
	  if(type_connexion)
	    {
	      *type_connexion = GE_CONNECT_BY_CAPTURE_COMMON_NEIGHBOUR;
	      return true;
	    }
	}
    }
  
  bool generate_pass_move = false;
  if(((g.tc==GE_BLACK)&&(gi1.ch.stone==GE_BLACK_STONE))
     ||((g.tc==GE_WHITE)&&(gi1.ch.stone==GE_WHITE_STONE)))
    {
      g.play(GE_PASS_MOVE);
      generate_pass_move = true;
    }
  
  bool rep = false;
  if(g.play(connexion))
    {
      list<int> l_rep;
      GE_Tactic tact;
      
      rep = tact.study_shisho2(g, connexion, &l_rep);
      g.backward();
    }
  else 
    {
      if(generate_pass_move)
	g.backward();
      
      if(type_connexion)
	*type_connexion = GE_CONNECT_BY_IMPOSSIBLE_PLAY;
      
      return true;
    }
  
  if(generate_pass_move)
    g.backward();
 
  if(rep)
    {
      if(type_connexion) 
	*type_connexion = GE_CONNECT_BY_CAPTURE_DISCONNECTION;
      return rep;
    }
  
  return false;
}


//NOT TESTED
void GE_Informations::get_groups_network(const GE_GroupInformation& gi, 
				      vector<int>& v_groups, 
				      GE_Marker& mk, 
				      int level_connexion)
{
  if(mk.is_treated(gi.index))
    return;
  
  v_groups.push_back(gi.index);
  mk.set_treated(gi.index);
  
  if(level_connexion==GE_CONNEXION) return;
  
  list<int>::const_iterator i_g = gi.solid_virtual_connexions.begin();
  while(i_g!=gi.solid_virtual_connexions.end())
    {
      get_groups_network(*(groups[*i_g]), v_groups, mk, level_connexion);
      i_g++;
    }
  
 
  if(level_connexion==GE_SOLID_VIRTUAL_CONNEXION) return;
  
  i_g = gi.virtual_connexions.begin();
  while(i_g!=gi.virtual_connexions.end())
    {
      get_groups_network(*(groups[*i_g]), v_groups, mk, level_connexion);
      i_g++;
    }

  if(level_connexion==GE_VIRTUAL_CONNEXION) return;
  
  i_g = gi.tactic_connexions.begin();
  while(i_g!=gi.tactic_connexions.end())
    {
      get_groups_network(*(groups[*i_g]), v_groups, mk, level_connexion);
      i_g++;
    }

  if(level_connexion==GE_TACTIC_CONNEXION) return;
  
  
  i_g = gi.false_wall_connexions.begin();
  while(i_g!=gi.false_wall_connexions.end())
    {
      get_groups_network(*(groups[*i_g]), v_groups, mk, level_connexion);
      i_g++;
    }
  
  if(level_connexion==GE_FALSE_WALL_CONNEXION) return;

  list<pair<int, int> >::const_iterator i_pc = 
    gi.possible_connexions.begin();
  while(i_pc!=gi.possible_connexions.end())
    {
      get_groups_network(*(groups[i_pc->first]), v_groups, mk, level_connexion);
      i_g++;
    }
  
  if(level_connexion==GE_POSSIBLE_CONNEXION) return;
}

void GE_Informations::init_groups_networks(int level_connexion)
{
  GE_Marker mk(groups.size());
  mk.update_treatment();
  
  int index_network = 0;
  for(int i = 0; i<(int)groups.size(); i++)
    {
      if(mk.is_treated(i)) continue;
      
      networks.push_back(new GE_GroupsNetwork());
      (networks[index_network])->init(*(groups[i]), groups, index_network, 
				      level_connexion, mk);
      
      index_network++;
    }
  
}


void GE_Informations::refine_possible_eyes()
{
  for(int i = 0; i<(int)intersections.size(); i++)
    {
      (intersections[i])->refine_possible_eye(*game, groups, to_num_group);
    }
}



void GE_Informations::get_near_possible_eyes(GE_GroupInformation& gi, 
					  GE_Marker& mk_locations, 
					  list<int>& l_possible_eyes)
{
  list<int>::const_iterator i_fi = gi.ch.free_intersections.begin();
  
  if(gi.ch.stone==GE_BLACK_STONE)
    {    
      while(i_fi!=gi.ch.free_intersections.end())
	{
	  if(not mk_locations.is_treated(*i_fi))
	    {
	      mk_locations.set_treated(*i_fi);
	      if((intersections[*i_fi])->possible_black_eye==GE_TRUE)
		{
		  l_possible_eyes.push_back(*i_fi);
		}
	    }
	  i_fi++;
	} 
    }
  else
    {
      while(i_fi!=gi.ch.free_intersections.end())
	{
	  if(not mk_locations.is_treated(*i_fi))
	    {
	      mk_locations.set_treated(*i_fi);
	      if((intersections[*i_fi])->possible_white_eye==GE_TRUE)
		{
		  l_possible_eyes.push_back(*i_fi);
		}
	    }
	  i_fi++;
	} 
    }

  /*
  for(int i = 0; i<(int)gi.neighbour_groups.size(); i++)
    {
      int num_group = (gi.neighbour_groups)[i];
      
      cons
      for(int loc = 0; loc<(groups[i])
      if(not mk_locations.is_treated(*i_fi))
	{
	  mk_locations.set_treated(*i_fi);
	  if((intersections[*i_fi])->possible_white_eye==GE_TRUE)
	    {
	      l_possible_eyes.push_back(*i_fi);
	    }
	}
      i_fi++;
      
    }
  */
}


void GE_Informations::get_near_possible_eyes(GE_GroupsNetwork& gn, 
					  GE_Marker& mk_locations, 
					  list<int>& l_possible_eyes)
{
  for(int i = 0; i<(int)(gn.groups).size(); i++)
    {

      int num_group =  (gn.groups)[i];
      GE_GroupInformation& gi = *(groups[num_group]);
      
      get_near_possible_eyes(gi, mk_locations, l_possible_eyes);
    }
}

void GE_Informations::init_possible_eyes(GE_GroupsNetwork& gn, 
				      GE_Marker& mk_locations)
{
  get_near_possible_eyes(gn, mk_locations, gn.possible_eyes);
}


void GE_Informations::init_possible_eyes()
{
  GE_Marker mk_intersections(intersections.size());
  mk_intersections.update_treatment();

  for(int i = 0; i<(int)networks.size(); i++)
    {
      init_possible_eyes(*(networks[i]), mk_intersections);
      mk_intersections.update_treatment();
    }
}


void GE_Informations::get_dead_stones(list<int>& l_dead) const
{
  for(int i = 0; i<(int)groups.size(); i++)
    {
      const GE_GroupInformation& gi = *(groups[i]);
      
      if(gi.get_status()==GE_DEAD)
	{
	  GE_ListOperators::merge(gi.ch.links, l_dead, true);
	}
    }
}

void GE_Informations::get_useless_moves_by_dead_stones(int color_stone, 
						    list<int>& l_useless) 
  const
{
  int enemy_color_stone = GE_COLOR_STONE_ENEMY(color_stone);
  
  for(int i = 0; i<(int)groups.size(); i++)
    {
      const GE_GroupInformation& gi = *(groups[i]);
      
      if((gi.get_color_stone()==enemy_color_stone)
	 &&(gi.get_status()==GE_DEAD))
	{
	  GE_ListOperators::merge(gi.ch.free_intersections, 
			       l_useless, true);
	}
    }
}

void GE_Informations::get_useless_connexions(int color_stone, 
					  list<int>& l_useless) 
  const
{
  for(int i = 0; i<(int)groups.size(); i++)
    {
      const GE_GroupInformation& gi = *(groups[i]);
      
      if(gi.get_color_stone()!=color_stone) continue;
      
      vector<int> all_connexions;
      gi.get_all_connexions(all_connexions, GE_FALSE_WALL_CONNEXION);     
      
      
      for(int j = 0; j<(int)all_connexions.size(); j++)
	{
	  int num_group = all_connexions[j];
	  const GE_GroupInformation& gi2 = *(groups[num_group]);
	  
	  list<int> temp_connexions;
	  (gi.ch).get_connexions(gi2.ch, temp_connexions);
	  
	  GE_ListOperators::merge(temp_connexions, l_useless, true);
	}
    }

  //GE_ListOperators::print_error(l_useless, move_to_string, GE_Directions::board_size);

}




void GE_Informations::print_error() const
{
  //GE_IntersectionInformation* intersections;
  
  cerr<<"intersections"<<endl;
  for(int i = 0; i<(int)intersections.size(); i++)
    {
      (intersections[i])->print_error();
    }
  


  cerr<<"groups"<<endl;
  for(int i = 0; i<(int)groups.size(); i++)
    {
      (groups[i])->print_error();
    }
  
  cerr<<endl;
  
  cerr<<"holes"<<endl;
  for(int i = 0; i<(int)holes.size(); i++)
    {
      (holes[i])->print_error();
    }
  cerr<<endl;
  
  cerr<<"networks"<<endl;
  for(int i = 0; i<(int)networks.size(); i++)
    {
      (networks[i])->print_error();
    }
  cerr<<endl;
}

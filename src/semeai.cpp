#include "semeai.h"
#include <stdio.h>
#include <stdlib.h>

#define GE_PARAM_SEMEAI_DETECTION_1 0.83
#define GE_PARAM_SEMEAI_DETECTION_2 0.14

#define GE_PARAM_DEATH_OR_LIFE_DETECTION_1 0.83

#define GE_PARAM_MAX_LENGTH_SIMULATION 20


bool GE_AnalyzeSemeai::to_reinitialize = true;
vector<double> GE_AnalyzeSemeai::expertise_semeai;
double GE_AnalyzeSemeai::semeai_threat_atari = 100; //100;
double GE_AnalyzeSemeai::semeai_tsuke = 0;
double GE_AnalyzeSemeai::semeai_kata = 0; //40;
double GE_AnalyzeSemeai::semeai_tobi = 30; //30;
//double GE_AnalyzeSemeai::semeai_keima = 20;
double GE_AnalyzeSemeai::semeai_dame = -80;

double GE_AnalyzeSemeai::semeai_liberties = 100;
double GE_AnalyzeSemeai::semeai_solid_liberties = 20;
double GE_AnalyzeSemeai::semeai_eyes = 10;

double GE_AnalyzeSemeai::semeai_solid_move = 20;

double GE_AnalyzeSemeai::last_move_liberties = 5;
double GE_AnalyzeSemeai::last_move_save_atari = 500; //30;
double GE_AnalyzeSemeai::last_move_blocage = 100;

double GE_AnalyzeSemeai::semeai_atari_too_big = -300;          //(size>2)
double GE_AnalyzeSemeai::semeai_avoid_atari = -200;            //(size=2)
double GE_AnalyzeSemeai::semeai_save_big_atari = 400;

double GE_AnalyzeSemeai::semeai_capture_last_move = 500;
double GE_AnalyzeSemeai::semeai_last_move_avoid_connexion = 100;
double GE_AnalyzeSemeai::semeai_last_move_atari = 100; //50;
double GE_AnalyzeSemeai::semeai_win_race = 100;

double GE_AnalyzeSemeai::semeai_approach_move = 100;
double GE_AnalyzeSemeai::semeai_urgent_approach_move = 500;

double GE_AnalyzeSemeai::semeai_disconnection_double_cut = 30;
double GE_AnalyzeSemeai::semeai_disconnection_tobi = 30;
double GE_AnalyzeSemeai::semeai_connection_tobi = 10;
double GE_AnalyzeSemeai::semeai_blocage = 30;

double GE_AnalyzeSemeai::death_kill_eye = 400;
double GE_AnalyzeSemeai::death_kill_semi_eye = 200;

double GE_AnalyzeSemeai::life_make_eye = 400;
double GE_AnalyzeSemeai::life_make_semi_eye = 200;


double GE_AnalyzeSemeai::tree_semeai_very_good_answer = 200;
double GE_AnalyzeSemeai::tree_semeai_good_answer = 100;
double GE_AnalyzeSemeai::tree_semeai_bad_answer = -100;
double GE_AnalyzeSemeai::tree_semeai_very_bad_answer = -200;

double GE_AnalyzeSemeai::tree_semeai_weight_amaf = 0;//100;

// struct SequenceKillStone
// {
//   vector<int> sequence;
// };

// struct KillStone
// {//   int goal;
  
//   list<SequenceKillStone> sequences;
  
//   vector<int> reduction_liberties;
// };

// void KillStone::init(int location, const GE_Chain& ch)
// {
//   list<int>::const_iterator i_fi = ch.free_intersections.begin();
  
//   while(i_fi!=ch.free_intersections.end())
//     {
//       reduction_liberties.push_back(*i_fi);
//       i_fi++;
//     }
// }


// void KillStone::init_solution()
// {
//   sequences.push_back(SequenceKillStone());
  
//   for(int i = 0; i<reduction_liberties.size())
//     {
//       sequences.sequence.push_back(reduction_liberties[i]);
//       sequences.sequence.push_back(GE_PASS_MOVE);
//     }
// }


GE_Semeai::GE_Semeai() 
{
  problem = GE_GOAL_SEMEAI;
  main_black_stone = GE_PASS_MOVE;
  main_white_stone = GE_PASS_MOVE;
  main_black_size = 0;
  main_white_size = 0;
  
  main_black_nb_liberties = 0;
  main_white_nb_liberties = 0;

  mode_big_location = false;

  main_tree_semeai = 0;
}

bool GE_Semeai::is_a_new_stone_semeai(const GE_Game& g, int location, 
				   list<int>* new_stones, 
				   GE_Marker* mk) const 
{
  bool is_checked = false;
  bool is_allocated = false;
  
  if(not mk)
    {
      mk = new GE_Marker(GE_Directions::board_area);
      is_allocated = true;
    }
  
  mk->update_treatment();
  
  const list<int>* semeai_stones = 0;
  
  if(g.tc==GE_BLACK)
    semeai_stones = &white_stones;
  else semeai_stones = &black_stones;
  
  list<int>::const_iterator i_s = semeai_stones->begin();
  
  while(i_s!=semeai_stones->end())
    {
      mk->set_treated(*i_s);
      i_s++;
    }
  
  if(new_stones)
    {
      list<int>::const_iterator i_ns = new_stones->begin();
      
      while(i_ns!=new_stones->end())
	{
	  mk->set_treated(*i_ns);
	  i_ns++;
	}
    }
  
  int big_location = GE_Bibliotheque::to_big_goban[location];
  
  
  for(int dir = GE_NORTH; dir<=GE_WEST; dir++)
    {
      int big_location_dir = big_location+GE_Directions::directions[dir];
      int location_dir = GE_Bibliotheque::to_goban[big_location_dir];

      if(location_dir<0) continue;
      
      if(mk->is_treated(location_dir))
	{
	  if(new_stones) 
	    new_stones->push_back(location);
	  
	  is_checked = true;
	  
	  break;
	}
    }
  
  if(is_allocated)
    {
      delete mk;
      mk = 0;
    }

  return is_checked;
}

bool GE_Semeai::main_stone_is_vital(int color_stone) const
{
  if(color_stone==GE_BLACK_STONE)
    {
      if(main_black_size==1)
	{
	  if(black_stones.size()==1)
	    return true;
	  
	  return false;
	}

      return (main_black_size>0);
    }
  else
    {
      if(main_white_size==1)
	{
	  if(white_stones.size()==1)
	    return true;
	  
	  return false;
	}
      
      return (main_white_size>0);
    }
}

bool GE_Semeai::is_a_semeai_stone(const GE_Game& g, 
			       int location) const
{
  switch((g.goban)->get_stone(location))
    {
    case GE_BLACK_STONE : 
      return GE_ListOperators::is_in(black_stones, location);
      
    case GE_WHITE_STONE : 
      return GE_ListOperators::is_in(white_stones, location);
      
    default: ;
    }
  
  return false; 
}

bool GE_Semeai::is_a_semeai_stone_fast(int location) const
{
  return marker_all_stones.is_treated(location);
}

bool GE_Semeai::is_a_vital_group(const GE_Chain& ch) const
{
  const list<int>* vital_groups = 0;
  int main_stone = 0;
  
  if(ch.stone==GE_BLACK_STONE)
    {
      vital_groups = &black_vital_stones;
      main_stone = main_black_stone;
    }
  else 
    {
      vital_groups = &white_vital_stones;
      main_stone = main_white_stone;
    }
  
  list<int>::const_iterator i_vg = vital_groups->begin();
  
  while(i_vg!=vital_groups->end())
    {
      if(ch.belongs(*i_vg))
	{
	  return true;
	}
      
      i_vg++;
    }
  
  if(main_stone==GE_PASS_MOVE)
    return false;
  
  
  if(main_stone_is_vital(ch.stone))
    {
      if(ch.belongs(main_stone))
	return true;
    }
  
  return false;
}


bool GE_Semeai::is_a_vital_group(const GE_Game& g, GE_Informations& infos, 
			      int num_group, int main_opp_num_group, 
			      GE_Marker* mk, bool with_connected)
{
  
  GE_GroupInformation& gi = *((infos.groups)[num_group]);
  
  if(mk)
    {
      list<int>::const_iterator i_s = gi.ch.links.begin();
      
      while(i_s!=gi.ch.links.end())
	{
	  mk->set_treated(*i_s);
	  i_s++;
	}
    }
  
  
  if((with_connected)
     &&(not GE_VectorOperators::is_in(gi.neighbour_groups, main_opp_num_group)))
    return false;
  
  for(int i = 0; i<(int)gi.neighbour_groups.size(); i++)
    {
      int temp_num_group = gi.neighbour_groups[i];
      
      if(temp_num_group==main_opp_num_group)
	continue;
      
      
      GE_GroupInformation& temp_gi = *((infos.groups)[temp_num_group]);
      if(temp_gi.get_size()==1) continue;
      
      int temp_stone = temp_gi.get_one_stone();
 
      if(not is_a_semeai_stone(g, temp_stone))
	return true;
    }
  
  
  return false;
}
  

void GE_Semeai::init_vital_stones(const GE_Game& g, 
			       GE_Informations& infos, 
			       GE_Marker* mk)
{
  bool is_allocated = false;
  
  if(not mk)
    {
      mk = new GE_Marker(GE_Directions::board_area);
      is_allocated = true;
    }
  
  mk->update_treatment();
  
  int black_main_num_group = -1;
  int white_main_num_group = -1;

  if(main_black_stone>=0)
  {
  assert((g.goban)->get_stone(main_black_stone)==GE_BLACK_STONE);
    black_main_num_group = infos.get_index(g, main_black_stone);
  }
  
  if(main_white_stone>=0)
  {
  assert((g.goban)->get_stone(main_white_stone)==GE_WHITE_STONE);
    white_main_num_group = infos.get_index(g, main_white_stone);
  }
  
  
  if((black_main_num_group>=0)
     &&(main_stone_is_vital(GE_BLACK_STONE)))
    {
      const GE_GroupInformation& gi = *(infos.groups[black_main_num_group]);
      list<int>::const_iterator i_s = gi.ch.links.begin();
      while(i_s!=gi.ch.links.end())
	{
	  mk->set_treated(*i_s);
	  
	  i_s++;
	}
    }
  
  
  if((white_main_num_group>=0)
     &&(main_stone_is_vital(GE_WHITE_STONE)))
    {
      const GE_GroupInformation& gi = *(infos.groups[white_main_num_group]);
      list<int>::const_iterator i_s = gi.ch.links.begin();
      while(i_s!=gi.ch.links.end())
	{
	  mk->set_treated(*i_s);
	  
	  i_s++;
	}
    }  
  
  
  if((white_main_num_group>=0)
     &&((infos.groups[white_main_num_group])->get_size()>1))
    {
      list<int>::const_iterator i_b = black_stones.begin();
      while(i_b!=black_stones.end())
	{
	  int black_location = *i_b;
	  i_b++;
	  
	  if(mk->is_treated(black_location))
	    continue;
	  
	  
	  int num_group = infos.get_index(g, black_location);
	  
	  if(is_a_vital_group(g, infos, num_group, white_main_num_group, mk))
	    black_vital_stones.push_back(black_location);
	  
	}
    }
  
  if((black_main_num_group>=0)
     &&((infos.groups[black_main_num_group])->get_size()>1))
    {
      list<int>::const_iterator i_w = white_stones.begin();
      while(i_w!=white_stones.end())
	{
	  int white_location = *i_w;
	  i_w++;
	  
	  if(mk->is_treated(white_location))
	    continue;
	  
	  
	  int num_group = infos.get_index(g, white_location);
	  
	  if(is_a_vital_group(g, infos, num_group, black_main_num_group, mk))
	    white_vital_stones.push_back(white_location);
	  
	}
    }
  
  if(is_allocated)
    {
      delete mk;
      mk = 0;
    }
}

void GE_Semeai::init_frontiers(const GE_Game& g, GE_Informations& infos)
{
  int black_main_num_group = -1;
  
  if(main_black_stone>=0)
  {
  assert((g.goban)->get_stone(main_black_stone)==GE_BLACK_STONE);
    black_main_num_group = infos.get_index(g, main_black_stone);
  }
  
  if((black_main_num_group>=0)
     &&(main_stone_is_vital(GE_BLACK_STONE)))
    {
      const GE_GroupInformation& gi = *(infos.groups[black_main_num_group]);
      
      for(int i = 0; i<(int)gi.neighbour_groups.size(); i++)
	{
	  int num_neighbour = (gi.neighbour_groups)[i];
	  
	  const GE_GroupInformation& ngi = *(infos.groups[num_neighbour]);
	  
	  int frontier_stone = ngi.get_one_stone();
	  
	  if(not is_a_semeai_stone(g, frontier_stone))
	    {
	      white_frontiers.push_back(frontier_stone);
	      
	      if((ngi.get_size()>=3)) //TODO DEATH to improve +3
		//||(is_a_vital_group(g, infos, num_neighbour, 
		//black_main_num_group)))
		white_vital_frontiers.push_back(frontier_stone);
	    }
	}
    }
  
  int white_main_num_group = -1;
  
  if(main_white_stone>=0)
  {
  assert((g.goban)->get_stone(main_white_stone)==GE_WHITE_STONE);
    white_main_num_group = infos.get_index(g, main_white_stone);
  }
  
  
  if((white_main_num_group>=0)
     &&(main_stone_is_vital(GE_WHITE_STONE)))
    {
      const GE_GroupInformation& gi = *(infos.groups[white_main_num_group]);
      
      for(int i = 0; i<(int)gi.neighbour_groups.size(); i++)
	{
	  int num_neighbour = (gi.neighbour_groups)[i];
	  
	  const GE_GroupInformation& ngi = *(infos.groups[num_neighbour]);
	  
	  int frontier_stone = ngi.get_one_stone();
	  
	  if(not is_a_semeai_stone(g, frontier_stone))
	    {
	      black_frontiers.push_back(frontier_stone);
	      
	      if((ngi.get_size()>=3))
		//||(is_a_vital_group(g, infos, num_neighbour, 
		//white_main_num_group)))
		black_vital_frontiers.push_back(frontier_stone);
	    }
	}
    } 
}




/*
  bool GE_Semeai::is_outside(int location) const
  {
  
  
  }
*/

void GE_Semeai::tools_liberties(GE_Game& g, 
			     GE_SemeaiTools& st, 
			     int free_location, 
			     int color_stone) const
{
  list<int>* solid_liberties = 0;
  list<int>* eyes = 0;

  if(color_stone==GE_BLACK_STONE)
    {
      solid_liberties = &st.black_solid_liberties;
      eyes = &st.black_eyes;
    }
  else
    {
      solid_liberties = &st.white_solid_liberties;
      eyes = &st.white_eyes;
    }
  
  bool generate_pass_move = false;
  
  if(((g.tc==GE_BLACK)&&(color_stone==GE_BLACK_STONE))
     ||((g.tc==GE_WHITE)&&(color_stone==GE_WHITE_STONE)))
    {
      g.play(GE_PASS_MOVE);
      generate_pass_move = true;
    }
  
  if(g.play(free_location))
    {
  assert((g.goban)->get_stone(free_location)!=GE_WITHOUT_STONE);
      GE_Chain ch;
      ch.init(*(g.goban), free_location);
      
      if(ch.is_atari())
	{
	  solid_liberties->push_back(free_location);
	}
      
      g.backward();
    }
  else
    {
      GE_ListOperators::push_if_not(*eyes, free_location);
    }
  
  
  if(generate_pass_move)
    g.backward();  
}


void GE_Semeai::tools_liberties(GE_Game& g, 
			     GE_SemeaiTools& st) const
{
       const GE_Semeai  * semeai=this;	
 /* fprintf(stderr,"affichage du goban juste avant plantage:\n");fflush(stderr);
  g.print_error_goban();*/
  { list<int>::const_iterator it = semeai->white_vital_stones.begin(); while (it!=semeai->white_vital_stones.end()) { /*cerr<<"location:" << move_to_string(GE_Directions::board_size,*it)<<" " << (g.goban)->get_stone(*it)<<endl;*/ assert((g.goban)->get_stone(*it)==GE_WHITE_STONE); it++; }} { list<int>::const_iterator it = semeai->black_vital_stones.begin(); while (it!=semeai->black_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_BLACK_STONE); it++; }} // BLIBLI
  list<int>::const_iterator i_bv = black_vital_stones.begin();
  while(i_bv!=black_vital_stones.end())
    {
      int black_location = *i_bv;
      i_bv++;
      
      GE_Chain ch;
      if (g.goban->get_stone(black_location)!=GE_BLACK_STONE)
      {
	      g.print_error_goban(); cerr<<endl;
	      this->print_error_semeai(true);cerr<<endl;fflush(stderr);
      }
      assert(g.goban->get_stone(black_location)==GE_BLACK_STONE);
      ch.init(*(g.goban), black_location);
      
      GE_ListOperators::merge(ch.free_intersections, st.black_liberties);

      list<int>::const_iterator i_fi = ch.free_intersections.begin();
      while(i_fi!=ch.free_intersections.end())
	{
	  tools_liberties(g, st, *i_fi, GE_BLACK_STONE);
	  
	  i_fi++;
	}
    }
  
  { list<int>::const_iterator it = semeai->white_vital_stones.begin(); while (it!=semeai->white_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_WHITE_STONE); it++; }} { list<int>::const_iterator it = semeai->black_vital_stones.begin(); while (it!=semeai->black_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_BLACK_STONE); it++; }} // BLIBLI
  list<int>::const_iterator i_wv = white_vital_stones.begin();
  while(i_wv!=white_vital_stones.end())
    {
      int white_location = *i_wv;
      i_wv++;
      
      GE_Chain ch;
      if (g.goban->get_stone(white_location)!=GE_WHITE_STONE)
      {
	      g.print_error_goban(); cerr<<endl;
	      this->print_error_semeai(true);cerr<<endl;fflush(stderr);
      }
      assert(g.goban->get_stone(white_location)==GE_WHITE_STONE);
      ch.init(*(g.goban), white_location);
      
      GE_ListOperators::merge(ch.free_intersections, st.white_liberties);
      
      list<int>::const_iterator i_fi = ch.free_intersections.begin();
      while(i_fi!=ch.free_intersections.end())
	{
	  tools_liberties(g, st, *i_fi, GE_WHITE_STONE);

	  i_fi++;
	}
      

    }
  
  if((main_black_stone>=0)
     &&(main_stone_is_vital(GE_BLACK_STONE)))
    {
  assert((g.goban)->get_stone(main_black_stone)!=GE_WITHOUT_STONE);
      GE_Chain ch;
      //g.print_error_goban();
      //cerr << main_black_stone<<endl;
     // ch.init(*(g.goban), main_black_stone);    // BUG1 OTJBH
      {
	      int mv = main_black_stone;
	      GE_Goban gob = *(g.goban);
 if(mv<0) assert(0);
 //fprintf(stderr,"<%d>",gob.height);fflush(stderr);
   pair<int, int> mv2 = int_to_pair(gob.height, mv);

     int** marker;
       GE_Allocator::init(&marker, gob.height, gob.width, GE_NOT_SEEN);
       assert(gob.board[mv2.first][mv2.second]==GE_BLACK_STONE);
         ch.init(gob, &marker, mv2.first, mv2.second);
	   GE_Allocator::desallocate(&marker, gob.height);

    }


      assert(ch.free_intersections.size()>0);
      assert(ch.free_intersections.size()<1000);
      GE_ListOperators::merge(ch.free_intersections, st.black_liberties);
      
      list<int>::const_iterator i_fi = ch.free_intersections.begin();
      while(i_fi!=ch.free_intersections.end())
	{
	  tools_liberties(g, st, *i_fi, GE_BLACK_STONE);
	  
	  i_fi++;
	}
    }
  
  if((main_white_stone>=0)
     &&(main_stone_is_vital(GE_WHITE_STONE)))
    {
	assert((g.goban)->get_stone(main_white_stone)==GE_WHITE_STONE);
      GE_Chain ch;
      ch.init(*(g.goban), main_white_stone);
      
      GE_ListOperators::merge(ch.free_intersections, st.white_liberties);

      list<int>::const_iterator i_fi = ch.free_intersections.begin();
      while(i_fi!=ch.free_intersections.end())
	{
	  tools_liberties(g, st, *i_fi, GE_WHITE_STONE);

	  i_fi++;
	}
    }
}

void GE_Semeai::tools_liberties(GE_Game& g)
{
  GE_SemeaiTools st;
  
  fprintf(stderr,"celui-la\n");fflush(stderr);
  tools_liberties(g, st);
  fprintf(stderr,"pas celui-la\n");fflush(stderr);

  black_liberties = st.black_liberties;
  white_liberties = st.white_liberties;
  
  black_solid_liberties = st.black_solid_liberties;
  white_solid_liberties = st.white_solid_liberties;

  black_eyes = st.black_eyes;
  white_eyes = st.white_eyes;
}


//the move is assumed NO played
double GE_Semeai::evaluate_by_answer(const GE_Game& g, const GE_TreeSemeai& ts, 
				  int mv) const
{
  
  if((ts.black_answers.size()==0)
     ||(ts.white_answers.size()==0))
    {
      //cerr<<"*** evaluate_by_answer *** ";
      //cerr<<"the tree_semeai answers is not initialized"<<endl;
      return 0;
    }

  
  int last_move = g.get_last_move();
  
  if(last_move==GE_ILLEGAL_MOVE) return 0;
  
  int mv1 = last_move;
  if(last_move==GE_PASS_MOVE)
    mv1 = GE_Directions::board_area;
  
  int mv2 = mv;
  if(mv==GE_PASS_MOVE)
    mv2 = GE_Directions::board_area;
  
  
  int points = 0;
  
  if(g.tc==GE_BLACK)
    points = (ts.black_answers)[mv1][mv2];
  else points = (ts.white_answers)[mv1][mv2];
  
  if((points<GE_WIN)&&(points>GE_LOSS)) return 0;
  
  
  if(points>=2*GE_WIN)
    return GE_AnalyzeSemeai::tree_semeai_very_good_answer;
  else 
    {
      if(points>=GE_WIN)
	return GE_AnalyzeSemeai::tree_semeai_good_answer;
      else
	{
	  if(points<=2*GE_LOSS)
	    return GE_AnalyzeSemeai::tree_semeai_very_bad_answer;
	  else
	    {
	      if(points<=GE_LOSS)
		return GE_AnalyzeSemeai::tree_semeai_bad_answer;
	    }
	}
      
    }
  
  
  return 0;
}

double GE_Semeai::evaluate_by_amaf(const GE_Game& g, const GE_TreeSemeai& ts, 
				int mv) const
{
  if((ts.black_amaf_wins.size()==0)
     ||(ts.white_amaf_wins.size()==0)
     ||(ts.black_amaf_sims.size()==0)
     ||(ts.white_amaf_sims.size()==0))
    {
      //cerr<<"*** evaluate_by_tree *** ";
      //cerr<<"the tree_semeai amaf is not initialized"<<endl;
      return 0;
    }


  int temp_mv = mv;
  if(temp_mv==GE_PASS_MOVE)
    temp_mv = GE_Directions::board_area;
  
  double amaf_win_rate = 0;
  
  if(g.tc==GE_BLACK)
    {
      amaf_win_rate = ts.black_amaf_wins[temp_mv]
	/(double)(ts.black_amaf_sims[temp_mv]+1);
    }
  else
    {
      amaf_win_rate = ts.white_amaf_wins[temp_mv]
	/(double)(ts.white_amaf_sims[temp_mv]+1);
    }
  
  return (amaf_win_rate-0.5f)*GE_AnalyzeSemeai::tree_semeai_weight_amaf;
  
}

double GE_Semeai::evaluate_by_tree(const GE_Game& g, const GE_TreeSemeai& ts, 
				int mv) const
{
  double eval_by_answer = evaluate_by_answer(g, ts, mv); 
  double eval_by_amaf = evaluate_by_amaf(g, ts, mv);

  double eval_tree = eval_by_answer+eval_by_amaf;

  return eval_tree;

}

double GE_Semeai::evaluate_move(GE_Game& g, int mv, const vector<double>& expertise, 
			     const GE_TreeSemeai* ts)
  const
{

	
  if(mv==GE_PASS_MOVE) return 0;
  
  double eval_by_tree = 0;
  
  if(ts)
    eval_by_tree = evaluate_by_tree(g, *ts, mv); 
  
  GE_SemeaiTools st;
  double eval_last_move = 0;
  
  if(not g.play(mv)) return GE_VAL_ILLEGAL_MOVE;
 
 
 
 { list<int>::const_iterator it = white_vital_stones.begin(); while (it!=white_vital_stones.end()) { if ((g.goban)->get_stone(*it)!=GE_WHITE_STONE) {g.backward();return 1000000;}; it++; }} { list<int>::const_iterator it = black_vital_stones.begin(); while (it!=black_vital_stones.end()) { if ((g.goban)->get_stone(*it)!=GE_BLACK_STONE) {g.backward();return 1000000;}; it++; }} 
	if((g.goban)->get_stone(main_black_stone)==GE_WITHOUT_STONE)
	{ 
		g.backward();
		if(main_stone_is_vital(GE_BLACK_STONE)) 
			return 1000000;
		else return 500;
	}
	if((g.goban)->get_stone(main_white_stone)==GE_WITHOUT_STONE)
        {
		g.backward();
                if(main_stone_is_vital(GE_WHITE_STONE))
                        return 1000000;
                else return 500;
        }
 
	

 
  fprintf(stderr,"celui-lo\n");fflush(stderr);
  tools_liberties(g, st);
  fprintf(stderr,"celui-lu\n");fflush(stderr);
  assert((g.goban)->get_stone(mv)!=GE_WITHOUT_STONE);
  GE_Chain ch; ch.init(*(g.goban), mv);
  
  if(ch.liberties<=2)
    eval_last_move = eval_last_move -   
      (GE_AnalyzeSemeai::last_move_liberties*(3-ch.liberties));
  
  if(not is_a_vital_group(ch))
    {
      eval_last_move+=GE_AnalyzeSemeai::semeai_solid_move;
    }
  
  if(ch.is_atari())
    {
      /*
	int approach_move = ch.free_intersections.back();
	
	if(ch.stone==GE_BLACK_STONE)  
	out_black_moves.push_back(approach_move);
	else out_white_moves.push_back(approach_move);
      */
      //new_evaluations[approach_move]+=semeai_approach_move;
      //expertise[approach_move]+=semeai_approach_move;
      
      
      if(is_a_vital_group(ch))
	{
	  g.backward();
	  return GE_VAL_SUICIDE_MOVE;
	}
      
      if(ch.size()>2)
	eval_last_move = eval_last_move + GE_AnalyzeSemeai::semeai_atari_too_big;
      else
	{
	  if(ch.size()==2)
	    eval_last_move = 
	      eval_last_move + GE_AnalyzeSemeai::semeai_avoid_atari;

	  if(ch.size()==1)
	    {
	      //TODO a affiner (pourquoi se mettre en atari) ?
	      eval_last_move = 
		eval_last_move + GE_AnalyzeSemeai::semeai_avoid_atari;
	    }


	}

      
    }
  
  g.backward();
  
  
  double black_eval_liberties = 0;
  double white_eval_liberties = 0;
  double eval_liberties = 0;
  
  double black_eval_solid_liberties = 0;
  double white_eval_solid_liberties = 0;
  double eval_solid_liberties = 0;
  
  double black_eval_eyes = 0;
  double white_eval_eyes = 0;
  double eval_eyes = 0;
  
  double eval = 0;
  
  black_eval_liberties = (int)st.black_liberties.size()
    -(int)black_liberties.size();
  white_eval_liberties = (int)st.white_liberties.size()
    -(int)white_liberties.size();
  
  black_eval_solid_liberties = (int)st.black_solid_liberties.size()
    -(int)black_solid_liberties.size();
  white_eval_solid_liberties = (int)st.white_solid_liberties.size()
    -(int)white_solid_liberties.size();
  
  black_eval_eyes = (int)st.black_eyes.size()-(int)black_eyes.size();
  white_eval_eyes = (int)st.white_eyes.size()-(int)white_eyes.size();
  
  if(g.tc==GE_BLACK)
    {
      eval_liberties = black_eval_liberties-white_eval_liberties;
      eval_solid_liberties = black_eval_solid_liberties
	-white_eval_solid_liberties; //forcera a jouer sur les coups atari ?
      eval_eyes = black_eval_eyes-white_eval_eyes;
    }
  else
    {
      eval_liberties = white_eval_liberties-black_eval_liberties;
      eval_solid_liberties = white_eval_solid_liberties
	-black_eval_solid_liberties; //forcera a jouer sur les coups atari ?
      eval_eyes = white_eval_eyes-black_eval_eyes;
    }
  
  eval_liberties*=GE_AnalyzeSemeai::semeai_liberties;
  eval_solid_liberties*=GE_AnalyzeSemeai::semeai_solid_liberties;
  eval_eyes*=GE_AnalyzeSemeai::semeai_eyes;
  
  eval = eval_liberties+eval_solid_liberties+eval_eyes
    +eval_last_move+eval_by_tree+expertise[mv];
  
  return eval;
}

void GE_Semeai::eval_all_moves(GE_Game& g, const vector<double>& expertise, 
			    const list<int>& l_moves, 
			    list<pair<int, double> >& l_eval, 
			    const GE_TreeSemeai* ts) const
{
  list<int>::const_iterator i_mv = l_moves.begin();
  
  while(i_mv!=l_moves.end())
    {
      double eval = evaluate_move(g, *i_mv, expertise, ts);
      l_eval.push_back(make_pair(*i_mv, eval));
      
      i_mv++;
    }
}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

double GE_Semeai::evaluate_move2(GE_Game& g, int mv, vector<double>& expertise, 
			      list<int>& new_moves, const GE_TreeSemeai* ts)
  const
{
	const GE_Semeai* semeai = this;

  if(mv==GE_PASS_MOVE) return 0;
  
  double eval_by_tree = 0;

  if(ts)
    eval_by_tree = evaluate_by_tree(g, *ts, mv); 
  

  GE_SemeaiTools st;
  double eval_last_move = 0;
  
////  { list<int>::const_iterator it = semeai->white_vital_stones.begin(); while (it!=semeai->white_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_WHITE); it++; }} { list<int>::const_iterator it = semeai->black_vital_stones.begin(); while (it!=semeai->black_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_BLACK); it++; }} // BLIBLI
  if(not g.play(mv)) 
    {
// // { list<int>::const_iterator it = semeai->white_vital_stones.begin(); while (it!=semeai->white_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_WHITE); it++; }} { list<int>::const_iterator it = semeai->black_vital_stones.begin(); while (it!=semeai->black_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_BLACK); it++; }} // BLIBLI
      expertise[mv] = GE_VAL_ILLEGAL_MOVE;
      return GE_VAL_ILLEGAL_MOVE;
    }
  
  { list<int>::const_iterator it = semeai->white_vital_stones.begin(); while (it!=semeai->white_vital_stones.end()) { if ((g.goban)->get_stone(*it)!=GE_WHITE_STONE) {expertise[mv]=1000000;g.backward();return 1000000;}; it++; }} { list<int>::const_iterator it = semeai->black_vital_stones.begin(); while (it!=semeai->black_vital_stones.end()) { if ((g.goban)->get_stone(*it)!=GE_BLACK_STONE) {expertise[mv]=1000000;g.backward();return 1000000;}; it++; }} 
	
	if((g.goban)->get_stone(main_black_stone)==GE_WITHOUT_STONE)
        {
                g.backward();
                if(main_stone_is_vital(GE_BLACK_STONE))
		{
			expertise[mv] = 1000000;
                        return 1000000;
		}
                else 
		{
			expertise[mv] = 500;
			return 500;
		}
        }
        if((g.goban)->get_stone(main_white_stone)==GE_WITHOUT_STONE)
        {
                g.backward();
                if(main_stone_is_vital(GE_WHITE_STONE))
		{
			expertise[mv] = 1000000;
                        return 1000000;
		}
                else 
		{
			expertise[mv] = 500;
			return 500;
		}
        }




  //fprintf(stderr,"celui-ci\n");fflush(stderr);
////  { list<int>::const_iterator it = semeai->white_vital_stones.begin(); while (it!=semeai->white_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_WHITE); it++; }} { list<int>::const_iterator it = semeai->black_vital_stones.begin(); while (it!=semeai->black_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_BLACK); it++; }} // BLIBLI

  tools_liberties(g, st);
 // fprintf(stderr,"pas celui-ci\n");fflush(stderr);
  assert((g.goban)->get_stone(mv)!=GE_WITHOUT_STONE);
  GE_Chain ch; ch.init(*(g.goban), mv);
  g.backward();
  
////  { list<int>::const_iterator it = semeai->white_vital_stones.begin(); while (it!=semeai->white_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_WHITE); it++; }} { list<int>::const_iterator it = semeai->black_vital_stones.begin(); while (it!=semeai->black_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_BLACK); it++; }} // BLIBLI
  
  if(ch.liberties<=2)
    eval_last_move = eval_last_move -   
      (GE_AnalyzeSemeai::last_move_liberties*(3-ch.liberties));
  
////  { list<int>::const_iterator it = semeai->white_vital_stones.begin(); while (it!=semeai->white_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_WHITE); it++; }} { list<int>::const_iterator it = semeai->black_vital_stones.begin(); while (it!=semeai->black_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_BLACK); it++; }} // BLIBLI
  
  if(not is_a_vital_group(ch))
    {
      eval_last_move+=GE_AnalyzeSemeai::semeai_solid_move;
    }
  
////  { list<int>::const_iterator it = semeai->white_vital_stones.begin(); while (it!=semeai->white_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_WHITE); it++; }} { list<int>::const_iterator it = semeai->black_vital_stones.begin(); while (it!=semeai->black_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_BLACK); it++; }} // BLIBLI
  if(ch.is_atari())
    {
      int approach_move = ch.free_intersections.back();
      
////  { list<int>::const_iterator it = semeai->white_vital_stones.begin(); while (it!=semeai->white_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_WHITE); it++; }} { list<int>::const_iterator it = semeai->black_vital_stones.begin(); while (it!=semeai->black_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_BLACK); it++; }} // BLIBLI
      if((expertise[approach_move]!=GE_VAL_ILLEGAL_MOVE)
	 &&(expertise[approach_move]!=GE_VAL_SUICIDE_MOVE))
	{
// // { list<int>::const_iterator it = semeai->white_vital_stones.begin(); while (it!=semeai->white_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_WHITE); it++; }} { list<int>::const_iterator it = semeai->black_vital_stones.begin(); while (it!=semeai->black_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_BLACK); it++; }} // BLIBLI
	  g.play(mv);
	  g.play(GE_PASS_MOVE);
	  
	  if(g.play(approach_move))
	    {
	      GE_Chain ch_approach;
  assert((g.goban)->get_stone(approach_move)!=GE_WITHOUT_STONE);
	      ch_approach.init(*(g.goban), approach_move);
	      
	      g.backward(); //approach_move
	      
	      new_moves.push_back(approach_move);
	      
	      if(not ch_approach.is_atari()) 
		{
		  expertise[approach_move]+=
		    GE_AnalyzeSemeai::semeai_approach_move;
		  
		  //TODO 
		  //optimization doit couter tres cher en temps de calcul
		  
		  GE_Chains neighbours;
		  neighbours.neighbour_chains(*(g.goban), mv);
		  
		  list<GE_Chain>::const_iterator i_chn = neighbours.sets.begin();
		  while(i_chn!=neighbours.sets.end())
		    { 
		      if((i_chn->is_atari())
			 &&(i_chn->size()>=2)
			 &&(i_chn->stone!=ch.stone))
			{
			  int other_attack = 
			    i_chn->free_intersections.front();
			  
			  (g.goban)->set_stone(other_attack, ch.stone);
			  
			  GE_Chain ch_attack;
  assert((g.goban)->get_stone(other_attack)!=GE_WITHOUT_STONE);
			  ch_attack.init(*(g.goban), other_attack);
			  
			  (g.goban)->unset_stone(other_attack);
			  
			  
			  if(ch_attack.liberties<=1)
			    {
			      
			      (g.goban)->set_stone(approach_move, ch.stone);
			      
			      GE_Chains temp_neighbours;
			      temp_neighbours.neighbour_chains(*(g.goban), *i_chn);
			      
			      (g.goban)->unset_stone(approach_move);
			      
			      
			      if(temp_neighbours.get_nb_atari(g.tc)==0)
				{
				  //cerr<<"urgent approach move : ";
				  //cerr<<move_to_string(GE_Directions::board_size, 
				  //		       approach_move)<<endl;
				  //(g.goban)->print_error(mv);
				  //cerr<<endl;
				  
				  if((ch_approach.liberties>2)||(ch_attack.liberties==0))
				    {
				      expertise[approach_move]+=
					GE_AnalyzeSemeai::semeai_urgent_approach_move;
				    }
				  break;
				}
			      else
				{
				  //cerr<<"not urgent : ";
				  //cerr<<move_to_string(GE_Directions::board_size, 
				  //approach_move)<<endl;
				  //(g.goban)->print_error(mv);
				  //cerr<<endl;
				}
			    }
			}
		      
		      i_chn++;
		    }
		}
	      else
		{
		  //cerr<<"stupid approach: "<<endl;
		  
		  //cerr<<move_to_string(GE_Directions::board_size, 
		  //approach_move)<<endl;
		}
	    }
	  
	  g.backward(); //GE_PASS_MOVE
	  g.backward(); //mv
////  { list<int>::const_iterator it = semeai->white_vital_stones.begin(); while (it!=semeai->white_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_WHITE); it++; }} { list<int>::const_iterator it = semeai->black_vital_stones.begin(); while (it!=semeai->black_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_BLACK); it++; }} // BLIBLI
	}
  
      if(is_a_vital_group(ch))
	{
	  //cerr<<"suicide move: ";
	  //cerr<<move_to_string(GE_Directions::board_size, mv)<<endl;
	  
////  { list<int>::const_iterator it = semeai->white_vital_stones.begin(); while (it!=semeai->white_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_WHITE); it++; }} { list<int>::const_iterator it = semeai->black_vital_stones.begin(); while (it!=semeai->black_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_BLACK); it++; }} // BLIBLI
	  expertise[mv] = GE_VAL_SUICIDE_MOVE;
	  return GE_VAL_SUICIDE_MOVE;
	}
      
////  { list<int>::const_iterator it = semeai->white_vital_stones.begin(); while (it!=semeai->white_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_WHITE); it++; }} { list<int>::const_iterator it = semeai->black_vital_stones.begin(); while (it!=semeai->black_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_BLACK); it++; }} // BLIBLI
      if(ch.size()>2)
	eval_last_move = eval_last_move + GE_AnalyzeSemeai::semeai_atari_too_big;
      else
	{
	  if(ch.size()==2)
	    eval_last_move = 
	      eval_last_move + GE_AnalyzeSemeai::semeai_avoid_atari;
	  
	  if(ch.size()==1)
	    {
	      //TODO a affiner (pourquoi se mettre en atari) ?
	      eval_last_move = 
		eval_last_move + GE_AnalyzeSemeai::semeai_avoid_atari;
	    }
	}
////  { list<int>::const_iterator it = semeai->white_vital_stones.begin(); while (it!=semeai->white_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_WHITE); it++; }} { list<int>::const_iterator it = semeai->black_vital_stones.begin(); while (it!=semeai->black_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_BLACK); it++; }} // BLIBLI
    }
  
  double black_eval_liberties = 0;
  double white_eval_liberties = 0;
  double eval_liberties = 0;
  
  double black_eval_solid_liberties = 0;
  double white_eval_solid_liberties = 0;
  double eval_solid_liberties = 0;
  
  double black_eval_eyes = 0;
  double white_eval_eyes = 0;
  double eval_eyes = 0;
  
  double eval = 0;
  
  black_eval_liberties = (int)st.black_liberties.size()
    -(int)black_liberties.size();
  white_eval_liberties = (int)st.white_liberties.size()
    -(int)white_liberties.size();
  
  black_eval_solid_liberties = (int)st.black_solid_liberties.size()
    -(int)black_solid_liberties.size();
  white_eval_solid_liberties = (int)st.white_solid_liberties.size()
    -(int)white_solid_liberties.size();
  
  black_eval_eyes = (int)st.black_eyes.size()-(int)black_eyes.size();
  white_eval_eyes = (int)st.white_eyes.size()-(int)white_eyes.size();
  
  if(g.tc==GE_BLACK)
    {
      eval_liberties = black_eval_liberties-white_eval_liberties;
      eval_solid_liberties = black_eval_solid_liberties
	-white_eval_solid_liberties; //forcera a jouer sur les coups atari ?
      eval_eyes = black_eval_eyes-white_eval_eyes;
    }
  else
    {
      eval_liberties = white_eval_liberties-black_eval_liberties;
      eval_solid_liberties = white_eval_solid_liberties
	-black_eval_solid_liberties; //forcera a jouer sur les coups atari ?
      eval_eyes = white_eval_eyes-black_eval_eyes;
    }
  
////  { list<int>::const_iterator it = semeai->white_vital_stones.begin(); while (it!=semeai->white_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_WHITE); it++; }} { list<int>::const_iterator it = semeai->black_vital_stones.begin(); while (it!=semeai->black_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_BLACK); it++; }} // BLIBLI
  eval_liberties*=GE_AnalyzeSemeai::semeai_liberties;
  eval_solid_liberties*=GE_AnalyzeSemeai::semeai_solid_liberties;
  eval_eyes*=GE_AnalyzeSemeai::semeai_eyes;
  
  eval = eval_liberties+eval_solid_liberties+eval_eyes
    +eval_last_move+eval_by_tree+expertise[mv];
  
////  { list<int>::const_iterator it = semeai->white_vital_stones.begin(); while (it!=semeai->white_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_WHITE); it++; }} { list<int>::const_iterator it = semeai->black_vital_stones.begin(); while (it!=semeai->black_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_BLACK); it++; }} // BLIBLI
  
  if((expertise[mv]!=GE_VAL_SUICIDE_MOVE)
     ||(expertise[mv]!=GE_VAL_ILLEGAL_MOVE))
    expertise[mv] = eval;
  
////  { list<int>::const_iterator it = semeai->white_vital_stones.begin(); while (it!=semeai->white_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_WHITE); it++; }} { list<int>::const_iterator it = semeai->black_vital_stones.begin(); while (it!=semeai->black_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_BLACK); it++; }} // BLIBLI
  return eval;
}

void GE_Semeai::eval_all_moves2(GE_Game& g, vector<double>& expertise, 
			     const list<int>& l_moves, 
			     list<pair<int, double> >& l_eval, 
			     const GE_TreeSemeai* ts) const
{
  list<int>::const_iterator i_mv = l_moves.begin();
  
  list<int> all_moves = l_moves;
  
  list<int> new_moves;
  
  while(i_mv!=l_moves.end())
    {
      evaluate_move2(g, *i_mv, expertise, new_moves, ts);    
      i_mv++;
    }

  while(not new_moves.empty())
    {
      int mv = new_moves.front();
      new_moves.pop_front();
      
      if(GE_ListOperators::push_if_not(all_moves, mv))
	evaluate_move2(g, mv, expertise, new_moves, ts);
    }

  while(not all_moves.empty())
    {
      int mv = all_moves.front();
      all_moves.pop_front();
      l_eval.push_back(make_pair(mv, expertise[mv]));
    }
}



//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


int GE_Semeai::get_best_move(list<pair<int, double> >& l_eval) const
{
  int best_move = GE_PASS_MOVE;
  double best_eval = GE_VAL_ILLEGAL_MOVE;
  list<pair<int, double> >::iterator i_mv = l_eval.begin();
  int modulo = 2;


  list<pair<int, double> >::iterator iter = l_eval.begin();
  while(iter!=l_eval.end())
    {

      if(iter->second==best_eval)
	{
	  if(rand()%modulo==0)
	    {
	      best_move = iter->first;
	      i_mv = iter;
	    }
	  modulo++;
	}
      
      if(iter->second>best_eval)
	{
	  i_mv = iter;
	  best_move = iter->first;
	  best_eval = iter->second;
	  modulo = 2;
	}
      
      iter++;
    }

  //assert(i_mv->first==best_move);

  if(best_move!=GE_PASS_MOVE)
    l_eval.erase(i_mv);
  

  return best_move;
}




void GE_Semeai::to_mode_big_location()
{
  list<int>::iterator i_s = black_stones.begin();
  while(i_s!=black_stones.end())
    {
      *i_s = GE_Bibliotheque::to_big_goban[*i_s];
      i_s++;
    }
  
  i_s = white_stones.begin();
  while(i_s!=white_stones.end())
    {
      *i_s = GE_Bibliotheque::to_big_goban[*i_s];
      i_s++;
    }
  
  if(main_black_stone>=0)
    main_black_stone = GE_Bibliotheque::to_big_goban[main_black_stone];
  
  if(main_white_stone>=0)
    main_white_stone = GE_Bibliotheque::to_big_goban[main_white_stone];
  
  mode_big_location = true;
}


void GE_Semeai::init_tree(GE_Game& g, GE_Informations* infos)
{
  tree_semeai.init(this, &g, infos);
}


void GE_Semeai::init_all_trees(GE_Game& g, GE_Informations* infos)
{
  tree_semeai.init(this, &g, infos);
  tree_semeai.module_ko = true;
  after_pass.init(this, &g, infos);
  after_pass.module_ko = false;
  with_ko.init(this, &g, infos);
  with_ko.module_ko = false;
}

void GE_Semeai::init_all_trees3(GE_Game& g, GE_Informations* infos)
{
  only_win.init(this, &g, infos);
  only_win.main_goal = GE_TO_WIN;
  
  at_least_win_with_ko.init(this, &g, infos);
  at_least_win_with_ko.main_goal = GE_TO_WIN_WITH_KO;
  
  at_least_draw.init(this, &g, infos);
  at_least_draw.main_goal = GE_TO_DRAW;
  
  at_least_draw_with_ko.init(this, &g, infos);
  at_least_draw_with_ko.main_goal = GE_TO_DRAW_WITH_KO;
  
  at_least_loss_with_ko.init(this, &g, infos);
  at_least_loss_with_ko.main_goal = GE_TO_LOSS_WITH_KO;
}

int GE_Semeai::eval_semeai() const
{
  return main_black_size+main_white_size;
}


  
int GE_Semeai::solve(GE_Game& g, GE_Informations* infos, 
		  int nb_sims_by_packs, int max_packs, int* best_mv, 
		  int study_pass, int mode_print)
{
  if(not solveur.is_init)
    solveur.init(*this, g, infos, study_pass);
  
  int res = 
    solveur.solve(g, nb_sims_by_packs, max_packs, best_mv, mode_print);  
  
  return res;
}


bool GE_Semeai::is_a_real_semeai(const GE_Game& g, GE_Informations& infos, 
			      bool* semeai_black, bool* semeai_white)
{
  if(problem!=GE_GOAL_SEMEAI) return false;
  
  bool black_vital = false;
  bool white_vital = false;
  
  assert((g.goban)->get_stone(main_black_stone)==GE_BLACK_STONE);
  assert((g.goban)->get_stone(main_white_stone)==GE_WHITE_STONE);
  int black_num_group = infos.get_index(g, main_black_stone);
  int white_num_group = infos.get_index(g, main_white_stone);
  
  if(not black_vital_stones.empty())
    {
      black_vital = true;
    }
  
  if(not black_vital)
    { 
      if(is_a_vital_group(g, infos, black_num_group, white_num_group, 0, false))
	black_vital = true;
    }
  
  
  if(not white_vital_stones.empty())
    {
      white_vital = true;
    }
  
  if(not white_vital)
    {
      if(is_a_vital_group(g, infos, white_num_group, black_num_group, 0, false))
	white_vital = true;
    }
  
  
  if(semeai_black)
    *semeai_black = black_vital;
  
  if(semeai_white)
    *semeai_white = white_vital;
  
  return ((black_vital)&&(white_vital));
}



void GE_Semeai::print_semeai() const
{
  string msg_pb;
  GE_Tactic::problem_to_string(problem, msg_pb);
  
  cout<<"problem: "<<msg_pb<<endl;


  cout<<"black stones: "<<endl;
  GE_ListOperators::print(black_stones, move_to_string, 
		       GE_Directions::board_size);
  cout<<"main black stone ";
  if(main_black_stone>=0)
    cout<<move_to_string(GE_Directions::board_size, main_black_stone);
  else cout<<"pass move";
  cout<<"  (size = "<<main_black_size<<")"<<endl;
  cout<<"vital black stones ";
  GE_ListOperators::print(black_vital_stones, move_to_string, 
		       GE_Directions::board_size);
  
  cout<<"white stones: "<<endl;
  GE_ListOperators::print(white_stones, move_to_string, 
		       GE_Directions::board_size);
  cout<<"main white stone ";
  if(main_white_stone>=0)
    cout<<move_to_string(GE_Directions::board_size, main_white_stone);
  else cout<<"pass move";
  cout<<"  (size = "<<main_white_size<<")"<<endl;
  cout<<"vital white stones ";
  GE_ListOperators::print(white_vital_stones, move_to_string, 
		       GE_Directions::board_size);
}




void GE_Semeai::print_error_semeai(bool detail) const
{
  string msg_pb;
  GE_Tactic::problem_to_string(problem, msg_pb);
  
  cerr<<"problem: "<<msg_pb<<endl;

  cerr<<"black stones: "<<endl;
  GE_ListOperators::print_error(black_stones, move_to_string, 
			     GE_Directions::board_size);
  cerr<<"main black stone ";
  if(main_black_stone>=0)
    cerr<<move_to_string(GE_Directions::board_size, main_black_stone);
  else cerr<<"pass move";
  cerr<<"  (size = "<<main_black_size<<" - ";
  cerr<<"liberties = "<<main_black_nb_liberties<<")"<<endl;
  cerr<<"vital black stones ";
  GE_ListOperators::print_error(black_vital_stones, move_to_string, 
			     GE_Directions::board_size);
  cerr<<"black frontiers ";
  GE_ListOperators::print_error(black_frontiers, move_to_string, 
			     GE_Directions::board_size);
  cerr<<"black vital frontiers ";
  GE_ListOperators::print_error(black_vital_frontiers, move_to_string, 
			     GE_Directions::board_size);
  
  if(detail)
    {
      cerr<<"black important liberties ";
      GE_ListOperators::print_error(black_liberties, move_to_string, 
				 GE_Directions::board_size);
      
      cerr<<"black solid liberties ";
      GE_ListOperators::print_error(black_solid_liberties, move_to_string, 
				 GE_Directions::board_size);
      
      cerr<<"black eyes ";
      GE_ListOperators::print_error(black_eyes, move_to_string, 
				 GE_Directions::board_size);
    }
  
  cerr<<"white stones: "<<endl;
  GE_ListOperators::print_error(white_stones, move_to_string, 
			     GE_Directions::board_size);
  cerr<<"main white stone ";
  if(main_white_stone>=0)
    cerr<<move_to_string(GE_Directions::board_size, main_white_stone);
  else cerr<<"pass move";
  cerr<<"  (size = "<<main_white_size<<" - ";
  cerr<<"liberties = "<<main_white_nb_liberties<<")"<<endl;
  cerr<<"vital white stones ";
  GE_ListOperators::print_error(white_vital_stones, move_to_string, 
			     GE_Directions::board_size);
  cerr<<"white frontiers ";
  GE_ListOperators::print_error(white_frontiers, move_to_string, 
			     GE_Directions::board_size);
  cerr<<"white vital frontiers ";
  GE_ListOperators::print_error(white_vital_frontiers, move_to_string, 
			     GE_Directions::board_size);
  
  if(detail)
    {
      cerr<<"white important liberties ";
      GE_ListOperators::print_error(white_liberties, move_to_string, 
				 GE_Directions::board_size);
      
      cerr<<"white solid liberties ";
      GE_ListOperators::print_error(white_solid_liberties, move_to_string, 
				 GE_Directions::board_size);
      
      cerr<<"white eyes ";
      GE_ListOperators::print_error(white_eyes, move_to_string, 
				 GE_Directions::board_size);
    }
}




void GE_AnalyzeSemeai::to_mode_big_location()
{
  list<GE_Semeai>::iterator i_sem = semeais.begin();
  
  while(i_sem!=semeais.end())
    {
      i_sem->to_mode_big_location();
      i_sem++;
    }
}


/*
  void AnalyzeSemeais::mergeSemeais(GE_Semeai& sem1, GE_Semeai& sem2)
  {
  
  }
*/

void GE_AnalyzeSemeai::setSemeais(const GE_Game& g, GE_Marker& mk, int location, 
			       GE_Semeai& sem)
{
  if((g.goban)->get_stone(location)==GE_WITHOUT_STONE)
    cerr<<"*** ERROR *** setSemeais"<<endl;
  
  list<int>* sem_stones = 0;
  list<int>* sem_adv_stones = 0;
  
  if((g.goban)->get_stone(location)==GE_BLACK_STONE)
    {
      sem_stones = &(sem.black_stones);
      sem_adv_stones = &(sem.white_stones);
    }
  else
    {
      sem_stones = &(sem.white_stones);
      sem_adv_stones = &(sem.black_stones);
    }
  
  
  if(not mk.is_treated(location))
    {
      sem_stones->push_back(location);
      mk.set_treated(location);
      
      list<int>::const_iterator i_st = temp_semeais[location].begin();
      
      while(i_st!=temp_semeais[location].end())
	{
	  
	  if(not mk.is_treated(*i_st))
	    { 
	      //sem_adv_stones->push_back(*i_st);
	      //mk.set_treated(*i_st);
	      setSemeais(g, mk, *i_st, sem);
	    }
	  i_st++;
	}     
    }
}


void GE_AnalyzeSemeai::setSemeais(const GE_Game& g, GE_Marker& mk, GE_Chains& chs)
{
  
  mk.update_treatment();
  
  for(int i = 0; i<(int)temp_semeais.size(); i++)
    {
      if((temp_semeais[i].size()!=0)
	 &&(not mk.is_treated(i)))
	{
	  semeais.push_back(GE_Semeai());
	  GE_Semeai& sem = semeais.back();
	  
	  setSemeais(g, mk, i, sem);
	}
    }
  
  list<GE_Semeai>::iterator i_sem = semeais.begin();
  
  while(i_sem!=semeais.end())
    {
      i_sem->marker_all_stones.reinit(GE_Directions::board_area);


      list<int>::const_iterator i_bs = (i_sem->black_stones).begin();
      
      while(i_bs!=(i_sem->black_stones).end())
	{
	  (i_sem->marker_all_stones).set_treated(*i_bs);
	  
	  (chs.sets).push_back(GE_Chain());
  assert((g.goban)->get_stone(*i_bs)!=GE_WITHOUT_STONE);
	  ((chs.sets).back()).init(*(g.goban), *i_bs);
	  
	  if(i_sem->main_black_stone==GE_PASS_MOVE)
	    {
	      i_sem->main_black_size = ((chs.sets).back()).size();
	      i_sem->main_black_stone = *i_bs;
	      i_sem->main_black_nb_liberties = 
		((chs.sets).back()).liberties;
	    }
	  else
	    {
	      if(((chs.sets).back()).size()>i_sem->main_black_size)
		{
		  i_sem->main_black_size = ((chs.sets).back()).size();
		  i_sem->main_black_stone = *i_bs;
		  i_sem->main_black_nb_liberties = 
		    ((chs.sets).back()).liberties;
		}
	    }
	  
	  i_bs++;
	}
      
      list<int>::const_iterator i_ws = (i_sem->white_stones).begin();
      
      while(i_ws!=(i_sem->white_stones).end())
	{
	  (i_sem->marker_all_stones).set_treated(*i_ws);
	  
	  (chs.sets).push_back(GE_Chain());
  assert((g.goban)->get_stone(*i_ws)!=GE_WITHOUT_STONE);
	  ((chs.sets).back()).init(*(g.goban), *i_ws);
	  
	  if(i_sem->main_white_stone==GE_PASS_MOVE)
	    {
	      i_sem->main_white_size = (chs.sets).back().size();
	      i_sem->main_white_stone = *i_ws;
	      i_sem->main_white_nb_liberties = 
		((chs.sets).back()).liberties;
	    }
	  else
	    {
	      if((chs.sets).back().size()>i_sem->main_white_size)
		{
		  i_sem->main_white_size = (chs.sets).back().size();
		  i_sem->main_white_stone = *i_ws;
		  i_sem->main_white_nb_liberties = 
		    ((chs.sets).back()).liberties;
		}
	    }
	  
	  i_ws++;
	}
      
      i_sem++;
    }
  
}




void GE_AnalyzeSemeai::setSemeais(const GE_Game& g, vector<vector<int> >& v_semeai, 
			       int nb_sims, GE_Chains& chs)
{
  temp_semeais.clear();
  temp_semeais.resize(v_semeai.size());
  
  for(int i = 0; i<(int)v_semeai.size(); i++)
    {
      int color_stone = (g.goban)->get_stone(i);
      
      if(color_stone==GE_WITHOUT_STONE)
	continue;
      
      for(int j = i+1; j<(int)v_semeai[i].size(); j++)
	{
	  int color_stone2 = (g.goban)->get_stone(j);
	  
	  if(color_stone2==GE_WITHOUT_STONE)
	    continue;
	  
	  if(color_stone==color_stone2) continue;
	  
	  
	  if((((v_semeai[i][j]+v_semeai[j][i])/(float)nb_sims)>=GE_PARAM_SEMEAI_DETECTION_1)
	     &&((v_semeai[i][j]/(float)nb_sims)>GE_PARAM_SEMEAI_DETECTION_2)
	     &&((v_semeai[j][i]/(float)nb_sims)>GE_PARAM_SEMEAI_DETECTION_2))
	    {
	      temp_semeais[i].push_back(j);
	      temp_semeais[j].push_back(i);
	      
	      /*
		cerr<<"SEMEAI --> ";
		cerr<<move_to_string(GE_Directions::board_size, i)<<" - ";
		cerr<<move_to_string(GE_Directions::board_size, j)<<" ";
		cerr<<"("<<v_semeai[i][j]<<"; "<<v_semeai[j][i]<<")";
	      
		//cerr<<((int)((v_semeai[i][j]/(v_semeai[i][j]+v_semeai[j][i])*10000)))/100.f;
		cerr<<endl;
	      */
	    }
	  
	}  
    }
  
  GE_Marker mk(v_semeai.size());
  setSemeais(g, mk, chs);  
  
  //print_error_semeais();
  //chs.print(); //chs est a debugger
}


void GE_AnalyzeSemeai::setDeathOrLife(GE_Game& g, const GE_Informations& infos)
{
  for(int i = 0; i<(int)(infos.groups.size()); i++)
    {
      const GE_GroupInformation& gi = *((infos.groups)[i]);
      
      int status = gi.get_status();
      
      if((gi.get_size()>1)
	 &&(status==GE_UNKNOWN_DEATH_OR_LIFE)
	 &&(gi.stats_life<=GE_PARAM_DEATH_OR_LIFE_DETECTION_1)
	 &&(gi.stats_life>=1-GE_PARAM_DEATH_OR_LIFE_DETECTION_1))
	{
	  semeais.push_back(GE_Semeai());
	  GE_Semeai& sem = semeais.back();
	  
	  sem.problem = GE_GOAL_DEATH_AND_LIFE;
	  
	  int num_network = -1;
	  
	  list<int>* sem_stones = 0;
	  list<int>* opp_sem_stones = 0;
	  
	  if(gi.get_color_stone()==GE_BLACK_STONE)
	    {
	      sem.black_stones = gi.ch.links;
	      sem.main_black_stone = sem.black_stones.front();
	      sem.main_black_size = gi.ch.size();
	      sem.main_black_nb_liberties = gi.ch.liberties;
	      num_network = infos.get_network(g, sem.main_black_stone);
	      sem_stones = &(sem.black_stones);
	      opp_sem_stones = &(sem.white_stones);
	    }
	  else
	    {
	      sem.white_stones = gi.ch.links;
	      sem.main_white_stone = sem.white_stones.front();
	      sem.main_white_size = gi.ch.size();
	      sem.main_white_nb_liberties = gi.ch.liberties;
	      num_network = infos.get_network(g, sem.main_white_stone);
	      sem_stones = &(sem.white_stones);
	      opp_sem_stones = &(sem.black_stones);
	    }
	  
	  const GE_GroupsNetwork& gn = *((infos.networks)[num_network]);
	  
	  for(int i_net = 0; i_net<(int)gn.groups.size(); i_net++)
	    {
	      int num_group2 = (gn.groups)[i_net];
	      const GE_GroupInformation& gi2 = *((infos.groups)[num_group2]);
	      
	      for(int i_neighbour = 0; 
		  i_neighbour<(int)gi2.neighbour_groups.size(); 
		  i_neighbour++)
		{
		  int num_neighbour = 
		    (gi2.neighbour_groups)[i_neighbour];
		  
		  const GE_GroupInformation& g_neighbour = 
		    *((infos.groups)[num_neighbour]);
		  
		  int status_neighbour = g_neighbour.get_status();
		  
		  if(status_neighbour>=GE_ALIVE) continue;
		  
		  if(((status_neighbour==GE_UNKNOWN_DEATH_OR_LIFE)
		      &&(g_neighbour.stats_life<=0.6f))
		     ||(status_neighbour==GE_KILL_SAVE)
		     ||(status_neighbour==GE_KILL)
		     ||(status_neighbour<GE_UNKNOWN_DEATH_OR_LIFE))
		    {
		      GE_ListOperators::merge(g_neighbour.ch.links, 
					   *opp_sem_stones, 
					   true);
		    }
		}
	      


	      if(gi2.index==gi.index) continue;
	      
	      GE_ListOperators::merge(gi2.ch.links, *sem_stones, true);
	    }
	  
	  sem.marker_all_stones.reinit(GE_Directions::board_area);
	  
	  list<int>::const_iterator i_st = sem_stones->begin();
	  while(i_st!=sem_stones->end())
	    {
	      sem.marker_all_stones.set_treated(*i_st);
	      i_st++;
	    }

	  i_st = opp_sem_stones->begin();
	  while(i_st!=opp_sem_stones->end())
	    {
	      sem.marker_all_stones.set_treated(*i_st);
	      i_st++;
	    }
	  

	  //sem.tools_liberties(g);
	  /*
	    list<int>::const_iterator i_st = gi.ch.links.begin();
	    while(i_st!=gi.ch.links.end())
	    {
	    sem.marker_all_stones.set_treated(*i_st);
	    i_st++;
	    }
	  */
	}
    }
}


void GE_AnalyzeSemeai::initVitalStones(const GE_Game& g, GE_Informations& infos)
{
  list<GE_Semeai>::iterator i_sem = semeais.begin();

  while(i_sem!=semeais.end())
    {
      i_sem->init_vital_stones(g, infos);
      
      i_sem++;
    }
}

void GE_AnalyzeSemeai::initFrontiers(const GE_Game& g, GE_Informations& infos)
{
  list<GE_Semeai>::iterator i_sem = semeais.begin();
  
  while(i_sem!=semeais.end())
    {
      i_sem->init_frontiers(g, infos);
      
      i_sem++;
    }
}



void GE_AnalyzeSemeai::toolsLiberties(GE_Game& g)
{
  list<GE_Semeai>::iterator i_sem = semeais.begin();
  
  while(i_sem!=semeais.end())
    {
//	    i_sem->print_error_semeai();//BLIBLI
      i_sem->tools_liberties(g);
      
      i_sem++;
    }
}


/*
 * for the moment, 
 * the correction is
 * only suppression
 *
 * // if no group semeai --> suppression
 * // if one group semeai --> semeai becomes death and life
 */

void GE_AnalyzeSemeai::correctFalseSemeai(const GE_Game& g, GE_Informations& infos)
{
  list<GE_Semeai>::iterator i_sem = semeais.begin();
  
  while(i_sem!=semeais.end())
    {
      bool semeai_black = false;
      bool semeai_white = false;
      
      if((i_sem->problem==GE_GOAL_SEMEAI)
	 &&(not i_sem->is_a_real_semeai(g, infos, &semeai_black, &semeai_white)))
	{
	  //cerr<<"### suppression ### not a semeai"<<endl;
	  i_sem = semeais.erase(i_sem); //pas de i_sem++
	  continue;
	  
	  
	  /*
	  //cerr<<"GE_FALSE SEMEAI"<<endl;
	  i_sem->print_error_semeai();
	  
	  if((not semeai_black)&&(not semeai_white))
	  {
	  cerr<<"### suppression ### not a semeai"<<endl;
	  i_sem = semeais.erase(i_sem); //pas de i_sem++
	  continue;
	  }
	  
	  cerr<<"### transmutation ### semeai --> death or life"<<endl;
	  i_sem->problem = GE_GOAL_DEATH_AND_LIFE;
	  
	  if(not semeai_black)
	  {
	  cerr<<"black is not a group of semeai"<<endl;
	  
	  i_sem->main_black_stone = GE_PASS_MOVE;
	  i_sem->main_black_size = 0;
	  i_sem->main_black_nb_liberties = 0;
	  }
	  
	  if(not semeai_white)
	  {
	  cerr<<"white is not a group of semeai"<<endl;
	  
	  i_sem->main_white_stone = GE_PASS_MOVE;
	  i_sem->main_white_size = 0;
	  i_sem->main_white_nb_liberties = 0;
	  }
	  
	  cerr<<"NEW PROBLEM"<<endl;
	  i_sem->print_error_semeai();
	  */
	}  
      
      i_sem++;
    }  
}




void GE_AnalyzeSemeai::getInterestingMoves(GE_Game& game, const GE_Semeai& sem, 
					int location, 
					GE_Marker& mk, list<int>& l_moves)
  const
{
  int big_location = GE_Bibliotheque::to_big_goban[location];
  
  for(int dir = GE_NORTH; dir<=GE_WEST; dir++)
    {
      int big_neighbour =  big_location+GE_Directions::directions[dir];
      
      int neighbour = GE_Bibliotheque::to_goban[big_neighbour];
      
      
      if(neighbour<0) continue;
      if((game.goban)->get_stone(neighbour)!=GE_WITHOUT_STONE) continue;
      if(not mk.is_treated(neighbour)) 
	{
	  l_moves.push_back(neighbour);
	  mk.set_treated(neighbour);
	}
      
      
      int big_neighbour_tobi =  big_neighbour+
	GE_Directions::directions[dir];
      
      int neighbour_tobi = GE_Bibliotheque::to_goban[big_neighbour_tobi];
      
      
      if(neighbour_tobi<0) continue;
      if((game.goban)->get_stone(neighbour_tobi)!=GE_WITHOUT_STONE) continue;
      //if(GE_Bibliotheque::is_in_border(big_neighbour_tobi)) continue;
      if(mk.is_treated(neighbour_tobi)) continue;
      
      l_moves.push_back(neighbour_tobi);
      mk.set_treated(neighbour_tobi);
    }
  
  
  int col_location = location%GE_Directions::board_size;
  for(int dir = GE_NORTHEAST; dir<=GE_SOUTHWEST; dir++)
    {
      int big_neighbour =  big_location+GE_Directions::directions[dir];
      
      int neighbour = GE_Bibliotheque::to_goban[big_neighbour];
      
      if(neighbour<0) continue;
      if((game.goban)->get_stone(neighbour)!=GE_WITHOUT_STONE)
	continue;
      if(mk.is_treated(neighbour)) continue;
 

      int col_neighbour=neighbour%GE_Directions::board_size;
      int val_dir = col_neighbour-col_location;
      int loc1 = neighbour-val_dir;
      int loc2 = location+val_dir;
      if(((game.goban)->get_stone(loc1)!=GE_WITHOUT_STONE)
	 &&(not sem.is_a_semeai_stone_fast(loc1))
	 &&((game.goban)->get_stone(loc2)!=GE_WITHOUT_STONE)
	 &&(not sem.is_a_semeai_stone_fast(loc2)))
	continue;

      
      l_moves.push_back(neighbour);
      mk.set_treated(neighbour);
    }
  
 
}

void GE_AnalyzeSemeai::getInterestingMovesByLastMove
(GE_Game& g, 
 const GE_Semeai& sem, 
 GE_Marker& mk, 
 list<int>& new_list, 
 list<int>& new_black_stones, 
 list<int>& new_white_stones) const 
{
  int last_move = (g.get_last_move());
  if(last_move==GE_PASS_MOVE) return;
  
  
  bool is_new = false;
  if(g.tc==GE_BLACK)
    is_new = sem.is_a_new_stone_semeai(g, last_move, &new_white_stones);
  else 
    is_new = sem.is_a_new_stone_semeai(g, last_move, &new_black_stones);
  
  //if(not is_new) return;
  
  //if((not is_new)&&(not ch.is_atari())) 
  if(not is_new)
    {
      GE_Chains neighbours_last_move;
      neighbours_last_move.neighbour_chains(*(g.goban), last_move);
      list<GE_Chain>::const_iterator i_ch = neighbours_last_move.sets.begin();
      while(i_ch!=neighbours_last_move.sets.end())
	{
	  if(i_ch->is_atari())
	    {
	      int free_location = (i_ch->free_intersections).back();
	      if(not mk.is_treated(free_location))
		{
		  new_list.push_back(free_location);
		  mk.set_treated(free_location);
		}
	    }
	  else
	    {
	      if((i_ch->liberties==2)
		 &&(i_ch->stone==(g.goban)->get_stone(last_move)))
		{
		  int free_location1 = (i_ch->free_intersections).front();
		  if(not mk.is_treated(free_location1))
		    {
		      new_list.push_back(free_location1);
		      mk.set_treated(free_location1);
		    }
		  
		  int free_location2 = (i_ch->free_intersections).back();
		  if(not mk.is_treated(free_location2))
		    {
		      new_list.push_back(free_location2);
		      mk.set_treated(free_location2);
		    }
		}
	      
	      if((i_ch->liberties==3)
		 &&(i_ch->stone==(g.goban)->get_stone(last_move)))
		{
		  list<int>::const_iterator i_fi = 
		    i_ch->free_intersections.begin();
		  
		  list<int> not_already_moves;
		  bool move_dame = false;
		  while(i_fi!=i_ch->free_intersections.end())
		    {
		      if(not mk.is_treated(*i_fi))
			{
			  not_already_moves.push_back(*i_fi);
			}
		      
		      if((g.goban)->is_a_dame_location(*i_fi, i_ch->stone))
			{
			  move_dame = true;
			  //break;
			}
		      
		      i_fi++;
		    }
		  
		  if(move_dame)
		    {
		      list<int>::const_iterator i_nam 
			= not_already_moves.begin();
		      
		      while(i_nam!=not_already_moves.end())
			{
			  if(g.play(*i_nam))
			    {
			      GE_Chain ch_att;
  assert((g.goban)->get_stone(*i_nam)!=GE_WITHOUT_STONE);
			      ch_att.init(*(g.goban), *i_nam);
			      
			      g.backward();
			      
			      if(ch_att.liberties>2)
				{
				  if(not mk.is_treated(*i_nam))
				    {
				      new_list.push_back(*i_nam);
				      mk.set_treated(*i_nam);
				      
				      //cerr<<"new move by dame: ";
				      //cerr<<
				      //	move_to_string(GE_Directions::board_size, 
				      //	*i_nam)<<endl;
				    } 
				}
			      
			    }
			  
			  i_nam++;
			}
		    }
		  
		  
		}
	    }
	  
	  
	  
	  
	  i_ch++;
	}
    }
  else
    {
  assert((g.goban)->get_stone(last_move)!=GE_WITHOUT_STONE);
      GE_Chain ch;
      ch.init(*(g.goban), last_move);
      
      list<int>::const_iterator i_ch = ch.links.begin();
      
      while(i_ch!=ch.links.end())
	{
	  getInterestingMoves(g, sem, *i_ch, mk, new_list);
	  i_ch++;
	}
    }
}





void GE_AnalyzeSemeai::getInterestingMoves(GE_Game& g, const GE_Semeai& sem, 
					GE_Marker& mk, 
					list<int>& l_moves) const
{
  GE_Marker mk2(GE_Directions::board_area);
  list<int>::const_iterator i_bs = sem.black_stones.begin();
  
  while(i_bs!=sem.black_stones.end())
    {
      
      if(not mk2.is_treated(*i_bs))
	{
	  getInterestingMoves(g, sem, *i_bs, mk, l_moves);
	  mk2.set_treated(*i_bs);
	}
      
	assert((g.goban)->get_stone(*i_bs)==GE_BLACK_STONE);

      GE_Chain ch;
  assert((g.goban)->get_stone(*i_bs)!=GE_WITHOUT_STONE);
      ch.init(*(g.goban), *i_bs);
      list<int>::const_iterator i_l = ch.links.begin();
      while(i_l!=ch.links.end())
	{
	  if(not mk2.is_treated(*i_l))
	    {
	      getInterestingMoves(g, sem, *i_l, mk, l_moves);
	      mk2.set_treated(*i_l);
	    }
	  
	  i_l++;
	}
      
      i_bs++;
    }

  list<int>::const_iterator i_ws = sem.white_stones.begin();
  
  while(i_ws!=sem.white_stones.end())
    {
      
      if(not mk2.is_treated(*i_ws))
	{
	  getInterestingMoves(g, sem, *i_ws, mk, l_moves);
	  mk2.set_treated(*i_ws);
	}
      
	assert((g.goban)->get_stone(*i_ws)==GE_WHITE_STONE);

  assert((g.goban)->get_stone(*i_ws)!=GE_WITHOUT_STONE);
      GE_Chain ch;
      ch.init(*(g.goban), *i_ws);
      list<int>::const_iterator i_l = ch.links.begin();
      while(i_l!=ch.links.end())
	{
	  if(not mk2.is_treated(*i_l))
	    {
	      getInterestingMoves(g, sem, *i_l, mk, l_moves);
	      mk2.set_treated(*i_l);
	    }
	  
	  i_l++;
	}
      
      i_ws++;
    }
}



void GE_AnalyzeSemeai::getInterestingMoves(GE_Game& g, const GE_Semeai& sem, 
					const GE_Informations& infos, 
					int location, 
					GE_Marker& mk, 
					list<int>& l_moves) const
{
  int num_group = infos.get_index(g, location);
  
  const GE_GroupInformation& gi = *(infos.groups[num_group]);
  
  for(int i_ng = 0; i_ng<(int)gi.neighbour_groups.size(); i_ng++)
    {
      int num_neighbour = gi.neighbour_groups[i_ng];
      const GE_GroupInformation& ngi = *(infos.groups[num_neighbour]);

      const GE_Chain& n_ch = ngi.ch;
      
      if(n_ch.is_atari())
	{
	  int free_location = n_ch.free_intersections.back();
	  if(not mk.is_treated(free_location))
	    {
	      l_moves.push_back(free_location);
	      mk.set_treated(free_location);
	    }
	  
	  continue;
	}
      
      if(n_ch.liberties<=3)
	{
	  list<pair<int, int> >::const_iterator i_pc =
	    ngi.possible_connexions.begin();
	  
	  while(i_pc!=ngi.possible_connexions.end())
	    {
	      int candidate_move = i_pc->second;
	      
	      if(mk.is_treated(candidate_move))
		{
		  i_pc++;
		  continue;
		}
	      
	      //!!!!
	      
	      bool generate_pass_move = false;
	      
	      if(((g.tc==GE_WHITE)&&(n_ch.stone==GE_WHITE_STONE))
		 ||((g.tc==GE_BLACK)&&(n_ch.stone==GE_BLACK_STONE)))
		{
		  generate_pass_move = true;
		  g.play(GE_PASS_MOVE);
		}
	      
	      if(g.play(candidate_move))
		{
		  int nb_liberties = n_ch.liberties-1;
		  //-1 because of the last move  
		  bool no_more_liberties = true;
		  list<int>::const_iterator i_fi =
		    n_ch.free_intersections.begin();
		  while(i_fi!=n_ch.free_intersections.end())
		    {
		      if(not g.play(*i_fi))
			{
			  i_fi++;
			  continue;
			}
		      
		      GE_Chain ch_new;
  assert((g.goban)->get_stone(*i_fi)!=GE_WITHOUT_STONE);
		      ch_new.init(*(g.goban), *i_fi);
		      
		      if(ch_new.liberties>nb_liberties)
			{
			  no_more_liberties = false;
			  
			  g.backward();
			  break;
			}
		      
		      
		      g.backward();
		      i_fi++;
		    }
		  
		  
		  
		  if(no_more_liberties)
		    {
		      //cerr<<"out move: ";
		      //cerr<<move_to_string(GE_Directions::board_size, 
		      //		   candidate_move)<<endl;
		      
		      l_moves.push_back(candidate_move);
		      mk.set_treated(candidate_move);
		    }
		  
		  g.backward();
		}
	      
	      
	      if(generate_pass_move)
		g.backward();
	      
	      //!!!!
	      
	      i_pc++;
	    }
	  
	  list<int>::const_iterator i_tc = 
	    ngi.tactic_connexions.begin();
	  while(i_tc!=ngi.tactic_connexions.end())
	    {
	      const GE_GroupInformation& gi_tc = 
		*(infos.groups[*i_tc]);
	      
	      int candidate_move = 
		n_ch.get_one_connexion(gi_tc.ch);
	      
	      if(not mk.is_treated(candidate_move))
		{
		  bool generate_pass_move = false;
		  
		  if(((g.tc==GE_WHITE)&&(n_ch.stone==GE_WHITE_STONE))
		     ||((g.tc==GE_BLACK)&&(n_ch.stone==GE_BLACK_STONE)))
		    {
		      generate_pass_move = true;
		      g.play(GE_PASS_MOVE);
		    }
		  
		  if(g.play(candidate_move))
		    {
		      GE_Chain temp_ch;
  assert((g.goban)->get_stone(candidate_move)!=GE_WITHOUT_STONE);
		      temp_ch.init(*(g.goban), candidate_move);
		      if(not temp_ch.is_atari())
			{
			  int nb_liberties = n_ch.liberties-1;
			  //-1 because of the last move  
			  bool no_more_liberties = true;
			  list<int>::const_iterator i_fi =
			    n_ch.free_intersections.begin();
			  while(i_fi!=n_ch.free_intersections.end())
			    {
			      if(not g.play(*i_fi))
				{
				  i_fi++;
				  continue;
				}
			      
  assert((g.goban)->get_stone(*i_fi)!=GE_WITHOUT_STONE);
			      GE_Chain ch_new;
			      ch_new.init(*(g.goban), *i_fi);
			      
			      if(ch_new.liberties>nb_liberties)
				{
				  no_more_liberties = false;
				  
				  g.backward();
				  break;
				}
			      
			      
			      g.backward();
			      i_fi++;
			    }
			  
			    
			  
			  if(no_more_liberties)
			    {
			      //cerr<<"out move: ";
			      //cerr<<move_to_string(GE_Directions::board_size, 
			      //candidate_move)<<endl;
			      
			      l_moves.push_back(candidate_move);
			      mk.set_treated(candidate_move);
			    }
			}
		      
		      g.backward();
		    }
		  
		  
		  if(generate_pass_move)
		    g.backward();
		  
		}
	      
	      i_tc++;
	    }
	}   
    }
}



void GE_AnalyzeSemeai::getInterestingMoves(GE_Game& g, const GE_Semeai& sem, 
					const GE_Informations& infos, 
					GE_Marker& mk, 
					list<int>& l_moves) const
{
  getInterestingMoves(g, sem, mk, l_moves);


  list<int>::const_iterator i_bs = sem.black_stones.begin();
  while(i_bs!=sem.black_stones.end())
    {
      getInterestingMoves(g, sem, infos, *i_bs, mk, l_moves);
      i_bs++;
    }
  
  list<int>::const_iterator i_ws = sem.white_stones.begin();
  while(i_ws!=sem.white_stones.end())
    {
      getInterestingMoves(g, sem, infos, *i_ws, mk, l_moves);
      i_ws++;
    }
}



/*
  bool GE_AnalyzeSemeai::isThreatSemeai(GE_Game& g, int location)
  {
  return true;
  }
*/

void GE_AnalyzeSemeai::getNewInterestingMoves(GE_Game& g, 
					   const GE_Semeai& sem, 
					   const list<int>& old_list, 
					   GE_Marker& mk, 
					   list<int>& new_list) const
{
  mk.update_treatment();
  
  new_list = old_list;
  list<int>::const_iterator i_nl = new_list.begin();
  while(i_nl!=new_list.end())
    {
      mk.set_treated(*i_nl);
      i_nl++;
    }
  
  int last_location = g.get_last_move();

  if(last_location==GE_PASS_MOVE) return;
  
  GE_ListOperators::erase(new_list, last_location);
  
  assert((g.goban)->get_stone(last_location)!=GE_WITHOUT_STONE);
  GE_Chain ch;
  ch.init(*(g.goban), last_location);
  list<int>::const_iterator i_ch = ch.links.begin();
  
  while(i_ch!=ch.links.end())
    {
      getInterestingMoves(g, sem, *i_ch, mk, new_list);
      i_ch++;
    }
  
  
  const list<int>& last_eaten = (g.stones_taken).back(); 
  list<int>::const_iterator i_capt = last_eaten.begin();
  while(i_capt!=last_eaten.end())
    {
      new_list.push_back(*i_capt);
      mk.set_treated(*i_capt);
      i_capt++;
    }
}

void GE_AnalyzeSemeai::getNewInterestingMoves2(GE_Game& g, 
					    const GE_Semeai& sem, 
					    const list<int>& old_list, 
					    const list<int>& old_black_stones, 
					    const list<int>& old_white_stones, 
					    GE_Marker& mk, 
					    list<int>& new_list, 
					    list<int>& new_black_stones, 
					    list<int>& new_white_stones) const
{
  mk.update_treatment();
  
  new_list = old_list;
  new_black_stones = old_black_stones;
  new_white_stones = old_white_stones;

  list<int>::const_iterator i_nl = new_list.begin();
  while(i_nl!=new_list.end())
    {
      mk.set_treated(*i_nl);
      i_nl++;
    }
  
  int last_location = g.get_last_move();
  
  if(last_location==GE_PASS_MOVE) return;
  
  GE_ListOperators::erase(new_list, last_location);
  
  const list<int>& last_eaten = (g.stones_taken).back(); 
  list<int>::const_iterator i_capt = last_eaten.begin();
  while(i_capt!=last_eaten.end())
    {
      new_list.push_back(*i_capt);
      mk.set_treated(*i_capt);
      i_capt++;
    }
  
  getInterestingMovesByLastMove(g, sem, mk, new_list, new_black_stones, 
				new_white_stones);


  /*
    bool is_new = false;
    if(g.tc==GE_BLACK)
    is_new = sem.is_a_new_stone_semeai(g, last_location, &new_white_stones);
    else 
    is_new = sem.is_a_new_stone_semeai(g, last_location, &new_black_stones);
  
    GE_Chain ch;
    ch.init(*(g.goban), last_location);
  
  
    if((not is_new)&&(not ch.is_atari())) 
    return;

  
    list<int>::const_iterator i_ch = ch.links.begin();
  
    while(i_ch!=ch.links.end())
    {
    getInterestingMoves(g, sem, *i_ch, mk, new_list);
    i_ch++;
    }
  
  */
}


void GE_AnalyzeSemeai::updateNewInterestingMoves2(GE_Game& g, 
					       const GE_Semeai& sem, 
					       GE_Marker& mk, 
					       list<int>& new_list, 
					       list<int>& new_black_stones, 
					       list<int>& new_white_stones) 
  const
{
  //mk.update_treatment();
  
  /*
    list<int>::const_iterator i_nl = new_list.begin();
    while(i_nl!=new_list.end())
    {
    mk.set_treated(*i_nl);
    i_nl++;
    }
  */
  
  int last_location = g.get_last_move();
  
  if(last_location==GE_PASS_MOVE) return;
  
  GE_ListOperators::erase(new_list, last_location);
  mk.set_no_treated(last_location);
  
  //BUG: Il faut les remettre dans la liste de coups
  //meme si c'est le dernier coup n'est pas une nouvelle pierre 
  //du semeai
  const list<int>& last_eaten = (g.stones_taken).back(); 
  list<int>::const_iterator i_capt = last_eaten.begin();
  while(i_capt!=last_eaten.end())
    {
      new_list.push_back(*i_capt);
      mk.set_treated(*i_capt);
      i_capt++;
    }
 


  getInterestingMovesByLastMove(g, sem, mk, new_list, new_black_stones, 
				new_white_stones);

  /*
    bool is_new = false;
    if(g.tc==GE_BLACK)
    is_new = sem.is_a_new_stone_semeai(g, last_location, &new_white_stones);
    else 
    is_new = sem.is_a_new_stone_semeai(g, last_location, &new_black_stones);
    
    //if(not is_new) return;
    
    
    

    
    GE_Chain ch;
    ch.init(*(g.goban), last_location);
    
    if((not is_new)&&(not ch.is_atari())) 
    return;
    
    list<int>::const_iterator i_ch = ch.links.begin();
  
    while(i_ch!=ch.links.end())
    {
    getInterestingMoves(g, sem, *i_ch, mk, new_list);
    i_ch++;
    }
  */
}



void GE_AnalyzeSemeai::getSaveMoves(GE_Game& g, const GE_Semeai& sem, GE_Chain& ch_atari, 
				 list<int>& save_moves)
{
  assert(ch_atari.is_atari());
  
  int temp_saving_atari = ch_atari.free_intersections.front();
  
  if(g.play(temp_saving_atari))
    {
  assert((g.goban)->get_stone(temp_saving_atari)!=GE_WITHOUT_STONE);
      GE_Chain ch_saving_atari;
      ch_saving_atari.init(*(g.goban), temp_saving_atari);
      
      g.backward();
      
      if(not ch_saving_atari.is_atari())
	GE_ListOperators::push_if_not(save_moves, temp_saving_atari); 
    }
  
  
  GE_Chains chs;
  chs.neighbour_chains(*(g.goban), ch_atari);
  
  list<GE_Chain>::const_iterator i_ch =chs.sets.begin();
  while(i_ch!=chs.sets.end())
    {
      if((i_ch->stone!=ch_atari.stone)
	 &&(i_ch->is_atari()))
	{
	  GE_ListOperators::push_if_not(save_moves, 
				     (i_ch->free_intersections).front());
	}
      
      i_ch++;
    }
}


bool GE_AnalyzeSemeai::getSaveMoves(GE_Game& g, const GE_Semeai& sem, 
				 list<int>& save_moves)
{
  
  int main_stone;
  const list<int>* vital_stones = 0;
  int color_stone = GE_WITHOUT_STONE;
  
  if(g.tc==GE_BLACK)
    {
      main_stone = sem.main_black_stone;
      vital_stones = &(sem.black_vital_stones);
      color_stone = GE_BLACK_STONE;
    }
  else
    {
      main_stone = sem.main_white_stone;
      vital_stones = &(sem.white_vital_stones);
      color_stone = GE_WHITE_STONE;
    }
  
  if((main_stone>=0)
     &&(sem.main_stone_is_vital(color_stone)))
    {
      GE_Chain main_ch;
      
	assert((g.goban)->get_stone(main_stone)==color_stone);

      main_ch.init(*(g.goban), main_stone);
      
      if(main_ch.is_atari())
	{
	  getSaveMoves(g, sem, main_ch, save_moves);
	  return true;
	}
    }
  
  
  list<int>::const_iterator i_vs = vital_stones->begin();
  while(i_vs!=vital_stones->end())
    {
	 assert((g.goban)->get_stone(*i_vs)==color_stone);

      GE_Chain vs_ch;
	vs_ch.init(*(g.goban),*i_vs); //BUG HERE JBH (vu) init oublie !!!!

      if(vs_ch.is_atari())
	{
	  getSaveMoves(g, sem, vs_ch, save_moves);
	  return true;
	}
      
      i_vs++;
    }
  
  return false;
}



void GE_AnalyzeSemeai::initExpertiseSemeai()
{
  if(not to_reinitialize)
    {
      return;
    }
  
  if(expertise_semeai.size()==0)
    {      
      expertise_semeai.resize(GE_Directions::board_area, 0);
    }
  
  
  for(int i = 0; i<(int)expertise_semeai.size(); i++)
    {
      expertise_semeai[i] = 0;
    }

  to_reinitialize = false;
}


void GE_AnalyzeSemeai::saveBigAtari(GE_Game& g,  const GE_Semeai& sem)
{
  const list<int>* last_atari = 0;
  
  if(g.tc==GE_BLACK)
    last_atari = &(sem.last_black_atari);
  else last_atari = &(sem.last_white_atari);
  
  list<int>::const_iterator i_la = last_atari->begin();
  
  
  list<int> save_moves;
  while(i_la!=last_atari->end())
    {
  assert((g.goban)->get_stone(*i_la)!=GE_WITHOUT_STONE);
      GE_Chain ch_atari;
      ch_atari.init(*(g.goban), *i_la);
      
      if(ch_atari.size()>2)
	getSaveMoves(g, sem, ch_atari, save_moves);
      else
	{
	  if(ch_atari.size()==2)
	    {
	      //TODO Cas a traiter
	      getSaveMoves(g, sem, ch_atari, save_moves);
	    }
	  else
	    {
	      int dangerous_location = ch_atari.free_intersections.front();
	      g.play(GE_PASS_MOVE);
	      if(not g.play(dangerous_location))
		g.backward();
	      else
		{
		  GE_Chains chs;
		  chs.neighbour_chains(*(g.goban), dangerous_location);
		  
		  g.backward();
		  g.backward();
		  
		  list<GE_Chain>::const_iterator i_ch =chs.sets.begin();
		  while(i_ch!=chs.sets.end())
		    {
		      if(i_ch->is_atari())
			if(i_ch->stone==ch_atari.stone)
			  {
			    getSaveMoves(g, sem, ch_atari, save_moves);
			    break;
			  }
		      
		      i_ch++;
		    }
		}  
	    }
	  
	  //TODO avoid to build an opponent living group
	}
      
      i_la++;
    }
  
  list<int>::const_iterator i_sm = save_moves.begin();
  while(i_sm!=save_moves.end())
    {
      expertise_semeai[*i_sm]+=semeai_save_big_atari;
      i_sm++;
    }
  
}



void GE_AnalyzeSemeai::computeExpertiseSemeai(GE_Game& g, 
					   const GE_Semeai& sem, 
					   const list<int>& l_moves)
{ 
  initExpertiseSemeai();
  threatAtari(g, sem);
  //localExpertise(g);
  //avoidDame(g, sem);
  captureLastMove(g, sem);
  maybeWinRace(g, sem);
  saveBigAtari(g, sem);
  evaluate_moves_by_pattern(g, sem, l_moves);

  to_reinitialize = true;
}


void GE_AnalyzeSemeai::localExpertise(GE_Game& g)
{
  int last_location = g.get_last_move();
  if(last_location==GE_PASS_MOVE) return;
  
  int big_last_location = GE_Bibliotheque::to_big_goban[last_location];
  
  for(int i = GE_NORTH; i<=GE_WEST; i++)
    {
      int big_neighbour = big_last_location+GE_Directions::directions[i];
      int neighbour = GE_Bibliotheque::to_goban[big_neighbour];
      
      if(neighbour<0)
	continue;
      
      expertise_semeai[neighbour]+=semeai_tsuke;
      
      int big_neighbour_tobi = big_neighbour+GE_Directions::directions[i];
      int neighbour_tobi = GE_Bibliotheque::to_goban[big_neighbour_tobi];
      
      if(neighbour_tobi<0)
	continue;
      
      expertise_semeai[neighbour]+=semeai_tobi;
      
    }

  for(int i = GE_NORTHEAST; i<=GE_SOUTHWEST; i++)
    {
      int big_neighbour = big_last_location+GE_Directions::directions[i];
      int neighbour = GE_Bibliotheque::to_goban[big_neighbour];
      
      if(neighbour<0)
	continue;
      
      expertise_semeai[neighbour]+=semeai_kata;
    }
}







void GE_AnalyzeSemeai::threatAtari(GE_Game& g, const GE_Semeai& sem)
{
  int main_stone = GE_PASS_MOVE;
  int opp_main_stone = GE_PASS_MOVE;
  const list<int>* opp_vital_stone = 0;
int opp_color_stone = GE_BLACK_STONE;
  
  if((g.tc)==GE_BLACK)
    {
      main_stone = sem.main_black_stone;
      opp_main_stone = sem.main_white_stone;
      opp_vital_stone = &(sem.white_vital_stones);
      opp_color_stone = GE_WHITE_STONE;
    }
  else
    {
      main_stone = sem.main_white_stone;
      opp_main_stone = sem.main_black_stone;
      opp_vital_stone = &(sem.black_vital_stones);
      opp_color_stone = GE_BLACK_STONE;
    }
  
  if((opp_main_stone>=0)&&((g.goban)->get_stone(opp_main_stone)==opp_color_stone))
    {
  assert((g.goban)->get_stone(opp_main_stone)!=GE_WITHOUT_STONE);
      GE_Chain ch_opp; ch_opp.init(*(g.goban), opp_main_stone);
      
      if(ch_opp.liberties==2)
	{
	  int gm1 = ch_opp.free_intersections.front();
	  int gm2 = ch_opp.free_intersections.back();
	  
	  expertise_semeai[gm1]+=semeai_threat_atari;
	  expertise_semeai[gm2]+=semeai_threat_atari;
	}     
    }
  
  list<int>::const_iterator i_s = opp_vital_stone->begin();
  
  while(i_s!=opp_vital_stone->end())
    {
  assert((g.goban)->get_stone(*i_s)!=GE_WITHOUT_STONE);
      GE_Chain temp_ch; temp_ch.init(*(g.goban), *i_s);
      
      
      if(temp_ch.liberties==2)
	{
	  int gm1 = temp_ch.free_intersections.front();
	  int gm2 = temp_ch.free_intersections.back();
	  
	  expertise_semeai[gm1]+=semeai_threat_atari;
	  expertise_semeai[gm2]+=semeai_threat_atari;
	} 
      
      
      i_s++;
    }
}



bool GE_AnalyzeSemeai::getPossibleConnexions(GE_Game& g, const GE_Semeai& sem, 
					  const GE_Chain& ch, 
					  int location, 
					  list<int>& l_connexions, 
					  int dir)
{
  if(dir==GE_HERE)
    {
      getPossibleConnexions(g, sem, ch, location, l_connexions, GE_NORTH);
      getPossibleConnexions(g, sem, ch, location, l_connexions, GE_SOUTH);
      getPossibleConnexions(g, sem, ch, location, l_connexions, GE_WEST);
      getPossibleConnexions(g, sem, ch, location, l_connexions, GE_EAST);
    }
  
  if(location<0) return false;
  
  int big_location = GE_Bibliotheque::to_big_goban[location];
  int big_location_dir = big_location+GE_Directions::directions[dir];
  int location_dir = GE_Bibliotheque::to_goban[big_location_dir];
  
  if(location_dir<0) return false;
  if((g.goban)->get_stone(location_dir)!=GE_WITHOUT_STONE)
    return false;
  
  bool generate_pass_move = false;
  if(((g.tc==GE_BLACK)&&(ch.stone==GE_WHITE_STONE))
     ||((g.tc==GE_WHITE)&&(ch.stone==GE_BLACK_STONE)))
    {
      g.play(GE_PASS_MOVE);
      generate_pass_move = true;
    }
  
  if(not g.play(location_dir))
    {
      if(generate_pass_move)
	g.backward(); //GE_PASS_MOVE
      
      return false;
    }
  
  assert((g.goban)->get_stone(location_dir)!=GE_WITHOUT_STONE);
  GE_Chain ch_prime;
  ch_prime.init(*(g.goban), location_dir);
  
  if(ch_prime.is_atari())
    {
      g.backward(); //location_dir
      if(generate_pass_move) 
	g.backward(); //GE_PASS_MOVE
      
      return false;
    }
 

  for(int dir2 = GE_NORTH; dir2<=GE_WEST; dir2++)
    {
      int big_neighbour = big_location_dir+GE_Directions::directions[dir2];
      
      if(big_neighbour==big_location) continue;
      
      int neighbour = GE_Bibliotheque::to_goban[big_neighbour];
      if(neighbour<0) continue;
      
      if((g.goban)->get_stone(neighbour)==ch.stone)
	{
	  if(ch.belongs(neighbour))
	    continue;
	  
	  if(not sem.is_a_semeai_stone_fast(neighbour))
	    {      
	      g.backward(); //location_dir
	      if(generate_pass_move) 
		g.backward(); //GE_PASS_MOVE
	      
	      
	      GE_ListOperators::push_if_not(l_connexions, location_dir);
	      
	      return true;
	    }
	  
	  continue;
	}
      
      
      if((g.goban)->get_stone(neighbour)!=GE_WITHOUT_STONE)
	continue;
      
      int big_neighbour_tobi =  big_neighbour+GE_Directions::directions[dir2];
      int neighbour_tobi = GE_Bibliotheque::to_goban[big_neighbour_tobi];
      if(neighbour_tobi<0) continue;
      
      if((g.goban)->get_stone(neighbour_tobi)==ch.stone)
	{
	  if(ch.belongs(neighbour_tobi))
	    continue;
	  
	  if(sem.is_a_semeai_stone_fast(neighbour_tobi))
	    continue;
	  
	  if(not g.play(neighbour))
	    {
	      g.backward(); //location_dir
	      if(generate_pass_move) 
		g.backward(); //GE_PASS_MOVE
	      
	      
	      GE_ListOperators::push_if_not(l_connexions, location_dir);
	      
	      return true;
	    }
	  
  assert((g.goban)->get_stone(neighbour)!=GE_WITHOUT_STONE);
	  GE_Chain temp_ch;
	  temp_ch.init(*(g.goban), neighbour);
	  
	  g.backward(); //neighbour;
	  
	  if(temp_ch.is_atari())
	    {
	      g.backward(); //location_dir
	      if(generate_pass_move) 
		g.backward(); //GE_PASS_MOVE
	      
	      
	      GE_ListOperators::push_if_not(l_connexions, location_dir);
	      
	      return true;
	    }
	}
      
    }
  
  for(int dir3 = GE_NORTHEAST; dir3<=GE_SOUTHWEST; dir3++)
    {
      int big_neighbour_kosumi = 
	big_location_dir+GE_Directions::directions[dir3];
      
      int neighbour_kosumi = GE_Bibliotheque::to_goban[big_neighbour_kosumi];
      if(neighbour_kosumi<0) continue;
      
      if((g.goban)->get_stone(neighbour_kosumi)==ch.stone)
	{
	  if(ch.belongs(neighbour_kosumi))
	    continue;
	  
	  if(sem.is_a_semeai_stone_fast(neighbour_kosumi))
	    continue;
       
  assert((g.goban)->get_stone(neighbour_kosumi)!=GE_WITHOUT_STONE);
	  GE_Chain ch_kosumi;
	  ch_kosumi.init(*(g.goban), neighbour_kosumi);

	  list<int> temp_connexions;

	  ch_prime.get_connexions(ch_kosumi, temp_connexions);

	  if((temp_connexions.size()>2)||(temp_connexions.size()==0))
	    continue;
	  
	  if(temp_connexions.size()==2)
	    {
	      g.backward(); //location_dir
	      if(generate_pass_move) 
		g.backward(); //GE_PASS_MOVE
	      
	      GE_ListOperators::push_if_not(l_connexions, location_dir);
	      
	      return true;
	    }
	  
	  int temp_location = temp_connexions.front();
	  
	  if(not g.play(temp_location))
	    {
	      g.backward(); //location_dir
	      if(generate_pass_move) 
		g.backward(); //GE_PASS_MOVE
	      
	      GE_ListOperators::push_if_not(l_connexions, location_dir);
	      
	      return true;
	    }
	  
  assert((g.goban)->get_stone(temp_location)!=GE_WITHOUT_STONE);
	  GE_Chain temp_ch;
	  temp_ch.init(*(g.goban), temp_location);
	  
	  g.backward(); //temp_location
	  
	  if(temp_ch.is_atari())
	    {
	      g.backward(); //location_dir
	      if(generate_pass_move) 
		g.backward(); //GE_PASS_MOVE
	      
	      GE_ListOperators::push_if_not(l_connexions, location_dir);
	
	      //cerr<<"menace connexion kosumi : ";
	      //cerr<<move_to_string(GE_Directions::board_size, location_dir);
	      //cerr<<endl;

	      
	      return true;
	    }
	  
	}
    }
  
  g.backward(); //location_dir
  if(generate_pass_move)
    g.backward(); //GE_PASS_MOVE
  
  
  return false;
}


bool GE_AnalyzeSemeai::is_move_atari(GE_Game& g, int mv)
{
  if(not g.play(mv))
    return true;
  
  assert((g.goban)->get_stone(mv)!=GE_WITHOUT_STONE);
  GE_Chain ch;
  ch.init(*(g.goban), mv);
  
  g.backward();
  
  return ch.is_atari();
}


void GE_AnalyzeSemeai::captureLastMove(GE_Game& g, const GE_Semeai& sem)
{
  int last_move = g.get_last_move();
  
  if(last_move>=0)
    {
  assert((g.goban)->get_stone(last_move)!=GE_WITHOUT_STONE);
      GE_Chain temp_ch; temp_ch.init(*(g.goban), last_move);
      
      if(temp_ch.is_atari())
	{
	  GE_Chains chs;
	  chs.neighbour_chains(*(g.goban), last_move);
	  
	  int nb_ataris = 0;
	  list<GE_Chain>::const_iterator i_ch =chs.sets.begin();
	  while(i_ch!=chs.sets.end())
	    {
	      if(i_ch->stone!=temp_ch.stone)
		{
		  if(i_ch->is_atari())
		    {
		      nb_ataris++;
		      //expertise_semeai[temp_ch.free_intersections.back()]
		      //+=semeai_capture_last_move;
		      
		      //return;
		    }
		  
		}
	      i_ch++;
	    }  
	  
	  if(nb_ataris>0)
	    expertise_semeai[temp_ch.free_intersections.back()]
	      +=(((2+nb_ataris)/3.f)*semeai_capture_last_move);
	  
	  return;
	}
      
      if(temp_ch.liberties==2)
	{
	  if(not is_move_atari(g, temp_ch.free_intersections.back()))
	    expertise_semeai[temp_ch.free_intersections.back()]
	      +=semeai_last_move_atari;
	  
	  if(not is_move_atari(g, temp_ch.free_intersections.front()))
	    expertise_semeai[temp_ch.free_intersections.front()]
	      +=semeai_last_move_atari;
	}
      
      
      //TODO optimization doit couter tres cher en temps de calcul
      int main_semeai_stone = GE_PASS_MOVE;
      
      if((g.goban)->get_stone(last_move)==GE_BLACK_STONE)
	main_semeai_stone = sem.main_black_stone;
      else main_semeai_stone = sem.main_white_stone;
      
      if((main_semeai_stone>=0)
	 &&(temp_ch.belongs(main_semeai_stone)))
	{  
	  list<int> l_connexions;
	  
	  getPossibleConnexions(g, sem, temp_ch, last_move, l_connexions);
	  
	  list<int>::const_iterator i_c = l_connexions.begin();
	  
	  while(i_c!=l_connexions.end())
	    {
	      
	      expertise_semeai[*i_c]
		+=semeai_last_move_avoid_connexion;
	      
	      i_c++;
	    }
	  
	}
      
      //end optimization
      
    }
}



bool GE_AnalyzeSemeai::studyLastMove(GE_Game& g, const GE_Semeai& sem)
{
  initExpertiseSemeai();
  
  list<int>* last_atari = 0;
  int last_color_stone = GE_WITHOUT_STONE;
  
  if(g.tc==GE_BLACK)
    {
      last_atari = &sem.last_black_atari;
      last_color_stone = GE_WHITE_STONE;
    }
  else
    {
      last_atari = &sem.last_white_atari;
      last_color_stone = GE_BLACK_STONE;
    }
  
  last_atari->clear();
  
  int last_move = g.get_last_move();
  
  if(last_move<0)
    {
      return false;
    }
 

  GE_Chains chs;
  chs.neighbour_chains(*(g.goban), last_move);
  
  list<GE_Chain>::const_iterator i_ch =chs.sets.begin();
  while(i_ch!=chs.sets.end())
    {
      if(i_ch->stone!=last_color_stone)
	{
	  if(i_ch->is_atari())
	    {
	      
	      int temp_save_atari = (i_ch->free_intersections).front();
	      
	      if(g.play(temp_save_atari))
		{
  assert((g.goban)->get_stone(temp_save_atari)!=GE_WITHOUT_STONE);
		  GE_Chain ch_saving;
		  ch_saving.init(*(g.goban), temp_save_atari);
		  
		  bool is_captured = ch_saving.is_atari();
		  
		  if(not is_captured)
		    {
		      GE_Tactic tact;
		      is_captured = tact.study_shisho2(g, temp_save_atari);
		    }
		  
		  //TODO fuir pour empecher la formation d'un deuxieme oeil ?

		  g.backward();
		  
		  if(not is_captured)
		    { 
		      expertise_semeai[temp_save_atari]
			+=last_move_save_atari;
		    }
		  else
		    {
		      //cerr<<"no saving move: "<<move_to_string(GE_Directions::board_size, temp_save_atari)<<endl;
		    }
		}
	      //cerr<<move_to_string(GE_Directions::board_size, g.get_last_move())<<endl;
	      //cerr<<move_to_string(GE_Directions::board_size, (i_ch->free_intersections).front())<<endl;
	      //assert(0);
	      
	      last_atari->push_back((i_ch->links).front());
	    }
	}
      
      i_ch++;
    }
  
  return (not last_atari->empty());
}




void GE_AnalyzeSemeai::maybeWinRace(GE_Game& g, const GE_Semeai& sem)
{
  
  list<int> weak_bfi;
  
  if((sem.main_black_stone>=0)
     &&(sem.main_stone_is_vital(GE_BLACK_STONE)))
    {
  assert((g.goban)->get_stone(sem.main_black_stone)!=GE_WITHOUT_STONE);
      GE_Chain temp_ch; temp_ch.init(*(g.goban), sem.main_black_stone);
      
      //if(weak_bfi.empty())
      weak_bfi = temp_ch.free_intersections;
    }
  
  
  list<int>::const_iterator i_bvs = sem.black_vital_stones.begin();  
  while(i_bvs!=sem.black_vital_stones.end())
    {
  assert((g.goban)->get_stone(*i_bvs)!=GE_WITHOUT_STONE);
      GE_Chain temp_ch; temp_ch.init(*(g.goban), *i_bvs);
      
      if(weak_bfi.empty())
	{
	  weak_bfi = temp_ch.free_intersections;
	}
      else
	{
	  if(temp_ch.free_intersections.size()<weak_bfi.size())
	    weak_bfi = temp_ch.free_intersections;
	}
      
      i_bvs++;
    }

  
  list<int> weak_wfi;
  
  if((sem.main_white_stone>=0)
     &&(sem.main_stone_is_vital(GE_WHITE_STONE)))
    {
  assert((g.goban)->get_stone(sem.main_white_stone)!=GE_WITHOUT_STONE);
      GE_Chain temp_ch; temp_ch.init(*(g.goban), sem.main_white_stone);
      
      //if(weak_wfi.empty())
      weak_wfi = temp_ch.free_intersections;
    }
  

  list<int>::const_iterator i_wvs = sem.white_vital_stones.begin();  
  while(i_wvs!=sem.white_vital_stones.end())
    {
  assert((g.goban)->get_stone(*i_wvs)!=GE_WITHOUT_STONE);
      GE_Chain temp_ch; temp_ch.init(*(g.goban), *i_wvs);
      
      if(weak_wfi.empty())
	{
	  weak_wfi = temp_ch.free_intersections;
	}
      else
	{
	  if(temp_ch.free_intersections.size()<weak_wfi.size())
	    weak_wfi = temp_ch.free_intersections;
	}

      i_wvs++;
    }


  if(g.tc==GE_BLACK)
    {
      if(weak_wfi.size()<=weak_bfi.size())
	{
	  list<int>::const_iterator iw = weak_wfi.begin();
	  
	  while(iw!=weak_wfi.end())
	    {
	      expertise_semeai[*iw]+=semeai_win_race;
	      iw++;
	    }
	}
    }
  else
    {
      if(weak_bfi.size()<=weak_wfi.size())
	{
	  list<int>::const_iterator ib = weak_bfi.begin();
	  
	  while(ib!=weak_bfi.end())
	    {
	      expertise_semeai[*ib]+=semeai_win_race;
	      ib++;
	    }
	}
    }
}

void GE_AnalyzeSemeai::avoidDame(GE_Game& g, const GE_Semeai& sem)
{
  cerr<<"yahoo"<<endl;


  int main_stone = GE_PASS_MOVE;
  //int opp_main_stone = GE_PASS_MOVE;
  int color_stone = GE_WITHOUT_STONE;
  
  if((g.tc)==GE_BLACK)
    {
      main_stone = sem.main_black_stone;
      //opp_main_stone = sem.main_white_stone;
      color_stone = GE_BLACK_STONE;
    }
  else
    {
      main_stone = sem.main_white_stone;
      //opp_main_stone = sem.main_black_stone;
      color_stone = GE_WHITE_STONE;
    }
  

  if(main_stone<0) return;
    
  assert((g.goban)->get_stone(main_stone)!=GE_WITHOUT_STONE);
  GE_Chain ch; ch.init(*(g.goban), main_stone);
  
  list<int>::const_iterator i_fr = ch.free_intersections.begin(); 
  
  while(i_fr!=ch.free_intersections.end())
    {
      
      if((g.goban)->get_nb_neighbours(*i_fr, color_stone)==1)
	{
	  if((g.goban)->get_nb_liberties(*i_fr)==0)
	    expertise_semeai[*i_fr]+=(2*semeai_dame);
	  
	  if((g.goban)->get_nb_liberties(*i_fr)==1)
	    expertise_semeai[*i_fr]+=(semeai_dame);
	}
      
      
      i_fr++;
    }
  

  cerr<<"end yahoo"<<endl;
}


double GE_AnalyzeSemeai::disconnection_by_double_cut(GE_Game& g, int mv, int dir)
{
  if(mv==GE_PASS_MOVE)
    return 0;
  
  if(dir==GE_UNKNOWN)
    {
      disconnection_by_double_cut(g, mv, GE_NORTHWEST);
      disconnection_by_double_cut(g, mv, GE_NORTHEAST);
      disconnection_by_double_cut(g, mv, GE_SOUTHWEST);
      disconnection_by_double_cut(g, mv, GE_SOUTHEAST);
      return 0;
    }
  
  int color_stone = GE_WITHOUT_STONE;
  int enemy_color_stone = GE_WITHOUT_STONE;
  if(g.tc==GE_BLACK)
    {
      color_stone = GE_BLACK_STONE;
      enemy_color_stone = GE_WHITE_STONE;
    }
  else
    {
      color_stone = GE_WHITE_STONE;
      enemy_color_stone = GE_BLACK_STONE;
    }
  
  int big_mv = GE_Bibliotheque::to_big_goban[mv];
  
  int big_neighbour_up;
  int big_neighbour_left;
  int big_neighbour_left_up;
  
  switch(dir)
    {
    case GE_NORTHWEST :
      big_neighbour_up = big_mv+GE_Directions::directions[GE_NORTH];
      big_neighbour_left = big_mv+GE_Directions::directions[GE_WEST];
      big_neighbour_left_up = big_mv+GE_Directions::directions[GE_NORTHWEST];
      break;
      
    case GE_SOUTHWEST :
      big_neighbour_up = big_mv+GE_Directions::directions[GE_WEST];
      big_neighbour_left = big_mv+GE_Directions::directions[GE_SOUTH];
      big_neighbour_left_up = big_mv+GE_Directions::directions[GE_SOUTHWEST];
      break;
      
    case GE_SOUTHEAST :
      big_neighbour_up = big_mv+GE_Directions::directions[GE_SOUTH];
      big_neighbour_left = big_mv+GE_Directions::directions[GE_EAST];
      big_neighbour_left_up = big_mv+GE_Directions::directions[GE_SOUTHEAST];
      break;
      
    case GE_NORTHEAST :
      big_neighbour_up = big_mv+GE_Directions::directions[GE_EAST];
      big_neighbour_left = big_mv+GE_Directions::directions[GE_NORTH];
      big_neighbour_left_up = big_mv+GE_Directions::directions[GE_NORTHEAST];
      break;
      
    default : 
      assert(0);
      
    }
  
  int neighbour_up = GE_Bibliotheque::to_goban[big_neighbour_up];
  int neighbour_left = GE_Bibliotheque::to_goban[big_neighbour_left];
  int neighbour_left_up = GE_Bibliotheque::to_goban[big_neighbour_left_up];
  
  if(neighbour_left_up<0)
    return 0;
  
  int stone_up = (g.goban)->get_stone(neighbour_up);
  int stone_left = (g.goban)->get_stone(neighbour_left);
  int stone_left_up = (g.goban)->get_stone(neighbour_left_up);
  
  if((stone_up==stone_left)
     &&(stone_up==enemy_color_stone)
     &&(stone_left_up==color_stone))
    {
      GE_Chain ch;
  assert((g.goban)->get_stone(neighbour_up)!=GE_WITHOUT_STONE);
      ch.init(*(g.goban), neighbour_up);
      if(ch.belongs(neighbour_left))
	return 0;
      
      expertise_semeai[mv]+=semeai_disconnection_double_cut;
      return semeai_disconnection_double_cut;
    }
  
  return 0;
}


double GE_AnalyzeSemeai::disconnection_tobi(GE_Game& g, int mv, int dir)
{
  if(mv==GE_PASS_MOVE)
    return 0;
  
  if(dir==GE_UNKNOWN)
    {
      disconnection_tobi(g, mv, GE_NORTH);
      disconnection_tobi(g, mv, GE_EAST);
      return 0;
    }
  
  int color_stone = GE_WITHOUT_STONE;
  int enemy_color_stone = GE_WITHOUT_STONE;
  if(g.tc==GE_BLACK)
    {
      color_stone = GE_BLACK_STONE;
      enemy_color_stone = GE_WHITE_STONE;
    }
  else
    {
      color_stone = GE_WHITE_STONE;
      enemy_color_stone = GE_BLACK_STONE;
    }
  
  int big_mv = GE_Bibliotheque::to_big_goban[mv];
  
  int big_neighbour_left;
  int big_neighbour_right;
  
  switch(dir)
    {
    case GE_NORTH :
      big_neighbour_left = big_mv+GE_Directions::directions[GE_NORTH];
      big_neighbour_right = big_mv+GE_Directions::directions[GE_SOUTH];
      break;
      
    case GE_EAST :
      big_neighbour_left = big_mv+GE_Directions::directions[GE_WEST];
      big_neighbour_right = big_mv+GE_Directions::directions[GE_EAST];
      break;
      
    default : 
      assert(0);
      
    }
  
  int neighbour_left = GE_Bibliotheque::to_goban[big_neighbour_left];
  int neighbour_right = GE_Bibliotheque::to_goban[big_neighbour_right];
  
  if(neighbour_left<0)
    return 0;
  
  if(neighbour_right<0)
    return 0;
  
  
  int stone_left = (g.goban)->get_stone(neighbour_left);
  int stone_right = (g.goban)->get_stone(neighbour_right);
  
  if((stone_left==stone_right)
     &&(stone_left==enemy_color_stone))
    {
  assert((g.goban)->get_stone(neighbour_left)!=GE_WITHOUT_STONE);
      GE_Chain ch;
      ch.init(*(g.goban), neighbour_left);
      if(ch.belongs(neighbour_right))
	return 0;
      
      expertise_semeai[mv]+=semeai_disconnection_tobi;
      return semeai_disconnection_tobi;
    }
  
  return 0;
}

double GE_AnalyzeSemeai::connection_tobi(GE_Game& g, int mv, int dir)
{
  if(mv==GE_PASS_MOVE)
    return 0;
  
  if(dir==GE_UNKNOWN)
    {
      connection_tobi(g, mv, GE_NORTH);
      connection_tobi(g, mv, GE_EAST);
      return 0;
    }
  
  int color_stone = GE_WITHOUT_STONE;
  int enemy_color_stone = GE_WITHOUT_STONE;
  if(g.tc==GE_BLACK)
    {
      color_stone = GE_BLACK_STONE;
      enemy_color_stone = GE_WHITE_STONE;
    }
  else
    {
      color_stone = GE_WHITE_STONE;
      enemy_color_stone = GE_BLACK_STONE;
    }
  
  int big_mv = GE_Bibliotheque::to_big_goban[mv];
  
  int big_neighbour_left;
  int big_neighbour_right;
  
  switch(dir)
    {
    case GE_NORTH :
      big_neighbour_left = big_mv+GE_Directions::directions[GE_NORTH];
      big_neighbour_right = big_mv+GE_Directions::directions[GE_SOUTH];
      break;
      
    case GE_EAST :
      big_neighbour_left = big_mv+GE_Directions::directions[GE_WEST];
      big_neighbour_right = big_mv+GE_Directions::directions[GE_EAST];
      break;
      
    default : 
      assert(0);
      
    }
  
  int neighbour_left = GE_Bibliotheque::to_goban[big_neighbour_left];
  int neighbour_right = GE_Bibliotheque::to_goban[big_neighbour_right];
  
  if(neighbour_left<0)
    return 0;
  
  if(neighbour_right<0)
    return 0;
  
  
  int stone_left = (g.goban)->get_stone(neighbour_left);
  int stone_right = (g.goban)->get_stone(neighbour_right);
  
  if((stone_left==stone_right)
     &&(stone_left==color_stone))
    {
  assert((g.goban)->get_stone(neighbour_left)!=GE_WITHOUT_STONE);
      GE_Chain ch;
      ch.init(*(g.goban), neighbour_left);
      if(ch.belongs(neighbour_right))
	return 0;
      
      expertise_semeai[mv]+=semeai_connection_tobi;
      return semeai_connection_tobi;
    }
  
  return 0;
}



double GE_AnalyzeSemeai::blocage(GE_Game& g, int mv, int dir)
{
  if(mv==GE_PASS_MOVE)
    return 0;

  if(dir==GE_UNKNOWN)
    {
      blocage(g, mv, GE_NORTH);
      blocage(g, mv, GE_EAST);
      blocage(g, mv, GE_SOUTH);
      blocage(g, mv, GE_WEST);
      return 0;
    }
  
  int big_mv = GE_Bibliotheque::to_big_goban[mv];
  if(GE_Bibliotheque::is_in_border(big_mv))
    return 0;
  
  int color_stone = GE_WITHOUT_STONE;
  int enemy_color_stone = GE_WITHOUT_STONE;
  if(g.tc==GE_BLACK)
    {
      color_stone = GE_BLACK_STONE;
      enemy_color_stone = GE_WHITE_STONE;
    }
  else
    {
      color_stone = GE_WHITE_STONE;
      enemy_color_stone = GE_BLACK_STONE;
    }
  
  int big_neighbour_up;
  int big_neighbour_left;
  int big_neighbour_right;
  
  switch(dir)
    {
    case GE_NORTH :
      big_neighbour_up = big_mv+GE_Directions::directions[GE_NORTH];
      big_neighbour_left = big_mv+GE_Directions::directions[GE_WEST];
      big_neighbour_right = big_mv+GE_Directions::directions[GE_EAST];
      break;
      
    case GE_WEST :
      big_neighbour_up = big_mv+GE_Directions::directions[GE_WEST];
      big_neighbour_left = big_mv+GE_Directions::directions[GE_SOUTH];
      big_neighbour_right = big_mv+GE_Directions::directions[GE_NORTH];
      break;
      
    case GE_SOUTH :
      big_neighbour_up = big_mv+GE_Directions::directions[GE_SOUTH];
      big_neighbour_left = big_mv+GE_Directions::directions[GE_EAST];
      big_neighbour_right = big_mv+GE_Directions::directions[GE_WEST];
      break;
      
    case GE_EAST :
      big_neighbour_up = big_mv+GE_Directions::directions[GE_EAST];
      big_neighbour_left = big_mv+GE_Directions::directions[GE_NORTH];
      big_neighbour_right = big_mv+GE_Directions::directions[GE_SOUTH];
      break;
      
    default : 
      assert(0);
      
    }
  
  int neighbour_up = GE_Bibliotheque::to_goban[big_neighbour_up];
  int neighbour_left = GE_Bibliotheque::to_goban[big_neighbour_left];
  int neighbour_right = GE_Bibliotheque::to_goban[big_neighbour_right];
  
  if(neighbour_left<0)
    return 0;
  
  if(neighbour_right<0)
    return 0;

  if(neighbour_up<0)
    return 0;
  
  int stone_up = (g.goban)->get_stone(neighbour_up);
  int stone_left = (g.goban)->get_stone(neighbour_left);
  int stone_right = (g.goban)->get_stone(neighbour_right);
  
  if((stone_left==stone_right)
     &&(stone_left==GE_WITHOUT_STONE)
     &&(stone_up==enemy_color_stone))
    {
      expertise_semeai[mv]+=semeai_blocage;
      if(neighbour_up==(g.get_last_move()))
	{
	  expertise_semeai[mv]+=last_move_blocage;
	  //cerr<<"blocage"<<endl;
	}
      
      return semeai_disconnection_double_cut;
    }
  
  return 0;
}



/******************************************
 *                                        *
 * Patterns for problems of death or life *
 *                                        *
 ******************************************/
double GE_AnalyzeSemeai::kill_eye(GE_Game& g, int mv, int dir)
{
  if(mv==GE_PASS_MOVE)
    return 0;
  
  if(dir==GE_UNKNOWN)
    {
      kill_eye(g, mv, GE_NORTH);
      kill_eye(g, mv, GE_EAST);
      kill_eye(g, mv, GE_SOUTH);
      kill_eye(g, mv, GE_WEST);
      return 0;
    }
  
  int big_mv = GE_Bibliotheque::to_big_goban[mv];
  
  int color_stone = GE_WITHOUT_STONE;
  int enemy_color_stone = GE_WITHOUT_STONE;
  if(g.tc==GE_BLACK)
    {
      color_stone = GE_BLACK_STONE;
      enemy_color_stone = GE_WHITE_STONE;
    }
  else
    {
      color_stone = GE_WHITE_STONE;
      enemy_color_stone = GE_BLACK_STONE;
    }
  
  int big_neighbour_up;
  int big_neighbour_left_up;
  int big_neighbour_right_up;
  int big_neighbour_tobi_up;
  
  switch(dir)
    {
    case GE_NORTH :
      big_neighbour_up = big_mv+GE_Directions::directions[GE_NORTH];
      big_neighbour_left_up = big_neighbour_up+GE_Directions::directions[GE_WEST];
      big_neighbour_right_up = big_neighbour_up+GE_Directions::directions[GE_EAST];
      big_neighbour_tobi_up = big_neighbour_up
	+GE_Directions::directions[GE_NORTH];
      break;
      
    case GE_WEST :
      big_neighbour_up = big_mv+GE_Directions::directions[GE_WEST];
      big_neighbour_left_up = big_neighbour_up+GE_Directions::directions[GE_SOUTH];
      big_neighbour_right_up = big_neighbour_up+GE_Directions::directions[GE_NORTH];
      big_neighbour_tobi_up = big_neighbour_up
	+GE_Directions::directions[GE_WEST];
      break;
      
    case GE_SOUTH :
      big_neighbour_up = big_mv+GE_Directions::directions[GE_SOUTH];
      big_neighbour_left_up = big_neighbour_up+GE_Directions::directions[GE_EAST];
      big_neighbour_right_up = big_neighbour_up+GE_Directions::directions[GE_WEST];
      big_neighbour_tobi_up = big_neighbour_up
	+GE_Directions::directions[GE_SOUTH];
      break;
      
    case GE_EAST :
      big_neighbour_up = big_mv+GE_Directions::directions[GE_EAST];
      big_neighbour_left_up = big_neighbour_up+GE_Directions::directions[GE_NORTH];
      big_neighbour_right_up = big_neighbour_up+GE_Directions::directions[GE_SOUTH];
      big_neighbour_tobi_up = big_neighbour_up
	+GE_Directions::directions[GE_EAST];
      break;
      
    default :
      assert(0);
    }
  
  int neighbour_up = GE_Bibliotheque::to_goban[big_neighbour_up];
  
  if(neighbour_up<0)
    return 0;
  
  int stone_up = (g.goban)->get_stone(neighbour_up);
  
  if(stone_up==enemy_color_stone)
    return 0;

  int neighbour_left_up = GE_Bibliotheque::to_goban[big_neighbour_left_up];
  int neighbour_right_up = GE_Bibliotheque::to_goban[big_neighbour_right_up];
  int neighbour_tobi_up = GE_Bibliotheque::to_goban[big_neighbour_tobi_up];
  
  int stone_tobi_up = GE_EDGE; 
  int stone_left_up = GE_EDGE; 
  int stone_right_up = GE_EDGE;
  
  if(neighbour_left_up>=0)
    stone_left_up = (g.goban)->get_stone(neighbour_left_up);
  
  if(stone_left_up==color_stone) return 0;
  
  
  if(neighbour_right_up>=0)
    stone_right_up = (g.goban)->get_stone(neighbour_right_up);
  
  if(stone_right_up==color_stone) return 0;
  
  
  if(neighbour_tobi_up>=0)
    stone_tobi_up = (g.goban)->get_stone(neighbour_tobi_up);
  
  if(stone_tobi_up==color_stone) return 0;
  
  
  if(stone_right_up==stone_left_up)
    {
      if(stone_left_up!=enemy_color_stone) return 0;
      
      if(stone_left_up==stone_tobi_up)
	{
	  if(stone_left_up==enemy_color_stone)
	    {
	      /**************
	       * ?O?        *
	       * OAO        *
	       * ?X?        *
	       * A = . or @ *
	       **************/
	      
	      expertise_semeai[mv]+=death_kill_eye;
	      
	      return death_kill_eye;
	    }
	}
      else
	{
	  if(stone_left_up==enemy_color_stone)
	    {
	      /**************
	       * ?.?        *
	       * OAO        *
	       * ?X?        *
	       * A = . or @ *
	       **************/
	      //. can mean GE_EDGE
	      
	      expertise_semeai[mv]+=death_kill_semi_eye;
	      
	      return death_kill_semi_eye;
	    }
	}
      
      return 0;
    }
  
  if(stone_tobi_up!=enemy_color_stone) return 0;
  
  if(stone_tobi_up==stone_left_up)
    {
      if(stone_tobi_up==enemy_color_stone)
	{
	  /**************
	   * ?O?        *
	   * OA.        *
	   * ?X?        *
	   * A = . or @ *
	   **************/
	  //. can mean GE_EDGE
	  
	  expertise_semeai[mv]+=death_kill_semi_eye;
	  
	  return death_kill_semi_eye;
	}
    }
  
  
  if(stone_tobi_up==stone_right_up)
    {
      if(stone_tobi_up==enemy_color_stone)
	{
	  /**************
	   * ?O?        *
	   * .AO        *
	   * ?X?        *
	   * A = . or @ *
	   **************/
	  //. can mean GE_EDGE
	  
	  expertise_semeai[mv]+=death_kill_semi_eye;
	  
	  return death_kill_semi_eye;
	}
    }
  
  return 0;
}

double GE_AnalyzeSemeai::make_eye(GE_Game& g, int mv, int dir)
{
  if(mv==GE_PASS_MOVE)
    return 0;
  
  if(dir==GE_UNKNOWN)
    {
      make_eye(g, mv, GE_NORTH);
      make_eye(g, mv, GE_EAST);
      make_eye(g, mv, GE_SOUTH);
      make_eye(g, mv, GE_WEST);
      return 0;
    }
  
  int big_mv = GE_Bibliotheque::to_big_goban[mv];
  
  int color_stone = GE_WITHOUT_STONE;
  int enemy_color_stone = GE_WITHOUT_STONE;
  if(g.tc==GE_BLACK)
    {
      color_stone = GE_BLACK_STONE;
      enemy_color_stone = GE_WHITE_STONE;
    }
  else
    {
      color_stone = GE_WHITE_STONE;
      enemy_color_stone = GE_BLACK_STONE;
    }
  
  int big_neighbour_up;
  int big_neighbour_left_up;
  int big_neighbour_right_up;
  int big_neighbour_tobi_up;
  
  switch(dir)
    {
    case GE_NORTH :
      big_neighbour_up = big_mv+GE_Directions::directions[GE_NORTH];
      big_neighbour_left_up = big_neighbour_up+GE_Directions::directions[GE_WEST];
      big_neighbour_right_up = big_neighbour_up+GE_Directions::directions[GE_EAST];
      big_neighbour_tobi_up = big_neighbour_up
	+GE_Directions::directions[GE_NORTH];
      break;
      
    case GE_WEST :
      big_neighbour_up = big_mv+GE_Directions::directions[GE_WEST];
      big_neighbour_left_up = big_neighbour_up+GE_Directions::directions[GE_SOUTH];
      big_neighbour_right_up = big_neighbour_up+GE_Directions::directions[GE_NORTH];
      big_neighbour_tobi_up = big_neighbour_up
	+GE_Directions::directions[GE_WEST];
      break;
      
    case GE_SOUTH :
      big_neighbour_up = big_mv+GE_Directions::directions[GE_SOUTH];
      big_neighbour_left_up = big_neighbour_up+GE_Directions::directions[GE_EAST];
      big_neighbour_right_up = big_neighbour_up+GE_Directions::directions[GE_WEST];
      big_neighbour_tobi_up = big_neighbour_up
	+GE_Directions::directions[GE_SOUTH];
      break;
      
    case GE_EAST :
      big_neighbour_up = big_mv+GE_Directions::directions[GE_EAST];
      big_neighbour_left_up = big_neighbour_up+GE_Directions::directions[GE_NORTH];
      big_neighbour_right_up = big_neighbour_up+GE_Directions::directions[GE_SOUTH];
      big_neighbour_tobi_up = big_neighbour_up
	+GE_Directions::directions[GE_EAST];
      break;
      
    default :
      assert(0);
    }
  
  int neighbour_up = GE_Bibliotheque::to_goban[big_neighbour_up];
  
  if(neighbour_up<0)
    return 0;
    

  int stone_up = (g.goban)->get_stone(neighbour_up);
  
  if(stone_up==color_stone)
    return 0;
  
  int neighbour_left_up = GE_Bibliotheque::to_goban[big_neighbour_left_up];
  int neighbour_right_up = GE_Bibliotheque::to_goban[big_neighbour_right_up];
  int neighbour_tobi_up = GE_Bibliotheque::to_goban[big_neighbour_tobi_up];
  
  int stone_tobi_up = GE_EDGE; 
  int stone_left_up = GE_EDGE; 
  int stone_right_up = GE_EDGE;
  
  if(neighbour_left_up>=0)
    stone_left_up = (g.goban)->get_stone(neighbour_left_up);
  
  if(stone_left_up==enemy_color_stone) return 0;
  
  
  if(neighbour_right_up>=0)
    stone_right_up = (g.goban)->get_stone(neighbour_right_up);
  
  if(stone_right_up==enemy_color_stone) return 0;
  
  
  if(neighbour_tobi_up>=0)
    stone_tobi_up = (g.goban)->get_stone(neighbour_tobi_up);
  
  if(stone_tobi_up==enemy_color_stone) return 0;
  
  
  if(stone_right_up==stone_left_up)
    {
      if(stone_left_up!=color_stone) return 0;
      
      if(stone_left_up==stone_tobi_up)
	{
	  if(stone_left_up==color_stone)
	    {
	      /**************
	       * ?@?        *
	       * @A@        *
	       * ?X?        *
	       * A = . or O *
	       **************/
	      
	      expertise_semeai[mv]+=life_make_eye;
	      
	      return life_make_eye;
	    }
	}
      else
	{
	  if(stone_left_up==color_stone)
	    {
	      /**************
	       * ?.?        *
	       * @A@        *
	       * ?X?        *
	       * A = . or O *
	       **************/
	      //. can mean GE_EDGE
	      
	      expertise_semeai[mv]+=life_make_semi_eye;
	      
	      return life_make_semi_eye;
	    }
	}
      
      return 0;
    }
  
  if(stone_tobi_up!=color_stone) return 0;
  
  if(stone_tobi_up==stone_left_up)
    {
      if(stone_tobi_up==color_stone)
	{
	  /**************
	   * ?@?        *
	   * @A.        *
	   * ?X?        *
	   * A = . or O *
	   **************/
	  //. can mean GE_EDGE
	  
	  expertise_semeai[mv]+=life_make_semi_eye;
	  
	  return life_make_semi_eye;
	}
    }
  
  
  if(stone_tobi_up==stone_right_up)
    {
      if(stone_tobi_up==color_stone)
	{
	  /**************
	   * ?@?        *
	   * .A@        *
	   * ?X?        *
	   * A = . or O *
	   **************/
	  //. can mean GE_EDGE
	  
	  expertise_semeai[mv]+=life_make_semi_eye;
	  
	  return life_make_semi_eye;
	}
    }
  
  return 0;
}




void GE_AnalyzeSemeai::evaluate_move_by_tsumego_pattern(GE_Game& g, 
						     const GE_Semeai& sem, 
						     int mv)
{
  if(sem.problem==GE_GOAL_DEATH_AND_LIFE)
    {
      if(((sem.main_black_stone==GE_PASS_MOVE)&&(g.tc==GE_BLACK))
	 ||((sem.main_white_stone==GE_PASS_MOVE)&&(g.tc==GE_WHITE)))
	kill_eye(g, mv);
      else make_eye(g, mv);
    }
}



void GE_AnalyzeSemeai::evaluate_move_by_pattern(GE_Game& g, const GE_Semeai& sem, 
					     int mv)
{
  disconnection_by_double_cut(g, mv);
  disconnection_tobi(g, mv);
  connection_tobi(g, mv);
  blocage(g, mv);
  
  evaluate_move_by_tsumego_pattern(g, sem, mv);
}


void GE_AnalyzeSemeai::evaluate_moves_by_pattern(GE_Game& g, const GE_Semeai& sem, 
					      const list<int>& l_moves)
{
  list<int>::const_iterator i_mv = l_moves.begin();
  
  while(i_mv!=l_moves.end())
    {
      evaluate_move_by_pattern(g, sem, *i_mv);
      
      i_mv++;
    }
}






bool GE_AnalyzeSemeai::isExpertiseBetter(const int mv1, const int mv2)
{
  if(mv2==GE_PASS_MOVE) return true;
  if(mv1==GE_PASS_MOVE) return false;
  return (expertise_semeai[mv2]<expertise_semeai[mv1]);
}


void GE_AnalyzeSemeai::sort(list<int>& l_moves) const
{ 
  l_moves.sort(GE_AnalyzeSemeai::isExpertiseBetter);
}



//after the move
double GE_AnalyzeSemeai::evaluate_end_semeai(GE_Game& g, const GE_Semeai& sem, 
					  bool end_with_ko) const
{
  /*
    if((g.get_last_move()==GE_PASS_MOVE)
    &&(g.get_before_last_move()==GE_PASS_MOVE))
    return GE_SITUATION_SEKI;
  */
  
  int main_stone = GE_PASS_MOVE;
  int opp_main_stone = GE_PASS_MOVE;
  int main_size = 0;
  const list<int>* stones  = 0;
  int color_stone = GE_WITHOUT_STONE;
  int opp_color_stone = GE_WITHOUT_STONE;
  int opp_main_liberties = 0;
  const list<int>* vital_stones = 0;
  const list<int>* opp_vital_stones = 0;
  const list<int>* frontier_stones = 0;
  const list<int>* opp_frontier_stones = 0;
  
  assert(sem.main_black_stone>=0);
  assert(sem.main_white_stone>=0);
  assert(sem.main_black_stone<500);
  assert(sem.main_white_stone<500);
  
  if(g.tc==GE_BLACK)
    {
      main_stone = sem.main_black_stone;
      main_size = sem.main_black_size;
      stones  = &(sem.black_stones);
      color_stone = GE_BLACK_STONE;
      opp_main_stone = sem.main_white_stone;
      opp_color_stone = GE_WHITE_STONE;
      vital_stones = &(sem.black_vital_stones);
      frontier_stones = &(sem.black_vital_frontiers);
      opp_main_liberties = sem.main_white_nb_liberties;
      opp_vital_stones = &(sem.white_vital_stones);
      opp_frontier_stones = &(sem.white_vital_frontiers);
    }
  else 
    {
      main_stone = sem.main_white_stone;
      main_size = sem.main_white_size;
      stones = &(sem.white_stones);
      color_stone = GE_WHITE_STONE;
      opp_main_stone = sem.main_black_stone;
      opp_color_stone = GE_BLACK_STONE;
      vital_stones = &(sem.white_vital_stones);
      frontier_stones = &(sem.white_vital_frontiers);
      opp_main_liberties = sem.main_black_nb_liberties;
      opp_vital_stones = &(sem.black_vital_stones);
      opp_frontier_stones = &(sem.black_vital_frontiers);
    }
  
  
  list<int>::const_iterator i_v = vital_stones->begin();
  while(i_v!=vital_stones->end())
    {
      if((g.goban)->get_stone(*i_v)==GE_WITHOUT_STONE)
	{
	  return GE_LOSS;
	}
      
      i_v++;
    }
  
  list<int>::const_iterator i_f = frontier_stones->begin();
  while(i_f!=frontier_stones->end())
    {
      if((g.goban)->get_stone(*i_f)==GE_WITHOUT_STONE)
	{
	  //cerr<<"end by capture of a frontier"<<endl;
	  return GE_LOSS;
	}
      
      i_f++;
    }
int min_size_victory_capture_group = 3;

  if((g.goban)->get_stone(main_stone)!=color_stone)
    {
	if(sem.main_stone_is_vital(color_stone))
		return GE_LOSS;


      if(main_size==1)
	{
	  if((g.stones_taken.back()).size()>=min_size_victory_capture_group)
	    return GE_LOSS;
	  
	  if(stones->size()==1) return GE_LOSS;
	  
	  int nb_captures = 0;
	  list<int>::const_iterator i_s = stones->begin();
	  
	  while(i_s!=stones->end())
	    {
	      if((g.goban)->get_stone(*i_s)!=color_stone)
		nb_captures++;
	      
	      i_s++;
	    }
	  
	  if(nb_captures>2) return GE_LOSS;
	  if((nb_captures==2)&&(stones->size()==2))
	    return GE_LOSS;
	  
	  return GE_NO_RESULT;
	  
	}
      
      return GE_LOSS;
    }
  

 if((g.goban)->get_stone(opp_main_stone)!=opp_color_stone)
 {
	//assert((g.stones_taken.back()).size()<min_size_victory_capture_group);  //TODO ajuster le parametre (suivant evolution)
	assert(!sem.main_stone_is_vital(opp_color_stone));

	 return GE_NO_RESULT;
 }

  //GE_Chain ch; ch.init(*(g.goban), main_stone);
  assert((g.goban)->get_stone(opp_main_stone)!=GE_WITHOUT_STONE);
  GE_Chain ch_opp; ch_opp.init(*(g.goban), opp_main_stone);
  //if(ch.liberties+3<ch_opp.liberties) return GE_LOSS;
  
  assert((g.goban)->get_stone(opp_main_stone)!=GE_WITHOUT_STONE);
  if((ch_opp.size()>1)&&(ch_opp.liberties==1))
    return GE_WIN;
  
  if((ch_opp.size()>1)&&(ch_opp.liberties==2))
    {
      /*
	if(ch.liberties==2)
	{
	if((vital_stones->empty())
	&&(opp_vital_stones->empty()))
	{
	if((not GE_Informations::is_mobile(g, ch))
	&&(not GE_Informations::is_mobile(g, ch_opp)))
	{
	const int& free_location1 = 
	ch.free_intersections.front();
	const int& free_location2 = 
	ch.free_intersections.back();
	
	//TODO a affiner (differents types de seki)
	
	if((ch_opp.is_one_liberty(free_location1))
	&&(ch_opp.is_one_liberty(free_location2)))
	{
	//cerr<<"end by seki"<<endl;
	return GE_DRAW;
	}	  
	}
	}
	}
      */
      
      
      int att = ch_opp.free_intersections.front();
      int def = ch_opp.free_intersections.back();
      
      bool win_with_ko = false;
      for(int i = 0; i<2; i++)
	{
	  
	  if(not g.play(att))
	    {
	      //we reverse attack and defense (the last turn)
	      att = ch_opp.free_intersections.back();
	      def = ch_opp.free_intersections.front();
	      continue;
	    }
	  
	  bool is_ko = g.has_taken_ko();	  
	  
	  bool is_win = false;
	  
	  if(not g.play(def))
	    { //KO ?
	      GE_Chains chs; chs.neighbour_chains(*(g.goban), ch_opp);
	      
	      if(chs.get_nb_atari(GE_NEXT_COLOR(g.tc))==0)
		is_win = true;
	      
	      g.backward();
	      
	      //if(is_win) return GE_WIN;
	      
	      if(is_win)
		{
		  if(not is_ko)
		    return GE_WIN;
		  else win_with_ko = true; 
		}
	      
	      
	      att = ch_opp.free_intersections.back();
	      def = ch_opp.free_intersections.front();
	      continue;
	    }
	  
  assert((g.goban)->get_stone(def)!=GE_WITHOUT_STONE);
	  GE_Chain temp_ch; temp_ch.init(*(g.goban), def);
	  g.backward();
	  
	  if(temp_ch.liberties<=1)
	    {
	      GE_Chains chs; chs.neighbour_chains(*(g.goban), ch_opp);
	      //ch_opp is good (not a bug)
	      
	      if(chs.get_nb_atari(GE_NEXT_COLOR(g.tc))==0)
		{
		  is_win = true;
		  
		}
	    }
	  
	  g.backward();
	  
	  
	  //if(is_win) return GE_WIN;
	  if(is_win)
	    {
	      if(not is_ko)
		return GE_WIN;
	      else win_with_ko = true;
	    }
	  
	  
	  
	  //we reverse attack and defense (the last turn)
	  att = ch_opp.free_intersections.back();
	  def = ch_opp.free_intersections.front();
	}
      
      if(win_with_ko)
	{
	  if(end_with_ko)
	    return GE_WIN_WITH_KO;
	}
      
      //etude du shisho
      //En principe, le code ci-avant le shisho devient obsolete 
      // avec le shisho      
      GE_Tactic tact;
      if(tact.study_shisho2(g, ch_opp.links.front()))
	{
	  if(tact.ko_for_attack)
	    {
	      //cerr<<"ko in shisho"<<endl;
	      if(end_with_ko)
		return GE_WIN_WITH_KO;
	    }
	  else
	    {
	      //cerr<<"end by shisho"<<endl;
	      return GE_WIN;
	    }
	}
    }
  
//  fprintf(stderr," %d %d\n",ch_opp.liberties,opp_main_liberties);fflush(stderr);
  assert(ch_opp.liberties>0);assert(opp_main_liberties>0);
  assert(ch_opp.liberties<500);assert(opp_main_liberties<500);
  if(ch_opp.liberties>=opp_main_liberties+4)
    {
      bool is_already_in_danger = false;
      
      list<int>::const_iterator i_ovs = opp_vital_stones->begin();
      
      while(i_ovs!=opp_vital_stones->end())
	{
  assert((g.goban)->get_stone(*i_ovs)!=GE_WITHOUT_STONE);
	  GE_Chain ch_ovs; ch_ovs.init(*(g.goban), *i_ovs);
	  if(not ch_ovs.belongs(opp_main_stone))
	    {
	      is_already_in_danger = true;
	      break;
	    }
	  i_ovs++;
	}
      
      if(not is_already_in_danger)
	//TO DO implement the case of vital groups
	{
	  //for this moment, we consider that is a defeat.
	  return GE_LOSS; //NO_RESULT_TOO_LIBERTIES
	}
    }
  
  
  if(ch_opp.is_alive(*(g.goban))) //TODO ajouter le marker
    {
      //cerr<<"end by alive"<<endl;
      return GE_LOSS;
    }
  
  
  return GE_NO_RESULT; //not finished
}

bool GE_AnalyzeSemeai::can_alive(GE_Game& g, const GE_Chain& ch, 
			      const list<int>& l_moves, 
			      int max_prof, 
			      int* kill_move, 
			      GE_Marker* mk_eyes, 
			      int prof) const
{
  assert(ch.size()>0);
  
  bool is_allocated = false;
  
  if(not mk_eyes)
    {
      mk_eyes = new GE_Marker(GE_Directions::board_area);
      is_allocated = true;
    }
  
  if(ch.is_alive(*(g.goban), mk_eyes))
    {
      if(is_allocated)
	{
	  delete mk_eyes;
	  mk_eyes = 0;
	}

      
      //cerr<<"life: ";
      //GE_ListOperators::print_error(alive_sequence, move_to_string, 
      //			 GE_Directions::board_size);
      
      
      return true;
    }
  
  if(prof>max_prof)
    {
      
      if(is_allocated)
	{
	  delete mk_eyes;
	  mk_eyes = 0;
	}
      
      //cerr<<"too depth: ";
      //GE_ListOperators::print_error(alive_sequence, move_to_string, 
      //GE_Directions::board_size);
      
      return false;
    }
  
  
  //eval l_moves
  //list<int> l_moves2 = l_moves;
  
  int one_stone = ch.links.front();
  list<int>::const_iterator i_mv = l_moves.begin();
  
  bool has_a_legal_move = false;
  
  assert(l_moves.back()==GE_PASS_MOVE);
  
  
  while(true)  //i_mv!=l_moves.end())
    {

      assert(i_mv!=l_moves.end());

      int att = *i_mv;
      i_mv++;
      
      if(att==GE_PASS_MOVE)
	{
	  if(has_a_legal_move)
	    break;
	}
      
      if(not g.play(att))
	{
	  continue;
	}
      
      //alive_sequence.push_back(att);
      
      
      if(att!=GE_PASS_MOVE)
	has_a_legal_move = true;

      if(((g.goban)->get_stone(one_stone)==GE_WITHOUT_STONE))
	{
	  g.backward(); //att
	  //alive_sequence.pop_back();
	  
	  if(is_allocated)
	    {
	      delete mk_eyes;
	      mk_eyes = 0;
	    }
	  
	  if(kill_move) *kill_move = att;
	  
	  return false;
	}

      bool temp_alive = false;
      list<int>::const_iterator i_mv2 = l_moves.begin();
      while(i_mv2!=l_moves.end())
	{
	  int def = *i_mv2;
	  i_mv2++;
	  
	  if(def==GE_PASS_MOVE) continue;
	  if(def==att) continue;
	  if(not g.play(def)) continue;
	  
	  //alive_sequence.push_back(def);
	  
  assert((g.goban)->get_stone(one_stone)!=GE_WITHOUT_STONE);
	  GE_Chain ch_new;
	  ch_new.init(*(g.goban), one_stone);
	  
	  temp_alive = can_alive(g, ch_new, l_moves, max_prof, 0, 
				 mk_eyes, prof+1);
	  
	  g.backward();  //def  
	  //alive_sequence.pop_back();
	  
	  if(temp_alive) break;
	}

      g.backward(); //att
      //alive_sequence.pop_back();
      
      if(not temp_alive)
	{ 
	  if(is_allocated)
	    {
	      delete mk_eyes;
	      mk_eyes = 0;
	    }
	  
	  if(kill_move) *kill_move = att;
	  
	  return false;
	}
      
      if(att==GE_PASS_MOVE)
	break;
      
    }
  
  if(is_allocated)
    {
      delete mk_eyes;
      mk_eyes = 0;
    }
  


  return true;
  //return false;
}





bool GE_AnalyzeSemeai::can_sacrifice(GE_Game& g, 
				  const GE_Chain& ch, 
				  const GE_Chain& ch_seki, 
				  bool end_with_ko, 
				  int* sacrifice)
  const 
{
  //alive_sequence.clear();
  

  if((int)ch_seki.size()>=6)
    return false;
  
  bool generate_pass_move = false;
  
  if(((g.tc==GE_BLACK)&&(ch_seki.stone==GE_WHITE_STONE))
     ||(g.tc==GE_WHITE)&&(ch_seki.stone==GE_BLACK_STONE))
    {
      g.play(GE_PASS_MOVE);
      generate_pass_move = true;
    }
  
  list<int>::const_iterator i_fi = ch_seki.free_intersections.begin();
  
  assert(ch_seki.liberties==2);
  
  int loop = 0;
  int att = ch_seki.free_intersections.front();
  int def = ch_seki.free_intersections.back();
  
  while(loop<2)
    {
      loop++;
      
      //ch_seki.print_error();
      //g.print_error_goban(); cerr<<endl;
      
      if(not g.play(att))
	{
	  att = ch_seki.free_intersections.back();
	  def = ch_seki.free_intersections.front();
	  
	  continue;
	}
      
      //alive_sequence.push_back(att);
      
      //g.print_error_goban(); cerr<<endl;
      
      if(not g.play(def))
	{
	  g.backward();  //att
	  if(generate_pass_move)
	    g.backward();  //passmove
	  
	  if(sacrifice) *sacrifice = att;
	  
	  return true;
	}
      
      //alive_sequence.push_back(def);
      
      int max_prof = 4;
      list<int> l_moves = g.stones_taken.back();
      l_moves.push_back(GE_PASS_MOVE);
      int kill_move = GE_PASS_MOVE;
      
      
      int one_stone = ch.links.front();
      //g.print_error_goban();
      //cerr<<move_to_string(GE_Directions::board_size, one_stone)<<endl<<endl;
      
      GE_Chain ch_new;
  assert((g.goban)->get_stone(one_stone)!=GE_WITHOUT_STONE);
      ch_new.init(*(g.goban), one_stone);
 
      assert(ch.size()>0);
      assert(ch_new.size()>0);

      bool temp_alive = can_alive(g, ch_new, l_moves, max_prof, &kill_move);

      g.backward(); //def
      g.backward(); //att
      
      if(not temp_alive)
	{
	  if(sacrifice) *sacrifice = att;
	  
	  if(generate_pass_move)
	    g.backward();  //passmove
	  
	  
	  return true;
	}
      
      att = ch_seki.free_intersections.back();
      def = ch_seki.free_intersections.front();
      
      
      //alive_sequence.clear();
    }
  
  if(generate_pass_move)
    g.backward();
  
  return false;
}


bool GE_AnalyzeSemeai::is_end_by_seki(GE_Game& g, 
				   const GE_Chain& ch, 
				   bool end_with_ko)
  const 
{
  assert(ch.size()>0);

  GE_Chains chs;
  chs.neighbour_chains(*(g.goban), ch);
  
  list<GE_Chain>::const_iterator i_ch = chs.sets.begin();
  
  while(i_ch!=chs.sets.end())
    {
      if((i_ch->stone!=ch.stone)
	 &&(i_ch->liberties==2)
	 &&(not GE_Informations::is_mobile(g, *i_ch)))
	{
	  GE_Chains temp_chs;
	  temp_chs.neighbour_chains(*(g.goban), *i_ch);
	  if(temp_chs.sets.size()==2) //+itself
	    {
	      assert(i_ch->size()>0);

	      if(not can_sacrifice(g, ch, *i_ch, end_with_ko))
		{
		  return true;
		}
	      else
		{
		  //cerr<<"NO SEKI"<<endl;
		  //g.print_error_goban();
		  //cerr<<endl<<endl;
		}
	    }
	}
      
      i_ch++;
    }
  
  return false;
}



double GE_AnalyzeSemeai::evaluate_end_death_or_life(GE_Game& g, 
						 const GE_Semeai& sem, 
						 bool end_with_ko, 
						 bool end_with_seki) 
  const
{
  /*
    if((g.get_last_move()==GE_PASS_MOVE)
    &&(g.get_before_last_move()==GE_PASS_MOVE))
    return GE_SITUATION_SEKI;
  */
  
  int main_stone = GE_PASS_MOVE;
  int opp_main_stone = GE_PASS_MOVE;
  int main_size = 0;
  const list<int>* stones  = 0;
  int color_stone = GE_WITHOUT_STONE;
  int opp_color_stone = GE_WITHOUT_STONE;
  int opp_main_liberties = 0;
  const list<int>* vital_stones = 0;
  const list<int>* opp_vital_stones = 0;
  const list<int>* frontier_stones = 0;
  const list<int>* opp_frontier_stones = 0;
  
  
  
  if(g.tc==GE_BLACK)
    {
      main_stone = sem.main_black_stone;
      main_size = sem.main_black_size;
      stones  = &(sem.black_stones);
      color_stone = GE_BLACK_STONE;
      opp_main_stone = sem.main_white_stone;
      opp_color_stone = GE_WHITE_STONE;
      vital_stones = &(sem.black_vital_stones);
      frontier_stones = &(sem.black_vital_frontiers);
      opp_main_liberties = sem.main_white_nb_liberties;
      opp_vital_stones = &(sem.white_vital_stones);
      opp_frontier_stones = &(sem.white_vital_frontiers);
    }
  else 
    {
      main_stone = sem.main_white_stone;
      main_size = sem.main_white_size;
      stones = &(sem.white_stones);
      color_stone = GE_WHITE_STONE;
      opp_main_stone = sem.main_black_stone;
      opp_color_stone = GE_BLACK_STONE;
      vital_stones = &(sem.white_vital_stones);
      frontier_stones = &(sem.white_vital_frontiers);
      opp_main_liberties = sem.main_black_nb_liberties;
      opp_vital_stones = &(sem.black_vital_stones);
      opp_frontier_stones = &(sem.black_vital_frontiers);
    }
  
  /*
    list<int>::const_iterator i_v = vital_stones->begin();
    while(i_v!=vital_stones->end())
    {
    if((g.goban)->get_stone(*i_v)==GE_WITHOUT_STONE)
    {
    return GE_LOSS;
    }
    
    i_v++;
    }
  */
  
  list<int>::const_iterator i_f = frontier_stones->begin();
  while(i_f!=frontier_stones->end())
    {
      if((g.goban)->get_stone(*i_f)==GE_WITHOUT_STONE)
	{
	  //cerr<<"end by capture of a frontier"<<endl;
	  return GE_LOSS;
	}
      
      i_f++;
    }
 int min_size_victory_capture_group = 3;  
  if((main_stone>=0)
     &&((g.goban)->get_stone(main_stone)!=color_stone))
  {
	if(sem.main_stone_is_vital(color_stone))
		return GE_LOSS;

      if(main_size==1)
	{
	  if((g.stones_taken.back()).size()>=min_size_victory_capture_group)
	    return GE_LOSS;
	  
	  if(stones->size()==1) return GE_LOSS;
	  
	  int nb_captures = 0;
	  list<int>::const_iterator i_s = stones->begin();
	  
	  while(i_s!=stones->end())
	    {
	      if((g.goban)->get_stone(*i_s)!=color_stone)
		nb_captures++;
	      
	      i_s++;
	    }
	  
	  if(nb_captures>2) return GE_LOSS;
	  if((nb_captures==2)&&(stones->size()==2))
	    return GE_LOSS;
	  
	  return GE_NO_RESULT;
	}
      
      return GE_LOSS;
    }
  
  
  //GE_Chain ch; ch.init(*(g.goban), main_stone);
  
  if(opp_main_stone>=0)
    {

 if((g.goban)->get_stone(opp_main_stone)!=opp_color_stone)
 {
	//assert((g.stones_taken.back()).size()<min_size_victory_capture_group);  //TODO ajuster le parametre (suivant evolution)
	assert(!sem.main_stone_is_vital(opp_color_stone));

	 return GE_NO_RESULT;
 }

  assert((g.goban)->get_stone(opp_main_stone)!=GE_WITHOUT_STONE);
      GE_Chain ch_opp; ch_opp.init(*(g.goban), opp_main_stone);
      //if(ch.liberties+3<ch_opp.liberties) return GE_LOSS;
      
      
      if((ch_opp.size()>1)&&(ch_opp.liberties==1))
	return GE_WIN;
      
      if((ch_opp.size()>1)&&(ch_opp.liberties==2))
	{
	  /*
	    if(ch.liberties==2)
	    {
	    if((vital_stones->empty())
	    &&(opp_vital_stones->empty()))
	    {
	    if((not GE_Informations::is_mobile(g, ch))
	    &&(not GE_Informations::is_mobile(g, ch_opp)))
	    {
	    const int& free_location1 = 
	    ch.free_intersections.front();
	    const int& free_location2 = 
	    ch.free_intersections.back();
	    
	    //TODO a affiner (differents types de seki)
	    
	    if((ch_opp.is_one_liberty(free_location1))
	    &&(ch_opp.is_one_liberty(free_location2)))
	    {
	    //cerr<<"end by seki"<<endl;
	    return GE_DRAW;
	    }	  
	    }
	    }
	    }
	  */
	  
	  
	  int att = ch_opp.free_intersections.front();
	  int def = ch_opp.free_intersections.back();
	  
	  bool win_with_ko = false;
	  for(int i = 0; i<2; i++)
	    {
	      
	      if(not g.play(att))
		{
		  //we reverse attack and defense (the last turn)
		  att = ch_opp.free_intersections.back();
		  def = ch_opp.free_intersections.front();
		  continue;
		}
	      
	      bool is_ko = g.has_taken_ko();	  
	      
	      bool is_win = false;
	      
	      if(not g.play(def))
		{ //KO ?
		  GE_Chains chs; chs.neighbour_chains(*(g.goban), ch_opp);
		  
		  if(chs.get_nb_atari(GE_NEXT_COLOR(g.tc))==0)
		    is_win = true;
		  
		  g.backward();
		  
		  //if(is_win) return GE_WIN;
		  
		  if(is_win)
		    {
		      if(not is_ko)
			return GE_WIN;
		      else win_with_ko = true; 
		    }
		  
		  
		  att = ch_opp.free_intersections.back();
		  def = ch_opp.free_intersections.front();
		  continue;
		}
	      
  assert((g.goban)->get_stone(def)!=GE_WITHOUT_STONE);
	      GE_Chain temp_ch; temp_ch.init(*(g.goban), def);
	      g.backward();
	      
	      if(temp_ch.liberties<=1)
		{
		  GE_Chains chs; chs.neighbour_chains(*(g.goban), ch_opp);
		  //ch_opp is good (not a bug)
		  
		  if(chs.get_nb_atari(GE_NEXT_COLOR(g.tc))==0)
		    {
		      is_win = true;
		      
		    }
		}
	      
	      g.backward();
	      
	      
	      //if(is_win) return GE_WIN;
	      if(is_win)
		{
		  if(not is_ko)
		    return GE_WIN;
		  else win_with_ko = true;
		}
	      
	      
	      
	      //we reverse attack and defense (the last turn)
	      att = ch_opp.free_intersections.back();
	      def = ch_opp.free_intersections.front();
	    }
	  
	  if(win_with_ko)
	    {
	      if(end_with_ko)
		return GE_WIN_WITH_KO;
	    }
	  
	  //etude du shisho
	  //En principe, le code ci-avant le shisho devient obsolete 
	  // avec le shisho      
	  GE_Tactic tact;
	  if(tact.study_shisho2(g, ch_opp.links.front()))
	    {
	      if(tact.ko_for_attack)
		{
		  //cerr<<"ko in shisho"<<endl;
		  if(end_with_ko)
		    return GE_WIN_WITH_KO;
		}
	      else
		{
		  //cerr<<"end by shisho"<<endl;
		  return GE_WIN;
		}
	    }
	}
      

      if(ch_opp.liberties>=opp_main_liberties+6)
	{
	  //cerr<<"end by too liberties"<<endl;
	  
	  //for this moment, we consider that is a defeat.
	  return GE_LOSS; //NO_RESULT_TOO_LIBERTIES
	  
	  /*
	    bool is_already_in_danger = false;
	    
	    list<int>::const_iterator i_ovs = opp_vital_stones->begin();
	    
	    while(i_ovs!=opp_vital_stones->end())
	    {
	    GE_Chain ch_ovs; ch_ovs.init(*(g.goban), *i_ovs);
	    if(not ch_ovs.belongs(opp_main_stone))
	    {
	    is_already_in_danger = true;
	    break;
	    }
	    i_ovs++;
	    }
	    
	    if(not is_already_in_danger)
	    //TO DO implement the case of vital groups
	    {
	    cerr<<"end by too liberties"<<endl;
	    
	    //for this moment, we consider that is a defeat.
	    return GE_LOSS; //NO_RESULT_TOO_LIBERTIES
	    }
	  */
	}
      

      
      if(ch_opp.is_alive(*(g.goban))) //TODO ajouter le marker
	{
	  //cerr<<"end by alive"<<endl;
	  return GE_LOSS;
	}
      
      
      if(end_with_seki)
	{
	  if((ch_opp.liberties>1)
	     &&(is_end_by_seki(g, ch_opp, end_with_ko)))
	    {
	      //cerr<<"end with seki"<<endl;
	      //g.print_error_goban();
	      //cerr<<endl<<endl;
	      

	      //assert(0);

	      return GE_DRAW; //GE_LOSS_WITH_SEKI; 
	    }
	}
    }
  
  return GE_NO_RESULT; //not finished
}


double GE_AnalyzeSemeai::evaluate_end_situation(GE_Game& g, 
					     const GE_Semeai& sem, 
					     bool end_with_ko, 
					     bool end_with_seki) 
  const
{
  switch(sem.problem)
    {
    case GE_GOAL_SEMEAI : 
      return evaluate_end_semeai(g, sem, end_with_ko);

    case GE_GOAL_DEATH_AND_LIFE :
      return  evaluate_end_death_or_life(g, sem, end_with_ko, 
					 end_with_seki);
      
    default : ;
    }

  assert(0);

  return GE_NO_RESULT;
}




double GE_AnalyzeSemeai::study_semeai(GE_Game& g, const GE_Semeai& sem, 
				   list<int>* l_moves)
{
  //max_prof = 11;
  
  GE_Marker mk(GE_Directions::board_area);
  
  list<int> interesting_moves; 
  getInterestingMoves(g, sem, mk, interesting_moves);
  computeExpertiseSemeai(g, sem, interesting_moves);
  //cerr<<"avant tri "; GE_ListOperators::print_error(interesting_moves, move_to_string, GE_Directions::board_size); cerr<<endl;
  //interesting_moves.push_front(GE_PASS_MOVE);
  sort(interesting_moves);
  
  //interesting_moves.push_back(GE_PASS_MOVE);
  //cerr<<"apres tri "; GE_ListOperators::print_error(interesting_moves, move_to_string, GE_Directions::board_size); cerr<<endl;
  
  return study_semeai(g, sem, mk, interesting_moves, l_moves, -1, 1);
}

double GE_AnalyzeSemeai::study_semeai(GE_Game& g, const GE_Semeai& sem, GE_Marker& mk, 
				   list<int>& interesting_moves, 
				   list<int>* l_moves, double at_more, 
				   int prof)
{
  /* 
     double end_semeai = evaluate_end_semeai(g, sem);    
     if(end_semeai!=GE_NO_RESULT)
     {
     return end_semeai;
     }
  */
  
  //cerr<<"game:";
  //GE_ListOperators::print_error(g.game, move_to_string, GE_Directions::board_size);
  //cerr<<endl;
  
  if(prof>11/*max_prof*/) return GE_NO_RESULT; //return 0;
  
  list<int>::const_iterator i_mv = interesting_moves.begin();
  
  double best_result = -1000;
  bool no_result = false;
  //bool old_ko = ko;
  while(i_mv!=interesting_moves.end())
    {
      int location = *i_mv;
      i_mv++;
      
      if(prof==1)
	cerr<<move_to_string(GE_Directions::board_size, location)<<endl;
      
      
      //if(l_moves) cerr<<move_to_string(GE_Directions::board_size, location)<<endl;
      
      if(not g.play(location)) 
	{
	  //ko = true;
	  continue;
	}
      
      
      double end_semeai = evaluate_end_semeai(g, sem);    
      if(end_semeai!=GE_NO_RESULT)
	{
	  g.backward();

	  end_semeai = -end_semeai;
	  
	  if(l_moves)
	    {     
	      if(end_semeai==best_result)
		{
		  l_moves->push_back(location);
		  continue;
		}
	      
	      if(end_semeai>best_result)
		{
		  best_result = end_semeai;
		  l_moves->clear();
		  l_moves->push_back(location);
		  continue;
		}
	      
	      continue;
	    }
	  
	  if(end_semeai==GE_WIN)
	    {
	      return end_semeai;
	    }
	  
	  if(end_semeai>best_result)
	    {
	      best_result = end_semeai;  
	    }
	  
	  if(best_result>=at_more)
	    {
	      return at_more;
	    }
	  
	  continue;
	}
    
      
      list<int> new_list;
      getNewInterestingMoves(g, sem, interesting_moves, mk, new_list);
      //computeExpertiseSemeai(g, sem, new_list);
      //sort(new_list);
      
      
      int result = study_semeai(g, sem, mk, new_list, 0, -best_result, prof+1);
      
      g.backward();
      //ko = old_ko;

      if(result==GE_NO_RESULT)
	{
	  no_result = true;
	  continue;
	}
      
      result = -result;
      
      if(l_moves)
	{
	  
	  if(result==best_result)
	    {
	      l_moves->push_back(location);
	      continue;
	    }
	  
	  if(result>best_result)
	    {
	      best_result = result;
	      l_moves->clear();
	      l_moves->push_back(location);
	      continue;
	    }
	  
	  continue;
	}
      
      if(result==GE_WIN)
	{
	  return result;
	}
      
      if(result>best_result)
	{
	  best_result = result;  
	}
      
      if(best_result>=at_more)
	{
	  return at_more;
	}
      
    }

  if((not l_moves)&&(no_result))
    return GE_NO_RESULT;
  
  
  return best_result;
}







void GE_AnalyzeSemeai::find_a_solution
(GE_Game& g, 
 const GE_Semeai& sem, 
 int (*fct)(list<int>&, list<int>::iterator&), 
 list<int>& first_solution, 
 int& victory) const
{
  first_solution.clear();
  
  //Dresser la liste des coups pour le semeai
  list<int> interesting_moves;
  
  list<int> new_black_stones;
  list<int> new_white_stones;
  
  
  GE_Marker mk(GE_Directions::board_area);
  getInterestingMoves(g, sem, mk, interesting_moves);
  
  double end_semeai = 0;
  int nb_pass = 0;

  while(true)
    {
      if(first_solution.size()>1000)
	{
	  victory = 0;
	  break;
	}

      //Choisir un coup dans la liste des coups et le jouer
      list<int> all_moves_for_semeai = interesting_moves;
      

      list<pair<int, double> > l_eval;
      
      computeExpertiseSemeai(g, sem, all_moves_for_semeai);
      sem.eval_all_moves(g, expertise_semeai, 
			 all_moves_for_semeai, l_eval);

     
      while(true)
	{
	  //if(all_moves_for_semeai.empty())
	  if(((fct)&&(all_moves_for_semeai.empty()))
	     ||((not fct)&&(l_eval.empty())))
	    {
	      g.play(GE_PASS_MOVE);
	      nb_pass++;
	      first_solution.push_back(GE_PASS_MOVE);      
	      
	      break;
	    }
	  
	  int location = GE_PASS_MOVE;
	  
	  if(fct)
	    location = GE_ListOperators::select(all_moves_for_semeai, *fct);
	  else	    
	    {
	      location = sem.get_best_move(l_eval);
	      //if(location!=GE_PASS_MOVE)
	      GE_ListOperators::erase(all_moves_for_semeai, location);
	    }
	  
	  
	  if(g.play(location)) 
	    {
	      if(location!=GE_PASS_MOVE) 
		nb_pass = 0;
	      else nb_pass++;
	      first_solution.push_back(location);
	      break;
	    }
	}
      
      if(nb_pass==2)
	{
	  victory = 0;
	  break;
	}
      
      
      end_semeai = evaluate_end_semeai(g, sem);    
      
      if(end_semeai!=GE_NO_RESULT)
	{
	  //cerr<<end_semeai;
	  if(((int)first_solution.size())%2==1) 
	    victory = -end_semeai;
	  else victory = end_semeai;

	  //cerr<<" - "<<end_semeai<<endl;
	  break;
	}
      
      list<int> new_list;
      //getNewInterestingMoves(g, interesting_moves, mk, new_list);
      
      
      list<int> old_black_stones = new_black_stones;
      list<int> old_white_stones = new_white_stones;
      getNewInterestingMoves2(g, sem, interesting_moves, 
			      old_black_stones, 
			      old_white_stones, 
			      mk, 
			      new_list, 
			      new_black_stones, 
			      new_white_stones);
      
      interesting_moves = new_list;
      
    }
  
  int nb_backward = first_solution.size();
  
  while(nb_backward>0)
    {
      g.backward();
      nb_backward--;
    }
}




void GE_AnalyzeSemeai::moves_for_semeais(GE_Game& g, list<int>& l_moves) const
{
  //int num_semeai = 1;
  list<GE_Semeai>::const_iterator i_sem = semeais.begin();
 
  //  list<int> temp_l_moves1;
 
  GE_Marker mk(GE_Directions::board_area);
  while(i_sem!=semeais.end())
    {
      mk.update_treatment();
      //      list<int> temp_l_moves0;
      //      if (i_sem->main_black_size+i_sem->main_white_size>4)
      getInterestingMoves(g, *i_sem, mk, l_moves);
     
     
      i_sem++;
    }
 
  //cerr<<"liste de coups interessants pour tous les semeais: "<<endl; GE_ListOperators::print_error(l_moves, move_to_string, GE_Directions::board_size); cerr<<endl;
}



void GE_AnalyzeSemeai::moves_for_semeais(GE_Game& g, list<int>& bl_moves, list<int>& wl_moves) const
{
  //  int num_semeai = 1;
  
  list<GE_Semeai>::const_iterator i_sem = semeais.begin();
  
  
  //ADD JBH
  const GE_Semeai* sem = 0;
  
  int max_size = -1;
  while(i_sem!=semeais.end())
    {
      int sz_sem_b = (i_sem->black_stones).size();
      int sz_sem_w = (i_sem->white_stones).size();
      if((sz_sem_b>1)
	 &&(sz_sem_w>1)
	 &&((sz_sem_b*2)>=sz_sem_w)
	 &&((sz_sem_w*2)>=sz_sem_b))
	{
	  
	  if((sz_sem_b+sz_sem_w)>max_size)
	    {
	      max_size = (sz_sem_b+sz_sem_w);
	      sem = &(*i_sem);
	    }
	}
      
      i_sem++;
    }
  
  if(not sem) return;
  
  //sem->print_error_semeai();
  
  
  GE_Marker mkb(GE_Directions::board_area);
  GE_Marker mkw(GE_Directions::board_area);
  mkb.update_treatment();
  mkw.update_treatment();
  
  list<int>::const_iterator i_bl = sem->black_stones.begin();
  while(i_bl!=sem->black_stones.end())
    {
      getInterestingMoves(g, *sem, *i_bl, mkb, wl_moves);
      i_bl++;
    }
  list<int>::const_iterator i_wl = sem->white_stones.begin();
  while(i_wl!=sem->white_stones.end())
    {
      getInterestingMoves(g, *sem, *i_wl, mkw, bl_moves);
      i_wl++;
    }
  
  /*
    getInterestingMoves(g, *sem, mkb, bl_moves);
    getInterestingMoves(g, *sem, mkw, wl_moves);
  */
  //END ADD JBH
  
  
  
  
  /*
  //  list<int> temp_l_moves1;
  
  GE_Marker mkb(GE_Directions::board_area);
  GE_Marker mkw(GE_Directions::board_area);
  while(i_sem!=semeais.end())
  {
  mkb.update_treatment();
  mkw.update_treatment();
  //      list<int> temp_l_moves0;
  //      if (i_sem->main_black_size+i_sem->main_white_size>4)
  list<int>::const_iterator i_bl = i_sem->black_stones.begin();
  while(i_bl!=i_sem->black_stones.end())
  {
  getInterestingMoves(g, *i_bl, mkb, wl_moves);
  i_bl++;
  }
  list<int>::const_iterator i_wl = i_sem->white_stones.begin();
  while(i_wl!=i_sem->white_stones.end())
  {
  getInterestingMoves(g, *i_wl, mkw, bl_moves);
  i_wl++;
  }
  
  
  
  
  
  i_sem++;
  }
  */
  //cerr<<"liste de coups interessants pour tous les semeais: "<<endl; GE_ListOperators::print_error(l_moves, move_to_string, GE_Directions::board_size); cerr<<endl;
}


void GE_AnalyzeSemeai::moves_for_semeais(GE_Game& g, list<pair<int, double> >& l_eval) const
{
  //int num_semeai = 1;
  
  list<GE_Semeai>::const_iterator i_sem = semeais.begin();
  
  
  //ADD JBH
  const GE_Semeai* sem = 0;
  
  int max_size = -1;
  while(i_sem!=semeais.end())
    {
      int sz_sem_b = (i_sem->black_stones).size();
      int sz_sem_w = (i_sem->white_stones).size();
      if((sz_sem_b>1)
	 &&(sz_sem_w>1)
	 &&((sz_sem_b*2)>=sz_sem_w)
	 &&((sz_sem_w*2)>=sz_sem_b))
	{
	  
	  if((sz_sem_b+sz_sem_w)>max_size)
	    {
	      max_size = (sz_sem_b+sz_sem_w);
	      sem = &(*i_sem);
	    }
	}
      
      i_sem++;
    }
 
  if(not sem) return;
  
  
  GE_Marker mk(GE_Directions::board_area);
  mk.update_treatment();
  
  list<int> l_moves;
  getInterestingMoves(g, *sem, mk, l_moves);
  
  GE_AnalyzeSemeai::computeExpertiseSemeai(g, *sem, l_moves);
  sem->eval_all_moves(g, GE_AnalyzeSemeai::expertise_semeai, l_moves, l_eval);
}






void GE_AnalyzeSemeai::print_semeais() const
{
  int num_semeai = 1;
  list<GE_Semeai>::const_iterator i_sem = semeais.begin();
  
  while(i_sem!=semeais.end())
    {
      cout<<"semeai "<<num_semeai;
      i_sem->print_semeai();
      cout<<endl;
      i_sem++;
      num_semeai++;
    }
}


void GE_AnalyzeSemeai::print_error_semeais() const
{
  int num_semeai = 1;
  list<GE_Semeai>::const_iterator i_sem = semeais.begin();
  
  while(i_sem!=semeais.end())
    {
      cerr<<"semeai "<<num_semeai<<endl;
      i_sem->print_error_semeai();
      cerr<<endl;
      i_sem++;
      num_semeai++;
    }
}

void GE_AnalyzeSemeai::print_error_moves_for_semeai(GE_Game& g, bool eval) 
  const
{
  int num_semeai = 1;
  list<GE_Semeai>::const_iterator i_sem = semeais.begin();
  
  while(i_sem!=semeais.end())
    {
      cerr<<"semeai "<<num_semeai<<endl;
      i_sem->print_error_semeai();
      cerr<<endl;
      
      cerr<<"moves for semeai: "<<endl;
      GE_Marker mk(GE_Directions::board_area);
      list<int> l_moves;
      getInterestingMoves(g, *i_sem, mk, l_moves);
      GE_ListOperators::print_error(l_moves, move_to_string, 
				 GE_Directions::board_size);
      
      if(eval)
	{
	  list<int> copy_l_moves = l_moves;
	  computeExpertiseSemeai(g, *i_sem, copy_l_moves);
	  
	  cerr<<"evaluation: "<<endl;
	  list<int>::const_iterator i_mv = l_moves.begin();
	  while(i_mv!=l_moves.end())
	    {
	      cerr<<move_to_string(GE_Directions::board_size, *i_mv)<<"  ";
	      cerr<<i_sem->evaluate_move(g, *i_mv, expertise_semeai)<<endl;
	      
	      i_mv++;
	    }
	}
 
      cerr<<endl;
      
      i_sem++;
      num_semeai++;
    }
}



void GE_AnalyzeSemeai::print_error_moves_for_semeai_and_a_solution
(GE_Game& g, bool eval) const
{
  int num_semeai = 1;
  list<GE_Semeai>::const_iterator i_sem = semeais.begin();
  
  while(i_sem!=semeais.end())
    {
      cerr<<"semeai "<<num_semeai<<endl;
      i_sem->print_error_semeai();
      cerr<<endl;
      
      cerr<<"moves for semeai: "<<endl;
      GE_Marker mk(GE_Directions::board_area);
      list<int> l_moves;
      getInterestingMoves(g, *i_sem, mk, l_moves);
      GE_ListOperators::print_error(l_moves, move_to_string, 
				 GE_Directions::board_size);
      
      if(eval)
	{
	  list<int> copy_l_moves = l_moves;
	  computeExpertiseSemeai(g, *i_sem, copy_l_moves);
	  
	  cerr<<"evaluation: "<<endl;
	  list<int>::const_iterator i_mv = l_moves.begin();
	  while(i_mv!=l_moves.end())
	    {
	      cerr<<move_to_string(GE_Directions::board_size, *i_mv)<<"  ";
	      cerr<<i_sem->evaluate_move(g, *i_mv, expertise_semeai)<<endl;
	      
	      i_mv++;
	    }
	}
      
      cerr<<endl;
      
      list<int> one_solution;
      int victory;
      //find_a_solution(g, *i_sem, &GE_ListOperators::random, one_solution, victory);
      find_a_solution(g, *i_sem, 0, one_solution, victory);
      
      GE_ListOperators::print_error(one_solution, move_to_string, 
				 GE_Directions::board_size);
      switch(victory)
	{
	case GE_WIN : cerr<<"win"<<endl; break;
	case GE_LOSS : cerr<<"loss"<<endl; break;
	case GE_DRAW : cerr<<"draw"<<endl; break;
	case GE_WIN_WITH_KO : cerr<<"win with ko"<<endl; break;
	case GE_LOSS_WITH_KO : cerr<<"loss with ko"<<endl; break;
	case GE_WIN_WITH_SEKI : cerr<<"win with seki"<<endl; break;
	case GE_LOSS_WITH_SEKI : cerr<<"loss with seki"<<endl; break;
	default : cerr<<"unknown result"<<endl; assert(0);
	}
      
      cerr<<endl<<endl;
      
      i_sem++;
      num_semeai++;
    }
}


void GE_AnalyzeSemeai::study_semeais(GE_Game& g, int max_simus, 
				  GE_Informations* infos)
{
  list<GE_Semeai>::iterator i_sem = semeais.begin();
  
  while(i_sem!=semeais.end())
    {
      GE_AnalyzeSemeai as;
      //cerr<<as.evaluate_end_situation(g, *i_sem)<<endl;

      i_sem->init_tree(g, infos); 
      //(i_sem->tree_semeai).module_ko = false; //false : gain avec ou ko ou sans ko, c'est la meme chose
      
      (i_sem->tree_semeai).compute(g, max_simus);
      i_sem++;
    }  
}


int GE_AnalyzeSemeai::study_best_semeais(GE_Game& g, int max_simus, 
					 GE_Informations* infos, 
					 GE_Semeai** best_semeai)
{
  assert(best_semeai);
  
  cerr<<"i study best semeai"<<endl;

  list<GE_Semeai>::iterator i_sem = semeais.begin();
  
  GE_Semeai* sem = 0;
  
  int max_size = -1;
  while(i_sem!=semeais.end())
    {
      
      //i_sem->print_error_semeai();

      int sz_sem_b = (i_sem->black_stones).size();
      int sz_sem_w = (i_sem->white_stones).size();
      
      fprintf(stderr," this semeai has size %d x %d  --- ",sz_sem_b,sz_sem_w);fflush(stdout);
      if((sz_sem_b>3)
	 &&(sz_sem_w>3)
	 &&((sz_sem_b*3)>=sz_sem_w)
	 &&((sz_sem_w*3)>=sz_sem_b))
	{
		fprintf(stderr,"selected");
	  if((sz_sem_b+sz_sem_w)>max_size)
	    {
	      max_size = (sz_sem_b+sz_sem_w);
	      sem = &(*i_sem);
	    }
	}
      fprintf(stderr,"\n");
      i_sem++;
    }
  if(not sem) 
    {
      if(best_semeai) (*best_semeai) = 0;
      return GE_NO_RESULT;
    }
  
  /*
    sem->init_tree(g, infos); 
    //(i_sem->tree_semeai).module_ko = false; //false : gain avec ou ko ou sans ko, c'est la meme chose
    int res = (sem->tree_semeai).compute(g, max_simus);
    
    sem->main_tree_semeai = &(sem->tree_semeai);
  */
  
  int temp_best_mv = GE_PASS_MOVE;
  //int res = sem->solve(g,infos,500,6,&temp_best_mv,-1,5); 

  
  //int res = sem->solve(g,infos,10000,1,&temp_best_mv,-1,5); 
  int res = sem->solve(g,infos,max_simus,1,&temp_best_mv,-1,5); 

  if(temp_best_mv!=GE_PASS_MOVE)
    {
      assert((sem->solveur).best_tree);
      sem->main_tree_semeai = &(*(sem->solveur).best_tree);
      sem->main_tree_semeai->print_best_move();
    }
  
  if(best_semeai)
    *best_semeai = &(*sem);
  
  return res;
}


int GE_AnalyzeSemeai::study_semeais2(GE_Game& g, int max_simus, 
				  GE_Informations* infos)
{
  list<GE_Semeai>::iterator i_sem = semeais.begin();
  
  int best_move = GE_PASS_MOVE;
  int best_eval = -1;
  //int best_res = 0;
  
  int best_draw_move = GE_PASS_MOVE;
  int best_draw_eval = -1;
  
  
  while(i_sem!=semeais.end())
    {
      int temp_eval = i_sem->eval_semeai();
      
      i_sem->init_all_trees(g, infos);
      
      g.play(GE_PASS_MOVE);
      (i_sem->after_pass).compute(g, max_simus);
      g.backward();
      
      int res_after_pass = -(i_sem->after_pass).get_simplified_result();
      if(res_after_pass==GE_WIN)
	{
	  //cerr<<"semeai --> nothing"<<endl;
	  i_sem++;
	  continue;
	}
      
      (i_sem->with_ko).compute(g, max_simus);
      int res_with_ko = (i_sem->with_ko).get_simplified_result();
      //if(res_with_ko<GE_DRAW)
      if(((i_sem->with_ko).result<GE_DRAW)
	 &&((i_sem->with_ko).result!=GE_NO_RESULT))
	{
	  cerr<<"semeai --> lost"<<endl;
	  i_sem++;
	  continue;
	}
      
      (i_sem->tree_semeai).compute(g, max_simus);
      int res_tree_semeai = (i_sem->tree_semeai).get_simplified_result();
      
      if(((i_sem->tree_semeai).result==GE_WIN)
	 ||(res_tree_semeai==GE_WIN))
	{
	  //cerr<<"semeai --> win"<<endl;
	  

	  int temp_move = (i_sem->tree_semeai).get_best_move();
	  if((temp_move!=GE_PASS_MOVE)&&(temp_eval>best_eval))
	    { 
	      //best_res = GE_WIN;
	      best_eval = temp_eval;
	      best_move = temp_move;
	    }
	}
      else
	{
	  if((i_sem->tree_semeai).result==GE_LOSS)
	    {
	      i_sem++;
	      continue;
	    }
	  
	  if(res_with_ko==GE_WIN)
	    {
	      //cerr<<"semeai --> win with ko"<<endl;
	      
	      int temp_move = (i_sem->with_ko).get_best_move();
	      if((temp_move!=GE_PASS_MOVE)&&(temp_eval>best_eval))
		{ 
		  //best_res = GE_WIN_WITH_KO;
		  best_eval = temp_eval;
		  best_move = temp_move;
		}
	      
	      i_sem++;
	      continue;
	    }
	  
	  if(res_with_ko==GE_DRAW)
	    {
	      int temp_move = (i_sem->with_ko).get_best_move();
	      if((temp_move!=GE_PASS_MOVE)&&(temp_eval>best_draw_eval))
		{ 
		  best_draw_eval = temp_eval;
		  best_draw_move = temp_move;
		}
	    }
	}
            
      /*
	if(res_with_ko==GE_WIN)
	{
	(i_sem->tree_semeai).compute(g, max_simus);
	
	if((i_sem->tree_semeai).result==GE_WIN)
	{
	//cerr<<"semeai --> win"<<endl;
	
	int temp_move = (i_sem->tree_semeai).get_best_move();
	if((temp_move!=GE_PASS_MOVE)&&(temp_eval>best_eval))
	{ 
	//best_res = GE_WIN;
	best_eval = temp_eval;
	best_move = temp_move;
	}
	}
	else
	{
	//cerr<<"semeai --> win with ko"<<endl;
	
	int temp_move = (i_sem->with_ko).get_best_move();
	if((temp_move!=GE_PASS_MOVE)&&(temp_eval>best_eval))
	{ 
	//best_res = GE_WIN_WITH_KO;
	best_eval = temp_eval;
	best_move = temp_move;
	}
	}
	}
	else 
	{
	//cerr<<"semeai --> draw"<<endl;
	int temp_move = (i_sem->with_ko).get_best_move();
	if((temp_move!=GE_PASS_MOVE)&&(temp_eval>best_draw_eval))
	{ 
	best_draw_eval = temp_eval;
	best_draw_move = temp_move;
	}
	}
      */

      i_sem++;
    }
  
  if(best_move!=GE_PASS_MOVE) return best_move;
  
  return best_draw_move;
}


int GE_AnalyzeSemeai::study_semeais3(GE_Game& g, int max_simus, 
				  GE_Informations* infos)
{
  
  list<GE_Semeai>::iterator i_sem = semeais.begin();
  
  while(i_sem!=semeais.end())
    {
      i_sem->print_error_semeai();
      
      GE_AnalyzeSemeai as;
      cerr<<as.evaluate_end_situation(g, *i_sem)<<endl;
      
      i_sem->init_all_trees3(g, infos);
      
      
      if((i_sem->problem==GE_GOAL_DEATH_AND_LIFE)
	 &&(get_complexity(g, *i_sem, infos)>100)) 
	 //&&(get_complexity(g, *i_sem, infos)>13)) //TODO Define
	{
	  cerr<<"too complex"<<endl;
	  i_sem++;
	  continue;
	}
      

      cerr<<"only win :"<<endl;
      (i_sem->only_win).compute(g, max_simus);
      cerr<<endl;

      cerr<<"at least win with ko :"<<endl;
      (i_sem->at_least_win_with_ko).compute(g, max_simus);
      cerr<<endl;
      
      //cerr<<"at least draw without ko :"<<endl;
      //(i_sem->at_least_draw).compute(g, max_simus);
      //cerr<<endl;

      cerr<<"at least draw with ko :"<<endl;
      (i_sem->at_least_draw_with_ko).compute(g, max_simus);
      cerr<<endl;
      
      cerr<<"at least loss with ko :"<<endl;
      (i_sem->at_least_loss_with_ko).compute(g, max_simus);
      cerr<<endl;

      i_sem++;
    }  
  
  return GE_PASS_MOVE;
}

int GE_AnalyzeSemeai::solve_semeais(GE_Game& g, int nb_sims_by_packs, 
				 int nb_packs, 
				 GE_Informations* infos, 
				 int study_pass, 
				 int mode_print)
{
  int best_mv = GE_PASS_MOVE;
  
  double best_val_sem = -1;
  
  list<GE_Semeai>::iterator i_sem = semeais.begin();
  
  while(i_sem!=semeais.end())
    {
      if(mode_print>0)
	i_sem->print_error_semeai();
      
      double temp_val_sem = (double)i_sem->eval_semeai();
      
      
      //GE_AnalyzeSemeai as;
      //cerr<<as.evaluate_end_situation(g, *i_sem)<<endl;
      
      if((i_sem->problem==GE_GOAL_DEATH_AND_LIFE)
	 &&(get_complexity(g, *i_sem, infos)>13)) //TODO Define
	{
	  cerr<<"too complex"<<endl;
	  i_sem++;
	  continue;
	}
      
      int temp_mv = GE_PASS_MOVE;
      i_sem->solve(g, infos, nb_sims_by_packs, nb_packs, &temp_mv, 
		   study_pass, mode_print);
      
      if(temp_mv!=GE_PASS_MOVE)
	{
	  if(best_val_sem<temp_val_sem)
	    {
	      best_val_sem = temp_val_sem;
	      best_mv = temp_mv;
	    }
	}
      
      i_sem++;
    }  
  
  return best_mv;
}



int GE_AnalyzeSemeai::get_complexity(GE_Game& g, const GE_Semeai& sem, 
				  const GE_Informations* infos) const
{
  list<int> l_moves;
  
  GE_Marker mk_moves;
  mk_moves.reinit(GE_Directions::board_area);
  
  if(not infos)
    getInterestingMoves(g, sem, mk_moves, l_moves);
  else getInterestingMoves(g, sem, *infos, mk_moves, l_moves);
  
  return (int)l_moves.size();
}


/**************
 * GE_TreeSemeai *
 **************/


GE_NodeSemeai::GE_NodeSemeai()
{
  prof = 0;
  move = GE_ILLEGAL_MOVE;
  index_child = -1;
  expertise = 0;
  result = GE_NO_RESULT;
  score = 0;  //if we take a certain number of stones but not the whole
  
  last_result = GE_LOSS;
  //inhibited = false;
  
  tree = 0;
  
  wins = 0;
  wins_with_ko = 0;
  wins_with_seki = 0;
  draws = 0;
  loss_with_seki = 0; 
  loss_with_ko = 0;
  sims = 0;
  father = 0;
  
  max_last_wins = 10;//10;
  nb_last_wins = 0;
  nb_last_loss = 0;
  nb_last_draw = 0;
  
  
  set_uninhibited();
  
  /*
    int virtual_win = GE_LOSS;
    while((int)last_wins.size()<max_last_wins)
    {
    last_wins.push_back(virtual_win);
    if(virtual_win==GE_WIN) nb_last_wins++;
    if(virtual_win==GE_LOSS) nb_last_loss++;
    if(virtual_win==GE_DRAW) nb_last_draw++;
    virtual_win = -virtual_win;
    } 
  */	
}

int GE_NodeSemeai::choice_child(int* index_fils, int min_sims) const
{

  double best_win_rate = 0.1f; //-1;
  int best_move_rate = GE_PASS_MOVE;
  int temp_index_rate = -1;
  
  int best_result = -GE_NO_RESULT;
  //int best_score_result = 0;
  int best_move_result = GE_PASS_MOVE; 
  int temp_index_result = -1;

  for(int i_child = 0; i_child<(int)children.size(); i_child++)
    {
      const GE_NodeSemeai& ns = children[i_child];
      
      if(ns.result==GE_WIN) 
	continue;  //or GE_LOSS ?? a checker
      
      
      if(ns.result==GE_LOSS)
	{
	  assert(this->result==GE_WIN);


	  if(index_fils) *index_fils = i_child;
	  return ns.move;
	}
      
      switch(ns.result)
	{
	case GE_NO_RESULT :
	  {

	    double temp_win_rate = (ns.wins)/(double)(ns.sims+min_sims);
	    
	    //cerr<<temp_win_rate<<endl;


	    if(best_win_rate<temp_win_rate)
	      {
		best_win_rate = temp_win_rate;
		best_move_rate = ns.move;
		temp_index_rate = i_child;
		
	      }
	  }
	  break;
	  
	default :
	  {
	    if(best_result>ns.result)
	      {
		best_result = ns.result;
		best_move_result = ns.move;
		temp_index_result = i_child;
	      } 
	    ;
	  }
	}
    }
  
  
  if(best_move_rate!=GE_PASS_MOVE)
    {
      double virtual_win_rate = (-best_result+GE_WIN+1)/(double)(2*(GE_WIN+1));  
      // (-best_result+4)/(double)(8);  

      //cerr<<best_win_rate<<" - "<<virtual_win_rate<<endl;

      if(best_win_rate>=virtual_win_rate) 
	{
	  if(index_fils) *index_fils = temp_index_rate;
	  
	  return best_move_rate;
	}
    }
  
  if(index_fils) *index_fils = temp_index_result;
  
  return best_move_result; 
}

int GE_NodeSemeai::get_index_child(int location) const
{
  if(location==GE_PASS_MOVE) return -1;
  
  for(int i_child = 0; i_child<(int)children.size(); i_child++) 
    {
      const GE_NodeSemeai& ns = children[i_child];
      
      if(ns.move==location)
	return i_child;
    }
  
  return -1;
}


double GE_NodeSemeai::evaluate_winning_branch() const
{
  double exploitation = 1; //TODO 0 ou 1 ?
  
  const GE_NodeSemeai* temp_ns = &(*this);
  if(not temp_ns->father) return 0.5f;
  temp_ns = temp_ns->father;
  
  if(not temp_ns->father) return 0.5f;
  temp_ns = temp_ns->father;
  
  int to_divide = 1;
  
  int penalty = -5000;
  int nb_penalty = 0;
  
  while(temp_ns->father)
    {
      assert(not temp_ns->is_end());
      
      if(temp_ns->nb_last_wins==0)
	nb_penalty++;
      
      exploitation+=(temp_ns->nb_last_wins+0.5f)/(temp_ns->max_last_wins+1);
      //exploitation*=(1/temp_ns->prof)*(temp_ns->wins+0.5f)/(temp_ns->sims+1);
      //cerr<<temp_ns->wins<<" - "<<temp_ns->sims<<endl;
      
      
      temp_ns = temp_ns->father;
      if(not temp_ns->father) break;
      
      temp_ns = temp_ns->father;
      if(not temp_ns->father) break;
      
      to_divide++;
    }
  
  exploitation = exploitation/to_divide;
  
  
  //print_sequence();
  //cerr<<exploitation<<endl;
  
  return /*exploitation+*/nb_penalty*penalty;
}


double GE_NodeSemeai::evaluate_exploitation() const
{
  double exploitation = 1; //TODO 0 ou 1 ?
  
  const GE_NodeSemeai* temp_ns = &(*this);
  if(not temp_ns->father) return 0.5f;
  temp_ns = temp_ns->father;
  
  if(not temp_ns->father) return 0.5f;
  temp_ns = temp_ns->father;
  
  int to_divide = 1;
  
  while(temp_ns->father)
    {
      assert(not temp_ns->is_end());
      
      exploitation+=(temp_ns->wins+0.5f)/(temp_ns->sims+1);
      //exploitation*=(1/temp_ns->prof)*(temp_ns->wins+0.5f)/(temp_ns->sims+1);
      //cerr<<temp_ns->wins<<" - "<<temp_ns->sims<<endl;
      
      
      temp_ns = temp_ns->father;
      if(not temp_ns->father) break;
      
      temp_ns = temp_ns->father;
      if(not temp_ns->father) break;
      
      to_divide++;
    }
  
  exploitation = exploitation/to_divide;
  
  //print_sequence();
  //cerr<<exploitation<<endl;
  
  return exploitation;
}



double GE_NodeSemeai::evaluate_branch() const
{
  double exploitation = 0;
  double local_exploitation = 0;
  double eval_amaf_root = 0;
  
  
  const GE_NodeSemeai* temp_ns = &(*this);
  if(not temp_ns->father) return 0.5f;
  temp_ns = temp_ns->father;
  
  if(not temp_ns->father) return 0.5f;
  temp_ns = temp_ns->father;
  
  int penalty = -5000;
  int nb_penalty = 0;
  
  double weight_exploitation = 1500;//1500;//1500; //1000;
  double weight_amaf = 0;
  int bonus_last_wins = 0;//-5;//-100;//10;//500; //nombre negatif ????
  
  int to_divide = 1;
  int to_divide2 = 0;
  
  int last_mv = temp_ns->move;
  while(temp_ns->father)
    {
      assert(not temp_ns->is_end());
      
      if(temp_ns->nb_last_wins==0)
	nb_penalty++;
      
      //if(temp_ns->nb_last_wins/(double)(temp_ns->max_last_wins)>=0.75f)
      
      
      exploitation+=((temp_ns->wins+0.5f)/(temp_ns->sims+1));
      
      //exploitation*=(1/temp_ns->prof)*(temp_ns->wins+0.5f)/(temp_ns->sims+1);
      //cerr<<temp_ns->wins<<" - "<<temp_ns->sims<<endl;
      
      /*
	if(temp_ns->sims>5*temp_ns->max_last_wins)
	{
	//	local_exploitation
	//	+=(1-((temp_ns->nb_last_wins+0.5f)
	//	/(double)(temp_ns->max_last_wins+1)));
	  
	  
	local_exploitation+=((temp_ns->nb_last_wins+0.5f)
	/(double)(temp_ns->max_last_wins+1));
	  
	to_divide2++;
	}
      */
	  

      last_mv = temp_ns->move;
      
      temp_ns = temp_ns->father;
      if(not temp_ns->father)
	{
	  if(tree)
	    {
	      //weight_amaf = 10000.f/pow(1+(tree->root_semeai).sims, 0.5f);
	      
	      double win_rate = tree->get_win_rate_amaf(last_mv);
	      eval_amaf_root = weight_amaf*(win_rate-0.5f);
	    }
	  
	  break;
	}
      
      temp_ns = temp_ns->father;
      if(not temp_ns->father) break;
      
      to_divide++;
    }
  
  exploitation = exploitation/(double)to_divide;
  if(to_divide2!=0)
    local_exploitation = (local_exploitation/(double)to_divide2)-0.5f;
  else local_exploitation = 0;
  
  //print_sequence();
  //cerr<<exploitation<<endl;
  
  
  double eval = weight_exploitation*exploitation+
    eval_amaf_root+bonus_last_wins*local_exploitation;//+nb_penalty*penalty;
  
  return eval;
}




//the node is assumed to be a leaf
void GE_NodeSemeai::update_better(GE_NodeSemeai** best_node, double& best_diff_eval, 
			       double best_eval_brother)
{
  //double temp_diff_eval = (best_eval_brother-expertise-prof*100);
  
  //double temp_diff_eval = (expertise-best_eval_brother-prof*100); //(correct (?) but ...)
  
  //double temp_diff_eval = expertise-best_eval_brother;
  //double temp_diff_eval = expertise-best_eval_brother-prof*10;
  //double temp_diff_eval = best_eval_brother-expertise; 
  
  
  //double exploitation = evaluate_exploitation();
  //double winning_branch = evaluate_winning_branch();
  
  double eval_branch = evaluate_branch();
  
  double weight_prof = -5;//-10;//-10;//-50; //-50; 10;
  
  double temp_diff_eval = (expertise-best_eval_brother
			   +weight_prof*prof
			   +eval_branch);
  //+exploitation*1000); //+winning_branch); 
  
  //if(prof==14) cerr<<best_diff_eval<<" - "<<temp_diff_eval<<endl;
  
  if(not *best_node)
    {
      *best_node = &(*this);
      best_diff_eval = temp_diff_eval;
      return;
    }
  
  if(temp_diff_eval>best_diff_eval)
    {
      best_diff_eval = temp_diff_eval;
      *best_node = &(*this);
    } 
}



void GE_NodeSemeai::choose_in_all_nodes(GE_NodeSemeai** best_node, 
				     double& best_diff_eval)
{
  if(last_result==GE_WIN)
    {
      //assert(not (children[best_child].is_inhibited()));
      
      //cerr<<move_to_string(GE_Directions::board_size, 
      //		   (children[best_child]).move)<<endl;
      
      assert(best_child>=0);
      assert(not is_end());
      
      if(children[best_child].is_end())
	{
	  cerr<<"index: "<<children[best_child].index_child<<" "<<best_child;
	  cerr<<endl;
	  cerr<<"result: "<<children[best_child].result<<endl;
	  cerr<<"last result: "<<children[best_child].last_result<<endl;
	  cerr<<"nb children: "<<children.size()<<endl;
	  for(int i = 0; i<(int)children.size(); i++)
	    {
	      cerr<<children[i].index_child<<" "<<children[best_child].result;
	      cerr<<" "<<children[best_child].last_result<<endl;
	    }
	  
	  GE_NodeSemeai& ns_child = children[best_child]; 
	  for(int i = 0; i<(int)ns_child.children.size(); i++)
	    {
	      cerr<<ns_child.children[i].index_child<<" ";
	      cerr<<ns_child.children[i].result;
	      cerr<<" "<<ns_child.children[i].last_result<<endl;
	    }
	  
	  
	  assert(not (children[best_child]).is_end());
	}
      
      (children[best_child]).choose_in_all_nodes(best_node, best_diff_eval);
    }
  else
    {
      
      bool all_inhibited = true;
      
      for(int i = 0; i<(int)children.size(); i++)
	{
	  GE_NodeSemeai* temp_node = &(children[i]);
	  
	  if(temp_node->is_end())
	    continue;
	  
	  if(temp_node->is_inhibited()) continue;
	  
	  all_inhibited = false;
	  
	  
	  if(temp_node->is_leaf())
	    {
	      temp_node->update_better(best_node, best_diff_eval, 
				       best_child_expertise);
	    }
	  else
	    {
	      temp_node->choose_in_all_nodes(best_node, best_diff_eval);
	    } 
	}
      
      if(all_inhibited)
	{
	  for(int i = 0; i<(int)children.size(); i++)
	    {
	      GE_NodeSemeai* temp_node = &(children[i]);
	      
	      if(temp_node->is_end())
		continue;
	      
	      temp_node->set_uninhibited();
	      
	      
	      if(temp_node->is_leaf())
		{
		  temp_node->update_better(best_node, best_diff_eval, 
					   best_child_expertise);
		}
	      else
		{
		  temp_node->choose_in_all_nodes(best_node, best_diff_eval);
		} 
	    }
	}  
    }
}

void GE_NodeSemeai::print_sequence() const
{
  list<int> seq;
  //list<int> nb_sims;
  //list<int> nb_wins;
  
  const GE_NodeSemeai* temp_ns = &(*this);
  
  while(temp_ns->father)
    {
      assert(not temp_ns->is_end());
      
      //seq.push_front(temp_ns->move);
      cerr<<move_to_string(GE_Directions::board_size, temp_ns->move);
      cerr<<" "<<temp_ns->wins<<" "<<temp_ns->sims<<endl;
      
      temp_ns = temp_ns->father;
      if(not temp_ns->father) break;
    }

  //GE_ListOperators::print_error(seq, move_to_string, GE_Directions::board_size);
}




GE_TreeSemeai::GE_TreeSemeai(const GE_Semeai* sem, GE_Game* g, GE_Informations* infos)
{
  if((sem)&&(g)) init(sem, g, infos);
}


void GE_TreeSemeai::init(const GE_Semeai* sem, GE_Game* g, GE_Informations* infos)
{
  first_player = GE_BLACK;
  main_goal = GE_TO_ANYTHING;

  num_sim = 0;
  nb_nodes = 0;
  prof = 0;
  root_semeai = GE_NodeSemeai();
  root_semeai.id = nb_nodes;
  root_semeai.tree = &(*this);
  nb_nodes++;
  
  result = GE_NO_RESULT;
  current_node = &root_semeai;
  if(sem)
    semeai = &(*sem);
  else semeai = 0;
  
  mk_semeai.reinit(GE_Directions::board_area);
  
  module_ko = true;
  module_seki = true;
  seki_detected = false;

  if(g) first_player = g->tc;
  
  
  if((g)&&(sem))
    {
      GE_AnalyzeSemeai as;
      if(not infos)
	as.getInterestingMoves(*g, *sem, mk_semeai, 
			       root_interesting_moves);
      else
	as.getInterestingMoves(*g, *sem, *infos, mk_semeai, 
			       root_interesting_moves);
    }
  
  
  if(black_answers.size()==0)
    {
      black_answers.resize(GE_Directions::board_area+1);
      for(int i = 0; i<(int)black_answers.size(); i++)
	{
	  black_answers[i].resize(GE_Directions::board_area+1, 0);
	}
    }
  
  if(white_answers.size()==0)
    {
      white_answers.resize(GE_Directions::board_area+1);
      for(int i = 0; i<(int)white_answers.size(); i++)
	{
	  white_answers[i].resize(GE_Directions::board_area+1, 0);
	}
    }
  
  if(black_amaf_wins.size()==0) 
    black_amaf_wins.resize(GE_Directions::board_area+1, 0);
  
  if(black_amaf_sims.size()==0) 
    black_amaf_sims.resize(GE_Directions::board_area+1, 0);
  
  if(white_amaf_wins.size()==0) 
    white_amaf_wins.resize(GE_Directions::board_area+1, 0);
  
  if(white_amaf_sims.size()==0) 
    white_amaf_sims.resize(GE_Directions::board_area+1, 0);
}



void GE_TreeSemeai::init_leaf(GE_Game& g, const GE_Semeai& sem)
{ 
  
  bool is_founded = false;
  
////  { list<int>::const_iterator it = semeai->white_vital_stones.begin(); while (it!=semeai->white_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_WHITE); it++; }} { list<int>::const_iterator it = semeai->black_vital_stones.begin(); while (it!=semeai->black_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_BLACK); it++; }} // BLIBLI
  list<pair<int, double> > l_eval;
  
////  { list<int>::const_iterator it = semeai->white_vital_stones.begin(); while (it!=semeai->white_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_WHITE); it++; }} { list<int>::const_iterator it = semeai->black_vital_stones.begin(); while (it!=semeai->black_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_BLACK); it++; }} // BLIBLI
  if(GE_AnalyzeSemeai::studyLastMove(g, sem))
    {
      list<int> save_moves;
      if(GE_AnalyzeSemeai::getSaveMoves(g, sem, save_moves))
	{
//  { list<int>::const_iterator it = semeai->white_vital_stones.begin(); while (it!=semeai->white_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_WHITE); it++; }} { list<int>::const_iterator it = semeai->black_vital_stones.begin(); while (it!=semeai->black_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_BLACK); it++; }} // BLIBLI
	  GE_AnalyzeSemeai::computeExpertiseSemeai(g, sem, save_moves);
//  { list<int>::const_iterator it = semeai->white_vital_stones.begin(); while (it!=semeai->white_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_WHITE); it++; }} { list<int>::const_iterator it = semeai->black_vital_stones.begin(); while (it!=semeai->black_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_BLACK); it++; }} // BLIBLI
	  sem.eval_all_moves(g, GE_AnalyzeSemeai::expertise_semeai, 
			     save_moves, l_eval, this);
//  { list<int>::const_iterator it = semeai->white_vital_stones.begin(); while (it!=semeai->white_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_WHITE); it++; }} { list<int>::const_iterator it = semeai->black_vital_stones.begin(); while (it!=semeai->black_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_BLACK); it++; }} // BLIBLI
	  
	  
	  list<pair<int, double> >::const_iterator iter = l_eval.begin();
	  while(iter!=l_eval.end())
	    {
	      if((iter->second!=GE_VAL_ILLEGAL_MOVE)
		 &&(iter->second!=GE_VAL_SUICIDE_MOVE))
		{ 
		  //&&(iter->second!=GE_VAL_SUICIDE_MOVE))
		  //The last test was added with the mysterious bug 
		  //(not corrected) assert(best_node)
		  
		  is_founded = true;
		  break;
		}
	      
	      iter++;
	    }
	  
	  if(not is_founded)
	    {
	      l_eval.clear();
	      
	      //cerr<<"*** SEMEAI --- ECHEC MODULE GE_SAVE MOVE ***"<<endl; 
	    }
	}
      
    }
  
  if(not is_founded)
    {
      GE_AnalyzeSemeai::computeExpertiseSemeai(g, sem, interesting_moves);
//  { list<int>::const_iterator it = semeai->white_vital_stones.begin(); while (it!=semeai->white_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_WHITE); it++; }} { list<int>::const_iterator it = semeai->black_vital_stones.begin(); while (it!=semeai->black_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_BLACK); it++; }} // BLIBLI
      sem.eval_all_moves2(g, GE_AnalyzeSemeai::expertise_semeai, 
			  interesting_moves, l_eval, this);
//  { list<int>::const_iterator it = semeai->white_vital_stones.begin(); while (it!=semeai->white_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_WHITE); it++; }} { list<int>::const_iterator it = semeai->black_vital_stones.begin(); while (it!=semeai->black_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_BLACK); it++; }} // BLIBLI
    }
  
  
  
  double best_expertise = GE_VAL_ILLEGAL_MOVE;
  list<pair<int, double> >::const_iterator iter = l_eval.begin();
  while(iter!=l_eval.end())
    {

	if (iter->second!=GE_VAL_ILLEGAL_MOVE)
	{
		if (not g.play(iter->first))
			assert(0);
		else g.backward();
	}

      if((iter->second == GE_VAL_ILLEGAL_MOVE)
	 ||(iter->second == GE_VAL_SUICIDE_MOVE))
	{	  
	  iter++;
	  continue;
	}
      
      /*
	if(current_node->id==root_semeai.id)
	{
	cerr<<move_to_string(GE_Directions::board_size, iter->first)<<" - "<<iter->second<<endl;
	}
      */



      int new_index = (int)(current_node->children).size();
      (current_node->children).push_back(GE_NodeSemeai());
      
      GE_NodeSemeai& ns = (current_node->children).back();
      ns.id = nb_nodes;
      nb_nodes++;
      ns.father = &(*current_node);
      ns.index_child = new_index;
      ns.prof = prof+1;
      ns.tree = &(*this);
      assert(ns.father);
      
      
      ns.move = iter->first;
      ns.expertise = iter->second;
      
      if(best_expertise<ns.expertise)
	best_expertise = ns.expertise;
      
      iter++;
      
    }
  
  
  if((current_node->children).empty())
    {
      (current_node->children).push_back(GE_NodeSemeai());
      
      GE_NodeSemeai& ns = (current_node->children).back();
      ns.id = nb_nodes;
      nb_nodes++;
      ns.father = &(*current_node);
      ns.index_child = 0;
      ns.prof = prof+1;
      assert(ns.father);
      
      
      
      ns.move = GE_PASS_MOVE;
      ns.expertise = 0;
    }
  
  current_node->best_child_expertise = best_expertise;
  
}




int GE_TreeSemeai::choose_leaf(GE_NodeSemeai** next_node)
{ 
  if(current_node->is_leaf())
    {
      //TO IMPLEMENT
      assert(0);
    }
  
  GE_NodeSemeai* best_node = 0;           //&(current_node->children[0]);
  double best_eval = GE_VAL_ILLEGAL_MOVE; //best_node->expertise;
  
  int modulo = 2;
  for(int i_child = 0; i_child<(int)(current_node->children).size(); 
      i_child++)
    {
      GE_NodeSemeai* temp_node = &(current_node->children[i_child]);
      
      double child_eval = temp_node->expertise;
      
      assert(temp_node->is_leaf()); //temp_node is a leaf
      assert(not temp_node->is_end()); //temp_node is not a terminal leaf
      
      /*
	if(not temp_node->is_leaf())
	continue;
	
	if(temp_node->is_end())
	continue;
     
	
	if(not best_node)
	{
	best_node = &(*temp_node);
	best_eval = child_eval;
	  
	modulo = 2;
	continue;
	}
      */     
      
      if(child_eval==best_eval)
	{
	  
	  if(rand()%modulo==0)
	    best_node = &(*temp_node);
	  
	  modulo++;
	  continue;
	}
      
      if(child_eval>best_eval)
	{
	  best_node = &(*temp_node);
	  best_eval = child_eval;
	  
	  modulo = 2;
	}
    }
  
  assert(best_node); //ATTENTION UN buig s'est proiduit ici mais n'a pas ete 
  //corrige (bug qui semble apparemment extremement rare)
  
  
  int& location = best_node->move;
  *next_node = &(*best_node);
  
  return location;
}






double GE_TreeSemeai::choose_in_all_nodes(GE_NodeSemeai** best_node)
{
  
  //double best_child_eval = root_semeai.best_child_expertise;
  double best_diff_eval = GE_LOSS;

  root_semeai.choose_in_all_nodes(best_node, best_diff_eval);
  
  return best_diff_eval;
}


void GE_TreeSemeai::update_tree(GE_Game& g, int victory, int mode_print)
{
  
  if((mode_print>0)&&(num_sim%mode_print==0))
    {
      list<int> l_moves;
      
      list<GE_NodeSemeai*>::const_iterator i_ns = nodes_sequence.begin();
      
      if(i_ns!=nodes_sequence.end()) i_ns++;
      
      while(i_ns!=nodes_sequence.end())
	{
	  l_moves.push_back((*i_ns)->move);
	  i_ns++;
	}
      
      if(victory==GE_DRAW)
	cerr<<"D   ";
      else
	{
	  if((victory==GE_WIN)||(victory==GE_LOSS))
	    {
	      if(((victory==GE_WIN)&&(nodes_sequence.size()%2==1))
		 ||((victory==GE_LOSS)&&(nodes_sequence.size()%2==0)))
		cerr<<"V   ";
	      else 
		cerr<<"L   ";
	    }
	  else
	    {
	      if((victory==GE_WIN_WITH_KO)||(victory==GE_LOSS_WITH_KO))
		{
		  if(((victory==GE_WIN_WITH_KO)
		      &&(nodes_sequence.size()%2==1))
		     ||((victory==GE_LOSS_WITH_KO)
			&&(nodes_sequence.size()%2==0)))
		    cerr<<"VK  ";
		  else cerr<<"LK  ";
		}
	      else
		{
		  if(((victory==GE_WIN_WITH_SEKI)
		      &&(nodes_sequence.size()%2==1))
		     ||((victory==GE_LOSS_WITH_SEKI)
			&&(nodes_sequence.size()%2==0)))
		    cerr<<"VS  ";
		  else cerr<<"LS  ";
		}

	      
	    }
	}
      
      
      cerr<<"SS: ";
      GE_ListOperators::print_error(l_moves, move_to_string, 
				 GE_Directions::board_size);
      
      /*
	if((int)black_amaf_sims.size()>0)
	{
	for(int i = 0; i<GE_Directions::board_area; i++)
	{
	double b_win_rate = black_amaf_wins[i]
	/(double)(black_amaf_sims[i]+1);
	      
	double w_win_rate = white_amaf_wins[i]
	/(double)(white_amaf_sims[i]+1);
	      
	cerr<<move_to_string(GE_Directions::board_size, i)<<" : ";
	cerr<<((int)(b_win_rate*10000))/100.f<<" - ";
	cerr<<((int)(w_win_rate*10000))/100.f<<endl;
	}
	}
	cerr<<endl;
      */
    }
  
  
  //   //DEBUG
  //   assert(not nodes_sequence.empty());
  //   GE_NodeSemeai* ns = &(*nodes_sequence.back());
  //   list<GE_NodeSemeai*>::const_reverse_iterator i_ns = nodes_sequence.rbegin();
  
  
  //   while(i_ns!=nodes_sequence.rend())
  //     {
  //       cerr<<"coherence: "<<ns->id<<" - "<<(*i_ns)->id<<endl;
  //       assert(ns->id==(*i_ns)->id);
  //       ns = &(*(ns->father));
  //       i_ns++;
  //     }

  
  //   //END DEBUG
  
  
  int nb_backward = nodes_sequence.size();
  int last_index = -1;
  
  int mv1 = GE_ILLEGAL_MOVE;
  int mv2 = GE_ILLEGAL_MOVE;
  
  
  while(true)
    {
      GE_NodeSemeai* last_node = nodes_sequence.back();
      //cerr<<victory<<" "<<move_to_string(GE_Directions::board_size, last_node->move)<<endl;
      
      assert(not last_node->is_end());
      
      if(nb_backward==2)
	{
	  if((mode_print>0)&&(num_sim%mode_print==0))
	    {
	      cerr<<move_to_string(GE_Directions::board_size, last_node->move);
	      cerr<<" - "<<victory<<endl;
	    }
	}
      
      
      mv2 = mv1;
      mv1 = last_node->move;
      
      if(mv1==GE_PASS_MOVE) mv1 = GE_Directions::board_area;
      
      last_node->update(victory);
      if(victory==GE_WIN)
	{
	  last_node->best_child = last_index;
	}
      
      if((mv1!=GE_ILLEGAL_MOVE)&&(mv2!=GE_ILLEGAL_MOVE))
	{
	  if(g.tc==GE_BLACK)
	    black_answers[mv1][mv2]+=victory;
	  else white_answers[mv1][mv2]+=victory;
	}
      
      if(mv1!=GE_ILLEGAL_MOVE)
	{
	  if(g.tc==GE_WHITE)
	    {
	      if(victory==GE_LOSS)
		black_amaf_wins[mv1]++;
	      black_amaf_sims[mv1]++;
	    }
	  else
	    {
	      if(victory==GE_LOSS)
		white_amaf_wins[mv1]++;
	      white_amaf_sims[mv1]++;
	    }
	}
      
      
      //PATCH cas de nulle detectee lors de la remontee dans l'arbre
      if(last_node->result!=GE_NO_RESULT)
	victory = last_node->result;   //-last_node->result(cf nd.update) ????
      /*
	if(last_node->result==GE_DRAW)
	{
	victory = GE_DRAW;
	}
      */
      
      
      //FIN PATCH
      
      
      victory = -victory;
      
      pop_move();
      nb_backward--;
      last_index = last_node->index_child;
      
      if(nb_backward==0) return;
      
      g.backward();
    }
}


void GE_TreeSemeai::set_sequence(GE_NodeSemeai& ns, int mode_print)
{
  //cerr<<ns.index_child<<endl;

  prof = 0;
  current_node = &ns;
  clear_sequence();
   
  GE_NodeSemeai* temp_ns = &ns;
  while(true)
    {
      assert(not temp_ns->is_end());

      nodes_sequence.push_front(&(*temp_ns));
      if(not temp_ns->father) break;

      prof++;
      
      temp_ns = temp_ns->father;
    }


  if((mode_print>0)&&(num_sim%mode_print==0))
    {
      list<int> l_moves;
      
      list<GE_NodeSemeai*>::const_iterator i_ns = nodes_sequence.begin();

      if(i_ns!=nodes_sequence.end()) i_ns++;

      while(i_ns!=nodes_sequence.end())
	{
	  l_moves.push_back((*i_ns)->move);
	  i_ns++;
	}
      
      cerr<<"NS: ";
      GE_ListOperators::print_error(l_moves, move_to_string, 
				 GE_Directions::board_size);
      
      
    }

}

void GE_TreeSemeai::set_game(GE_Game& g)
{
  black_pass = false;
  white_pass = false;

  ko_for_black = false;
  ko_for_white = false;
  
  mk_semeai.update_treatment();  //pour les coups du semeai
  
  interesting_moves = root_interesting_moves;
  list<int>::const_iterator i_mv = root_interesting_moves.begin();

  while(i_mv!=root_interesting_moves.end())
    {
      mk_semeai.set_treated(*i_mv);
      i_mv++;
    }

  new_black_stones.clear();
  new_white_stones.clear();
  
  list<GE_NodeSemeai*>::const_iterator i_ns = nodes_sequence.begin();
  
  if(i_ns!=nodes_sequence.end())
    i_ns++;
  
  
  while(i_ns!=nodes_sequence.end())
    {
      int mv = (*i_ns)->move;
      
      assert(mv!=GE_ILLEGAL_MOVE);
      
      if(not g.play(mv)) assert(0);
      
      if(g.has_taken_ko())
	{
	  if(g.tc==GE_BLACK)
	    {
	      ko_for_white = true;
	      //cerr<<"*** launch simulation *** white has taken ko"<<endl;
	    }
	  else 
	    {
	      ko_for_black = true;
	      //cerr<<"*** launch simulation *** black has taken ko"<<endl;
	    }
	}
      
      
      if(mv==GE_PASS_MOVE)
	{
	  if(g.tc==GE_BLACK) 
	    white_pass = true;
	  else black_pass = true;
	}
      
      
      GE_AnalyzeSemeai as;
      as.updateNewInterestingMoves2(g, *semeai, 
				    mk_semeai, 
				    interesting_moves, 
				    new_black_stones, 
				    new_white_stones); 

      i_ns++;
    }
}

void GE_TreeSemeai::init_before_simulation(GE_Game& g)
{
  GE_NodeSemeai* next_node = 0;
  
  choose_in_all_nodes(&next_node);
  if(not next_node)
    {
      next_node = &root_semeai;

      assert(root_semeai.is_leaf());
    }
  else
    {
      //cerr<<move_to_string(GE_Directions::board_size, next_node->move)<<endl;
      
      if(next_node->move!=GE_ILLEGAL_MOVE)
	assert(next_node->father);
    }
  
  
  
  //set_sequence(*next_node);
  set_sequence(*next_node, 1000);
  set_game(g);
}


int GE_TreeSemeai::convert_to_win(GE_Game& g, int& victory) const
{
  
  if(g.tc==first_player)
    {
      assert(victory!=GE_WIN);
      
      victory = GE_LOSS;
      
      
      /*
	switch(victory)
	{
	case GE_WIN_WITH_KO : victory = GE_LOSS; break;
	#define GE_DRAW 0
	#define GE_LOSS_WITH_KO -1 :
      */
    }
  else
    {
      assert(victory!=GE_LOSS);
      
      victory = GE_WIN;
    }
  
  return victory;
}

int GE_TreeSemeai::convert_to_win_with_ko(GE_Game& g, int& victory) const 
{
  if(g.tc==first_player)
    {
      if(victory>=GE_WIN_WITH_KO)
	victory = GE_WIN;
      else victory = GE_LOSS;
    }
  else
    {
      if(victory<=GE_LOSS_WITH_KO)
	victory = GE_LOSS;
      else victory = GE_WIN;
    }
  
  return victory;
}



int GE_TreeSemeai::convert_to_win_with_seki(GE_Game& g, int& victory) const 
{
  if(g.tc==first_player)
    {
      if(victory>=GE_WIN_WITH_SEKI)
	victory = GE_WIN;
      else victory = GE_LOSS;
    }
  else
    {
      if(victory<=GE_LOSS_WITH_SEKI)
	victory = GE_LOSS;
      else victory = GE_WIN;
    }
  
  return victory;
}


int GE_TreeSemeai::convert_to_draw(GE_Game& g, int& victory) const
{
  cerr<<"*** convert_draw_without_ko *** NOT IMPLEMENTED"<<endl;
  assert(0);
  return victory;
}


int GE_TreeSemeai::convert_to_draw_with_ko(GE_Game& g, int& victory) const
{
  
  if(g.tc==first_player)
    {
      if(victory>=GE_DRAW)
	victory = GE_WIN;
      else victory = GE_LOSS;
    }
  else
    {
      if(victory<=GE_DRAW)
	victory = GE_LOSS;
      else victory = GE_WIN;
    }
  
  return victory;
}

int GE_TreeSemeai::convert_to_loss_with_seki(GE_Game& g, int& victory) const 
{
  if(g.tc==first_player)
    {
      if(victory>=GE_LOSS_WITH_SEKI)
	victory = GE_WIN;
      else victory = GE_LOSS;
    }
  else
    {
      if(victory<=GE_WIN_WITH_SEKI)
	victory = GE_LOSS;
      else victory = GE_WIN;
    }
  
  return victory;
}


int GE_TreeSemeai::convert_to_loss_with_ko(GE_Game& g, int& victory) const 
{
  if(g.tc==first_player)
    {
      if(victory>=GE_LOSS_WITH_KO)
	victory = GE_WIN;
      else victory = GE_LOSS;
    }
  else
    {
      if(victory<=GE_WIN_WITH_KO)
	victory = GE_LOSS;
      else victory = GE_WIN;
    }
  
  return victory;
}



int GE_TreeSemeai::convert_victory(GE_Game& g, int& victory) const
{
  if(main_goal==GE_TO_ANYTHING) return victory;
  if((victory==GE_WIN)||(victory==GE_LOSS)) return victory;
  
  switch(main_goal)
    {
    case GE_TO_WIN : return convert_to_win(g, victory);
    case GE_TO_WIN_WITH_KO : return convert_to_win_with_ko(g, victory);
    case GE_TO_DRAW : return convert_to_draw(g, victory);
    case GE_TO_DRAW_WITH_KO : return convert_to_draw_with_ko(g, victory);
    case GE_TO_LOSS_WITH_KO : return convert_to_loss_with_ko(g, victory);
    case GE_TO_WIN_WITH_SEKI : return convert_to_win_with_seki(g, victory);
    case GE_TO_LOSS_WITH_SEKI : return convert_to_loss_with_seki(g, victory);
      
    default : assert(0);
    }
  
  return victory;
}





int GE_TreeSemeai::evaluate_end_simulation(GE_Game& g, int& victory) const
{
  bool end_with_ko = true;
  bool end_with_seki = true;
  
  if(g.tc==first_player)
    {
      if((main_goal==GE_TO_WIN)||(main_goal==GE_TO_DRAW))
	end_with_ko = false;
      
      if((main_goal==GE_TO_WIN)||(main_goal==GE_TO_WIN_WITH_KO))
	end_with_seki = false;
    }
  else
    {
      if(main_goal==GE_TO_LOSS_WITH_KO) 
	end_with_ko = false;
      
      if((main_goal==GE_TO_DRAW_WITH_KO)||(main_goal==GE_TO_LOSS_WITH_KO))
	end_with_seki = false;
    }
  
  GE_AnalyzeSemeai as;
  
  victory = as.evaluate_end_situation(g, *semeai, end_with_ko, end_with_seki);    
  if(victory==GE_NO_RESULT) return GE_NO_RESULT;
  
  if(g.tc==GE_BLACK)
    {
      if((victory==GE_WIN)&&(ko_for_black))
	victory = GE_WIN_WITH_KO;
      
      if((victory==GE_LOSS)&&(ko_for_white))
	victory = GE_LOSS_WITH_KO;
      
      if((victory>GE_DRAW)&&(black_pass==true))
	victory = GE_DRAW;
      else
	{
	  if((victory<GE_DRAW)&&(white_pass==true))
	    victory = GE_DRAW;
	}
    }
  else
    {
      if((victory==GE_WIN)&&(ko_for_white))
	victory = GE_WIN_WITH_KO;
      
      if((victory==GE_LOSS)&&(ko_for_black))
	victory = GE_LOSS_WITH_KO;
      
      if((victory>GE_DRAW)&&(white_pass==true))
	victory = GE_DRAW;
      else
	{
	  if((victory<GE_DRAW)&&(black_pass==true))
	    victory = GE_DRAW;
	}
    }
  
  if((main_goal==GE_TO_ANYTHING)&&(not module_ko))
    {
      if(victory==GE_WIN_WITH_KO)
	victory = GE_WIN;
      
      if(victory==GE_LOSS_WITH_KO)
	victory = GE_LOSS;
    }
  
  
  convert_victory(g, victory);
  
  return victory;
}


int GE_TreeSemeai::launch_simulation(GE_Game& g)
{  
  int nb_pass = 0;
  int victory = GE_NO_RESULT;
  GE_AnalyzeSemeai as;
  

  if(current_node->id!=root_semeai.id)
    {
      if(evaluate_end_simulation(g, victory)!=GE_NO_RESULT)
	return victory;
    }
  
//  { list<int>::const_iterator it = semeai->white_vital_stones.begin(); while (it!=semeai->white_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_WHITE); it++; }} { list<int>::const_iterator it = semeai->black_vital_stones.begin(); while (it!=semeai->black_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_BLACK); it++; }}

  while(true)
    {
      //g.print_error_goban(); 
      
      assert(current_node);
      
      assert(current_node->is_leaf());
      
//  { list<int>::const_iterator it = semeai->white_vital_stones.begin(); while (it!=semeai->white_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_WHITE); it++; }} { list<int>::const_iterator it = semeai->black_vital_stones.begin(); while (it!=semeai->black_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_BLACK); it++; }} // BLIBLI
      init_leaf(g, *semeai);
      
//  { list<int>::const_iterator it = semeai->white_vital_stones.begin(); while (it!=semeai->white_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_WHITE); it++; }} { list<int>::const_iterator it = semeai->black_vital_stones.begin(); while (it!=semeai->black_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_BLACK); it++; }} // BLIBLI
      assert(not current_node->is_leaf());
      
//  { list<int>::const_iterator it = semeai->white_vital_stones.begin(); while (it!=semeai->white_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_WHITE); it++; }} { list<int>::const_iterator it = semeai->black_vital_stones.begin(); while (it!=semeai->black_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_BLACK); it++; }} // BLIBLI
      
      GE_NodeSemeai* next_node = 0;
      int location = choose_leaf(&next_node);
      
      if(!g.is_allowed(location))
      { 
	      fprintf(stderr,"affichage:\n");fflush(stderr);    // FIXME BUG
	      g.print_error_goban();
	      fprintf(stderr,"affichage2:\n");fflush(stderr);
	     cerr<<move_to_string(GE_Directions::board_size,location)<<endl;

	      assert(0);
      }
      
//  { list<int>::const_iterator it = semeai->white_vital_stones.begin(); while (it!=semeai->white_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_WHITE); it++; }} { list<int>::const_iterator it = semeai->black_vital_stones.begin(); while (it!=semeai->black_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_BLACK); it++; }} // BLIBLI
      
      nodes_sequence.push_back(&(*next_node));
//  { list<int>::const_iterator it = semeai->white_vital_stones.begin(); while (it!=semeai->white_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_WHITE); it++; }} { list<int>::const_iterator it = semeai->black_vital_stones.begin(); while (it!=semeai->black_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_BLACK); it++; }} // BLIBLI
      current_node = &(*next_node);
      
//  { list<int>::const_iterator it = semeai->white_vital_stones.begin(); while (it!=semeai->white_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_WHITE); it++; }} { list<int>::const_iterator it = semeai->black_vital_stones.begin(); while (it!=semeai->black_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_BLACK); it++; }} // BLIBLI
      assert(not current_node->is_end());
      
      //cerr<<move_to_string(GE_Directions::board_size, location)<<endl;
      
//  { list<int>::const_iterator it = semeai->white_vital_stones.begin(); while (it!=semeai->white_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_WHITE); it++; }} { list<int>::const_iterator it = semeai->black_vital_stones.begin(); while (it!=semeai->black_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_BLACK); it++; }} // BLIBLI
      
      if(not g.play(location))
      { 
	      fprintf(stderr,"affichage:\n");fflush(stderr);    // FIXME BUG
	      g.print_error_goban();
	      fprintf(stderr,"affichage2:\n");fflush(stderr);
	     cerr<<move_to_string(GE_Directions::board_size,location)<<endl;

	      assert(0);
      }
      
//  { list<int>::const_iterator it = semeai->white_vital_stones.begin(); while (it!=semeai->white_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_WHITE); it++; }} { list<int>::const_iterator it = semeai->black_vital_stones.begin(); while (it!=semeai->black_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_BLACK); it++; }} // BLIBLI
      if(g.has_taken_ko())
	{
	  if(g.tc==GE_BLACK)
	    {
	      ko_for_white = true;
	      //cerr<<"*** launch simulation *** white has taken ko"<<endl;
	    }
	  else 
	    {
	      ko_for_black = true;
	      //cerr<<"*** launch simulation *** black has taken ko"<<endl;
	    }
	}
      
      prof++;
      
//  { list<int>::const_iterator it = semeai->white_vital_stones.begin(); while (it!=semeai->white_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_WHITE); it++; }} { list<int>::const_iterator it = semeai->black_vital_stones.begin(); while (it!=semeai->black_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_BLACK); it++; }} // BLIBLI
      if(location!=GE_PASS_MOVE) 
	nb_pass = 0;
      else 
	{
	  if(g.tc==GE_BLACK)
	    white_pass = true;
	  else black_pass = true;
	  
	  nb_pass++;
	}
      
//  { list<int>::const_iterator it = semeai->white_vital_stones.begin(); while (it!=semeai->white_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_WHITE); it++; }} { list<int>::const_iterator it = semeai->black_vital_stones.begin(); while (it!=semeai->black_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_BLACK); it++; }} // BLIBLI
      if((black_pass)&&(white_pass))
	{
	  victory = 0;
	  convert_victory(g, victory);
	  break;
	}
      
//  { list<int>::const_iterator it = semeai->white_vital_stones.begin(); while (it!=semeai->white_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_WHITE); it++; }} { list<int>::const_iterator it = semeai->black_vital_stones.begin(); while (it!=semeai->black_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_BLACK); it++; }} // BLIBLI
      
      if(nb_pass==2)
	{
	  victory = 0;
	  convert_victory(g, victory);
	  break;
	}
      
//  { list<int>::const_iterator it = semeai->white_vital_stones.begin(); while (it!=semeai->white_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_WHITE); it++; }} { list<int>::const_iterator it = semeai->black_vital_stones.begin(); while (it!=semeai->black_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_BLACK); it++; }} // BLIBLI
      //if(prof>30)
      if(prof>GE_PARAM_MAX_LENGTH_SIMULATION)
	{
	  victory = 0;
	  convert_victory(g, victory);
	  break;
	}
      
      
//  { list<int>::const_iterator it = semeai->white_vital_stones.begin(); while (it!=semeai->white_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_WHITE); it++; }} { list<int>::const_iterator it = semeai->black_vital_stones.begin(); while (it!=semeai->black_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_BLACK); it++; }} // BLIBLI
      if(evaluate_end_simulation(g, victory)!=GE_NO_RESULT)
	break;
      
//  { list<int>::const_iterator it = semeai->white_vital_stones.begin(); while (it!=semeai->white_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_WHITE); it++; }} { list<int>::const_iterator it = semeai->black_vital_stones.begin(); while (it!=semeai->black_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_BLACK); it++; }} // BLIBLI
      as.updateNewInterestingMoves2(g, *semeai, mk_semeai, 
				    interesting_moves, 
				    new_black_stones, 
				    new_white_stones);    
//  { list<int>::const_iterator it = semeai->white_vital_stones.begin(); while (it!=semeai->white_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_WHITE); it++; }} { list<int>::const_iterator it = semeai->black_vital_stones.begin(); while (it!=semeai->black_vital_stones.end()) { assert((g.goban)->get_stone(*it)==GE_BLACK); it++; }} // BLIBLI
    }
  
  
  return victory;    
}


void GE_TreeSemeai::play_sequence(GE_Game& g)
{
  //if(root_semeai.is_leaf()) cerr<<"the root node is a leaf"<<endl;
  init_before_simulation(g);
  int victory = launch_simulation(g);
  //update_tree(g, victory);
  update_tree(g, victory, 1000);
}

int GE_TreeSemeai::compute(GE_Game& g, int max_simus, int* location, 
			bool mode_print)
{
  assert(semeai);
  
  if(main_goal!=GE_TO_ANYTHING)
    assert(module_ko==true);
  
  
  //(g.goban)->print_error();
  
  
  num_sim = 0;
  
  if(max_simus>0)
    {
      
      while(result==GE_NO_RESULT)
	{

	  num_sim++;
	  
	  if(num_sim%1000==0)
	    {
	      cerr<<"simulation "<<num_sim;
	      cerr<<"   ("<<nb_nodes<<" nodes)"<<endl;
	      if(num_sim>1) print_best_move();
	      //cerr<<"goban before simulation"<<endl;
	      //g.print_error_goban();
	      //cerr<<endl;
	    }
	  
	  play_sequence(g);
	  
	  result = root_semeai.result;

	  if(num_sim>=max_simus)
	    break;
	}
    }
  else
    {
      while(result==GE_NO_RESULT)
	{
	
	  num_sim++;
	  
	  if(num_sim%1000==0)
	    {
	      cerr<<"simulation "<<num_sim;
	      cerr<<"   ("<<nb_nodes<<" nodes)"<<endl;
	      if(num_sim>1) print_best_move();
	      //cerr<<"goban before simulation"<<endl;
	      //g.print_error_goban();
	      //cerr<<endl;
	    }

	  play_sequence(g);
	  
	  result = root_semeai.result;
	  
	}
    }
  
  /*
  //cerr<<"after "<<num_sim<<" simulations, "<<endl;
  if(num_sim>1) print_best_move();
  
  int best_location = GE_PASS_MOVE;
  if(result==GE_WIN)
  {
  int i_child = root_semeai.best_child;
  GE_NodeSemeai& best_node = root_semeai.children[i_child];
      
  best_location = best_node.move; 
      
  if(location)
  *location = best_location;
  }
  
  //if(mode_print)
  
  {
  cerr<<"after "<<num_sim<<" simulations, "<<endl;
  switch(result)
  {
  case GE_DRAW : cerr<<"draw"<<endl; break;
  case GE_LOSS : cerr<<"loss"<<endl; break;
  case GE_WIN : 
  cerr<<"win "; 
  cerr<<move_to_string(GE_Directions::board_size, best_location)<<endl;
  break;
  case GE_LOSS_WITH_KO : cerr<<"loss with ko"<<endl; break;
  case GE_WIN_WITH_KO :
  cerr<<"win with ko ";
  //cerr<<move_to_string(GE_Directions::board_size, best_location);
  cerr<<endl;
  break;
	
  case GE_LOSS_WITH_SEKI : cerr<<"loss with seki"<<endl; break;
  case GE_WIN_WITH_SEKI :
  cerr<<"win with seki ";
  //cerr<<move_to_string(GE_Directions::board_size, best_location);
  cerr<<endl;
  break;
	
  default : ;
  }
    
  }
  */

  return result;
}


int GE_TreeSemeai::progress(GE_Game& g, int nb_simus, int* nb_real_simus)
{
  if(result!=GE_NO_RESULT) //TODO 
    {
      assert(0);
      if(nb_real_simus) *nb_real_simus = 0;
      return result;
    }
  
  int temp_sim = 0;

  while(result==GE_NO_RESULT)
    {
      temp_sim++;
      num_sim++;
      
      /*
	if(num_sim%1000==0)
	{
	cerr<<"simulation "<<num_sim;
	cerr<<"   ("<<nb_nodes<<" nodes)"<<endl;
	if(num_sim>1) print_best_move();
	//cerr<<"goban before simulation"<<endl;
	//g.print_error_goban();
	//cerr<<endl;
	}
      */
      
      play_sequence(g);
      
      result = root_semeai.result;
      
      if(temp_sim>=nb_simus)
	break;
    }
  
  if(nb_real_simus) nb_real_simus = &temp_sim;
  fprintf(stderr,"in progress: %d\n",result);fflush(stderr);
  return result;
}




int GE_TreeSemeai::get_best_move(int& res, int& best_eval1, 
			      int& best_eval2, 
			      const GE_NodeSemeai** ns) const
{
  res = result;
  
  if(result!=GE_NO_RESULT)
    {
      
      if(result==GE_WIN)
	{
	  int i_child = root_semeai.best_child;
	  const GE_NodeSemeai& best_node = root_semeai.children[i_child];
	 
	  *ns = &best_node;
	  return best_node.move; 
	}
      
      for(int i = 0; i<(int)root_semeai.children.size(); i++)
	{
	  if((root_semeai.children[i]).result==-result)
	    {
	      *ns = &root_semeai.children[i];
	      return root_semeai.children[i].move;
	    }
	  
	}

      assert(0);
    }
  
  int best_mv1 = GE_PASS_MOVE;
  int best_res1 = GE_LOSS;
  const GE_NodeSemeai* best_child1 = 0;
  
  int best_mv2 = GE_PASS_MOVE;
  int best_res2 = -1;
  const GE_NodeSemeai* best_child2 = 0;
  
  
  for(int i = 0; i<(int)root_semeai.children.size(); i++)
    {
      const GE_NodeSemeai& child_node = (root_semeai.children)[i];
      
      if(child_node.result!=GE_NO_RESULT)
	{
	  if((-child_node.result)>=best_res1)
	    {
	      best_res1 = -child_node.result;
	      best_mv1 = child_node.move;
	      best_child1 = &child_node;
	    }
	  
	  continue;
	}

      /*
	if(child_node.sims>=best_res2)
	{
	best_res2 = child_node.sims;
	best_mv2 = child_node.move;
	best_child2 = &child_node;
	}
      */
      
      /*
	int temp_result = 
	(int)((child_node.wins/(float)child_node.sims)*10000);
	if(temp_result>=best_res2)
	{
	best_res2 = temp_result;
	best_mv2 = child_node.move;
	best_child2 = &child_node;
	}
      */
      
      int temp_result = child_node.wins;
      if(temp_result>=best_res2)
	{
	  best_res2 = temp_result;
	  best_mv2 = child_node.move;
	  best_child2 = &child_node;
	  
	}
    }
  
  assert(best_child2);
  assert(best_res1!=GE_WIN);
  
  if(best_res1==GE_LOSS)
    {
      *ns = &(*best_child2); 
      best_eval2 = best_res2;
      return best_mv2;
    }
  
  if(best_res1==GE_LOSS_WITH_KO)
    {
      if((not best_child2)
	 ||(best_child2->wins/(float)best_child2->sims<0.5f))
	{
	  *ns = &(*best_child1); 
	  best_eval1 = best_res1;
	  return best_mv1;
	}
      else
	{
	  *ns = &(*best_child2); 
	  best_eval2 = best_res2;
	  return best_mv2;
	}
    }

  /*
    if(best_res1==GE_LOSS_WITH_SEKI)
    {
    if((not best_child2)
    ||(best_child2->wins/(float)best_child2->sims<0.6f))
    {
    *ns = &(*best_child1); 
    best_eval1 = best_res1;
    return best_mv1;
    }
    else
    {
    *ns = &(*best_child2); 
    best_eval2 = best_res2;
    return best_mv2;
    }
    }
  */
  
  
  if(best_res1==GE_DRAW)
    {
      if((not best_child2)
	 ||(best_child2->wins/(float)best_child2->sims<0.7f))
	{
	  *ns = &(*best_child1); 
	  best_eval1 = best_res1;
	  return best_mv1;
	}
      else
	{
	  *ns = &(*best_child2); 
	  best_eval2 = best_res2;
	  return best_mv2;
	}
    }
  
  /*
    if(best_res1==GE_WIN_WITH_SEKI)
    {
    if((not best_child2)
    ||(best_child2->wins/(float)best_child2->sims<0.8f))
    {
    *ns = &(*best_child1); 
    best_eval1 = best_res1;
    return best_mv1;
    }
    else
    {
    *ns = &(*best_child2); 
    best_eval2 = best_res2;
    return best_mv2;
    }
    }
  */
  
  if(best_res1==GE_WIN_WITH_KO)
    {
      /*
       *ns = &(*best_child1); 
       best_eval1 = best_res1;
       return best_mv1;
      */
      
      if((not best_child2)
	 ||(best_child2->wins/(float)best_child2->sims<0.9f))
	{
	  *ns = &(*best_child1); 
	  best_eval1 = best_res1;
	  return best_mv1;
	}
      else
	{
	  *ns = &(*best_child2); 
	  best_eval2 = best_res2;
	  return best_mv2;
	}
    }
  
  assert(0);
  
  return GE_PASS_MOVE;
}



int GE_TreeSemeai::get_best_move() const
{
  
  int res = GE_NO_RESULT;
  int best_eval1 = GE_LOSS;
  int best_eval2 = -1;
  const GE_NodeSemeai* ns = 0;
  
  return get_best_move(res, best_eval1, best_eval2, &ns);
}


int GE_TreeSemeai::get_simplified_result() const
{  
  int res = GE_NO_RESULT;
  int best_eval1 = GE_LOSS;
  int best_eval2 = -1;
  const GE_NodeSemeai* ns = 0;
  
  get_best_move(res, best_eval1, best_eval2, &ns);

  if(res!=GE_NO_RESULT)
    {
      if(res>GE_DRAW) return GE_WIN;
      if(res<GE_DRAW) return GE_LOSS;
      return GE_DRAW;
    }
  
  assert(best_eval1!=GE_NO_RESULT);
  
  if(best_eval1!=GE_LOSS)
    {
      if(best_eval1>GE_DRAW) return GE_WIN;
      if(best_eval1<GE_DRAW) return GE_LOSS;
      return GE_DRAW;
    }
  
  assert(ns);

  float win_rate = (ns->wins+0.5f)/(float)(ns->sims+1);
  if(win_rate>0.5f) return GE_WIN;
  if(win_rate<0.5f) return GE_LOSS;
  return GE_DRAW;
}


double GE_TreeSemeai::get_win_rate_amaf(int location) const
{
  
  if((black_amaf_wins.size()==0)
     ||(white_amaf_wins.size()==0)
     ||(black_amaf_sims.size()==0)
     ||(white_amaf_sims.size()==0))
    {
      //cerr<<"*** evaluate_by_tree *** ";
      //cerr<<"the tree_semeai amaf is not initialized"<<endl;
      return 0.5f;
    }
  
  
  int temp_mv = location;
  if(temp_mv==GE_PASS_MOVE)
    temp_mv = GE_Directions::board_area;
  
  double amaf_win_rate = 0;
  
  if(first_player==GE_BLACK)
    {
      amaf_win_rate = (black_amaf_wins[temp_mv]+0.5f)
	/(double)(black_amaf_sims[temp_mv]+1);
    }
  else
    {
      amaf_win_rate = (white_amaf_wins[temp_mv]+0.5f)
	/(double)(white_amaf_sims[temp_mv]+1);
    }
  
  return amaf_win_rate;
}



void GE_TreeSemeai::print_best_move() const
{

  int res = GE_NO_RESULT;
  int best_eval1 = GE_LOSS;
  int best_eval2 = -1;
  const GE_NodeSemeai* ns = 0;
  
  int best_location = get_best_move(res, best_eval1, best_eval2, &ns);
  cerr<<move_to_string(GE_Directions::board_size, best_location);
  cerr<<"  (";

  if(res!=GE_NO_RESULT)
    {
      string msg_result;
      GE_Tactic::result_to_string(res, msg_result);
      
      cerr<<msg_result;
    }
  else
    {
      if(best_eval2>=0)
	{
	  assert(ns);
	  //cerr<<best_eval2<<" sims - ";
	  //cerr<<((int)((ns->wins/(float)ns->sims)*10000))/100.f<<"%";

	  //cerr<<ns->sims<<" sims - ";
	  //cerr<<best_eval2/100.f<<"%";

	  cerr<<ns->sims<<" sims - ";
	  cerr<<((int)((ns->wins+0.5f)/(float)(ns->sims+1)*10000))/100.f<<"%";
	}
      else
	{
	  string msg_result;
	  GE_Tactic::result_to_string(best_eval1, msg_result);
	  
	  cerr<<msg_result<<" ?";
	}
    }
  
  cerr<<")"<<endl;
}


/***************************
 ***************************
 **                       **
 ** The solveur of semeai **
 **                       **
 ***************************
 ***************************/


void GE_SolveurSemeai::init(GE_Semeai & sem, GE_Game& g, GE_Informations* infos, 
			    int study_after_pass)
{
  is_init = true;
  
  study_pass = study_after_pass;
  if(study_pass>0)
    {
      after_pass.init(&sem, &g, infos);
      //after_pass.module_ko = false;
      after_pass.main_goal = GE_TO_WIN_WITH_KO;
    }
  
  
  current_tree = &only_win;
  best_tree = &only_win;
  
  precedent_tree = &at_least_win_with_ko;
  follow_tree = &only_win;
  
  only_win.init(&sem, &g, infos);
  only_win.main_goal = GE_TO_WIN;
  
  at_least_win_with_ko.init(&sem, &g, infos);
  at_least_win_with_ko.main_goal = GE_TO_WIN_WITH_KO;
  
  //at_least_draw.init(this, &g, infos);
  //at_least_draw.main_goal = GE_TO_DRAW;
  
  at_least_draw_with_ko.init(&sem, &g, infos);
  at_least_draw_with_ko.main_goal = GE_TO_DRAW_WITH_KO;
  
  at_least_loss_with_ko.init(&sem, &g, infos);
  at_least_loss_with_ko.main_goal = GE_TO_LOSS_WITH_KO;
}


void GE_SolveurSemeai::update_precedent_and_follow_trees()
{
  if(current_tree==&only_win)
    {
      precedent_tree = &at_least_win_with_ko;
      follow_tree = &only_win;
      return;
    }
  
  if(current_tree==&at_least_win_with_ko)
    {
      precedent_tree = &at_least_draw_with_ko;
      follow_tree = &only_win;
      return;
    }
  
  if(current_tree==&at_least_win_with_ko)
    {
      precedent_tree = &at_least_draw_with_ko;
      follow_tree = &only_win;
      return;
    }
  
  if(current_tree==&at_least_draw_with_ko)
    {
      precedent_tree = &at_least_loss_with_ko;
      follow_tree = &at_least_win_with_ko;
      return;
    }
  
  
  if(current_tree==&at_least_loss_with_ko)
    {
      precedent_tree = &at_least_loss_with_ko;
      follow_tree = &at_least_draw_with_ko;
      return;
    }
}


void GE_SolveurSemeai::update_current_tree(GE_TreeSemeai& ts)
{
  if(ts.result==GE_NO_RESULT)
    current_tree = &ts;
}




void GE_SolveurSemeai::next_tree()
{
  int res = GE_NO_RESULT;
  double stats_win = -1;
  
  get_current_result(res, stats_win);
  
  if(current_tree==&only_win)
    {
      if(res!=GE_NO_RESULT)
	{
	  if(res==GE_LOSS)
	    update_current_tree(at_least_win_with_ko);
	  
	  return;
	}
      
      if(stats_win<0.33f)
	{
	  update_current_tree(at_least_win_with_ko);
	}
      
      
      
      //current_tree = &at_least_win_with_ko;
      
      return;
    }
  
  if(current_tree==&at_least_win_with_ko)
    {
      if(res!=GE_NO_RESULT)
	{
	  if(res==GE_WIN)
	    update_current_tree(only_win);
	  else update_current_tree(at_least_draw_with_ko);
	  
	  return;
	}
      
      if(stats_win>=0.66f)
	update_current_tree(only_win);
      
      if(stats_win<0.33f)
	update_current_tree(at_least_draw_with_ko);
      
      return;
    }
  
  if(current_tree==&at_least_draw_with_ko)
    {
      if(res!=GE_NO_RESULT)
	{
	  if(res==GE_WIN)
	    update_current_tree(at_least_win_with_ko);
	  else update_current_tree(at_least_loss_with_ko);
	  
	  return;
	}
      
      if(stats_win>=0.66f)
	update_current_tree(at_least_win_with_ko);
      
      if(stats_win<0.33f)
	update_current_tree(at_least_loss_with_ko);
      
      return;
    }
  
  if(current_tree==&at_least_loss_with_ko)
    {
      if(res!=GE_NO_RESULT)
	{
	  if(res==GE_WIN)
	    update_current_tree(at_least_draw_with_ko);
	  
	  return;
	}
      
      if(stats_win>=0.66f)
	update_current_tree(at_least_draw_with_ko);
      
      return;
    }  
}

void GE_SolveurSemeai::update_next_tree()
{
  next_tree();
  update_precedent_and_follow_trees();
}


int GE_SolveurSemeai::get_result(int& res, double& stats_life, 
			      const GE_TreeSemeai* ts, 
			      int min_sim) const
{
  const GE_TreeSemeai* temp_tree = 0;
  
  int best_eval1 = GE_LOSS; //doesn't use
  int best_eval2 = -1;
  
  if(ts)
    temp_tree = &(*ts);
  else temp_tree = &(*best_tree);
  
  if(temp_tree->nb_nodes<=1)
    return GE_PASS_MOVE;

  const GE_NodeSemeai* ns = 0; 
  int best_location = temp_tree->get_best_move(res, best_eval1, 
					       best_eval2, &ns);
  
  stats_life = (ns->wins+0.5f)/(float)(ns->sims+min_sim);
  
  //cerr<<move_to_string(GE_Directions::board_size,best_location)<<endl;
  //cerr<<best_location<<endl;

  return best_location;
}


int GE_SolveurSemeai::get_current_result(int& res, double& stats_life) 
  const
{
  return get_result(res, stats_life, &(*current_tree));
}


int GE_SolveurSemeai::get_best_result(int& res, double& stats_life) 
  const
{
  return get_result(res, stats_life, &(*best_tree));
}

//TODO +AUTRES CAS A METTRE
bool GE_SolveurSemeai::is_solved() const
{
  if((only_win.result==GE_WIN)||(at_least_loss_with_ko.result==GE_LOSS))
    return true;


  if(current_tree->result==GE_WIN)
    {
      if(follow_tree->result==GE_LOSS)
	return true;
    }
  
  if(current_tree->result==GE_LOSS)
    {
      if(precedent_tree->result==GE_WIN)
	return true;
    }
     
  return false;
}

int GE_SolveurSemeai::get_best_move(int min_sims) 
{
  int temp_res = 0;
  double stats_life = 0;
  int temp_mv = GE_PASS_MOVE;
  temp_mv = get_result(temp_res, stats_life, &only_win, min_sims);
  
  if((temp_res==GE_WIN)||(stats_life>=0.33f))
    {
      best_tree = &only_win;
      return temp_mv;
    }
  
  temp_mv = get_result(temp_res, stats_life, &at_least_win_with_ko, min_sims);
  

  if((temp_res==GE_WIN)||(stats_life>=0.33f))
    {
      best_tree = &at_least_win_with_ko;
      return temp_mv;
    }
  
  temp_mv = get_result(temp_res, stats_life, &at_least_draw_with_ko, min_sims);

  if((temp_res==GE_WIN)||(stats_life>=0.33f))
    { 
      best_tree = &at_least_draw_with_ko;
      return temp_mv;
    }
  
  temp_mv = get_result(temp_res, stats_life, &at_least_loss_with_ko, min_sims);

  if((temp_res==GE_WIN)||(stats_life>=0.33f))
    { 
      best_tree = &at_least_loss_with_ko;
      return temp_mv;
    }
  
  
  return GE_PASS_MOVE;
}


int GE_SolveurSemeai::solve(GE_Game& g, int nb_simus, int nb_times, 
			    int* best_move, int mode_print)
{
  int num_loop = 0;
  int res = GE_NO_RESULT;
  
  if(study_pass>0)
    {
      g.play(GE_PASS_MOVE);
      (after_pass).compute(g, study_pass);
      g.backward();
      
      int res_after_pass = -after_pass.get_simplified_result();
      if(res_after_pass==GE_WIN)
	{ 
	  
	  if(best_move) *best_move = GE_PASS_MOVE;
	  
	  if(mode_print>=0)
	    {
	      cerr<<"### RESULT ###"<<endl;
	      cerr<<"pass (win)"<<endl;
	    }
	  
	  return res_after_pass;
	}
    }
  
  
  
  while(true)
    {
      num_loop++;
      
      int nb_real_simus = 0;
      
      assert(current_tree);
      
      //cerr<<current_tree->result<<endl;
      res = current_tree->progress(g, nb_simus, &nb_real_simus);
     fprintf(stderr,"after progress: %d\n",res); fflush(stderr);
      if((mode_print>0)&&(num_loop%mode_print==0))
	{
	  cerr<<"loop "<<num_loop<<endl;
	  print_error();
	}
      
      if(is_solved()) break;
      
      update_next_tree();
      
      if(num_loop>=nb_times) break;
    }
  

  if(mode_print>=0)
    {
      if(current_tree==&only_win)
	cerr<<"only win ---> ";
      
      if(current_tree==&at_least_win_with_ko)
	cerr<<"at least win with ko ---> ";
      
      if(current_tree==&at_least_draw_with_ko)
	cerr<<"at least draw with ko ---> ";
      
      if(current_tree==&at_least_loss_with_ko)
	cerr<<"at least loss with ko ---> ";
      
      
      cerr<<"### RESULT ###"<<endl;
      if(res==GE_WIN)
	{
	  current_tree->print_best_move();
	}
      else
	{
	  if((res==GE_LOSS)&&(precedent_tree->result==GE_WIN))
	    precedent_tree->print_best_move();
	  else cerr<<"GE_NO_RESULT"<<endl;
	}
    }

  if(best_move)
    {
      *best_move = get_best_move();
      
      cerr<<"best move for semeai: ";
      if((*best_move)!=GE_PASS_MOVE)
	cerr<<move_to_string(GE_Directions::board_size,*best_move)<<endl;
      else cerr<<"pass move"<<endl;
    }
  
  fprintf(stderr,"here %d!\n",res);fflush(stderr);
  return res;
}



void GE_SolveurSemeai::print_error() const
{
  cerr<<"only win: "<<endl;
  if(only_win.num_sim<=1)
    cerr<<"#####"<<endl;
  else
    {
      cerr<<only_win.num_sim<<" sims - ";
      only_win.print_best_move();    
    }
  
  cerr<<endl;
 
  
  cerr<<"at least, win with ko: "<<endl;
  if(at_least_win_with_ko.num_sim<=1)
    cerr<<"#####"<<endl;
  else
    {
      cerr<<at_least_win_with_ko.num_sim<<" sims - ";
      at_least_win_with_ko.print_best_move();    
    }
  
  cerr<<endl;
  
  
  cerr<<"at least, draw with ko: "<<endl;
  if(at_least_draw_with_ko.num_sim<=1)
    cerr<<"#####"<<endl;
  else
    {
      cerr<<at_least_draw_with_ko.num_sim<<" sims - ";
      at_least_draw_with_ko.print_best_move();    
    }
  
  cerr<<endl;
  
  
  cerr<<"at least, loss with ko: "<<endl;
  if(at_least_loss_with_ko.num_sim<=1)
    cerr<<"#####"<<endl;
  else
    {
      cerr<<at_least_loss_with_ko.num_sim<<" sims - ";
      at_least_loss_with_ko.print_best_move();    
    }
  
  cerr<<endl;
  
}


/*
  victory = as.evaluate_end_semeai(g, *semeai);    
  if(victory!=GE_NO_RESULT)
  {
  if(g.tc==GE_BLACK)
  {
  if((victory==GE_WIN)&&(black_pass==true))
  victory = 0;
  else
  {
  if((victory==GE_LOSS)&&(white_pass==true))
  victory = 0;
  }
  }
  else
  {
  if((victory==GE_WIN)&&(white_pass==true))
  victory = 0;
  else
  {
  if((victory==GE_LOSS)&&(black_pass==true))
  victory = 0;
  }
  }
  
  return victory;
  }
*/

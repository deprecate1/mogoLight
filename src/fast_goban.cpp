#include "fast_goban.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int GE_FastGoban::stop = 1000;

void GE_FastGoban::merge_chains(int seed_chain1, int seed_chain2)
{
  if(id_chains[seed_chain1]==id_chains[seed_chain2]) return;
  
  int next1 = chains[seed_chain1];
  int next2 = chains[seed_chain2];
  
  chains[seed_chain1] = next2;
  chains[seed_chain2] = next1;
  
  int current_location = seed_chain1;
  do
    {
      id_chains[current_location] = id_chains[seed_chain2];
      current_location = chains[current_location];
    }
  while(current_location!=seed_chain1);
}

bool GE_FastGoban::place_stone(int location, int color_stone, int max_length)
{
  //play pass
  
  if(location==GE_PASS_MOVE)
    {
      ko = GE_ILLEGAL_MOVE;
      return true;
    }

  //legality of the move - the ko
  
  if(location==ko)
    {
      goban[location] = color_stone;
      
      int temp_length = 0;
      bool is_dead = is_dead1(game.back(), temp_length);
      
      goban[location] = GE_WITHOUT_STONE;

      if(is_dead)
	{
	  if(temp_length==1)
	    return false; //illegal move (ko)
	}
      else return false; //illegal move (no freedoms)
        
      //assert(0);
      //return false;
    }
  int enemy_color_stone = -color_stone;

  int neighbour_north = location+GE_Directions::directions[GE_NORTH];
  int neighbour_south = location+GE_Directions::directions[GE_SOUTH];
  int neighbour_west = location+GE_Directions::directions[GE_WEST];
  int neighbour_east = location+GE_Directions::directions[GE_EAST];

  if(is_eye(color_stone, neighbour_north, neighbour_south, neighbour_west, neighbour_east))
    {
      if(avoid_play_eye(location, enemy_color_stone))
	return false;
    }

  goban[location] = color_stone;
   

  
  //disappearance of enemy dead chains
  int nb_eaten_stones = 0;
  if(goban[neighbour_north]==enemy_color_stone) 
    nb_eaten_stones += clean_dead_chain(neighbour_north, max_length);
  
  if(goban[neighbour_south]==enemy_color_stone) 
    nb_eaten_stones += clean_dead_chain(neighbour_south, max_length);
  
  if(goban[neighbour_west]==enemy_color_stone) 
    nb_eaten_stones += clean_dead_chain(neighbour_west, max_length);
  
  if(goban[neighbour_east]==enemy_color_stone) 
    nb_eaten_stones += clean_dead_chain(neighbour_east, max_length);
  
  chains[location] = location;
  
  // legality of the move - freedoms
  
  if((nb_eaten_stones==0)&&(is_dead0(location, color_stone))&&(is_dead0(neighbour_north, color_stone))
     &&(is_dead0(neighbour_south, color_stone))&&(is_dead0(neighbour_west, color_stone))
     &&(is_dead0(neighbour_east, color_stone)))
    {

      chains[location] = -1;
      goban[location] = GE_WITHOUT_STONE;
      return false;
    }
  
  id_chains[location] = nb_chains;
  nb_chains++;
 
  if(nb_eaten_stones!=1) ko = GE_ILLEGAL_MOVE;
  
  //merge of friend neighbour chain
  
  if(goban[neighbour_north]==color_stone) merge_chains(location, neighbour_north);
  if(goban[neighbour_south]==color_stone) merge_chains(location, neighbour_south);
  if(goban[neighbour_west]==color_stone) merge_chains(location, neighbour_west);
  if(goban[neighbour_east]==color_stone) merge_chains(location, neighbour_east);
   
  return true;
}

bool GE_FastGoban::is_surrounded4(int location, int color_stone)
{
  int neighbour_north = location+GE_Directions::directions[GE_NORTH];
  if(goban[neighbour_north]==color_stone)
    return true;
  
  int neighbour_south = location+GE_Directions::directions[GE_SOUTH];
  if(goban[neighbour_south]==color_stone)
    return true;
  
  int neighbour_west = location+GE_Directions::directions[GE_WEST];
  if(goban[neighbour_west]==color_stone)
    return true;
  
  int neighbour_east = location+GE_Directions::directions[GE_EAST];
  if(goban[neighbour_east]==color_stone)
    return true;
  
  return false;
}


bool GE_FastGoban::is_dead0(int seed_chain, int color_stone)
{
  if(goban[seed_chain]!=color_stone)
    return true;
  
  int current_location = seed_chain;
  
  do
    {
      if(is_surrounded4(current_location, GE_WITHOUT_STONE))
	return false;
      
      current_location = chains[current_location];
    }
  while(current_location!=seed_chain);
  
  return true;
}


bool GE_FastGoban::is_dead1(int seed_chain, int& chain_length)
{
  chain_length = 0;
  int current_location = seed_chain;
  
  do
    {
      if(is_surrounded4(current_location, GE_WITHOUT_STONE))
	return false;
      
      current_location = chains[current_location];
      chain_length++;
    }
  while(current_location!=seed_chain);
  
  return true;
}

int GE_FastGoban::clean_dead_chain(int seed_chain, int max_length)
{
  int chain_length = 0;
  if(is_dead1(seed_chain, chain_length))
    {
      int current_location = seed_chain;

      if(chain_length>=max_length)
	{
	  do
	    {
	      goban[current_location] = GE_WITHOUT_STONE;
	      current_location = chains[current_location];
	    }
	  while(current_location!=seed_chain);
	}
      else
	{
	  if(chain_length==1) ko = current_location;
	  
	  do
	    {
	      goban[current_location] = GE_WITHOUT_STONE;
	      possible_moves.push_back(current_location);
	      current_location = chains[current_location];
	    }
	  while(current_location!=seed_chain);
	}
      
      return chain_length;
    }
  
  return 0;
}


int GE_FastGoban::play(int max_length)
{
  int color_stone = turn;
  
  nb_choice = possible_moves.size();
  
  while(nb_choice>0)
    {
      //std::cout<<"stay: "<<nb_choice<<std::endl;
      
      int index = rand()%nb_choice;
      int location = possible_moves[index];
      
      //std::cout<<turn<<" "<<GE_Directions::to_move_string(location)<<std::endl;

      if(not place_stone(location, color_stone, max_length))
	{
	  int last_index = nb_choice-1;
	  possible_moves[index] = possible_moves[last_index];
	  possible_moves[last_index] = location;
	  
	  nb_choice--;
	  continue;
	}
      
      
      int last_free_location = possible_moves[possible_moves.size()-1];
      possible_moves[index] = last_free_location;   
      possible_moves.pop_back();
      game.push_back(location);
      
      //the next player
      turn = -turn;
      
      return location;
    }
  
  //the next player
  turn = -turn;
  ko = GE_ILLEGAL_MOVE;
  game.push_back(GE_PASS_MOVE);
  return GE_PASS_MOVE;
}

bool GE_FastGoban::is_eye(int color_stone, int north, int west, int south, int east)
{
  if((goban[north]!=GE_EDGE)&&(goban[north]!=color_stone))
    return false;
  
  if((goban[west]!=GE_EDGE)&&(goban[west]!=color_stone))
    return false;
  
  if((goban[south]!=GE_EDGE)&&(goban[south]!=color_stone))
    return false;

  if((goban[east]!=GE_EDGE)&&(goban[east]!=color_stone))
    return false;

  return true;
}


bool GE_FastGoban::avoid_play_eye(int location, int enemy_color_stone)
{
  //in order avoid infinite repetition
  
  int neighbour_nw = location+GE_Directions::directions[GE_NORTHWEST];
  int neighbour_se = location+GE_Directions::directions[GE_SOUTHEAST];
  int neighbour_sw = location+GE_Directions::directions[GE_SOUTHWEST];
  int neighbour_ne = location+GE_Directions::directions[GE_NORTHEAST];
  
  int nb_enemy = 0;

  if(goban[neighbour_nw]==enemy_color_stone)
    {
      if(goban[neighbour_se]==GE_EDGE)
	return false;
      nb_enemy++;
    }
  
  if(goban[neighbour_se]==enemy_color_stone) 
    {
      if(goban[neighbour_nw]==GE_EDGE)
	return false;
      nb_enemy++;
    }
  
  if(goban[neighbour_sw]==enemy_color_stone) 
    {
      if(goban[neighbour_ne]==GE_EDGE)
	return false;
      nb_enemy++;
    }
  
  if(goban[neighbour_ne]==enemy_color_stone) 
    {
      if(goban[neighbour_se]==GE_EDGE)
	return false;
      nb_enemy++;
    }
  
  if(nb_enemy<2)
    return true;
  
  return false;
}



void GE_FastGoban::init()
{
  chains.resize(GE_Directions::big_board_area, -1);
  id_chains.resize(GE_Directions::big_board_area, -1);
  goban.resize(GE_Directions::big_board_area, GE_WITHOUT_STONE);
  

  for(int i = 0; i<GE_Directions::big_board_size; i++)
    {
      //chains[i*GE_Directions::big_board_size] = -1;
      goban[i*GE_Directions::big_board_size] = GE_EDGE;
    }

  for(int i = 0; i<GE_Directions::big_board_size; i++)
    {
      //chains[i*GE_Directions::big_board_size+(GE_Directions::big_board_size-1)] = -1;
      goban[i*GE_Directions::big_board_size+(GE_Directions::big_board_size-1)] = GE_EDGE;
    }
  
  for(int j = 0; j<GE_Directions::big_board_size; j++)
    {
      //chains[j] = -1;
      goban[j] = GE_EDGE;
    }

  for(int j = 0; j<GE_Directions::big_board_size; j++)
    {
      //chains[(GE_Directions::big_board_size-1)*GE_Directions::big_board_size+j] = -1;
      goban[(GE_Directions::big_board_size-1)*GE_Directions::big_board_size+j] = GE_EDGE;
    }

  clear();

 //  for(int i = 0; i<GE_Directions::big_board_size; i++)
//     {
//       for(int j = 0; j<GE_Directions::big_board_size; j++)
// 	cout<<goban[i*GE_Directions::big_board_size+j]<<" ";
//       cout<<endl;
//     }
}


void GE_FastGoban::clear(int old_turn)
{
  
  possible_moves.clear();
  game.clear();
  game.push_back(GE_ILLEGAL_MOVE);
  turn = old_turn;
  ko = GE_ILLEGAL_MOVE;

  nb_chains = 0;
  
  for(int i = 1; i<GE_Directions::big_board_size-1; i++)
    for(int j = 1; j<GE_Directions::big_board_size-1; j++)
      {
	chains[i*GE_Directions::big_board_size+j] = -1;
	id_chains[i*GE_Directions::big_board_size+j] = -1;
	goban[i*GE_Directions::big_board_size+j] = GE_WITHOUT_STONE;
	possible_moves.push_back(i*GE_Directions::big_board_size+j);
      }
  
  nb_choice = possible_moves.size();
  score = 0;
}

void GE_FastGoban::reinit(const GE_Goban& gob)
{
  for(int i = 0; i<gob.height; i++)
    for(int j = 0; j<gob.width; j++)
      goban[(i+1)*GE_Directions::big_board_size+(j+1)] = (gob.board)[i][j];
  
  //TO REINITIALIZE OTHER ATTRIBUTES

  init_chains(gob);
  init_possible_moves();
  game.clear();
  game.push_back(GE_ILLEGAL_MOVE);
  turn = GE_BLACK_STONE;
  ko = GE_ILLEGAL_MOVE;
  
  nb_choice = possible_moves.size();
  score = 0;
}

void GE_FastGoban::init_chains(const GE_Goban& gob)
{
  GE_Chains chs(gob);
  
  list<GE_Chain>::const_iterator i_ch = chs.sets.begin();
  nb_chains = 0;
  
  while(i_ch!=chs.sets.end())
    {
      list<int>::const_iterator i_s = (i_ch->links).begin();
      
      int last_elt = GE_Directions::to_big_move((i_ch->links).back());
      while(i_s!=(i_ch->links).end())
	{
	  int big_location = GE_Directions::to_big_move(*i_s);
	  
	  id_chains[big_location] = nb_chains;
	  chains[big_location] = last_elt;
	  
	  i_s++;
	  last_elt = big_location;
	}
      

      nb_chains++;
      i_ch++;
    }
}

void GE_FastGoban::init_possible_moves()
{
  possible_moves.clear();

  int start = 1+GE_Directions::directions[GE_SOUTH];
  int end = start+GE_Directions::board_size;
  
  while(start<GE_Directions::limit_higher_location)
    {
      for(int j = start; j<end; j++)
	{
	  if(goban[j]==GE_WITHOUT_STONE)
	    possible_moves.push_back(j);
	}
      
      start+=GE_Directions::directions[GE_SOUTH];
      end+=GE_Directions::directions[GE_SOUTH];
    }

  nb_choice = possible_moves.size();
}



void GE_FastGoban::toGoban(GE_Goban& gob) const
{
  gob = GE_Goban(GE_Directions::board_size, GE_Directions::board_size);
  
  for(int i = 1; i<GE_Directions::big_board_size-1; i++)
    for(int j = 1; j<GE_Directions::big_board_size-1; j++)
      (gob.board)[i-1][j-1] = goban[i*GE_Directions::big_board_size+j];
}

void GE_FastGoban::fromGame(const GE_Game& game) 
{
  this->clear();
  
  turn = (game.tc==GE_BLACK) ? GE_BLACK_STONE : GE_WHITE_STONE;
  
  int start = 1+GE_Directions::directions[GE_SOUTH];
  int end = start+GE_Directions::board_size;
  
  while(start<GE_Directions::limit_higher_location)
    {
      for(int j = start ; j<end; j++)
	goban[j] = (game.goban)->get_stone(GE_Directions::to_move(j));
      
      start+=GE_Directions::directions[GE_SOUTH];
      end+=GE_Directions::directions[GE_SOUTH];
    }

  init_chains(*(game.goban));
  init_possible_moves();
}

void GE_FastGoban::copy(const GE_FastGoban& fg) 
{   
  possible_moves = fg.possible_moves;
  game = fg.game;
  turn = fg.turn;
  ko = fg.ko;
  
  nb_chains = fg.nb_chains;
  
  int start = 1+GE_Directions::directions[GE_SOUTH];
  int end = start+GE_Directions::board_size;
  
  while(start<GE_Directions::limit_higher_location)
    {
      for(int j = start; j<end; j++)
	{
	  goban[j] = (fg.goban)[j];
	  chains[j] = (fg.chains)[j];
	  id_chains[j] = (fg.id_chains)[j];
	}
      
      start+=GE_Directions::directions[GE_SOUTH];
      end+=GE_Directions::directions[GE_SOUTH];
    }
  
  nb_choice = fg.nb_choice;
  score = fg.score;

  //cout<<nb_choice<<" "<<possible_moves.size()<<endl;
  //assert(nb_choice==(int)possible_moves.size());
}



//a tester: quand la première ligne puis première et deuxième ligne sont vides (semble marcher)

//the goban is supposed not empty and well-finished
int GE_FastGoban::evaluate_score()
{
  int end_score = 0;
  int pas = 1;
  
  int start = 1+GE_Directions::directions[GE_SOUTH];
  int end = start+GE_Directions::board_size;
  int temp;
  int incr = 1;
  int maj_location = (GE_Directions::big_board_size-2)*GE_Directions::big_board_size
    +(GE_Directions::big_board_size-1);
  
  int nb_locations = 0;
  
  int index = start;
  while(start<maj_location)  //we shouldn't go out of this loop because of this test 
    { //In reality, it should be a while(true)
      index = start;
      
      while(index!=end)
	{
	  assert(goban[index]!=GE_EDGE);
	  
	  if(goban[index]==GE_WITHOUT_STONE)
	    end_score++;
	  else break;
	  
	  index+=incr;
	  
	  nb_locations++;
	}
      
      if((goban[index]!=GE_WITHOUT_STONE)&&(index!=end))
	break;  //the really test of exit of this loop
      
      //start+=GE_Directions::directions[GE_SOUTH];
      incr = -incr;
      temp = start+GE_Directions::directions[GE_SOUTH]+incr;
      start = end+GE_Directions::directions[GE_SOUTH]+incr;
      end = temp;
    }
  
  if(goban[index]==GE_WHITE_STONE)
    {
      end_score = -end_score;
      pas = -1;
    }
  
  int next_color_stone = -goban[index];
  
  for(int j = index; j!=end; j+=incr)
    {
      if(goban[j]==next_color_stone)
	{
	  pas = -pas;
	  next_color_stone = -next_color_stone;
	}
      
      end_score+=pas;
      
      assert(goban[j]!=GE_EDGE);
      nb_locations++;
    }
  
  //start+=GE_Directions::directions[GE_SOUTH];
  //end+=GE_Directions::directions[GE_SOUTH];
  incr = -incr;
  temp = start+GE_Directions::directions[GE_SOUTH]+incr;
  start = end+GE_Directions::directions[GE_SOUTH]+incr;
  end = temp;

  
  
  while(start<maj_location)
    {
      
      for(int j = start ; j!=end; j+=incr)
	{
	  if(goban[j]==next_color_stone)
	    {
	      pas = -pas;
	      next_color_stone = -next_color_stone;
	    }
	  
	  end_score+=pas;
	  
	  assert(goban[j]!=GE_EDGE);
	  nb_locations++;
	}
      
      
      //std::cout<<"temp score: "<<end_score<<endl;
      
      incr = -incr;
      temp = start+GE_Directions::directions[GE_SOUTH]+incr;
      start = end+GE_Directions::directions[GE_SOUTH]+incr;
      end = temp;
    }
  
  //cout<<"nb locations: "<<nb_locations<<endl;
  //assert(nb_locations==GE_Directions::board_size*GE_Directions::board_size);

  score = end_score;

  return end_score;
}


int GE_FastGoban::launch_simulation(int max_capture)
{
  int nb_pass = 0;
  int num_loop = 1;

  int last_move = GE_PASS_MOVE;

  while(nb_pass!=2)
    {
      if(num_loop==stop)
	{
	  //cout<<"STOP"<<endl;
	  return 0;
	  //break;
	}
      
      //std::cerr<<num_loop<<std::endl;
      num_loop++;
      
      //print_goban(last_move);
      //print_chain();
      
      last_move = play(max_capture);
      
      if(last_move==GE_PASS_MOVE)
	nb_pass++;
      else nb_pass = 0;
    }

  
  //DEBUG-DEBUG-DEBUG-DEBUG-DEBUG-DEBUG-DEBUG-DEBUG-DEBUG
  //  {
  //     GE_Goban gob;
  //     toGoban(gob);
  //     gob.print();
  //     std::cout<<goban[1+GE_Directions::directions[GE_SOUTH]]<<" --> ";
  //     std::cout<<evaluate_score()<<" "<<(int)GE_EvaluationScore::evaluate_score2(gob, 0)<<endl;
  //     assert(evaluate_score()==(int)GE_EvaluationScore::evaluate_score2(gob, 0)); 
  //   }
  //DEBUG-DEBUG-DEBUG-DEBUG-DEBUG-DEBUG-DEBUG-DEBUG-DEBUG
  
  //return 0;
  return evaluate_score();
  
  //std::cout<<"end of the simulation"<<std::endl;
}


void GE_FastGoban::print_goban(int location) const 
{
  GE_Goban gob;
  toGoban(gob);
  
 
  if(location==GE_PASS_MOVE)
    return gob.print();
  
  int mv = GE_Directions::to_move(location);
  gob.print(mv);
}


void GE_FastGoban::print_chain() const
{
  std::cout<<"chains: "<<std::endl;

  vector<int> marker;
  int default_num = 10;
  marker.resize(GE_Directions::big_board_area, default_num);

  int num_chain = default_num+1;

  for(int i = 1; i<GE_Directions::big_board_size-1; i++)
    for(int j = 1; j<GE_Directions::big_board_size-1; j++)
      {
	int seed_chain = i*GE_Directions::big_board_size+j;
	int current_location = seed_chain;

	if(goban[seed_chain]==GE_WITHOUT_STONE) continue;
	if(chains[seed_chain]==-1) continue;
	if(marker[seed_chain]!=default_num) continue;

	do
	  { 
	    marker[current_location] = num_chain;
	    current_location = chains[current_location];
	  }
	while(current_location!=seed_chain);

	num_chain++;
      }
  
  for(int i = 1; i<GE_Directions::big_board_size-1; i++)
    {
      for(int j = 1; j<GE_Directions::big_board_size-1; j++)
	std::cout<<marker[i*GE_Directions::big_board_size+j]<<" ";
      
      std::cout<<std::endl;
    }

  //assert(0);
}


void GE_FastGoban::print_error_goban(int location) const 
{
  GE_Goban gob;
  toGoban(gob);
  
 
  if(location==GE_PASS_MOVE)
    return gob.print_error();
  
  int mv = GE_Directions::to_move(location);
  gob.print_error(mv);
}


bool GE_FastGoban::isAlone4(int location, int color_stone) const 
{
  for(int i = GE_NORTH; i<=GE_WEST; i++)
    {
      int neighbour = location+GE_Directions::directions[i];

      if(goban[neighbour]==color_stone) return false; 
    }
  
  return true;
}


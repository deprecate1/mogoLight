#include "amaf2.h"


GE_Amaf2::GE_Amaf2()
{
  values = 0;
  black_values = 0;
  white_values = 0;
  score = 0;
  nb_simulations = 0;
  sum_places = 0;
  size = 0;
  
  sequences = 0;
  black_sequences = 0;
  white_sequences = 0;
  //score_sequences = 0;
  //simulations_sequences = 0;
  places_sequences = 0;
}

GE_Amaf2::GE_Amaf2(int size)
{
  this->size = size;
  values = new int[size];
  black_values = new int[size];
  white_values = new int[size];  
  score = new int[size];
  nb_simulations = new int[size];

  for(int i = 0; i<size; i++)
    {
      values[i] = 0;
      black_values[i] = 0;
      white_values[i] = 0;
      score[i] = 0;
      nb_simulations[i] = 0;
    }
  
  sum_places = 0;

  GE_Allocator::init(&sequences, size);
  GE_Allocator::init(&black_sequences, size);
  GE_Allocator::init(&white_sequences, size);
  //GE_Allocator::init(&score_sequences, size);
  //GE_Allocator::init(&simulations_sequences, size);
  
  places_sequences = 0;
}


GE_Amaf2::~GE_Amaf2()
{
  if(values)
    {
      delete[] values;
      values = 0;
    }

  if(black_values)
    {
      delete[] black_values;
      black_values = 0;
    }

  if(white_values)
    {
      delete[] white_values;
      white_values = 0;
    } 

  if(score)
    {
      delete[] score;
      score = 0;
    }

  if(nb_simulations)
    {
      delete[] nb_simulations;
      nb_simulations = 0;
    }
  
  if(sum_places)
    {
      delete[] sum_places;
      sum_places= 0;
    }

  
  GE_Allocator::desallocate(&sequences, size);
  GE_Allocator::desallocate(&black_sequences, size);
  GE_Allocator::desallocate(&white_sequences, size);
  //GE_Allocator::desallocate(&score_sequences, size);
  //GE_Allocator::desallocate(&simulations_sequences, size);
  GE_Allocator::desallocate(&places_sequences, size);  
}


void GE_Amaf2::init_sum_places(const int height) 
{
  if(sum_places)
    {
      delete[] sum_places;
      sum_places = 0;
    }

  sum_places = new int[this->size];

  for(int i = 0; i<this->size; i++)
    sum_places[i] = 0;

  for(int i = 0; i<this->size; i++)
    {
      int black_place = this->print_place(black_values, height, i, GE_BLACK, false, false);
      int white_place = this->print_place(white_values, height, i, GE_WHITE, false, false);
      sum_places[i] = -(black_place+white_place); 
    }
}


void GE_Amaf2::clear(int** v)
{
  if(not (*v)) return;

  for(int i = 0; i<size; i++)
    (*v)[i] = 0;
}

void GE_Amaf2::clear(int*** s)
{
  if(not (*s)) return;

  for(int i = 0; i<size; i++)
    for(int j = 0; j<size; j++)
      (*s)[i][j] = 0;
}

void GE_Amaf2::clear()
{
  this->clear(&values);
  this->clear(&black_values);
  this->clear(&white_values);
  this->clear(&score);
  this->clear(&nb_simulations);
  this->clear(&sum_places);

  this->clear(&sequences);
  this->clear(&black_sequences);
  this->clear(&white_sequences);
  //this->clear(&score_sequences);
  //this->clear(&simulations_sequences);
  this->clear(&places_sequences);
}

//the case of zero could be a value no-updated (the value of initialization)
//so in some cases, we prefer ignore this value (cf GE_Simulation)
int GE_Amaf2::get_max(const int* v, bool not_zero) const
{
  if(not v) return 0;

  int temp_max = 0;

  if(not_zero)
    {
      for(int i = 0; i<size; i++)
	{
	  if(v[i]!=0)
	    if(v[i]>temp_max)
	      temp_max = v[i];
	}
      
      return temp_max;
    }

  temp_max = v[0];
  for(int i = 1; i<size; i++)
    {
      if(v[i]>temp_max)
	temp_max = v[i];
    }
  
  return temp_max;
}


//the case of zero could be a value no-updated (the value of initialization)
//so in some cases, we prefer ignore this value (cf GE_Simulation)
int GE_Amaf2::get_max(const int** s, bool not_zero) const
{
  if(not s) return 0;
  
  int temp_max = 0;
  
  if(not_zero)
    {
      for(int i = 0; i<size; i++)
	for(int j = 0; j<size; j++)
	  {
	    if(i==j) continue;
	    
	    if(s[i][j]!=0)
	      if(s[i][j]>temp_max)
		temp_max = s[i][j];
	  }
      
      return temp_max;
    }
  
  temp_max = s[0][0];
  for(int i = 0; i<size; i++)
    for(int j = 0; j<size; j++)
      {
	if(i==j) continue;
	
	if(s[i][j]>temp_max)
	  temp_max = s[i][j];
      }
  
  return temp_max;
}


int GE_Amaf2::print_place(const int* v, int height, int mv, 
		      team_color tc, bool endline, bool mode_print) const
{
  if(not v) return -1;
  
  int temp_tc = tc;
  if(v==black_values)
    temp_tc = GE_BLACK;
  else
    {
      if(v==white_values)
	temp_tc = GE_WHITE;
      else temp_tc = GE_NEUTRAL_COLOR;
    }
  
  
  if(mv==GE_PASS_MOVE)
    return -1;// false;
  
  list<int> val;
  
  for(int i = 0; i<size; i++)
    val.push_back(this->normalize(v, i));
  
  int nb_places = val.size();
  
  val.sort();
  val.reverse();
  
  int place = 0;
  
  while(not val.empty())
    {
      place++;
      
      if(this->normalize(v, mv)==val.front())
	{
	  if(mode_print)
	    {
	      if(temp_tc==GE_WHITE) 
		cout<<"W ";
	      else
		{
		  if(temp_tc==GE_BLACK)
		    cout<<"B ";
		  else 
		    {
		      if(v==score)
			cout<<"S ";
		      else 
			{
			  if(v==sum_places)
			    cout<<"+ ";
			  else cout<<"# ";
			}
		    }
		}
	      
	      cout<<move_to_string(height, mv);
	      cout<<" --> "<<place<<"/"<<nb_places;
	      cout<<" ("<<this->normalize(v, mv)<<" points)";
	      if(endline) cout<<endl;
	    }
	  
	  return place;
	  //return true;
	}
      
      val.pop_front();
    }
  
  if(endline) cout<<endl;
  
  return place;
}

void GE_Amaf2::print_place(int height, int mv) const
{
  this->print_place(values, height, mv);
  this->print_place(black_values, height, mv);
  this->print_place(white_values, height, mv);
  this->print_place(score, height, mv);
  this->print_place(sum_places, height, mv);
}



void GE_Amaf2::preferred_moves(int* v, int height, list<int>* interesting_moves, 
			   int nb, bool mode_print) const
{
  bool is_allocated = false;
  if(not interesting_moves)
    {
      interesting_moves = new list<int>;
      is_allocated = true;
    }

  list<int> val;
  
  for(int i = 0; i<size; i++)
    val.push_back(this->normalize(v, i));
  
  val.sort();
  val.reverse();
  
  if(mode_print)
    {
      if(v==values) cout<<"# ";
      if(v==black_values) cout<<"B ";
      if(v==white_values) cout<<"W ";
      if(v==score) cout<<"S ";
      
      cout<<val.front()<<" points  ";
    }
  
  while(not val.empty())
    {
      for(int i = 0; i<size; i++)
	{
	  if(this->normalize(v, i)==val.front())
	    {
	      val.pop_front();
	      (*interesting_moves).push_back(i);
	    }

	  if(val.empty()) break;

	  //if(v==score) cout<<this->normalize(v, i)<<" "<<val.front()<<endl;
	}
    }
  
  
  
  if(mode_print)
    {
      
      list<int>::const_iterator i_m = interesting_moves->begin();
      int nb_loop = 0;
      while((nb_loop<nb)&&(i_m!=interesting_moves->end()))
	{
	  cout<<move_to_string(height, *i_m)<<" ";
	  i_m++;
	  nb_loop++;
	}
      
      cout<<endl;
    }
  
  if(is_allocated)
    {
      delete interesting_moves;
      interesting_moves = 0;
    }
}


void GE_Amaf2::print_preferred_moves(int height, int nb) const
{
  this->preferred_moves(values, height, 0, nb, true);
  this->preferred_moves(black_values, height, 0, nb, true);
  this->preferred_moves(white_values, height, 0, nb, true);
  this->preferred_moves(score, height, 0, nb, true);
}

//we apply the strategy of minimax
//OPTIMIZATION --> alpha-beta
int GE_Amaf2::get_best_sequence(int** seq, list<int>* best_seq, GE_Game* game, 
			     bool not_zero, bool mode_print, int height) const
{
  int seq_value = GE_DEFAULT_BEST_MAX_VALUE; //the goal is a minimization
  
  bool is_allocated = false;;
  
  if(not best_seq)
    {
      best_seq = new list<int>;
      is_allocated = true;
    }
  
  if(not_zero)
    {
      for(int mv = 0; mv<size; mv++)
	{
	  if(game)
	    if(not game->is_allowed(mv))
	      continue;
	  
	  int best_answer = GE_PASS_MOVE;
	  int best_value =  GE_DEFAULT_BEST_MIN_VALUE; //the goal is a maximization
	  
	  for(int ans = 0; ans<size; ans++)
	    {
	      //cout<<seq[mv][ans]<<endl;
	      
	      if(seq[mv][ans]==0) continue; //because of initialization
      
	      if(seq[mv][ans]>best_value)
		{
		  if(game)
		    {
		      list<int> temp_seq;
		      temp_seq.push_back(mv);
		      temp_seq.push_back(ans);
		      if(not game->is_valid(temp_seq))
			continue;
		    }
		  

		  best_value = seq[mv][ans];
		  best_answer = ans;
		}
	    }
	  
	  //we minimize the answer of the adversary
	  if(best_value<seq_value)
	    {
	      if(best_value==GE_DEFAULT_BEST_MIN_VALUE) continue;

	      seq_value = best_value;
	      
	      best_seq->clear();
	      best_seq->push_back(mv);
	      best_seq->push_back(best_answer);
	    }
	}
    }
  else
    {
      for(int mv = 0; mv<size; mv++)
	{
	  if(game)
	    if(not game->is_allowed(mv))
	      continue;
	  
	  int best_answer = GE_PASS_MOVE;
	  int best_value =  GE_DEFAULT_BEST_MIN_VALUE;
	  
	  for(int ans = 0; ans<size; ans++)
	    {  
	      if(seq[mv][ans]==0) continue; //because of initialization
	      
	      if(seq[mv][ans]>best_value)
		{
		  if(game)
		    {
		      list<int> temp_seq;
		      temp_seq.push_back(mv);
		      temp_seq.push_back(ans);
		      if(not game->is_valid(temp_seq))
			continue;
		    }
		  
		  best_value = seq[mv][ans];
		  best_answer = ans;
		}
	    }
	  
	  //we minimize the answer of the adversary
	  if(best_value<seq_value)
	    {
	      if(best_value==GE_DEFAULT_BEST_MIN_VALUE) continue;
	      
	      seq_value = best_value;
	      
	      best_seq->clear();
	      best_seq->push_back(mv);
	      best_seq->push_back(best_answer);
	    }
	}
    }
  
  if(mode_print)
    {
      if(seq==sequences)
	cout<<"S# ";
      if(seq==black_sequences)
	cout<<"SB ";
      if(seq==white_sequences)
	cout<<"SW ";
      
      //we try to guess the value of height of the goban (if it is not given)
      int possible_height = (int)sqrt(size);
      if(possible_height*possible_height!=size)
	possible_height = height;

      cout<<"the best sequence: ";
      cout<<"("<<seq_value<<" points)   ";
      GE_ListOperators::print(*best_seq, move_to_string, possible_height);
   
    }
  
  if(is_allocated)
    {
      if(best_seq)
	delete best_seq;
      best_seq = 0;
    }

  return seq_value;
}

void GE_Amaf2::print_best_sequence(GE_Game* game, int height, bool not_zero) const
{
  this->get_best_sequence(sequences, 0, game, not_zero, true, height);
  this->get_best_sequence(black_sequences, 0, game, not_zero, true, height);
  this->get_best_sequence(white_sequences, 0, game, not_zero, true, height);
}


int GE_Amaf2::normalize(const int* v, int mv) const
{
  /*
  if(v==score)
    {
      if(nb_simulations[mv]==0)
	return 0; //AMAF_UNDEFINED;
      else return (int)(v[mv]/nb_simulations[mv]);
    }
  */
  return (int)v[mv];
}
  


void GE_Amaf2::update(int mv, list<int>& seq, int point, team_color tc, int size_max)
{
  if(mv==GE_PASS_MOVE) 
    {
      seq.clear();
      return;
    }

  list<int>::const_iterator i_seq = seq.begin();
  int divider = 1;
  int nb_loop = divider;

  while(i_seq!=seq.end())
    {
      bool is_pass_move = (*i_seq==GE_PASS_MOVE);
      //int val = (point/divider);
      int val = point;

      if(val==0) break;

      if(nb_loop%2==0)
	{
	  i_seq++;
	  nb_loop++;
	  continue;
	}
      
      if(is_pass_move)
	{
	  i_seq++;
	  divider++;
	  nb_loop++;
	  continue;
	}
      
      sequences[mv][*i_seq] += val;
      if(tc==GE_BLACK)
	{
	  if(nb_loop%2==1)
	    black_sequences[mv][*i_seq] += val;
	  else black_sequences[mv][*i_seq] -= val;
	}
      else 
	{
	  if(nb_loop%2==1)
	    white_sequences[mv][*i_seq] += val;
	  else white_sequences[mv][*i_seq] -= val;
	}
      
      i_seq++;
      divider++;
      nb_loop++;
    }
  
  if(seq.size()>0)
    if((int)seq.size()>size_max)
      seq.pop_back();
  
  seq.push_front(mv);
}

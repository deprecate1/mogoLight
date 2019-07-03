#include "amaf.h"


GE_Amaf::GE_Amaf()
{
  values = 0;
  black_values = 0;
  white_values = 0;
  score = 0;
  nb_simulations = 0;
  sum_places = 0;
  size = 0;
}

GE_Amaf::GE_Amaf(int size)
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
}


GE_Amaf::~GE_Amaf()
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
}


void GE_Amaf::init_sum_places(const int height) 
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


void GE_Amaf::clear(int** v)
{
  if(not (*v)) return;

  for(int i = 0; i<size; i++)
    (*v)[i] = 0;
}

void GE_Amaf::clear()
{
  this->clear(&values);
  this->clear(&black_values);
  this->clear(&white_values);
  this->clear(&score);
  this->clear(&nb_simulations);
  this->clear(&sum_places);
}

//the case of zero could be a value no-updated (the value of initialization)
//so in some cases, we prefer ignore this value (cf GE_Simulation)
int GE_Amaf::get_max(const int* v, bool not_zero) const
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


int GE_Amaf::print_place(const int* v, int height, int mv, 
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

void GE_Amaf::print_place(int height, int mv) const
{
  this->print_place(values, height, mv);
  this->print_place(black_values, height, mv);
  this->print_place(white_values, height, mv);
  this->print_place(score, height, mv);
  this->print_place(sum_places, height, mv);
}



void GE_Amaf::preferred_moves(int* v, int height, list<int>* interesting_moves, 
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


void GE_Amaf::print_preferred_moves(int height, int nb) const
{
  this->preferred_moves(values, height, 0, nb, true);
  this->preferred_moves(black_values, height, 0, nb, true);
  this->preferred_moves(white_values, height, 0, nb, true);
  this->preferred_moves(score, height, 0, nb, true);
}


int GE_Amaf::normalize(const int* v, int mv) const
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
  

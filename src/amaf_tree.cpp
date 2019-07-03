#include "amaf_tree.h"

GE_AmafTree::GE_AmafTree()
{
  am = new GE_Amaf(GE_DEFAULT_SIZE_GOBAN);
  first_place = 0;
  is_back = false;
  best_move = GE_PASS_MOVE;
  max_first_place = 0;
}

GE_AmafTree::GE_AmafTree(int size)
{
  am = new GE_Amaf(size);
  first_place = new int[size];
  
  for(int i = 0; i<size; i++)
    first_place[i] = 0;
  
  is_back = false;
  best_move = GE_PASS_MOVE;
  max_first_place = 0;
}

GE_AmafTree::~GE_AmafTree()
{
  if(am)
    {
      delete am;
      am = 0;
    }

  if(first_place)
    {
      delete[] first_place;
      first_place = 0;
    }
}


void GE_AmafTree::build_sequence(GE_Game& g, bool backward)
{
 
  new_sequence.clear();

  int* bv = am->values;
  int* wv = am->values;
  
  bv = am->black_values;
  wv = am->white_values;
  
  list<int> black_moves;
  list<int> white_moves;
  am->preferred_moves(bv, (g.goban)->height, &black_moves, 1, false);
  am->preferred_moves(wv, (g.goban)->height, &white_moves, 1, false);

  list<int>::const_iterator i_last_sequence = last_sequence.begin();
  while(true)
    {
      list<int>* l_moves;
      int mv = GE_PASS_MOVE;

      /*
	if((am->get_max(am->white_values, true)>am->get_max(am->black_values, true)))
	l_moves = &white_moves;
	else l_moves = &black_moves;
      */
      
      if(g.tc==GE_WHITE)
	l_moves = &white_moves;
      else l_moves = &black_moves;
     
      
      list<int>::const_iterator i_move = l_moves->begin();
      while(i_move!=l_moves->end())
	{
	  
	  if((g.is_allowed(*i_move))
	     &&(not g.is_stupid(*i_move)))
	    {
	      mv = *i_move;
	      break;
	    }
	  
	  i_move++;
	}
      
      if(mv==GE_PASS_MOVE)
	break;
      
      new_sequence.push_back(mv);
      g.play(mv);
      
      if((i_last_sequence==last_sequence.end())
	 ||(*i_last_sequence!=mv))
	break;
      
      i_last_sequence++;
    }
 
  last_sequence = new_sequence;
  
  if(not last_sequence.empty())
    if(last_sequence.front()!=GE_PASS_MOVE)
      {
	int& best_mv = last_sequence.front();

	first_place[best_mv]++;
	if(first_place[best_mv]>=max_first_place)
	  {
	    this->best_move = best_mv;
	    this->max_first_place = first_place[best_mv];
	  }
      }
  
  if(backward)
    {
      while(not new_sequence.empty())
	{
	  g.backward();
	  new_sequence.pop_back();
	}

      is_back = true;
    }
 
}

void GE_AmafTree::build_tree(GE_Game& g, bool mode_print, int max_loop)
{
  int nb_loop = 0;
  while(true)
    {
      nb_loop++;
      this->build_sequence(g, false);
      
      if(this->is_back)
	{
	  list<int>::const_iterator i_last_sequence = last_sequence.begin();
	  
	  
	  while(i_last_sequence!=last_sequence.end())
	    {
	      g.play(*i_last_sequence);
	      i_last_sequence++;
	    }
	  
	  this->is_back = false;
	}
      
      GE_Simulation::launch(g);
      GE_Simulation::update_amaf(g, *am, last_sequence.size());
      
      /*
      //made in  GE_Simulation::update_amaf(g, *am, last_sequence.size());
      list<int>::const_iterator i_last_sequence = last_sequence.begin();
      
      while(i_last_sequence!=last_sequence.end())
      {
      g.backward();
      i_last_sequence++;
      }
      */
      
      if(mode_print)
	{
	  if(nb_loop%1000==0)
	    {
	      cout<<"loop "<<nb_loop<<endl;
	      cout<<"current sequence: ";
	      GE_ListOperators::print(last_sequence, move_to_string, (g.goban)->height);
	      cout<<"best move: "<<move_to_string((g.goban)->height, best_move);
	      cout<<" ("<<max_first_place<<" points)"<<endl;
	      
	      cout<<endl<<endl;
	    }
	} 
      
      if(max_loop>0)
	if(nb_loop>max_loop)
	  break;
    }
}


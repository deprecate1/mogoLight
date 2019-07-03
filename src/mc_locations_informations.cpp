#include "mc_locations_informations.h"



GE_MCLocationsInformations::GE_MCLocationsInformations()
{
  size = 0;
  color_win = GE_BLACK;
  black_times = 0;
  black_wins = 0;
  white_times = 0;
  white_wins = 0;
  empty_times = 0;
  empty_wins = 0;
  nb_simulations = 0;
}

GE_MCLocationsInformations::GE_MCLocationsInformations(int size, int winner)
{
  this->size = size;
  color_win = winner;
  black_times = new int[size];
  black_wins = new int[size];
  white_times = new int[size];
  white_wins = new int[size];
  empty_times = new int[size];
  empty_wins = new int[size];
  nb_simulations = 0;
}


GE_MCLocationsInformations::~GE_MCLocationsInformations()
{
  if(black_times)
    {
      delete[] black_times;
      black_times = 0;
    }
  
  if(black_wins)
    {
      delete[] black_wins;
      black_wins = 0;
    }
  
  if(white_times)
    {
      delete[] white_times;
      white_times = 0;
    }
  
  if(white_wins)
    {
      delete[] white_wins;
      white_wins = 0;
    }

  if(empty_times)
    {
      delete[] empty_times;
      empty_times = 0;
    }
  
  if(empty_wins)
    {
      delete[] empty_wins;
      empty_wins = 0;
    }
}

void GE_MCLocationsInformations::update(const GE_Goban& gob, team_color winner)
{
  nb_simulations++;
  for(int i = 0; i<size; i++)
    {
      switch(gob.get_stone(i))
	{
	case GE_BLACK_STONE:
	  black_times[i]++;
	  if(winner==color_win) black_wins[i]++;
	  break;
	  
	case GE_WHITE_STONE:
	  white_times[i]++;
	  if(winner==color_win) white_wins[i]++;
	  break;
	  
	case GE_WITHOUT_STONE :
	  empty_times[i]++;
	  if(winner==color_win) empty_wins[i]++;
	  break;
	  
	default:
	  assert(0);
	}
    }
}


double GE_MCLocationsInformations::criterion_bw_on_bt(int location)
{
  if(color_win==GE_BLACK)
    {
      if(black_times[location]==0) return 0.f;
      return black_wins[location]/(double)black_times[location];
    }
  else
    {
      if(white_times[location]==0) return 0.f;
      return white_wins[location]/(double)white_times[location];
    }
  
  return 0.f;
}

double GE_MCLocationsInformations::criterion_bw_on_sim(int location)
{
  if(color_win==GE_BLACK)
    return black_wins[location]/(double)nb_simulations;
  else return white_wins[location]/(double)nb_simulations;
}
  

double GE_MCLocationsInformations::criterion_bt_on_sim(int location)
{
  if(color_win==GE_BLACK)
    return black_times[location]/(double)nb_simulations;
  else return white_times[location]/(double)nb_simulations;
}

double GE_MCLocationsInformations::criterion_ww_on_wt(int location)
{
  if(color_win==GE_WHITE)
    {
      if(black_times[location]==0) return 1.f;
      return 1-(black_wins[location]/(double)black_times[location]);
    }
  else
    {
      if(white_times[location]==0) return 1.f;
      return 1-(white_wins[location]/(double)white_times[location]);
    }
  
  return 0.f;
}

double GE_MCLocationsInformations::criterion_ww_on_sim(int location)
{
  if(color_win==GE_WHITE)
    return 1-(black_wins[location]/(double)nb_simulations);
  else return 1-(white_wins[location]/(double)nb_simulations);
}

double GE_MCLocationsInformations::criterion_wt_on_sim(int location)
{
  if(color_win==GE_WHITE)
    return 1-(black_times[location]/(double)nb_simulations);
  else return 1-(white_times[location]/(double)nb_simulations);
}

double GE_MCLocationsInformations::criterion_ew_on_et(int location)
{
  if(empty_times[location]==0) return 1.f;
  return 1-(empty_wins[location]/(double)empty_times[location]);
}

double GE_MCLocationsInformations::criterion_ew_on_sim(int location)
{
  return 1-(empty_wins[location]/(double)nb_simulations);
}

double GE_MCLocationsInformations::criterion_et_on_sim(int location)
{
  return 1-(empty_times[location]/(double)nb_simulations);
}


double GE_MCLocationsInformations::criterion(int location, int num_criterion)
{
  switch(num_criterion)
    {
    case 0 : return criterion_bw_on_bt(location);
    case 1 : return criterion_ww_on_wt(location);
    case 2 : return criterion_et_on_sim(location);
      
      /*
    case 0 : return criterion_bw_on_bt(location);
    case 1 : return criterion_bw_on_sim(location);
    case 2 : return criterion_bt_on_sim(location);
    case 3 : return criterion_ww_on_wt(location);
    case 4 : return criterion_ww_on_sim(location);
    case 5 : return criterion_wt_on_sim(location);
    case 6 : return criterion_ew_on_et(location);
    case 7 : return criterion_ew_on_sim(location);
    case 8 : return criterion_et_on_sim(location);
    case 9 : return criterion_bt_on_sim(location)*(1-criterion_bt_on_sim(location));
    case 10 : return criterion_wt_on_sim(location)*(1-criterion_wt_on_sim(location));
      */
    default : assert(0);
    }
  
  return 0.f;
}

int GE_MCLocationsInformations::pour_cent_mille(double val)
{
  return (int)(100000.f*val);
}


int GE_MCLocationsInformations::get_place(int location, int num_criterion, 
				       int& nb_places, GE_Game* g)
{
  nb_places = 0;
  list<int> val;
  
  for(int i = 0; i<size; i++)
    {
      if((not g)||(g->is_allowed(i)))
	  val.push_back(this->pour_cent_mille(criterion(i, num_criterion)));
    }

  val.sort();
  val.reverse();
  
  int eval_location = this->pour_cent_mille(criterion(location, num_criterion));
  //cout<<"titi "<<eval_location<<endl;

  int place = 0;
  
  while(not val.empty())
    {
      //cout<<"tutu "<<val.front()<<endl;
      place++;
      
      if(eval_location==val.front())
	{
	  while(not val.empty())
	    {
	      if(eval_location==val.front()) 
		nb_places++;
	      else break;
	      val.pop_front();
	    }

	  return place;
	}
      val.pop_front();
    }

  assert(0);
  
  return -1;
}




void GE_MCLocationsInformations::print_place_criteria(int location, int max_criteria, GE_Game* g, 
						   int height)
{
  int h;
  if(g)
    h = (g->goban)->height;
  else h = height;

  cout<<move_to_string(h, location)<<"  ";
  
  for(int i = 0; i<max_criteria; i++)
    {
      int nb_places = 0;
      cout<<get_place(location, i, nb_places, g)<<" ("<<nb_places<<")";
      if(i<max_criteria-1) cout<<" - ";
    }
  
  cout<<endl;
}

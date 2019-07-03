#include "fast_mc_locations_informations.h"

GE_FastMCLocationsInformations::GE_FastMCLocationsInformations(int winner)
{
  order.resize(GE_Directions::big_board_area);
  for(int i = 0; i<(int)order.size(); i++)
    order[i].resize(GE_Directions::big_board_area, 0);
  
  order_before_enemy.resize(GE_Directions::big_board_area);
  for(int i = 0; i<(int)order_before_enemy.size(); i++)
    order_before_enemy[i].resize(GE_Directions::big_board_area, 0);
  
  black_times.resize(GE_Directions::big_board_area, 0);
  black_wins.resize(GE_Directions::big_board_area, 0);
  
  white_times.resize(GE_Directions::big_board_area, 0);
  white_wins.resize(GE_Directions::big_board_area, 0);
  
  empty_times.resize(GE_Directions::big_board_area, 0);
  empty_wins.resize(GE_Directions::big_board_area, 0);
  
  black_frontier_times.resize(GE_Directions::big_board_area, 0);
  black_frontier_wins.resize(GE_Directions::big_board_area, 0);
  
  white_frontier_times.resize(GE_Directions::big_board_area, 0);
  white_frontier_wins.resize(GE_Directions::big_board_area, 0);
  
  values.resize(GE_Directions::big_board_area, 0);
  black_values.resize(GE_Directions::big_board_area, 0);
  white_values.resize(GE_Directions::big_board_area, 0);
  score.resize(GE_Directions::big_board_area, 0);
  nb_begins.resize(GE_Directions::big_board_area, 0);
  
  color_win = winner;
  nb_simulations = 0;
  nb_wins = 0;
}


//NOT TESTED
void GE_FastMCLocationsInformations::clear(int winner)
{
  if(order.size()>0)
    {
      for(int i = 0; i<GE_Directions::big_board_area; i++)
	for(int j = 0; j<GE_Directions::big_board_area; j++)
	  order[i][j] = 0;
    }
    
  if(order_before_enemy.size()>0)
    { 
      for(int i = 0; i<GE_Directions::big_board_area; i++)
	for(int j = 0; j<GE_Directions::big_board_area; j++)
	  order_before_enemy[i][j] = 0;
    }
 
  if(black_times.size()>0)
    for(int i = 0; i<GE_Directions::big_board_area; i++)
      black_times[i] = 0;

  if(black_wins.size()>0)
    for(int i = 0; i<GE_Directions::big_board_area; i++)
      black_wins[i] = 0;
  
  if(white_times.size()>0)
    for(int i = 0; i<GE_Directions::big_board_area; i++)
      white_times[i] = 0;

  if(white_wins.size()>0)
    for(int i = 0; i<GE_Directions::big_board_area; i++)
      white_wins[i] = 0;

    
  if(empty_times.size()>0)
    for(int i = 0; i<GE_Directions::big_board_area; i++)
      empty_times[i] = 0;

  if(empty_wins.size()>0)
    for(int i = 0; i<GE_Directions::big_board_area; i++)
      empty_wins[i] = 0;
  
  if(black_frontier_times.size()>0)
    for(int i = 0; i<GE_Directions::big_board_area; i++)
      black_frontier_times[i] = 0;
  
  if(black_frontier_wins.size()>0)
    for(int i = 0; i<GE_Directions::big_board_area; i++)
      black_frontier_wins[i] = 0;

  if(white_frontier_times.size()>0)
    for(int i = 0; i<GE_Directions::big_board_area; i++)
      white_frontier_times[i] = 0;
  
  if(white_frontier_wins.size()>0)
    for(int i = 0; i<GE_Directions::big_board_area; i++)
      white_frontier_wins[i] = 0;

  if(values.size()>0)
    for(int i = 0; i<GE_Directions::big_board_area; i++)
      values[i] = 0;

  if(black_values.size()>0)
    for(int i = 0; i<GE_Directions::big_board_area; i++)
      black_values[i] = 0;
  
  if(white_values.size()>0)
    for(int i = 0; i<GE_Directions::big_board_area; i++)
      white_values[i] = 0;
 
  if(score.size()>0)
    for(int i = 0; i<GE_Directions::big_board_area; i++)
      score[i] = 0;

  if(nb_begins.size()>0)
    for(int i = 0; i<GE_Directions::big_board_area; i++)
      nb_begins[i] = 0;

  color_win = winner;
  nb_simulations = 0;
  nb_wins = 0;
}



void GE_FastMCLocationsInformations::update(const GE_FastGoban& fg, float komi)
{
  int winner = GE_BLACK_STONE;
  
  if(fg.score-komi>0)
    winner = GE_BLACK_STONE;
  else winner = GE_WHITE_STONE;

  nb_simulations++;
  
  int start = 1+GE_Directions::directions[GE_SOUTH];
  int end = start+GE_Directions::board_size;
  
  if(winner==color_win)
    {
      nb_wins++;

      while(start<GE_Directions::limit_higher_location)
	{
	  for(int j = start ; j<end; j++)
	    {
	      switch(fg.goban[j])
		{
		case GE_BLACK_STONE:
		  black_times[j]++;
		  black_wins[j]++;
		  
		  if(not fg.isAlone4(j, GE_WHITE_STONE))
		    {
		      black_frontier_times[j]++;
		      black_frontier_wins[j]++;
		    }
		  break;
		  
		case GE_WHITE_STONE:
		  white_times[j]++;
		  white_wins[j]++;
		  
		  if(not fg.isAlone4(j, GE_BLACK_STONE))
		    {
		      white_frontier_times[j]++;
		      white_frontier_wins[j]++;
		    }
		  break;
		  
		case GE_WITHOUT_STONE :
		  empty_times[j]++;
		  empty_wins[j]++;
		  break;
		  
		default:
		  assert(0);
		}
	      
	      
	      assert(fg.goban[j]!=GE_EDGE);
	    }
	  
          start+=GE_Directions::directions[GE_SOUTH];
	  end+=GE_Directions::directions[GE_SOUTH];
	}
    }
  else
    {
      while(start<GE_Directions::limit_higher_location)
	{
	  for(int j = start ; j<end; j++)
	    {
	      switch(fg.goban[j])
		{
		case GE_BLACK_STONE:
		  black_times[j]++;
		  
		  if(not fg.isAlone4(j, GE_WHITE_STONE))
		    black_frontier_times[j]++;
		  
		  break;
		  
		case GE_WHITE_STONE:
		  white_times[j]++;
		  
		  if(not fg.isAlone4(j, GE_BLACK_STONE))
		    white_frontier_times[j]++;
		  
		  break;
		  
		case GE_WITHOUT_STONE :
		  empty_times[j]++;
		  
		  break;
	      
		default:
		  assert(0);
		}
	  
	  
	      assert(fg.goban[j]!=GE_EDGE);
	    }
      
	  start+=GE_Directions::directions[GE_SOUTH];
	  end+=GE_Directions::directions[GE_SOUTH];
	}
  
    }
  
  int temp_turn = fg.turn;
  if(fg.game.size()%2==0) //don't forget the illegal move is the first move 
    {
      if(fg.turn==GE_BLACK_STONE)
	temp_turn = GE_WHITE_STONE;
      else temp_turn = GE_BLACK_STONE;
    }
  else 
    {
      if(fg.turn==GE_BLACK_STONE)
	temp_turn = GE_BLACK_STONE;
      else temp_turn = GE_WHITE_STONE;
    }
  
  int point = fg.game.size();
  if(winner!=color_win)
    point = -point;
  
  //#####################################################################
  if(fg.game.size()>1)
    {
      int temp_mv = fg.game[1];
      if((temp_mv!=GE_PASS_MOVE)&&(temp_mv!=GE_RESIGN))
	score[temp_mv]+=(int)fg.score;
    }
  //#####################################################################


  for(int i = 1;  i<(int)fg.game.size(); i++)
    {
      int mv = fg.game[i];
      
      if((mv==GE_PASS_MOVE)||(mv==GE_RESIGN))
	{
	  temp_turn = -temp_turn;
	  
	  if(point>0)
	    point = -(point - 1);
	  else point = -(point+1);
	  
	  continue;
	}
      
      values[mv]+=point;

      if(temp_turn==GE_BLACK_STONE)
	black_values[mv]+=point;
      else white_values[mv]+=point;
      
      if(point>0)
	point = -(point - 1);
      else point = -(point+1);
      
      temp_turn = -temp_turn;
    }
  
  //   while(start<GE_Directions::limit_higher_location)
  //     {
  //       for(int j = start ; j<end; j++)
  // 	{
  // 	  switch(fg.goban[j])
  // 	    {
  // 	    case GE_BLACK_STONE:
  // 	      black_times[j]++;
  // 	      if(winner==color_win) black_wins[j]++;
  
  // 	      if(not fg.isAlone4(j, GE_WHITE_STONE))
  // 		{
  // 		  black_frontier_times[j]++;
  // 		  if(winner==color_win) black_frontier_wins[j]++;
  // 		}
  // 	      break;
  
  // 	    case GE_WHITE_STONE:
  // 	      white_times[j]++;
  // 	      if(winner==color_win) white_wins[j]++;
	      
  // 	      if(not fg.isAlone4(j, GE_BLACK_STONE))
  // 		{
  // 		  white_frontier_times[j]++;
  // 		  if(winner==color_win) white_frontier_wins[j]++;
  // 		}
  // 	      break;
  
  // 	    case GE_WITHOUT_STONE :
  // 	      empty_times[j]++;
  // 	      if(winner==color_win) empty_wins[j]++;
  // 	      break;
  
  // 	    default:
  // 	      assert(0);
  // 	    }
  
  
  // 	  assert(fg.goban[j]!=GE_EDGE);
  // 	}
  
  //       start+=GE_Directions::directions[GE_SOUTH];
  //       end+=GE_Directions::directions[GE_SOUTH];
  //     }
  
}



double GE_FastMCLocationsInformations::criterion_bw_on_bt(int location) const
{
  if(color_win==GE_BLACK_STONE)
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


double GE_FastMCLocationsInformations::criterion_bw_on_sim(int location) const
{
  if(color_win==GE_BLACK_STONE)
    return black_wins[location]/(double)nb_simulations;
  else return white_wins[location]/(double)nb_simulations;
}
  

double GE_FastMCLocationsInformations::criterion_bt_on_sim(int location) const
{
  if(color_win==GE_BLACK_STONE)
    return black_times[location]/(double)nb_simulations;
  else return white_times[location]/(double)nb_simulations;
}


double GE_FastMCLocationsInformations::criterion_ww_on_wt(int location) const
{
  if(color_win==GE_WHITE_STONE)
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

double GE_FastMCLocationsInformations::criterion_ww_on_sim(int location) const
{
  if(color_win==GE_WHITE_STONE)
    return 1-(black_wins[location]/(double)nb_simulations);
  else return 1-(white_wins[location]/(double)nb_simulations);
}

double GE_FastMCLocationsInformations::criterion_wt_on_sim(int location) const
{
  if(color_win==GE_WHITE_STONE)
    return 1-(black_times[location]/(double)nb_simulations);
  else return 1-(white_times[location]/(double)nb_simulations);
}

double GE_FastMCLocationsInformations::criterion_ew_on_et(int location) const
{
  if(empty_times[location]==0) return 1.f;
  return 1-(empty_wins[location]/(double)empty_times[location]);
}

double GE_FastMCLocationsInformations::criterion_ew_on_sim(int location) const
{
  return 1-(empty_wins[location]/(double)nb_simulations);
}

double GE_FastMCLocationsInformations::criterion_et_on_sim(int location) const
{
  return 1-(empty_times[location]/(double)nb_simulations);
}


double GE_FastMCLocationsInformations::criterion_bfw_on_bft(int location) const
{
  if(color_win==GE_BLACK_STONE)
    {
      if(black_frontier_times[location]==0) return 0.f;
      return black_frontier_wins[location]/(double)black_frontier_times[location];
    }
  else
    {
      if(white_frontier_times[location]==0) return 0.f;
      return white_frontier_wins[location]/(double)white_frontier_times[location];
    }
  
  return 0.f;
}


double GE_FastMCLocationsInformations::criterion_bfw_on_sim(int location) const
{
  if(color_win==GE_BLACK_STONE)
    return black_frontier_wins[location]/(double)nb_simulations;
  else return white_frontier_wins[location]/(double)nb_simulations;
}
  

double GE_FastMCLocationsInformations::criterion_bft_on_sim(int location) const
{
  if(color_win==GE_BLACK_STONE)
    return black_frontier_times[location]/(double)nb_simulations;
  else return white_frontier_times[location]/(double)nb_simulations;
}


double GE_FastMCLocationsInformations::criterion_wfw_on_wft(int location) const
{
  if(color_win==GE_WHITE_STONE)
    {
      if(black_frontier_times[location]==0) return 1.f;
      return 1-(black_frontier_wins[location]/(double)black_frontier_times[location]);
    }
  else
    {
      if(white_frontier_times[location]==0) return 1.f;
      return 1-(white_frontier_wins[location]/(double)white_frontier_times[location]);
    }
  
  return 0.f;
}

double GE_FastMCLocationsInformations::criterion_wfw_on_sim(int location) const
{
  if(color_win==GE_WHITE_STONE)
    return 1-(black_frontier_wins[location]/(double)nb_simulations);
  else return 1-(white_frontier_wins[location]/(double)nb_simulations);
}

double GE_FastMCLocationsInformations::criterion_wft_on_sim(int location) const
{
  if(color_win==GE_WHITE_STONE)
    return 1-(black_frontier_times[location]/(double)nb_simulations);
  else return 1-(white_frontier_times[location]/(double)nb_simulations);
}

double GE_FastMCLocationsInformations::criterion_black_values(int location) const
{
  if(color_win==GE_BLACK_STONE)
    return black_values[location];
  else return white_values[location];
}

double GE_FastMCLocationsInformations::criterion_white_values(int location) const
{
  if(color_win==GE_WHITE_STONE)
    return black_values[location];
  else return white_values[location];
}

double GE_FastMCLocationsInformations::criterion_score(int location) const
{
  if(color_win==GE_WHITE_STONE)
    return -score[location];
  else return score[location];
}

double GE_FastMCLocationsInformations::criterion_mean_points(int location) const
{
  double mean = 0;

  if(color_win==GE_BLACK_STONE)
    {
      mean = mean_of_neighbours(black_values, location);
      return black_values[location] - mean;
    }
  else 
    {
      mean = mean_of_neighbours(white_values, location);
      return white_values[location] - mean;
    }
  
  assert(0);

  return 0;
}



double GE_FastMCLocationsInformations::criterion(int location, int num_criterion) const
{
  switch(num_criterion)
    {
      /*
	case 0 : return criterion_bw_on_bt(location);
	case 1 : return criterion_ww_on_wt(location);
	case 2 : return criterion_et_on_sim(location);
      */ 
      /*    
    case 0 : return (values[location]/100000.f);
    case 1 : return (criterion_black_values(location)/100000.f);
    case 2 : return (criterion_white_values(location)/100000.f);
*/
     
    case 0 : return criterion_bw_on_bt(location);
    case 1 : return criterion_bw_on_sim(location);
    case 2 : return criterion_bt_on_sim(location);  
    case 3 : return criterion_ww_on_wt(location);
    case 4 : return criterion_ww_on_sim(location);
    case 5 : return 1-criterion_wt_on_sim(location);
    case 6 : return 1-criterion_ew_on_et(location);
    case 7 : return criterion_ew_on_sim(location);
    case 8 : return criterion_et_on_sim(location);
    case 9 : return criterion_bfw_on_bft(location);
    case 10 : return criterion_bfw_on_sim(location);
    case 11 : return criterion_bft_on_sim(location);  
    case 12 : return criterion_wfw_on_wft(location);
    case 13 : return 1-criterion_wfw_on_sim(location);
    case 14 : return 1-criterion_wft_on_sim(location);
      
    case 15 : return criterion_bt_on_sim(location)*(1-criterion_bt_on_sim(location));
    case 16 : return criterion_wt_on_sim(location)*(1-criterion_wt_on_sim(location));
      
    case 17 : return (values[location]/100000.f);
    case 18 : return (criterion_black_values(location)/100000.f);
    case 19 : return (criterion_white_values(location)/100000.f);
    case 20 : return (criterion_score(location)/100000.f);
    case 21 : return (criterion_mean_points(location)/100000.f);
      
    default : assert(0);
    }
  
  return 0.f;
}

int GE_FastMCLocationsInformations::pour_cent_mille(double val) const
{
  return (int)(100000.f*val);
}


int GE_FastMCLocationsInformations::get_place(int location, int num_criterion, 
					   int& nb_places, GE_Game* g) const 
{
  nb_places = 0;
  list<int> val;
  
  int start = 1+GE_Directions::directions[GE_SOUTH];
  int end = start+GE_Directions::board_size;
  
  while(start<GE_Directions::limit_higher_location)
    {
      for(int j = start ; j<end; j++)
	{
	  int mv = GE_Directions::to_move(j);
	  
	  if((not g)||(g->is_allowed(mv)))
	    val.push_back(this->pour_cent_mille(criterion(j, num_criterion)));
	}
      
      start+=GE_Directions::directions[GE_SOUTH];
      end+=GE_Directions::directions[GE_SOUTH];
    }
  
  val.sort();
  val.reverse();
  
  int eval_location = this->pour_cent_mille(criterion(location, num_criterion));
  int place = 0;
  
  while(not val.empty())
    {
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
	
	  //cout<<eval_location<<endl;
	  return place;
	}
      val.pop_front();
    }

  assert(0);
  
  return -1;
}




void GE_FastMCLocationsInformations::print_places(int criteria, GE_Game* g, 
					       int height, int big_location)
{
  int h;
  if(g)
    h = (g->goban)->height;
  else h = height;
  
  int place = 0;

  int last_move = GE_ILLEGAL_MOVE;
  if(g) 
    {
      last_move = g->get_last_move();
      if(last_move!=GE_PASS_MOVE)
	last_move = GE_Directions::to_big_move(last_move);
    }

  if(big_location!=GE_ILLEGAL_MOVE) last_move = big_location;

  
  int start = 1+GE_Directions::directions[GE_SOUTH];
  int end = start+GE_Directions::board_size;
    
  while(start<GE_Directions::limit_higher_location)
    {
      
      for(int j = start ; j<end; j++)
	{
	  int mv = GE_Directions::to_move(j);
	  
	  if(last_move==j)
	    cout<<"(";
	  else
	    if(j==start) cout<<" ";
	  
	  if((g)&&(not g->is_allowed(mv)))
	    {
	      switch((g->goban)->get_stone(mv))
		{
		case GE_BLACK_STONE :
		  cout<<"BB";
		  if(h*h>=100) cout<<"B"; 
		  break;
		  
		case GE_WHITE_STONE :
		  cout<<"WW";
		  if(h*h>=100) cout<<"W"; 
		  break;
		  
		case GE_WITHOUT_STONE :
		  cout<<"XX";
		  if(h*h>=100) cout<<"X"; 
		}
	      
	      if(last_move==j)
		cout<<")";
	      else if(last_move!=j+1)
		cout<<" ";
	      
	      continue;
	    }
	  int nb_places;
	  place = get_place(j, criteria, nb_places, g);
	  
	  if(place<10)
	    cout<<" ";
	  else
	    if((h*h>=100)&&(place<100))
	      cout<<" ";
	  
	  cout<<place;
	  
	  if((h*h>=100)&&(place<10))
	    cout<<" ";
	  

	  if(last_move==j)
	    cout<<")";
	  else if(last_move!=j+1)
	    cout<<" ";
	}
      
      cout<<endl;
      
      start+=GE_Directions::directions[GE_SOUTH];
      end+=GE_Directions::directions[GE_SOUTH];
    }

  cout<<endl;
}






void GE_FastMCLocationsInformations::print_place_criteria(int location, int max_criteria, GE_Game* g, 
						       int height)
{
  int h;
  if(g)
    h = (g->goban)->height;
  else h = height;
  
  cout<<move_to_string(h, location)<<"  ";
  
  int big_location = GE_Directions::to_big_move(location); 
  
  for(int i = 0; i<max_criteria; i++)
    {
      int nb_places = 0;
      cout<<get_place(big_location, i, nb_places, g)/*<<" ("<<nb_places<<")"*/;
      if(i<max_criteria-1) cout<<" - ";
    }
  
  cout<<endl;
}

void GE_FastMCLocationsInformations::get_best_moves(list<int>& l_moves, int num_criterion, GE_Game* g, 
						 const GE_RefereeGo* rg, bool anti_stupid) const
{
  
  if((rg)&&(not g)) assert(0);
  
  int tc = GE_BLACK;
  if(g) tc = g->tc;
  
  int start = 1+GE_Directions::directions[GE_SOUTH];
  int end = start+GE_Directions::board_size;
  
  int min_place = GE_Directions::big_board_size*GE_Directions::big_board_size;
  
  while(start<GE_Directions::limit_higher_location)
    {
      for(int j = start ; j<end; j++)
	{ 
	  int nb_places = 0;
	  
	  int location = GE_Directions::to_move(j);

	  if(((not rg)||(rg->is_allowed(*(g->goban), location, tc)))
	     &&((not anti_stupid)||(not g->is_stupid(location))))
	    {
	      int place = this->get_place(j, num_criterion, nb_places, g);
	      
	      if(place==min_place)
		{ 
		  l_moves.push_back(location);
		  continue;
		}
	      
	      if(place<min_place)
		{ 
		  min_place = place;
		  l_moves.clear();
		  l_moves.push_back(location);
		  continue;
		}
	    }
	}
      
      start+=GE_Directions::directions[GE_SOUTH];
      end+=GE_Directions::directions[GE_SOUTH];
    }
  
}


void GE_FastMCLocationsInformations::update_stats_semeai
(const GE_FastGoban& fg, vector<vector<int> >& v_semeai)
{
  for(int i = 0; i<(int)v_semeai.size(); i++)
    {
      int big_location1 = GE_Bibliotheque::to_big_goban[i];
      for(int j = i+1; j<(int)v_semeai[i].size(); j++)
	{
	  int big_location2 = GE_Bibliotheque::to_big_goban[j];
	  
	  if(((fg.goban)[big_location1]==GE_BLACK_STONE)
	     &&((fg.goban)[big_location2]!=GE_WHITE_STONE))
	    {
	      v_semeai[i][j]++;
	      continue;
	    }

	  if(((fg.goban)[big_location2]==GE_BLACK_STONE)
	     &&((fg.goban)[big_location1]!=GE_WHITE_STONE))
	    {
	      v_semeai[i][j]++;
	      continue;
	    }
	  
	  if(((fg.goban)[big_location1]==GE_WHITE_STONE)
	     &&((fg.goban)[big_location2]!=GE_BLACK_STONE))
	    {
	      v_semeai[j][i]++;
	      continue;
	    }

	  if(((fg.goban)[big_location2]==GE_WHITE_STONE)
	     &&((fg.goban)[big_location1]!=GE_BLACK_STONE))
	    {
	      v_semeai[j][i]++;
	      continue;
	    }
	}
    }
}

void GE_FastMCLocationsInformations::update_stats_semeai
(const GE_FastGoban& fg, const vector<int>& big_black_stones, 
 const vector<int>& big_white_stones, vector<vector<int> >& v_semeai)
{
  for(int i_b = 0; i_b<(int)big_black_stones.size(); i_b++)
    {
      int big_location_B = big_black_stones[i_b];
      int location_B = GE_Bibliotheque::to_goban[big_location_B];
      
      if(fg.goban[big_location_B]==GE_BLACK_STONE)
	{
	  for(int i_w = 0; i_w<(int)big_white_stones.size(); i_w++)
	    {
	      int big_location_W = big_white_stones[i_w];
	      int location_W = GE_Bibliotheque::to_goban[big_location_W];
	      
	      
	      if(fg.goban[big_location_W]!=GE_WHITE_STONE)
		v_semeai[location_B][location_W]++;
	    }
	}
      else
	{
	  for(int i_w = 0; i_w<(int)big_white_stones.size(); i_w++)
	    {
	      int big_location_W = big_white_stones[i_w];
	      int location_W = GE_Bibliotheque::to_goban[big_location_W];
	      
	      
	      if(fg.goban[big_location_W]==GE_WHITE_STONE)
		v_semeai[location_W][location_B]++;
	    }
	}
    }
}



void GE_FastMCLocationsInformations::update_stats_group
(const GE_FastGoban& fg, const GE_FastGoban& gob_after_simu, 
 vector<int>& v_group)
{
  for(int i = 0; i<(int)v_group.size(); i++)
    {
      int big_location = GE_Bibliotheque::to_big_goban[i];
      if(fg.goban[big_location]==gob_after_simu.goban[big_location])
	v_group[i]++;
    }
}





//fast mc uses big locations
void GE_FastMCLocationsInformations::print_stats(int big_location) const
{
  if(big_location==GE_ILLEGAL_MOVE)
    {
      if(color_win==GE_BLACK_STONE)
	std::cerr<<"For Black, "<<std::endl;
      else std::cerr<<"For White, "<<std::endl;
      
      std::cerr<<nb_simulations<<" - "<<nb_wins<<endl;
      for(int i = 0; i<GE_Directions::board_area; i++)
	print_stats(GE_Bibliotheque::to_big_goban[i]);
      cerr<<endl;

      return;
    }
  
  cerr<<move_to_string(GE_Directions::board_size, GE_Bibliotheque::to_goban[big_location])<<": ";
  if(color_win==GE_BLACK_STONE)
    {
      cerr<<black_values[big_location]<<" | ";
      cerr<<black_wins[big_location]<<" - "<<black_times[big_location]<<" | ";
      cerr<<white_wins[big_location]<<" - "<<white_times[big_location]<<" | ";
      cerr<<empty_wins[big_location]<<" - "<<empty_times[big_location]<<endl;
    }
  else
    {
      cerr<<white_values[big_location]<<" | ";
      cerr<<white_wins[big_location]<<" - "<<white_times[big_location]<<" | ";
      cerr<<black_wins[big_location]<<" - "<<black_times[big_location]<<" | ";
      cerr<<empty_wins[big_location]<<" - "<<empty_times[big_location]<<endl;
    }
}

void GE_FastMCLocationsInformations::print_one_stats(int big_location, int num_move) const
{
  std::cerr<<"("<<nb_wins<<"/"<<nb_simulations<<") ";
  
  if(num_move>0) std::cerr<<num_move<<". ";
  
  if(color_win==GE_BLACK_STONE)
    std::cerr<<"B ";
  else std::cerr<<"W ";

  print_stats(big_location);
}


/*
double GE_FastMCLocationsInformations::sum_of_neighbours(vector<int>& tab, 
						      int big_location)
{
  
}
*/

double GE_FastMCLocationsInformations::mean_of_neighbours(const vector<int>& tab, 
						       int big_location) const
{
  double mean = 0;
  int nb_valid_neighbours = 0;
  
  for(int i = GE_NORTH; i<GE_SOUTHWEST; i++)
    {
      int big_neighbour = big_location+GE_Directions::directions[i];
      if(tab[big_neighbour]==0) continue;

      mean+=tab[big_neighbour];
      nb_valid_neighbours++;
    }
  
  if(mean==0) return 0;
  
  return mean/(double)(nb_valid_neighbours);
}

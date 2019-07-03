#include "algorithm.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


void GE_Algorithm::test(int size_goban, bool mode_print, bool print_res, 
		     bool print_err)
{ 
  int success = 0;
  int failure = 0;
  int nb_checked = 0;
  
  string name;
  
  GE_DbGamesGo dgg; 
  list<GE_FileSGF>::const_iterator i_sgf = dgg.games.begin();
  while(i_sgf!=dgg.games.end())
    {
      if(size_goban>0)
	if(i_sgf->size_goban!=size_goban)
	  {
	    i_sgf++;
	    continue;
	  }

      if(mode_print) 
      {
	i_sgf->print();
	cout<<endl;
      }
      int height = i_sgf->size_goban;
      srand(time(NULL));
      GE_Goban gob(height);
      GE_Game game(gob);
      
      list<int>::const_iterator i_g = (i_sgf->game).begin();
      while(i_g!=(i_sgf->game).end())
	{
	  
	  GE_Tactic tact;
	  GE_Chains neighbours_last_move;
	  GE_Chain chain_last_move;
	  GE_Chain chain_before_last_move;
	  int last_move = game.get_last_move();
	  int before_last_move = game.get_before_last_move();
	  
	  
	  if(last_move>=0)
	    {
	      neighbours_last_move.neighbour_chains(*(game.goban), last_move);
	      chain_last_move.init(*(game.goban), last_move);
	    }
	  
	  if(before_last_move>=0)
	    chain_before_last_move.init(*(game.goban), before_last_move);
	  
	  
	  
	  list<int> one_coord; 
	  list<pair<int, int> > solutions; 
	  int nb_algos = 0;
	  
	  //get_moves_by_random(game, one_coord, 1); name = "moves by random"; nb_algos++;
	  //tact.eat_or_to_be_eaten(game, neighbours_last_move, solutions, -1, -1); name = "eat or to be eaten"; nb_algos++;
	  //success: 2931 (71.6626%)   failure: 1159 (28.3374%)
	  //get_moves_by_shape(game, last_move, one_coord, GE_CONNEXITY_8); name = "moves by shape"; nb_algos++;
	  //tact.flee0(game, neighbours_last_move, solutions, -1); name = "flee0"; nb_algos++;
	  //success: 8247 (42.1647%)   failure: 11312 (57.8353%)

	  //tact.flee1(game, neighbours_last_move, solutions, -1, true, true); name = "flee1"; nb_algos++;
	  //-1 true false success: 8245 (46.8546%)   failure: 9352 (53.1454%)
	  //-1 true true success: 7616 (57.2632%)   failure: 5684 (42.7368%)
	  
	  //tact.threat0(game, chain_last_move, solutions, true); name = "threat0"; nb_algos++;
	  //true success: 4943 (38.297%)   failure: 7964 (61.703%)
	  
	  //tact.attacks_double_atari0(game, last_move, chain_last_move, one_coord, true); name = "attacks double atari 0"; nb_algos++;
	  //true success: 192 (27.3504%)   failure: 510 (72.6496%)
	    
	  //tact.attacks_two_groups0(game, last_move, chain_last_move, one_coord, 3, true); name = "attacks two groups 0"; nb_algos++;
	  //true 1 success: 174 (72.5%)   failure: 66 (27.5%)
	  //true 2 --> tact.attacks_double_atari0
	  //true 3 success: 314 (21.4188%)   failure: 1152 (78.5812%)
	  //true 4 success: 306 (23.0248%)   failure: 1023 (76.9752%)


	  //tact.reduce_liberties0(game, chain_last_move, solutions, 6, true); name = "reduce liberties0"; nb_algos++; 
	  //-1 true success: 36989 (26.99%)   failure: 100058 (73.01%)
	  //1 false success: 453 (75%)   failure: 151 (25%)
	  //2 true success: 4943 (38.5569%)   failure: 7877 (61.4431%)
	  //3 true success: 11272 (34.2781%)   failure: 21612 (65.7219%)
	  //4 true success: 10662 (21.597%)   failure: 38706 (78.403%)
	  //5 true success: 4881 (23.9535%)   failure: 15496 (76.0465%)
	  //6 true success: 2116 (22.09%)   failure: 7463 (77.91%)
	  
	  //tact.attacks_by_reducing_liberties0(game, chain_last_move, chain_before_last_move, solutions, true); name = "attacks by reducing liberties0"; nb_algos++; 
	  // success: 6223 (16.4351%)   failure: 31641 (83.5649%)
	  //tact.connect_if_dead_virtual_connection0(game, last_move, solutions, false); name = "connect if dead virtual connection 0"; nb_algos++; 
	  //false success: 3089 (41.4186%)   failure: 4369 (58.5814%)
	  
	  //tact.connect0(game, last_move, one_coord, true); name = "connect 0"; nb_algos++; 
	  //success: 1907 (52.7378%)   failure: 1709 (47.2622%)
	    

	  //tact.peep_connect0(game, last_move, one_coord, true); name = "peep connect 0"; nb_algos++; 
	  //true success: 3404 (46.1685%)   failure: 3969 (53.8315%)
	    
	  //tact.tsuke_hane0(game, last_move, one_coord, true); name = "tsuke hane 0"; nb_algos++;
	  //success: 11173 (35.4383%)   failure: 20355 (64.5617%)
	  
	  //tact.kosumi_tsuke_stretch0(game, last_move, one_coord, true); name = "kosumi tsuke stretch 0"; nb_algos++;
	  //true success: 3133 (23.4805%)   failure: 10210 (76.5195%)
	  
	  //tact.thrust_block0(game, last_move, one_coord, true); name = "thrust block 0"; nb_algos++;
	  //success: 5388 (58.6098%)   failure: 3805 (41.3902%)
	  
	  //tact.increase_liberties0(game, last_move, one_coord, true); name = "increase liberties 0"; nb_algos++; 
	  //true success: 3844 (16.0007%)   failure: 20180 (83.9993%)
	    
	  //tact.prevent_connection0(game, last_move, solutions, -1, 0, true); name = "prevent connection 0"; nb_algos++; 
	  //tact.prevent_connection0(game, last_move, solutions, 3, &chain_last_move, true); name = "prevent connection 0"; nb_algos++; 
	  //-1 0 true success: 6147 (11.6159%)   failure: 46772 (88.3841%)
	  //1 0 true success: 667 (68.9762%)   failure: 300 (31.0238%)
	  //2 0 true success: 1526 (22.9336%)   failure: 5128 (77.0664%)
	  //3 0 true success: 2047 (13.0757%)   failure: 13608 (86.9243%)
	  //-1 ch true success: 5500 (14.1421%)   failure: 33391 (85.8579%)
	  //2 ch true success: 1273 (22.724%)   failure: 4329 (77.276%)
	  //3 ch true success: 1903 (15.0721%)   failure: 10723 (84.9279%)
	  
	  //tact.one_point_jump0(game, before_last_move, one_coord, true); name = "one point jump 0"; nb_algos++; 
	  //success: 1955 (3.15048%)   failure: 60099 (96.8495%)
	    


	  //GE_Strategy strat; strat.maximize_territory(game, &one_coord); name = "maximize territory"; nb_algos++;
	  //GE_Strategy strat; strat.maximize_suffocation(game, &one_coord); name = "maximize suffocation"; nb_algos++; //success: 15023 (10.3922%)   failure: 129537 (89.6078%)
	  //GE_Strategy strat; strat.wall0(game, one_coord); name = "wall0"; nb_algos++;
	  //success: 3131 (22.7626%)   failure: 10624 (77.2374%)
	  //GE_Strategy strat; strat.jam0(game, one_coord); name = "jam0"; nb_algos++;
	  //get_moves_by_amaf0(game, one_coord, 100, 10); name = "amaf0"; nb_algos++;
	  //sim 1000 max_length 5  (one game 19x19) success: 8 (5.09554%)   failure: 149 (94.9045%)

	  //GE_Algorithm::solutions0(game, one_coord); name = "solutions 0"; nb_algos++;
	  GE_Algorithm::solutions1(game, one_coord); name = "solutions 1"; nb_algos++;

	  //   list<int> l_shape; get_moves_by_shape(game, last_move, l_shape, GE_SHAPE_PYRAMID);
	  // 	  list<int> inter;
	  // 	  GE_ListOperators::intersection(one_coord, l_shape, inter);
	  // 	  one_coord = inter;
	  
	  
	  
	  
	  if(nb_algos==0)
	    {
	      cout<<"*** ERROR *** no algorithm"<<endl;
	      exit(-1);
	    }
	  
	  if(nb_algos>1)
	    {
	      cout<<"*** ERROR *** too many algorithms"<<endl;
	      exit(-1);
	    }
	  
	  
	  if(mode_print) cout<<name<<" --> ";
	  
	  
	  if((one_coord.empty())&&(solutions.empty()))
	    {
	      if(mode_print) cout<<" no solution"<<endl;
	      
	    }
	  else
	    {
	      if((one_coord.empty())&&(not solutions.empty()))
		one_coord = GE_ListOperators::get_one_coordinate(solutions); 
	      
	      int nb_loop = one_coord.size();
	      while(nb_loop>0)
		{
		  nb_loop--;
		  
		  if(game.is_allowed(one_coord.front()))
		    one_coord.push_back(one_coord.front());
		  
		  one_coord.pop_front();
		  
		}
	      
	      GE_ListOperators::unique(one_coord);
	     
	      if(one_coord.empty())
		{
		  if(mode_print) cout<<" no solution because of ko"<<endl;;
		}
	      else
		{
		  
		  
		  nb_checked++;
		  if(GE_ListOperators::is_in(one_coord, *i_g))
		    {
		      success++;
		      
		// 	cout<<"success n°"<<success<<endl;
			
// 			//gob.print(*i_g);
// 			gob.print(last_move);
// 			cout<<"solutions --> ";
// 			GE_ListOperators::print(one_coord, move_to_string, (game.goban)->height);
// 			cout<<"and the good move is "<<move_to_string(gob.height, *i_g)<<endl;
// 			cout<<endl;		      
		      
		    }
		  else 
		    {
		      failure++;

		      if(print_err)
			{
			  cout<<"failure n°"<<failure<<endl;
			  
			  //gob.print(*i_g);
			  gob.print(last_move);
			  cout<<"solutions --> ";
			  GE_ListOperators::print(one_coord, move_to_string, (game.goban)->height);
			  cout<<"but the good move is "<<move_to_string(gob.height, *i_g)<<endl;
			  cout<<endl;
			}
		    }
		  if(mode_print) 
		    {
		      GE_ListOperators::print(one_coord, move_to_string, (game.goban)->height);
		      cout<<endl;
		    }
		}
	    }	  
	  
	  game.play(*i_g);
	  
	  //[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
	  
	  //    list<int> l_moves;
	  // 	    if(GE_Tactic::study_shisho(game, *i_g, &l_moves))
	  // 	    {
	  // 	    gob.print(*i_g);
	  // 	    GE_ListOperators::print(l_moves, move_to_string, (game.goban)->height);
	  // 	    cout<<move_to_string(gob.height, *i_g)<<endl;
	  // 	    cout<<endl;
	  // 	    //sleep(5);
	  // 	    exit(-1);
	  // 	    }   
	  //]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

	  if(mode_print) 
	    {
	      gob.print(*i_g);
	      cout<<move_to_string(gob.height, *i_g)<<endl;
	      cout<<endl;
	      //sleep(5);
	    }   
	  i_g++;




// 	  if((mode_print)||(print_res))
// 	    {
// 	      cout<<name<<endl;
// 	      cout<<"success: "<<success<<" ("<<100*success/(float)nb_checked<<"%)";
// 	      cout<<"   failure: "<<failure<<" ("<<100*failure/(float)nb_checked<<"%)"<<endl;
// 	    }


	}
      
      i_sgf++;

     
      
      if((mode_print)||(print_res))
	{
       	  cout<<name<<endl;
       	  cout<<"success: "<<success<<" ("<<100*success/(float)nb_checked<<"%)";
       	  cout<<"   failure: "<<failure<<" ("<<100*failure/(float)nb_checked<<"%)"<<endl;
       	}
      
    }
  
  if((mode_print)||(print_res))
    {
      cout<<name<<endl;
      cout<<"success: "<<success<<" ("<<100*success/(float)nb_checked<<"%)";
      cout<<"   failure: "<<failure<<" ("<<100*failure/(float)nb_checked<<"%)"<<endl;
    }
  
}



bool GE_Algorithm::solutions0(GE_Game& game, list<int>& solutions)
{
  list<pair<int, int> > temp_solutions;
  GE_Tactic tact;
  
  list<int> one_coord;
  GE_Chains neighbours_last_move;
  GE_Chain chain_last_move;
  GE_Chain chain_before_last_move;
  int last_move = game.get_last_move();
  int before_last_move = game.get_before_last_move();
  
  
  if(last_move>=0)
    {
      neighbours_last_move.neighbour_chains(*(game.goban), last_move);
      chain_last_move.init(*(game.goban), last_move);
    }
  
  if(before_last_move>=0)
    chain_before_last_move.init(*(game.goban), before_last_move);
   
  //75%
  tact.reduce_liberties0(game, chain_last_move, temp_solutions, 1, true);
  solutions = GE_ListOperators::get_one_coordinate(temp_solutions); 
  if(not solutions.empty()) return true;

  //72.5%
  tact.attacks_two_groups0(game, last_move, chain_last_move, solutions, 1, true); 
  if(not solutions.empty()) return true;

  //71.6626%
  tact.eat_or_to_be_eaten(game, neighbours_last_move, temp_solutions, -1, -1);
  solutions = GE_ListOperators::get_one_coordinate(temp_solutions); 
  if(not solutions.empty()) return true;
  
  //68.9762%
  tact.prevent_connection0(game, last_move, temp_solutions, 1, 0, true); 
  solutions = GE_ListOperators::get_one_coordinate(temp_solutions); 
  if(not solutions.empty()) return true;

  /*
  //42.1647%
  tact.flee0(game, neighbours_last_move, temp_solutions, -1); 
  solutions = GE_ListOperators::get_one_coordinate(temp_solutions); 
  if(not solutions.empty()) return true;
  */

  //53.6394%
  tact.flee1(game, neighbours_last_move, temp_solutions, -1, true, true);
  solutions = GE_ListOperators::get_one_coordinate(temp_solutions); 
  if(not solutions.empty()) return true;
  
  //38.5569%
  tact.reduce_liberties0(game, chain_last_move, temp_solutions, 2, true); 
  solutions = GE_ListOperators::get_one_coordinate(temp_solutions); 
  if(not solutions.empty()) return true;
    
  //38.297%
  tact.threat0(game, chain_last_move, temp_solutions, true); 
  solutions = GE_ListOperators::get_one_coordinate(temp_solutions); 
  if(not solutions.empty()) return true;

  //58.6098%
  tact.thrust_block0(game, last_move, solutions, true);
  if(not solutions.empty()) return true;
  
  //52.7378%
  tact.connect0(game, last_move, solutions, true);
  if(not solutions.empty()) return true;

  //46.1685%
  tact.peep_connect0(game, last_move, solutions, true);
  if(not solutions.empty()) return true;
  
  //41.4186%
  tact.connect_if_dead_virtual_connection0(game, last_move, temp_solutions, false); 
  solutions = GE_ListOperators::get_one_coordinate(temp_solutions); 
  if(not solutions.empty()) return true;  
  /*
  //34.4557%
  tact.tsuke_hane0(game, last_move, solutions, true); 
  if(not solutions.empty()) return true;  
  
  //23.4805%
  tact.kosumi_tsuke_stretch0(game, last_move, one_coord, true);
  if(not solutions.empty()) return true;
  */ 
  /*
  //34.2781%
  tact.reduce_liberties0(game, chain_last_move, temp_solutions, 3, true);
  solutions = GE_ListOperators::get_one_coordinate(temp_solutions); 
  if(not solutions.empty()) return true;
  */
  /*
//16%
  tact.increase_liberties0(game, last_move, solutions, true);
  if(not solutions.empty()) return true;  
*/
  return false;
}



bool GE_Algorithm::solutions1(GE_Game& game, list<int>& solutions)
{
  list<pair<int, int> > temp_solutions;
  GE_Tactic tact;
  
  list<int> one_coord;
  GE_Chains neighbours_last_move;
  GE_Chain chain_last_move;
  GE_Chain chain_before_last_move;
  int last_move = game.get_last_move();
  int before_last_move = game.get_before_last_move();

  
  if(last_move>=0)
    {
      neighbours_last_move.neighbour_chains(*(game.goban), last_move);
      chain_last_move.init(*(game.goban), last_move);
    }
 
  if(before_last_move>=0)
    chain_before_last_move.init(*(game.goban), before_last_move);


  //75%
  tact.reduce_liberties0(game, chain_last_move, temp_solutions, 1, true);
  solutions = GE_ListOperators::get_one_coordinate(temp_solutions); 
  if(not solutions.empty()) return true;

  if(tact.study_shisho2(game, game.get_last_move(), &solutions))
    {
      if(not solutions.empty()) return true;
    }

  //72.5%
  tact.attacks_two_groups0(game, last_move, chain_last_move, solutions, 1, true); 
  if(not solutions.empty()) return true;

  //71.6626%
  tact.eat_or_to_be_eaten(game, neighbours_last_move, temp_solutions, -1, -1);
  solutions = GE_ListOperators::get_one_coordinate(temp_solutions); 
  if(not solutions.empty()) return true;
  
  //68.9762%
  tact.prevent_connection0(game, last_move, temp_solutions, 1, 0, true); 
  solutions = GE_ListOperators::get_one_coordinate(temp_solutions); 
  if(not solutions.empty()) return true;

  /*
  //42.1647%
  tact.flee0(game, neighbours_last_move, temp_solutions, -1); 
  solutions = GE_ListOperators::get_one_coordinate(temp_solutions); 
  if(not solutions.empty()) return true;
  */

  //53.6394%
  tact.flee1(game, neighbours_last_move, temp_solutions, -1, true, true);
  solutions = GE_ListOperators::get_one_coordinate(temp_solutions); 
  if(not solutions.empty()) return true;
  
  //38.5569%
  tact.reduce_liberties0(game, chain_last_move, temp_solutions, 2, true); 
  solutions = GE_ListOperators::get_one_coordinate(temp_solutions); 
  if(not solutions.empty()) return true;
    
  //38.297%
  tact.threat0(game, chain_last_move, temp_solutions, true); 
  solutions = GE_ListOperators::get_one_coordinate(temp_solutions); 
  if(not solutions.empty()) return true;

  //58.6098%
  tact.thrust_block0(game, last_move, solutions, true);
  if(not solutions.empty()) return true;
  
  //52.7378%
  tact.connect0(game, last_move, solutions, true);
  if(not solutions.empty()) return true;

  //46.1685%
  tact.peep_connect0(game, last_move, solutions, true);
  if(not solutions.empty()) return true;
  
  //41.4186%
  tact.connect_if_dead_virtual_connection0(game, last_move, temp_solutions, false); 
  solutions = GE_ListOperators::get_one_coordinate(temp_solutions); 
  if(not solutions.empty()) return true;  
  /*
  //34.4557%
  tact.tsuke_hane0(game, last_move, solutions, true); 
  if(not solutions.empty()) return true;  
  
  //23.4805%
  tact.kosumi_tsuke_stretch0(game, last_move, one_coord, true);
  if(not solutions.empty()) return true;
  */ 
  /*
  //34.2781%
  tact.reduce_liberties0(game, chain_last_move, temp_solutions, 3, true);
  solutions = GE_ListOperators::get_one_coordinate(temp_solutions); 
  if(not solutions.empty()) return true;
  */
  /*
//16%
  tact.increase_liberties0(game, last_move, solutions, true);
  if(not solutions.empty()) return true;  
*/
  return false;
}






bool GE_Algorithm::can_be_useless(GE_Game& g, int location, int color_stone)
{
  const int& height = (g.goban)->height;
  const int& width = (g.goban)->width;
  
  GE_Chain ch_N;
  GE_Chain ch_S;
  GE_Chain ch_W;
  GE_Chain ch_E;
  
  pair<int, int> loc2 = int_to_pair(height, location);
  pair<int, int> locN = make_pair(loc2.first-1, loc2.second);
  pair<int, int> locE = make_pair(loc2.first, loc2.second+1);
  pair<int, int> locS = make_pair(loc2.first+1, loc2.second);
  pair<int, int> locW = make_pair(loc2.first, loc2.second-1);
  
  if(GE_IS_IN_GOBAN(locN.first, locN.second, height, width))
    {
      if((g.goban)->get_stone(locN)==color_stone)
	{
	  ch_N.init(*(g.goban), pair_to_int(height, locN));
	  if(ch_N.is_atari())
	    return false;
	}
    }  

  if(GE_IS_IN_GOBAN(locS.first, locS.second, height, width))
    {
      if((g.goban)->get_stone(locS)==color_stone)
	{
	  ch_S.init(*(g.goban), pair_to_int(height, locS));
	  if(ch_S.is_atari())
	    return false;
	}
    }  
  
  if(GE_IS_IN_GOBAN(locW.first, locW.second, height, width))
    {
      if((g.goban)->get_stone(locW)==color_stone)
	{
	  ch_W.init(*(g.goban), pair_to_int(height, locW));
	  if(ch_W.is_atari())
	    return false;
	}
    }  

   if(GE_IS_IN_GOBAN(locE.first, locE.second, height, width))
    {
      if((g.goban)->get_stone(locE)==color_stone)
	{
	  ch_E.init(*(g.goban), pair_to_int(height, locE));
	  if(ch_E.is_atari())
	    return false;
	}
    }  
  
  
  return true;
}


bool GE_Algorithm::accept_move(GE_Game& g, int location, string* msg)
{
 
  if(location==GE_PASS_MOVE) return true;
  
  pair<int, int> loc2 = int_to_pair((g.goban)->height, location);
  
  int future_color_stone;
  if(g.tc==GE_BLACK)
    future_color_stone = GE_BLACK_STONE;
  else future_color_stone = GE_WHITE_STONE;
  
  if(not can_be_useless(g, location, future_color_stone))
    return true;
  
  (g.goban)->play(location, GE_NEXT_COLOR(g.tc));
  GE_Chain ch_virt;
  ch_virt.init(*(g.goban), location);
  (g.goban)->back_play(location);
  if(ch_virt.is_atari())
    {
      



      (*msg) = "REFUSED: ";
      (*msg) = (*msg) + "nothing - ";
      (*msg) = (*msg) + "self atari";
      
      return false;
    }
  
  
  
  GE_Tactic tact;
  int motif = 0;

  if(tact.connect_kosumi(g, loc2, GE_NORTH, future_color_stone))
    if(tact.useless_connect_kosumi(g, loc2, GE_NORTH, future_color_stone, motif))
      {
	if(msg)
	  {
	    string msg_motif;
	    (*msg) = "REFUSED: ";
	    (*msg) = (*msg) + "connect kosumi - ";
	    tact.motif_to_string(motif, msg_motif);
	    (*msg) = (*msg) + msg_motif;
	  }
	return false;
      }
  
  if(tact.connect_kosumi(g, loc2, GE_SOUTH, future_color_stone))
    if(tact.useless_connect_kosumi(g, loc2, GE_SOUTH, future_color_stone, motif))
      {
	if(msg)
	  {
	    string msg_motif;
	    (*msg) = "REFUSED: ";
	    (*msg) = (*msg) + "connect kosumi - ";
	    tact.motif_to_string(motif, msg_motif);
	    (*msg) = (*msg) + msg_motif;
	  }
	return false;
      }
  
  if(tact.connect_kosumi(g, loc2, GE_EAST, future_color_stone))
    if(tact.useless_connect_kosumi(g, loc2, GE_EAST, future_color_stone, motif))
      {
	if(msg)
	  {
	    string msg_motif;
	    (*msg) = "REFUSED: ";
	    (*msg) = (*msg) + "connect kosumi - ";
	    tact.motif_to_string(motif, msg_motif);
	    (*msg) = (*msg) + msg_motif;
	  }
	return false;
      }
  
  if(tact.connect_kosumi(g, loc2, GE_WEST, future_color_stone))
    if(tact.useless_connect_kosumi(g, loc2, GE_WEST, future_color_stone, motif))
      {
	if(msg)
	  {
	    string msg_motif;
	    (*msg) = "REFUSED: ";
	    (*msg) = (*msg) + "connect kosumi - ";
	    tact.motif_to_string(motif, msg_motif);
	    (*msg) = (*msg) + msg_motif;
	  }
	return false;
      }
  
  //connect tobi
   if(tact.connect_tobi(g, loc2, GE_NORTH, future_color_stone))
    if(tact.useless_connect_tobi(g, loc2, GE_NORTH, future_color_stone, motif))
      {
	if(msg)
	  {
	    string msg_motif;
	    (*msg) = "REFUSED: ";
	    (*msg) = (*msg) + "connect tobi - ";
	    tact.motif_to_string(motif, msg_motif);
	    (*msg) = (*msg) + msg_motif;
	  }
	return false;
      }
  
  if(tact.connect_tobi(g, loc2, GE_EAST, future_color_stone))
    if(tact.useless_connect_tobi(g, loc2, GE_EAST, future_color_stone, motif))
      {
	if(msg)
	  {
	    string msg_motif;
	    (*msg) = "REFUSED: ";
	    (*msg) = (*msg) + "connect tobi - ";
	    tact.motif_to_string(motif, msg_motif);
	    (*msg) = (*msg) + msg_motif;
	  }
	return false;
      }
  
  



  return true;
}

void GE_Algorithm::test_accept(int size_goban, bool mode_print)
{ 
  int nb_refused = 1;

  GE_DbGamesGo dgg; 
  list<GE_FileSGF>::const_iterator i_sgf = dgg.games.begin();
  while(i_sgf!=dgg.games.end())
    {
      if(size_goban>0)
	if(i_sgf->size_goban!=size_goban)
	  {
	    i_sgf++;
	    continue;
	  }
      
      if(mode_print) 
	{
	  i_sgf->print();
	  cout<<endl;
	}
      int height = i_sgf->size_goban;
      srand(time(NULL));
      GE_Goban gob(height);
      GE_Game game(gob);
      
      list<int>::const_iterator i_g = (i_sgf->game).begin();
      while(i_g!=(i_sgf->game).end())
	{
	  
	  string msg_error;
	  bool acceptation = GE_Algorithm::accept_move(game, *i_g, &msg_error);
	  if(not acceptation)
	    {
	      if(mode_print) 
		{
		  

		  gob.print(*i_g);
		  cout<<move_to_string(gob.height, *i_g)<<endl;
		  cout<<nb_refused<<". "<<msg_error;
		  if(game.tc==GE_BLACK) cout<<" (for black)";
		  else cout<<" (for white)";
		  cout<<endl;
		  //sleep(5);
		  nb_refused++;
		}   
	    }
	  

	  
	  game.play(*i_g);
	  
	  //  if(not acceptation)
	  // 	    {
	  // 	      if(mode_print) 
	  // 		{
	  // 		  cout<<"after the move"<<endl;
	  // 		  gob.print(*i_g);
	  // 		  cout<<endl<<endl<<endl;
	  // 		  //sleep(5);
	  // 		}   
	  // 	    }
	  

	  i_g++;



	}
      
      i_sgf++;
 
    }
  
}


int GE_Algorithm::reflex_move(GE_Game& g, GE_Informations* infos)
{
  int last_move = (g.get_last_move());
  
  if(last_move<0) return GE_PASS_MOVE;
  
  if(infos)
    {
      int num_group = (infos->get_index(g, last_move));
      
      const GE_GroupInformation& gi = *((infos->groups)[num_group]);
      
      int status_group = gi.get_status();
      
      if((gi.get_size()>1)
	 &&((status_group==GE_KILL)||(status_group==GE_KILL_SAVE))
	 &&(gi.motif_dead==GE_KILL_BY_SHISHO))
	{
	  cerr<<"reflex move by shisho"<<endl;


	  list<int> l_moves;
	  GE_Tactic tact;
	  tact.study_shisho2(g, last_move, &l_moves);
	  
	  return GE_ListOperators::select(l_moves, 
				       GE_ListOperators::random);
	} 
    }
  
  return GE_PASS_MOVE;
}

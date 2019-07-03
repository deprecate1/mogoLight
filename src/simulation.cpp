#include "simulation.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int GE_Simulation::num_move_beginning = 0;
bool GE_Simulation::is_end = false;
int GE_Simulation::winner = GE_NEUTRAL_COLOR;
int GE_Simulation::points = 0;
float GE_Simulation::komi = 0.5f;
bool GE_Simulation::update_komi = false;
bool GE_Simulation::printing = false;
list<int> GE_Simulation::free_intersections; 


void GE_Simulation::fill_free_intersections(const GE_Game& g)
{
  int size_goban = (g.goban)->size();
  
  for(int i = 0; i<size_goban; i++)
    {
      if((g.goban)->get_stone(i)==GE_WITHOUT_STONE)
	free_intersections.push_back(i);
    }
}

void GE_Simulation::update_free_intersections(const GE_Game& g)
{
  //a possibility
  // if(not no_chosen.empty())
  //     free_intersections.splice(free_intersections.end(), no_chosen);
  
  list<int> stones_taken;
  g.get_last_stones_taken(stones_taken);
  if(not stones_taken.empty())
    free_intersections.splice(free_intersections.end(), stones_taken);
}



//TO DEBUG we assume that we have one move no stupid.
float GE_Simulation::choice_komi(GE_Game& g, const float* my_komi, bool thinking, 
			      bool mode_print)
{
  if(my_komi)
    GE_Simulation::komi = *my_komi;

  if(mode_print)
    {
      cout<<"*** choice of komi ***"<<endl;
      cout<<"   the old komi: "<<GE_Simulation::komi<<endl;
    }
  
     
  if(not thinking)
    {
      if(mode_print) cout<<"   no modification for komi"<<endl;
      return GE_Simulation::komi;
    }
  
  bool is_victorious = false;
  GE_Amaf am((g.goban)->size());
  
  int nb_sim = 0;
  const int max_loop = 5;//10*(g.goban)->size();
  while(nb_sim<max_loop)
    {
      nb_sim++;
      GE_Simulation::launch(g);
      GE_Simulation::update_amaf(g, am);
    }
  
  if(g.tc==GE_WHITE)
    {
      if(am.get_max(am.white_values, true)>0)
	{
	  GE_Simulation::komi--;
	  is_victorious = true;
	}
      else
	{
	  GE_Simulation::komi++;
	  is_victorious = false;
	}
    }
  else
    {
      if(am.get_max(am.black_values, true)>0)
	{
	  GE_Simulation::komi++;
	  is_victorious = true;
	}
      else
	{
	  GE_Simulation::komi--;
	  is_victorious = false;
	}
    }
  
  while(true)
    {
      am.clear();
      
      nb_sim = 0;
      while(nb_sim<max_loop)
	{
	  nb_sim++;
	  GE_Simulation::launch(g);
	  GE_Simulation::update_amaf(g, am);
	}
      
      if(g.tc==GE_WHITE)
	{
	  if(am.get_max(am.white_values, true)>0)
	    {
	      if(not is_victorious) break;
	      
	      GE_Simulation::komi--;
	      is_victorious = true;
	    }
	  else
	    {
	      if(is_victorious)
		{
		  GE_Simulation::komi++;
		  break;
		}
	      
	      GE_Simulation::komi++;
	      is_victorious = false;
	    }
	}
      else
	{
	  if(am.get_max(am.black_values, true)>0)
	    {
	      if(not is_victorious) break;
	      
	      GE_Simulation::komi++;
	      is_victorious = true;
	    }
	  else
	    {
	      if(is_victorious)
		{
		  GE_Simulation::komi--;
		  break;
		}
	      
	      GE_Simulation::komi--;
	      is_victorious = false;
	    }
	}
    }
  
  if(mode_print)
    cout<<"   the new komi: "<<GE_Simulation::komi<<endl;

  return GE_Simulation::komi;
}


int GE_Simulation::choice(GE_Game& g, const int size_goban)
{
  if(is_end)
    {
      if(free_intersections.empty())
	GE_Simulation::fill_free_intersections(g);

      int mv = GE_PASS_MOVE;
      int (*fct_random)(list<int>&, list<int>::iterator&) = &GE_ListOperators::random;
      list<int> no_chosen;
      
      while(not free_intersections.empty())
	{
	  int mv = GE_ListOperators::select(free_intersections, *fct_random);
	  
	  if((not g.is_allowed(mv))||(g.is_stupid2(mv)))
	    no_chosen.push_back(mv);
	  else break;
	  
	  /*
	    if((not g.is_allowed(mv))||(g.is_stupid3(mv))) //CELUI CI(9x9)
	    no_chosen.push_back(mv);
	    else break;
	  */	  
	  /*
	  if((not g.is_allowed(mv))||(g.is_self_kill_group0(mv)))
	    no_chosen.push_back(mv);
	  else break;
	  */
	}
     
      bool is_empty = free_intersections.empty();
      if(not no_chosen.empty())
	free_intersections.splice(free_intersections.end(), no_chosen);
      
      if(is_empty)
	return GE_PASS_MOVE;
      else return mv;
      
      //     list<int> l_moves;
      //       for(int i = 0; i<size_goban; i++)
      // 	{
      // 	  if(not g.is_allowed(i)) continue;
      // 	  if(g.is_stupid2(i)) continue;
      
      // 	  l_moves.push_back(i);
      // 	}
      
      //       if(l_moves.empty())
      // 	return GE_PASS_MOVE;
      
      //       return GE_ListOperators::random(l_moves);
    }
  
  int nb_loop = 1;
  int max_loop = 2*size_goban;//2*size_goban;
  while(true)
    {
      int mv = rand()%size_goban;
      
      if(g.is_allowed(mv))
	{
	  if(not g.is_stupid2(mv))
	    return mv;
	  
	  /*
	    if(not g.is_stupid3(mv))
	    return mv;
	  */

	  /*
	    if(not g.is_self_kill_group0(mv))
	    return mv;
	  */
	}

      nb_loop++;
      
      if(nb_loop > max_loop)
	{
	  is_end = true;
	  return choice(g, size_goban);
	}    
    }
  
  return GE_PASS_MOVE;
}

int GE_Simulation::launch(GE_Game& g)
{
  free_intersections.clear();

  const int size_goban = (g.goban)->size();
  const int max_game = (int)(1.3f*size_goban)+num_move_beginning; //1.3*size_goban (en 9x9)
  num_move_beginning = (g.game).size();
  is_end = false;
  winner = GE_NEUTRAL_COLOR;
  
  int nb_pass = 0;
  while(nb_pass<2)
    {
      int mv = choice(g, size_goban);
      
      if((int)g.game.size()>max_game)
	{
	  //mv = GE_PASS_MOVE;
	  //return GE_NEUTRAL_COLOR;
	  return evaluate(g);
	}

      g.play2(mv);
      
      if(GE_Simulation::is_end) GE_Simulation::update_free_intersections(g);
      
      if(mv==GE_PASS_MOVE)
	nb_pass++;
      else nb_pass = 0;
    }  

  return evaluate(g);
}


int GE_Simulation::evaluate(GE_Game& g, float komi)
{
  int score = 0;
  int& height = (g.goban)->height;
  int& width = (g.goban)->width; 
  for(int i = 0; i<height; i++)
    for(int j = 0; j<width; j++)
      {
	switch((g.goban)->board[i][j])
	  {
	  case GE_WHITE_STONE : 
	    score--;
	    break;

	  case GE_BLACK_STONE : 
	    score++;
	    break;
	    
	  case GE_WITHOUT_STONE :
	    {
	      int color_stone = GE_WITHOUT_STONE;
	      if(GE_IS_IN_GOBAN(i-1, j, height, width))
		color_stone = (g.goban)->board[i-1][j];
	      else color_stone = (g.goban)->board[i+1][j];

	      if(color_stone==GE_WITHOUT_STONE)
		break;
	      
	      if((not (GE_IS_IN_GOBAN(i+1, j, height, width))
		  ||((g.goban)->board[i+1][j]==color_stone))
		 &&(not (GE_IS_IN_GOBAN(i-1, j, height, width))
		    ||((g.goban)->board[i-1][j]==color_stone))
		 &&(not (GE_IS_IN_GOBAN(i, j+1, height, width))
		    ||((g.goban)->board[i][j+1]==color_stone))
		 &&(not (GE_IS_IN_GOBAN(i, j-1, height, width))
		    ||((g.goban)->board[i][j-1]==color_stone)))
		{
		  if(color_stone==GE_WHITE_STONE)
		    score--;
		  else score++;
		}
	    }
	    break;

	  default:;
	  }
      }

  //float score_f = score - komi; //the komi is in favour of white
  float score_f = score - GE_Simulation::komi; //the komi is in favour of white
  points = abs((int)score_f);
  
  if(update_komi)
    {

      if(printing)
	{
	  cout<<"*** updating the komi ***"<<endl;
	  cout<<"   the old komi: "<<GE_Simulation::komi<<endl;
	}

       if(score_f>0)
	 GE_Simulation::komi = score - 0.5f;
       else GE_Simulation::komi = score + 0.5f;
       /*
       //juste
      if(score_f>0)
	GE_Simulation::komi = score + 0.5f;
      else GE_Simulation::komi = score - 0.5f;
       */
      /*
      if(score_f>0)
	GE_Simulation::komi++;
      else GE_Simulation::komi--;
      */

      if(printing)
	cout<<"   the new komi: "<<GE_Simulation::komi<<endl;
    }

  if(score_f>0.f)
    {
      winner = GE_BLACK;
      return GE_BLACK;
    }

  winner = GE_WHITE;
  return GE_WHITE;  
}


void GE_Simulation::update_amaf(GE_Game& g, GE_Amaf& am)
{
  int score = 1;
  //int score2 = 1 + points;
  int score2 = score;

  if((int)g.game.size()<=num_move_beginning)
    return;

  while(true)
    {
      int mv = g.game.back();
      
      if(mv!=GE_PASS_MOVE)
	{
	  if(winner!=GE_NEUTRAL_COLOR)
	    {
	      if(winner!=(g.tc))
		{
		  am.values[mv] += score;
		  if(g.tc==GE_BLACK)
		    am.white_values[mv] += score2;
		  else am.black_values[mv] += score2;
		}
	      else 
		{
		  am.values[mv] -= score;
		  if(g.tc==GE_BLACK)
		    am.white_values[mv] -= score2;
		  else am.black_values[mv] -= score2;
		}
	    }
	}
      
      score++;
      score2++;
      
      g.backward();
      
     
      
      if((int)g.game.size()==num_move_beginning)
	{
	  if(mv!=GE_PASS_MOVE)
	    {
	      if(winner==g.tc)  //== because g.backward
		am.score[mv] += points;
	      else am.score[mv] -= points;
	      am.nb_simulations[mv]++;
	    }
	  break;
	}
    }  
}

void GE_Simulation::update_amaf(GE_Game& g, GE_Amaf& am, int shift_move)
{
  int score = 1;
  //int score2 = 1 + points;
  int score2 = score;
  
  if((int)g.game.size()<=num_move_beginning)
    return;

  while(true)
    {
      int mv = g.game.back();
      
      if(mv!=GE_PASS_MOVE)
	{
	  if(winner!=GE_NEUTRAL_COLOR)
	    {
	     
	      // 	if(winner==GE_BLACK)
	      // 		{
	      // 		  if(g.tc==GE_BLACK)
	      // 		    am.white_values[mv] -= score;
	      // 		  else am.black_values[mv] += score;
	      // 		}
	      // 	      else
	      // 		{
	      // 		  if(g.tc==GE_BLACK)
	      // 		    am.white_values[mv] += score;
	      // 		  else am.black_values[mv] -= score;
	      // 		}
		
	      // 		<==>
	      


	      if(winner!=(g.tc))
		{
		  am.values[mv] += score;
		  if(g.tc==GE_BLACK)
		    am.white_values[mv] += score;
		  else am.black_values[mv] += score;
		}
	      else 
		{
		 
		  am.values[mv] -= score;
		  if(g.tc==GE_BLACK)
		    am.white_values[mv] -= score;
		  else am.black_values[mv] -= score;
		  
		}
	    }
	}
      
      score++;
      score2++;
      
      g.backward();
      
     
      
      if((int)g.game.size()==num_move_beginning-shift_move)
	{
	  if(mv!=GE_PASS_MOVE)
	    {
	      if(winner==g.tc)  //== because g.backward
		am.score[mv] += points;
	      else am.score[mv] -= points;
	      am.nb_simulations[mv]++;
	    }
	  break;
	}
    }  
}






void GE_Simulation::update_amaf(GE_Game& g, GE_Amaf2& am2, int shift_move)
{
  int score = 1;
  //int score2 = 1 + points;
  int score2 = score;
  
  if((int)g.game.size()<=num_move_beginning)
    return;
  
  int next_mv = GE_PASS_MOVE;
  
  list<int> seq;
  const int max_size = 100; //100 is better than -1 ?

  while(true)
    {
      const int mv = g.game.back();
  
      if(mv!=GE_PASS_MOVE)
	{
	  if(winner!=GE_NEUTRAL_COLOR)
	    {
	      
	      if(winner!=(g.tc))
		{
		  am2.update(mv, seq, -score, GE_NEXT_COLOR(g.tc), max_size); 
		  // TO IMPLEMENT sub-function + constant ...

		  am2.values[mv] += score;
		  if(g.tc==GE_BLACK)
		    am2.white_values[mv] += score;		    
		  else
		    am2.black_values[mv] += score;
		}
	      else 
		{
		  am2.update(mv, seq, score, GE_NEXT_COLOR(g.tc), max_size);

		  am2.values[mv] -= score;
		  if(g.tc==GE_BLACK)
		    am2.white_values[mv] -= score;
		  else
		    am2.black_values[mv] -= score;
		}
	    }
	}
      
      score++;
      score2++;
      
      g.backward();
      
      
      
      if((int)g.game.size()==num_move_beginning-shift_move)
	{
	  if(mv!=GE_PASS_MOVE)
	    {
	      if(winner==g.tc)  //== because g.backward
		am2.score[mv] += points;
	      else am2.score[mv] -= points;
	      am2.nb_simulations[mv]++;
	    }
	  break;
	}

      
      next_mv = mv;
    }  
}

void GE_Simulation::update_mc_informations(GE_Game& g, GE_MCLocationsInformations& mcli, int shift_move)
{
  mcli.update(*(g.goban), winner);

  while(true)
    {
      g.backward();
      
      if((int)g.game.size()==num_move_beginning-shift_move)
	break;
    }
}



// void GE_Simulation::update_amaf(GE_Game& g, GE_Amaf2& am2, int shift_move)
// {
//   int score = 1;
//   //int score2 = 1 + points;
//   int score2 = score;
  
//   if((int)g.game.size()<=num_move_beginning)
//     return;
  
//   int next_mv = GE_PASS_MOVE;
  
//   while(true)
//     {
//       const int mv = g.game.back();
//       bool no_pass_move = ((mv!=GE_PASS_MOVE)&&(next_mv!=GE_PASS_MOVE));
      
//       if(mv!=GE_PASS_MOVE)
// 	{
// 	  if(winner!=GE_NEUTRAL_COLOR)
// 	    {
	      
// 	      if(winner!=(g.tc))
// 		{
// 		  if(no_pass_move)
// 		    am2.sequences[mv][next_mv]-=score; //because it is a bad answer
		  
// 		  am2.values[mv] += score;
// 		  if(g.tc==GE_BLACK)
// 		    {
// 		      am2.white_values[mv] += score;
		      
// 		      if(no_pass_move)
// 			am2.white_sequences[mv][next_mv]-=score; //because it is a bad answer
// 		    }
// 		  else
// 		    {
// 		      am2.black_values[mv] += score;
// 		      if(no_pass_move)
// 			am2.black_sequences[mv][next_mv]-=score; //because it is a bad answer
// 		    }
// 		}
// 	      else 
// 		{
// 		  if(no_pass_move)
// 		    am2.sequences[mv][next_mv]+=score; //because it is a good answer
		  
// 		  am2.values[mv] -= score;
// 		  if(g.tc==GE_BLACK)
// 		    {
// 		      am2.white_values[mv] -= score;
		      
// 		      if(no_pass_move)
// 			am2.white_sequences[mv][next_mv]+=score; //because it is a good answer
// 		    }
// 		  else
// 		    {
// 		      am2.black_values[mv] -= score;
		      
// 		      if(no_pass_move)
// 			am2.black_sequences[mv][next_mv]+=score; //because it is a good answer
// 		    }
		  
// 		}
// 	    }
// 	}
      
//       score++;
//       score2++;
      
//       g.backward();
      
      
      
//       if((int)g.game.size()==num_move_beginning-shift_move)
// 	{
// 	  if(mv!=GE_PASS_MOVE)
// 	    {
// 	      if(winner==g.tc)  //== because g.backward
// 		am2.score[mv] += points;
// 	      else am2.score[mv] -= points;
// 	      am2.nb_simulations[mv]++;
// 	    }
// 	  break;
// 	}

      
//       next_mv = mv;
//     }  
// }



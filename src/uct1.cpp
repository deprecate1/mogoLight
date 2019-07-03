#include "uct1.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int GE_ExpertiseUct1::useless_by_dead_stone = -500;
int GE_ExpertiseUct1::useless_connexion = -300;

int GE_ExpertiseUct1::pattern_empty = 300;
int GE_ExpertiseUct1::pattern_bad_connection_tobi = -300;
int GE_ExpertiseUct1::pattern_bad_nobi = -300;


GE_Node1::GE_Node1(int mv)
{
  //is_black_player = true;
  move = mv;
  nb_stones = -1;
  nb_wins = 0;
  nb_sims = 0;
  win_rate = -1;
  //max_points = -1000000000;
  //min_points = 1000000000;

  virtual_wins = 1;
  virtual_loss = 1;
}

int GE_Node1::set_move(int mv)
{
  int old_move = move;
  move = mv;
  return old_move;
}



void GE_Node1::init(const GE_Uct1& uct1, const GE_Informations* infos)
{
  //is_black_player = is_black;
  amaf_wins.resize(GE_Directions::board_area+1, 0);
  amaf_sim.resize(GE_Directions::board_area+1, 0);
  children.resize(GE_Directions::board_area+1);
  win_rate = (uct1.nb_wins/(double)uct1.nb_sims);
  
  if(uct1.to_expertise)
    {
      uct1.compute_expertise(infos);
      
      for(int i = 0; i<(int)children.size(); i++)
	{
	  children[i].set_move(i);
	  
	  //int virtual_wins = (uct1.expertise_go.nb_wins)[i];
	  //int virtual_loss = (uct1.expertise_go.nb_loss)[i];
	  
	  //children[i].nb_wins += virtual_loss; 
	  //because of the strange implementation
	  //children[i].nb_sims += virtual_sims;
	  
	  children[i].virtual_wins = (uct1.expertise_go.nb_wins)[i]+1;
	  children[i].virtual_loss = (uct1.expertise_go.nb_loss)[i]+1;
	  
	  /*
	    if(this->move==GE_ILLEGAL_MOVE)
	    {
	    cerr<<move_to_string(GE_Directions::board_size, i);
	    cerr<<" "<<children[i].virtual_wins;
	    cerr<<" "<<children[i].virtual_loss<<endl;
	    }
	  */
	}
    }
  else
    {
      for(int i = 0; i<(int)children.size(); i++)
	{
	  children[i].set_move(i);
	}
    }
  
  if(this->move==GE_ILLEGAL_MOVE)
    {
      list<int>::const_iterator i_mv = uct1.forbidden_moves.begin();
      while(i_mv!=(uct1.forbidden_moves).end())
	{
	  children[*i_mv].nb_sims = GE_ILLEGAL_MOVE;
	  i_mv++;
	}
    }
  
  
}

bool GE_Node1::isLeaf() const
{
  return (children.size()==0);
}

bool GE_Node1::is_already_simulated() const
{
  return not isLeaf();
  //return (nb_sims!=0);
}

double GE_Node1::getProbaWin()
{
  if(nb_sims==0) return 0;
  return nb_wins/(double)nb_sims;
}


void GE_Node1::computeLeaf(GE_Uct1& uct1)
{
  /*
    if((uct1.game)->tc==GE_BLACK)
    uct1.fmcli.clear(GE_BLACK_STONE);
    else uct1.fmcli.clear(GE_WHITE_STONE);
  */
  
  uct1.clear_amaf_values();
  uct1.nb_wins = 0;
  uct1.nb_sims = 0;
  uct1.fg.fromGame(*(uct1.game));
  
  double timing = 0;
  int& num_sim = uct1.nb_sims;

  while(true)
    {      
      clock_t t_start = clock();
      //fg.fromGame(*game);
      uct1.fg_simu.copy(uct1.fg);
      uct1.fg_simu.launch_simulation(uct1.min_capture);
      uct1.update_amaf_values();

      //uct1.fmcli.update(uct1.fg_simu, uct1.komi);
      clock_t t_end = clock();
      
      timing+=((t_end-t_start)/(float)CLOCKS_PER_SEC);
      
      num_sim++;
      
      //if((timer>=0)&&(timing>timer)) break;
      
      if((uct1.max_simulations>=0)&&(num_sim>=uct1.max_simulations)) break;
    }
}


void GE_Node1::computeLeafByUCT(GE_Uct1& uct1)
{
  assert(0);
  // debugguer pb avec is_sub_uct
  //ajouter une seconde variable


  GE_Uct1 temp_uct;
  temp_uct.init(*(uct1.game), (uct1.komi), 1, 4, 0.3f);
  
  temp_uct.is_sub_uct = true;
  
  temp_uct.compute(100);
  
  amaf_wins = (temp_uct.root_node).amaf_wins;
  amaf_sim = (temp_uct.root_node).amaf_sim;
  nb_wins = (temp_uct.root_node).nb_wins;
  nb_sims = (temp_uct.root_node).nb_sims;
}




double GE_Node1::getNbChildrenNoLeaf()
{
  int nb_children = 0;
   for(int location = 0; location<GE_Directions::board_area; location++)
     {
       if(not children[location].isLeaf())
	 nb_children++;
     }

   return nb_children;
}


double GE_Node1::getExploration(int location)
{
  if(amaf_sim[location]==0) 
    {
      //argmax
     //  float amaf_proba = -1.f;
      
//       for(int i = 0; i<GE_Directions::board_area;i++)
// 	{
// 	  if(children[location].isLeaf()) continue;
	  
// 	  double& wins_children = children[location].amaf_wins[location];
// 	  double& sims_children = children[location].amaf_sim[location];
	  
// 	  float influence_times = sims_children/(float)sims_children+2; 
// 	  float temp_proba = 
// 	    (wins_children/(float)sims_children)*influence_times; 
	  
// 	  if(temp_proba>amaf_proba)
// 	    temp_proba = amaf_proba;
// 	}
      
//       if(amaf_proba<0) 
// 	return 1/2.f;
//       else return amaf_proba;
      
      
      //mean
   //    int wins_children = 0;
//       int sims_children = 0;
      
//       for(int i = 0; i<GE_Directions::board_area;i++)
// 	{
// 	  if(children[location].isLeaf()) continue;
      
// 	  wins_children += children[location].amaf_wins[location];
// 	  sims_children += children[location].amaf_sim[location];
// 	}
      
//       if(sims_children==0)
//        	return 1/2.f;
//       else return wins_children/(float)sims_children;
      
      //return 0.5f;
      
      return 0.45f;

      //return 0;
      //return 1;
    }

  return amaf_wins[location]/amaf_sim[location];
}

double GE_Node1::getExploitation(int location)
{
  return (1-((children[location].nb_wins+0.2f)
	     /(children[location].nb_sims+1)));
  
  //return (1-children[location].getProbaWin());
}


double GE_Node1::evalChildNode(double alpha, int location, 
			    const GE_Uct1& uct1)
{
  //double alpha = getProbaWin();
  double exploitation = getExploitation(location);
  double exploration = getExploration(location);
  double coeff_exp = uct1.coeff_exploration;
 
  
  double fraction_sims = (children[location].nb_sims+1)/(double)(nb_sims+1);
  
  double temp_ratio = 1-((children[location].nb_wins)/(double)(children[location].nb_sims+1));
  //double diff_ratio = (fraction_sims*(temp_ratio-win_rate))/2.f+0.5f;
  double diff_ratio = (temp_ratio-win_rate)/2.f+0.5f;
  if(diff_ratio<=0) diff_ratio = 0.001;
  
  //coeff_exp+=1/pow((float)uct1.nodes_sequence.size(), 3.f);
  
  double repartition = getNbChildrenNoLeaf();
  repartition=repartition/(float)(GE_Directions::board_area);
  
  if(uct1.nodes_sequence.size()%2==0) 
    repartition = 1 - repartition;
  
  double virtual_exploitation = 
    1-(virtual_wins/(double)(virtual_wins+virtual_loss));
  
  double virtual_ratio = (virtual_exploitation-win_rate)/2.f+0.5f;
  
  //repartition = pow((float)repartition, 1/2.f);
  
  //repartition = 1;
  
  //return repartition*(alpha*exploitation+coeff_exp*(1-alpha)*exploration
  //	      +(rand()%1000/(float)10000));
  //return alpha*exploitation*repartition+coeff_exp*(1-alpha)*exploration;
  //return alpha*exploitation*repartition*diff_ratio+coeff_exp*(1-alpha)*exploration;
  
  //return alpha*diff_ratio*virtual_ratio+(1-alpha)*exploration;
  
  //+(rand()%1000/(float)10000);
  
  return alpha*diff_ratio+(1-alpha)*exploration;


  //return repartition*(alpha*exploitation+coeff_exp*(1-alpha)*exploration);
}

double GE_Node1::getCandidates(GE_Uct1& uct1, list<int>& l_candidates)
{

  if(nb_stones<0)
    nb_stones = ((uct1.game)->goban)->count_stones();

  double stone_density = nb_stones/(double)(GE_Directions::board_area); 
   
  //double alpha = getProbaWin();
  //double alpha = 1-((1-getProbaWin())/(sqrt((float)nb_sims)));
  //double alpha = 1-((1-getProbaWin()*stone_density)
  //	    /((pow((float)nb_sims, 7/24.f))));
  double alpha = 1-((1-getProbaWin()*pow((float)stone_density, 2.f))
  		    /((pow((float)nb_sims, 7/24.f))));
  //double alpha = 1-((1-getProbaWin()*stone_density)
  //	    /((pow((float)nb_sims, (float)stone_density))));
  
  //double alpha = 1-((1-getProbaWin())
  //	    /((pow((float)nb_sims, (float)stone_density))));
  
  
  //if(alpha<=0.5f) alpha = 0;
  //alpha = 1;
  //alpha = 0;
  
  double max_eval = 0;
  
  l_candidates.clear();
  //l_candidates.push_back(GE_PASS_MOVE);  
  
  for(int location = 0; location<GE_Directions::board_area; location++)
    {
      double eval_node = evalChildNode(alpha, location, 
				       uct1);
      if(eval_node<max_eval)
	continue;
      
      
      if(children[location].nb_sims<0)
	continue;
      
      //if((children[location].nb_sims==0)
      if((not children[location].is_already_simulated())
	 &&(not (uct1.game)->is_allowed(location)))
	{
	  children[location].nb_sims = GE_ILLEGAL_MOVE;
	  continue;
	}
      
      
      
      //if(not isLegalMove()) continue; //TODO A IMPLEMENTER
      
      if(eval_node>max_eval)
	{
	  max_eval = eval_node;
	  l_candidates.clear();
	  l_candidates.push_back(location);
	  continue;
	}
      
      if(eval_node==max_eval)
	{
	  l_candidates.push_back(location);
	  continue;
	}
    } 
  
  if(l_candidates.empty())
    l_candidates.push_back(GE_PASS_MOVE);
  
  //assert(not l_candidates.empty());
  
  return max_eval;
}

int GE_Node1::choice(GE_Uct1& uct1, double* eval)
{
  list<int> candidates;
  if(eval)
    (*eval) = getCandidates(uct1, candidates);
  else getCandidates(uct1, candidates);
  
  return GE_ListOperators::random(candidates);
}


int GE_Node1::fastChoice(GE_Uct1& uct1, double* eval)
{
  
  //double alpha = getProbaWin();
  double alpha = 1-((1-getProbaWin())/(float)nb_sims);
  //if(alpha<=0.5f) alpha = 0;
  //alpha = 1;
  //alpha = 0;
  
  double max_eval = 0;
  
  //l_candidates.push_back(GE_PASS_MOVE);
  

  int best_choice = GE_ILLEGAL_MOVE;
  int nb_try = 2;
  
  for(int location = 0; location<GE_Directions::board_area; location++)
    {
      double eval_node = evalChildNode(alpha, location, 
				       uct1);
      if(eval_node<max_eval)
	continue;
      
      if(children[location].nb_sims<0)
	continue;
      
      //if((children[location].nb_sims==0)
      if((not children[location].is_already_simulated())
	 &&(not (uct1.game)->is_allowed(location)))
	{
	  children[location].nb_sims = GE_ILLEGAL_MOVE;
	  continue;
	}
      
      
      if(eval_node>max_eval)
	{
	  nb_try = 2;
	  max_eval = eval_node;
	  best_choice = location;
	  continue;
	}
      
      if(eval_node==max_eval)
	{
	  if(rand()%nb_try==0)
	    best_choice = location;
	  nb_try++;
	}
      
      if(best_choice==GE_ILLEGAL_MOVE)
	{
	  best_choice = location;
	  max_eval = eval_node;
	}
    } 
  
  if(eval)
    (*eval) = max_eval;
  
  if(best_choice==GE_ILLEGAL_MOVE)
    best_choice = GE_PASS_MOVE;
  
  return best_choice;
}



double GE_Node1::getBestCandidates(GE_Game& game, list<int>& l_candidates)
{
  double max_eval = 0;
  
  l_candidates.clear();
  //l_candidates.push_back(GE_PASS_MOVE);
  
  for(int location = 0; location<GE_Directions::board_area; location++)
    {
      if(children[location].nb_sims<0)
	continue;
      
      //if((children[location].nb_sims==0)
      if((not children[location].is_already_simulated())
	 &&(not game.is_allowed(location)))
	{
	  children[location].nb_sims = GE_ILLEGAL_MOVE;
	  continue;
	}
      
      double eval_node = getExploitation(location);
	 
      //if(not isLegalMove()) continue; //TODO A IMPLEMENTER
      
      if(eval_node>max_eval)
	{
	  max_eval = eval_node;
	  l_candidates.clear();
	  l_candidates.push_back(location);
	  continue;
	}
      
      if(eval_node==max_eval)
	{
	  l_candidates.push_back(location);
	  continue;
	}
    } 
  
    
  if(l_candidates.empty())
    l_candidates.push_back(GE_PASS_MOVE);
  //assert(not l_candidates.empty());



  return max_eval;
}

int GE_Node1::bestChoice(GE_Game& game, double* eval)
{
  list<int> candidates;
  if(eval)
    (*eval) = getBestCandidates(game, candidates);
  else getBestCandidates(game, candidates);

  return GE_ListOperators::random(candidates);
}

void GE_Node1::preferredMoves(GE_Game& game, list<int>& l_moves, int nb_pref)
{
  l_moves.clear();
  list<int> l_eval;
  
  int location = 0;
  for(location = 0; location<GE_Directions::board_area; location++)
    {
      if(children[location].nb_sims<0)
	continue;
      
      //if((children[location].nb_sims==0)
      if((not children[location].is_already_simulated()) 
	 &&(not game.is_allowed(location)))
	{
	  children[location].nb_sims = GE_ILLEGAL_MOVE;
	  continue;
	}
      
      int eval_node = (int)(getExploitation(location)*100000);
      l_eval.push_back(eval_node);
    } 
  
  l_eval.sort();
  //l_eval.reverse();
  
  location = -1;
  int num_pref = 0;
  int eval = -1;
 
  if(not l_eval.empty())
    {
      eval = l_eval.back();
      l_eval.pop_back();
    }
  else 
    return;
  
  while(num_pref<nb_pref)
    {      
      location = (location+1)%(GE_Directions::board_area);
      
      if(children[location].nb_sims<0)
	continue;
      
      //if((children[location].nb_sims==0)
      if((not children[location].is_already_simulated())
	 &&(not game.is_allowed(location)))
	{
	  children[location].nb_sims = GE_ILLEGAL_MOVE;
	  continue;
	}
      
      int eval_node = (int)(getExploitation(location)*100000);
      if(eval==eval_node)
	{
	  l_moves.push_back(location);
	  if(l_eval.empty()) break;
	  eval = l_eval.back();
	  l_eval.pop_back();
	  num_pref++;
	} 
    } 
}



void GE_Node1::update(GE_Uct1& uct1, const GE_Informations* infos)
{
  assert(uct1.nb_wins<=uct1.nb_sims);
  
  if(isLeaf())
    {
      //std::cerr<<"init leaf"<<std::endl;
      this->init(uct1, infos);
    }
  if(uct1.nodes_sequence.size()%2==1)
    {
      for(int i = 0; i<GE_Directions::board_area; i++)
	{
	  amaf_wins[i] = uct1.black_values[i];
	  amaf_sim[i] = uct1.black_sims[i];
	}


      nb_wins+=uct1.nb_wins;
      nb_sims+=uct1.nb_sims;
    
    }
  else
    {
      for(int i = 0; i<GE_Directions::board_area; i++)
	{
	  amaf_wins[i] = uct1.white_values[i];
	  amaf_sim[i] = uct1.white_sims[i];
	}
      
      nb_wins+=(uct1.nb_sims-uct1.nb_wins);
      nb_sims+=uct1.nb_sims;
    }
}


void GE_Node1::print() //const
{
  for(int i = 0; i<GE_Directions::board_area; i++)
    {
      cerr<<move_to_string(GE_Directions::board_size, i);
      
      cerr<<" "<<(children[i]).nb_wins<<" ";
      cerr<<(children[i]).nb_sims<<" (";
      cerr<<getExploitation(i)<<"%) - ";
      //cerr<<1-getProbaWin(location);
      
      //cerr<<(int)(((node->children[i]).wins/(float)<(node->children[i]).sims)*10000)/100.f<<"%) - ";
      
      cerr<<amaf_wins[i]<<" "<<amaf_sim[i]<<" (";
      cerr<<getExploration(i)<<"%)";
      cerr<<endl;
    }
}




GE_Uct1::GE_Uct1()
{
  //prof = 0;
  komi = GE_KOMI;
  max_simulations = 1;
  min_capture = 4;
  game = 0;
  current_node = 0;
  coeff_exploration = 1;
  to_expertise = true;
}

void GE_Uct1::init(GE_Game& g, float km, int max_simus, int min_capt, 
		double coeff_exp)
{
  //prof = 0;
  root_goban = *(g.goban);
  current_node = &root_node;
  nodes_sequence.push_back(&root_node);
  
  game = &g;
  
  komi = km;
  max_simulations = max_simus;
  min_capture = min_capt;
  
  coeff_exploration = coeff_exp;

  nb_wins = 0;
  nb_sims = 0;

  fg.init();
  fg_simu.init();

  is_sub_uct = false;
  
  expertise_go.set_sims_by_node(max_simus);
}

void GE_Uct1::init_root_node()
{
  if(not root_node.isLeaf()) return;
  
  int mem_max_simulations = this->max_simulations;
  this->max_simulations = 10000;
  
  current_node = &root_node;
  nodes_sequence.clear();
  nodes_sequence.push_back(&root_node);
  
  current_node->computeLeaf(*this);
  current_node->update(*this);

  this->max_simulations = mem_max_simulations;
}



void GE_Uct1::playSequence()
{
  //prof = 0;
  
  while(true)
    {
      if(current_node->isLeaf())
	{
	  if(not is_sub_uct)
	    current_node->computeLeaf(*this);
	  else current_node->computeLeafByUCT(*this);
	  
	  break;
	}
      
      int location = current_node->choice(*this);
      //int location = current_node->fastChoice(*this);
      if(location==GE_PASS_MOVE)
	{
	  while(true)
	    {
	      assert(not nodes_sequence.empty());
	      GE_Node1* nd = nodes_sequence.front();
	      nd->update(*this);
	      nodes_sequence.front() = 0;
	      nodes_sequence.pop_front();
	      if(nodes_sequence.empty()) return;
	      game->backward();
	    }
	}
      
      game->play(location);
      GE_Node1* nd = &(current_node->children[location]);
      nodes_sequence.push_back(&(*nd));
      current_node = nd;
      //prof++;
    }
  
  while(true)
    {
      assert(not nodes_sequence.empty());
      GE_Node1* nd = nodes_sequence.front();
      nd->update(*this);
      nodes_sequence.front() = 0;
      nodes_sequence.pop_front();
      if(nodes_sequence.empty()) break;
      game->backward();
    }
  
  
}

void GE_Uct1::clear_amaf_values()
{
  for(unsigned int i = 0; i<black_values.size(); i++)
    {
      black_values[i] = 0;
      black_sims[i] = 0;
      white_values[i] = 0;
      white_sims[i] = 0;
    }
  
}

void GE_Uct1::update_amaf_values()
{
  if(black_values.size()==0)
    {
      black_values.resize(GE_Directions::board_area+1, 0);
      black_sims.resize(GE_Directions::board_area+1, 0);
      white_values.resize(GE_Directions::board_area+1, 0);
      white_sims.resize(GE_Directions::board_area+1, 0);  
    }
  
  
  int point = fg_simu.game.size()-1;
  bool blackWins = (fg_simu.score-komi)>0;
  
  
  // if(blackWins) 
  //     std::cerr<<"1 win for black"<<endl;
  //   else std::cerr<<"1 win for white"<<endl;
  
  if(((blackWins)&&(fg.turn==GE_BLACK_STONE))
     ||((not blackWins)&&(fg.turn==GE_WHITE_STONE)))
    {
      
      nb_wins++;
      
      point = fg_simu.game.size()-1;
      for(int i = 1; i<(int)fg_simu.game.size(); i+=2)
	{
	  const int& big_location = fg_simu.game[i];
	  
	  //cerr<<move_to_string(GE_Directions::board_size, 
	  //GE_Directions::to_move(big_location))<<endl;

	  if(big_location==GE_PASS_MOVE) continue;
	  
	  int location = GE_Bibliotheque::to_goban[big_location];
	  
	  //cerr<<move_to_string(GE_Directions::board_size, location)<<endl;
	  
	  assert(point>=0);
	  black_values[location]+=point;
	  black_sims[location]+=point;
	  point-=2;
	  
	}
      
      
      point = fg_simu.game.size()-2;
      for(int i = 2; i<(int)fg_simu.game.size(); i+=2)
	{
	  const int& big_location = fg_simu.game[i];
	  
	  if(big_location==GE_PASS_MOVE) continue;
	  
	  int location = GE_Bibliotheque::to_goban[big_location];
	  
	  assert(point>=0);
	  white_sims[location]+=point;
	  point-=2;
	} 
    }
  else
    {
      point = fg_simu.game.size()-1;
      for(int i = 1; i<(int)fg_simu.game.size(); i+=2)
	{
	  const int& big_location = fg_simu.game[i];
	  
	  //cerr<<big_location<<endl;

	  if(big_location==GE_PASS_MOVE) continue;
	  
	  int location = GE_Bibliotheque::to_goban[big_location];
	
	  //cerr<<move_to_string(GE_Directions::board_size, location)<<endl;
  
	  assert(point>=0);
	  black_sims[location]+=point;
	  point-=2;
	}
      
      
      point = fg_simu.game.size()-2;
      for(int i = 2; i<(int)fg_simu.game.size(); i+=2)
	{
	  const int& big_location = fg_simu.game[i];
	  
	  if(big_location==GE_PASS_MOVE) continue;
	  
	  int location = GE_Bibliotheque::to_goban[big_location];
	  
	  assert(point>=0);
	  white_values[location]+=point;
	  white_sims[location]+=point;
	  point-=2;
	}     
    }
}  


double GE_Uct1::getEval()
{
  return root_node.getProbaWin();
}


void GE_Uct1::compute(int nb_seq, const GE_Informations* infos)
{
  int num_seq = 0;
  
  to_expertise = false;
  
  if((to_expertise)&&(root_node.isLeaf()))
    {
      current_node = &root_node;
      nodes_sequence.clear();
      nodes_sequence.push_back(&root_node);
      
      current_node->computeLeaf(*this);
      current_node->update(*this, infos);
    }
  
  //init_root_node();
  
  while(true)
    {    
      if((num_seq%1500==0)&&(not root_node.isLeaf()))
	{
	  double eval = -1;
	  int next_move = root_node.bestChoice(*game, &eval);
	  cerr<<"sequence "<<num_seq<<" --> ";
	  cerr<<(int)(root_node.getProbaWin()*10000)/100.f<<"% ";
	  cerr<<"("<<move_to_string(GE_Directions::board_size, next_move);
	  cerr<<" : "<<((int)(eval*10000))/100.f<<"%"<<")";
	  cerr<<endl;
	  best_sequence();
	  next_sequence();
	  preferred_moves(0, 15);
	  cerr<<endl;
	  
	  if(root_node.win_rate>eval)
	    cerr<<"BAD MOVE"<<endl;
	  else cerr<<"GOOD MOVE"<<endl;
	  
	  
	  //root_node.print();
	  //cerr<<endl<<endl;
	}
      
      current_node = &root_node;
      nodes_sequence.clear();
      nodes_sequence.push_back(&root_node);
      
      num_seq++;
      
      playSequence();
      
      if(nb_seq<=0) continue;
      if(num_seq>=nb_seq) break;


    //   cerr<<"root wins: "<<root_node.nb_wins<<" - ";
//       cerr<<"root sims: "<<root_node.nb_sims<<endl;
//       sleep(10);

    }
}



void GE_Uct1::next_sequence(list<int>* l_seq)
{
  GE_Node1* nd = &root_node;
  bool is_allocated = false;


  if(not l_seq)
    {
      is_allocated = true;
      l_seq = new list<int>;
    }
  
  while(not nd->isLeaf())
    {
      int next_move = nd->choice(*this);
      if(next_move==GE_PASS_MOVE) break;

      game->play(next_move);
      nd = &(nd->children[next_move]);
      l_seq->push_back(next_move);
    }
  
  int nb_back = l_seq->size();
  while(nb_back!=0)
    {
      game->backward();
      nb_back--;
    }


  if(is_allocated)
    {
      std::cerr<<"NS: ";
      GE_ListOperators::print_error(*l_seq, move_to_string, GE_Directions::board_size);
      delete l_seq;
    }
}

void GE_Uct1::best_sequence(list<int>* l_seq)
{
  GE_Node1* nd = &root_node;
  bool is_allocated = false;
  
  
  if(not l_seq)
    {
      is_allocated = true;
      l_seq = new list<int>;
    }
  
  while(not nd->isLeaf())
    {
      int next_move = nd->bestChoice(*game);
      if(next_move==GE_PASS_MOVE) break;

      game->play(next_move);
      nd = &(nd->children[next_move]);
      l_seq->push_back(next_move);
    }
  
  int nb_back = l_seq->size();
  while(nb_back!=0)
    {
      game->backward();
      nb_back--;
    }


  if(is_allocated)
    {
      std::cerr<<"BS: ";
      GE_ListOperators::print_error(*l_seq, move_to_string, GE_Directions::board_size);
      delete l_seq;
    }
}


void GE_Uct1::preferred_moves(list<int>* l_pref, int nb_pref)
{
  
  bool is_allocated = false;
  
  if(not l_pref)
    {
      is_allocated = true;
      l_pref = new list<int>;
    }
  
  root_node.preferredMoves(*game, *l_pref, nb_pref);
  if(is_allocated)
    {
      std::cerr<<"PM: ";
      GE_ListOperators::print_error(*l_pref, move_to_string, 
				 GE_Directions::board_size);
      delete l_pref;
    }
}


int GE_Uct1::best_move()
{
  return root_node.bestChoice(*game);
}


void GE_Uct1::compute_expertise(const GE_Informations* infos) const
{
  if(game)
    {
      expertise_go.compute(*game, infos);
    }
}

void GE_Uct1::get_forbidden_moves_by_expertise(list<int>& forbidden, 
					    const GE_Informations* infos) 
  const
{
  assert(game);
  
  expertise_go.compute(*game, infos);

  for(int mv = 0; mv<=GE_Directions::board_area; mv++)
    {
      if((expertise_go.nb_wins)[mv]<(expertise_go.nb_loss)[mv])
	{
	  //cerr<<move_to_string(GE_Directions::board_size, mv);
	  //cerr<<" "<<(expertise_go.nb_wins)[mv];
	  //cerr<<" "<<(expertise_go.nb_loss)[mv]<<endl;
	  
	  forbidden.push_back(mv);
	}
    }
}




/**********************************
 ***                            *** 
 *** Expertise for the tree Uct ***
 ***                            ***  
 **********************************/


void GE_ExpertiseUct1::set(int mv, int val)
{
  int sims = val;
  
  if(val<0)
    {
      sims = -val;
      nb_loss[mv]+=sims;//(int)((sims/100.f)*nb_sims_by_node);
    }
  else
    {
      nb_wins[mv]+=sims; //(int)((sims/100.f)*nb_sims_by_node);
    }
}


int GE_ExpertiseUct1::expertise_dead_stones(int color_stone, 
					 const GE_Informations& infos)
{
  list<int> useless_dead;
  infos.get_useless_moves_by_dead_stones(color_stone, useless_dead); 
  while(not useless_dead.empty())
    {
      int useless_move = useless_dead.front();
      useless_dead.pop_front();
      
      set(useless_move, useless_by_dead_stone);
    }
  
  return useless_by_dead_stone;
}


int GE_ExpertiseUct1::expertise_useless_connexions(int color_stone, 
						const GE_Informations& infos)
{
  list<int> useless_connexions;
  infos.get_useless_connexions(color_stone, useless_connexions); 
  while(not useless_connexions.empty())
    {
      int useless_move = useless_connexions.front();
      useless_connexions.pop_front();
      
      set(useless_move, useless_connexion);
    }
  
  return useless_connexion;
}


int GE_ExpertiseUct1::expertise_bad_connection_tobi(const GE_Game& g, int mv, 
						 int color_stone, int dir)
{
  int big_location = GE_Bibliotheque::to_big_goban[mv];
  
  if(dir==GE_HERE)
    {
      if(GE_Bibliotheque::is_in_border(big_location))
	return 0;
      
      expertise_bad_connection_tobi(g, mv, color_stone, GE_NORTH);
      expertise_bad_connection_tobi(g, mv, color_stone, GE_EAST);
      
      return 0;
    }
  
  int big_neighbour_up; 
  int big_neighbour_down; 
  
  switch(dir)
    {
    case GE_NORTH : 
      big_neighbour_up = big_location+GE_Directions::directions[GE_NORTH]; 
      big_neighbour_down = big_location+GE_Directions::directions[GE_SOUTH]; 
      break;
      
    case GE_EAST :
      big_neighbour_up = big_location+GE_Directions::directions[GE_EAST]; 
      big_neighbour_down = big_location+GE_Directions::directions[GE_WEST]; 
      break;
      
    default :
      assert(0);
    }
  
  int neighbour_up = GE_Bibliotheque::to_goban[big_neighbour_up];
  int neighbour_down = GE_Bibliotheque::to_goban[big_neighbour_down];
  
  if(((g.goban)->get_stone(neighbour_up)!=color_stone)
     ||((g.goban)->get_stone(neighbour_down)!=color_stone))
    {
      return 0;
    }
  
  for(int dir = GE_NORTH; dir<=GE_SOUTHWEST; dir++)
    {
      int big_neighbour = big_location+GE_Directions::directions[dir];
      
      if(big_neighbour==big_neighbour_up) continue;
      if(big_neighbour==big_neighbour_down) continue;
      
      int neighbour = GE_Bibliotheque::to_goban[big_neighbour];
      
      if((g.goban)->get_stone(neighbour)!=GE_WITHOUT_STONE)
	return 0;
    }
  
  set(mv, pattern_bad_connection_tobi);
  
  return pattern_bad_connection_tobi;
}


int GE_ExpertiseUct1::expertise_bad_nobi(const GE_Game& g, int mv, 
				      int color_stone, int dir)
{
  int big_location = GE_Bibliotheque::to_big_goban[mv];
  
  if(dir==GE_HERE)
    {
      if(GE_Bibliotheque::is_in_border(big_location))
	return 0;
      
      expertise_bad_nobi(g, mv, color_stone, GE_NORTH);
      expertise_bad_nobi(g, mv, color_stone, GE_EAST);
      expertise_bad_nobi(g, mv, color_stone, GE_WEST);
      expertise_bad_nobi(g, mv, color_stone, GE_SOUTH);
      
      return 0;
    }
  
  int big_neighbour_up = big_location+GE_Directions::directions[dir]; 
  int neighbour_up = GE_Bibliotheque::to_goban[big_neighbour_up];
  
  if((g.goban)->get_stone(neighbour_up)!=color_stone)
    {
      return 0;
    }
  
  for(int dir = GE_NORTH; dir<=GE_SOUTHWEST; dir++)
    {
      int big_neighbour = big_location+GE_Directions::directions[dir];
      
      if(big_neighbour==big_neighbour_up) continue;
      
      int neighbour = GE_Bibliotheque::to_goban[big_neighbour];
      
      if((g.goban)->get_stone(neighbour)!=GE_WITHOUT_STONE)
	return 0;
    }
  
  set(mv, pattern_bad_nobi);
  
  return pattern_bad_nobi;
}





int GE_ExpertiseUct1::expertise_full_empty_zone(const GE_Game& g, int mv)
{
  int big_location = GE_Bibliotheque::to_big_goban[mv];
  
  if(GE_Bibliotheque::is_in_border(big_location))
    return 0;
  
  for(int dir = GE_NORTH; dir<=GE_SOUTHWEST; dir++)
    {
      int big_neighbour = big_location+GE_Directions::directions[dir];
      int neighbour = GE_Bibliotheque::to_goban[big_neighbour];
      
      if((g.goban)->get_stone(neighbour)!=GE_WITHOUT_STONE)
	return 0;
    }
  
  set(mv, pattern_empty);
  
  return pattern_empty;
}

void GE_ExpertiseUct1::compute_pattern(GE_Game& g, int color_stone)
{
  for(int i = 0; i<GE_Directions::board_area; i++)
    {
      expertise_full_empty_zone(g, i);
      expertise_bad_connection_tobi(g, i, color_stone);
      expertise_bad_nobi(g, i, color_stone);
    }
}



void GE_ExpertiseUct1::compute(GE_Game& g, const GE_Informations* infos)
{
  //assert(0);


  reinit();
  
  
  int color_stone = GE_WITHOUT_STONE;
  
  if(g.tc==GE_BLACK)
    {
      color_stone = GE_BLACK_STONE;
    }
  else
    {
      color_stone = GE_WHITE_STONE;
    }
  
  if(infos)
    {
      expertise_dead_stones(color_stone, *infos);
      expertise_useless_connexions(color_stone, *infos);
    }
  
  compute_pattern(g, color_stone);
}


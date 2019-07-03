#ifndef GE_SEMEAI_H
#define GE_SEMEAI_H

#include <iostream>
using namespace std;

#include "chain.h"


#include "marker.h"

#include "game_go.h"
#include "bibliotheque.h"
#include "fast_goban.h"

#include "const_tactic.h"
#include "location_information.h"


/* struct SequenceKillStone */
/* { */
/*   vector<int> sequence; */
/* }; */

/* struct KillStone */
/* { */
/*   int goal; */
  
/*   list<SequenceKillStone> sequences; */
  
/*   vector<int> killing_moves; */
  
/*   void init(int, const GE_Chain&); */
  
/* }; */


struct GE_Semeai;
struct GE_TreeSemeai;

struct GE_NodeSemeai
{
  int id;
  int index_child;
  int move;
  int prof;
  double expertise;
  

  int result;
  double score;
  
  int wins;
  int wins_with_ko;
  int draws;
  int loss_with_ko;
  int wins_with_seki;
  int loss_with_seki;
  int sims;
  
  int last_result;
  int best_child;
  double best_child_expertise;
  
  bool inhibited;
  
  
  list<int> last_wins;
  int max_last_wins;
  int nb_last_wins;
  int nb_last_loss;
  int nb_last_draw;
  
  const GE_TreeSemeai* tree;
  
  //list<int> chain_atari;
  
  
  vector<GE_NodeSemeai> children;
  GE_NodeSemeai* father;
  
  GE_NodeSemeai();
  
  ~GE_NodeSemeai()
  {
    father = 0;
  }
  
  bool is_inhibited() const 
  {
    return inhibited;
  }
  
  
  bool set_uninhibited() 
  {
    bool old_inhibited = inhibited; 
    inhibited = false;
    
    reinit_last_wins();
    
    return old_inhibited;
  }
  
  void reinit_last_wins()
  {
    last_wins.clear();

    nb_last_wins = 0;
    nb_last_loss = 0;
    nb_last_draw = 0;
    
    int virtual_win = GE_LOSS;
    while((int)last_wins.size()<max_last_wins)
      {
	last_wins.push_back(virtual_win);
	if(virtual_win==GE_WIN) nb_last_wins++;
	if(virtual_win==GE_LOSS) nb_last_loss++;
	if(virtual_win==GE_DRAW) nb_last_draw++;
	virtual_win = -virtual_win;
      }   
  }
  
  void set_result(int res)
  {
    result = res;
  }
  
  void set_score(double sc)
  {
    score = sc;
  }
  
  
  void update(int is_win)
  {  
    last_wins.push_back(-is_win);
    switch(is_win)
      {
      case GE_WIN : nb_last_loss++; break;
      case GE_LOSS : nb_last_wins++; break;
      case GE_DRAW : nb_last_draw++; break;
      case GE_WIN_WITH_KO : nb_last_loss++; break;
      case GE_LOSS_WITH_KO : break;
      case GE_WIN_WITH_SEKI : nb_last_loss++; break;
      case GE_LOSS_WITH_SEKI : break;
      default : ;
      }
    //if(is_win==GE_LOSS) nb_last_wins++; 
    if((int)last_wins.size()>max_last_wins)
      {
	switch(last_wins.front())
	  {
	  case GE_WIN : nb_last_wins--; break;
	  case GE_LOSS : nb_last_loss--; break;
	  case GE_DRAW : nb_last_draw--; break;
	  case GE_LOSS_WITH_KO : nb_last_loss--; break;
	  case GE_WIN_WITH_KO : break;
	  case GE_WIN_WITH_SEKI : break;
	  case GE_LOSS_WITH_SEKI : nb_last_loss--; break;
	  default : ;
	  }
	
	last_wins.pop_front();
      }
    
    if(nb_last_loss==max_last_wins)
      inhibited = true;
    
    assert((int)last_wins.size()==max_last_wins);
    assert(nb_last_wins>=0);
    
    sims++;    
    
    switch(is_win)
      {
      case GE_DRAW : draws++; last_result = GE_DRAW; break;
      case GE_WIN : last_result = GE_WIN; break;
      case GE_LOSS : wins++; last_result = GE_LOSS; break;
      case GE_WIN_WITH_KO : loss_with_ko++; last_result = GE_WIN_WITH_KO; break; 
      case GE_LOSS_WITH_KO : wins_with_ko++; last_result = GE_LOSS_WITH_KO; break;
      case GE_WIN_WITH_SEKI : loss_with_seki++; last_result = GE_WIN_WITH_SEKI; 
	break;
      case GE_LOSS_WITH_SEKI : wins_with_seki++; last_result = GE_LOSS_WITH_SEKI; 
	break;
	
      default : ;
      }
    
    
    if(is_end())
      {
	assert(0);
	return;
      }
    
    if(is_leaf())
      set_result(is_win);
    else
      {
	bool without_result = false;
	int best_result = -GE_NO_RESULT;
	
	//cerr<<"before"<<endl;
	for(int i_child = 0; i_child<(int)children.size(); i_child++) 
	  {
	    
	    int temp_result = (children[i_child]).result;
	    
	    //cerr<<i_child<<" - "<<temp_result<<endl;
	    
	    if(temp_result==GE_LOSS)
	      {
		set_result(GE_WIN);
		//best_result = GE_NO_RESULT;
		break;
	      }
	    
	    if(temp_result==GE_NO_RESULT)
	      {
		without_result = true;
		continue;
	      }
	    
	    if(temp_result<best_result) //c'est pour l'adversaire
	      {
		
		best_result = temp_result;
	      }
	    
	  }
	//cerr<<"after"<<endl;
	
	
	//if(best_result!=GE_NO_RESULT)
	if((not without_result)&&(result==GE_NO_RESULT))
	  {
	    set_result(-best_result); 
	  }
      }
  }



  bool is_leaf() const
  {
    return (children.size()==0);
  }
  
  bool is_end() const
  {
    return (result!=GE_NO_RESULT);
  }


  int choice_child(int* = 0, int = 30) const;
  int get_index_child(int) const;
  
  double evaluate_winning_branch() const;
  double evaluate_exploitation() const; 
  double evaluate_branch() const;
  void update_better(GE_NodeSemeai**, double&, double);
  void choose_in_all_nodes(GE_NodeSemeai**, double&);
  
  void print_sequence() const;
};


struct GE_TreeSemeai
{
  GE_NodeSemeai root_semeai;
  int result;  
  
  int first_player;
  
  int main_goal;

  int prof;
  
  int num_sim;
  int nb_nodes;
  
  GE_NodeSemeai* current_node;
  list<GE_NodeSemeai*> nodes_sequence;
  
  int last_result;
  list<GE_NodeSemeai*> best_sequence;
  
  bool is_updated;
  
  
  const GE_Semeai* semeai;
  GE_Marker mk_semeai;
  list<int> root_interesting_moves;
  list<int> interesting_moves;
  list<int> new_black_stones;
  list<int> new_white_stones;
  bool black_pass;
  bool white_pass;
  bool ko_for_black;
  bool ko_for_white;
  
  vector<vector<int> > black_answers;
  vector<vector<int> > white_answers;
  
  vector<int> black_amaf_wins;
  vector<int> black_amaf_sims;
  vector<int> white_amaf_wins;
  vector<int> white_amaf_sims;


  bool module_ko;
  bool module_seki;
  bool seki_detected;
  
  
  GE_TreeSemeai(const GE_Semeai* = 0, GE_Game* = 0, GE_Informations* = 0); 
  void init(const GE_Semeai* = 0, GE_Game* = 0, GE_Informations* = 0);
  
  int pop_move()
  {
    if(nodes_sequence.empty()) return -1;
    nodes_sequence.back() = 0;
    nodes_sequence.pop_back();
    
    return (int) nodes_sequence.size();
  }
  
  void clear_sequence()
  {
    while(not nodes_sequence.empty())
      {
	pop_move();	
      } 
  }
  

  double choose_in_all_nodes(GE_NodeSemeai**);

  void init_leaf(GE_Game&, const GE_Semeai&);
  int choose_leaf(GE_NodeSemeai**); 
  
  
  private:
  void set_sequence(GE_NodeSemeai&, int = -1);
  void set_game(GE_Game&);
  void init_before_simulation(GE_Game&);
  
  int convert_to_win(GE_Game&, int&) const;
  int convert_to_win_with_ko(GE_Game&, int&) const;
  int convert_to_win_with_seki(GE_Game&, int&) const;
  int convert_to_draw(GE_Game&, int&) const;
  int convert_to_draw_with_ko(GE_Game&, int&) const;
  int convert_to_loss_with_seki(GE_Game&, int&) const;
  int convert_to_loss_with_ko(GE_Game&, int&) const;
  int convert_victory(GE_Game&, int&) const;
  
  int evaluate_end_simulation(GE_Game&, int&) const;
  int launch_simulation(GE_Game&);
  
  void update_tree(GE_Game&, int,  int = -1);
  
  void play_sequence(GE_Game&);
  
  public:
  int compute(GE_Game&, int = -1, int* = 0, bool = false);
  int progress(GE_Game&, int, int* = 0);
  int get_best_move(int&, int&, int&, const GE_NodeSemeai**) const;
  int get_best_move() const;
  int get_simplified_result() const;
  double get_win_rate_amaf(int) const;
  void print_best_move() const;
};


struct GE_SolveurSemeai
{
  bool is_init;
  
  GE_TreeSemeai* precedent_tree;
  GE_TreeSemeai* current_tree;
  GE_TreeSemeai* follow_tree;
  
  GE_TreeSemeai* best_tree;

  int study_pass;
  GE_TreeSemeai after_pass;
  
  GE_TreeSemeai only_win;
  GE_TreeSemeai at_least_win_with_ko;
  GE_TreeSemeai at_least_draw;
  GE_TreeSemeai at_least_draw_with_ko;
  GE_TreeSemeai at_least_loss_with_ko;
  
  GE_SolveurSemeai()
  {
    study_pass = -1;
    is_init = false;
    current_tree = 0;
    best_tree = 0;
    precedent_tree = 0;
    follow_tree = 0;
  }
  
  ~GE_SolveurSemeai()
  {
    current_tree = 0;
    best_tree = 0;
    precedent_tree = 0;
    follow_tree = 0;
  }
  
  
  void init(GE_Semeai&, GE_Game&, GE_Informations*, int = -1);
  void update_precedent_and_follow_trees();
  void update_current_tree(GE_TreeSemeai&);
  void next_tree();
  void update_next_tree();
  int get_result(int&, double&, const GE_TreeSemeai* = 0, int = 1) const;
  int get_current_result(int&, double&) const;
  int get_best_result(int&, double&) const;
  
  bool is_solved() const;
  int get_best_move(int = 30);
  int solve(GE_Game&, int, int, int* = 0, int = 0);
  void print_error() const;
};






struct GE_SemeaiTools 
{
  list<int> black_liberties;
  list<int> black_solid_liberties;
  
  list<int> white_liberties;
  list<int> white_solid_liberties;

  list<int> black_eyes;
  list<int> white_eyes;
};




struct GE_Semeai
{
  int problem; 

  list<int> black_stones;
  list<int> white_stones;

  int main_black_stone;
  int main_white_stone;

  int main_black_size;
  int main_white_size;

  bool mode_big_location;
  
  list<int> black_vital_stones;
  list<int> white_vital_stones;
  
  list<int> black_frontiers;
  list<int> white_frontiers;
  list<int> black_vital_frontiers;
  list<int> white_vital_frontiers;

  
  list<int> black_liberties;
  list<int> white_liberties;
  list<int> black_solid_liberties;
  list<int> white_solid_liberties;
  list<int> black_eyes;
  list<int> white_eyes;
  
  int main_black_nb_liberties;
  int main_white_nb_liberties;
  
  

  GE_Marker marker_all_stones;
  
  //mutable list<int> out_black_moves;
  //mutable list<int> out_white_moves;
  
  mutable list<int> last_black_atari;
  mutable list<int> last_white_atari;
  
  mutable GE_TreeSemeai tree_semeai;
  mutable GE_TreeSemeai after_pass;
  mutable GE_TreeSemeai with_ko;
  
  mutable GE_TreeSemeai only_win;
  mutable GE_TreeSemeai at_least_win_with_ko;
  mutable GE_TreeSemeai at_least_draw;
  mutable GE_TreeSemeai at_least_draw_with_ko;
  mutable GE_TreeSemeai at_least_loss_with_ko;
  
  GE_SolveurSemeai solveur;


  GE_TreeSemeai* main_tree_semeai;


  GE_Semeai();
  
  bool is_a_semeai_stone(const GE_Game&, int) const;
  bool is_a_semeai_stone_fast(int) const;
  bool main_stone_is_vital(int = GE_BLACK_STONE) const;
  
  bool is_a_new_stone_semeai(const GE_Game&, int, list<int>* = 0, 
			     GE_Marker* = 0) const;

  bool is_a_vital_group(const GE_Chain&) const;
  bool is_a_vital_group(const GE_Game&, GE_Informations&, int, int, GE_Marker* = 0, 
			bool = true);
  void init_vital_stones(const GE_Game&, GE_Informations&, GE_Marker* = 0);
  
  void init_frontiers(const GE_Game&, GE_Informations&);

  //bool is_outside(int) const;
  
  void tools_liberties(GE_Game&, GE_SemeaiTools&, int, int) const;
  void tools_liberties(GE_Game&, GE_SemeaiTools&) const;
  void tools_liberties(GE_Game&);

  
  double evaluate_by_answer(const GE_Game&, const GE_TreeSemeai&, int) const;
  double evaluate_by_amaf(const GE_Game&, const GE_TreeSemeai&, int) const;
  double evaluate_by_tree(const GE_Game&, const GE_TreeSemeai&, int) const;
  
  double evaluate_move(GE_Game&, int, const vector<double>&, 
		       const GE_TreeSemeai* = 0) const;
  void eval_all_moves(GE_Game&, const vector<double>&, const list<int>&, 
		      list<pair<int, double> >&, const GE_TreeSemeai* = 0) const;
  
  
  double evaluate_move2(GE_Game&, int, vector<double>&, list<int>&, 
			const GE_TreeSemeai* = 0) const;
  void eval_all_moves2(GE_Game&, vector<double>&, const list<int>&, 
		       list<pair<int, double> >&, 
		       const GE_TreeSemeai* = 0) const;


  int get_best_move(list<pair<int, double> >&) const;
  
  void init_tree(GE_Game&, GE_Informations* = 0);
  void init_all_trees(GE_Game&, GE_Informations* = 0);
  void init_all_trees3(GE_Game&, GE_Informations* = 0);
  
  int eval_semeai() const;
  
  int solve(GE_Game&, GE_Informations*, int, int, int* = 0, int = -1, 
	    int = 0);

  bool is_a_real_semeai(const GE_Game&, GE_Informations&, bool* = 0, bool* = 0);

  void to_mode_big_location();
  
  void print_semeai() const;
  void print_error_semeai(bool = true) const;
};


struct GE_AnalyzeSemeai
{
  list<GE_Semeai> semeais;
  //int max_prof;
  //bool ko;


  static bool to_reinitialize;
  static vector<double> expertise_semeai;
  static double semeai_threat_atari;
  static double semeai_tsuke;
  static double semeai_kata;
  static double semeai_tobi;
  static double semeai_dame;
  //static double semeai_keima;

  static double semeai_liberties;
  static double semeai_solid_liberties;
  static double semeai_eyes;
  
  static double semeai_solid_move;
  
  static double last_move_liberties;
  static double last_move_save_atari;
  static double last_move_blocage;

  static double semeai_atari_too_big;   //(size>2)
  static double semeai_avoid_atari;     //(size=2)
  static double semeai_save_big_atari;

  static double semeai_capture_last_move;
  static double semeai_last_move_avoid_connexion;
  static double semeai_last_move_atari;
  static double semeai_win_race;
  static double semeai_approach_move;
  static double semeai_urgent_approach_move;

  static double semeai_disconnection_double_cut;
  static double semeai_disconnection_tobi;
  static double semeai_connection_tobi;
  static double semeai_blocage;
  
  static double death_kill_eye;
  static double death_kill_semi_eye;
  static double life_make_eye;
  static double life_make_semi_eye;

  static double tree_semeai_very_good_answer;
  static double tree_semeai_good_answer;
  static double tree_semeai_very_bad_answer;
  static double tree_semeai_bad_answer;
  static double tree_semeai_weight_amaf;
  

  //mutable list<int> alive_sequence;


  GE_AnalyzeSemeai() 
  { 
    //max_prof = 1000; 
    //ko = false;
  }

  vector<list<int> > temp_semeais;
  
  void clear()
  {
    temp_semeais.clear();
    semeais.clear();
  }


  void to_mode_big_location();
  
  void setSemeais(const GE_Game&, GE_Marker&, int, GE_Semeai&);
  //void setSemeais(const GE_Game&, GE_Marker&);
  void setSemeais(const GE_Game&, GE_Marker&, GE_Chains&);
  void setSemeais(const GE_Game&, vector<vector<int> >&, int, GE_Chains&);
  
  void setDeathOrLife(GE_Game&, const GE_Informations&);
  
  void initVitalStones(const GE_Game&, GE_Informations&);
  void initFrontiers(const GE_Game&, GE_Informations&);
  void toolsLiberties(GE_Game&);
  
  void correctFalseSemeai(const GE_Game&, GE_Informations&);


  void getInterestingMoves(GE_Game&, const GE_Semeai&, int, GE_Marker&, 
			   list<int>&) const;
  void getInterestingMoves(GE_Game&, const GE_Semeai&, GE_Marker&, 
			   list<int>&) const;
  
  void getInterestingMoves(GE_Game&, const GE_Semeai&, 
			   const GE_Informations&, 
			   int, GE_Marker&, list<int>&) const;
  void getInterestingMoves(GE_Game&, const GE_Semeai&, const GE_Informations&, 
			   GE_Marker&, list<int>&) const;
  
  
  void getInterestingMovesByLastMove(GE_Game&, const GE_Semeai&, GE_Marker&, 
				     list<int>&, list<int>&, 
				     list<int>&) const; 
  
  
  void getNewInterestingMoves(GE_Game&, const GE_Semeai&, const list<int>&, 
			      GE_Marker&, list<int>&) const;
  void getNewInterestingMoves2(GE_Game&, const GE_Semeai&, 
			       const list<int>&, 
			       const list<int>&, 
			       const list<int>&, 
			       GE_Marker&, 
			       list<int>&, 
			       list<int>&, 
			       list<int>&) const;
  void updateNewInterestingMoves2(GE_Game&, const GE_Semeai&, 
				  GE_Marker&, 
				  list<int>&, 
				  list<int>&, 
				  list<int>&) const;
  
  static void getSaveMoves(GE_Game&, const GE_Semeai&, GE_Chain&, list<int>&);
  static bool getSaveMoves(GE_Game&, const GE_Semeai&, list<int>&);  
  
  
  static void initExpertiseSemeai();
  static void computeExpertiseSemeai(GE_Game&, const GE_Semeai&, const list<int>&);
  static void threatAtari(GE_Game&, const GE_Semeai&);
  static void localExpertise(GE_Game&);
  static void avoidDame(GE_Game&, const GE_Semeai&);
  
  static bool getPossibleConnexions(GE_Game&, const GE_Semeai&, const GE_Chain&, 
				    int, list<int>&, int = GE_HERE);
  
  static bool is_move_atari(GE_Game&, int);
  static void captureLastMove(GE_Game&, const GE_Semeai&);
  static bool studyLastMove(GE_Game&, const GE_Semeai&);
  static void maybeWinRace(GE_Game&, const GE_Semeai&);
  static void saveBigAtari(GE_Game&, const GE_Semeai&);
  
  static double disconnection_by_double_cut(GE_Game&, int, int = GE_UNKNOWN);
  static double disconnection_tobi(GE_Game&, int, int = GE_UNKNOWN);
  static double connection_tobi(GE_Game&, int, int = GE_UNKNOWN);
  
  static double blocage(GE_Game&, int, int = GE_UNKNOWN);
  
  static double kill_eye(GE_Game&, int, int = GE_UNKNOWN);
  static double make_eye(GE_Game&, int, int = GE_UNKNOWN);
  static void evaluate_move_by_tsumego_pattern(GE_Game&, const GE_Semeai&, int);
  
  static void evaluate_move_by_pattern(GE_Game&, const GE_Semeai&, int);
  static void evaluate_moves_by_pattern(GE_Game&, const GE_Semeai&, 
					const list<int>&);
  
  static bool isExpertiseBetter(const int, const int);
  void sort(list<int>&) const;
  
  
  //after the move
  double evaluate_end_semeai(GE_Game&, const GE_Semeai&, bool = true) const;
  
  
  bool can_alive(GE_Game&, const GE_Chain&, const list<int>&, int = 4, int* = 0, 
		 GE_Marker* = 0, int = 1) const;
  bool can_sacrifice(GE_Game&, const GE_Chain&, 
		     const GE_Chain&, bool, int* = 0) const;
  bool is_end_by_seki(GE_Game&, const GE_Chain&, bool) const;
  double evaluate_end_death_or_life(GE_Game&, const GE_Semeai&, bool = true, 
				    bool = true) const;
  double evaluate_end_situation(GE_Game&, const GE_Semeai&, bool = true, 
				bool = true) const;


  double study_semeai(GE_Game&, const GE_Semeai&, list<int>* = 0);
  double study_semeai(GE_Game&, const GE_Semeai&, GE_Marker&, list<int>&, 
		      list<int>* = 0, double = -1, int = 1);
  
  void find_a_solution(GE_Game&, const GE_Semeai&, 
		       int (*)(list<int>&, list<int>::iterator&), 
		       list<int>&, int& victory) const;
  
  void moves_for_semeais(GE_Game&, list<int>&) const;
  void moves_for_semeais(GE_Game&, list<int>&, list<int>&) const;
  void moves_for_semeais(GE_Game&, list<pair<int, double> >&) const;
  
  
  void study_semeais(GE_Game&, int = -1, GE_Informations* = 0);
  int study_best_semeais(GE_Game& g, int max_simus, GE_Informations* infos, 
			 GE_Semeai** best_semeai);

				     
  int study_semeais2(GE_Game&, int = 1000, GE_Informations* = 0);
  int study_semeais3(GE_Game&, int = 1000, GE_Informations* = 0);
  
  int solve_semeais(GE_Game&, int, int, GE_Informations* = 0, int = -1, int = 0);
  
  int get_complexity(GE_Game&, const GE_Semeai&, 
		     const GE_Informations* = 0) const;
  
  void print_semeais() const;
  void print_error_semeais() const;
  void print_error_moves_for_semeai(GE_Game&, bool = true) const;
  void print_error_moves_for_semeai_and_a_solution(GE_Game&, bool = true) const;
};



#endif














/*
  void update(int is_win)
  {
  sims++;
    
    
  switch(is_win)
  {
  case 0 : draws++; last_result = 0; break;
  case 1 : wins++; last_result = 1; break;
  case -1 : last_result = -1; break;
  default : ;
  }

    
  if(is_end())
  {
  //assert(0);
  return;
  }
    
  if(is_leaf())
  set_result(is_win);
  else
  {
  bool without_result = false;
  int best_result = GE_NO_RESULT;
	
  cerr<<"before"<<endl;
  for(int i_child = 0; i_child<(int)children.size(); i_child++) 
  {
	   
  int temp_result = (children[i_child]).result;
	    
  cerr<<i_child<<" - "<<temp_result<<endl;

  if(temp_result==1)
  {
  set_result(-1);
  //best_result = GE_NO_RESULT;
  break;
  }
	    
  if(temp_result==GE_NO_RESULT)
  {
  without_result = true;
  continue;
  }
	    
  if(temp_result>best_result)
  {
		
  best_result = temp_result;
  }
	    
  }
  cerr<<"after"<<endl;

	
  //if(best_result!=GE_NO_RESULT)
  if((not without_result)&&(result==GE_NO_RESULT))
  {
  set_result(-best_result); 
  }
  }
  }
*/
  

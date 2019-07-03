#ifndef GE_UCT1_H
#define GE_UCT1_H

#include <assert.h>
#include <time.h>

#include "const_rule.h"
#include "fast_goban.h"
#include "game_go.h"
#include "tools_ge.h"
#include "fast_mc_locations_informations.h"
#include "bibliotheque.h"

#include "location_information.h"

struct GE_Uct1;

struct GE_ExpertiseUct1
{
  int nb_sims_by_node;
  
  vector<int> nb_wins;
  vector<int> nb_loss;
  
  static int useless_by_dead_stone;
  static int useless_connexion;
  
  static int pattern_empty;
  static int pattern_bad_connection_tobi;
  static int pattern_bad_nobi;
  

  GE_ExpertiseUct1(int nb_sims = 1)
  {
    nb_sims_by_node = nb_sims; 
    nb_wins.resize(GE_Directions::board_area+1, 0);
    nb_loss.resize(GE_Directions::board_area+1, 0);
  }
  
  int set_sims_by_node(int nb_sims = 1)
  {
    int old_value = nb_sims_by_node;
    nb_sims_by_node = nb_sims; 
    return old_value;
  }
  
  void reinit()
  {
    if(nb_wins.size()==0)
      nb_wins.resize(GE_Directions::board_area+1, 0);
    
    if(nb_loss.size()==0)
      nb_loss.resize(GE_Directions::board_area+1, 0);
    
    
    for(int i = 0; i<(int)nb_wins.size(); i++)
      {
	nb_wins[i] = 0;
      }
    
    for(int i = 0; i<(int)nb_loss.size(); i++)
      {
	nb_loss[i] = 0;
      }
  }
  
  void set(int, int);
  
  int expertise_dead_stones(int, const GE_Informations&);
  int expertise_useless_connexions(int, const GE_Informations&);
  
  int expertise_full_empty_zone(const GE_Game&, int);
  int expertise_bad_connection_tobi(const GE_Game&, int, int, int = GE_HERE);
  int expertise_bad_nobi(const GE_Game&, int, int, int = GE_HERE);
  
  void compute_pattern(GE_Game&, int);

  void compute(GE_Game&, const GE_Informations* = 0);
  
};



struct GE_Node1
{
  int move;
  
  vector<double> amaf_wins;
  vector<double> amaf_sim;
  
  //bool is_black_player;
  
  //vector<double> amaf_points;
  //int max_points;
  //int min_points;
  
  
  double win_rate;
  int nb_wins;
  int nb_sims;
  
  int virtual_wins;
  int virtual_loss;
  
  int nb_stones;
  
  
  vector<GE_Node1> children;
  
  
  
  GE_Node1(int = GE_ILLEGAL_MOVE);
  int set_move(int);
  void init(const GE_Uct1&, const GE_Informations* = 0);
  
  bool isLeaf() const;
  bool is_already_simulated() const;
  double getProbaWin();

  void computeLeaf(GE_Uct1&);
  void computeLeafByUCT(GE_Uct1&);
  double getNbChildrenNoLeaf();
  double getExploitation(int);
  double getExploration(); 
  double getExploration(int);
  double evalChildNode(double, int, const GE_Uct1&);
  double getCandidates(GE_Uct1&, list<int>&);
  int choice(GE_Uct1&, double* = 0);
  int fastChoice(GE_Uct1&, double* = 0);
  double getBestCandidates(GE_Game&, list<int>&);
  int bestChoice(GE_Game&, double* = 0);
  void preferredMoves(GE_Game&, list<int>&, int = 5);
  void update(GE_Uct1&, const GE_Informations* = 0);
  void print();// const;
};


struct GE_Uct1
{
  GE_Node1 root_node;
  GE_Goban root_goban;
  
  GE_Node1* current_node;
  list<GE_Node1*> nodes_sequence;
  
  
  //GE_FastMCLocationsInformations fmcli;
  
  GE_Game* game;
  GE_FastGoban fg;
  GE_FastGoban fg_simu;

  float komi;
  
  int max_simulations;
  int min_capture;
  
  double coeff_exploration;
  
  
  //int prof;

  vector<double> black_values;
  vector<double> black_sims;
  vector<double> white_values;
  vector<double> white_sims;
  int nb_wins;
  int nb_sims;

  bool is_sub_uct;

  
  bool to_expertise;
  mutable GE_ExpertiseUct1 expertise_go;

  list<int> forbidden_moves;

 
  GE_Uct1();
  ~GE_Uct1() {}
  void init(GE_Game&, float = GE_KOMI, int = 1, int = 4, double = 1);
  void init_root_node();
  
  double evalNode();
  void playSequence();
  double evalLeaf();
  void clear_amaf_values();
  void update_amaf_values();
  void compute(int = 10000, const GE_Informations* = 0);

  double getEval();
  
  void next_sequence(list<int>* = 0);
  void best_sequence(list<int>* = 0);
  void preferred_moves(list<int>* = 0, int = 10);
  int best_move();

  void compute_expertise(const GE_Informations* = 0) const;
  
  void get_forbidden_moves_by_expertise(list<int>&, 
					const GE_Informations* = 0) const;
  
};




#endif

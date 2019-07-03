#ifndef GE_INTERSECTION_INFORMATION_H
#define GE_INTERSECTION_INFORMATION_H

#include <vector>

#include "const_tools_ge.h"
#include "tactic.h"
#include "marker.h"
#include "bibliotheque.h"
#include "fast_goban.h"

struct GE_GroupInformation;
struct GE_IntersectionInformation
{
  int intersection;
  int possible_white_eye;
  int possible_black_eye;
  int is_alive;
  
  GE_IntersectionInformation(int = GE_PASS_MOVE);
  
  void init_possible_eye(GE_Game&);
  void refine_possible_eye(GE_Game&, 
			   const vector<GE_GroupInformation*>&, 
			   const vector<int>&);
  bool is_alive_by_pass(GE_Game&, int, int = 1, bool = false);
  bool is_fit_for_slaughter(GE_Game&, int, int = 1); //est tuable
  bool is_dead(GE_Game&, int);

  void print_error() const;

};


struct GE_GroupInformation
{
  int index;
  GE_Chain ch;
  //list<int> virtually_connected;
  //list<int> seki;
  
  int status;
  int motif_alive;
  int motif_dead;
  list<int> save_moves;
  list<int> kill_moves;


  int mobility;
  

  int nb_holes;
  int nb_eyes;
  list<int> false_eyes;
  list<pair<int, int> > possible_connexions;
  list<int> false_wall_connexions;
  list<int> tactic_connexions;
  list<int> virtual_connexions;
  list<int> solid_virtual_connexions;
  
  double stats_life;
  
  vector<int> neighbour_holes;
  vector<int> neighbour_groups;
  
  GE_GroupInformation(int, const GE_Chain&);
  
  //void init_virtually_connected();
  //void init_seki();
 
  int get_one_stone() const;
  int get_size() const;
  int get_color_stone() const;
  void append_neighbour_hole(int);
  void append_neighbour_group(int);
  void append_false_eye(int);
  void append_possible_connexion(int, int);
  void append_false_wall_connexion(int);
  void append_tactic_connexion(int);
  void append_virtual_connexion(int);
  void append_solid_virtual_connexion(int);
  
  void get_all_connexions(vector<int>&, int = GE_POSSIBLE_CONNEXION) const;
 
  double set_stats_life(double);
  
  
  //int set_alive(int);
  
  int set_status(int);
  int get_status(int = -1) const;
  int set_motif_status(int);
  
  void init_mobility(GE_Game&);
  
  bool study_alive(GE_Game&);
  bool study_alive_by_stats();
  
  void print_error() const;
  
};

struct GE_HoleInformation
{
  int index;
  list<int> hole;
  list<int> black_chains;
  list<int> white_chains;


  int type;
  
  GE_HoleInformation() { index = -1; }
  ~GE_HoleInformation() {}
  
  void init(const GE_Game&, const GE_Chain&, 
	    const vector<GE_GroupInformation*>&, 
	    const vector<int>&, int = 1);
  void init_type_hole();
  int set_type_hole(int);
  int get_color_false_eye() const;
  

  void print_error() const;
};



struct GE_GroupsNetwork
{
  int index;
  vector<int> groups;
  int level_connexion;
  list<int> possible_eyes;
  
  
  GE_GroupsNetwork() { index = -1; level_connexion = GE_NO_CONNEXION; }
  ~GE_GroupsNetwork() {}
  
  
  void init(const GE_GroupInformation&, const vector<GE_GroupInformation*>&, 
	    int, int, GE_Marker&);
  void init(const GE_GroupInformation&, const vector<GE_GroupInformation*>&, 
	    GE_Marker&, int);
 
  void print_error();
};



struct GE_Informations
{
  vector<vector<bool> > mk_virtual_connected;

  GE_Game* game;
  
  vector<GE_IntersectionInformation*> intersections;
  
  vector<GE_GroupInformation*> groups;
  vector<int> to_num_group;
  
  vector<GE_HoleInformation*> holes;
  
  vector<GE_GroupsNetwork*> networks;
  

  GE_Informations();
  GE_Informations(int);
  GE_Informations(GE_Game&);
  ~GE_Informations();

  int get_num_group(int, int) const;
  int get_num_hole(int) const;
  //get the index of the group if it is stone
  //or get the index of the hole if it is an empty location
  int get_index(const GE_Game&, int) const;
  int get_network(const GE_Game&, int) const;
  
  
  void get_dead_stones(GE_Game&, list<int>&);
  void erase_group(int);
  
  void get_dead_stones_by_simulation(GE_Game&, list<int>&);
  
  
  
  void init_holes(const GE_Game&);
  void init_neighbour_groups(const GE_Game&);
  
  int get_nb_holes();
  
  private:
  void init_type_false_eye(GE_HoleInformation&);
  void init_type_false_eye(const GE_GroupInformation&);
  public:
  void init_type_holes();
  
  void erase_possible_connection(GE_GroupInformation&, GE_GroupInformation&);
  bool init_virtual_connection(GE_GroupInformation&, GE_GroupInformation&, 
			       list<int>* = 0);
  void init_virtual_connections(const GE_HoleInformation&);
  void init_false_wall_connections(const GE_GroupInformation&, 
				   vector<vector<bool> >&);
  void init_false_wall_connections();
  void init_virtual_connections();
  
  void init_stats_group(vector<int>&, int, team_color = GE_NEUTRAL_COLOR);
  void study_alive(GE_Game&); 
  void study_alive_by_stats();
  void find_unkillable();
  void init_alive_with_unkillable_group(const GE_GroupInformation&);
  void init_alive_with_unkillable_group();
  bool init_alive_with_alive_group(const GE_GroupInformation&);
  void init_alive_with_alive_group();
  void init_alive_with_possible_connexion(GE_GroupInformation&);
  void init_alive_with_possible_connexion();
  void init_alive_by_direct_connexion_two_eyes(GE_GroupInformation&);
  void init_alive_by_direct_connexion_two_eyes();
  bool init_alive_by_connexion_two_eyes(GE_GroupInformation&, int&, GE_Marker&);
  void init_alive_by_connexion_two_eyes();
  void init_alive_by_connexion_different_one_eyes(GE_GroupInformation&);
  void init_alive_by_connexion_different_one_eyes();


  bool all_groups_stationary_by_kill(const GE_HoleInformation&);
  bool is_perhaps_seki(const GE_GroupInformation&);
  void find_sekis();
  static bool is_mobile(GE_Game&, const GE_Chain&);
  int count_eye(const GE_GroupInformation&, list<int>* = 0);
  int set_nb_eyes(GE_GroupInformation&, int = -1);  
  void init_eyes();
  void init_false_eyes();
  void init_group_mobility(GE_Game&);
  
  bool is_really_connexion(GE_Game&, GE_GroupInformation&, GE_GroupInformation&, int, 
			   int* = 0);
  void get_groups_network(const GE_GroupInformation&, 
			  vector<int>&, 
			  GE_Marker&, 
			  int = GE_TACTIC_CONNEXION);
  void init_groups_networks(int = GE_TACTIC_CONNEXION);

  

  void refine_possible_eyes();
  
  void get_near_possible_eyes(GE_GroupInformation&, GE_Marker&, list<int>&);
  void get_near_possible_eyes(GE_GroupsNetwork&, GE_Marker&, list<int>&);
  
  void init_possible_eyes(GE_GroupsNetwork&, GE_Marker&);
  void init_possible_eyes();
  
  void get_dead_stones(list<int>&) const;
  void get_useless_moves_by_dead_stones(int, list<int>&) const;
  void get_useless_connexions(int, list<int>&) const;

  void print_error() const;
};


#endif

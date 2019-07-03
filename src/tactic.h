#ifndef GE_TACTIC_H
#define GE_TACTIC_H

#include "game_go.h"
#include "chain.h"
#include "const_geography.h"
#include "amaf.h"
#include "simulation.h"
#include <assert.h>
#include "fast_goban.h"

#include "const_tactic.h"

#define GE_TYPE_THREATEN_CAPTURE 1

/* struct Threaten */
/* { */
/*   int type; */
/*   int mv; */
/*   int val; */
/*   list<pair<int, int> > answers; */

/*   Threaten(int, int, int); */

  
/* }; */

/* struct ListThreatens */
/* { */
/*   list<Threaten> threatens; */

/*   buildThreatens(int); */
    
/*   buildThreatenCapture */

/* }; */

struct GE_Tactic
{
  static int max_depth;
  mutable list<int> sequence_shisho;
  
  mutable bool ko_for_attack;  //implemented in the shisho 
  mutable bool ko_for_defense; //not already used 
  //(not implemented in the shisho)


  GE_Tactic() { ko_for_attack = false; ko_for_defense = false; }
  ~GE_Tactic() {};

  void eat_or_to_be_eaten(GE_Game&, GE_Chains&, list<pair<int, int> >&, 
			  int = -1, int = -1) const;
  void flee0(GE_Game&, GE_Chains&, list<pair<int, int> >&, int = -1) const;
  void flee1(GE_Game&, GE_Chains&, list<pair<int, int> >&, int = -1, bool = false, bool = false) 
    const;
  void threat0(GE_Game&, GE_Chain&, list<pair<int, int> >&, bool = false) const;
  void attacks_double_atari0(GE_Game&, int, GE_Chain&, list<int>&, bool = false) const;
  void attacks_two_groups0(GE_Game&, int, GE_Chain&, list<int>&, int = 1, bool = false) const;
  void reduce_liberties0(GE_Game&, GE_Chain&, list<pair<int, int> >&, 
			 int = -1, bool = false);
  void attacks_by_reducing_liberties0(GE_Game&, GE_Chain&, GE_Chain&, 
				      list<pair<int, int> >&, bool = false);
  void connect0(GE_Game&, pair<int, int>&, int, list<int>&, bool = false) const;
  void connect0(GE_Game&, int, list<int>&, bool = false) const;
  void connect_if_dead_virtual_connection0(GE_Game&, int, list<pair<int, int> >&, 
					   bool = false);
  void increase_liberties0(GE_Game&, pair<int, int>&, int, list<int>&, bool = false) const;
  void increase_liberties0(GE_Game&, int, list<int>&, bool = false) const;

  void prevent_connection0(GE_Game&, int, pair<int, int>&, int, list<pair<int, int> >&, 
			   int = -1, GE_Chain* = 0, bool = false) const;
  void prevent_connection0(GE_Game&, int, list<pair<int, int> >&, int = -1, GE_Chain* = 0, 
			   bool = false) const;

  void one_point_jump0(GE_Game&, const pair<int, int>&, int, list<int>&, bool = false) const;
  void one_point_jump0(GE_Game&, int, list<int>&, bool = false) const;
  

  /*  void kill_group(); */
  /*   void save_group(); */
  /*   void eat_or_to_be_eaten(); */
  /*   void eat(); */
  /*   void atari(); */
  /*   void double_atari(); */
  /*   void make_eye(); */
  
  static bool study_shisho_after_move(GE_Game&, int, list<int>* = 0);
  static bool study_shisho(GE_Game&, int, list<int>* = 0);
  static bool study_shisho(GE_Game&, int, GE_Chain&, list<int>* = 0);

  
  
  /******************
   * Basic instinct *
   ******************/
  
  void peep_connect0(GE_Game&, pair<int, int>&, int, list<int>&, bool = false);
  void peep_connect0(GE_Game&, int, list<int>&, bool = false);
  void tsuke_hane0(GE_Game&, const pair<int, int>&, int, list<int>&, bool = false) const;
  void tsuke_hane0(GE_Game&, int, list<int>&, bool = false) const;
  void kosumi_tsuke_stretch0(GE_Game&, const pair<int, int>&, int, bool, list<int>&, 
			     bool = false) const;
  void kosumi_tsuke_stretch0(GE_Game&, int, list<int>&, bool = false) const;
  void thrust_block0(GE_Game&, const pair<int, int>&, int, list<int>&, bool = false) const;
  void thrust_block0(GE_Game&, int, list<int>&, bool = false) const;
  


  bool connect_kosumi(GE_Game&, const pair<int, int>&, int, 
		      int = GE_BLACK_STONE) const;
  bool useless_connect_kosumi(GE_Game&, const pair<int, int>&, 
			      int, int, int&) const;
  
  
  bool connect_tobi(GE_Game&, const pair<int, int>&, int, 
		    int = GE_BLACK_STONE) const;
  bool useless_connect_tobi(GE_Game&, const pair<int, int>&, 
			    int, int, int&) const;
  

  void motif_to_string(int, string&) const; 
  static void death_to_string(int, string&);
  static void hole_to_string(int, string&);
  static void mobility_to_string(int, string&);
  static void connect_to_string(int, string&);
  static void result_to_string(int, string&);
  static void problem_to_string(int, string&);

  
  bool study_shisho_after_move2(GE_Game&, int, list<int>* = 0) const;
  bool study_shisho2(GE_Game&, int, list<int>* = 0) const;
  void prepare_new_defenses_for_shisho(const GE_Game&, const list<int>&, 
				       int, list<int>&, int = -1) const;
  bool study_shisho2(GE_Game&, int, GE_Chain&, list<int>*, const list<int>&, 
		     int = 1) const;

};


bool append_move(GE_Game&, int, int, list<int>&);
void get_moves_by_random(GE_Game&, list<int>&, int = 1);
void get_moves_by_shape(GE_Game&, int, list<int>&, int = GE_CONNEXITY_4);
void get_moves_by_amaf0(GE_Game&, list<int>&, int = 100, int = 1);


#endif

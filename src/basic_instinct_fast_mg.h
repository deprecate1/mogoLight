//
// C++ Implementation: basic_instinct_fast_mg
//
// Description:
//
//
// Author: Hoock Jean-Baptiste, Olivier Teytaud, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//



#ifndef BASIC_INSTINCT_FAST_MG_H
#define BASIC_INSTINCT_FAST_MG_H



/*
  in this class, we find some basic concepts of strategy (or tactics).
  
  You should find more information at this page 
  http://senseis.xmp.net/?BasicInstinct
  
*/

#include <iostream>
using namespace std;

#include <list>


#include "fastboard_mg.h"
#include "goban_mg.h"
#include "gobannode_mg.h"
#include "typedefs_mg.h"
#include "tools.h"
#ifdef GOLDENEYE
#include "semeai.h"
#include "bibliotheque.h"
#include "all_include.h"
#endif

#include "pattern.h"

//#define TOO_FREE 


namespace MoGo 
{
  class BasicInstinctFast
  {
#ifdef AUTOTUNE
    static char fileName[500];
    static FILE *file;
#endif
#ifdef GOLDENEYE
    static pthread_mutex_t mutexComputeSlow;
#endif

    static bool debug_peep_connect;
    static bool debug_atari_extend;
    static bool debug_tsuke_hane;
    static bool debug_thrust_block;
    static bool debug_kosumi_tsuke_stretch;
    static bool debug_bump_stretch;
    
    static bool debug_eat;
    static bool debug_threat;
    static bool debug_eat_or_to_be_eaten;
    static bool debug_connect;
    static bool debug_connect_if_dead_virtual_connection;
    static bool debug_reduce_liberties;
    static bool debug_increase_liberties;
    static bool debug_prevent_connection;
    static bool debug_one_point_jump;
    static bool debug_kogeima;

    static bool debug_wall;
    static bool debug_kosumi;
    static bool debug_kata;
    
    static bool debug_extend;
    static bool debug_cross_cut;

    static bool debug_bad_tobi;
    static bool debug_bad_kogeima;
    static bool debug_bad_elephant;
    static bool debug_bad_empty_triangle;
    static bool debug_bad_hazama_tobi;

    static bool debug_avoid_border;
    
    static bool debug_too_free;
    
    static bool debug_demi_noeud_bambou;
    
    static double bonus_line_influence;
    static double bonus_line_territory;
    static double malus_line_defeat;
    static double malus_line_death;


    static double bonus_peep_connect;
    static double bonus_atari_extend;
    static double bonus_tsuke_hane;
    static double bonus_thrust_block;
    static double bonus_kosumi_tsuke_stretch;
    static double bonus_kosumi_tsuke_stretch2; 
    //when I need sabaki (cf senseis: basicInstinct)
    static double bonus_bump_stretch;

    static double bonus_eat;
    static double bonus_threat;
    static double bonus_eat_or_to_be_eaten;
    static double bonus_connect;
    static double bonus_connect_if_dead_virtual_connection;
    static double bonus_reduce_liberties;
    static double bonus_increase_liberties;
    static double bonus_prevent_connection;
    static double bonus_one_point_jump;
    static double bonus_kogeima;
				
    static double bonus_wall;
    static double bonus_kosumi;
    static double bonus_kata;
    
    static double bonus_extend;
    static double bonus_cross_cut;

    static double malus_bad_tobi;
    static double malus_bad_kogeima;
    static double malus_bad_elephant;
    static double malus_bad_empty_triangle;
    static double malus_bad_hazama_tobi;

    static double malus_alone_border;

    static double malus_too_free;
    static int param_too_free;

    static double malus_border;
    static double bonus_if_last_border;
    static double bonus_if_last_corner;

    static double bonus_demi_noeud_bambou;
    
    static double stats_distances[19][19];
    static double weight_distance;

    static double stats_distances2[19][19];
    static double weight_distance2;
    

#ifdef TEST_RANDOM_PATTERN
    static GE_Pattern test_pattern; 
#endif    


#ifdef RANDOM_PATTERN
    static vector<list<GE_Pattern> > random_patterns;
#endif
    
#ifdef MODULE_RANDOM_PATTERN
    vector<int> v_goban; //it used for test_pattern and random patterns
#endif


#ifdef GUILLAUME
    static double GCoeff;
    static int uninitialized;
#endif
#ifdef AUTOTUNE
	  public:
    static double configValue(char *s);
	  private:
#endif

#ifdef GOLDENEYE
    FastBoard* fastboard;
#else
    const FastBoard* fastboard;
#endif
    Location last_move;
    Location before_last_move;
    
  public:
#ifdef AUTOTUNE
    static int fileNumber;  
#endif
    
    BasicInstinctFast() {}
#ifdef GOLDENEYE
    BasicInstinctFast(FastBoard* fastboard, const Location before_last_move) 
#else
    BasicInstinctFast(const FastBoard* fastboard, const Location before_last_move) 
#endif
      { 
	this->fastboard = fastboard; 
	this->last_move = fastboard->lastMove();
	this->before_last_move = before_last_move;
      }
    
#ifdef GOLDENEYE
    BasicInstinctFast(FastBoard& fastboard, const Location before_last_move) 
#else
    BasicInstinctFast(const FastBoard& fastboard, const Location before_last_move) 
#endif
      { 
	this->fastboard = &fastboard;
	this->last_move = fastboard.lastMove();
	this->before_last_move = before_last_move;
      }

    ~BasicInstinctFast() {}
    
    //Location set_before_last_move(Location blm)
    //{
    // Location old_blm = before_last_move;
    //  before_last_move = blm;
    //  return old_blm;
    //}
    
    void init0(double**) const;
    void init_local0(double**) const;
    void init_by_distance_before_last_move0(double**) const;


    bool peep_connect0(int, double** = 0, int* = 0) const;
    void peep_connect0(double** = 0) const;
    
    bool atari_extend0(int, double** = 0, Location* = 0) const;
    void atari_extend0(double** = 0) const;
    
    bool atari_extend1(int, double** = 0, Location* = 0) const;
    void atari_extend1(double** = 0) const;

    bool eat0(double**, Location* = 0) const;
    bool threat0(double**, Location* = 0, Location* = 0) const;
    bool reduce_liberties0(double**, int = 3) const;

    bool increase_liberties0(int, double** = 0, Location* = 0) const;
    void increase_liberties0(double** = 0) const;
    
    bool eat_or_to_be_eaten0(int, double** = 0, Location* = 0) const;
    void eat_or_to_be_eaten0(double** = 0) const;
    
    bool connect0(int, double** = 0, Location* = 0) const;
    void connect0(double** = 0) const;

    
    bool connect_if_dead_virtual_connection0(list<Location>&, list<Location>&,
					     double** = 0, Location* = 0) const;
    void connect_if_dead_virtual_connection0(double** = 0) const; 


    bool prevent_connection0(int, double** = 0, Location* = 0) const;
    void prevent_connection0(double** = 0) const;

    bool one_point_jump0(int, double** = 0, Location* = 0) const;
    void one_point_jump0(double** = 0) const;

    bool kogeima0(int, double** = 0, Location* = 0, Location* = 0) const;
    void kogeima0(double** = 0) const;

    bool tsuke_hane0(int, double** = 0, int* = 0, int* = 0) const;
    void tsuke_hane0(double** = 0) const;
    
    
    bool kosumi_tsuke_stretch0(int, double** = 0, int* = 0, int* = 0, int* = 0, int* = 0) const;
    void kosumi_tsuke_stretch0(double** = 0) const;
    
    bool bump_stretch0(int, double** = 0, int* = 0, int* = 0) const;
    void bump_stretch0(double** = 0) const;
    
    
    bool wall0(double** = 0, int* = 0, int* = 0) const;
    bool wall1(int, double** = 0, int* = 0, int* = 0) const;
    void wall1(double** = 0) const;

    bool kosumi0(int, double** = 0, int* = 0) const;
    void kosumi0( double** = 0) const;
    
    bool kata0(int, double** = 0, int* = 0) const;
    void kata0(double** = 0) const;

    bool extend0(int, double** = 0, int* = 0, int* = 0) const;
    void extend0(double** = 0) const;

    bool cross_cut0(int, double** = 0, int* = 0) const;
    void cross_cut0(double** = 0) const;

    bool bad_tobi0(int, double** = 0, int* = 0, int* = 0) const; 
    void bad_tobi0(double** = 0) const;
    
    bool bad_kogeima0(int, double** = 0, int* = 0, int* = 0) const; 
    void bad_kogeima0(double** = 0) const;

    bool bad_elephant0(int, double** = 0, int* = 0) const; 
    void bad_elephant0(double** = 0) const;
    
    bool bad_empty_triangle0(int, double** = 0, list<int>* = 0) const; 
    void bad_empty_triangle0(double** = 0) const;

    bool bad_hazama_tobi0(int, double** = 0, int* = 0) const; 
    void bad_hazama_tobi0(double** = 0) const;

    void alone_border0(int, double** = 0) const;
    void alone_border0(double** = 0) const;

    void avoid_border0(double** = 0, int* = 0, int* = 0) const;
    void avoid_border1(double** = 0, int* = 0, int* = 0) const;

    void too_free0(double** = 0, int = 4) const;
    void too_free1(int, int, double** = 0, int = 3) const;
    void too_free1(int, double** = 0, int = 3) const;
    void too_free2(int, int, double** = 0, int = 4) const;
    void too_free2(int, double** = 0, int = 4) const;

    
    void demi_noeud_bambou0(int, int, double** = 0) const;
    void demi_noeud_bambou0(int, double** = 0) const;
    

#ifdef RANDOM_PATTERN
    static bool load_random_patterns(const char*, int = NUM_THREADS_MAX);
#endif    


    void compute(double**) const;
    inline void compute(double*) const;
#ifdef GUILLAUME
void computeGuillaume(double** basic_instinct, int color) const;
void computeGuillaume13(double** basic_instinct, int color) const;
#endif

    void compute(double**, int);
#ifdef GOLDENEYE
    static void computeSlow(FastBoard * , int ,vector<int>*guessLocation, vector<int>*guessColor,vector<double>*guessWeight,GobanNode * node);
#endif
    void compute(double**, int, int);

    inline void compute(double*, int);

  };
}

void MoGo::BasicInstinctFast::compute(double* basic_instinct) const
{
  this->compute(&basic_instinct);
}

void MoGo::BasicInstinctFast::compute(double* basic_instinct, int color) 
{
  this->compute(&basic_instinct,color);
}

#endif

#ifndef GE_FAST_GOBAN_H
#define GE_FAST_GOBAN_H

#include <iostream>
using namespace std;

#include <vector>

#include <assert.h>

#include "const_goban.h"
#include "goban.h"
#include "const_geography.h"
#include "evaluation_score.h"
#include "game_go.h"
#include "chain.h"
#include "bibliotheque.h"


struct GE_FastGoban
{
  static int stop;



  vector<int> game;
  
  vector<int> goban;
  vector<int> possible_moves;
  vector<int> chains;
  vector<int> id_chains;
  int nb_chains;


  int turn;
  int nb_choice;
  int ko;
  
  int score;
  
  bool place_stone(int, int, int);
  void merge_chains(int, int);
  
  bool is_surrounded4(int, int);
  bool is_dead0(int, int);
  bool is_dead1(int, int&);
  int clean_dead_chain(int, int);
  int play(int);
  
  int evaluate_score();
  int launch_simulation(int = 4);
  

  bool is_eye(int, int, int, int, int);
  bool avoid_play_eye(int, int);
  
  void init();
  void clear(int = GE_BLACK_STONE);
  void reinit(const GE_Goban&);
  
  void init_chains(const GE_Goban&);
  void init_possible_moves();

  void toGoban(GE_Goban&) const;
  void fromGame(const GE_Game&);

  void copy(const GE_FastGoban&);

  void print_goban(int = GE_PASS_MOVE) const;
  void print_chain() const;

  void print_error_goban(int = GE_PASS_MOVE) const;


  bool isAlone4(int, int = GE_WITHOUT_STONE) const;
};


/*
  on ne peut pas rejouer sur une location capturée si la chaine capturée est de longueur 
  supérieure ou égale à 4.
*/



/* struct FastChain */
/* { */
/*   Vector<int> next; */
/*   //Vector<int> id; */

 /*   void update(int); */

/* }; */

/* void FastChain::update(int location) */
/* { */
/*   next */
/* } */


/* struct GE_FastGoban */
/* { */
/*   Vector<int> goban; */
/*   Vector<int> free_locations; */
/*   FastChain chains; */

/*   void update(int); */

/* }; */

#endif

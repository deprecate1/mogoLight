#ifndef GE_CHAIN_H
#define GE_CHAIN_H

#include "goban.h"
#include "const_geography.h"
#include "tools_ge.h"
#include "move.h"
#include "bibliotheque.h"
#include "marker.h"

#include <cmath>

struct GE_Chain
{
  list<int> links;
  list<int> free_intersections;
  int liberties;
  int stone;       //the color of stone
  static int height;
  
  GE_Chain() {}
  ~GE_Chain() {}

  void init(const GE_Goban&, int***, int, int);
  void init(const GE_Goban&, int);
  void build_chain(const GE_Goban&, int***, int, int, int = GE_HERE);

  bool is_atari() const;
  bool is_dead() const;

  bool is_virtually_connected(const GE_Chain&, list<int>* = 0, bool = false) 
    const;
  int get_connexions(const GE_Chain&, list<int>&) const;
  int get_one_connexion(const GE_Chain&) const;
  
  int size() const;
  double distance(const GE_Chain&, const int) const;
  
  bool belongs(int) const;
  bool is_one_liberty(int) const;
  
  bool is_possible_eye(const GE_Goban&, int, GE_Marker* = 0, list<int>* = 0, 
		       bool = true, bool = false) const;
  int get_eye_candidates(const GE_Goban&, list<int>&, GE_Marker* = 0, 
			  list<list<int> >* = 0) const;
  bool is_alive(const GE_Goban&, GE_Marker* = 0) const;
  
  void print() const;
  void print_error() const;
};

struct GE_Chains
{
  list<GE_Chain> sets;

  GE_Chains() {}
  GE_Chains(const GE_Goban&);
  ~GE_Chains() {}
  
  void init(const GE_Goban&);

  //void neighbour_chains(const GE_Goban&, const int);
  void neighbour_chains(const GE_Goban&, const int, int*** = 0);
  void neighbour_chains(const GE_Goban&, const GE_Chain&, int*** = 0);
  void neighbour_chains(const GE_Goban&, const GE_Chains&, int*** = 0);

  int get_nb_dead(const team_color& = GE_NEUTRAL_COLOR, int* = 0) const;
  int get_nb_atari(const team_color& = GE_NEUTRAL_COLOR, int* = 0, 
		   list<int>* = 0) const;

  void get_weaker_chain(GE_Chain&) const;

  void print() const;
};

#endif

#ifndef GE_PATTERN_H
#define GE_PATTERN_H

#include <iostream>
using namespace std;

#include <string>
#include <assert.h>

#include <vector>
#include <list>

#include <fstream>

#include <cstdlib>

#include "tools_ge.h"

#define GE_SYMMETRY_NS 1
#define GE_SYMMETRY_WE 2
#define GE_SYMMETRY_NWSE 3
#define GE_SYMMETRY_NESW 4
#define GE_ROTATION_1 5
#define GE_ROTATION_2 6
#define GE_ROTATION_3 7



struct GE_Pattern
{
  string name;
  
  string pattern;
  
  string b_complete_pattern;
  string b_complete_patternNS;
  string b_complete_patternWE;
  string b_complete_patternNWSE;
  string b_complete_patternNESW;
  string b_complete_patternROT1;
  string b_complete_patternROT2;
  string b_complete_patternROT3;
  
  string w_complete_pattern;
  string w_complete_patternNS;
  string w_complete_patternWE;
  string w_complete_patternNWSE;
  string w_complete_patternNESW;
  string w_complete_patternROT1;
  string w_complete_patternROT2;
  string w_complete_patternROT3;
  
   
  list<int> matching_positions;
  list<int> matching_positionsNS;
  list<int> matching_positionsWE;
  list<int> matching_positionsNWSE;
  list<int> matching_positionsNESW;
  list<int> matching_positionsROT1;
  list<int> matching_positionsROT2;
  list<int> matching_positionsROT3;


  double val_pattern;
  int length;
 
  static int black_location;
  static int white_location;
  static int empty_location;
  static int edge_location;
  
  int board_size;
  vector<int> board;
  const vector<int>* pointer_board;

  
  static char nothing;
  static char edge;
  static char empty;
  static char empty_or_edge;
  static char white;
  static char white_or_edge;
  static char white_or_empty;
  static char not_black;
  static char black;
  static char black_or_edge;
  static char black_or_empty;
  static char not_white;
  static char black_or_white;
  static char not_empty;
  static char not_edge;
  static char all;  
  static char here;

  static list<int> all_choices;

  static const char reverse(const char);
  

  GE_Pattern()
  {
    length = -1;
    val_pattern = -1;
    pointer_board = 0;
    board_size = -1;
  }

  ~GE_Pattern()
  {
    pointer_board = 0;
  }


  static void configure(const vector<int>&, int = 1, int = 0, int = -1);
  static void configure_edge(int);
  


  const char get(int) const;
  const char get(const string&, int) const;
  static char get(const string&, int, int);
  char set(int, char);
  char set(string&, string&, list<int>&, int, char);
  static char set(string&, int, int, char);
  
  int size() const { return length*length; }
  int to_pos(int) const;
  int get_pos(int, int) const;
  int symmetryNS(int) const;
  int symmetryWE(int) const;
  int symmetryNWSE(int) const;
  int symmetryNESW(int) const;
  int rotation(int, unsigned int) const;
  
  void init_empty_pattern(string&);
  void init(const string&);
  
  int get_pos_by_transformation(int, int);
  void apply(int, string&, string&, list<int>&);
  
  void init_board(const vector<int>&, int = -1, int = -1);
  void init_board(int, int, int, const vector<int>&, int = -1, int = -1);
  
  
  void sort_matching(const string&, list<int>&);

  bool match(const string&, int, int) const;
  bool match(const string&, const list<int>&, int) const;
  bool match(int, int) const;
  
  int get_matching(int, list<int>&, bool = false) const;
  
  bool match_out(int, int);
  int get_matching_out(int, list<int>&, 	bool = false) const;
  
  
  //
  
  bool match(const int*, int, const string&, 
	     const list<int>&, int) const;
  bool match(const int*, int, const string&, int, int) const;
  
  bool match(const int*, int, int, int) const;
  
  int get_matching(const int*, int, int, list<int>&, bool = false) const;
  
  //
  


  static bool can_match(const char, const char);
  static bool can_match(const GE_Pattern&, const GE_Pattern&);
  
  
  static int get_point(char);
  static int get_point(const GE_Pattern&);
  int get_point() const;

  static void set_all_choices();

  static void create_random_pattern(string&, int = 0);
  //  static void create_random_pattern0(string&, int = 6, int = 5, int = 10);
  static void create_random_pattern0(string&, int = 3, int = 2, int = 10);
  

  static void save_pattern(const char*, const string&);
  static bool load_pattern(const char*, string&);
  
  void print_error(bool = false) const;
};


#endif

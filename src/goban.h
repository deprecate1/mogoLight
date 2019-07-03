/*************************
 * the board of the game *
 *************************/


#ifndef GE_GOBAN_H
#define GE_GOBAN_H

#include <iostream> 
using namespace std;

#include <list>

#include "const_goban.h"
#include "move.h"

class GE_Goban
{
 public:
  int height;
  int width;
  int** board;

 public:
  GE_Goban();
  GE_Goban(const bool);
  GE_Goban(int);
  GE_Goban(int, int);
  GE_Goban(const GE_Goban&);
  GE_Goban(const GE_Goban&, const team_color&);
  ~GE_Goban();

  GE_Goban& operator=(const GE_Goban&);
  bool operator==(const GE_Goban&) const;
  
  void init_goban(const int, const int=GE_WIDTH_GOBAN_NOT_DEFINED);
  void allocate_goban();
  void desallocate_goban();
  void reallocate_goban(int, int);

  

  void set_stone(const int, const int = GE_BLACK_STONE);
  void unset_stone(const int);

  void play(const int, const team_color& = GE_BLACK);
  void play(const pair<int, int>&, const team_color& = GE_BLACK);
  void play(const string&, const team_color& = GE_BLACK);

  void back_play(const int);

  void update(const list<int>&);
  void update(const list<list<int> >&);

  bool is_color(const team_color&, const int, const int, bool&) const;
  bool is_color(const team_color&, const int, bool&) const;
  bool is_color(const team_color&, const int) const;
  int size() const;
  inline int get_stone(int) const;
  inline int get_stone(const pair<int, int>&) const;
  
  int eye(int) const;
  int eye(pair<int, int>&) const;
  int eye0(int) const;
  int eye0(pair<int, int>&) const;

  int get_nb_liberties(const int, list<int>* = 0) const;
  int get_nb_liberties(const int, const int, list<int>* = 0) const;

  bool is_alone(int) const;
  bool is_alone(const pair<int, int>&) const;
  bool is_alone(int, int) const;

  void get_empty_intersections(GE_Goban&) const;
  int count_stones(team_color = GE_NEUTRAL_COLOR) const;
  
  int get_nb_neighbours(int, int = GE_WITHOUT_STONE) const;
  int get_nb_neighbours(const pair<int, int>&, int = GE_WITHOUT_STONE) const;
  int get_nb_neighbours(int, int, int) const;

  bool is_a_dame_location(int, int) const;
  
  void print_row(const int) const;
  void print() const;
  void print(const int) const;
  void print(const pair<int, int>&) const;

  void print_error_row(const int) const;
  void print_error() const;
  void print_error(const int) const;
  void print_error(const pair<int, int>&) const;


};


inline int GE_Goban::get_stone(int mv) const
{
  pair<int, int> mv2 = int_to_pair(height, mv);
  //assert(mv!=GE_PASS_MOVE);
  return board[mv2.first][mv2.second];
}

inline int GE_Goban::get_stone(const pair<int, int>& mv2) const
{
  return board[mv2.first][mv2.second];
}



const string stone_to_string(const int);
pair<int, int> distance_goban(const pair<int, int>&, const pair<int, int>&);
pair<int, int> distance_goban(const int, const int, const int = GE_DEFAULT_SIZE_GOBAN);


//float distance_goban(int, int, int, int);





#endif

#ifndef GE_FILE_GO_H
#define GE_FILE_GO_H

#include <iostream>
using namespace std;

#include <utility>
#include <fstream>
#include <list>
#include <string>

#include "const_goban.h"

struct GE_InfosPosition
{
  string filename;


  string report;
  int color;
  int last_move;
  list<int> game;
  int score;
  
  int height;
  int width;
  int* goban;

  bool load(const char* = 0);
  bool read_position(ifstream&, list<string>&, string&);
  bool translation(list<string>&);
  //bool to_goban();

  private:
  char* position;
  pair<char, int> to_black;
  pair<char, int> to_white;
  pair<char, int> to_free;
  char nothing;
  char frame;
  char last_move_before; //for example: '('
  char last_move_after;  //for example: ')'

  list<pair<char, int> > characters_used; 

  bool inventory_characters(const list<string>&);
  bool is_in_characters_used(char, int* = 0) const;

};

#endif

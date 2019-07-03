#ifndef GE_FILE_SGF_H
#define GE_FILE_SGF_H

#include "move.h"
#include "const_goban.h"
#include "tools_ge.h"

#include <iostream>
using namespace std;

#include <list>

#include <fstream>
#include <assert.h>


struct GE_FileSGF
{
  //const char* file_name;
  string file_name;
  int size_goban;
  float komi;
  
  list<int> black_stones;
  list<int> white_stones;
  list<int> game;
  
  GE_FileSGF() {}
  //GE_FileSGF(const char*);
  GE_FileSGF(const string&, int = -1);
  ~GE_FileSGF() {}
  
  bool read_move(ifstream&, string&) const;
  bool read_size(ifstream&);
  bool read_komi(ifstream&);
  bool read_position(ifstream&);
  bool read_game(int = -1);
  
  void restart(ifstream&);

  void print() const;
};


#endif

#ifndef GE_MOVE_H
#define GE_MOVE_H

#include <iostream>
using namespace std;

#include <utility>
#include <sstream>

#include <assert.h>

#include "const_move.h"

pair<int, int> int_to_pair(const int, const int);
int pair_to_int(const int, const pair<int, int>&);

int char_to_move_for_go(const char);
int char_to_move_for_go_sgf(const char);
pair<int, int> string_to_move(const string&);
const string move_to_string(const int, const int);
const string move_to_string(const pair<int, int>&);
const string move_to_string_gtp(const int, const int);
const string move_to_string_gtp(const pair<int, int>&);
const string move_to_string_gtp(int, const pair<int, int>&);

pair<int, int> string_gtp_to_move(const string&);
int string_gtp_to_move(const string&, int);


const string move_to_string_gtp_sym(const int, const int);
const string move_to_string_gtp_sym(int, const pair<int, int>&);

#endif

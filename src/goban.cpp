#include "goban.h"
#include <stdlib.h>

GE_Goban::GE_Goban()
{
  this->init_goban(GE_DEFAULT_SIZE_GOBAN);
}

GE_Goban::GE_Goban(const bool mode)
{
  if(mode)
    this->init_goban(GE_DEFAULT_SIZE_GOBAN);
  else this->board = 0;
}

GE_Goban::GE_Goban(int size_goban)
{
  this->init_goban(size_goban);
}


GE_Goban::GE_Goban(const int h, const int w)
{
  this->init_goban(h, w);
}

GE_Goban::GE_Goban(const GE_Goban& gob)
{
  height = gob.height;
  width = gob.width;

  this->allocate_goban();

  for(int i = 0; i<gob.height; i++)
    for(int j = 0; j<gob.width; j++)
      this->board[i][j] = gob.board[i][j];
}

GE_Goban::GE_Goban(const GE_Goban& gob, const team_color& col)
{
  height = gob.height;
  width = gob.width;

  this->allocate_goban();

  int stone_color = ((col==GE_BLACK) ? GE_BLACK_STONE : GE_WHITE_STONE);
  
  for(int i = 0; i<gob.height; i++)
    for(int j = 0; j<gob.width; j++)
      {
	if(gob.board[i][j]==stone_color)
	  this->board[i][j] = gob.board[i][j];
	else this->board[i][j] = GE_WITHOUT_STONE;
      }
}


GE_Goban::~GE_Goban()
{
  this->desallocate_goban();
}

GE_Goban& GE_Goban::operator=(const GE_Goban& gob)
{
  this->reallocate_goban(gob.height, gob.width);
  
  for(int i = 0; i<gob.height; i++)
    for(int j = 0; j<gob.width; j++)
      this->board[i][j] = gob.board[i][j];

  return *this; 
}

bool GE_Goban::operator==(const GE_Goban& gob) const
{
  if(this->height!=gob.height) return false;
  if(this->width!=gob.width) return false;

  for(int i = 0; i<gob.height; i++)
    for(int j = 0; j<gob.width; j++)
      if(this->board[i][j] != gob.board[i][j])
	return false;

  return true; 
}



void GE_Goban::init_goban(const int h, const int w)
{
  height = h;
  width = w;
  
  if(w==GE_WIDTH_GOBAN_NOT_DEFINED)
    width = h;
  
  this->allocate_goban();
  
  for(int i = 0; i<height; i++)
    for(int j = 0; j<width; j++)
      board[i][j] = GE_WITHOUT_STONE;
}

void GE_Goban::allocate_goban()
{
  board = new int*[height];
  
  for(int i = 0; i<height; i++)
    board[i] = new int[width];
}

void GE_Goban::reallocate_goban(int h, int w)
{
  this->desallocate_goban();
  
  height = h;
  width = w;

  this->allocate_goban();
}


void GE_Goban::desallocate_goban()
{
  if(not board) return;

  for(int i = 0; i<height; i++)
    {
      if(board[i]) delete[] board[i];
      board[i] = 0;
    }
  
  if(board) delete[] board;
  board = 0;
}

const string stone_to_string(const int stone) 
{
  switch(stone)
    {
    case GE_WHITE_STONE : return "O";
    case GE_BLACK_STONE : return "@";
    case GE_WITHOUT_STONE : return ".";
    case GE_BLACK_STONE_POTENTIAL : return "b";
    case GE_WHITE_STONE_POTENTIAL : return "w";
    case GE_STRONG_BLACK_STONE_POTENTIAL : return "B";
    case GE_STRONG_WHITE_STONE_POTENTIAL : return "W";
    case GE_NEUTRAL_STONE : return "*";
    default : return "#";
    }
}


void GE_Goban::set_stone(const int location, const int color_stone)
{
  pair<int, int> loc2 = int_to_pair(height, location);
  board[loc2.first][loc2.second] = color_stone;
}


void GE_Goban::unset_stone(const int location)
{
  pair<int, int> loc2 = int_to_pair(height, location);
  board[loc2.first][loc2.second] = GE_WITHOUT_STONE;
}



void GE_Goban::play(const pair<int, int>& mv, const team_color& col)
{
  if(mv.first==GE_PASS_MOVE) return;

  if(col==GE_BLACK)
    board[mv.first][mv.second] = GE_BLACK_STONE;
  else board[mv.first][mv.second] = GE_WHITE_STONE;
}

void GE_Goban::play(const int mv, const team_color& col)
{
  pair<int, int> mv2 = int_to_pair(this->height, mv);
  this->play(mv2, col);
}

void GE_Goban::play(const string& s_mv, const team_color& col)
{
  this->play(string_to_move(s_mv), col);
}

void GE_Goban::back_play(const int mv)
{
  if(mv==GE_PASS_MOVE) return;

  pair<int, int> mv2 = int_to_pair(height, mv);
  board[mv2.first][mv2.second] = GE_WITHOUT_STONE;
}

void GE_Goban::update(const list<int>& stones_taken)
{
  list<int>::const_iterator j_stone = stones_taken.begin();
  while(j_stone!=stones_taken.end())
    {
      pair<int, int> intersection = int_to_pair(height, *j_stone);
      board[intersection.first][intersection.second] = GE_WITHOUT_STONE;
      j_stone++;
    }
}

void GE_Goban::update(const list<list<int> >& stones_taken)
{
  list<list<int> >::const_iterator i_stones = stones_taken.begin();
  
  while(i_stones!=stones_taken.end())
    {
      list<int>::const_iterator j_stone = i_stones->begin();
      while(j_stone!=i_stones->end())
	{
	  pair<int, int> intersection = int_to_pair(height, *j_stone);
	  board[intersection.first][intersection.second] = GE_WITHOUT_STONE;
	  j_stone++;
	}
      
      i_stones++;
    }
}


bool GE_Goban::is_color(const team_color& col, const int i, const int j, 
		     bool& is_neutral) const
{
  int color_stone;
  int potential_color_stone;  
  int strong_potential_color_stone;
  
  if(col==GE_WHITE)
    {
      color_stone = GE_WHITE_STONE;
      potential_color_stone = GE_WHITE_STONE_POTENTIAL;
      strong_potential_color_stone = GE_STRONG_WHITE_STONE_POTENTIAL;
    }
  else
    {
      color_stone = GE_BLACK_STONE;
      potential_color_stone = GE_BLACK_STONE_POTENTIAL;
      strong_potential_color_stone = GE_STRONG_BLACK_STONE_POTENTIAL;
    }


  if((this->board[i][j]==color_stone)
     ||(this->board[i][j]==potential_color_stone)
     ||(this->board[i][j]==strong_potential_color_stone))
    return true;
  
  if((this->board[i][j]==GE_WITHOUT_STONE)
     ||(this->board[i][j]==GE_NEUTRAL_STONE))
    is_neutral = true;

  return false;
}

bool GE_Goban::is_color(const team_color& col, const int stone, 
		     bool& is_neutral) const
{
  if(stone==GE_PASS_MOVE) return false;

  pair<int, int> coord = int_to_pair(height, stone);
  return is_color(col, coord.first, coord.second, is_neutral);
}

bool GE_Goban::is_color(const team_color& col, const int stone) const
{
  if(stone==GE_PASS_MOVE) return false;

  bool is_neutral = false;
  pair<int, int> coord = int_to_pair(height, stone);
  return is_color(col, coord.first, coord.second, is_neutral);
}

int GE_Goban::size() const
{
  return height * width;
}

int GE_Goban::eye(int location) const
{
  pair<int, int> coord = int_to_pair(height, location);
  return this->eye(coord);
}


int GE_Goban::eye(pair<int, int>& coord) const
{
  pair<int, int> start;

  if(GE_IS_IN_GOBAN(coord.first+1, coord.second, height, width))
    start = make_pair(coord.first+1, coord.second);
  else start = make_pair(coord.first-1, coord.second);

  int nb_false = 0;
  int size_chain = 0;
  int& color_stone = board[start.first][start.second];
  
  if(color_stone==GE_WITHOUT_STONE) return GE_NEUTRAL_COLOR;
  
  if(GE_IS_IN_GOBAN(coord.first+1, coord.second+1, height, width))
    {
      if(board[coord.first+1][coord.second+1]==color_stone)
	size_chain++;
      else nb_false++;
      
      if(board[coord.first+1][coord.second]==color_stone)
	size_chain++;
      else return GE_NEUTRAL_COLOR;
      
      if(board[coord.first][coord.second+1]==color_stone)
	size_chain++;
      else return GE_NEUTRAL_COLOR;
    }
  else
    {
      if(GE_IS_IN_GOBAN(coord.first+1, coord.second, height, width))
	{
	  if(board[coord.first+1][coord.second]==color_stone)
	    size_chain++;
	  else return GE_NEUTRAL_COLOR;
	}
      
      if(GE_IS_IN_GOBAN(coord.first, coord.second+1, height, width))
	{
	  if(board[coord.first][coord.second+1]==color_stone)
	    size_chain++;
	  else return GE_NEUTRAL_COLOR;
	}
    }

  if(nb_false>1) return GE_NEUTRAL_COLOR;
  
  if(GE_IS_IN_GOBAN(coord.first-1, coord.second-1, height, width))
    {
      if(board[coord.first-1][coord.second-1]==color_stone)
	size_chain++;
      else nb_false++;
      
      if(board[coord.first-1][coord.second]==color_stone)
	size_chain++;
      else return GE_NEUTRAL_COLOR;
      
      if(board[coord.first][coord.second-1]==color_stone)
	size_chain++;
      else return GE_NEUTRAL_COLOR;
    }
  else
    {
      if(GE_IS_IN_GOBAN(coord.first-1, coord.second, height, width))
	{
	  if(board[coord.first-1][coord.second]==color_stone)
	    size_chain++;
	  else return GE_NEUTRAL_COLOR;
	}
      
      if(GE_IS_IN_GOBAN(coord.first, coord.second-1, height, width))
	{
	  if(board[coord.first][coord.second-1]==color_stone)
	    size_chain++;
	  else return GE_NEUTRAL_COLOR;
	}
    }
  
  if(nb_false>1) return GE_NEUTRAL_COLOR;
  
  if(GE_IS_IN_GOBAN(coord.first-1, coord.second+1, height, width))
    {
      if(board[coord.first-1][coord.second+1]==color_stone)
	size_chain++;
      else nb_false++;
    }
  
  if(GE_IS_IN_GOBAN(coord.first+1, coord.second-1, height, width))
    {
      if(board[coord.first+1][coord.second-1]==color_stone)
	size_chain++;
      else nb_false++;
    }
  
  if(size_chain>=7)
    return (color_stone==GE_WHITE_STONE) ? GE_WHITE : GE_BLACK;
  
  if(nb_false==0)
    return (color_stone==GE_WHITE_STONE) ? GE_WHITE : GE_BLACK;
  
  return GE_NEUTRAL_COLOR;
}

int GE_Goban::eye0(int location) const
{
  pair<int, int> coord = int_to_pair(height, location);
  return this->eye0(coord);
}

int GE_Goban::eye0(pair<int, int>& coord) const
{
  int& i = coord.first;
  int& j = coord.second; 

  int color_stone;
  
  if(GE_IS_IN_GOBAN(i+1, j, height, width))
    color_stone = board[i+1][j];
  else color_stone = board[i-1][j];
  
  if(color_stone==GE_WITHOUT_STONE) return GE_NEUTRAL_COLOR;
  
  if(GE_IS_IN_GOBAN(i, j-1, height, width))
    if(color_stone!=board[i][j-1])
      return GE_NEUTRAL_COLOR;
  
  if(GE_IS_IN_GOBAN(i, j+1, height, width))
    if(color_stone!=board[i][j+1])
      return GE_NEUTRAL_COLOR;
  
  if(GE_IS_IN_GOBAN(i-1, j, height, width))
    if(color_stone!=board[i-1][j])
      return GE_NEUTRAL_COLOR;
  
  //I think this test is useless
  if(GE_IS_IN_GOBAN(i+1, j, height, width))
    if(color_stone!=board[i+1][j])
      return GE_NEUTRAL_COLOR;
  
  return (color_stone==GE_WHITE_STONE) ? GE_WHITE : GE_BLACK;
}


/*
//TO IMPLEMENT: a recursive version of eye 

int GE_Goban::eye2(pair<int, int>& coord) const
{
  pair<int, int> start;
  
  if(GE_IS_IN_GOBAN(coord.first+1, coord.second, height, width))
    start = make_pair(coord.first+1, coord.second);
  else start = make_pair(coord.first-1, coord.second);
  
  int nb_false = 0;
  int size_chain = 0;
  int& color_stone = board[start.first][start.second];
  
  if(color_stone==GE_WITHOUT_STONE) return GE_NEUTRAL_COLOR;
  
  if(GE_IS_IN_GOBAN(coord.first+1, coord.second+1, height, width))
    {
      if(board[coord.first+1][coord.second+1]==color_stone)
	size_chain++;
      else
	{
	  if(board[coord.first+1][coord.second+1]==GE_WITHOUT_STONE)
	    {
	      pair<int, int> neighbour = make_pair(coord.first+1, coord.second+1);
	      potential_eyes.push_back(pair_to_int(height, neighbour));
	    }
	}
      
      if(board[coord.first+1][coord.second]==color_stone)
	size_chain++;
      else return GE_NEUTRAL_COLOR;
      
      if(board[coord.first][coord.second+1]==color_stone)
	size_chain++;
      else return GE_NEUTRAL_COLOR;
    }
  else
    {
      if(GE_IS_IN_GOBAN(coord.first+1, coord.second, height, width))
	{
	  if(board[coord.first+1][coord.second]==color_stone)
	    size_chain++;
	  else return GE_NEUTRAL_COLOR;
	}
      
      if(GE_IS_IN_GOBAN(coord.first, coord.second+1, height, width))
	{
	  if(board[coord.first][coord.second+1]==color_stone)
	    size_chain++;
	  else return GE_NEUTRAL_COLOR;
	}
    }

  if(nb_false>1) return GE_NEUTRAL_COLOR;
  
  if(GE_IS_IN_GOBAN(coord.first-1, coord.second-1, height, width))
    {
      if(board[coord.first-1][coord.second-1]==color_stone)
	size_chain++;
      else
	{
	  if(board[coord.first-1][coord.second-1]==GE_WITHOUT_STONE)
	    {
	      pair<int, int> neighbour = make_pair(coord.first-1, coord.second-1);
	      potential_eyes.push_back(pair_to_int(height, neighbour));     
	    }
	}      
      
      if(board[coord.first-1][coord.second]==color_stone)
	size_chain++;
      else return GE_NEUTRAL_COLOR;
      
      if(board[coord.first][coord.second-1]==color_stone)
	size_chain++;
      else return GE_NEUTRAL_COLOR;
    }
  else
    {
      if(GE_IS_IN_GOBAN(coord.first-1, coord.second, height, width))
	{
	  if(board[coord.first-1][coord.second]==color_stone)
	    size_chain++;
	  else return GE_NEUTRAL_COLOR;
	}
      
      if(GE_IS_IN_GOBAN(coord.first, coord.second-1, height, width))
	{
	  if(board[coord.first][coord.second-1]==color_stone)
	    size_chain++;
	  else return GE_NEUTRAL_COLOR;
	}
    }
  
  if(nb_false>1) return GE_NEUTRAL_COLOR;
  
  if(GE_IS_IN_GOBAN(coord.first-1, coord.second+1, height, width))
    {
      if(board[coord.first-1][coord.second+1]==color_stone)
	size_chain++;
      else 
	{
	  if(board[coord.first-1][coord.second+1]==GE_WITHOUT_STONE)
	    {
	      pair<int, int> neighbour = make_pair(coord.first-1, coord.second+1);
	      potential_eyes.push_back(pair_to_int(height, neighbour));     
	    }
	}      
    }
  
  if(GE_IS_IN_GOBAN(coord.first+1, coord.second-1, height, width))
    {
      if(board[coord.first+1][coord.second-1]==color_stone)
	size_chain++;
      else
	{
	  if(board[coord.first+1][coord.second-1]==GE_WITHOUT_STONE)
	    {
	      pair<int, int> neighbour = make_pair(coord.first+1, coord.second-1);
	      potential_eyes.push_back(pair_to_int(height, neighbour));     
	    }
	}      
    }
  
  if(size_chain>=7)
    return (color_stone==GE_WHITE_STONE) ? GE_WHITE : GE_BLACK;
  
  if(nb_false==0)
    return (color_stone==GE_WHITE_STONE) ? GE_WHITE : GE_BLACK;

  board[coord.first][coord.second] = color_stone;
  while(not potential_eyes.empty())
    {
      pair<int, int> potential_eye = int_to_pair(height, potential_eyes.front());
      
      if(this->eyes2(potential_eye)==GE_NEUTRAL_COLOR)
	break;
      
      potential_eyes.pop_front();
    }
  
  board[coord.first][coord.second] = GE_WITHOUT_STONE;
  
  return GE_NEUTRAL_COLOR;
}
*/

int GE_Goban::get_nb_liberties(const int mv, list<int>* liberties) const
{
  pair<int, int> mv2 = int_to_pair(height, mv); 
  return get_nb_liberties(mv2.first, mv2.second, liberties);
}

int GE_Goban::get_nb_liberties(const int i, const int j, list<int>* liberties) const
{
  int nb_liberties = 0;
  int ii = i;
  int jj = j+1;
  
  if(GE_IS_IN_GOBAN(ii, jj, height, width))
    if(board[ii][jj]==GE_WITHOUT_STONE)
      {
	nb_liberties++;
	if(liberties) 
	  liberties->push_back(pair_to_int(height, make_pair(ii, jj)));
      }

  ii = i;
  jj = j-1;
  if(GE_IS_IN_GOBAN(ii, jj, height, width))
    if(board[ii][jj]==GE_WITHOUT_STONE)
      {
	nb_liberties++;
	if(liberties) 
	  liberties->push_back(pair_to_int(height, make_pair(ii, jj)));
      }
  
  ii = i+1;
  jj = j;
  if(GE_IS_IN_GOBAN(ii, jj, height, width))
    if(board[ii][jj]==GE_WITHOUT_STONE)
      {
	nb_liberties++;
	if(liberties) 
	  liberties->push_back(pair_to_int(height, make_pair(ii, jj)));
      }
  
  ii = i-1;
  jj = j;
  if(GE_IS_IN_GOBAN(ii, jj, height, width))
    if(board[ii][jj]==GE_WITHOUT_STONE)
      {
	nb_liberties++;
	if(liberties) 
	  liberties->push_back(pair_to_int(height, make_pair(ii, jj)));
      }

  return nb_liberties;
}

bool GE_Goban::is_alone(int mv) const
{
  pair<int, int> mv2 = int_to_pair(height, mv);
  return is_alone(mv2.first, mv2.second);
}

bool GE_Goban::is_alone(const pair<int, int>& mv2) const
{
  return is_alone(mv2.first, mv2.second);
}

bool GE_Goban::is_alone(int i, int j) const
{
  int ii = i;
  int jj = j+1;
  
  if(GE_IS_IN_GOBAN(ii, jj, height, width))
    if(board[ii][jj]==board[i][j])
      return false;

  ii = i;
  jj = j-1;
  if(GE_IS_IN_GOBAN(ii, jj, height, width))
    if(board[ii][jj]==board[i][j])
      return false;
  
  ii = i+1;
  jj = j;
  if(GE_IS_IN_GOBAN(ii, jj, height, width))
    if(board[ii][jj]==board[i][j])
      return false;
  
  ii = i-1;
  jj = j;
  if(GE_IS_IN_GOBAN(ii, jj, height, width))
    if(board[ii][jj]==board[i][j])
      return false;
  
  return true;
}


//to calculate score (by a negative)
//GE_BLACK_STONE and GE_WHITE_STONE become GE_WITHOUT_STONE
//GE_WITHOUT_STONE becomes GE_BLACK_STONE
void GE_Goban::get_empty_intersections(GE_Goban& gob) const
{
  for(int i = 0; i<height; i++)
    for(int j = 0; j<width; j++)
      if(board[i][j]==GE_WITHOUT_STONE)
	gob.board[i][j] = GE_BLACK_STONE;
      else gob.board[i][j] = GE_WITHOUT_STONE;
}

int GE_Goban::count_stones(team_color tc) const
{
  int nb_stones = 0;
  
  for(int i = 0; i<height; i++)
    for(int j = 0; j<width; j++)
      if(board[i][j]!=GE_WITHOUT_STONE)
	{
	  switch(tc)
	    {
	    case GE_WHITE : if(board[i][j]==GE_WHITE_STONE) nb_stones++; break;
	    case GE_BLACK : if(board[i][j]==GE_BLACK_STONE) nb_stones++; break;
	    default : nb_stones++;
	    }
	}

  return nb_stones;
}



int GE_Goban::get_nb_neighbours(int location, int color_stone) const
{
  pair<int, int> loc = int_to_pair(height, location);
  return get_nb_neighbours(loc.first, loc.second, color_stone);
}

int GE_Goban::get_nb_neighbours(const pair<int, int>& loc, int color_stone) const 
{
  return get_nb_neighbours(loc.first, loc.second, color_stone);
}

int GE_Goban::get_nb_neighbours(int i, int j, int color_stone) const 
{
  int ii = i;
  int jj = j+1;

  int nb_stones = 0;
  
  if(GE_IS_IN_GOBAN(ii, jj, height, width))
    if(board[ii][jj]==color_stone)
      nb_stones++;

  ii = i;
  jj = j-1;
  if(GE_IS_IN_GOBAN(ii, jj, height, width))
    if(board[ii][jj]==color_stone)
      nb_stones++;
  
  ii = i+1;
  jj = j;
  if(GE_IS_IN_GOBAN(ii, jj, height, width))
    if(board[ii][jj]==color_stone)
      nb_stones++;
  
  ii = i-1;
  jj = j;
  if(GE_IS_IN_GOBAN(ii, jj, height, width))
    if(board[ii][jj]==color_stone)
      nb_stones++;
  
  return nb_stones;
}

bool GE_Goban::is_a_dame_location(int mv, int color_stone) const
{
  if(mv==GE_PASS_MOVE) return false;
  
  pair<int, int> mv2 = int_to_pair(height, mv);
  
  const int& i = mv2.first;
  const int& j = mv2.second;
  
  int ii = i;
  int jj = j+1;
  
  int nb_black_stones = 0;
  int nb_white_stones = 0;
  int nb_locations = 0;
  
  if(GE_IS_IN_GOBAN(ii, jj, height, width))
    {
      if(board[ii][jj]==GE_WHITE_STONE)
	nb_white_stones++;
      else 
	{
	  if(board[ii][jj]==GE_BLACK_STONE)
	    nb_black_stones++;
	}
      
      nb_locations++;
    }
  
  ii = i;
  jj = j-1;
  if(GE_IS_IN_GOBAN(ii, jj, height, width))
    {
      if(board[ii][jj]==GE_WHITE_STONE)
	nb_white_stones++;
      else 
	{
	  if(board[ii][jj]==GE_BLACK_STONE)
	    nb_black_stones++;
	}
      
      nb_locations++;
    }
  
  ii = i+1;
  jj = j;
  if(GE_IS_IN_GOBAN(ii, jj, height, width))
    {
      if(board[ii][jj]==GE_WHITE_STONE)
	nb_white_stones++;
      else 
	{
	  if(board[ii][jj]==GE_BLACK_STONE)
	    nb_black_stones++;
	}
      
      nb_locations++;
    }
  
  ii = i-1;
  jj = j;
  if(GE_IS_IN_GOBAN(ii, jj, height, width))
    {
      if(board[ii][jj]==GE_WHITE_STONE)
	nb_white_stones++;
      else 
	{
	  if(board[ii][jj]==GE_BLACK_STONE)
	    nb_black_stones++;
	}
      
      nb_locations++;
    }
  
  if(color_stone==GE_BLACK_STONE)
    {
      if((nb_black_stones==1)
	 &&(nb_locations==nb_white_stones+1))
	return true;
      else return false;
    }
  else
    {
      if((nb_white_stones==1)
	 &&(nb_locations==nb_black_stones+1))
	return true;
      else return false;
    }
 
  return false;
}

void GE_Goban::print_row(const int row) const
{
  for(int j = 0; j<width; j++)
    cout<<stone_to_string(board[row][j])<<" ";
  
  cout<<endl;
}

void GE_Goban::print() const
{
  for(int i = 0; i<width; i++)
    this->print_row(i);
  
  cout<<endl;
}

void GE_Goban::print(const int mov) const
{
  pair<int, int> mov2 = int_to_pair(height, mov);
  this->print(mov2);
}

void GE_Goban::print(const pair<int, int>& mov) const
{
  char c = 'A';

  cout<<"  ";
  for(int i = 0; i<height; i++)
    {
      cout<<c<<" ";
      c++;
      if(c=='I') c++;
    }

  cout<<endl;
  c = 'A';

  for(int i = 0; i<height; i++)
    {
      cout<<c;
      c++;
      if(c=='I') c++;

      if((mov.first==i)&&(mov.second==0))
	cout<<"(";
      else cout<<" ";
 
      for(int j = 0; j<width; j++)
	{
	  cout<<stone_to_string(board[i][j]);
	  
	  if((mov.first==i)&&(mov.second==j+1))
	    {
	      cout<<"(";
	      continue;
	    }
	  
	  if((mov.first==i)&&(mov.second==j))
	    {
	      cout<<")";
	      continue;
	    }
	  
	  cout<<" ";
	}

      cout<<endl;
    } 
  
  cout<<endl;
}



void GE_Goban::print_error_row(const int row) const
{
  for(int j = 0; j<width; j++)
    cerr<<stone_to_string(board[row][j])<<" ";
  
  cerr<<endl;
}

void GE_Goban::print_error() const
{
  for(int i = 0; i<width; i++)
    this->print_error_row(i);
  
  cerr<<endl;
}

void GE_Goban::print_error(const int mov) const
{
  pair<int, int> mov2 = int_to_pair(height, mov);
  this->print_error(mov2);
}

void GE_Goban::print_error(const pair<int, int>& mov) const
{
  char c = 'A';

  cerr<<"  ";
  for(int i = 0; i<height; i++)
    {
      cerr<<c<<" ";
      c++;
      if(c=='I') c++;
    }

  cerr<<endl;
  c = 'A';

  for(int i = 0; i<height; i++)
    {
      cerr<<c;
      c++;
      if(c=='I') c++;

      if((mov.first==i)&&(mov.second==0))
	cerr<<"(";
      else cerr<<" ";
 
      for(int j = 0; j<width; j++)
	{
	  cerr<<stone_to_string(board[i][j]);
	  
	  if((mov.first==i)&&(mov.second==j+1))
	    {
	      cerr<<"(";
	      continue;
	    }
	  
	  if((mov.first==i)&&(mov.second==j))
	    {
	      cerr<<")";
	      continue;
	    }
	  
	  cerr<<" ";
	}

      cerr<<endl;
    } 
  
  cerr<<endl;
}


pair<int, int> distance_goban(const pair<int, int>& coord1, 
			     const pair<int, int>& coord2)
{
  pair<int, int> dist;

  dist.first = abs(coord1.first-coord2.first);
  dist.second = abs(coord1.second-coord2.second);
  return dist;
}

pair<int, int> distance_goban(const int mv1, const int mv2, 
			     const int height)
{
  pair<int, int> coord1 = int_to_pair(height, mv1);
  pair<int, int> coord2 = int_to_pair(height, mv2);
  return distance_goban(coord1, coord2);
}




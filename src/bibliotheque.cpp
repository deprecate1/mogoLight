#include "bibliotheque.h"

vector<int> GE_Bibliotheque::to_goban;
vector<int> GE_Bibliotheque::to_big_goban;
vector<vector<int> > GE_Bibliotheque::distance_pion;

int GE_Directions::board_area = 361;
int GE_Directions::board_size = 19;
int GE_Directions::big_board_area = 441;
int GE_Directions::big_board_size = 21;
int GE_Directions::directions[GE_NB_DIRECTIONS];
int GE_Directions::limit_higher_location = 419;

void GE_Directions::init(int h, int w)
{
  board_area = h*w;
  board_size = w;
  big_board_area = (h+2)*(w+2);
  big_board_size = w+2;
  
  directions[GE_HERE] = 0;
  directions[GE_NORTH] = -big_board_size;
  directions[GE_SOUTH] = big_board_size;
  directions[GE_WEST] = -1;
  directions[GE_EAST] = 1;
  directions[GE_NORTHEAST] = -big_board_size+1;
  directions[GE_SOUTHEAST] = big_board_size+1;
  directions[GE_NORTHWEST] = -big_board_size-1;
  directions[GE_SOUTHWEST] = big_board_size-1;

  limit_higher_location = (big_board_size-2)*big_board_size+(big_board_size-1);
}

int GE_Directions::to_move(int mv)
{
  return GE_Directions::board_size*((mv/GE_Directions::big_board_size)-1)
    +((mv%GE_Directions::big_board_size)-1);
}


int GE_Directions::to_big_move(int mv)
{
  return GE_Directions::big_board_size*((mv/GE_Directions::board_size)+1)
    +((mv%GE_Directions::board_size)+1);
}

string GE_Directions::to_move_string(int mv)
{
  int move = GE_Directions::to_move(mv);
  return move_to_string(GE_Directions::board_size, move);
}

void GE_Bibliotheque::init(int h, int w)
{
  assert(h==w);
  
  GE_Directions::init(h, w);
  to_goban.resize(GE_Directions::big_board_area, -1);
  to_big_goban.resize(GE_Directions::board_area, -1);

  for(int i = 0; i<(int)to_goban.size(); i++)
    to_goban[i] = -1;
  
  for(int i = 0; i<(int)to_big_goban.size(); i++)
    to_big_goban[i] = GE_Directions::to_big_move(i);
  
  int start = 1+GE_Directions::directions[GE_SOUTH];
  int end = start+GE_Directions::board_size;
  
  while(start<GE_Directions::limit_higher_location)
    {
      for(int j = start; j<end; j++)
	to_goban[j] = GE_Directions::to_move(j);
      
      start+=GE_Directions::directions[GE_SOUTH];
      end+=GE_Directions::directions[GE_SOUTH];
    }

  init_distance_pion();
}

void GE_Bibliotheque::init_distance_pion()
{
  distance_pion.resize(GE_Directions::board_area);
  
  for(int i = 0; i<(int)GE_Directions::board_area; i++)
    {
      distance_pion[i].resize(GE_Directions::board_area, 0);
    }
  
  
  for(int i = 0; i<(int)GE_Directions::board_area; i++)
    {
      int num_col1 = i%GE_Directions::board_size;
      int num_row1 = i/GE_Directions::board_size;
      
      for(int j = 0; j<(int)GE_Directions::board_area; j++)
	{
	  int num_col2 = j%GE_Directions::board_size;
	  int num_row2 = j/GE_Directions::board_size;
	  
	  int diff_col = num_col1-num_col2;
	  if(diff_col<0) diff_col = -diff_col;

	  int diff_row = num_row1-num_row2;
	  if(diff_row<0) diff_row = -diff_row;
	  
	  distance_pion[i][j] = diff_col+diff_row;
	}      
    } 
}

bool GE_Bibliotheque::is_in_border(int big_location)
{
  int big_neighbourNE = big_location+GE_Directions::directions[GE_NORTHEAST];
  if(to_goban[big_neighbourNE]<0) return true;
  
  int big_neighbourSW = big_location+GE_Directions::directions[GE_SOUTHWEST];
  if(to_goban[big_neighbourSW]<0) return true;
  
  return false;
}

#include "pattern.h"

int GE_Pattern::black_location = 1;
int GE_Pattern::white_location = 0;
int GE_Pattern::empty_location = -1;
int GE_Pattern::edge_location = -2;

char GE_Pattern::nothing = '!';
char GE_Pattern::edge = '#';
char GE_Pattern::empty = '.';
char GE_Pattern::empty_or_edge = 'E';
char GE_Pattern::white = 'O';
char GE_Pattern::white_or_edge = 'o';
char GE_Pattern::white_or_empty = 'Q';
char GE_Pattern::not_black = 'J';
char GE_Pattern::black = '@';
char GE_Pattern::black_or_edge = 'a';
char GE_Pattern::black_or_empty = 'A';
char GE_Pattern::not_white = 'K';
char GE_Pattern::black_or_white = 'B';
char GE_Pattern::not_empty = 'L';
char GE_Pattern::not_edge = 'M';
char GE_Pattern::all = '?';
char GE_Pattern::here = 'X';

list<int> GE_Pattern::all_choices;

const char GE_Pattern::reverse(const char c)
{
  if(c==black) return white;
  if(c==white) return black;
  if(c==white_or_edge) return black_or_edge;
  if(c==white_or_empty) return black_or_empty;
  if(c==black_or_edge) return white_or_edge;
  if(c==black_or_empty) return white_or_empty;
  if(c==not_white) return not_black;
  if(c==not_black) return not_white;
  
  return c;
}

void GE_Pattern::configure(const vector<int>& temp_board, int bl, 
			int wl, int el)
{
  black_location = bl;
  white_location = wl;
  empty_location = el;
  
  
  int temp_color_location = temp_board[0];
  
  if((temp_color_location==bl)
     ||(temp_color_location==wl)
     ||(temp_color_location==el))
    {
      edge_location = 0;
      
      while(true)
	{
	  if((edge_location==black_location)
	     ||(edge_location==white_location)
	     ||(edge_location==empty_location))
	    {
	      edge_location++;
	      continue;
	    }
	  
	  break;
	}
    }
  else 
    {
      edge_location = temp_color_location;
    }

  /*
  for(int i = 0; i<(int)(temp_board.size()); i++)
    {
      if(temp_board[i]==black_location) continue;
      if(temp_board[i]==white_location) continue;
      if(temp_board[i]==empty_location) continue;
      if(temp_board[i]==edge_location) continue;

      cerr<<"*** ERROR *** board no valid (1) !"<<endl;
      
      assert(0);
    }
  */

  if(bl==wl)
    {
      cerr<<"*** ERROR *** black and white locations are the same"<<endl;
      assert(0);
    }

  if(bl==el)
    {
      cerr<<"*** ERROR *** black and empty locations are the same"<<endl;
      assert(0);
    }
  
  if(wl==el)
    {
      cerr<<"*** ERROR *** white and empty locations are the same"<<endl;
      assert(0);
    }
  
}
  



void GE_Pattern::configure_edge(int edge_color)
{
  edge_location = edge_color;

  if((edge_location==black_location)
     ||(edge_location==white_location)
     ||(edge_location==empty_location))
    {
      cerr<<"*** ERROR *** color of edge is not valid"<<endl;
      assert(0);
    }
}



const char GE_Pattern::get(int pos) const
{
  return get(b_complete_pattern, pos);
}


const char GE_Pattern::get(const string& s_pattern, int pos) const
{
  int ncol = pos%length;
  int nraw = pos/length;
  
  return s_pattern[nraw*length+(ncol+nraw)];
}



char GE_Pattern::get(const string& s_pattern, int temp_length, int pos)
{
  int ncol = pos%temp_length;
  int nraw = pos/temp_length;
  
  int real_pos = nraw*(temp_length+1)+ncol;
  
  return s_pattern[real_pos];
}



char GE_Pattern::set(int pos, char val)
{
  return set(b_complete_pattern, w_complete_pattern, 
	     matching_positions, pos, val);
}

char GE_Pattern::set(string& bs_pattern, string& ws_pattern, 
		  list<int>& l_pattern, int pos, char val)
{
  int ncol = pos%length;
  int nraw = pos/length;
  
  int real_pos = nraw*length+(ncol+nraw); 
  
  int old_val = bs_pattern[real_pos];
  
  assert(bs_pattern[real_pos]!='\n');
  bs_pattern[real_pos] = val;
  ws_pattern[real_pos] = reverse(val);
  
  
  if(val!=GE_Pattern::all)
    l_pattern.push_back(pos);
  
  return old_val;
}


char GE_Pattern::set(string& s_pattern, int temp_length, int pos, char val)
{
  int ncol = pos%temp_length;
  int nraw = pos/temp_length;
  
  int real_pos = nraw*(temp_length+1)+ncol;
  
  char old_val = s_pattern[real_pos];
  s_pattern[real_pos] = val;
  
  return old_val;
}



int GE_Pattern::to_pos(int index) const
{
  int iraw = index/(length+1);
  int icol = index%(length+1);
  
  return get_pos(iraw, icol);
}

int GE_Pattern::get_pos(int nraw, int ncol) const
{
  return nraw*length+ncol;
}

int GE_Pattern::symmetryNS(int pos) const
{
  int ncol = pos%length;
  int nraw = pos/length;
  
  int new_ncol = ncol;
  int new_nraw = length-1-nraw;
  
  return get_pos(new_nraw, new_ncol);
}


int GE_Pattern::symmetryWE(int pos) const
{
  int ncol = pos%length;
  int nraw = pos/length;
  
  int new_ncol = length-1-ncol;  
  int new_nraw = nraw;
  
  return get_pos(new_nraw, new_ncol);
}

int GE_Pattern::symmetryNWSE(int pos) const
{
  int ncol = pos%length;
  int nraw = pos/length;
  
  int new_ncol = length-1-nraw;
  int new_nraw = length-1-ncol;
  
  return get_pos(new_nraw, new_ncol);
}

int GE_Pattern::symmetryNESW(int pos) const
{
  int ncol = pos%length;
  int nraw = pos/length;
  
  int new_ncol = nraw;
  int new_nraw = ncol;
  
  return get_pos(new_nraw, new_ncol);
}



int GE_Pattern::rotation(int pos, unsigned int angle) const
{
  if(angle>=4) angle%=4;

  
  int ncol = pos%length;
  int nraw = pos/length;
  
  int new_ncol = ncol;
  int new_nraw = nraw;
  
  switch(angle)
    {
    case 1 :
      new_ncol = length-1-nraw;
      new_nraw = ncol;
      break;
      
    case 2 :
      new_ncol = length-1-ncol;
      new_nraw = length-1-nraw;
      break;


    case 3 : 
      new_ncol = nraw;
      new_nraw = length-1-ncol;
      break;
      
    default : ;     
    }
  

  return get_pos(new_nraw, new_ncol);
}




void GE_Pattern::init_empty_pattern(string& s_pattern)
{
  s_pattern = string(length*(length+1), '\n');
  
  for(int i = 0; i<(int)s_pattern.size(); i++)
    {
      if(i%(length+1)==length)
	continue;
      
      s_pattern[i] = GE_Pattern::all;
    }
  
}



void GE_Pattern::init(const string& pattern)
{
  this->pattern = pattern;
  
  int pos_here = -1;
  for(int i = 0; i<(int)(pattern.size()); i++)
    {
      if(pattern[i]!='\n')
	pos_here++;
      
      if(pattern[i]==GE_Pattern::here)
	break;
    }
  
  if(pos_here==-1) assert(0);
  
  int temp_length = 0;
  for(int i = 0; i<(int)(pattern.size()); i++)
    {
      if(pattern[i]=='\n')
	break;
      
      temp_length++;
    }
  
  int col_here = pos_here%temp_length;
  int raw_here = pos_here/temp_length;
  
  assert(temp_length>0);
  
  
  int temp_width = 0;
  for(int i = 0; i<(int)(pattern.size()); i++)
    {
      if(pattern[i]=='\n')
	temp_width++;
    }   

  assert(temp_width>0);
  
  int max_delta_bord = 0;
  if(col_here>raw_here)
    max_delta_bord = col_here+1;
  else max_delta_bord = raw_here+1;
  
  if(max_delta_bord<temp_length-col_here)
    max_delta_bord = temp_length-col_here;

  if(max_delta_bord<temp_width-raw_here)
    max_delta_bord = temp_width-raw_here;

  int real_length = 2*max_delta_bord-1;

  this->length = real_length;
  
  init_empty_pattern(this->b_complete_pattern);
  init_empty_pattern(this->w_complete_pattern);

  int real_raw_pos_here = real_length/2;
  int real_col_pos_here = real_length/2;
  int real_pos_here = get_pos(real_raw_pos_here, real_col_pos_here);
  
  set(real_pos_here, GE_Pattern::here);
  
  
  int real_raw_begin = real_raw_pos_here-raw_here;
  int real_col_begin = real_col_pos_here-col_here;
  
  int pos_begin = get_pos(real_raw_begin, real_col_begin);
  
  int temp_pos = pos_begin;
  int next_raw_pos = pos_begin+real_length;
  for(int i = 0; i<(int)(pattern.size()); i++)
    {
      if(pattern[i]=='\n') 
	{
	  temp_pos = next_raw_pos;
	  next_raw_pos += real_length;
	  
	  continue;
	}
      
      if(pattern[i]==GE_Pattern::here)
	assert(get(temp_pos)==GE_Pattern::here);
      
      set(temp_pos, pattern[i]);
      temp_pos++;
    } 
  
  //apply(&(this->symmetryNS), complete_patternNS)

  
  apply(GE_SYMMETRY_NS, b_complete_patternNS, w_complete_patternNS, 
	matching_positionsNS);
  
  apply(GE_SYMMETRY_WE, b_complete_patternWE, w_complete_patternWE, 
	matching_positionsWE);
  
  apply(GE_SYMMETRY_NWSE, b_complete_patternNWSE, w_complete_patternNWSE, 
	matching_positionsNWSE);
  
  apply(GE_SYMMETRY_NESW, b_complete_patternNESW, w_complete_patternNESW, 
	matching_positionsNESW);
  
  apply(GE_ROTATION_1, b_complete_patternROT1, w_complete_patternROT1, 
	matching_positionsROT1);
  
  apply(GE_ROTATION_2, b_complete_patternROT2, w_complete_patternROT2, 
	matching_positionsROT2);
  
  apply(GE_ROTATION_3, b_complete_patternROT3, w_complete_patternROT3, 
	matching_positionsROT3);

  sort_matching(b_complete_pattern, matching_positions);
  sort_matching(b_complete_patternNS, matching_positionsNS);
  sort_matching(b_complete_patternWE, matching_positionsWE);
  sort_matching(b_complete_patternNWSE, matching_positionsNWSE);
  sort_matching(b_complete_patternNESW, matching_positionsNESW);
  sort_matching(b_complete_patternROT1, matching_positionsROT1);
  sort_matching(b_complete_patternROT2, matching_positionsROT2);
  sort_matching(b_complete_patternROT3, matching_positionsROT3);
  
}


int GE_Pattern::get_pos_by_transformation(int pos, int num_transformation)
{
  switch(num_transformation)
    {
    case 0 : return pos;
    case GE_SYMMETRY_NS : return symmetryNS(pos);
    case GE_SYMMETRY_WE : return symmetryWE(pos);
    case GE_SYMMETRY_NWSE : return symmetryNWSE(pos);
    case GE_SYMMETRY_NESW : return symmetryNESW(pos);
    case GE_ROTATION_1 : return rotation(pos, 5);
    case GE_ROTATION_2 : return rotation(pos, 6);
    case GE_ROTATION_3 : return rotation(pos, 7);
    default : assert(0);
    }
}




void GE_Pattern::apply(int num_transformation, string& bs_pattern, 
		    string& ws_pattern, list<int>& l_pattern)
{
  init_empty_pattern(bs_pattern);
  init_empty_pattern(ws_pattern);

  for(int i = 0; i<(int)b_complete_pattern.size(); i++)
    {
      if(b_complete_pattern[i]=='\n') continue;      
      int temp_pos = to_pos(i);
      
      int new_pos = get_pos_by_transformation(temp_pos, 
					      num_transformation);
      
      set(bs_pattern, ws_pattern, l_pattern, new_pos, b_complete_pattern[i]);
    }
}


void GE_Pattern::init_board(const vector<int>& temp_board, int height, 
			 int width)
{
  if(temp_board[0]==edge_location)
    {
      pointer_board = &temp_board;
      
      board_size = -3;
     

      /*
	//board_size = 9 but 12 '#' before the first '.' .
	###########
	#.........#
	
	So, board_size is initialized by -3;
      */


      for(int i = 0; i<(int)temp_board.size(); i++)
	{
	  if(temp_board[i]==edge_location) 
	    {
	      board_size++;
	      continue;
	    }

	  break;
	}

      assert(board_size>0);
    }
  else 
    {
      assert(height>0);

      this->board_size = height;

      int big_board_area = (height+2)*(height+2);
      
      if(width>0) big_board_area = (height+2)*(width+2);
      
      board.clear();
      board.resize(big_board_area, edge_location);
      
      int big_height = height+2;
      
      int big_index = big_height+1;
      int next_raw_index = big_index+big_height;
      for(int i = 0; i<(int)temp_board.size(); i++)
	{
	  board[big_index] = temp_board[i];
	  
	  big_index++;
	  
	  if(i%height==(height-1))
	    {
	      big_index = next_raw_index;
	      next_raw_index += big_height; 
	    }
	}
      
      pointer_board = &board;
    }
}




void GE_Pattern::init_board(int bl, int wl, int el, 
			 const vector<int>& temp_board, int height, 
			 int width)
{
  configure(temp_board, bl, wl, el);
  init_board(temp_board, height, width);
}


void GE_Pattern::sort_matching(const string& s_pattern, list<int>& l_matching)
{
  int temp_size = (int)l_matching.size();
  
  list<int> copy_matching = l_matching;
  list<int> copy_matching2;
  
  l_matching.clear();
  
  while(not copy_matching.empty())
    {
      int pos = copy_matching.front();
      copy_matching.pop_front();
      
      char elt = get(s_pattern, pos);
      
      if((elt==GE_Pattern::black)
	 ||(elt==GE_Pattern::white))
	l_matching.push_back(pos);
      else copy_matching2.push_back(pos);
    }
  
  copy_matching = copy_matching2;
  copy_matching2.clear();
  while(not copy_matching.empty())
    {
      int pos = copy_matching.front();
      copy_matching.pop_front();
      
      char elt = get(s_pattern, pos);
      
      if(elt==GE_Pattern::empty)
	l_matching.push_back(pos);
      else copy_matching2.push_back(pos);
    }
  
  copy_matching = copy_matching2;
  copy_matching2.clear();
  while(not copy_matching.empty())
    {
      int pos = copy_matching.front();
      copy_matching.pop_front();
      
      
      char elt = get(s_pattern, pos);
      
      if(elt==GE_Pattern::edge)
	l_matching.push_back(pos);
      else copy_matching2.push_back(pos);
    }
  
  copy_matching = copy_matching2;
  copy_matching2.clear();
  while(not copy_matching.empty())
    {
      int pos = copy_matching.front();
      copy_matching.pop_front();
      
      char elt = get(s_pattern, pos);
      
      if((elt==GE_Pattern::black_or_edge)
	 ||(elt==GE_Pattern::white_or_edge))
	l_matching.push_back(pos);
      else copy_matching2.push_back(pos);
    }
  
  copy_matching = copy_matching2;
  copy_matching2.clear();
  while(not copy_matching.empty())
    {
      int pos = copy_matching.front();
      copy_matching.pop_front();
      
      
      char elt = get(s_pattern, pos);
      
      if(elt==GE_Pattern::empty_or_edge)
	l_matching.push_back(pos);
      else copy_matching2.push_back(pos);
    }
  
  
  copy_matching = copy_matching2;
  copy_matching2.clear();
  while(not copy_matching.empty())
    {
      int pos = copy_matching.front();
      copy_matching.pop_front();
      
      
      char elt = get(s_pattern, pos);
      
      if(elt==GE_Pattern::black_or_white)
	l_matching.push_back(pos);
      else copy_matching2.push_back(pos);
    }
  
  
  copy_matching = copy_matching2;
  copy_matching2.clear();
  while(not copy_matching.empty())
    {
      int pos = copy_matching.front();
      copy_matching.pop_front();
      
      
      char elt = get(s_pattern, pos);
      
      if((elt==GE_Pattern::black_or_empty)
	 ||(elt==GE_Pattern::white_or_empty))
	l_matching.push_back(pos);
      else copy_matching2.push_back(pos);
    }
  
   
  copy_matching = copy_matching2;
  copy_matching2.clear();
  while(not copy_matching.empty())
    {
      int pos = copy_matching.front();
      copy_matching.pop_front();
      
      
      char elt = get(s_pattern, pos);
      
      if(elt==GE_Pattern::not_empty)
	l_matching.push_back(pos);
      else copy_matching2.push_back(pos);
    }
  
  
  copy_matching = copy_matching2;
  copy_matching2.clear();
  while(not copy_matching.empty())
    {
      int pos = copy_matching.front();
      copy_matching.pop_front();
      
      
      char elt = get(s_pattern, pos);
      
      if((elt==GE_Pattern::not_black)
	 ||(elt==GE_Pattern::not_white))
	l_matching.push_back(pos);
      else copy_matching2.push_back(pos);
    }
  
  
  copy_matching = copy_matching2;
  copy_matching2.clear();
  while(not copy_matching.empty())
    {
      int pos = copy_matching.front();
      copy_matching.pop_front();
      
      
      char elt = get(s_pattern, pos);
      
      if(elt==GE_Pattern::not_edge)
	l_matching.push_back(pos);
      else copy_matching2.push_back(pos);
    }
  
  
  copy_matching = copy_matching2;
  copy_matching2.clear();
  while(not copy_matching.empty())
    {
      int pos = copy_matching.front();
      copy_matching.pop_front();
      
      l_matching.push_back(pos);
    }
  
  assert((int)l_matching.size()==temp_size);
}



bool GE_Pattern::match(const string& s_pattern, int pos, int big_location)
  const
{
  assert(pointer_board);

  char elt = get(s_pattern, pos);
   
  if((big_location<0)
     ||(big_location>=(int)(pointer_board->size())))
    {
      if((elt==GE_Pattern::edge)
	 ||(elt==GE_Pattern::not_black)
	 ||(elt==GE_Pattern::not_white)
	 ||(elt==GE_Pattern::not_empty)
	 ||(elt==GE_Pattern::black_or_edge)
	 ||(elt==GE_Pattern::white_or_edge)
	 ||(elt==GE_Pattern::empty_or_edge))
	{
	  return true;
	}
      else
	{
	  return false;
	}
    }
	
  int color_location = (*pointer_board)[big_location];

  
  if(elt==GE_Pattern::black)
    return (color_location==black_location);
  
  if(elt==GE_Pattern::white)
    return (color_location==white_location);

  if(elt==GE_Pattern::empty)
    return (color_location==empty_location);
  
  if(elt==GE_Pattern::edge)
    return (color_location==edge_location);

  
  if(elt==GE_Pattern::black_or_edge)
    return ((color_location==black_location)
	    ||(color_location==edge_location));
  
  if(elt==GE_Pattern::white_or_edge)
    return ((color_location==white_location)
	    ||(color_location==edge_location));
  
  if(elt==GE_Pattern::empty_or_edge)
    return ((color_location==empty_location)
	    ||(color_location==edge_location));
  
  if(elt==GE_Pattern::black_or_white)
    return ((color_location==black_location)
	    ||(color_location==white_location));


  if(elt==GE_Pattern::black_or_empty)
    return ((color_location==empty_location)
	    ||(color_location==black_location));
  
  if(elt==GE_Pattern::white_or_empty)
    return ((color_location==empty_location)
	    ||(color_location==white_location));
  
  if(elt==GE_Pattern::not_black)
    return (color_location!=black_location);
  
  if(elt==GE_Pattern::not_white)
    return (color_location!=white_location);
  
  if(elt==GE_Pattern::not_empty)
    return (color_location!=empty_location);

  
  if(elt==GE_Pattern::not_edge)
    return (color_location!=edge_location);
  
  
  if(elt==GE_Pattern::here)
    return (color_location==empty_location);
  
  if(elt==GE_Pattern::all)
    return true; 

  assert(elt!='\n');

  assert(0);
  
  return false;
}


bool GE_Pattern::match(const string& s_pattern, 
		    const list<int>& l_pattern, 
		    int big_location) const
{
  int shift = length/2;
  int big_height = board_size+2;
  
  list<int>::const_iterator i_pattern = l_pattern.begin();
  
  while(i_pattern!=l_pattern.end())
    {
      int pos = *i_pattern;
      i_pattern++;
      
      int nraw = pos/length;
      int ncol = pos%length;
      
      int shift_right = ncol-shift;
      int shift_down = nraw-shift;
      
      int temp_big_location = big_location+big_height*shift_down+
	shift_right;
      
      if(not match(s_pattern, pos, temp_big_location))
	return false;
	 
    }
  
  return true;
}


bool GE_Pattern::match(int big_location, int color_location) const
{
  assert((color_location==black_location)
	 ||(color_location==white_location));
  
  if(color_location==black_location)
    {
      if(match(b_complete_pattern, matching_positions, big_location)) 
	return true;
      
      if(match(b_complete_patternNS, matching_positionsNS, big_location)) 
	return true;
      
      if(match(b_complete_patternWE, matching_positionsWE, big_location)) 
	return true;
      
      if(match(b_complete_patternNWSE, matching_positionsNWSE, big_location)) 
	return true;
      
      if(match(b_complete_patternNESW, matching_positionsNESW, big_location)) 
	return true;
      
      if(match(b_complete_patternROT1, matching_positionsROT1, big_location)) 
	return true;
      
      if(match(b_complete_patternROT2, matching_positionsROT2, big_location)) 
	return true;
      
      if(match(b_complete_patternROT3, matching_positionsROT3, big_location)) 
	return true;
    }
  else
    {
      if(match(w_complete_pattern, matching_positions, big_location)) 
	return true;
      
      if(match(w_complete_patternNS, matching_positionsNS, big_location)) 
	return true;
      
      if(match(w_complete_patternWE, matching_positionsWE, big_location)) 
	return true;
      
      if(match(w_complete_patternNWSE, matching_positionsNWSE, big_location)) 
	return true;
      
      if(match(w_complete_patternNESW, matching_positionsNESW, big_location)) 
	return true;
      
      if(match(w_complete_patternROT1, matching_positionsROT1, big_location)) 
	return true;
      
      if(match(w_complete_patternROT2, matching_positionsROT2, big_location)) 
	return true;
      
      if(match(w_complete_patternROT3, matching_positionsROT3, big_location)) 
	return true;
    }
  

  return false;
}

int GE_Pattern::get_matching(int color_location, list<int>& l_matching, 
			  bool mode_print) const
{
  assert(pointer_board);
  
  for(int i = 0; i<(int)(pointer_board->size()); i++)
    {
      int big_location = i;
      if(match(big_location, color_location))
	l_matching.push_back(big_location);
    }
  
  if(mode_print)
    {
      cerr<<"matching moves: ";
	GE_ListOperators::print_error(l_matching);
    }

  return (int)l_matching.size();
}


bool GE_Pattern::match_out(int location, int color_location)
{
  int big_board_size = board_size+2;
  int big_location = location;
  
  if(pointer_board==&board)
    {
      big_location = big_board_size*((location/board_size)+1)
	+((location%board_size)+1);
    } 
  
  return match(big_location, color_location);
}


int GE_Pattern::get_matching_out(int color_location, list<int>& l_matching, 
			      bool mode_print) const
{
  get_matching(color_location, l_matching);

  if(pointer_board==&board)
    {
      int big_board_size = board_size+2;
      
      int nb_stay = l_matching.size();
      while(nb_stay>0)
	{
	  int big_location = l_matching.front();
	  l_matching.pop_front();
	  
	  int location = board_size*((big_location/big_board_size)-1)
	    +((big_location%big_board_size)-1);
	  
	  l_matching.push_back(location);
	  
	  nb_stay--;
	} 
    }
  
  if(mode_print)
    {
      cerr<<"matching moves: ";
	GE_ListOperators::print_error(l_matching);
    }

  return (int)l_matching.size();
}


//
bool GE_Pattern::match(const int* aBigBoard, 
		    int aBoardSize, 
		    const string& s_pattern, 
		    const list<int>& l_pattern, 
		    int big_location) const
{
  int shift = length/2;
  int big_height = aBoardSize+2;
  
  int bigBoardArea = big_height*big_height;
  
  list<int>::const_iterator i_pattern = l_pattern.begin();
  
  while(i_pattern!=l_pattern.end())
    {
      int pos = *i_pattern;
      i_pattern++;
      
      int nraw = pos/length;
      int ncol = pos%length;
      
      int shift_right = ncol-shift;
      int shift_down = nraw-shift;
      
      int temp_big_location = big_location+big_height*shift_down+
	shift_right;
      
      if(not match(aBigBoard, bigBoardArea, 
		   s_pattern, pos, temp_big_location))
	return false;
      
    }
  
  return true;
}



bool GE_Pattern::match(const int* aBigBoard, int aBigBoardArea, 
		    const string& s_pattern, int pos, int big_location)
  const
{
  
  char elt = get(s_pattern, pos);
  
  if((big_location<0)
     ||(big_location>=aBigBoardArea))
    {
      if((elt==GE_Pattern::edge)
	 ||(elt==GE_Pattern::not_black)
	 ||(elt==GE_Pattern::not_white)
	 ||(elt==GE_Pattern::not_empty)
	 ||(elt==GE_Pattern::black_or_edge)
	 ||(elt==GE_Pattern::white_or_edge)
	 ||(elt==GE_Pattern::empty_or_edge))
	{
	  return true;
	}
      else
	{
	  return false;
	}
    }
  
  int color_location = board[big_location];
  
  
  if(elt==GE_Pattern::black)
    return (color_location==black_location);
  
  if(elt==GE_Pattern::white)
    return (color_location==white_location);
  
  if(elt==GE_Pattern::empty)
    return (color_location==empty_location);
  
  if(elt==GE_Pattern::edge)
    return (color_location==edge_location);
  
  
  if(elt==GE_Pattern::black_or_edge)
    return ((color_location==black_location)
	    ||(color_location==edge_location));
  
  if(elt==GE_Pattern::white_or_edge)
    return ((color_location==white_location)
	    ||(color_location==edge_location));
  
  if(elt==GE_Pattern::empty_or_edge)
    return ((color_location==empty_location)
	    ||(color_location==edge_location));
  
  if(elt==GE_Pattern::black_or_white)
    return ((color_location==black_location)
	    ||(color_location==white_location));
  
  
  if(elt==GE_Pattern::black_or_empty)
    return ((color_location==empty_location)
	    ||(color_location==black_location));
  
  if(elt==GE_Pattern::white_or_empty)
    return ((color_location==empty_location)
	    ||(color_location==white_location));
  
  if(elt==GE_Pattern::not_black)
    return (color_location!=black_location);
  
  if(elt==GE_Pattern::not_white)
    return (color_location!=white_location);
  
  if(elt==GE_Pattern::not_empty)
    return (color_location!=empty_location);
  
  
  if(elt==GE_Pattern::not_edge)
    return (color_location!=edge_location);
  
  
  if(elt==GE_Pattern::here)
    return (color_location==empty_location);
  
  if(elt==GE_Pattern::all)
    return true; 
  
  assert(elt!='\n');
  
  assert(0);
  
  return false;
}

bool GE_Pattern::match(const int* aBigBoard, int aBoardSize, 
		    int big_location, int color_location) const
{
  assert((color_location==black_location)
	 ||(color_location==white_location));
  
  if(color_location==black_location)
    {
      if(match(aBigBoard, aBoardSize, b_complete_pattern, 
	       matching_positions, big_location)) 
	return true;
      
      if(match(aBigBoard, aBoardSize, b_complete_patternNS, 
	       matching_positionsNS, big_location)) 
	return true;
      
      if(match(aBigBoard, aBoardSize, b_complete_patternWE, 
	       matching_positionsWE, big_location)) 
	return true;
      
      if(match(aBigBoard, aBoardSize, b_complete_patternNWSE, 
	       matching_positionsNWSE, big_location)) 
	return true;
      
      if(match(aBigBoard, aBoardSize, b_complete_patternNESW, 
	       matching_positionsNESW, big_location)) 
	return true;
      
      if(match(aBigBoard, aBoardSize, b_complete_patternROT1, 
	       matching_positionsROT1, big_location)) 
	return true;
      
      if(match(aBigBoard, aBoardSize, b_complete_patternROT2, 
	       matching_positionsROT2, big_location)) 
	return true;
      
      if(match(aBigBoard, aBoardSize, b_complete_patternROT3, 
	       matching_positionsROT3, big_location)) 
	return true;
    }
  else
    {
      if(match(aBigBoard, aBoardSize, w_complete_pattern, 
	       matching_positions, big_location)) 
	return true;
      
      if(match(aBigBoard, aBoardSize, w_complete_patternNS, 
	       matching_positionsNS, big_location)) 
	return true;
      
      if(match(aBigBoard, aBoardSize, w_complete_patternWE, 
	       matching_positionsWE, big_location)) 
	return true;
      
      if(match(aBigBoard, aBoardSize, w_complete_patternNWSE, 
	       matching_positionsNWSE, big_location)) 
	return true;
      
      if(match(aBigBoard, aBoardSize, w_complete_patternNESW, 
	       matching_positionsNESW, big_location)) 
	return true;
      
      if(match(aBigBoard, aBoardSize, w_complete_patternROT1, 
	       matching_positionsROT1, big_location)) 
	return true;
      
      if(match(aBigBoard, aBoardSize, w_complete_patternROT2, 
	       matching_positionsROT2, big_location)) 
	return true;
      
      if(match(aBigBoard, aBoardSize, w_complete_patternROT3, 
	       matching_positionsROT3, big_location)) 
	return true;
    }
  

  return false;
}


int GE_Pattern::get_matching(const int* aBigBoard, int aBoardSize, 
			  int color_location, list<int>& l_matching, 
			  bool mode_print) const
{
  
  int bigBoardSize = aBoardSize+2;
  int bigBoardArea = bigBoardSize*bigBoardSize;
  
  for(int i = 0; i<bigBoardArea; i++)
    {
      int big_location = i;
      if(match(aBigBoard, aBoardSize, big_location, color_location))
	l_matching.push_back(big_location);
    }
  
  if(mode_print)
    {
      cerr<<"matching moves: ";
      GE_ListOperators::print_error(l_matching);
    }
  
  return (int)l_matching.size();
}


//



bool GE_Pattern::can_match(const char c1, const char c2)
{
  if(c1==GE_Pattern::all) return true;
  if(c2==GE_Pattern::all) return true;
  if(c1==c2) return true;
  if(c1==GE_Pattern::here) return false;
  if(c2==GE_Pattern::here) return false;
  if((c1==GE_Pattern::nothing)||(c2==GE_Pattern::nothing)) return false;

  if(c1==GE_Pattern::empty)
    {
      if(c2==GE_Pattern::edge) return false;
      if(c2==GE_Pattern::empty) return true;
      if(c2==GE_Pattern::empty_or_edge) return true;
      if(c2==GE_Pattern::white) return false;
      if(c2==GE_Pattern::white_or_edge) return false;
      if(c2==GE_Pattern::white_or_empty) return true;
      if(c2==GE_Pattern::not_black) return true;
      if(c2==GE_Pattern::black) return false;
      if(c2==GE_Pattern::black_or_edge) return false;
      if(c2==GE_Pattern::black_or_empty) return true;
      if(c2==GE_Pattern::not_white) return true;
      if(c2==GE_Pattern::black_or_white) return false;
      if(c2==GE_Pattern::not_empty) return false;
      if(c2==GE_Pattern::not_edge) return true;
      if(c2==GE_Pattern::all) return true;  
    }


  if(c1==GE_Pattern::black)
    {
      if(c2==GE_Pattern::edge) return false;
      if(c2==GE_Pattern::empty) return false;
      if(c2==GE_Pattern::empty_or_edge) return false;
      if(c2==GE_Pattern::white) return false;
      if(c2==GE_Pattern::white_or_edge) return false;
      if(c2==GE_Pattern::white_or_empty) return false;
      if(c2==GE_Pattern::not_black) return false;
      if(c2==GE_Pattern::black) return true;
      if(c2==GE_Pattern::black_or_edge) return true;
      if(c2==GE_Pattern::black_or_empty) return true;
      if(c2==GE_Pattern::not_white) return true;
      if(c2==GE_Pattern::black_or_white) return true;
      if(c2==GE_Pattern::not_empty) return true;
      if(c2==GE_Pattern::not_edge) return true;
      if(c2==GE_Pattern::all) return true;  
    }
  
  if(c1==GE_Pattern::white)
    {
      if(c2==GE_Pattern::edge) return false;
      if(c2==GE_Pattern::empty) return false;
      if(c2==GE_Pattern::empty_or_edge) return false;
      if(c2==GE_Pattern::white) return true;
      if(c2==GE_Pattern::white_or_edge) return true;
      if(c2==GE_Pattern::white_or_empty) return true;
      if(c2==GE_Pattern::not_black) return true;
      if(c2==GE_Pattern::black) return false;
      if(c2==GE_Pattern::black_or_edge) return false;
      if(c2==GE_Pattern::black_or_empty) return false;
      if(c2==GE_Pattern::not_white) return false;
      if(c2==GE_Pattern::black_or_white) return true;
      if(c2==GE_Pattern::not_empty) return true;
      if(c2==GE_Pattern::not_edge) return true;
      if(c2==GE_Pattern::all) return true;  
    }

  if(c1==GE_Pattern::edge)
    {
      if(c2==GE_Pattern::edge) return true;
      if(c2==GE_Pattern::empty_or_edge) return true;
      if(c2==GE_Pattern::white_or_edge) return true;
      if(c2==GE_Pattern::not_black) return true;
      if(c2==GE_Pattern::black_or_edge) return true;
      if(c2==GE_Pattern::not_white) return true;
      if(c2==GE_Pattern::not_empty) return true;
      if(c2==GE_Pattern::all) return true;  
      
      if(c2==GE_Pattern::empty) return false;
      if(c2==GE_Pattern::white) return false;
      if(c2==GE_Pattern::white_or_empty) return false;
      if(c2==GE_Pattern::black) return false;
      if(c2==GE_Pattern::black_or_empty) return false;
      if(c2==GE_Pattern::black_or_white) return false;
      if(c2==GE_Pattern::not_edge) return false;
    }
  
  if(c1==GE_Pattern::not_black)
    return (c2!=GE_Pattern::black);
  
  if(c1==GE_Pattern::not_white)
    return (c2!=GE_Pattern::white);
  
  if(c1==GE_Pattern::not_empty)
    return (c2!=GE_Pattern::empty);
  
  if(c1==GE_Pattern::not_edge)
    return (c2!=GE_Pattern::edge);

  if(c1==GE_Pattern::black_or_white)
    {
      if(c2==GE_Pattern::edge) return false;
      if(c2==GE_Pattern::empty) return false;
      if(c2==GE_Pattern::empty_or_edge) return false;

      return true;
    }

  if(c1==GE_Pattern::black_or_empty)
    {
      if(c2==GE_Pattern::edge) return false;
      if(c2==GE_Pattern::white) return false;
      if(c2==GE_Pattern::white_or_edge) return false;
      
      return true;
    }

  if(c1==GE_Pattern::black_or_edge)
    {
      if(c2==GE_Pattern::empty) return false;
      if(c2==GE_Pattern::white) return false;
      if(c2==GE_Pattern::white_or_empty) return false;
      
      return true;
    }
    
  if(c1==GE_Pattern::white_or_empty)
    {
      if(c2==GE_Pattern::edge) return false;
      if(c2==GE_Pattern::black) return false;
      if(c2==GE_Pattern::black_or_edge) return false;
      
      return true;
    }
  
  if(c1==GE_Pattern::white_or_edge)
    {
      if(c2==GE_Pattern::empty) return false;
      if(c2==GE_Pattern::black) return false;
      if(c2==GE_Pattern::black_or_empty) return false;
      
      return true;
    }
  
  if(c1==GE_Pattern::empty_or_edge)
    {
      if(c2==GE_Pattern::white) return false;
      if(c2==GE_Pattern::black) return false;
      if(c2==GE_Pattern::black_or_white) return false;
      
      return true;
    }
  
  assert(0);

  return true;
}


bool GE_Pattern::can_match(const GE_Pattern& patt1, const GE_Pattern& patt2)
{
  
  cerr<<"TODO IMPLEMENTATION"<<endl;
  assert(0);
  
  return true;
}

int GE_Pattern::get_point(char c)
{
  if(c==GE_Pattern::all) return 0;
  if(c==GE_Pattern::nothing) { assert(0); return -1; }
  if(c==GE_Pattern::edge) return 3;
  if(c==GE_Pattern::empty) return 3;
  if(c==GE_Pattern::empty_or_edge) return 2;
  if(c==GE_Pattern::white) return 3;
  if(c==GE_Pattern::white_or_edge) return 2;
  if(c==GE_Pattern::white_or_empty) return 2;
  if(c==GE_Pattern::not_black) return 1;
  if(c==GE_Pattern::black) return 3;
  if(c==GE_Pattern::black_or_edge) return 2;
  if(c==GE_Pattern::black_or_empty) return 2;
  if(c==GE_Pattern::not_white) return 1;
  if(c==GE_Pattern::black_or_white) return 2;
  if(c==GE_Pattern::not_empty) return 1;
  if(c==GE_Pattern::not_edge) return 1;  
  if(c==GE_Pattern::here) return 0;
  
  assert(0);
  return 0;
}



int GE_Pattern::get_point(const GE_Pattern& patt)
{
  int points = 0;
  list<int>::const_iterator i_pos = patt.matching_positions.begin();
  while(i_pos!=patt.matching_positions.end())
    {
      points+=GE_Pattern::get_point(patt.get(*i_pos));
      i_pos++;
    }

  return points;
}
  

int GE_Pattern::get_point() const 
{
  return GE_Pattern::get_point(*this);
}



void GE_Pattern::create_random_pattern(string& s_pattern, int mode)
{
  switch(mode)
    {
    case 0 : return create_random_pattern0(s_pattern);
    default : assert(0);
    }
}


void GE_Pattern::set_all_choices()
{
  all_choices.clear();
  
  all_choices.push_back(GE_Pattern::empty);
  all_choices.push_back(GE_Pattern::empty);
  all_choices.push_back(GE_Pattern::empty);
  all_choices.push_back(GE_Pattern::empty);
  all_choices.push_back(GE_Pattern::empty);
  all_choices.push_back(GE_Pattern::empty);
  all_choices.push_back(GE_Pattern::empty);
  all_choices.push_back(GE_Pattern::empty);
  all_choices.push_back(GE_Pattern::empty);
  all_choices.push_back(GE_Pattern::empty);
  all_choices.push_back(GE_Pattern::empty);
  all_choices.push_back(GE_Pattern::empty);
  all_choices.push_back(GE_Pattern::empty);
  all_choices.push_back(GE_Pattern::empty);
  all_choices.push_back(GE_Pattern::empty);
  all_choices.push_back(GE_Pattern::empty);
  all_choices.push_back(GE_Pattern::empty);
  all_choices.push_back(GE_Pattern::empty);
  all_choices.push_back(GE_Pattern::empty);
  all_choices.push_back(GE_Pattern::empty);
  
  all_choices.push_back(GE_Pattern::all);
  all_choices.push_back(GE_Pattern::all);
  all_choices.push_back(GE_Pattern::all);
  all_choices.push_back(GE_Pattern::all);
  all_choices.push_back(GE_Pattern::all);
  all_choices.push_back(GE_Pattern::all);
  all_choices.push_back(GE_Pattern::all);
  all_choices.push_back(GE_Pattern::all);
  all_choices.push_back(GE_Pattern::all);
  all_choices.push_back(GE_Pattern::all);
  all_choices.push_back(GE_Pattern::all);
  all_choices.push_back(GE_Pattern::all);
  all_choices.push_back(GE_Pattern::all);
  all_choices.push_back(GE_Pattern::all);
  all_choices.push_back(GE_Pattern::all);
  all_choices.push_back(GE_Pattern::all);
  all_choices.push_back(GE_Pattern::all);
  all_choices.push_back(GE_Pattern::all);
  all_choices.push_back(GE_Pattern::all);
  all_choices.push_back(GE_Pattern::all);
  
  all_choices.push_back(GE_Pattern::black);
  all_choices.push_back(GE_Pattern::black);
  all_choices.push_back(GE_Pattern::black);
  all_choices.push_back(GE_Pattern::black);
  all_choices.push_back(GE_Pattern::black);
  all_choices.push_back(GE_Pattern::black);
  all_choices.push_back(GE_Pattern::black);
  all_choices.push_back(GE_Pattern::black);
  all_choices.push_back(GE_Pattern::black);
  all_choices.push_back(GE_Pattern::black);

  all_choices.push_back(GE_Pattern::white);
  all_choices.push_back(GE_Pattern::white);
  all_choices.push_back(GE_Pattern::white);
  all_choices.push_back(GE_Pattern::white);
  all_choices.push_back(GE_Pattern::white);
  all_choices.push_back(GE_Pattern::white);
  all_choices.push_back(GE_Pattern::white);
  all_choices.push_back(GE_Pattern::white);
  all_choices.push_back(GE_Pattern::white);
  all_choices.push_back(GE_Pattern::white);

  all_choices.push_back(GE_Pattern::not_empty);
  all_choices.push_back(GE_Pattern::not_empty);
  all_choices.push_back(GE_Pattern::not_empty);
  all_choices.push_back(GE_Pattern::not_empty);
  all_choices.push_back(GE_Pattern::not_empty);

  all_choices.push_back(GE_Pattern::not_black);
  all_choices.push_back(GE_Pattern::not_black);
  all_choices.push_back(GE_Pattern::not_black);
  all_choices.push_back(GE_Pattern::not_black);
  all_choices.push_back(GE_Pattern::not_black);
  
  all_choices.push_back(GE_Pattern::not_white);
  all_choices.push_back(GE_Pattern::not_white);
  all_choices.push_back(GE_Pattern::not_white);
  all_choices.push_back(GE_Pattern::not_white);
  all_choices.push_back(GE_Pattern::not_white);
    
  all_choices.push_back(GE_Pattern::not_edge);

  all_choices.push_back(GE_Pattern::black_or_empty);
  all_choices.push_back(GE_Pattern::black_or_empty);
  all_choices.push_back(GE_Pattern::black_or_empty);
  
  all_choices.push_back(GE_Pattern::white_or_empty);
  all_choices.push_back(GE_Pattern::white_or_empty);
  all_choices.push_back(GE_Pattern::white_or_empty);
  
  all_choices.push_back(GE_Pattern::black_or_white);
  all_choices.push_back(GE_Pattern::black_or_white);
  all_choices.push_back(GE_Pattern::black_or_white);
  
  all_choices.push_back(GE_Pattern::black_or_edge);
  
  all_choices.push_back(GE_Pattern::white_or_edge);
  
  all_choices.push_back(GE_Pattern::empty_or_edge);
  all_choices.push_back(GE_Pattern::empty_or_edge); 
  all_choices.push_back(GE_Pattern::empty_or_edge);
}


void GE_Pattern::create_random_pattern0(string& s_pattern, 
				     int max_length, 
				     int max_width_moins_un, 
				     int param_border)
{
  int rand_length = 1+(rand()%max_length);
  int rand_width = 2+(rand()%max_width_moins_un);

  int rand_border1 = rand()%param_border;
  int rand_border2 = rand()%param_border;

  if(rand_border1==0)
    rand_width++;
  
  
  if(rand_border2==0)
    rand_length++;
  
  //cerr<<rand_length<<endl;
  //cerr<<rand_width<<endl;
  
  
  //s_pattern.clear();
  s_pattern = string((rand_length+1)*rand_width, '\n');
  
  if(rand_border1==0)
    {
      for(int pos = 0; pos<rand_length; pos++)
	{
	  GE_Pattern::set(s_pattern, rand_length, pos, GE_Pattern::all);
	}
      
      //cerr<<"yep 1"<<endl;

      GE_Pattern::set(s_pattern, rand_length, rand_length-1, GE_Pattern::edge);
    }
  
  
  if(rand_border2==0)
    {
      for(int pos = 0; pos<rand_width*rand_length; pos+=rand_length)
	{ 
	  GE_Pattern::set(s_pattern, rand_length, pos, GE_Pattern::all);
	}
      
      //cerr<<"yep 2: "<<rand_length<<" "<<rand_width<<endl;
      
      GE_Pattern::set(s_pattern, rand_length, (rand_width-1)*rand_length, 
		   GE_Pattern::edge);
    }
  
  int nb_positions = rand_length*rand_width;
  
  bool is_valid_pattern = false;
  
  list<int> l_positions;
  list<int> l_positions_copy;
  
  for(int i = 0; i<nb_positions; i++)
    {
      if(GE_Pattern::get(s_pattern, rand_length, i)=='\n')
	l_positions.push_back(i);
    }
  
  l_positions_copy = l_positions;
  
  
  if((rand_border1==0)||(rand_border2==0))
    is_valid_pattern = true;
  
  //if((rand_border1==0)&&(rand_border2==0)) cerr<<"corner pattern";


  while(true)
    {
      l_positions = l_positions_copy;
      
      int (*fct_random)(list<int>&, list<int>::iterator&) = 
	&GE_ListOperators::random;
      
      int pos_here = GE_ListOperators::select(l_positions, *fct_random);
      GE_Pattern::set(s_pattern, rand_length, pos_here, GE_Pattern::here);
      
      if(all_choices.empty())
	GE_Pattern::set_all_choices();
      
      
      while(not l_positions.empty())
	{
	  int pos = GE_ListOperators::select(l_positions, *fct_random);
	  //ce dernier rand ne sert pas mais ce n'est pas grave
	  
	  int rand_val = GE_ListOperators::select(all_choices, *fct_random, false);
	  
	  GE_Pattern::set(s_pattern, rand_length, pos, rand_val);
	  
	  if(rand_val!=GE_Pattern::all)
	    is_valid_pattern = true;
	}
      
      if(is_valid_pattern) break;
    }
  
}

void GE_Pattern::save_pattern(const char* filename, const string& s_pattern)
{
  ofstream f_pattern(filename);
  
  if(not f_pattern.is_open())
    {
      assert(0);
    }

  string temp_s_pattern = s_pattern;
  for(int i = 0; i<(int) temp_s_pattern.size(); i++)
    {
      if(temp_s_pattern[i]=='\n')
	temp_s_pattern[i] = GE_Pattern::nothing;
    }
  
  f_pattern<<temp_s_pattern;
  f_pattern.flush();
  
  f_pattern.close();
}

bool GE_Pattern::load_pattern(const char* filename, string& s_pattern)
{
  ifstream f_pattern(filename);

  if(not f_pattern.is_open())
    {
      return false;
    }
  
  f_pattern>>s_pattern;
  
  for(int i = 0; i<(int)s_pattern.size(); i++)
    {
      if(s_pattern[i]==GE_Pattern::nothing)
	s_pattern[i] = '\n';
    }
  

  f_pattern.close();

  return true;
}




void GE_Pattern::print_error(bool no_detail) const
{
  if(no_detail)
    {
      cerr<<"pattern "<<name<<" : "<<endl;
      cerr<<pattern<<val_pattern<<endl<<endl;
      return;
    }



  cerr<<"pattern : "<<endl;
  cerr<<pattern<<endl;
  cerr<<"black complete pattern : "<<endl;
  cerr<<b_complete_pattern<<endl;
  cerr<<"white complete pattern : "<<endl;
  cerr<<w_complete_pattern<<endl;
  GE_ListOperators::print_error(matching_positions);
  
  cerr<<"black complete pattern NS : "<<endl;
  cerr<<b_complete_patternNS<<endl;
  cerr<<"white complete pattern NS : "<<endl;
  cerr<<w_complete_patternNS<<endl;
  GE_ListOperators::print_error(matching_positionsNS);
  
  cerr<<"black complete pattern WE : "<<endl;
  cerr<<b_complete_patternWE<<endl;
  cerr<<"white complete pattern WE : "<<endl;
  cerr<<w_complete_patternWE<<endl;
  GE_ListOperators::print_error(matching_positionsWE);
  
  cerr<<"black complete pattern NWSE : "<<endl;
  cerr<<b_complete_patternNWSE<<endl;
  cerr<<"white complete pattern NWSE : "<<endl;
  cerr<<w_complete_patternNWSE<<endl;
  GE_ListOperators::print_error(matching_positionsNWSE);
  
  cerr<<"black complete pattern NESW : "<<endl;
  cerr<<b_complete_patternNESW<<endl;
  cerr<<"white complete pattern NESW : "<<endl;
  cerr<<w_complete_patternNESW<<endl;
  GE_ListOperators::print_error(matching_positionsNESW);
  
  cerr<<"black complete pattern ROT1 : "<<endl;
  cerr<<b_complete_patternROT1<<endl;
  cerr<<"white complete pattern ROT1 : "<<endl;
  cerr<<w_complete_patternROT1<<endl;
  GE_ListOperators::print_error(matching_positionsROT1);
  
  cerr<<"black complete pattern ROT2 : "<<endl;
  cerr<<b_complete_patternROT2<<endl;
  cerr<<"white complete pattern ROT2 : "<<endl;
  cerr<<w_complete_patternROT2<<endl;
  GE_ListOperators::print_error(matching_positionsROT2);
  
  cerr<<"black complete pattern ROT3 : "<<endl;
  cerr<<b_complete_patternROT3<<endl;
  cerr<<"white complete pattern ROT3 : "<<endl;
  cerr<<w_complete_patternROT3<<endl;
  GE_ListOperators::print_error(matching_positionsROT3);
  
}

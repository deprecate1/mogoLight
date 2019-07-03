#include "chain.h"

int GE_Chain::height = 0;

void GE_Chain::init(const GE_Goban& gob, int*** marker, int i, int j)
{
  height = gob.height;

  stone = (gob.board)[i][j];
  if(stone==GE_WITHOUT_STONE) {assert(0);((GE_Chain*)0)->size(); return;} //en principe, cela ne doit jamais arriver
  this->build_chain(gob, marker, i, j);
  liberties = (int)free_intersections.size();
}

void GE_Chain::init(const GE_Goban& gob, int mv)
{
  if(mv<0) assert(0);
  assert(gob.get_stone(mv)!=GE_WITHOUT_STONE);
assert(this);
//fprintf(stderr,"<%d>",gob.height);fflush(stderr);
  pair<int, int> mv2 = int_to_pair(gob.height, mv);
  assert(gob.board[mv2.first][mv2.second]!=GE_WITHOUT_STONE);
  
  int** marker;
  assert(gob.board[mv2.first][mv2.second]!=GE_WITHOUT_STONE);
  GE_Allocator::init(&marker, gob.height, gob.width, GE_NOT_SEEN);
  assert(gob.board[mv2.first][mv2.second]!=GE_WITHOUT_STONE);
  this->init(gob, &marker, mv2.first, mv2.second);
  GE_Allocator::desallocate(&marker, gob.height);
}



void GE_Chain::build_chain(const GE_Goban& gob, int*** marker, int i, int j, int dir)
{
  int ii = i;
  int jj = j;
  
  while(true)
    { 
      if(not GE_IS_IN_GOBAN(ii, jj, gob.height, gob.width))
	return;
      
      if(gob.board[ii][jj]!=this->stone)
	{
	  if(gob.board[ii][jj]==GE_WITHOUT_STONE)
	    {
	      //GE_ListOperators lo;
	      GE_ListOperators::push_if_not(free_intersections, 
					 pair_to_int(gob.height, 
						     make_pair(ii, jj)));
	    }
	  return;
	}
      
      if((*marker)[ii][jj]==GE_SEEN)
	return;
      
      links.push_back(pair_to_int(gob.height, make_pair(ii, jj)));
      (*marker)[ii][jj] = GE_SEEN;
      
      switch(dir)
	{
	case GE_NORTH :
	case GE_SOUTH :
	  this->build_chain(gob, marker, ii, jj+1, GE_EAST);
	  this->build_chain(gob, marker, ii, jj-1, GE_WEST);
	  break;
	case GE_EAST :
	case GE_WEST :
	  this->build_chain(gob, marker, ii-1, jj, GE_NORTH);
	  this->build_chain(gob, marker, ii+1, jj, GE_SOUTH);
	  break;
	  
	default :
	  this->build_chain(gob, marker, ii-1, jj, GE_NORTH);
	  this->build_chain(gob, marker, ii+1, jj, GE_SOUTH);
	  this->build_chain(gob, marker, ii, jj+1, GE_EAST);
	  this->build_chain(gob, marker, ii, jj-1, GE_WEST);
	  return;
	}
      
      switch(dir)
	{
	case GE_NORTH : ii--; break;
	case GE_SOUTH : ii++; break;
	case GE_EAST : jj++; break;
	case GE_WEST : jj--; break;
	default :;
	}
    }
}


bool GE_Chain::is_atari() const
{
  return (liberties==1);
}

bool GE_Chain::is_dead() const
{
  return (liberties==0);
}

int GE_Chain::size() const
{
  return (int)links.size();
}

bool GE_Chain::is_virtually_connected(const GE_Chain& ch, list<int>* inter, 
				   bool to_the_end) const
{
  //if(ch.free_intersections.size()<2) return false;
  //if((this->free_intersections).size()<2) return false;

  int nb_inter = 0;
  
  list<int>::const_iterator i_fi1 = (this->free_intersections).begin();
  
  while(i_fi1!=(this->free_intersections).end())
    {
      if(GE_ListOperators::is_in(ch.free_intersections, *i_fi1))
	{
	  nb_inter++;

	  if(inter)
	    inter->push_back(*i_fi1); 

	  if(not to_the_end)
	    if(nb_inter>1)
	      return true;
	}
      
      i_fi1++;
    }
  
  return (nb_inter>1);
}

int GE_Chain::get_connexions(const GE_Chain& ch, list<int>& connexions)
  const
{
  GE_ListOperators::intersection(this->free_intersections, 
			      ch.free_intersections, 
			      connexions);
  
  return (int)connexions.size();
}


int GE_Chain::get_one_connexion(const GE_Chain& ch)
  const
{
  list<int> connexions;
  
  if(get_connexions(ch, connexions)>0)
    return connexions.front();
  
  return GE_PASS_MOVE;
}


double GE_Chain::distance(const GE_Chain& ch, const int mode) const
{
  double dist = 0;
  
  list<int>::const_iterator i1 = ch.links.begin();
  while(i1!=ch.links.end())
    {
      
      list<int>::const_iterator i2 = this->links.begin();
      
      while(i2!=this->links.end())
	{
	  
	  pair<int, int> dist2 = distance_goban(*i1, *i2, height);
	  double temp_dist = 0;
	  
	  
	  switch(mode)
	    {
	    default :
	      temp_dist = sqrt((dist2.first * dist2.first) + (dist2.second*dist2.second));
	      if(temp_dist<dist) dist = temp_dist;
	    }

	  
	  i2++;
	}
      i1++;
    }
  
  return dist;
}

bool GE_Chain::belongs(int x) const
{
  return GE_ListOperators::is_in(links, x);
}

bool GE_Chain::is_one_liberty(int x) const
{
  return GE_ListOperators::is_in(free_intersections, x);
}


bool GE_Chain::is_possible_eye(const GE_Goban& gob, int location, GE_Marker* mk_eyes, 
			    list<int>* neighbour_stones, 
			    bool mode_recursive, bool mode_two) const
{
  int big_location = GE_Bibliotheque::to_big_goban[location];
  
  int big_neighbourN = big_location+GE_Directions::directions[GE_NORTH];
  int big_neighbourS = big_location+GE_Directions::directions[GE_SOUTH];
  int big_neighbourW = big_location+GE_Directions::directions[GE_WEST];
  int big_neighbourE = big_location+GE_Directions::directions[GE_EAST];
  
  int neighbourN = GE_Bibliotheque::to_goban[big_neighbourN];
  int neighbourS = GE_Bibliotheque::to_goban[big_neighbourS];
  int neighbourW = GE_Bibliotheque::to_goban[big_neighbourW];
  int neighbourE = GE_Bibliotheque::to_goban[big_neighbourE];
  
  int color_stone = this->stone;
  
  int empty_location = GE_PASS_MOVE;
  if(neighbourN>=0)
    {
      int color_neighbourN = gob.get_stone(neighbourN);
      if(color_neighbourN==GE_WITHOUT_STONE)
	{
	  if(mk_eyes) mk_eyes->set_treated(neighbourN);
	  empty_location = neighbourN;
	}
      else
	{
	  if(color_neighbourN!=color_stone)
	    return false;
	  else
	    {
	      if(neighbour_stones)
		if((not mk_eyes)||(not mk_eyes->is_treated(neighbourN)))
		  neighbour_stones->push_back(neighbourN);
	    }
	}
    }
  
  if(neighbourS>=0)
    {
      int color_neighbourS = gob.get_stone(neighbourS);
      if(color_neighbourS==GE_WITHOUT_STONE)
	{
	  if(mk_eyes) mk_eyes->set_treated(neighbourS);
	  if(empty_location!=GE_PASS_MOVE) return false;
	  empty_location = neighbourS;
	}
      else
	{
	  if(color_neighbourS!=color_stone)
	    return false;
	  else
	    {
	      if(neighbour_stones)
		if((not mk_eyes)||(not mk_eyes->is_treated(neighbourS)))
		  neighbour_stones->push_back(neighbourS);
	    }
	}
    }
  
  if(neighbourW>=0)
    {
      int color_neighbourW = gob.get_stone(neighbourW);
      if(color_neighbourW==GE_WITHOUT_STONE)
	{
	  if(mk_eyes) mk_eyes->set_treated(neighbourW);
	  if(empty_location!=GE_PASS_MOVE) return false;
	  empty_location = neighbourW;
	}
      else
	{
	  if(color_neighbourW!=color_stone)
	    return false;
	  else
	    {
	      if(neighbour_stones)
		if((not mk_eyes)||(not mk_eyes->is_treated(neighbourW)))
		  neighbour_stones->push_back(neighbourW);
	    }
	}
    }
  
  if(neighbourE>=0)
    {
      int color_neighbourE = gob.get_stone(neighbourE);
      if(color_neighbourE==GE_WITHOUT_STONE)
	{
	  if(mk_eyes) mk_eyes->set_treated(neighbourE);
	  if(empty_location!=GE_PASS_MOVE) return false;
	  empty_location = neighbourE;
	}
      else
	{
	  if(color_neighbourE!=color_stone)
	    return false;
	  else
	    {
	      if(neighbour_stones)
		if((not mk_eyes)||(not mk_eyes->is_treated(neighbourE)))
		  neighbour_stones->push_back(neighbourE);
	    }
	}
    }
  
  if(empty_location!=GE_PASS_MOVE)
    {
      if(mode_recursive)
	{
	  //cerr<<move_to_string(GE_Directions::board_size, empty_location)<<endl;
	  return is_possible_eye(gob, empty_location, mk_eyes, 
				 neighbour_stones, false, true);
	}
      
      if(mode_two)
	{
	  return true;
	}
      else return false;
    }
  
  return true;
}


int GE_Chain::get_eye_candidates(const GE_Goban& gob, list<int>& l_eyes, 
			      GE_Marker* mk_eyes, 
			      list<list<int> >* all_neighbours) const
{
  list<int>::const_iterator i_fi = free_intersections.begin();
  
  while(i_fi!=free_intersections.end())
    {
      if((not mk_eyes)||(not mk_eyes->is_treated(*i_fi)))
	{
	  list<int> neighbour_stones;
	  if(is_possible_eye(gob, *i_fi, mk_eyes, &neighbour_stones, 
			     true))
	    {
	      l_eyes.push_back(*i_fi);
	      if(all_neighbours)
		{
		  //TODO verifier la copie correcte de neighbour_stones
		  all_neighbours->push_back(neighbour_stones);
		}
	    }
	}
      
      i_fi++;
    }  

  //cerr<<"candidate eyes: ";
  //GE_ListOperators::print_error(l_eyes, move_to_string, GE_Directions::board_size);
  //cerr<<endl;

  return (int)l_eyes.size();
}


bool GE_Chain::is_alive(const GE_Goban& gob, GE_Marker* mk_eyes) const
{
  list<int> l_eyes;
  list<list<int> > all_neighbours;
  
  //Des tests a mettre pour accelerer
  if(is_atari())
    return false;
  
  
  if(liberties==2)
    {
      const int& lib1 = free_intersections.front();
      const int& lib2 = free_intersections.back();
      
      if(GE_Bibliotheque::distance_pion[lib1][lib2]==1)
	return false;
    }
  
  bool is_allocated = false;
  
  if(not mk_eyes)
    {
      mk_eyes = new GE_Marker(GE_Directions::board_area);
      is_allocated = true;
    }
  else mk_eyes->reinit();
  
  mk_eyes->update_treatment();
  
  list<int>::const_iterator i_l = links.begin();
  while(i_l!=links.end())
    {
      mk_eyes->set_treated(*i_l);
      
      i_l++;
    }    
  
  int nb_possible_eyes = 
    get_eye_candidates(gob, l_eyes, mk_eyes, &all_neighbours);
  
  if(nb_possible_eyes<2)
    {
      if(is_allocated) 
	{
	  delete mk_eyes;
	  mk_eyes = 0;
	}
      
      return false;
    }
  
  
  if(nb_possible_eyes==2)
    {
      const int& possible_eye1 = l_eyes.front();
      const int& possible_eye2 = l_eyes.back();
      
      if(GE_Bibliotheque::distance_pion[possible_eye1][possible_eye2]==1)
	{
	  
	  if(is_allocated) 
	    {
	      delete mk_eyes;
	      mk_eyes = 0;
	    }
	  
	  return false;
	}
    }

  list<int>::const_iterator i_le = l_eyes.begin();
  list<list<int> >::const_iterator i_an = all_neighbours.begin();
  
  int nb_eyes = 0;
  while(i_le!=l_eyes.end())
    {
      mk_eyes->update_treatment();
      
      if(i_an->empty())
	{
	  //cerr<<move_to_string(GE_Directions::board_size, *i_le)<<endl;
	  nb_eyes++;
	  
	  if(nb_eyes==2) 
	    {
	      if(is_allocated) 
		{
		  delete mk_eyes;
		  mk_eyes = 0;
		}
	      
	      return true;
	    }

	   i_le++;
	   i_an++;
	   continue;
	}
      
      GE_Chain temp_ch;
      temp_ch.init(gob, i_an->front());
      
      int memory = mk_eyes->get_memory(temp_ch.links.front());
      assert(memory<=1);
      
      
      list<int>::const_iterator i_lk = temp_ch.links.begin();
      if(i_an->size()==1)
	{
	  if(memory==1)
	    {
	      if(is_allocated) 
		{
		  delete mk_eyes;
		  mk_eyes = 0;
		}

	      return true;
	    }
	  
	  i_lk = temp_ch.links.begin();
	  while(i_lk!=temp_ch.links.end())
	    {
	      mk_eyes->increment(*i_lk);
	      i_lk++;
	    }
	  
	  i_le++;
	  i_an++;
	  continue;
	}
      
      i_lk = temp_ch.links.begin();
      while(i_lk!=temp_ch.links.end())
	{
	  mk_eyes->set_treated(*i_lk);  
	  i_lk++;
	}
      
      
      bool too_neighbours = false;
      list<int>::const_iterator i_ns = i_an->begin();
      while(i_ns!=i_an->end())
	{
	  if(not mk_eyes->is_treated(*i_ns))
	    {
	      too_neighbours = true;
	      break;
	    }
	  
	  i_ns++;
	}
      
      if(too_neighbours)
	{
	  if(memory>0)
	    {
	      i_lk = temp_ch.links.begin();
	      while(i_lk!=temp_ch.links.end())
		{
		  mk_eyes->memorize(*i_lk, memory);
		  i_lk++;
		}
	    }
	  //cerr<<"false eye: ";
	  //cerr<<move_to_string(GE_Directions::board_size, *i_le)<<endl;

	}
      else
	{
	  if(memory==1)
	    {
	      if(is_allocated) 
		{
		  delete mk_eyes;
		  mk_eyes = 0;
		}
    
	      return true;
	    }
	  else
	    {
	      i_lk = temp_ch.links.begin();
	      while(i_lk!=temp_ch.links.end())
		{
		  mk_eyes->increment(*i_lk);
		  i_lk++;
		} 
	    }
	}  
           
      i_le++;
      i_an++;
    }
  
  
  if(is_allocated) 
    {
      delete mk_eyes;
      mk_eyes = 0;
    }
  
  
  return false;
}


void GE_Chain::print() const
{
  const string (*pf)(int, int) = &move_to_string;

  //GE_ListOperators lo;
  cout<<"chain:"<<endl;
  GE_ListOperators::print(this->links, *pf, height);

  cout<<"free intersections:"<<endl;
  GE_ListOperators::print(this->free_intersections, *pf, height);

  cout<<"degree of liberty: "<<liberties<<endl;
  cout<<"color of the chain: "<<stone_to_string(stone)<<endl;
}


void GE_Chain::print_error() const
{
  const string (*pf)(int, int) = &move_to_string;
  
  //GE_ListOperators lo;
  cerr<<"chain:"<<endl;
  GE_ListOperators::print_error(this->links, *pf, height);
  
  cerr<<"free intersections:"<<endl;
  GE_ListOperators::print_error(this->free_intersections, *pf, height);
  
  cerr<<"degree of liberty: "<<liberties<<endl;
  cerr<<"color of the chain: "<<stone_to_string(stone)<<endl;
}



GE_Chains::GE_Chains(const GE_Goban& gob)
{
  this->init(gob);
}


void GE_Chains::init(const GE_Goban& gob)
{ 
  GE_Allocator all;
  int** marker;
  all.init(&marker, gob.height, gob.width, GE_NOT_SEEN);
  
  for(int i = 0; i<gob.height; i++)
    for(int j = 0; j<gob.width; j++)
      {
	if(gob.board[i][j]==GE_WITHOUT_STONE) continue;
	if(marker[i][j]==GE_SEEN) continue;
	
	sets.push_back(GE_Chain());
	(sets.back()).init(gob, &marker, i, j);
      }
      
  all.desallocate(&marker, gob.height);
}



// void GE_Chains::neighbour_chains(const GE_Goban& gob, const int stone)
// { 
//   if(stone==GE_PASS_MOVE)
//     return;

//   const pair<int, int> coord = int_to_pair(gob.height, stone);
//   const int& i = coord.first;
//   const int& j = coord.second;

//   GE_Allocator all;
//   int** marker;
//   all.init(&marker, gob.height, gob.width, GE_NOT_SEEN);
 
//   if((GE_IS_IN_GOBAN(i+1, j, gob.height, gob.width))
//      &&(marker[i+1][j]==GE_NOT_SEEN)
//      &&(gob.board[i+1][j]!=GE_WITHOUT_STONE))
//     {
//       sets.push_back(GE_Chain());
//       (sets.back()).init(gob, &marker, i+1, j);
//     }

//   if((GE_IS_IN_GOBAN(i-1, j, gob.height, gob.width))
//      &&(marker[i-1][j]==GE_NOT_SEEN)
//      &&(gob.board[i-1][j]!=GE_WITHOUT_STONE))
//     {
//       sets.push_back(GE_Chain());
//       (sets.back()).init(gob, &marker, i-1, j);
//     }

//   if((GE_IS_IN_GOBAN(i, j-1, gob.height, gob.width))
//      &&(marker[i][j-1]==GE_NOT_SEEN)
//      &&(gob.board[i][j-1]!=GE_WITHOUT_STONE))
//     {
//       sets.push_back(GE_Chain());
//       (sets.back()).init(gob, &marker, i, j-1);
//     }
 
//   if((GE_IS_IN_GOBAN(i, j+1, gob.height, gob.width))
//      &&(marker[i][j+1]==GE_NOT_SEEN)
//      &&(gob.board[i][j+1]!=GE_WITHOUT_STONE))
//     {
//       sets.push_back(GE_Chain());
//       (sets.back()).init(gob, &marker, i, j+1);
//     }
  
//   if((marker[i][j]==GE_NOT_SEEN)
//      &&(gob.board[i][j]!=GE_WITHOUT_STONE))
//     {
//       sets.push_back(GE_Chain());
//       (sets.back()).init(gob, &marker, i, j);
//     }

//   all.desallocate(&marker, gob.height); 
// }


void GE_Chains::neighbour_chains(const GE_Goban& gob, const int stone, int*** marker)
{ 
  if(stone==GE_PASS_MOVE)
    return;
  
  const pair<int, int> coord = int_to_pair(gob.height, stone);
  const int& i = coord.first;
  const int& j = coord.second;
  
  bool is_allocated = false;
  if(not marker)
    {
      marker = new int**;
      GE_Allocator::init(marker, gob.height, gob.width, GE_NOT_SEEN);
      is_allocated = true;
    }
  
  
  if((GE_IS_IN_GOBAN(i+1, j, gob.height, gob.width))
     &&((*marker)[i+1][j]==GE_NOT_SEEN)
     &&(gob.board[i+1][j]!=GE_WITHOUT_STONE))
    {
      sets.push_back(GE_Chain());
      (sets.back()).init(gob, marker, i+1, j);
    }
  
  if((GE_IS_IN_GOBAN(i-1, j, gob.height, gob.width))
     &&((*marker)[i-1][j]==GE_NOT_SEEN)
     &&(gob.board[i-1][j]!=GE_WITHOUT_STONE))
    {
      sets.push_back(GE_Chain());
      (sets.back()).init(gob, marker, i-1, j);
    }
  
  if((GE_IS_IN_GOBAN(i, j-1, gob.height, gob.width))
     &&((*marker)[i][j-1]==GE_NOT_SEEN)
     &&(gob.board[i][j-1]!=GE_WITHOUT_STONE))
    {
      sets.push_back(GE_Chain());
      (sets.back()).init(gob, marker, i, j-1);
    }
  
  if((GE_IS_IN_GOBAN(i, j+1, gob.height, gob.width))
     &&((*marker)[i][j+1]==GE_NOT_SEEN)
     &&(gob.board[i][j+1]!=GE_WITHOUT_STONE))
    {
      sets.push_back(GE_Chain());
      (sets.back()).init(gob, marker, i, j+1);
    }
  
  if(((*marker)[i][j]==GE_NOT_SEEN)
     &&(gob.board[i][j]!=GE_WITHOUT_STONE))
    {
      sets.push_back(GE_Chain());
      (sets.back()).init(gob, marker, i, j);
    }
  
  if(is_allocated)
    {
      GE_Allocator::desallocate(marker, gob.height); 
      delete marker;
      marker = 0;
    }
}



void GE_Chains::neighbour_chains(const GE_Goban& gob, const GE_Chain& ch, int*** marker)
{ 
  bool is_allocated = false;
  
  if(not marker)
    {
      marker = new int**;
      GE_Allocator::init(marker, gob.height, gob.width, GE_NOT_SEEN);
      is_allocated = true;
    }
  
  list<int>::const_iterator i_l = ch.links.begin();
  
  while(i_l!=ch.links.end())
    {
      this->neighbour_chains(gob, *i_l, marker);
      i_l++;
    }
  
  if(is_allocated)
    {
      GE_Allocator::desallocate(marker, gob.height); 
      delete marker;
      marker = 0;
    }
}

void GE_Chains::neighbour_chains(const GE_Goban& gob, const GE_Chains& chs, int*** marker)
{ 
  bool is_allocated = false;
  
  if(not marker)
    {
      marker = new int**;
      GE_Allocator::init(marker, gob.height, gob.width, GE_NOT_SEEN);
      is_allocated = true;
    }
  
  list<GE_Chain>::const_iterator i_ch = chs.sets.begin();
  
  while(i_ch!=chs.sets.end())
    {
      this->neighbour_chains(gob, *i_ch, marker);
      i_ch++;
    }
  
  if(is_allocated)
    {
      GE_Allocator::desallocate(marker, gob.height);
      delete marker;
      marker = 0;
    }
}




//return the number of dead chain
int GE_Chains::get_nb_dead(const team_color& col, int* val_potential) const
{
  if(val_potential) *val_potential = 0;
  int nb_dead = 0;
  list<GE_Chain>::const_iterator i_s = (this->sets).begin();
  
  while(i_s!=(this->sets).end())
    {
      if(i_s->is_dead())
	{
	  switch(col)
	    {
	    case GE_NEUTRAL_COLOR : nb_dead++; break;
	    case GE_WHITE : 
	      if(i_s->stone==GE_WHITE_STONE)
		{
		  (*val_potential) += (i_s->links).size();
		  nb_dead++;
		}
	      break;
	    case GE_BLACK :
	      if(i_s->stone==GE_BLACK_STONE)
		{
		  (*val_potential) += (i_s->links).size();
		  nb_dead++;
		}
	    default : ;
	    }
	}

      i_s++;
    }
  
  return nb_dead;
}

int GE_Chains::get_nb_atari(const team_color& col, int* val_threat, 
			 list<int>* free_places) const
{
  if(val_threat) *val_threat = 0;
  int nb_atari = 0;
  int color_stone = (col==GE_WHITE) ? GE_WHITE_STONE : GE_BLACK_STONE;
  list<GE_Chain>::const_iterator i_s = (this->sets).begin();
  
  while(i_s!=(this->sets).end())
    {
      if(not i_s->is_atari())
	{
	  i_s++;
	  continue;
	}
      
      if(col==GE_NEUTRAL_COLOR)
	{
	  nb_atari++;
	  i_s++;
	  continue;
	}
      
      if(color_stone==i_s->stone)
	{
	  if(val_threat) (*val_threat) += (i_s->links).size();
	  if(free_places)
	    if(not i_s->free_intersections.empty())
	      free_places->push_back(i_s->free_intersections.back());
	  nb_atari++;
	}
      
      i_s++;
    }
  
  return nb_atari;
}

void GE_Chains::get_weaker_chain(GE_Chain& ch) const
{
  list<GE_Chain>::const_iterator i_ch = this->sets.begin();

  while(i_ch!=this->sets.end())
    {
      //(1) degree of liberties
      if(i_ch->liberties > ch.liberties)
	{
	  i_ch++;
	  continue;
	}
      
      if(i_ch->liberties < ch.liberties)
	{
	  ch = *i_ch;
	  i_ch++;
	  continue;
	}
      
      //(2) the fattest chain
      if(i_ch->size() < ch.size())
	{
	  i_ch++;
	  continue;
	}

      if(i_ch->size() > ch.size())
	{
	  ch = *i_ch;
	  i_ch++;
	  continue;
	}

      //(3) the most isolated chain
      //to implement
      
      i_ch++;
    }
}

void GE_Chains::print() const 
{
  list<GE_Chain>::const_iterator i_c = sets.begin();
  int num = 1;

  while(i_c!=sets.end())
    {
      cout<<"chain n°"<<num<<":"<<endl;
      i_c->print();
      cout<<endl;
      i_c++;
      num++;
    }

}

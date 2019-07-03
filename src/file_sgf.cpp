#include "file_sgf.h"

//GE_FileSGF::GE_FileSGF(const char* name)
GE_FileSGF::GE_FileSGF(const string& name, int nb_moves)
{
  file_name = name;
  size_goban = GE_DEFAULT_SIZE_GOBAN;
  komi = -1.f;
  this->read_game(nb_moves);
}

void GE_FileSGF::restart(ifstream& f)
{
  if(f.eof())
    f.clear();

  f.seekg(0);
}

bool GE_FileSGF::read_move(ifstream& f, string& s_mv) const
{
  s_mv = "";
  
  char c;
  if(f.eof()) 
    return false;
  
  f>>c;
  
  if(c==']')
    {
      s_mv = "PASS";
      return true;
    }
  
  s_mv+=c;
  if(f.eof())
    return false;
  
  f>>c;
  s_mv+=c;

  if(f.eof())
    return false;
  
  f>>c;
  if(f.eof())
    return false;
  
  if(c!=']')
    s_mv = "";

  return true;
}


bool GE_FileSGF::read_size(ifstream& f)
{
  char precedent_char = ' ';
  char current_char = ' ';
  while(not f.eof())
    {
      f>>current_char;
     
      if(current_char=='Z')
	if(precedent_char=='S')
	  {
	    if(f.eof()) break;
	    f>>current_char;
	    if(current_char=='[')
	      {
		if(f.eof()) break;
		
		f>>this->size_goban;

		break;
	      }
	  }
      
      precedent_char = current_char;
    }

  return true;
}

bool GE_FileSGF::read_komi(ifstream& f)
{
  char precedent_char = ' ';
  char current_char = ' ';
  while(not f.eof())
    {
      f>>current_char;
     
      if(current_char=='M')
	if(precedent_char=='K')
	  {
	    if(f.eof()) break;
	    f>>current_char;
	    if(current_char=='[')
	      {
		if(f.eof()) break;
		
		f>>this->komi;

		break;
	      }
	  }
      
      precedent_char = current_char;
    }

  return true;
}


bool GE_FileSGF::read_position(ifstream& f)
{
  char precedent_char = ' ';
  char current_char = ' ';

  bool is_read = false;

  while(not f.eof())
    {
      f>>current_char;
      
      if((current_char=='B')&&(precedent_char=='A'))
	{
	  if(f.eof()) break;
	  
	  while(true)
	    {
	      f>>current_char;
	      if(current_char=='[')
		{
		  string s_mv;
		  
		  if(not this->read_move(f, s_mv))
		    {
		      f.close();
		      assert(0);
		      return false;
		    }
		  
		  
		  pair<int, int> mv = string_to_move(s_mv);
		  black_stones.push_back(pair_to_int(size_goban, mv));
		  is_read = true;
		}
	      else break;
	      
	      if(f.eof()) break; 
	    }
	  
	  break;
	}
          
      precedent_char = current_char;
    }
  
  //we replace the file pointer at the beginning
  restart(f); //f.seekg(0);
  while(not f.eof())
    {
      f>>current_char;
      
      if((current_char=='W')&&(precedent_char=='A'))
	{
	  if(f.eof()) break;
	  
	  while(true)
	    {
	      f>>current_char;
	      if(current_char=='[')
		{
		  string s_mv;
		  
		  if(not this->read_move(f, s_mv))
		    {
		      f.close();
		      assert(0);
		      return false;
		    }
		  
		  
		  pair<int, int> mv = string_to_move(s_mv);
		  white_stones.push_back(pair_to_int(size_goban, mv));

		  is_read = true;
		}
	      else break;
	      
	      if(f.eof()) break; 
	    }
	  
	  break;
	}
      
      precedent_char = current_char;
    }
  
  
  return is_read;
}



bool GE_FileSGF::read_game(int nb_moves)
{
  int num_move = 0;

  //ifstream f(file_name);
  ifstream f(file_name.c_str());
  
  if(not f.is_open())
    return false;

  //the reading of the size of the goban
  this->read_komi(f);

  //we replace the file pointer at the beginning
  f.seekg(0);

  //the reading of the size of the goban
  this->read_size(f);
  
  //we replace the file pointer at the beginning
  f.seekg(0);
  
  if(this->read_position(f))
    return true; //for this moment, we don't mix game and position
  
  
  restart(f);
  char c;
  string s_mv;
  while(not f.eof())
    {
      f>>c;

      if((c=='B')||(c=='W'))
	{
	  if(f.eof()) 
	    {
	      f.close();
	      return false;
	    }
	  f>>c;
	  
	  if(c=='[')
	    {
	      if(not this->read_move(f, s_mv))
		{
		  f.close();
		  return false;
		}
	      
	      if(s_mv=="")
		continue;

	      pair<int, int> mv = string_to_move(s_mv);
	      game.push_back(pair_to_int(size_goban, mv));
	    }
	}

      num_move++;
      if(num_move==nb_moves) break;
    }




  f.close();
  
  return true;
}


void GE_FileSGF::print() const
{
  const string (*pf)(int, int) = &move_to_string;

  cout<<"game:"<<endl;
  cout<<"file: "<<file_name<<endl;

  GE_ListOperators lo;
  cout<<"size of goban: "<<size_goban<<endl;

  lo.print(this->game, *pf, size_goban);
}

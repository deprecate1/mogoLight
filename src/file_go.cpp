
#include "file_go.h"

bool GE_InfosPosition::load(const char* name)
{
  report = "";

  string failure = "FAILURE\n";
  string success = "SUCCESS\n";

  if(not name)
    {
      if((this->filename).size()==0)
	{
	  report = "*** ERROR *** name of file not found\n";
	  report = report + failure;
	  
	  return false;
	}
      
      this->load((this->filename).c_str());
    }
  
  filename = name;
  
  ifstream go_stream(name);
  
  if(not go_stream.is_open())
    {
      report += "*** ERROR *** file not found\n";
      report += failure;
      return false;
    }
  else report+= "--- SMILE --- the file has been found\n";
  
  list<string> rows;
  string last_line;
  if(not this->read_position(go_stream, rows, last_line))
    {
      report += "*** ERROR *** the reading of position has failed\n";
      report += failure;
      return false;
    }
  else report += "--- SMILE --- the reading of position is successful\n";

  if(not this->translation(rows))
    {
      report+=failure;
      return false;
    }

  go_stream.close();
  
  


  report += success;
  return true;
}


bool GE_InfosPosition::read_position(ifstream& go_stream, list<string>& rows, string& last_line)
{
  last_line = "";
  rows.clear();

  unsigned int w = 0;
  
  while(not go_stream.eof())
    {
      string row;
      
      go_stream>>row;

      if(row.size()==0)
	continue;
      
      if(w<=0)
	w = row.size();
      
      if(row.size()!=w)
	last_line = row;
	  
      rows.push_back(row);
    }
  
  this->height = (int)rows.size();
  this->width = (int)w;

  if(rows.size()==0)
    return false;
  
  
  return true;
}

bool GE_InfosPosition::is_in_characters_used(char c, int* nb) const
{
  list<pair<char, int> >::const_iterator i_cu = characters_used.begin();
  while(i_cu!=characters_used.end())
    {
      if(i_cu->first==c)
	{
	  if (nb) *nb = i_cu->second;
	  return true;
	}
      
      i_cu++;
    }
  
  return false;
}


bool GE_InfosPosition::translation(list<string>& rows)
{
  //inventory of characters
  if(inventory_characters(rows))
    report+="   +++ SMILE +++ the inventory of characters is successful\n";
  else
    {
      report+="   ### ERROR ### the inventory of characters is successful\n";
      return false;
    }
  
  //verification of the characters number 
  if(characters_used.size()<=1)
    {
      report+="   ### ERROR ### the verification of characters number has failed\n";
      report+="      ---> the numbers of used characters is too small\n";
      return false;
    }
  
  if(characters_used.size()>=8)
    {
      report+="   ### ERROR ###  the verification of characters number has failed\n";
      report+="      ---> the numbers of used characters is too big\n";
    }
  else
    report+="   +++ SMILE +++ the verification of characters number is successful\n"; 
  

  //translation of characters
  int nb = 0;
  if((is_in_characters_used('W'))&&(is_in_characters_used('B')))
    {
      to_black = make_pair('B', GE_BLACK_STONE);
      to_white = make_pair('W', GE_WHITE_STONE);
    }
  
  if((is_in_characters_used('w'))&&(is_in_characters_used('b')))
    {
      to_black = make_pair('b', GE_BLACK_STONE);
      to_white = make_pair('w', GE_WHITE_STONE);
    }

  if((is_in_characters_used('b'))&&(is_in_characters_used('n')))
    {
      to_black = make_pair('n', GE_BLACK_STONE);
      to_white = make_pair('b', GE_WHITE_STONE);
    }

  if((is_in_characters_used('B'))&&(is_in_characters_used('N')))
    {
      to_black = make_pair('N', GE_BLACK_STONE);
      to_white = make_pair('B', GE_WHITE_STONE);
    }

  if(is_in_characters_used('@'))
    to_black = make_pair('@', GE_BLACK_STONE);
  
  if(is_in_characters_used('O'))
    to_white = make_pair('O', GE_WHITE_STONE);

  if(is_in_characters_used('0'))
    to_white = make_pair('0', GE_WHITE_STONE);

  if(is_in_characters_used('.'))
    to_free = make_pair('.', GE_WITHOUT_STONE);

  if(is_in_characters_used(' '))
    nothing = ' ';
  
  if(is_in_characters_used('#'))
    frame = '#';

  if(is_in_characters_used('*'))
    frame = '*';
  
  if(is_in_characters_used('-'))
    frame = '-';

  if(is_in_characters_used('(', &nb))
    {
      if(nb==1)
	last_move_before = '(';
    }
  
  if(is_in_characters_used(')', &nb))
    {
      if(nb==1)
	last_move_after = ')';
    }

  if(is_in_characters_used('[', &nb))
    {
      if(nb==1)
	last_move_before = '[';
    }
  
  if(is_in_characters_used(']', &nb))
    {
      if(nb==1)
	last_move_after = ']';
    }
  
  if(is_in_characters_used('{', &nb))
    {
      if(nb==1)
	last_move_before = '{';
    }
  
  if(is_in_characters_used('}', &nb))
    {
      if(nb==1)
	last_move_after = '}';
    }
  
  //TO GO ON
  
  


  return true;
}


bool GE_InfosPosition::inventory_characters(const list<string>& rows)
{
  list<string>::const_iterator i_r = rows.begin();

  while(i_r!=rows.end())
    {
      for(int i_c = 0; i_c<(int)i_r->size(); i_c++)
	{
	  const char c = (*i_r)[i_c];

	  list<pair<char, int> >::iterator i_cu = this->characters_used.begin();

	  while(i_cu!=characters_used.end())
	    {
	      if(c==i_cu->first)
		{
		  (i_cu->second)++;
		  break;
		}
	      
	      i_cu++;
	    }
	  
	  if(i_cu==characters_used.end())
	    characters_used.push_back(make_pair(c, 1));
	}
      
      
      i_r++;
    }

  return true;
}

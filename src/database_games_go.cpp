#include "database_games_go.h"

const string GE_DbGamesGo::default_name_list_files = "files_game.txt";

GE_DbGamesGo::GE_DbGamesGo()
{
  name_list_files = 0;
  nb_failed_files = 0;
  nb_files = 0;
  this->load();
}

GE_DbGamesGo::GE_DbGamesGo(const char* list_filename)
{
  name_list_files = list_filename;
  nb_failed_files = 0;
  nb_files = 0;
  this->load();
}


bool GE_DbGamesGo::load(const char* list_files)
{
  if(not list_files)
    {
      if(name_list_files)
	return this->load(name_list_files);
      else return this->load(default_name_list_files.c_str());
    }

  ifstream f_list(list_files);
  if(not f_list.is_open())
    {
      nb_files = 0;
      return false;
    }

  while(not f_list.eof())
    {
      string filename;
      f_list>>filename;
      
      if(filename=="") continue;

      try
	{
	  games.push_back(GE_FileSGF(filename));
	  //(games.back()).print();
	  nb_files++;
	}
      catch (...)
	{
	  nb_failed_files++;
	  failed_filenames.push_back(filename);
	}
    }

  f_list.close();
  
  return true;
}


int GE_DbGamesGo::get_nb_files() const
{
  return nb_files;
}

int GE_DbGamesGo::get_nb_failed_files() const
{
  return nb_failed_files;
}

int GE_DbGamesGo::get_max_length_game(int height, int width) const
{
  unsigned int max_length_game = 0;
  
  list<GE_FileSGF>::const_iterator i_sgf = games.begin();
  
  while(i_sgf!=games.end())
    {
      if((i_sgf->size_goban!=height)
	 ||(i_sgf->size_goban!=width))
	{
	  i_sgf++;
	  continue;
	}
      
      
      if((i_sgf->game).size()>max_length_game)
	max_length_game = (i_sgf->game).size();
      
      i_sgf++;
    }

  return (int)max_length_game;
}



void GE_DbGamesGo::print_failed_files() const
{
  cout<<"number of failed files: "<<nb_failed_files<<endl;
  
  list<string>::const_iterator i_f = failed_filenames.begin();
  cout<<"list of failed files:"<<endl;
  while(i_f!=failed_filenames.end())
    {
      cout<<*i_f<<endl;
      i_f++;
    }
}

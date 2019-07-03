#ifndef GE_DATABASE_GAMES_GO_H
#define GE_DATABASE_GAMES_GO_H

#include "file_sgf.h"

struct GE_DbGamesGo
{
  list<GE_FileSGF> games;
  
  private:
  const char* name_list_files; 
  static const string default_name_list_files;
  //name of the file containing the filenames of the database
  
  int nb_files;
  int nb_failed_files;
  list<string> failed_filenames;
  
  public:
  GE_DbGamesGo();
  GE_DbGamesGo(const char*);
  ~GE_DbGamesGo() {}
  
  bool load(const char* = 0);
  int get_nb_files() const;
  int get_nb_failed_files() const;

  int get_max_length_game(int = GE_DEFAULT_SIZE_GOBAN, int = GE_DEFAULT_SIZE_GOBAN) const;

  void print_failed_files() const;
};


#endif

//
// C++ Interface: gogametree_mg
//
// Description:
//
//
// Author: Yizao Wang <yizao.wang@polytechnique.edu> and Sylvain Gelly <gelly@lri.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MOGOOB_MG_H
#define MOGOOB_MG_H

#include "typedefs_mg.h"
#include <ext/hash_map>

#include <vector>
#include <list>

using namespace std;

namespace MoGo {

  void clearDatabase(MoGoHashMap(GobanKey, vector<double>, hashGobanKey, equalGobanKey) &database); 

  class OpeningBook {
	  
  public:

  static OpeningBook* getOpeningBook()
  {
    static OpeningBook* ob = NULL;
    if (ob==NULL) 
      ob = new OpeningBook();
    return ob;
  }

  OpeningBook(); 
  ~OpeningBook(); 

  void addSGFListDir(string *lst_dir);

  void deleteOB();

  unsigned int hashMapSize( ) const;
  
  void setPlayer(int p){
    player_color = p;
  }

  void setSize(int sz) {
    size = sz;
  }

  // player 0 = black, 1 = white
  double getNumberOfWins(Goban goban, int player);
  void dump(string* fileName);
  void undump(string* fileName);

  private:

  MoGoHashMap(GobanKey, vector<double>, hashGobanKey, equalGobanKey) statistics;
  int size;
  int player_color;

  void addGametoDatabase(list<int> *move_x, 
			 list<int> *move_y, 
			 int win_color);
  
  void parseListDir (string *lst_dir, list<string> *tokens);

  int convertMove(char move);

  int tokenizeSGF(string *gamestream,
		  list<int> *move_x,
		  list<int> *move_y);
    
  int parseSGF(string &filename,
	       list<int> *move_x,
	       list<int> *move_y);

  void storeSGF(string& filename);

  void addDir(const char *dir);
  };

}

#endif

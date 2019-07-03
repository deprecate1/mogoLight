//
// C++ Implementation: gogametree_mg
//
// Description:
//
//
// Author: Yizao Wang <yizao.wang@polytechnique.edu> and Sylvain Gelly <gelly@lri.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "gogametree_mg.h"
#include "gotools_mg.h"

#include <iostream>
#include <fstream>

#include <vector>
#include <algorithm>

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>

#include "openingbook_mg.h"

using namespace std;
using namespace MoGo;

namespace MoGo {

  void clearDatabase(MoGoHashMap(GobanKey, vector<double>, hashGobanKey, equalGobanKey) &database) {
    MoGoHashMap(GobanKey, vector<double>, hashGobanKey, equalGobanKey)().swap(database);
  }

  OpeningBook::~OpeningBook() {/**assert(0);**/
    deleteOB();
  }


  void MoGo::OpeningBook::deleteOB( ) {
    /*    for (MoGoHashMap(GobanKey, vector<double>, hashGobanKey, equalGobanKey)::
	   const_iterator i = statistics.begin(); i != statistics.end() ; ++i) {
      //     delete i->second;
      delete i->second;
      }*/
    //   statistics.clear();
    clearDatabase(statistics);
  }

MoGo::OpeningBook::OpeningBook( ) {/**assert(0);**/}


unsigned int MoGo::OpeningBook::hashMapSize( ) const {
  return statistics.size();
}



  void MoGo::OpeningBook::parseListDir(string *lst_dir, list<string> *tokens) {
  
    string delimiters(";");
  
    //cerr << "Tokenize" << endl;
    //cerr << "gamestream: " << *gamestream << endl;
    
    // skip delimiters at beginning.
    string::size_type lastPos 
      = lst_dir->find_first_not_of(delimiters, 0);
    
    // find first "non-delimiter".
    string::size_type pos = lst_dir->find_first_of(delimiters, lastPos);
    
    while (string::npos != pos || string::npos != lastPos) {
      // found a token, add it to the vector.
      tokens->push_back(lst_dir->substr(lastPos, pos - lastPos));      
      // skip delimiters.  Note the "not_of"
      lastPos = lst_dir->find_first_not_of(delimiters, pos);      
      // find next "non-delimiter"
      pos = lst_dir->find_first_of(delimiters, lastPos);
    }
  }


  

  void MoGo::OpeningBook::addGametoDatabase(list<int> *move_x, 
					    list<int> *move_y, 
					    int win_color) {
    static     int indice = 0;
    indice++;

    for (int permut=0;permut<8;permut++)
    {
    //cerr << "test1" << endl;

    Goban gb(size, size);
    list<int>::iterator itx, ity;

    int cpt=0;

    for (itx =  move_x->begin(), ity =  move_y->begin(); 
	 itx != move_x->end(); itx++, ity++){

	    	cpt++;
		if (cpt>12)
			break;
      //cerr << "test2" << endl;
      
      //gb->playMoveOnGoban((*itx), (*ity));
      int x,oldx;
      int y,oldy;
      x=(*itx);
      y=(*ity);
      oldx=x;
      oldy=y;

      switch(permut) 
      {
	      case 0:
	      break;

              case 1:
	      x=size-1-x;
	      break;

              case 2:
	      y=size-1-y;
	      break;

              case 3:
	      x=size-1-x;
	      y=size-1-y;
	      break;

              case 4:
	      oldy=x; oldx=y; x=oldx; y=oldy;
	      break;

              case 5:
	      x=size-1-x;
	      oldy=x; oldx=y; x=oldx; y=oldy;
	      break;

              case 6:
	      y=size-1-y;
	      oldy=x; oldx=y; x=oldx; y=oldy;
	      break;

              case 7:
	      x=size-1-x;
	      y=size-1-y;
	      oldy=x; oldx=y; x=oldx; y=oldy;
	      break;

	      default:
	      exit(-1);
	      break;
      }
      

      gb.playMoveOnGoban(x,y);
      //      if (indice > 1000)
      //gb->textModeShowGoban();

     
      GobanKey curr_key =  gb.getGobanKey();
      
      vector<double> curr_vect = statistics[curr_key];

      if (curr_vect.size() == 0) {

	//fprintf(stderr,"adding a new vector for gobankey ");GoTools::print(curr_key);fprintf(stderr,"\n");
	vector<double> new_vect;

	new_vect.push_back(1-win_color);
	new_vect.push_back(win_color);

	statistics[curr_key] = new_vect;
      }
      else {	
	//cerr << "test3" << endl;
	curr_vect[win_color] = curr_vect[win_color] + 1;

	//cerr << "test4" << endl;
	statistics[curr_key] = curr_vect;
	  //    fprintf(stderr,"incrementing statistics for gobankey ");GoTools::print(curr_key);fprintf(stderr,"\n");
		//	      fprintf(stderr," --> %g %g \n",curr_vect[0],curr_vect[1]);

      }

    }
    //cerr << "test5" << endl;
   }
  }


  // convert a position in sgf format to number

  int  MoGo::OpeningBook::convertMove(char move) {
    
    if (move == 'a')
      return 0;
    if (move == 'b')
      return 1;
    if (move == 'c')
      return 2;
    if (move == 'd')
      return 3;
    if (move == 'e')
      return 4;
    if (move == 'f')
      return 5;
    if (move == 'g')
      return 6;
    if (move == 'h')
      return 7;
    if (move == 'i')
      return 8;
    if (move == 'j')
      return 9;
    if (move == 'k')
      return 10;
    if (move == 'l')
      return 11;
    if (move == 'm')
      return 12;
    if (move == 'n')
      return 13;
    if (move == 'o')
      return 14;
    if (move == 'p')
      return 15;
    if (move == 'q')
      return 16;
    if (move == 'r')
      return 17;
    if (move == 's')
      return 18;
    
    // cout << move << endl;
    return -1;
  }


// 0 si noir gagne
// 1 si blanc gagne
int MoGo::OpeningBook::tokenizeSGF(string *gamestream,
				   list<int> *move_x,
				   list<int> *move_y){
  int win_color = -1;
  list<string> tokens;
  string delimiters(";");
 
  //cerr << "Tokenize" << endl;
  //cerr << "gamestream: " << *gamestream << endl;

  // skip delimiters at beginning.
  string::size_type lastPos 
    = gamestream->find_first_not_of(delimiters, 0);
    	
  // find first "non-delimiter".
  string::size_type pos = gamestream->find_first_of(delimiters, lastPos);

  while (string::npos != pos || string::npos != lastPos) {
      // found a token, add it to the vector.
      tokens.push_back(gamestream->substr(lastPos, pos - lastPos));      
      // skip delimiters.  Note the "not_of"
      lastPos = gamestream->find_first_not_of(delimiters, pos);      
      // find next "non-delimiter"
      pos = gamestream->find_first_of(delimiters, lastPos);
  }
  
   for (list<string>::iterator it = tokens.begin(); 
       it != tokens.end(); it++) {

     string str = (*it);
     // cout << "token: " << str << endl;

     if (str.find("B[", 0) == 0 || str.find("W[", 0) == 0){
       if (str.find("B[]", 0) == 0 || str.find("W[]", 0) == 0)
	 //continue;
	 return win_color;

       // cerr << "Move : " << (int) (convertMove(str[2])) << " - " << (int)(convertMove(str[3])) << endl;       

       move_x->push_back(convertMove(str[2]));
       move_y->push_back(convertMove(str[3]));
     }

     // Verifie que le joueur considere a gagne la partie
     // on utilise le champs RE de .sgf
     // cerr << str << " - pos : " << pos << endl;

     int pos = str.find("RE[", 0);          
     if (pos >= 0){
       if (str[pos+3] == 'w' || str[pos+3] == 'W')
	 win_color = 1;
       if (str[pos+3] == 'b' || str[pos+3] == 'B')
	 win_color = 0;
     }
   }
   return win_color;
}



  // Parse a SGF file and fill move_x and move_y with  the list of moves
  // Le fichier renvoi false si le fichier n'est pas interessant
  // car le joueur considéré n'a pas gagné la partie
  int MoGo::OpeningBook::parseSGF(string &filename,
				  list<int> *move_x,
				  list<int> *move_y) {
    
    //cerr << "parseSGF" << endl;
    
    const char* f = filename.c_str();
    ifstream inFile(f);
    
    if (!inFile) {
      // cerr << "Unable to open file datafile.txt";
      exit(1);
    }
    
    std::string gamestream;
    std::string line;

    // cout << "---------------------" << endl;

    while (inFile.good()) {
      
      inFile >> line;
      gamestream.insert(gamestream.length(), 
			line);    
      line = "";      
      // cout << line << endl << endl;      
    }
    
    //cout << "begin tokenize" << endl;  
    int win_color = tokenizeSGF(&gamestream, move_x, move_y);
    inFile.close();
    return win_color;
    //cout << "end tokenize" << endl;
    
   
    //list<int>::iterator itx, ity;
    //for (itx =  move_x->begin(), ity =  move_y->begin(); 
    //     itx != move_x->end(); itx++, ity++)
    //  cout << (*itx) << " - " << (*ity) << endl;
  }



  // store an SGF file the opening database
  void MoGo::OpeningBook::storeSGF(string& filename) {
    
    //cerr << "storeSGF" << endl;
    
    static int nb = 0;
    
    list<int> *move_x = new list<int> ();
    list<int> *move_y = new list<int> ();
    
    //cout << " parseSGF ..." << endl;
    int win_color;
    if ((win_color = parseSGF(filename, move_x, move_y)) != -1){
      nb++;


      if (nb%100==0) cerr << filename << " - " << nb << endl;
      
      // int win_color = parseSGF(filename, move_x, move_y);
      if (win_color == 0 || win_color == 1)
	// TODO, ajout dans l'arbre
        addGametoDatabase(move_x, move_y, win_color);
    }
    
    delete move_x;
    delete move_y;
  }




  // Add the SGFs present in a directory
  // in the opening book
  void MoGo::OpeningBook::addDir(const char *dir){
    
    cerr << "addDir" << endl;
    
    if (dir != NULL){
      
      struct dirent *lecture;
      DIR *rep;
      rep = opendir(dir);
      while ((lecture = readdir(rep)) != NULL) {    
	
	string str(dir);
	str.insert(str.length(), "/");
	str.insert(str.length(), lecture->d_name);
	
	int endPos 
	  = str.rfind(".sgf", str.size() - 1);
	
	if (endPos == (str.size() - 4))
	  storeSGF(str);	
      }
      closedir(rep);
    }    
}

  // Add a list of SGF directories to the opening book
  // the parameter string is a list of path separate by a ";"
  void MoGo::OpeningBook::addSGFListDir(string *lst_dir){
    
    std::list<string> dirs;
    list<string>::iterator it;
    
    if (lst_dir == NULL)
      return ;
    
    if (lst_dir->size() == 0)
      return ;
    
    parseListDir(lst_dir, &dirs);
    
    for (it =  dirs.begin();
	 it != dirs.end(); 
	 it++)
      addDir((*it).c_str());   
  }


}

void MoGo::OpeningBook::dump(string * fileName){
	if (fileName==NULL) return;
	if (fileName->size()==0) return;

	FILE *file;
	file=fopen(fileName->c_str(),"w");
	assert(file);
	for (MoGoHashMap(GobanKey, vector<double>, hashGobanKey, equalGobanKey)::const_iterator it = statistics.begin()
			; it != statistics.end() ; ++it) {
		GobanKey key = it->first;
		for (int i=0;i<key.size();i++)
			fprintf(file,"%u ",key[i]);
		assert(key.size()==5);
		vector<double>stats=it->second;
		fprintf(file,"%g %g\n",stats[0],stats[1]);
	}	
	fprintf(file,"0 0 0 0 0 0 0 \n");
	fclose(file);
	   
}


void MoGo::OpeningBook::undump(string * fileName)
{

	if (fileName==NULL) return;
	if (fileName->size()==0) return;

	FILE *file;
	file=fopen(fileName->c_str(),"r");
	//file=fopen(&(*fileName)[0],"r");
	assert(file);
	char buffer[5000];
	fgets(buffer,5000,file);
	int nbLines=0;
	bool stop=false;
	while (!stop)
	{
		nbLines++;
		if (nbLines%50000==0)
			fprintf(stderr,"%d gobans...\n",nbLines);
		vector<unsigned int> key;
		key.resize(5);
		vector<double> stats;
		stats.resize(2);
		double vx,vy;

		sscanf(buffer,"%u %u %u %u %u %lf %lf",&key[0],&key[1],&key[2],&key[3],&key[4],&vx,&vy);//stats[0],&stats[1]);
		//fprintf(stderr,"==>  %g %g \n",vx,vy);
		stats[0]=vx;
		stats[1]=vy;
		if (stats[0]+stats[1]+key[0]+key[1]==0)
			stop=true;
		else
		{
		//printf(buffer);
		//printf("%u %u %u %u %u %g %g \n",key[0],key[1],key[2],key[3],key[4],stats[0],stats[1]);
		statistics[key]=stats;
		fgets(buffer,5000,file);
		}
	}
	

}





double MoGo::OpeningBook::getNumberOfWins(Goban goban, int player){
  

  // On boucle sur les 8 permutations du goban
  // et on additionne les statistics

  GobanKey key = goban.getGobanKey();
 // printf("(%d)",statistics.count(key));
  if (statistics.count(key)==0) return 0;
 // printf("[%g %g]",statistics[key][0],statistics[key][1]);
  vector<double> stats = statistics[key];
 

  return stats[player];
}

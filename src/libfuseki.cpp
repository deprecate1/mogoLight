

#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>

#include <iostream>
#include <fstream>
#include <list>
#include <string>

#include <errno.h>
extern int errno; 

#include "libfuseki.h"

using namespace std;

// 
static libfuseki_node *root_white = NULL;
static libfuseki_node *root_black = NULL;

// renseigne sur le joueur a considerer.
static int player_color = -1; 


// Prototypes
static void generate_first_move(libfuseki_node *root,
				list<int> *p_move_x,
				list<int> *p_move_y,
				list<int> *count);
static void libfuseki_rotate_90(int x, int y, int size, 
				int *r_x, int* r_y);
static void libfuseki_symetrie(int x, int y, 
			       int size, int *r_x, int* r_y);
static void libfuseki_un_rotate_90(int x, int y, int size, 
				int *r_x, int* r_y);
static void libfuseki_un_symetrie(int x, int y, 
			       int size, int *r_x, int* r_y);

static libfuseki_node *create_fuseki_node(int move_x, int move_y){
  
  // cout << "create_fuseki_node: " << move_x << " - " << move_y << endl;

  libfuseki_node *new_node = new libfuseki_node();
  new_node->nb_pass = 1;
  
  // children = new list<struct libfuseki_node*>();

  // Dans le cas du noeud root
  // cela n'a pas d'importance
  new_node->move[0] = move_x;
  new_node->move[1] = move_y;
  return new_node;
}

static void buildFusekiTree(list<int> *move_x, 
			    list<int> *move_y,
			    int win_color){
  
  libfuseki_node *current_node = NULL;

  if (win_color == 0)
    current_node = root_black;
  else if (win_color == 1)
    current_node = root_white;

  //cout << "root: " << current_node->nb_pass << endl;

  list<int>::iterator itx, ity;

  for (itx =  move_x->begin(), ity =  move_y->begin(); 
       itx != move_x->end(); itx++, ity++){
    
    //cout << "  add " << (*itx) << " - " << (*ity) << endl;

    // Recherche si le coup courant est deja 
    // dans l'arbre
    bool found = false;
    list<struct libfuseki_node*>::iterator it_node, last_node;

    for (it_node =  current_node->children.begin();
	 it_node != current_node->children.end() && found != true; 
	 it_node++) {

      last_node = it_node;

      //cout << "   cherche : " 
      //	   << (*it_node)->move[0] << " - " << (*it_node)->move[1]
      //	   << endl;

      if ((*it_node)->move[0] == (*itx) && (*it_node)->move[1] == (*ity))
	found = true;
    }

    if (found == true) {
      
      // cout << "   node found" << endl; 
      // cout << "   move: " << (*last_node)->move[0] << "-" << (*last_node)->move[1] <<endl; 
      (*last_node)->nb_pass++;
      current_node = (*last_node);
      // cout << "   end node found" << endl;
 
    } else {
      // cout << "   add_new_node" << (*itx) << " - " << (*ity) << endl;
      libfuseki_node *new_node = create_fuseki_node((*itx), (*ity));
      current_node->children.push_back(new_node);
      current_node = new_node;
    }
  }
}

static int convertMove(char move) {

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


static char moveToCharSGF(int move) 
{
  
  if (move == 0)
    return 'a';
  if (move == 1)
    return 'b';
  if (move == 2)
    return 'c';
  if (move == 3)
    return 'd';
  if (move == 4)
    return 'e';
  if (move == 5)
    return 'f';
  if (move == 6)
    return 'g';
  if (move == 7)
    return 'h';
  if (move == 8)
    return 'i';
  if (move == 9)
    return 'j';
  if (move == 10)
    return 'k';
  if (move == 11)
    return 'l';
  if (move == 12)
    return 'm';
  if (move == 13)
    return 'n';
  if (move == 14)
    return 'o';
  if (move == 15)
    return 'p';
  if (move == 16)
    return 'q';
  if (move == 17)
    return 'r';
  if (move == 18)
    return 's';
  
  // cout << move << endl;
  return '#';
}


// 0 si noir gagne
// 1 si blanc gagne
static int tokenizeSGF(string *gamestream,
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
	 continue;

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


// Le fichier renvoi false si le fichier n'est pas interessant
// car le joueur considéré n'a pas gagné la partie
static int parseSGF(string &filename,
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


static void storeSGF(string& filename) {

  //cerr << "storeSGF" << endl;
	
  static int nb = 0;
	     
  if (root_white == NULL)
    root_white = create_fuseki_node(-1, -1);

  if (root_black == NULL)
    root_black = create_fuseki_node(-1, -1);

  list<int> *move_x = new list<int> ();
  list<int> *move_y = new list<int> ();

  //cout << " parseSGF ..." << endl;
  int win_color;
  if ((win_color = parseSGF(filename, move_x, move_y)) != -1){
    nb++;
    cerr << filename << " - " << nb << endl;
    
    // int win_color = parseSGF(filename, move_x, move_y);
    if (win_color == 0 || win_color == 1)
      buildFusekiTree(move_x, move_y, win_color);
  }
  
  delete move_x;
  delete move_y;
}

/*static void libfuseki_evaluatePath(list<int> *move_x, 
				   list<int> *move_y, 
				   list<int> *p_move_x, 
				   list<int> *p_move_y, 
				   list<int> *count) {*/
static list<int> * ttmx;
static list<int> * ttmy;
static list<int> * ttpmx;
static list<int> * ttpmy;
static list<int> * ttc;
static int ttnb;
static int ttnw;

static void libfuseki_subEvaluatePath(list<int> *move_x, 
				   list<int> *move_y, 
				   list<int> *p_move_x, 
				   list<int> *p_move_y, 
				   list<int> *count) {

  // Parcourir l'arbre
  libfuseki_node *current_node;
  //std::cerr << "player_color: " << player_color << std::endl;

  if (player_color == 0)
    current_node = root_black;
  else if (player_color == 1)
    current_node = root_white;

  // DEBUG
  // libfuseki_print();
  // DEBUG

  // gestion du premier coup
  if (move_x->empty())
    return generate_first_move(current_node, p_move_x, p_move_y, count);
  
  bool found;

  //std::cerr << "Explore fuseki tree ..." << std::endl;

  list<int>::iterator itx, ity;
  for (itx = move_x->begin(), ity =  move_y->begin(); 
       itx != move_x->end(); itx++, ity++){

    //std::cerr << "current pos: " << (*itx) << " - " << (*ity) << endl;

    // recherche si le coup courant est deja
    // dans l'arbre
    found = false;
    list<struct libfuseki_node*>::iterator it_node;

    for (it_node = current_node->children.begin();
	 it_node != current_node->children.end() && found == false; 
	 it_node++)

      if ((*it_node)->move[0] == (*itx) && (*it_node)->move[1] == (*ity)){

	//std::cerr << "testx : " << (int)(*it_node)->move[0] 
	//	  << "testy : " << (int)(*it_node)->move[1]
	//	  << std::endl;

	found = true;
	current_node = (*it_node);	
      }
    
    if (found == false)
      break ;
  }

  // L'arbre n'a pas de coup possible
  if (itx != move_x->end())
    return ;

  else if (found == true) {

 //    std::cerr << "last_current_node: " 
// 	      << (int)current_node->move[0] 
// 	      << "-" << (int)current_node->move[1] << std::endl;

    list<struct libfuseki_node*>::iterator it_node;

    for (it_node =  current_node->children.begin();
	 it_node != current_node->children.end(); 
	 it_node++){

      // cout << "add p_move: " 
      //	   << (*it_node)->move[0] << " - " 
      //	   << (*it_node)->move[1] << endl;

      p_move_x->push_back((*it_node)->move[0]);
      p_move_y->push_back((*it_node)->move[1]);
      count->push_back((*it_node)->nb_pass);
    }
  }
}


// void Rotate(char String[], int Size)
// 
// Shifts all elements in 'String' one position to the left.

void Rotate(int String[], int Count)
{
    int Tmp = String[0];

    for (int i = 0; i < Count - 1; i++)
	String[i] = String[i + 1];
    String[Count - 1] = Tmp;
}

// void Print(char String[], int Size)
// 
// Prints all elements in 'String'

void Print(int String[], int Size)
{
    cout << "size(" << Size << ")    " << String[0];
    for (int i = 1; i < Size; i++)
	cout << ',' << String[i];
}

void PermuteB(int List[], int Listw[],const int Size, int n = 0)
{
    if (Size == n)
    {
	/*cout << "#"; Print(List, Size);
	cout << " + ";Print(Listw, ttnw);
        cout << endl;*/
	list<int> ttmx2;
	list<int> ttmy2;
	int bm;
	int wm;
	bm=0;
	wm=0;
	for (int i=0;i<(*ttmx).size();i++)
	{
		int color=i%2;
		if (color==0)
		{
			int index = List[bm]*2;
			list<int>::iterator itx = (*ttmx).begin();
			list<int>::iterator ity = (*ttmy).begin();
			for (int k=0;k<index;k++) {itx++;ity++;}
			
			ttmx2.push_back(*itx);
			ttmy2.push_back(*ity);
		}
		else
		{
			int index = Listw[wm]*2+1;
			list<int>::iterator itx = (*ttmx).begin();
			list<int>::iterator ity = (*ttmy).begin();
			for (int k=0;k<index;k++) {itx++;ity++;}
			
			ttmx2.push_back(*itx);
			ttmy2.push_back(*ity);
		}
		if (color==0) bm++; else wm++;
	}
        libfuseki_subEvaluatePath(&ttmx2,&ttmy2,ttpmx,ttpmy,ttc);
    }
    else {
	for (int i = Size - n; i > 0 ; i--)
	{
	    PermuteB(List,Listw, Size, n + 1);
	    Rotate(List + n, Size - n);
	}
    }
}

void PermuteW(int List[], const int Size, int n = 0)
{
    if (Size == n)
    {
	//Print(List, Size);
        int Listb[ttnb];
 	for (int i=0;i<ttnb;i++) Listb[i]=i;
	PermuteB(Listb,List,ttnb);
    }
    else {
	for (int i = Size - n; i > 0 ; i--)
	{
	    PermuteW(List, Size, n + 1);
	    Rotate(List + n, Size - n);
	}
    }
}

/*main()
{
	int List[10];
	for (int i=0;i<10;i++) List[i]=i;
	Permute(List,10);
}*/
static void libfuseki_evaluatePath(list<int> *move_x, 
				   list<int> *move_y, 
				   list<int> *p_move_x, 
				   list<int> *p_move_y, 
				   list<int> *count) {
	int n=(*move_x).size();
	if (n<3)
		libfuseki_subEvaluatePath(move_x,move_y,p_move_x,p_move_y,count);
	else
	{
 ttmx=move_x;
 ttmy=move_y;
			list<int>::iterator itx = (*ttmx).begin();
			list<int>::iterator ity = (*ttmy).begin();
			for (int k=0;k<(*ttmx).size();k++)
			{
				cerr << "(" << (*itx) << "/" << *ity << ")" << endl;
				itx++;
				ity++;
			}
 ttpmx=p_move_x;
 ttpmy=p_move_y;
 ttc=count;
 ttnb=(n+1)/2;
 ttnw=n-ttnb;
 ttnb=ttnb;
 ttnw=ttnw;
///printf("========== %d   %d =============\n",ttnb,ttnw,n);
 int Listw[ttnw];
 for (int i=0;i<ttnw;i++) Listw[i]=i;
 PermuteW(Listw,ttnw);
	}
}

void libfuseki_addDir(const char *dir){
  
   cerr << "addDirFuseki" << endl;


   
   
  if (dir != NULL){
    struct dirent *lecture;
    DIR *rep;
    
    rep = opendir(dir);


    if (rep == NULL){
      cerr << "libfuseki_addDir: unable to open " << string(dir)  << endl;

      switch(errno) {

      case EACCES:
	cerr << "Acces interdit" << endl;
	break ;

      case EMFILE:
	cerr << "Trop de descripteurs de fichiers pour le processus en cours" << endl;
	break ;
      case ENFILE:
	cerr << "Trop de fichiers ouverts simultanÃment sur le systeme" << endl;
	break ;
      case ENOENT:
	cerr << "Le repertoire n'existe pas, ou name est une chaine vide" << endl;
	break ;
      case ENOMEM:
	cerr << "Pas assez de memoire." << endl;
	break ;
      case ENOTDIR:
	cerr << "name n'est pas un repertoire" << endl;
	break ;
      }
      
      return;
    }
   
    while ((lecture = readdir(rep)) && (lecture != NULL)) {
      
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

static void parseListDir(string *lst_dir, list<string> *tokens) {
  
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

void libfuseki_addListDir(string *lst_dir){

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
    libfuseki_addDir((*it).c_str());   
}

static void generate_first_move(libfuseki_node *root_node,
				list<int> *p_move_x,
				list<int> *p_move_y,
				list<int> *count) {
    

  cerr << "generate first move" << endl;  
  list<struct libfuseki_node*>::iterator it_node;

  for (it_node =  root_node->children.begin();
       it_node != root_node->children.end(); 
       it_node++){
    
       cerr << "first add p_move: " 
    	   << (int)((*it_node)->move[0]) << " - " 
    	   << (int)((*it_node)->move[1]) << " - "
	    << (int)((*it_node)->nb_pass) << endl;
       
    p_move_x->push_back((*it_node)->move[0]);
    p_move_y->push_back((*it_node)->move[1]);
    count->push_back((*it_node)->nb_pass);
  }  
}


// Black = 0 and White != 0
void libfuseki_setPlayer(int _player){
  player_color = _player;
}

int libfuseki_getPlayer(){
  return player_color;
}

static void libfuseki_addPossibleMove(list<int> *p_move_x,
				      list<int> *p_move_y,
				      list<int> *count,
				      list<int> *total_p_move_x,
				      list<int> *total_p_move_y,
				      list<int> *total_count) {

  bool found ;
  list<int>::iterator it_p_move_x, it_p_move_y, it_count;
  list<int>::iterator it_total_p_move_x, it_total_p_move_y, it_total_count;

  for (it_p_move_x = p_move_x->begin(), it_p_move_y = p_move_y->begin(), it_count = count->begin();
       it_p_move_x != p_move_x->end(); 
       it_p_move_x++, it_p_move_y++, it_count++) {

    found = false;

     for (it_total_p_move_x = total_p_move_x->begin(), 
	    it_total_p_move_y = total_p_move_y->begin(), 
	    it_total_count = total_count->begin();
       it_total_p_move_x != total_p_move_x->end(); 
	  it_total_p_move_x++, it_total_p_move_y++, it_total_count++) {

       if ((*it_p_move_x) == (*it_total_p_move_x) &&
	   (*it_p_move_y) == (*it_total_p_move_y)) {

	 //cerr << "increment move ..." << endl;
	 
	 (*it_total_count) += (*it_count);
	 found = true;
       }
     }

     if (found == false) {
       total_p_move_x->push_back((*it_p_move_x));
       total_p_move_y->push_back((*it_p_move_y));
       //cerr << "add move ... " << endl;
       total_count->push_back((*it_count));
     }
  }
}

/*
void permut(list<int> *li,int a, int b)
{
	int frg = (*li)[a];
	(*li)[a]=(*li)[b];
	(*li)[b]=frg;
}*/

void libfuseki_getMove(list<int> *move_x, 
		       list<int> *move_y, 
		       list<int> *p_move_x, 
		       list<int> *p_move_y, 
		       list<int> *count,
		       int size) {

  // version sans les symetries
 //  libfuseki_evaluatePath(move_x, 
// 			 move_y, 
// 			 p_move_x, 
// 			 p_move_y, 
// 			 count);

  if (player_color == 0 && root_black == NULL)
    return ;
  if (player_color == 1 && root_white == NULL)
    return ;

  // Test les 8 symetries
  list<int> tmp_p_move_x, tmp_p_move_y, tmp_count;

  list<int> tmp_move_x;
  list<int> tmp_move_y;
  list<int> final_tmp_move_x;
  list<int> final_tmp_move_y;

  list<int>::iterator itx, ity, it_count;
  int x, y;  

  // 1
  std::cerr << "1" << std::endl;
  libfuseki_evaluatePath(move_x, 
			 move_y, 
			 &tmp_p_move_x, 
			 &tmp_p_move_y, 
			 &tmp_count);

  // DEBUG
  for (itx =  tmp_p_move_x.begin(), ity =  tmp_p_move_y.begin(), it_count = tmp_count.begin(); 
	 itx != tmp_p_move_x.end(); itx++, ity++, it_count++)
    cerr << "node : " 
	 << (int)(*itx) 
	 << " - " << (int)(*ity)
	 << " nb : " << (*it_count)
	 << endl;
  // DEBUG


  if (!tmp_p_move_x.empty())
     libfuseki_addPossibleMove(&tmp_p_move_x, 
			       &tmp_p_move_y, 
			       &tmp_count,
			       p_move_x, 
			       p_move_y, 
			       count);
  
  tmp_p_move_x.clear(); 
  tmp_p_move_y.clear();
  tmp_count.clear();


  // 2
  std::cerr << "2" << std::endl;
  for (itx =  move_x->begin(), ity =  move_y->begin(); 
       itx != move_x->end(); itx++, ity++) {
    libfuseki_rotate_90((*itx), (*ity), size, &x, &y);
    tmp_move_x.push_back(x);
    tmp_move_y.push_back(y);
  }

  libfuseki_evaluatePath(&tmp_move_x, 
			 &tmp_move_y, 
			 &tmp_p_move_x, 
			 &tmp_p_move_y, 
			 &tmp_count);

  // DEBUG
  for (itx =  tmp_p_move_x.begin(), ity =  tmp_p_move_y.begin(), it_count = tmp_count.begin(); 
	 itx != tmp_p_move_x.end(); itx++, ity++, it_count++)
    cerr << "node : " 
	 << (int)(*itx) 
	 << " - " << (int)(*ity)
	 << " nb : " << (*it_count)
	 << endl;
  // DEBUG


  if (!tmp_p_move_x.empty()){
    // untransform
    for (itx =  tmp_p_move_x.begin(), ity =  tmp_p_move_y.begin(); 
	 itx != tmp_p_move_x.end(); itx++, ity++) {

      libfuseki_un_rotate_90((*itx), (*ity), size, &x, &y);
      final_tmp_move_x.push_back(x);
      final_tmp_move_y.push_back(y);
    }

    libfuseki_addPossibleMove(&final_tmp_move_x, 
			      &final_tmp_move_y, 
			      &tmp_count,
			      p_move_x, 
			      p_move_y, 
			      count);
  }
  
  tmp_p_move_x.clear(); 
  tmp_p_move_y.clear();
  final_tmp_move_x.clear(); 
  final_tmp_move_y.clear();
  tmp_count.clear();

  tmp_move_x.clear();
  tmp_move_y.clear();
  
   // 3
  std::cerr << "3" << std::endl;
  for (itx =  move_x->begin(), ity =  move_y->begin(); 
       itx != move_x->end(); itx++, ity++) {
    libfuseki_rotate_90((*itx), (*ity), size, &x, &y);
    libfuseki_rotate_90(x, y, size, &x, &y);
    tmp_move_x.push_back(x);
    tmp_move_y.push_back(y);
  }
  
  libfuseki_evaluatePath(&tmp_move_x, 
			 &tmp_move_y, 
			 &tmp_p_move_x, 
			 &tmp_p_move_y, 
			 &tmp_count);
  
  // DEBUG
  for (itx =  tmp_p_move_x.begin(), ity =  tmp_p_move_y.begin(), it_count = tmp_count.begin(); 
	 itx != tmp_p_move_x.end(); itx++, ity++, it_count++)
    cerr << "node : " 
	 << (int)(*itx) 
	 << " - " << (int)(*ity)
	 << " nb : " << (*it_count)
	 << endl;
  // DEBUG

  if (!tmp_p_move_x.empty()){
    // untransform
    for (itx =  tmp_p_move_x.begin(), ity =  tmp_p_move_y.begin(); 
	 itx != tmp_p_move_x.end(); itx++, ity++) {

      libfuseki_un_rotate_90((*itx), (*ity), size, &x, &y);
      libfuseki_un_rotate_90(x, y, size, &x, &y);
      final_tmp_move_x.push_back(x);
      final_tmp_move_y.push_back(y);
    }

    libfuseki_addPossibleMove(&final_tmp_move_x, 
			      &final_tmp_move_y, 
			      &tmp_count,
			      p_move_x, 
			      p_move_y, 
			      count);
  }
  
  tmp_p_move_x.clear();
  tmp_p_move_y.clear();
  final_tmp_move_x.clear(); 
  final_tmp_move_y.clear();
  tmp_count.clear();

  tmp_move_x.clear();
  tmp_move_y.clear();
  
 

  // 4
  std::cerr << "4" << std::endl;
  for (itx =  move_x->begin(), ity =  move_y->begin(); 
       itx != move_x->end(); itx++, ity++) {
    libfuseki_rotate_90((*itx), (*ity), size, &x, &y);
    libfuseki_rotate_90(x, y, size, &x, &y);
    libfuseki_rotate_90(x, y, size, &x, &y);
    tmp_move_x.push_back(x);
    tmp_move_y.push_back(y);
  }
  
  libfuseki_evaluatePath(&tmp_move_x, 
			 &tmp_move_y, 
			 &tmp_p_move_x, 
			 &tmp_p_move_y, 
			 &tmp_count);
  
  // DEBUG
  for (itx =  tmp_p_move_x.begin(), ity =  tmp_p_move_y.begin(), it_count = tmp_count.begin(); 
	 itx != tmp_p_move_x.end(); itx++, ity++, it_count++)
    cerr << "node : " 
	 << (int)(*itx) 
	 << " - " << (int)(*ity)
	 << " nb : " << (*it_count)
	 << endl;
  // DEBUG

 if (!tmp_p_move_x.empty()){
    // untransform
    for (itx =  tmp_p_move_x.begin(), ity =  tmp_p_move_y.begin(); 
	 itx != tmp_p_move_x.end(); itx++, ity++) {

      libfuseki_un_rotate_90((*itx), (*ity), size, &x, &y);
      libfuseki_un_rotate_90(x, y, size, &x, &y);
      libfuseki_un_rotate_90(x, y, size, &x, &y);
      final_tmp_move_x.push_back(x);
      final_tmp_move_y.push_back(y);
    }

    libfuseki_addPossibleMove(&final_tmp_move_x, 
			      &final_tmp_move_y, 
			      &tmp_count,
			      p_move_x, 
			      p_move_y,
			      count);
  }
  
  tmp_p_move_x.clear();
  tmp_p_move_y.clear();
  final_tmp_move_x.clear(); 
  final_tmp_move_y.clear();
  tmp_count.clear();

  tmp_move_x.clear();
  tmp_move_y.clear();



  // 5
  std::cerr << "5" << std::endl;
  for (itx =  move_x->begin(), ity =  move_y->begin(); 
       itx != move_x->end(); itx++, ity++) {
    libfuseki_symetrie((*itx), (*ity), size, &x, &y);
    tmp_move_x.push_back(x);
    tmp_move_y.push_back(y);
  }

  libfuseki_evaluatePath(&tmp_move_x, 
			 &tmp_move_y, 
			 &tmp_p_move_x, 
			 &tmp_p_move_y, 
			 &tmp_count);
  

  // DEBUG
  for (itx =  tmp_p_move_x.begin(), ity =  tmp_p_move_y.begin(), it_count = tmp_count.begin(); 
	 itx != tmp_p_move_x.end(); itx++, ity++, it_count++)
    cerr << "node : " 
	 << (int)(*itx) 
	 << " - " << (int)(*ity)
	 << " nb : " << (*it_count)
	 << endl;
  // DEBUG

  if (!tmp_p_move_x.empty()){
    // untransform
    for (itx =  tmp_p_move_x.begin(), ity =  tmp_p_move_y.begin(); 
	 itx != tmp_p_move_x.end(); itx++, ity++) {

      libfuseki_un_symetrie((*itx), (*ity), size, &x, &y);
      final_tmp_move_x.push_back(x);
      final_tmp_move_y.push_back(y);
    }

    libfuseki_addPossibleMove(&final_tmp_move_x, 
			      &final_tmp_move_y, 
			      &tmp_count,
			      p_move_x, 
			      p_move_y, 
			      count);
  }
  
  tmp_p_move_x.clear();
  tmp_p_move_y.clear();
  final_tmp_move_x.clear(); 
  final_tmp_move_y.clear();
  tmp_count.clear();

  tmp_move_x.clear();
  tmp_move_y.clear();

  // 6
  std::cerr << "6" << std::endl;
  for (itx =  move_x->begin(), ity =  move_y->begin(); 
       itx != move_x->end(); itx++, ity++) {
    libfuseki_symetrie((*itx), (*ity), size, &x, &y);
    libfuseki_rotate_90(x, y, size, &x, &y);
    tmp_move_x.push_back(x);
    tmp_move_y.push_back(y);
  }

  libfuseki_evaluatePath(&tmp_move_x, 
			 &tmp_move_y, 
			 &tmp_p_move_x, 
			 &tmp_p_move_y, 
			 &tmp_count);
  
  // DEBUG
  for (itx =  tmp_p_move_x.begin(), ity =  tmp_p_move_y.begin(), it_count = tmp_count.begin(); 
	 itx != tmp_p_move_x.end(); itx++, ity++, it_count++)
    cerr << "node : " 
	 << (int)(*itx) 
	 << " - " << (int)(*ity)
	 << " nb : " << (*it_count)
	 << endl;
  // DEBUG

if (!tmp_p_move_x.empty()){
    // untransform
    for (itx =  tmp_p_move_x.begin(), ity =  tmp_p_move_y.begin(); 
	 itx != tmp_p_move_x.end(); itx++, ity++) {

      libfuseki_un_rotate_90((*itx), (*ity), size, &x, &y);
      libfuseki_un_symetrie(x, y, size, &x, &y);      
      final_tmp_move_x.push_back(x);
      final_tmp_move_y.push_back(y);
    }

    libfuseki_addPossibleMove(&final_tmp_move_x, 
			      &final_tmp_move_y, 
			      &tmp_count,
			      p_move_x, 
			      p_move_y, 
			      count);
  }
  
  tmp_p_move_x.clear();
  tmp_p_move_y.clear();
  final_tmp_move_x.clear(); 
  final_tmp_move_y.clear();
  tmp_count.clear();

  tmp_move_x.clear();
  tmp_move_y.clear();
  

  // 7
  std::cerr << "7" << std::endl;
  for (itx =  move_x->begin(), ity =  move_y->begin(); 
       itx != move_x->end(); itx++, ity++) {
    libfuseki_symetrie((*itx), (*ity), size, &x, &y);
    libfuseki_rotate_90(x, y, size, &x, &y);
    libfuseki_rotate_90(x, y, size, &x, &y);
    tmp_move_x.push_back(x);
    tmp_move_y.push_back(y);
  }

   libfuseki_evaluatePath(&tmp_move_x, 
			 &tmp_move_y, 
			 &tmp_p_move_x, 
			 &tmp_p_move_y, 
			 &tmp_count);
  
   // DEBUG
  for (itx =  tmp_p_move_x.begin(), ity =  tmp_p_move_y.begin(), it_count = tmp_count.begin(); 
	 itx != tmp_p_move_x.end(); itx++, ity++, it_count++)
    cerr << "node : " 
	 << (int)(*itx) 
	 << " - " << (int)(*ity)
	 << " nb : " << (*it_count)
	 << endl;
  // DEBUG

  if (!tmp_p_move_x.empty()){
    // untransform
    for (itx =  tmp_p_move_x.begin(), ity =  tmp_p_move_y.begin(); 
	 itx != tmp_p_move_x.end(); itx++, ity++) {

      libfuseki_un_rotate_90((*itx), (*ity), size, &x, &y);
      libfuseki_un_rotate_90(x, y, size, &x, &y);
      libfuseki_un_symetrie(x, y, size, &x, &y);      
      final_tmp_move_x.push_back(x);
      final_tmp_move_y.push_back(y);
    }

    libfuseki_addPossibleMove(&final_tmp_move_x, 
			      &final_tmp_move_y, 
			      &tmp_count,
			      p_move_x, 
			      p_move_y, 
			      count);
  }
  
  tmp_p_move_x.clear();
  tmp_p_move_y.clear();
  final_tmp_move_x.clear(); 
  final_tmp_move_y.clear();
  tmp_count.clear();

  tmp_move_x.clear();
  tmp_move_y.clear();
  

  // 8
  std::cerr << "8" << std::endl;
  for (itx =  move_x->begin(), ity =  move_y->begin(); 
       itx != move_x->end(); itx++, ity++) {
    libfuseki_symetrie((*itx), (*ity), size, &x, &y);
    libfuseki_rotate_90(x, y, size, &x, &y);
    libfuseki_rotate_90(x, y, size, &x, &y);
    libfuseki_rotate_90(x, y, size, &x, &y);
    tmp_move_x.push_back(x);
    tmp_move_y.push_back(y);
  }

   libfuseki_evaluatePath(&tmp_move_x, 
			 &tmp_move_y, 
			 &tmp_p_move_x, 
			 &tmp_p_move_y, 
			 &tmp_count);
  
   // DEBUG
  for (itx =  tmp_p_move_x.begin(), ity =  tmp_p_move_y.begin(), it_count = tmp_count.begin(); 
	 itx != tmp_p_move_x.end(); itx++, ity++, it_count++)
    cerr << "node : " 
	 << (int)(*itx) 
	 << " - " << (int)(*ity)
	 << " nb : " << (*it_count)
	 << endl;
  // DEBUG

 if (!tmp_p_move_x.empty()){
    // untransform
    for (itx =  tmp_p_move_x.begin(), ity =  tmp_p_move_y.begin(); 
	 itx != tmp_p_move_x.end(); itx++, ity++) {

      libfuseki_un_rotate_90((*itx), (*ity), size, &x, &y);
      libfuseki_un_rotate_90(x,y, size, &x, &y);
      libfuseki_un_rotate_90(x,y, size, &x, &y);
      libfuseki_un_symetrie(x, y, size, &x, &y);      
      final_tmp_move_x.push_back(x);
      final_tmp_move_y.push_back(y);
    }

    libfuseki_addPossibleMove(&final_tmp_move_x, 
			      &final_tmp_move_y, 
			      &tmp_count,
			      p_move_x, 
			      p_move_y, 
			      count);
  }
  
  tmp_p_move_x.clear();
  tmp_p_move_y.clear();
  final_tmp_move_x.clear(); 
  final_tmp_move_y.clear();
  tmp_count.clear();

  tmp_move_x.clear();
  tmp_move_y.clear();

}

void libfuseki_print(){

  if (root_white == NULL || root_black == NULL)
    return ;
  
   
    
  cerr << "libfuseki_print white" << endl;
  list<struct libfuseki_node*>::iterator it_node;
  
  for (it_node =  root_white->children.begin();
       it_node != root_white->children.end();
       it_node++) {

    cerr << "node : " 
	 << (int)((*it_node)->move[0]) 
	 << " - " << (int)((*it_node)->move[1])
	 << " nb : " << (*it_node)->nb_pass
	 << endl;
  }

  cerr << "libfuseki_print black" << endl;  
  for (it_node =  root_black->children.begin();
       it_node != root_black->children.end();
       it_node++) {

    cerr << "node : " 
	 << (int)((*it_node)->move[0]) 
	 << " - " << (int)((*it_node)->move[1])
	 << " nb : " << (*it_node)->nb_pass
	 << endl;
  }
}

static void libfuseki_rotate_90(int x, int y, 
				int size, int *r_x, int* r_y) {
  *r_x = y;
  *r_y = size - x - 1;
}

static void libfuseki_symetrie(int x, int y, 
			       int size, int *r_x, int* r_y) {
  *r_x = size - x - 1;
  *r_y = y;
}

static void libfuseki_un_rotate_90(int x, int y, 
				int size, int *r_x, int* r_y) {
  *r_x = size - y - 1;
  *r_y = x;
}

static void libfuseki_un_symetrie(int x, int y, 
				  int size, int *r_x, int* r_y) {
  *r_x = size - x - 1;
  *r_y = y;

}






//a negative value for color shows the size of the goban
//0 for Black
//1 for White
void libfuseki_saveTreeSGF(const libfuseki_node* tree_fuseki, ofstream& flux, int color)
{
  assert(tree_fuseki);
  
  if(color<0)
    {
      flux<<"(;FF[4]";
      flux<<"SZ["<<-color<<"]";
    }
  else
    {
      //Color
      if(color==0) //Here, we assume Black
	flux<<";B";
      else flux<<";W";
      
      //cerr<< moveToCharSGF(tree_fuseki->move[0])<<moveToCharSGF(tree_fuseki->move[1])<<endl;
      
      
      //Move
      flux<<"["<<moveToCharSGF(tree_fuseki->move[0])<<moveToCharSGF(tree_fuseki->move[1])<<"]";
      
      //Comments
      flux<<"C[nb openings : "<<tree_fuseki->nb_pass;
      flux<<"\nnb played : "<<0;
      flux<<"\nnb wins : "<<0<<"]";
    }
  
  
  list<libfuseki_node*>::const_iterator i_ln = (tree_fuseki->children).begin();

  //The best move is marked by a round
  if((tree_fuseki->children).size()>0) 
    {
      flux<<"CR[";
      
      int max_pass = -1;
      int best_abs = 0;
      int best_ord = 0;
      while(i_ln!=(tree_fuseki->children).end())
	{
	  if((*i_ln)->nb_pass>max_pass)
	    {
	      max_pass = (*i_ln)->nb_pass;
	      best_abs = (*i_ln)->move[0];
	      best_ord = (*i_ln)->move[1];
	    }
	  
	  i_ln++;
	}
      
      flux<<moveToCharSGF(best_abs)<<moveToCharSGF(best_ord)<<"]"; 
    }
  
  //Next moves in the tree marked by a cross
  if((tree_fuseki->children).size()>0) flux<<"MA";
  i_ln = (tree_fuseki->children).begin();
  while(i_ln!=(tree_fuseki->children).end())
    {
      flux<<"["<<moveToCharSGF((*i_ln)->move[0])<<moveToCharSGF((*i_ln)->move[1])<<"]";
      i_ln++;
    }
  
  
  //Building next moves for the sgf
  i_ln = (tree_fuseki->children).begin();
  while(i_ln!=(tree_fuseki->children).end())
    {
      if((tree_fuseki->children).size()>1) flux<<"(";
      
      if(color==0)
	libfuseki_saveTreeSGF(*i_ln,flux,1); //1: We assume White
      else libfuseki_saveTreeSGF(*i_ln,flux,0);
      
      i_ln++;
      
      if((tree_fuseki->children).size()>1) flux<<")";
    }
  
  //End of the sequence
  if(color<0)
    flux<<")";
}

void libfuseki_saveTreesSGF(const string& black_tree_sgf, const string& white_tree_sgf, int size)
{
  
  //Black tree
  if(root_black!=NULL) 
    {
      ofstream fb(black_tree_sgf.c_str());
      if(not fb.is_open())
	{
	  assert(0);
	}
      
      
      libfuseki_saveTreeSGF(root_black,fb,-size);
      fb.close();
    }
  
  //White tree
  if(root_white!=NULL) 
    {
       ofstream fw(white_tree_sgf.c_str());
       if(not fw.is_open())
	 {
	   assert(0);
	 }
       
       libfuseki_saveTreeSGF(root_white,fw,-size);
       fw.close();
    }
}


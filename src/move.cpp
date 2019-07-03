#include "move.h"

pair<int, int> int_to_pair(const int height, const int mv)
{
  if(mv==GE_PASS_MOVE) return make_pair(GE_PASS_MOVE, GE_PASS_MOVE);
  if(mv==GE_RESIGN) return make_pair(GE_RESIGN, GE_RESIGN);


  return make_pair(mv/height, mv%height);
}


int pair_to_int(const int height, const pair<int, int>& mv)
{
  if(mv.first==GE_PASS_MOVE) return GE_PASS_MOVE;
  if(mv.first==GE_RESIGN) return GE_RESIGN;
  
  return mv.first*height + mv.second;
}


int char_to_move_for_go(const char c)
{

  if((c=='A')||(c=='a'))
    return 0;
  if((c=='B')||(c=='b'))
    return 1;
  if((c=='C')||(c=='c'))
    return 2;
  if((c=='D')||(c=='d'))
    return 3;
  if((c=='E')||(c=='e'))
    return 4;
  if((c=='F')||(c=='f'))
    return 5;
  if((c=='G')||(c=='g'))
    return 6;
  if((c=='H')||(c=='h'))
    return 7;
  if((c=='J')||(c=='j'))
    return 8;
  if((c=='K')||(c=='k'))
    return 9;
  if((c=='L')||(c=='l'))
    return 10;
  if((c=='M')||(c=='m'))
    return 11;
  if((c=='N')||(c=='n'))
    return 12;
  if((c=='O')||(c=='o'))
    return 13;
  if((c=='P')||(c=='p'))
    return 14;
  if((c=='Q')||(c=='q'))
    return 15;
  if((c=='R')||(c=='r'))
    return 16;
  if((c=='S')||(c=='s'))
    return 17;
  if((c=='T')||(c=='t'))
    return 18;


  if(c=='t')
    return GE_PASS_MOVE;

  return GE_ILLEGAL_MOVE;
}


int char_to_move_for_go_sgf(const char c)
{
  
  if((c=='A')||(c=='a'))
    return 0;
  if((c=='B')||(c=='b'))
    return 1;
  if((c=='C')||(c=='c'))
    return 2;
  if((c=='D')||(c=='d'))
    return 3;
  if((c=='E')||(c=='e'))
    return 4;
  if((c=='F')||(c=='f'))
    return 5;
  if((c=='G')||(c=='g'))
    return 6;
  if((c=='H')||(c=='h'))
    return 7;
  if((c=='J')||(c=='i'))
    return 8;
  if((c=='K')||(c=='j'))
    return 9;
  if((c=='L')||(c=='k'))
    return 10;
  if((c=='M')||(c=='l'))
    return 11;
  if((c=='N')||(c=='m'))
    return 12;
  if((c=='O')||(c=='n'))
    return 13;
  if((c=='P')||(c=='o'))
    return 14;
  if((c=='Q')||(c=='p'))
    return 15;
  if((c=='R')||(c=='q'))
    return 16;
  if((c=='S')||(c=='r'))
    return 17;
  if((c=='T')||(c=='s'))
    return 18;

  if(c=='t')
    return GE_PASS_MOVE;

  return GE_ILLEGAL_MOVE;
}







pair<int, int> string_to_move(const string& s_mv)
{
  pair<int, int> mv;
  
  if(s_mv=="PASS")
    return make_pair(GE_PASS_MOVE, GE_PASS_MOVE);

  if(s_mv=="GE_RESIGN")
    return make_pair(GE_RESIGN, GE_RESIGN);

  if((s_mv.size()<2)||(s_mv.size()>3))
    return make_pair(GE_ILLEGAL_MOVE, GE_ILLEGAL_MOVE);
  
  mv.first = char_to_move_for_go_sgf(s_mv[0]);
  mv.second = char_to_move_for_go_sgf(s_mv[1]);
  
  //a implementer (cas d'un chiffre)
  
  return mv;
}


const string move_to_string(const int height, const int c)
{
  pair<int, int> mv = int_to_pair(height, c);
  return move_to_string(mv);
}


const string move_to_string(const pair<int, int>& c)
{
  string res = "";
  
  if(c.first==GE_PASS_MOVE)
    return "PASS";
  
  if(c.first==GE_RESIGN)
    return "GE_RESIGN";


  /*
    0 1 2 3 4 5 6 7 8 9 
    A B C D E F G H J K
  */

  if(c.first<8)
    res+=(c.first+'A');
  else res+=(c.first+'A'+1);
  
  ostringstream os;
  os<<res<<(1+c.second);
  
  return os.str();
}


const string move_to_string_gtp(const int height, const int c)
{
  pair<int, int> mv = int_to_pair(height, c);
  return move_to_string_gtp(height, mv);
  //return move_to_string_gtp(mv);
}


const string move_to_string_gtp(const pair<int, int>& c)
{

  string res = "";
  
  if(c.first==GE_PASS_MOVE)
    return "pass";
  
  if(c.first==GE_RESIGN)
    return "resign";

  /*
    0 1 2 3 4 5 6 7 8 9 
    A B C D E F G H J K
  */
  
  if(c.first<8)
    res+=(c.first+'A');
  else res+=(c.first+'A'+1);
  
  ostringstream os;

  os<<res<<(1+c.second);
  //os<<res<<9-c.second; //a debugger (pb de symetrie)
  
  return os.str();
}



const string move_to_string_gtp(int height, const pair<int, int>& c)
{

  string res = "";
  
  if(c.first==GE_PASS_MOVE)
    return "pass";
  
  if(c.first==GE_RESIGN)
    return "resign";

  /*
    0 1 2 3 4 5 6 7 8 9 
    A B C D E F G H J K
  */
  
  if(c.first<8)
    res+=(c.first+'A');
  else res+=(c.first+'A'+1);
  
  ostringstream os;
  os<<res<<(1+c.second);
  //os<<res<<height-c.second;  //a debugger (pb de symetrie)

  return os.str();
}



pair<int, int> string_gtp_to_move(const string& s_mv)
{
  pair<int, int> mv;
  
  if((s_mv=="pass")||(s_mv=="PASS"))
    return make_pair(GE_PASS_MOVE, GE_PASS_MOVE);
  
  if((s_mv=="resign")||(s_mv=="GE_RESIGN"))
    return make_pair(GE_PASS_MOVE, GE_RESIGN); //TODO implement (GE_RESIGN, GE_RESIGN) ?
  
  if((s_mv.size()<2)||(s_mv.size()>3))
    return make_pair(GE_ILLEGAL_MOVE, GE_ILLEGAL_MOVE);
  

  istringstream is(s_mv);
  char col;

  is>>col>>mv.second;
  mv.first = char_to_move_for_go(col);
  mv.second--;

  return mv;
}

int string_gtp_to_move(const string& s_mv, int height)
{
  pair<int, int> mv2 = string_gtp_to_move(s_mv);
  if(mv2.first==GE_PASS_MOVE)
    return GE_PASS_MOVE;

  //the case of resign is to implement
  if(mv2.first==GE_RESIGN)
    return GE_PASS_MOVE;


  
  return pair_to_int(height, mv2);
}












const string move_to_string_gtp_sym(const int height, const int c)
{
  pair<int, int> mv = int_to_pair(height, c);
  return move_to_string_gtp_sym(height, mv);
}



const string move_to_string_gtp_sym(int height, const pair<int, int>& c)
{

  string res = "";
  
  if(c.first==GE_PASS_MOVE)
    return "pass";
  
  if(c.first==GE_RESIGN)
    return "resign";

  /*
    0 1 2 3 4 5 6 7 8 9 
    A B C D E F G H J K
  */
  
  if(c.first<8)
    res+=(c.first+'A');
  else res+=(c.first+'A'+1);
  
  ostringstream os;
  os<<res<<height-c.second;  //a debugger (pb de symetrie)

  return os.str();
}

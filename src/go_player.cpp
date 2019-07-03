#include "go_player.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


GE_GoPlayer::GE_GoPlayer() : referee(0) , tc(GE_BLACK) , avoid_atari(true) 
{
  name = "player";
  can_resign = false;
}

GE_GoPlayer::GE_GoPlayer(const GE_RefereeGo& rg)
{
  name = "player";
  
  referee = &rg;
  tc = GE_BLACK;
  avoid_atari = true;
  can_resign = false;
}

GE_GoPlayer::GE_GoPlayer(const GE_RefereeGo& rg, const team_color& col)
{
  name = "player";

  referee = &rg;
  tc = col;
  avoid_atari = true;
  can_resign = false;
}

int GE_GoPlayer::set_player(const team_color tc)
{
  int old_tc = this->tc;
  this->tc = tc; 
  return old_tc;
}

bool GE_GoPlayer::set_resign(const bool cr)
{
  int old_resign = this->can_resign;
  this->can_resign = cr; 
  return old_resign;
}

bool GE_GoPlayer::is_atari(const GE_Goban& gob, const int mv) const
{
  GE_Goban copy_goban(gob);

  GE_Chains ch1(copy_goban);
  int nb_ataris1 = ch1.get_nb_atari(tc);
  
  list<list<int> > stones_taken;
  GE_ManagerGame mg(copy_goban, tc);
  copy_goban.play(mv, tc);
  mg.get_stones_taken(stones_taken);
  copy_goban.update(stones_taken);
  
  GE_Chains ch2(copy_goban);
  int nb_ataris2 = ch2.get_nb_atari(tc);
  
  return (nb_ataris2 > nb_ataris1);
}

int GE_GoPlayer::get_moves(const GE_Goban& gob, list<int>& l_move) const
{
  l_move.push_back(GE_PASS_MOVE);
  return l_move.size();
}

int GE_GoPlayer::get_move(const GE_Goban& gob) const
{
  if(not referee)
    return GE_PASS_MOVE;

  for(int i = 0; i<gob.height; i++)
    for(int j = 0; j<gob.width; j++)
      {
	int mv = pair_to_int(gob.height, make_pair(i, j));
	
	if(referee->is_allowed(gob, mv, tc))
	  {
	    if(avoid_atari)
	      if(not is_atari(gob, mv))
		return mv;
	  }
      }
  
  return GE_PASS_MOVE;
}


bool GE_GoPlayer::is_human() const
{
  return false;
}

GE_HumanGoPlayer::GE_HumanGoPlayer() : GE_GoPlayer() 
{
  name = "human";
}

GE_HumanGoPlayer::GE_HumanGoPlayer(const GE_RefereeGo& rg) : GE_GoPlayer(rg) 
{
  name = "human";
}

GE_HumanGoPlayer::GE_HumanGoPlayer(const GE_RefereeGo& rg, const team_color& col) 
  : GE_GoPlayer(rg, col)
{
  name = "human";
}


int GE_HumanGoPlayer::get_move(const GE_Goban& gob) const
{
  while(true)
    {
      string s_mv = "";
      cout<<"enter your move"<<endl;
      cin>>s_mv;
      
      pair<int, int> mv2 = string_to_move(s_mv);
      int mv = pair_to_int(gob.height, mv2);
      
      if(not referee) 
	return mv;
      
      if(not referee->is_allowed(gob, mv, tc))
	{
	  cout<<"illegal move"<<endl;
	  continue;
	}

      return mv;
    }
  
  return GE_PASS_MOVE;
}

bool GE_HumanGoPlayer::is_human() const
{
  return true;
}


GE_Goldeneye::GE_Goldeneye() : GE_GoPlayer() 
{
  name = "goldeneye";
}

GE_Goldeneye::GE_Goldeneye(const GE_RefereeGo& rg) : GE_GoPlayer(rg) 
{
  name = "goldeneye";
}

GE_Goldeneye::GE_Goldeneye(const GE_RefereeGo& rg, const team_color& col) 
  : GE_GoPlayer(rg, col)
{
  name = "goldeneye";
}

int GE_Goldeneye::get_move(const GE_Goban& gob) const
{
  return GE_PASS_MOVE;
}


GE_RandomGoPlayer::GE_RandomGoPlayer() : GE_GoPlayer() 
{
  name = "random";
}

GE_RandomGoPlayer::GE_RandomGoPlayer(const GE_RefereeGo& rg) : GE_GoPlayer(rg) 
{
  name = "random";
}

GE_RandomGoPlayer::GE_RandomGoPlayer(const GE_RefereeGo& rg, const team_color& col) 
  : GE_GoPlayer(rg, col) 
{
  name = "random";
}

int GE_RandomGoPlayer::get_move(const GE_Goban& gob) const
{
  const int prod = gob.height*gob.width;
  const int modulo = prod + 1; //+ Pass move
  

  while(true)
    {
      int mv = rand()%modulo;
      if(mv==prod)
	return GE_PASS_MOVE;
      
      if(not referee)
	return GE_PASS_MOVE;
      
      if(referee->is_allowed(gob, mv, tc))
	{
	  if(avoid_atari)
	    if(not is_atari(gob, mv))
	      return mv;
	} 
    }
  
  return GE_PASS_MOVE;
}


GE_GoPlayerAnimal::GE_GoPlayerAnimal() 
  : GE_GoPlayer()
{
  name = "animal";
  game = 0;
}

GE_GoPlayerAnimal::GE_GoPlayerAnimal(GE_Game& g, const GE_RefereeGo& rg, 
			       const team_color& tc) 
  : GE_GoPlayer(rg, tc)
{
  name = "animal";
  game = &g;
}

int GE_GoPlayerAnimal::get_moves(const GE_Goban& gob, list<int>& lmoves) const
{ 
  return 0; 
}

int GE_GoPlayerAnimal::get_move(const GE_Goban& gob) const
{ 
  return GE_PASS_MOVE; 
}


GE_GoPlayerLion::GE_GoPlayerLion() : GE_GoPlayerAnimal()
{ 
  name = "lion";
}

GE_GoPlayerLion::GE_GoPlayerLion(GE_Game& g, const GE_RefereeGo& rg, 
			   const team_color& col) 
  : GE_GoPlayerAnimal(g, rg, col)
{
  name = "lion";
}


int GE_GoPlayerLion::get_moves(const GE_Goban& gob, list<int>& l_move) const
{
  if((not game)||(not referee))
    {
      l_move.push_back(GE_PASS_MOVE);
      return l_move.size();
    }
  
  GE_Strategy strat;
  strat.maximize_territory(*game, &l_move);
  if(l_move.empty())
    {
      l_move.push_back(GE_PASS_MOVE);
      return l_move.size();
    }
  
  return l_move.size();
}

int GE_GoPlayerLion::get_move(const GE_Goban& gob) const
{
  if(not game)
    return GE_PASS_MOVE;
  
  if(not referee)
    return GE_PASS_MOVE;
  
  GE_Strategy strat;
  list<int> l_move;
  
  strat.maximize_territory(*game, &l_move);
  if(l_move.empty())
    return GE_PASS_MOVE;

  int mv = GE_ListOperators::random(l_move);
  
  if(referee->is_allowed(gob, mv, tc))
    return mv;

  return GE_PASS_MOVE;
}




GE_GoPlayerMonkey::GE_GoPlayerMonkey() : GE_GoPlayerAnimal() 
{
  name = "monkey";
  dgg = 0;
}

GE_GoPlayerMonkey::GE_GoPlayerMonkey(GE_Game& g, GE_DbGamesGo& db, const GE_RefereeGo& rg, 
			       const team_color& col) 
  : GE_GoPlayerAnimal(g, rg, col)
{
  name = "monkey";
  dgg = &db;
}


//not debugging
int GE_GoPlayerMonkey::get_moves(const GE_Goban& gob, list<int>& l_move) const
{
  /*
    if((gob.height!=19)||(gob.width!=19)) 
    {
    l_move.push_back(GE_PASS_MOVE);
    return l_move.size();
    }
  */    

  if((not game)||(not referee)||(not dgg))
    {
      l_move.push_back(GE_PASS_MOVE);
      return l_move.size();
    }
  
  GE_Strategy strat;
  strat.maximize_resemblance(*game, *dgg, &l_move);
  if(l_move.empty())
    {
      l_move.push_back(GE_PASS_MOVE);
      return l_move.size();
    }
  
  return l_move.size();
}


//to debug (pass at the end?)
int GE_GoPlayerMonkey::get_move(const GE_Goban& gob) const
{
  //if((gob.height!=19)||(gob.width!=19)) return GE_PASS_MOVE;
  
  if(not game)
    return GE_PASS_MOVE;
  
  if(not referee)
    return GE_PASS_MOVE;
  
  if(not dgg)
    return GE_PASS_MOVE;

  GE_Strategy strat;
  list<int> l_move;
  
  strat.maximize_resemblance(*game, *dgg, &l_move);
  if(l_move.empty())
    return GE_PASS_MOVE;
  
  int mv = GE_ListOperators::random(l_move);
  
  if(referee->is_allowed(gob, mv, tc))
    return mv;
  
  return GE_PASS_MOVE;
}


GE_GoPlayerGorilla::GE_GoPlayerGorilla() : GE_GoPlayerMonkey() 
{
  name = "gorilla";
  filt = 0;
}

GE_GoPlayerGorilla::GE_GoPlayerGorilla(GE_Game& g, GE_DbGamesGo& db, 
				 const GE_RefereeGo& rg, 
				 const GE_Filter& f, 
				 const team_color& col) 
  : GE_GoPlayerMonkey(g, db, rg, col)
{
  name = "gorilla";
  dgg = &db; /// TO DEBUG --> pb with pointer on database (SF) 
  filt = &f;
}


//not debugging
int GE_GoPlayerGorilla::get_moves(const GE_Goban& gob, list<int>& l_move) const
{
  /*
    if((gob.height!=19)||(gob.width!=19)) 
    {
    l_move.push_back(GE_PASS_MOVE);
    return l_move.size();
    }
  */

  if((not game)||(not referee)||(not filt)||(not dgg))
    {
      l_move.push_back(GE_PASS_MOVE);
      return l_move.size();
    }
  
  GE_Strategy strat;
  strat.maximize_resemblance(*game, *dgg, *filt, &l_move);
  if(l_move.empty())
    {
      l_move.push_back(GE_PASS_MOVE);
      return l_move.size();
    }
  
  return l_move.size();
}


//to debug (pass at the end?)
int GE_GoPlayerGorilla::get_move(const GE_Goban& gob) const
{
  //if((gob.height!=19)||(gob.width!=19)) return GE_PASS_MOVE;
  
  if(not game)
    return GE_PASS_MOVE;
  
  if(not referee)
    return GE_PASS_MOVE;
  
  if(not dgg)
    return GE_PASS_MOVE;

  if(not filt)
    return GE_PASS_MOVE;

  GE_Strategy strat;
  list<int> l_move;
  
  strat.maximize_resemblance(*game, *dgg, *filt, &l_move);
  if(l_move.empty())
    return GE_PASS_MOVE;
  
  int mv = GE_ListOperators::random(l_move);
  
  if(referee->is_allowed(gob, mv, tc))
    return mv;
  
  return GE_PASS_MOVE;
}



GE_GoPlayerAnt::GE_GoPlayerAnt() : GE_GoPlayerAnimal() 
{
  name = "ant";
  dgg = 0;
}

GE_GoPlayerAnt::GE_GoPlayerAnt(GE_Game& g, GE_DbGamesGo& db, const GE_RefereeGo& rg, 
			 const team_color& col) 
  : GE_GoPlayerAnimal(g, rg, col)
{
  name = "ant";
  dgg = &db;
}


//not debugging
int GE_GoPlayerAnt::get_moves(const GE_Goban& gob, list<int>& l_move) const
{
  /*
  if((gob.height!=19)||(gob.width!=19)) 
    {
      l_move.push_back(GE_PASS_MOVE);
      return l_move.size();
    }
  */

  if((not game)||(not referee))
    {
      l_move.push_back(GE_PASS_MOVE);
      return l_move.size();
    }
 
  GE_ManagerMoves mm(gob.height, gob.width);
  mm.eval(*game, *dgg);
  
  int temp_max = 0;
  for(int i = 0; i<mm.height; i++)
    for(int j = 0; j<mm.width; j++)
      {
	if(mm.eval_moves[i][j]==temp_max)
	  l_move.push_back(pair_to_int(mm.height, make_pair(i, j)));
	
	if(mm.eval_moves[i][j]>temp_max)
	  {
	    temp_max = mm.eval_moves[i][j];
	    l_move.clear();
	    l_move.push_back(pair_to_int(mm.height, make_pair(i, j)));
	  }
      }
  
  if(l_move.empty())
    {
      l_move.push_back(GE_PASS_MOVE);
      return l_move.size();
    }
  
  return l_move.size();
}


//to debug (pass at the end?)
int GE_GoPlayerAnt::get_move(const GE_Goban& gob) const
{
  //if((gob.height!=19)||(gob.width!=19)) return GE_PASS_MOVE;
  
  if(not game)
    return GE_PASS_MOVE;
  
  if(not referee)
    return GE_PASS_MOVE;
 
  list<int> l_move;

  GE_ManagerMoves mm(gob.height, gob.width);
  mm.eval(*game, *dgg);
  
  //mm.print();

  int temp_max = 0;
  for(int i = 0; i<mm.height; i++)
    for(int j = 0; j<mm.width; j++)
      {
	if(mm.eval_moves[i][j]==temp_max)
	  l_move.push_back(pair_to_int(mm.height, make_pair(i, j)));
	
	if(mm.eval_moves[i][j]>temp_max)
	  {
	    temp_max = mm.eval_moves[i][j];
	    l_move.clear();
	    l_move.push_back(pair_to_int(mm.height, make_pair(i, j)));
	  }
      }
  
  if(l_move.empty())
    return GE_PASS_MOVE;
  
  int mv = GE_ListOperators::random(l_move);
  
  if(referee->is_allowed(gob, mv, tc))
    {
      if(avoid_atari)
	{
	  if(not is_atari(gob, mv))
	    return mv;
	}
      return mv;
    }
  
  return GE_PASS_MOVE;
}





GE_GoPlayerSnake::GE_GoPlayerSnake() : GE_GoPlayerAnimal()
{
  name = "snake";
}

GE_GoPlayerSnake::GE_GoPlayerSnake(GE_Game& g, const GE_RefereeGo& rg, 
			     const team_color& col) 
  : GE_GoPlayerAnimal(g, rg, col)
{
  name = "snake";
}


int GE_GoPlayerSnake::get_moves(const GE_Goban& gob, list<int>& l_move) const
{
  if((not game)||(not referee))
    {
      l_move.push_back(GE_PASS_MOVE);
      return l_move.size();
    }
  
  GE_Strategy strat;
  strat.maximize_suffocation(*game, &l_move);
  if(l_move.empty())
    {
      l_move.push_back(GE_PASS_MOVE);
      return l_move.size();
    }
  
  return l_move.size();
}

int GE_GoPlayerSnake::get_move(const GE_Goban& gob) const
{
  if(not game)
    return GE_PASS_MOVE;
  
  if(not referee)
    return GE_PASS_MOVE;
  
  GE_Strategy strat;
  list<int> l_move;
  
  strat.maximize_suffocation(*game, &l_move);
  if(l_move.empty())
    return GE_PASS_MOVE;

  int mv = GE_ListOperators::random(l_move);

  if(referee->is_allowed(gob, mv, tc))
    return mv;

  return GE_PASS_MOVE;
}




GE_GoPlayerBear::GE_GoPlayerBear() : GE_GoPlayerAnimal()
{
  name = "bear";
}

GE_GoPlayerBear::GE_GoPlayerBear(GE_Game& g, const GE_RefereeGo& rg, 
			   const team_color& col) 
  : GE_GoPlayerAnimal(g, rg, col)
{
  name = "bear"; 
}


int GE_GoPlayerBear::get_moves(const GE_Goban& gob, list<int>& l_move) const
{
  if((not game)||(not referee))
    {
      l_move.push_back(GE_PASS_MOVE);
      return l_move.size();
    }
  
  GE_Strategy strat;
  strat.maximize_greediness(*game, &l_move);
  if(l_move.empty())
    {
      l_move.push_back(GE_PASS_MOVE);
      return l_move.size();
    }
  
  return l_move.size();
}

int GE_GoPlayerBear::get_move(const GE_Goban& gob) const
{
  if(not game)
    return GE_PASS_MOVE;
  
  if(not referee)
    return GE_PASS_MOVE;
  
  GE_Strategy strat;
  list<int> l_move;
  
  strat.maximize_greediness(*game, &l_move);
  if(l_move.empty())
    return GE_PASS_MOVE;

  int mv = GE_ListOperators::random(l_move);

  if(referee->is_allowed(gob, mv, tc))
    return mv;

  return GE_PASS_MOVE;
}



GE_GoPlayerSnail::GE_GoPlayerSnail() 
  : GE_GoPlayerAnimal()
{
  name = "snail";
}

GE_GoPlayerSnail::GE_GoPlayerSnail(GE_Game& g, const GE_RefereeGo& rg, 
			   const team_color& col) 
  : GE_GoPlayerAnimal(g, rg, col)
{
  name = "snail";
}


int GE_GoPlayerSnail::get_moves(const GE_Goban& gob, list<int>& l_move) const
{
  if((not game)||(not referee))
    {
      l_move.push_back(GE_PASS_MOVE);
      return l_move.size();
    }
  
  GE_Strategy strat;
  strat.maximize_survivor(*game, &l_move);
  if(l_move.empty())
    {
      l_move.push_back(GE_PASS_MOVE);
      return l_move.size();
    }
  
  return l_move.size();
}

int GE_GoPlayerSnail::get_move(const GE_Goban& gob) const
{
  if(not game)
    return GE_PASS_MOVE;
  
  if(not referee)
    return GE_PASS_MOVE;
  
  GE_Strategy strat;
  list<int> l_move;
  
  strat.maximize_survivor(*game, &l_move);
  if(l_move.empty())
    return GE_PASS_MOVE;

  int mv = GE_ListOperators::random(l_move);
  
  if(referee->is_allowed(gob, mv, tc))
    return mv;

  return GE_PASS_MOVE;
}


GE_GoPlayerFlea::GE_GoPlayerFlea() : GE_GoPlayerAnimal()
{ 
  name = "flea";
}

GE_GoPlayerFlea::GE_GoPlayerFlea(GE_Game& g, const GE_RefereeGo& rg, 
			   const team_color& col) 
  : GE_GoPlayerAnimal(g, rg, col)
{
  name = "flea";
}


int GE_GoPlayerFlea::get_moves(const GE_Goban& gob, list<int>& l_move) const
{
  if((not game)||(not referee))
    {
      l_move.push_back(GE_PASS_MOVE);
      return l_move.size();
    }
  
  const int prod = gob.height*gob.width;
  const int modulo = prod + 1; //+ Pass move
  
  int loop = 0;
  while(true)
    {
      if(loop>1000) break;
      loop++;

      int mv = rand()%modulo;
      if(mv==prod)
	{
	  l_move.push_back(GE_PASS_MOVE);
	  return l_move.size();
	}
      
      if(referee->is_allowed(gob, mv, tc))
	{
	  if(avoid_atari)
	    if(not is_atari(gob, mv))
	      {
		l_move.push_back(mv);
		return l_move.size();
	      } 
	}
    }
  
  l_move.push_back(GE_PASS_MOVE);
  return l_move.size();
}

int GE_GoPlayerFlea::get_move(const GE_Goban& gob) const
{  
  if(not game)
    return GE_PASS_MOVE;
  
  if(not referee)
    return GE_PASS_MOVE;
  
  const int prod = gob.height*gob.width;
  const int modulo = prod + 1; //+ Pass move
  
  while(true)
    {
      int mv = rand()%modulo;
      if(mv==prod)
	return GE_PASS_MOVE;
      
      if(not referee)
	return GE_PASS_MOVE;
      
      if(referee->is_allowed(gob, mv, tc))
	{
	  if(avoid_atari)
	    if(not is_atari(gob, mv))
	      return mv;
	} 
    }
  
  return GE_PASS_MOVE;
}


GE_GoPlayerOctopus::GE_GoPlayerOctopus()
  : GE_GoPlayerAnimal()
{
  name = "octopus";
  gp = 0;
  nb_players = 0;
  mode_print = false;
  nb_max_move = GE_PARAM_MAX_MOVE;
}

GE_GoPlayerOctopus::GE_GoPlayerOctopus(GE_Game& g, const GE_RefereeGo& rg, 
				 const team_color& col, 
				 list<GE_GoPlayerAnimal*>& lgp, const bool is_print)
  : GE_GoPlayerAnimal(g, rg, col)
{
  name = "octopus";
  nb_players = lgp.size();
  mode_print = is_print;
  nb_max_move = GE_PARAM_MAX_MOVE;
  
  list<GE_GoPlayerAnimal*>::iterator i_gp = lgp.begin();
  gp = new GE_GoPlayerAnimal*[nb_players];
  for(int i = 0; i<nb_players; i++)
    {
      gp[i] = *i_gp;
      i_gp++;
    }

  this->tournament();
  this->init_sub_players(g, rg, tc);
}

GE_GoPlayerOctopus::~GE_GoPlayerOctopus()
{
  if(gp)
    {
      delete[] gp;
      gp = 0;
    }
}

void GE_GoPlayerOctopus::init_sub_players(GE_Game& g, const GE_RefereeGo& rg, team_color tc)
{
  for(int i = 0; i<nb_players; i++)
    {
      gp[i]->game = &g;
      gp[i]->referee = &rg;
      gp[i]->tc = tc;
      
      GE_GoPlayerOctopus* gpo = dynamic_cast<GE_GoPlayerOctopus*>(gp[i]);
      if(not gpo) continue;
      
      gpo->init_sub_players(g, rg, tc);
    }
}

int GE_GoPlayerOctopus::get_moves(const GE_Goban& gob, list<int>& l_move) const
{
  if((not gp) || (nb_players==0) || (not referee) || (not game))
    {
      l_move.push_back(GE_PASS_MOVE);
      return l_move.size();
    }
  
  (gp[0])->get_moves(gob, l_move);
  if(l_move.empty())
    {
      l_move.push_back(GE_PASS_MOVE);
      return l_move.size();
    }
  
  return l_move.size();
}

int GE_GoPlayerOctopus::get_move(const GE_Goban& gob) const
{
  list<int> temp_lmove;
  list<int> l_move;
  
  if((not gp) || (nb_players==0) || (not referee) || (not game))
    {
      l_move.push_back(GE_PASS_MOVE);
      return l_move.size();
    }
  
  (gp[0])->get_moves(gob, l_move);

  if(l_move.empty())
    return GE_PASS_MOVE;

  for(int i = 1; i<nb_players; i++)
    {
      list<int> temp_lmove;
      list<int> inter;
      (gp[i])->get_moves(gob, temp_lmove);
      if(GE_ListOperators::intersection(l_move, temp_lmove, inter))
	l_move = inter;
    }

  int mv = GE_ListOperators::random(l_move);
  
  if(referee->is_allowed(gob, mv, tc))
    return mv;
  
  return GE_PASS_MOVE;
}


bool GE_GoPlayerOctopus::match(const int p1, const int p2)
{
  GE_Goban gob;
  GE_Game g(gob);
  const GE_RefereeGo ref;

  
  GE_GoPlayerAnimal* black = &(*gp[p1]);
  GE_GoPlayerAnimal* white = &(*gp[p2]);
  
  black->tc = GE_BLACK;
  white->tc = GE_WHITE;
  
  black->game = &g;
  black->referee = &ref;

  white->game = &g;
  white->referee = &ref;
  
  GE_GoPlayerOctopus* gpo_black = dynamic_cast<GE_GoPlayerOctopus*>(black);
  GE_GoPlayerOctopus* gpo_white = dynamic_cast<GE_GoPlayerOctopus*>(white);
  
  if(gpo_black) gpo_black->init_sub_players(g, ref, GE_BLACK);
  if(gpo_white) gpo_white->init_sub_players(g, ref, GE_WHITE);
  
  int num_move = 1;
  while(num_move<=nb_max_move)
    {
      g.play(black->get_move(*(g.goban)));
      g.play(white->get_move(*(g.goban)));
      
      //g.play((gp[p1])->get_move(*(g.goban)));
      //g.play((gp[p2])->get_move(*(g.goban)));
      
      num_move++;
    }
 
  return (this->evaluate_score(*(g.goban))>0);
}

void GE_GoPlayerOctopus::tournament()
{
  int num_loop = 0;

  while(num_loop<nb_players)
    {
      num_loop++;
      if(mode_print)
	cout<<"loop: "<<num_loop<<endl;
      
      for(int i = 0; i<nb_players-1; i++)
	{
	  GE_GoPlayerAnimal* gp1 = gp[i];
	  GE_GoPlayerAnimal* gp2 = gp[i+1];
	  
	  if(mode_print)    
	    cout<<"match: "<<gp1->name<<" - "<<gp2->name<<" (";
	  
	  if(this->match(i, i+1))
	    {
	      if(mode_print)
		cout<<gp1->name<<" has won)"<<endl;
	    }
	  else 
	    {
	      if(mode_print)
		cout<<gp2->name<<" has won)"<<endl;
	      this->exchange(i, i+1);
	    }
	}
      
      if(mode_print) cout<<endl;
    }
}

void GE_GoPlayerOctopus::exchange(const int p1, const int p2)
{
  GE_GoPlayerAnimal* temp_player = gp[p1];

  gp[p1] = gp[p2];
  gp[p2] = temp_player;
}


float GE_GoPlayerOctopus::evaluate_score(const GE_Goban& gob) const
{
  int score = 0;
  GE_Strategy strat;
  GE_Goban territories;
  strat.prediction_territories(gob, territories);
  
  for(int i = 0; i<territories.height; i++)
    for(int j = 0; j<territories.width; j++)
      {
	switch(territories.board[i][j])
	  {
	  case GE_WHITE_STONE :
	  case GE_WHITE_STONE_POTENTIAL :
	  case GE_STRONG_WHITE_STONE_POTENTIAL : 
	    score--;
	    break;

	  case GE_BLACK_STONE :
	  case GE_BLACK_STONE_POTENTIAL :
	  case GE_STRONG_BLACK_STONE_POTENTIAL : 
	    score++;
	    break;
	    
	  default:;
	  }
      }

  float score_f = score - GE_KOMI; //the komi is in favour of white
  return score_f;
}





GE_GoPlayerDolphin::GE_GoPlayerDolphin() : GE_GoPlayerAnimal()
{
  name = "dolphin";
  timer = GE_DEFAULT_TIME_THINKING;
  my_komi = GE_KOMI;
  thinking_for_komi = false;
}


GE_GoPlayerDolphin::GE_GoPlayerDolphin(GE_Game& g, const GE_RefereeGo& rg, 
				 const team_color& col, float timing, 
				 bool thinking) 
  : GE_GoPlayerAnimal(g, rg, col)
{
  name = "dolphin";
  timer = timing;
  my_komi = GE_KOMI;
  thinking_for_komi = thinking;

  if(timing<=5.f)
    thinking_for_komi = false;
}


int GE_GoPlayerDolphin::get_moves(const GE_Goban& gob, list<int>& l_move) const
{
  if((not game)||(not referee))
    {
      l_move.push_back(GE_PASS_MOVE);
      return l_move.size();
    }

  int timing = 0;
  clock_t t1 = clock();
  GE_Simulation::choice_komi(*game, &my_komi);
  clock_t t2 = clock();
  timing += (t2 - t1);

  GE_Amaf am(gob.size());
  int nb_sim = 0;
  while(true)
    {
      nb_sim++;
      //cout<<"simulation n°"<<nb_sim<<"   ";//<<endl;
      
      t1 = clock();
      GE_Simulation::launch(*game);
      GE_Simulation::update_amaf(*game, am);
      t2 = clock();
      
      timing += (t2 - t1);
      
      
      if(timing/(float)CLOCKS_PER_SEC>timer)
	break;
    }
  
  list<int> val;
  list<int> l_moves;
  
  for(int i = 0; i<am.size; i++)
    val.push_back(am.values[i]);
  
  if(val.empty()) return GE_PASS_MOVE;
  
  val.sort();
  val.reverse();
  
  for(int i = 0; i<am.size; i++)
    {
      if(am.values[i]==val.front())
	if(game->is_allowed(i))
	  if(not game->is_stupid(i))
	    l_moves.push_back(i); 
    }
  
  if(l_moves.empty())
    {
      l_moves.push_back(GE_PASS_MOVE);
      return l_moves.size();
    }
  
  //allowed moves ? TO IMPLEMENT
  return l_moves.size();
}

int GE_GoPlayerDolphin::get_move(const GE_Goban& gob) const
{
  if(not game)
    return GE_PASS_MOVE;
  
  if(not referee)
    return GE_PASS_MOVE;
  
  int timing = 0;
  
  clock_t t1 = clock();
  //my_komi = GE_Simulation::choice_komi(*game, &my_komi, thinking_for_komi, true);
  clock_t t2 = clock();
  timing += (t2 - t1);
 

  //cout<<"timing "<<timing<<endl;
 
  bool was_updated = GE_Simulation::update_komi_on();
  //GE_Simulation::printing_on();

  GE_Amaf am(gob.size());
  int nb_sim = 0;
  while(true)
    {
      nb_sim++;
      //cout<<"simulation n°"<<nb_sim<<"   "; //<<endl;
      //if(game->tc==GE_WHITE) cout<<"GE_WHITE"<<endl;
      //else cout<<"GE_BLACK"<<endl;
      
      
      t1 = clock();
      GE_Simulation::launch(*game);
      GE_Simulation::update_amaf(*game, am);
      t2 = clock();
      
      timing += (t2 - t1);
      
      
      // 	if(nb_sim%100==0)
      // 	{
      // 	cout<<nb_sim<<endl;
      // 	/*
      // 	  if(game->tc==GE_WHITE)
      // 	  am.preferred_moves(am.white_values, gob.height); 
      // 	  else am.preferred_moves(am.black_values, gob.height);
      //       */
      
      // 	  if(am.get_max(am.white_values, true)>am.get_max(am.black_values, true))
      // 	    am.preferred_moves(am.white_values, gob.height); 
      // 	  else am.preferred_moves(am.black_values, gob.height);
      
      // 	  cout<<endl;
      // 	}
      

      if(timing/(float(CLOCKS_PER_SEC))>timer)
	break;
    }
  
  list<int> val;
  list<int> l_moves;
  
  const int* v = am.values; 
  
  
 
  if(game->tc==GE_WHITE)
    v = am.white_values;
  else v = am.black_values;
  GE_Simulation::reset_update_komi(was_updated);
  
  /*
    if(am.get_max(am.white_values, true)>am.get_max(am.black_values, true))
    v = am.white_values;
    else v = am.black_values;
  */
  
  for(int i = 0; i<am.size; i++)
    val.push_back(v[i]);
  
  
  if(val.empty()) return GE_PASS_MOVE;
  
  val.sort();
  val.reverse();
  
  //if(val.front()<0) cout<<"I think that I will lose"<<endl;
  //else cout<<"I think that I will win"<<endl;
    


  while(not val.empty())
    {
      for(int i = 0; i<am.size; i++)
	{
	  if(v[i]==val.front())
	    if(game->is_allowed(i))
	      if(not game->is_stupid(i))
		l_moves.push_back(i); 
	}
      
      if(l_moves.empty())
	{
	  val.pop_front();
	  continue;
	}

      int mv = GE_ListOperators::random(l_moves);
      
      if(referee->is_allowed(gob, mv, tc))
	return mv;
      
      GE_ListOperators::erase(l_moves, mv);
      if(l_moves.empty())
	val.pop_front();
    }

  for(int i=0; i<gob.size(); i++)
    {
      if(referee->is_allowed(gob, i, tc))
	if(gob.eye(i)==GE_NEUTRAL_COLOR)
	  if(not game->is_self_kill_group0(i))
	    return i;
    }


  return GE_PASS_MOVE;
}





GE_GoPlayerDolphin2::GE_GoPlayerDolphin2() : GE_GoPlayerDolphin()
{
  name = "dolphin2";
  timer = GE_DEFAULT_TIME_THINKING;
  my_komi = GE_KOMI;
  thinking_for_komi = false;
  max_simus = GE_DEFAULT_MAX_SIMUS;
}


GE_GoPlayerDolphin2::GE_GoPlayerDolphin2(GE_Game& g, const GE_RefereeGo& rg, 
				   const team_color& col, int max_simus, 
				   float timing, bool thinking) 
  : GE_GoPlayerDolphin(g, rg, col, timing, thinking)
{
  name = "dolphin2";
  this->max_simus = max_simus; 
}


int GE_GoPlayerDolphin2::get_moves(const GE_Goban& gob, list<int>& l_move) const
{
  if((not game)||(not referee))
    {
      l_move.push_back(GE_PASS_MOVE);
      return l_move.size();
    }

  l_move.push_back(this->get_move(gob));
  
  return l_move.size();
}

int GE_GoPlayerDolphin2::get_move(const GE_Goban& gob) const
{
  if(not game)
    return GE_PASS_MOVE;
  
  if(not referee)
    return GE_PASS_MOVE;
  
  GE_Simulation::update_komi_on();
  //GE_Simulation::printing_on();

  GE_AmafTree at(gob.size());
  at.build_tree(*game, false, this->max_simus);
  
  GE_Simulation::update_komi_off();
  //GE_Simulation::printing_off();

  int mv = at.get_best_move();
  if(mv!=GE_PASS_MOVE)
    if(referee->is_allowed(gob, mv, tc))
      if(not game->is_stupid(mv))
	return mv;
  

  if(not at.last_sequence.empty())
    {
      mv = at.last_sequence.front();
      
      if(referee->is_allowed(gob, mv, tc))
	if(not game->is_stupid(mv))
	  return mv;
    }
  
  for(int i=0; i<gob.size(); i++)
    {
      if(referee->is_allowed(gob, i, tc))
	if(gob.eye(i)==GE_NEUTRAL_COLOR)
	  return i;
    }

  
  return GE_PASS_MOVE;
}



GE_GoPlayerDolphin3::GE_GoPlayerDolphin3() : GE_GoPlayerAnimal()
{
  name = "dolphin3";
  timer = GE_DEFAULT_TIME_THINKING;
  my_komi = GE_KOMI;
  thinking_for_komi = false;
}


GE_GoPlayerDolphin3::GE_GoPlayerDolphin3(GE_Game& g, const GE_RefereeGo& rg, 
				 const team_color& col, float timing, 
				 bool thinking) 
  : GE_GoPlayerAnimal(g, rg, col)
{
  name = "dolphin3";
  timer = timing;
  my_komi = GE_KOMI;
  thinking_for_komi = thinking;

  if(timing<=5.f)
    thinking_for_komi = false;
}


int GE_GoPlayerDolphin3::get_moves(const GE_Goban& gob, list<int>& l_move) const
{
  if((not game)||(not referee))
    {
      l_move.push_back(GE_PASS_MOVE);
      return l_move.size();
    }

  list<int> solutions;
  if(GE_Algorithm::solutions0(*game, solutions))
    if(not solutions.empty())
      return solutions.size();


  int timing = 0;
  clock_t t1 = clock();
  GE_Simulation::choice_komi(*game, &my_komi);
  clock_t t2 = clock();
  timing += (t2 - t1);

  GE_Amaf am(gob.size());
  int nb_sim = 0;
  while(true)
    {
      nb_sim++;
      //cout<<"simulation n°"<<nb_sim<<"   ";//<<endl;
      
      t1 = clock();
      GE_Simulation::launch(*game);
      GE_Simulation::update_amaf(*game, am);
      t2 = clock();
      
      timing += (t2 - t1);
      
      
      if(timing/(float)CLOCKS_PER_SEC>timer)
	break;
    }
  
  list<int> val;
  list<int> l_moves;
  
  for(int i = 0; i<am.size; i++)
    val.push_back(am.values[i]);
  
  if(val.empty()) return GE_PASS_MOVE;
  
  val.sort();
  val.reverse();
  
  for(int i = 0; i<am.size; i++)
    {
      if(am.values[i]==val.front())
	if(game->is_allowed(i))
	  if(not game->is_stupid(i))
	    l_moves.push_back(i); 
    }
  
  if(l_moves.empty())
    {
      l_moves.push_back(GE_PASS_MOVE);
      return l_moves.size();
    }
  
  //allowed moves ? TO IMPLEMENT
  return l_moves.size();
}

int GE_GoPlayerDolphin3::get_move(const GE_Goban& gob) const
{
  if(not game)
    return GE_PASS_MOVE;
  
  if(not referee)
    return GE_PASS_MOVE;

  
  list<int> solutions;
  //if(GE_Algorithm::solutions0(*game, solutions))
  if(GE_Algorithm::solutions1(*game, solutions))
    if(not solutions.empty())
      {
	int mv = GE_ListOperators::random(solutions);
	
	if(referee->is_allowed(gob, mv, tc))
	  return mv;
      }
  
  int timing = 0;
  
  clock_t t1 = clock();
  //my_komi = GE_Simulation::choice_komi(*game, &my_komi, thinking_for_komi, true);
  clock_t t2 = clock();
  timing += (t2 - t1);
 

  //cout<<"timing "<<timing<<endl;
 
  bool was_updated = GE_Simulation::update_komi_on();
  //GE_Simulation::printing_on();

  GE_Amaf am(gob.size());
  int nb_sim = 0;
  while(true)
    {
      nb_sim++;
     
      t1 = clock();
      GE_Simulation::launch(*game);
      GE_Simulation::update_amaf(*game, am);
      t2 = clock();
      
      timing += (t2 - t1);      

      if(timing/(float(CLOCKS_PER_SEC))>timer)
	break;
    }
  
  list<int> val;
  list<int> l_moves;
  
  const int* v = am.values; 
  
  
 
  if(game->tc==GE_WHITE)
    v = am.white_values;
  else v = am.black_values;
  GE_Simulation::reset_update_komi(was_updated);
  
  /*
    if(am.get_max(am.white_values, true)>am.get_max(am.black_values, true))
    v = am.white_values;
    else v = am.black_values;
  */
  
  for(int i = 0; i<am.size; i++)
    val.push_back(v[i]);
  
  
  if(val.empty()) return GE_PASS_MOVE;
  
  val.sort();
  val.reverse();
  
  //if(val.front()<0) cout<<"I think that I will lose"<<endl;
  //else cout<<"I think that I will win"<<endl;
    


  while(not val.empty())
    {
      for(int i = 0; i<am.size; i++)
	{
	  if(v[i]==val.front())
	    if(game->is_allowed(i))
	      if(not game->is_stupid(i))
		l_moves.push_back(i); 
	}
      
      if(l_moves.empty())
	{
	  val.pop_front();
	  continue;
	}

      int mv = GE_ListOperators::random(l_moves);
      
      if(referee->is_allowed(gob, mv, tc))
	return mv;
      
      GE_ListOperators::erase(l_moves, mv);
      if(l_moves.empty())
	val.pop_front();
    }

  for(int i=0; i<gob.size(); i++)
    {
      if(referee->is_allowed(gob, i, tc))
	if(gob.eye(i)==GE_NEUTRAL_COLOR)
	  if(not game->is_self_kill_group0(i))
	    return i;
    }


  return GE_PASS_MOVE;
}



GE_GoPlayerHorse::GE_GoPlayerHorse() : GE_GoPlayerAnimal()
{ 
  name = "horse";
  min_capture = 4;
  komi = 7.5f;
  timer = 1.f;
  max_simulations = 10000;
}

GE_GoPlayerHorse::GE_GoPlayerHorse(GE_Game& g, const GE_RefereeGo& rg, 
			     const team_color& col, int min_taken, float km, 
			     float timing, int max_sim) 
  : GE_GoPlayerAnimal(g, rg, col)
{
  name = "horse";
  min_capture = min_taken;
  komi = km;
  timer = timing;
  max_simulations = max_sim;
  set_resign(true);
}


int GE_GoPlayerHorse::get_moves(const GE_Goban& gob, list<int>& l_move) const
{
  if((not game)||(not referee))
    {
      l_move.push_back(GE_PASS_MOVE);
      return l_move.size();
    }
  
  int turn = (game->tc==GE_BLACK) ? GE_BLACK_STONE : GE_WHITE_STONE;
  
  GE_FastGoban fg;
  GE_FastGoban fg_for_simu;
  
  //GE_Directions::init(gob.height, gob.width);
  fg.init();
  fg_for_simu.init();
  fg.fromGame(*game);
  
  GE_FastMCLocationsInformations fmcli(turn);
  
  clock_t t_start;
  clock_t t_end;
  float timing = 0.f;
  
  int num_sim = 0; 
  int num_criterion = 18;
  while(true)
    {      
      t_start = clock();
      fg_for_simu.fromGame(*game);
      fg_for_simu.launch_simulation(min_capture);
      fmcli.update(fg, komi);
      t_end = clock();
      
      timing+=((t_end-t_start)/(float)CLOCKS_PER_SEC);
      
      num_sim++;
      
      if((timer>=0)&&(timing>timer))
	break;
      
      if((max_simulations>=0)&&(num_sim>=max_simulations))
	break;
    }
  
  fmcli.get_best_moves(l_move, num_criterion, game, referee);
  
  if(l_move.empty())
    l_move.push_back(GE_PASS_MOVE);

  return l_move.size();
}

int GE_GoPlayerHorse::get_move(const GE_Goban& gob) const
{  
  if((not game)||(not referee))
    return GE_PASS_MOVE;
  
  list<int> l_move;
  int turn = (game->tc==GE_BLACK) ? GE_BLACK_STONE : GE_WHITE_STONE;
  
  GE_FastGoban fg;
  GE_FastGoban fg_for_simu;

  //GE_Directions::init(gob.height, gob.width);
  fg.init();
  fg_for_simu.init();
  fg.fromGame(*game);
  
  GE_FastMCLocationsInformations fmcli(turn);
  
  clock_t t_start;
  clock_t t_end;
  float timing = 0.f;
  
  int num_sim = 0; 
  int num_criterion = 18;
  while(true)
    {      
      t_start = clock();
      //fg.fromGame(*game);
      fg_for_simu.copy(fg);
      fg_for_simu.launch_simulation(min_capture);
      fmcli.update(fg_for_simu, komi);
      t_end = clock();
      
      timing+=((t_end-t_start)/(float)CLOCKS_PER_SEC);
      
      num_sim++;
      
      if((timer>=0)&&(timing>timer))
	break;
      
      if((max_simulations>=0)&&(num_sim>=max_simulations))
	break;
    }
  
  if(can_resign)
    {
      if((fmcli.nb_wins==0)&&(fmcli.nb_simulations>=1000))
	return GE_RESIGN;
    }
  
  
  fmcli.get_best_moves(l_move, num_criterion, game, referee);
  
  if(not l_move.empty())
    return GE_ListOperators::random(l_move);
    
  
  for(int i=0; i<gob.size(); i++)
    {
      if(referee->is_allowed(gob, i, tc))
	if(gob.eye(i)==GE_NEUTRAL_COLOR)
	  if(not game->is_self_kill_group0(i))
	    return i;
    }
  
  return GE_PASS_MOVE;
 
}




GE_GoPlayerSquirrel::GE_GoPlayerSquirrel() : GE_GoPlayerAnimal()
{ 
  name = "squirrel";
  min_capture = 4;
  komi = 7.5f;
  timer = 1.f;
  max_simulations = 10000;
  max_simulations_by_node = 1;
  coeff_exploration = 1;
}

GE_GoPlayerSquirrel::GE_GoPlayerSquirrel(GE_Game& g, const GE_RefereeGo& rg, 
				   const team_color& col, int min_taken, 
				   float km, float timing, int max_sim, 
				   int sim_by_node, double coeff_exp) 
  : GE_GoPlayerAnimal(g, rg, col)
{
  name = "squirrel";
  min_capture = min_taken;
  komi = km;
  timer = timing;
  max_simulations = max_sim;
  set_resign(true);
  max_simulations_by_node = sim_by_node;
  coeff_exploration = coeff_exp;
}

bool GE_GoPlayerSquirrel::pass(bool* no_resign) const
{
  int max_sim_for_pass = 100;
  int turn_adv = (game->tc==GE_BLACK) ? GE_WHITE_STONE : GE_BLACK_STONE;
  
  game->play(GE_PASS_MOVE);
  GE_FastMCLocationsInformations fmcli(turn_adv);
  
  GE_FastGoban fg;
  GE_FastGoban fg_for_simu;
  
  fg.init();
  fg_for_simu.init();
  fg.fromGame(*game);
  
  int num_sim = 0; 
  while(true)
    {      
      //fg.fromGame(*game);
      fg_for_simu.copy(fg);
      fg_for_simu.launch_simulation(min_capture);
      fmcli.update(fg_for_simu, komi);
      
      num_sim++;
      
      
      if(num_sim>=max_sim_for_pass)
	break;
    }
  
  game->backward();
  
  if(no_resign)
    {
      if(fmcli.nb_wins!=fmcli.nb_simulations)
	{
	  float winning = GE_EvaluationScore::evaluate_score2(*(game->goban), 
							   komi);
	  
	  if(((winning>0)&&(game->tc==GE_BLACK))
	     ||((winning<0)&&(game->tc==GE_WHITE)))
	    (*no_resign) = true;
	  else (*no_resign) = false;
	}
      else (*no_resign) = false;
    }


  if(fmcli.nb_wins==0)
    return true;

  return false;
}



int GE_GoPlayerSquirrel::get_moves(const GE_Goban& gob, list<int>& l_move) const
{
  l_move.push_back(GE_PASS_MOVE);
  return 0;
}

int GE_GoPlayerSquirrel::get_move(const GE_Goban& gob) const
{ 
  //A EFFACER
  // {
//     list<int> temp_l_moves;
//     GE_Tactic tact;
//     if(tact.study_shisho2(*game, game->get_last_move(), &temp_l_moves))
//       {
// 	GE_ListOperators::print_error(temp_l_moves, move_to_string, GE_Directions::board_size);
//       }
    
//     return GE_PASS_MOVE;
//   }
  

  //END A EFFACER
  
 
  if((not game)||(not referee))
    return GE_PASS_MOVE;
  
  //  if((game->tc)!=tc)
  //     cerr<<"*** ERROR *** Bad color of the player"<<endl;
  
  //   if(tc==GE_BLACK)
  //     cerr<<"Black Player";
  //   else cerr<<"White Player"<<endl;

  //{
  //     //TO BE DELETED
  
  //     int num_sim_stats = 0;
  //     int turn_stats = (game->tc==GE_BLACK) ? GE_BLACK_STONE : GE_WHITE_STONE;
  
  //     GE_FastGoban fg_stats;
  //     GE_FastGoban fg_for_simu_stats;
  
  //     //GE_Directions::init(gob.height, gob.width);
  //     fg_stats.init();
  //     fg_for_simu_stats.init();
  //     fg_stats.fromGame(*game);
  
  //     GE_FastMCLocationsInformations fmcli_stats(turn_stats);
  
  
  //     while(true)
  //       {      
  // 	fg_for_simu_stats.copy(fg_stats);
  // 	fg_for_simu_stats.launch_simulation(min_capture);
  // 	fmcli_stats.update(fg_for_simu_stats, komi);
  
  // 	num_sim_stats++;
  
  
  // 	if(num_sim_stats>=200000)
  // 	  break;
  //       }
  
  
  //     fmcli_stats.print_stats();
  
  //     return GE_RESIGN;
  
  //     //TO BE DELETED
  //   }
  
  
  if(game->get_last_move()==GE_PASS_MOVE)
    {
      float winning = GE_EvaluationScore::evaluate_score2(*(game->goban), komi);
      
      if(((winning>0)&&(game->tc==GE_BLACK))
	 ||((winning<0)&&(game->tc==GE_WHITE)))
	return GE_PASS_MOVE;
    }
  
  bool no_resign = false;
  if(pass(&no_resign)) 
    {
      //for CGOS 
      //float winning = GE_EvaluationScore::evaluate(game, komi);
      float winning = GE_EvaluationScore::evaluate_score2(*(game->goban), komi);
      
      if(((winning>0)&&(game->tc==GE_BLACK))
	 ||((winning<0)&&(game->tc==GE_WHITE)))
	return GE_PASS_MOVE;
      
      GE_GoPlayerHorse gph0(*game, *referee, tc, 
			 min_capture, komi, -1, 100000); 
      return gph0.get_move(*(game->goban));
      //end for CGOS
      
      
      return GE_PASS_MOVE;
    }
  
  
  list<int> l_move;
  int turn = (game->tc==GE_BLACK) ? GE_BLACK_STONE : GE_WHITE_STONE;
  
  GE_FastGoban fg;
  GE_FastGoban fg_for_simu;

  //GE_Directions::init(gob.height, gob.width);
  fg.init();
  fg_for_simu.init();
  fg.fromGame(*game);
  
  GE_FastMCLocationsInformations fmcli(turn);
  
  clock_t t_start;
  clock_t t_end;
  float timing = 0.f;


  //&&&&&
  vector<vector<int> > v_semeai;
  v_semeai.resize(GE_Directions::board_area);
  for(int i = 0; i<(int)v_semeai.size(); i++)
    {
      v_semeai[i].resize(GE_Directions::board_area, 0);
    }
  
  vector<int> v_group;
  v_group.resize(GE_Directions::board_area, 0);
  
  
  //&&&&&
  
  fg.print_error_goban();
  
  int num_sim = 0; 
  int max_sim_for_resign = 100;
  while(true)
    {      
      t_start = clock();
      //fg.fromGame(*game);
      fg_for_simu.copy(fg);
      fg_for_simu.launch_simulation(min_capture);
      fmcli.update(fg_for_simu, komi);
      
      //&&&&&
      //cerr<<"### "<<num_sim<<" ###"<<endl;
      //fg_for_simu.print_error_goban();
      GE_FastMCLocationsInformations::update_stats_semeai(fg_for_simu, v_semeai);
      GE_FastMCLocationsInformations::update_stats_group(fg, fg_for_simu, 
						      v_group);
      
      //&&&&&

      t_end = clock();
      
      timing+=((t_end-t_start)/(float)CLOCKS_PER_SEC);
      
      num_sim++;
      
      
      if(num_sim>=max_sim_for_resign)
	break;
    }
  
  //&&&&&
  GE_Chains chs;
  GE_AnalyzeSemeai a_sem;
  a_sem.setSemeais(*game, v_semeai, max_sim_for_resign, chs);
  a_sem.print_error_moves_for_semeai(*game);
  //a_sem.print_error_moves_for_semeai_and_a_solution(*game);
  if(not (a_sem.semeais).empty())
    {
      /*
      {
	LocalUct loc_uct;
	loc_uct.init(*game, komi, 1, min_capture, 
		     coeff_exploration);
	loc_uct.setSemeai((a_sem).semeais.back());

	loc_uct.compute(10000);
	int candidate_move0 = loc_uct.best_move();
	
	return GE_PASS_MOVE;
      }
      */

      /*
      list<int> l_moves;
      double result = a_sem.study_semeai(*game, (a_sem.semeais).back(), &l_moves);
      
      cerr<<"resultat semeai : ";
      cerr<<result<<endl;
      cerr<<"solutions: ";
      GE_ListOperators::print_error(l_moves, move_to_string, 
				 GE_Directions::board_size);
      cerr<<endl;
      */
    }

 
  //for(int i = 0; i<1000; i++)
  //{
  //if(i%100==0) cerr<<i<<endl;
  GE_Informations infos(*game);
  infos.init_stats_group(v_group, max_sim_for_resign, game->tc);
  infos.init_neighbour_groups(*game);
  infos.init_group_mobility(*game);
  infos.init_virtual_connections();
  infos.init_eyes();
  infos.init_false_eyes();
  infos.init_type_holes();
  infos.find_unkillable();
  infos.init_alive_with_unkillable_group();
  infos.init_alive_with_possible_connexion();
  infos.init_alive_by_direct_connexion_two_eyes();
  infos.init_alive_by_connexion_different_one_eyes();
  infos.init_alive_by_connexion_two_eyes();
  infos.init_alive_with_alive_group();
  
  infos.study_alive(*game);
  infos.find_sekis();
  
  
  //infos.init_false_wall_connections();
  
  infos.init_groups_networks(GE_POSSIBLE_CONNEXION);
  
  infos.study_alive_by_stats();
  
  infos.refine_possible_eyes();
  infos.init_possible_eyes();
    
  
  //}
  infos.print_error();
  
  for(int i = 0; i<(int)infos.groups.size(); i++)
    {
      const GE_Chain& ch = ((infos.groups)[i])->ch;
      ch.print_error();
      
      if(ch.is_alive(*(game->goban)))
	{
	  cerr<<"IN LIFE"<<endl<<endl;
	}
      else cerr<<"GE_UNKNOWN"<<endl<<endl;
      
    }
  
  int fast_move = GE_Algorithm::reflex_move(*game, &infos);
  if(fast_move!=GE_PASS_MOVE)
    {
      if(game->play(fast_move))
	{
	  game->backward();
	  return fast_move;
	}
      else
	assert(0);
    }
  

  
  a_sem.initVitalStones(*game, infos);
  a_sem.initFrontiers(*game, infos);
  a_sem.toolsLiberties(*game);
  //a_sem.print_error_moves_for_semeai_and_a_solution(*game);
  
  //a_sem.correctFalseSemeai(*game, infos);

  
  //a_sem.study_semeais(*game, -1, &infos);
  //int answer_semeai = a_sem.study_semeais2(*game, 1000, &infos);
  //if(answer_semeai!=GE_PASS_MOVE)
  //return answer_semeai;
  //a_sem.study_semeais3(*game, -1, &infos);
  int answer = a_sem.solve_semeais(*game, 500, 1000, &infos, 2000, 5); //500 20
  if(answer!=GE_PASS_MOVE) return answer;
  

  //return GE_PASS_MOVE;
  
  GE_AnalyzeSemeai a_dol;
  a_dol.setDeathOrLife(*game, infos);
  a_dol.toolsLiberties(*game);
  //a_dol.print_error_semeais();
  //a_dol.study_semeais3(*game, 100, &infos);

  //a_dol.solve_semeais(*game, 500, 1000, &infos, -1, 5);
  
  
  //  string mv1 = "B1";
  //   string mv2 = "F2";
  //   int loc1 = string_gtp_to_move(mv1, GE_Directions::board_size);
  //   int loc2 = string_gtp_to_move(mv2, GE_Directions::board_size);
  
  //   GE_Chain toto1; toto1.init(*(game->goban), loc1);
  //   GE_Chain toto2; toto2.init(*(game->goban), loc2);
  
  //   int sacrifice = GE_PASS_MOVE;
  //   a_sem.can_sacrifice(*game, toto1, toto2, true, &sacrifice);
  
  //   cerr<<move_to_string(GE_Directions::board_size, sacrifice)<<endl;
  
  //   assert(0);
  

  //&&&&&
  
  
  
  if(can_resign)
    {
      if((fmcli.nb_wins==0)&&(fmcli.nb_simulations>=100))
	{
	  if(no_resign) return GE_PASS_MOVE;
	  return GE_RESIGN;
	}
    }
  
  //for CGOS
  if(fmcli.nb_wins>80)
    if(max_simulations>2000)
      max_simulations = 2000;
  
  
  
  GE_Uct1 uct1;
  uct1.init(*game, komi, max_simulations_by_node, min_capture, 
	    coeff_exploration);
  uct1.compute(max_simulations, &infos);
  //uct1.compute(max_simulations);
  int candidate_move = uct1.best_move();
  
  
  
  list<int> forbidden_moves;
  uct1.get_forbidden_moves_by_expertise(forbidden_moves, &infos);
  
  if(GE_ListOperators::is_in(forbidden_moves, candidate_move))
    {
      std::cerr<<move_to_string(GE_Directions::board_size, candidate_move);
      std::cerr<<" REFUSED BY EXPERTISE"<<endl;
      
      
      GE_Uct1 uct2;
      
      uct2.init(*game, komi, max_simulations_by_node, min_capture, 
		coeff_exploration);
      uct2.forbidden_moves = forbidden_moves;
      uct2.compute(max_simulations, &infos);
      
      int candidate_move2 = uct2.best_move();
      
      double eval1 = uct1.getEval();
      double eval2 = uct2.getEval();
      
      if(eval1<=eval2+0.05f)
	candidate_move = candidate_move2;
    }
  
  
  
  
  //cerr<<uct1.getEval()<<endl;
  if(can_resign)
    if(not no_resign)
      if(uct1.getEval()<0.05f)
	return GE_RESIGN;
  
  if(referee->is_allowed(*(game->goban), candidate_move, game->tc))
    {
      if((not game->is_stupid(candidate_move))
	 ||(uct1.getEval()>0.2f)) //0.5
	return candidate_move;
    }
  
  
  std::cerr<<move_to_string(GE_Directions::board_size, candidate_move);
  std::cerr<<" REFUSED"<<endl;

  GE_GoPlayerHorse gph(*game, *referee, tc, 
		    min_capture, komi, timing, 100000); 
  return gph.get_move(*(game->goban));
  
  for(int i=0; i<gob.size(); i++)
    {
      if(referee->is_allowed(gob, i, tc))
	if(gob.eye(i)==GE_NEUTRAL_COLOR)
	  if(not game->is_self_kill_group0(i))
	    return i;
    }
  
  return GE_PASS_MOVE;
 
}



GE_GoPlayerSquirrel2::GE_GoPlayerSquirrel2() : GE_GoPlayerAnimal()
{ 
  name = "squirrel2";
  min_capture = 4;
  komi = 7.5f;
  timer = 1.f;
  max_simulations = 10000;
  max_simulations_by_node = 1;
  coeff_exploration = 1;
}

GE_GoPlayerSquirrel2::GE_GoPlayerSquirrel2(GE_Game& g, const GE_RefereeGo& rg, 
				   const team_color& col, int min_taken, 
				   float km, float timing, int max_sim, 
				   int sim_by_node, double coeff_exp) 
  : GE_GoPlayerAnimal(g, rg, col)
{
  name = "squirrel2";
  min_capture = min_taken;
  komi = km;
  timer = timing;
  max_simulations = max_sim;
  set_resign(true);
  max_simulations_by_node = sim_by_node;
  coeff_exploration = coeff_exp;
}

bool GE_GoPlayerSquirrel2::pass(bool* no_resign) const
{
  int max_sim_for_pass = 100;
  int turn_adv = (game->tc==GE_BLACK) ? GE_WHITE_STONE : GE_BLACK_STONE;
  
  game->play(GE_PASS_MOVE);
  GE_FastMCLocationsInformations fmcli(turn_adv);
  
  GE_FastGoban fg;
  GE_FastGoban fg_for_simu;
  
  fg.init();
  fg_for_simu.init();
  fg.fromGame(*game);
  
  int num_sim = 0; 
  while(true)
    {      
      //fg.fromGame(*game);
      fg_for_simu.copy(fg);
      fg_for_simu.launch_simulation(min_capture);
      fmcli.update(fg_for_simu, komi);
      
      num_sim++;
      
      
      if(num_sim>=max_sim_for_pass)
	break;
    }
  
  game->backward();
  
  if(no_resign)
    {
      if(fmcli.nb_wins!=fmcli.nb_simulations)
	{
	  float winning = GE_EvaluationScore::evaluate_score2(*(game->goban), 
							   komi);
	  
	  if(((winning>0)&&(game->tc==GE_BLACK))
	     ||((winning<0)&&(game->tc==GE_WHITE)))
	    (*no_resign) = true;
	  else (*no_resign) = false;
	}
      else (*no_resign) = false;
    }


  if(fmcli.nb_wins==0)
    return true;

  return false;
}



int GE_GoPlayerSquirrel2::get_moves(const GE_Goban& gob, list<int>& l_move) const
{
  l_move.push_back(GE_PASS_MOVE);
  return 0;
}

int GE_GoPlayerSquirrel2::get_move(const GE_Goban& gob) const
{ 
  //A EFFACER
  // {
//     list<int> temp_l_moves;
//     GE_Tactic tact;
//     if(tact.study_shisho2(*game, game->get_last_move(), &temp_l_moves))
//       {
// 	GE_ListOperators::print_error(temp_l_moves, move_to_string, GE_Directions::board_size);
//       }
    
//     return GE_PASS_MOVE;
//   }
  //END A EFFACER

 
  if((not game)||(not referee))
    return GE_PASS_MOVE;
  
  //  if((game->tc)!=tc)
  //     cerr<<"*** ERROR *** Bad color of the player"<<endl;
  
  //   if(tc==GE_BLACK)
  //     cerr<<"Black Player";
  //   else cerr<<"White Player"<<endl;

  //{
  //     //TO BE DELETED
  
  //     int num_sim_stats = 0;
  //     int turn_stats = (game->tc==GE_BLACK) ? GE_BLACK_STONE : GE_WHITE_STONE;
  
  //     GE_FastGoban fg_stats;
  //     GE_FastGoban fg_for_simu_stats;
  
  //     //GE_Directions::init(gob.height, gob.width);
  //     fg_stats.init();
  //     fg_for_simu_stats.init();
  //     fg_stats.fromGame(*game);
  
  //     GE_FastMCLocationsInformations fmcli_stats(turn_stats);
  
  
  //     while(true)
  //       {      
  // 	fg_for_simu_stats.copy(fg_stats);
  // 	fg_for_simu_stats.launch_simulation(min_capture);
  // 	fmcli_stats.update(fg_for_simu_stats, komi);
  
  // 	num_sim_stats++;
  
  
  // 	if(num_sim_stats>=200000)
  // 	  break;
  //       }
  
  
  //     fmcli_stats.print_stats();
  
  //     return GE_RESIGN;
  
  //     //TO BE DELETED
  //   }
  
  
  if(game->get_last_move()==GE_PASS_MOVE)
    {
      float winning = GE_EvaluationScore::evaluate_score2(*(game->goban), komi);
      
      if(((winning>0)&&(game->tc==GE_BLACK))
	 ||((winning<0)&&(game->tc==GE_WHITE)))
	return GE_PASS_MOVE;
    }
  
  bool no_resign = false;
  if(pass(&no_resign)) 
    {
      //for CGOS 
      //float winning = GE_EvaluationScore::evaluate(game, komi);
      float winning = GE_EvaluationScore::evaluate_score2(*(game->goban), komi);
      
      if(((winning>0)&&(game->tc==GE_BLACK))
	 ||((winning<0)&&(game->tc==GE_WHITE)))
	return GE_PASS_MOVE;
      
      GE_GoPlayerHorse gph0(*game, *referee, tc, 
			 min_capture, komi, -1, 100000); 
      return gph0.get_move(*(game->goban));
      //end for CGOS
      
      
      return GE_PASS_MOVE;
    }
  
  
  list<int> l_move;
  int turn = (game->tc==GE_BLACK) ? GE_BLACK_STONE : GE_WHITE_STONE;
  
  GE_FastGoban fg;
  GE_FastGoban fg_for_simu;

  //GE_Directions::init(gob.height, gob.width);
  fg.init();
  fg_for_simu.init();
  fg.fromGame(*game);
  
  GE_FastMCLocationsInformations fmcli(turn);
  
  clock_t t_start;
  clock_t t_end;
  float timing = 0.f;
  
  int num_sim = 0; 
  while(true)
    {      
      t_start = clock();
      //fg.fromGame(*game);
      fg_for_simu.copy(fg);
      fg_for_simu.launch_simulation(min_capture);
      fmcli.update(fg_for_simu, komi);
      t_end = clock();
      
      timing+=((t_end-t_start)/(float)CLOCKS_PER_SEC);
      
      num_sim++;
      
      
      if(num_sim>=100)
	break;
    }
  
  if(can_resign)
    {
      if((fmcli.nb_wins==0)&&(fmcli.nb_simulations>=100))
	{
	  if(no_resign) return GE_PASS_MOVE;
	  return GE_RESIGN;
	}
    }
 
  
  list<int> solutions;
  //if(GE_Algorithm::solutions0(*game, solutions))
  if(GE_Algorithm::solutions1(*game, solutions))
    if(not solutions.empty())
      {
	int mv = GE_ListOperators::random(solutions);
	
	if(referee->is_allowed(gob, mv, tc))
	  return mv;
      }
  
  
  //for CGOS
  if(fmcli.nb_wins>80)
    if(max_simulations>2000)
      max_simulations = 2000;
  

  GE_Uct1 uct1;
  uct1.init(*game, komi, max_simulations_by_node, min_capture, 
	    coeff_exploration);
  uct1.compute(max_simulations);
  int candidate_move = uct1.best_move();
  if(referee->is_allowed(*(game->goban), candidate_move, game->tc))
    {
      if((not game->is_stupid(candidate_move))
	 ||(uct1.getEval()>0.2f)) //0.5
	return candidate_move;
    }
	  
	  
  std::cerr<<move_to_string(GE_Directions::board_size, candidate_move);
  std::cerr<<" REFUSED"<<endl;

  GE_GoPlayerHorse gph(*game, *referee, tc, 
		    min_capture, komi, timing, 100000); 
  return gph.get_move(*(game->goban));
  
  for(int i=0; i<gob.size(); i++)
    {
      if(referee->is_allowed(gob, i, tc))
	if(gob.eye(i)==GE_NEUTRAL_COLOR)
	  if(not game->is_self_kill_group0(i))
	    return i;
    }
  
  return GE_PASS_MOVE;
 
}



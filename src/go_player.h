#ifndef GE_GO_PLAYER_H
#define GE_GO_PLAYER_H

#include "referee.h"
#include "goban.h"
#include "move.h"
#include "manager_game.h"
#include "chain.h"
#include "strategy.h"
#include "tools_ge.h"
#include "param_go_player.h"
#include "const_rule.h"
#include "manager_moves.h"
#include "filter.h"
#include "simulation.h"
#include "amaf_tree.h"
#include "algorithm.h"
#include "fast_mc_locations_informations.h"
#include "uct1.h"
#include "semeai.h"
#include "location_information.h"
//#include "local_uct.h"

class GE_GoPlayer
{
 public:
  string name;
  bool can_resign;

 public:
  const GE_RefereeGo* referee;
  team_color tc;
  bool avoid_atari;

  GE_GoPlayer(); 
  GE_GoPlayer(const GE_RefereeGo&);
  GE_GoPlayer(const GE_RefereeGo&, const team_color&);
  virtual ~GE_GoPlayer() {}

  int set_player(const team_color);
  bool set_resign(bool);
  bool is_atari(const GE_Goban&, const int) const;
  virtual int get_moves(const GE_Goban&, list<int>&) const;
  virtual int get_move(const GE_Goban&) const;
  virtual bool is_human() const;

  virtual void set_komi(float) {};
};


class GE_HumanGoPlayer : public GE_GoPlayer
{
 public:
  GE_HumanGoPlayer();
  GE_HumanGoPlayer(const GE_RefereeGo&);
  GE_HumanGoPlayer(const GE_RefereeGo&, const team_color&);
  
  
  virtual ~GE_HumanGoPlayer() {}
  
  virtual int get_move(const GE_Goban&) const;
  virtual bool is_human() const;
};

class GE_Goldeneye : public GE_GoPlayer
{
 public:
  GE_Goldeneye();
  GE_Goldeneye(const GE_RefereeGo&);
  GE_Goldeneye(const GE_RefereeGo&, const team_color&);
  
  ~GE_Goldeneye() {}
  
  int get_move(const GE_Goban&) const;
};

class GE_RandomGoPlayer : public GE_GoPlayer 
{
 public:
  GE_RandomGoPlayer();
  GE_RandomGoPlayer(const GE_RefereeGo&);
  GE_RandomGoPlayer(const GE_RefereeGo&, const team_color&);
  ~GE_RandomGoPlayer() {}
  
  int get_move(const GE_Goban&) const;
};

class GE_GoPlayerAnimal : public GE_GoPlayer
{
 public:
  GE_Game* game;

 public:
  GE_GoPlayerAnimal();
  GE_GoPlayerAnimal(GE_Game&, const GE_RefereeGo&, const team_color& = GE_BLACK);
  virtual ~GE_GoPlayerAnimal() {}

  virtual int get_moves(const GE_Goban&, list<int>&) const; 
  virtual int get_move(const GE_Goban&) const;
};

class GE_GoPlayerLion : public GE_GoPlayerAnimal
{
 public:
  GE_GoPlayerLion();
  GE_GoPlayerLion(GE_Game&, const GE_RefereeGo&, const team_color& = GE_BLACK);
  ~GE_GoPlayerLion() {}
  
  int get_moves(const GE_Goban&, list<int>&) const;
  int get_move(const GE_Goban&) const;
};

class GE_GoPlayerMonkey : public GE_GoPlayerAnimal
{
 public:
  GE_DbGamesGo* dgg;

  GE_GoPlayerMonkey();
  GE_GoPlayerMonkey(GE_Game&, GE_DbGamesGo&, const GE_RefereeGo&, 
		 const team_color& = GE_BLACK);
  ~GE_GoPlayerMonkey() {}
  
  int get_moves(const GE_Goban&, list<int>&) const;
  int get_move(const GE_Goban&) const;
};

class GE_GoPlayerGorilla : public GE_GoPlayerMonkey
{
 public:
  GE_DbGamesGo* dgg;
  const GE_Filter* filt;
  
  GE_GoPlayerGorilla();
  GE_GoPlayerGorilla(GE_Game&, GE_DbGamesGo&, const GE_RefereeGo&, 
		  const GE_Filter&, const team_color& = GE_BLACK);
  ~GE_GoPlayerGorilla() {}
  
  int get_moves(const GE_Goban&, list<int>&) const;
  int get_move(const GE_Goban&) const;
};

class GE_GoPlayerAnt : public GE_GoPlayerAnimal
{
 public:
  GE_DbGamesGo* dgg;

  GE_GoPlayerAnt();
  GE_GoPlayerAnt(GE_Game&, GE_DbGamesGo&, const GE_RefereeGo&, 
		 const team_color& = GE_BLACK);
  ~GE_GoPlayerAnt() {}
  
  int get_moves(const GE_Goban&, list<int>&) const;
  int get_move(const GE_Goban&) const;
};


class GE_GoPlayerSnake : public GE_GoPlayerAnimal
{
 public:
  GE_GoPlayerSnake();
  GE_GoPlayerSnake(GE_Game&, const GE_RefereeGo&, const team_color& = GE_BLACK);
  ~GE_GoPlayerSnake() {}
  
  int get_moves(const GE_Goban&, list<int>&) const;
  int get_move(const GE_Goban&) const;
};

class GE_GoPlayerBear : public GE_GoPlayerAnimal
{
 public:
  GE_GoPlayerBear();
  GE_GoPlayerBear(GE_Game&, const GE_RefereeGo&, const team_color& = GE_BLACK);
  ~GE_GoPlayerBear() {}
  
  int get_moves(const GE_Goban&, list<int>&) const;
  int get_move(const GE_Goban&) const;
};

class GE_GoPlayerSnail : public GE_GoPlayerAnimal
{
 public:
  GE_GoPlayerSnail();
  GE_GoPlayerSnail(GE_Game&, const GE_RefereeGo&, const team_color& = GE_BLACK);
  ~GE_GoPlayerSnail() {}
  
  int get_moves(const GE_Goban&, list<int>&) const;
  int get_move(const GE_Goban&) const;
};

class GE_GoPlayerFlea : public GE_GoPlayerAnimal
{
 public:
  GE_GoPlayerFlea();
  GE_GoPlayerFlea(GE_Game&, const GE_RefereeGo&, const team_color& = GE_BLACK);
  ~GE_GoPlayerFlea() {}
  
  int get_moves(const GE_Goban&, list<int>&) const;
  int get_move(const GE_Goban&) const;
};


class GE_GoPlayerOctopus : public GE_GoPlayerAnimal
{
  GE_GoPlayerAnimal** gp;
  int nb_players;
  int nb_max_move;
  bool mode_print;

 public:
  GE_GoPlayerOctopus();
  GE_GoPlayerOctopus(GE_Game&, const GE_RefereeGo&, 
		  const team_color&, list<GE_GoPlayerAnimal*>&, 
		  const bool = false);
  ~GE_GoPlayerOctopus();

  void init_sub_players(GE_Game&, const GE_RefereeGo&, team_color);
  
  int get_moves(const GE_Goban&, list<int>&) const;
  int get_move(const GE_Goban&) const;

  bool match(const int = 0, const int = 1);
  void tournament();
  
 private:
  void exchange(const int, const int);
  float evaluate_score(const GE_Goban&) const;
};


class GE_GoPlayerDolphin : public GE_GoPlayerAnimal
{
 public:
  float timer;
  mutable float my_komi;
  bool thinking_for_komi;

  GE_GoPlayerDolphin();
  GE_GoPlayerDolphin(GE_Game&, const GE_RefereeGo&, const team_color& = GE_BLACK, 
		  float = GE_DEFAULT_TIME_THINKING, bool =  false);
  ~GE_GoPlayerDolphin() {}
  
  virtual int get_moves(const GE_Goban&, list<int>&) const;
  virtual int get_move(const GE_Goban&) const;
};

class GE_GoPlayerDolphin2 : public GE_GoPlayerDolphin
{
 public:
  float timer;
  mutable float my_komi;
  bool thinking_for_komi;
  int max_simus;
  
  GE_GoPlayerDolphin2();
  GE_GoPlayerDolphin2(GE_Game&, const GE_RefereeGo&, const team_color& = GE_BLACK, 
		   int = GE_DEFAULT_MAX_SIMUS, float = GE_DEFAULT_TIME_THINKING, 
		   bool =  false);
  ~GE_GoPlayerDolphin2() {}
  
  int get_moves(const GE_Goban&, list<int>&) const;
  int get_move(const GE_Goban&) const;
};

class GE_GoPlayerDolphin3 : public GE_GoPlayerAnimal
{
 public:
  float timer;
  mutable float my_komi;
  bool thinking_for_komi;

  GE_GoPlayerDolphin3();
  GE_GoPlayerDolphin3(GE_Game&, const GE_RefereeGo&, const team_color& = GE_BLACK, 
		  float = GE_DEFAULT_TIME_THINKING, bool =  false);
  ~GE_GoPlayerDolphin3() {}
  
  virtual int get_moves(const GE_Goban&, list<int>&) const;
  virtual int get_move(const GE_Goban&) const;
};


class GE_GoPlayerHorse : public GE_GoPlayerAnimal
{
 public:
  int min_capture;
  float timer;
  float komi;
  int max_simulations;

  GE_GoPlayerHorse();
  GE_GoPlayerHorse(GE_Game&, const GE_RefereeGo&, 
		const team_color&, int = 4, float = GE_KOMI, 
		float = 1.f, int = 10000); 
  ~GE_GoPlayerHorse() {}
  
  virtual int get_moves(const GE_Goban&, list<int>&) const;
  virtual int get_move(const GE_Goban&) const;

  virtual void set_komi(float km) { komi = km; }
};

class GE_GoPlayerSquirrel : public GE_GoPlayerAnimal
{
 public:
  int min_capture;
  float timer;
  float komi;
  mutable int max_simulations;
  int max_simulations_by_node;
  double coeff_exploration;

  GE_GoPlayerSquirrel();
  GE_GoPlayerSquirrel(GE_Game&, const GE_RefereeGo&, 
		const team_color&, int = 4, float = GE_KOMI, 
		   float = 1.f, int = 10000, int = 1, double = 1); 
  ~GE_GoPlayerSquirrel() {}
  
  bool pass(bool* = 0) const;
  virtual int get_moves(const GE_Goban&, list<int>&) const;
  virtual int get_move(const GE_Goban&) const;

  virtual void set_komi(float km) { komi = km; }
};



class GE_GoPlayerSquirrel2 : public GE_GoPlayerAnimal
{
 public:
  int min_capture;
  float timer;
  float komi;
  mutable int max_simulations;
  int max_simulations_by_node;
  double coeff_exploration;

  GE_GoPlayerSquirrel2();
  GE_GoPlayerSquirrel2(GE_Game&, const GE_RefereeGo&, 
		const team_color&, int = 4, float = GE_KOMI, 
		   float = 1.f, int = 10000, int = 1, double = 1); 
  ~GE_GoPlayerSquirrel2() {}
  
  bool pass(bool* = 0) const;
  virtual int get_moves(const GE_Goban&, list<int>&) const;
  virtual int get_move(const GE_Goban&) const;

  virtual void set_komi(float km) { komi = km; }
};






#endif

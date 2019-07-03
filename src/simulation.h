#ifndef GE_SIMULATION_H
#define GE_SIMULATION_H

#include "game_go.h"
#include "tools_ge.h"
#include "const_rule.h"
#include "amaf.h"
#include "amaf2.h"
#include "mc_locations_informations.h"

class GE_Simulation
{
 private:
  static int num_move_beginning;
  static bool is_end;
  static int winner;
  static int points;
  static float komi;
  
  static bool update_komi;
  static bool printing;
  static list<int> free_intersections;
  
  static int evaluate(GE_Game&, float = GE_KOMI);
  static void fill_free_intersections(const GE_Game&);
  static void update_free_intersections(const GE_Game&);

 public:
 

  GE_Simulation() {}
  ~GE_Simulation() {}
 
  inline static bool update_komi_on();
  inline static bool update_komi_off();
  inline static void reset_update_komi(bool);
  inline static void printing_on();
  inline static void printing_off();
  
  
  static int choice(GE_Game&, const int);
  static float choice_komi(GE_Game&, const float* = 0, bool = false, bool = false);
  static int launch(GE_Game&);
  static void update_amaf(GE_Game&, GE_Amaf&);
  static void update_amaf(GE_Game&, GE_Amaf&, int);
  static void update_amaf(GE_Game&, GE_Amaf2&, int = 0);
  static void update_mc_informations(GE_Game&, GE_MCLocationsInformations&, int = 0);
};

bool GE_Simulation::update_komi_on()
{
  bool was_updated = GE_Simulation::update_komi;
  GE_Simulation::update_komi = true;
  return was_updated;
}

bool GE_Simulation::update_komi_off()
{
  bool was_updated = GE_Simulation::update_komi;
  GE_Simulation::update_komi = false;
  return was_updated;
}

void GE_Simulation::reset_update_komi(bool old_value)
{
  GE_Simulation::update_komi = old_value;
}



void GE_Simulation::printing_on()
{
  GE_Simulation::printing = true;
}

void GE_Simulation::printing_off()
{
  GE_Simulation::printing = false;
}





#endif

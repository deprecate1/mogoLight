#ifndef GE_AMAF_TREE_H
#define GE_AMAF_TREE_H

#include <list>
#include "amaf.h"
#include "simulation.h"

struct GE_AmafTree
{
  list<int> last_sequence;
  GE_Amaf* am;
  int* first_place;

  
  GE_AmafTree();
  GE_AmafTree(int);
  ~GE_AmafTree();
  
  void build_sequence(GE_Game&, bool = false);
  void build_tree(GE_Game&, bool = false, int = 0);
  inline int get_best_move(); 

  private:
  list<int> new_sequence;
  bool is_back;
  int best_move;
  int max_first_place;
};


int GE_AmafTree::get_best_move()
{
  return best_move;
}


#endif

#ifndef GE_FILTER_H
#define GE_FILTER_H

#include "tools_ge.h"
#include "const_filter.h"
#include "const_goban.h"
#include "database_games_go.h"
#include "statistic.h"

class GE_Filter
{
 public:
  float** filter;
  int height;
  int width;
  
  GE_Filter();
  GE_Filter(int, int = -1, int = -1);
  ~GE_Filter();
  
  
};



#endif

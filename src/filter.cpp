#include "filter.h"

GE_Filter::GE_Filter()
{
  filter = 0;
  height = 0;
  width = 0;
}


GE_Filter::GE_Filter(int h, int w, int mode)
{
  height = h;
  width = w;
  if(w == -1)
    width = h;
  
  GE_Allocator::allocate(&filter, height, width);
  
  switch(mode)
    {
    case GE_MODE_STAT_DISTANCE1 :
      {
	GE_DbGamesGo dgg;
	//assert(height==GE_DEFAULT_SIZE_GOBAN);
	//assert(width==GE_DEFAULT_SIZE_GOBAN);
	GE_Statistic::stat_distances(dgg, &filter, false, height, width);
	break;
      }
      
    default : 
      for(int i = 0; i<height; i++)
	for(int j = 0; j<width; j++)
	  filter[i][j] = 1.f;
    }
  
}


GE_Filter::~GE_Filter()
{
  GE_Allocator::desallocate(&filter, height);
}
  


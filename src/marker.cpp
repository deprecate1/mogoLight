#include "marker.h"

GE_Marker::GE_Marker(int sz)
{
  num_treatment = 2;
  seen.resize(sz, num_treatment);
  max_treatment = 1000000000;
  num_treatment++;
}

GE_Marker::GE_Marker(const GE_Marker& mk)
{
  num_treatment = mk.num_treatment;
  seen = mk.seen;
  max_treatment = mk.max_treatment;
}


bool GE_Marker::is_treated(int index) const
{
  return (seen[index]==num_treatment);
}


int GE_Marker::set_treated(int index) 
{
  int old_num = seen[index];
  seen[index] = num_treatment;
  return old_num;
}


int GE_Marker::set_no_treated(int index) 
{
  int old_num = seen[index];
  seen[index] = seen[index]-1;
  return old_num;
}



void GE_Marker::reinit(int sz)
{
  if(sz>=0)
    seen.resize(sz, 2);
  
  num_treatment = 2;
  
  for(unsigned int i = 0; i<seen.size(); i++)
    {
      seen[i] = num_treatment;     
    } 
  
  num_treatment++;
}


void GE_Marker::update_treatment()
{  
  if(num_treatment==max_treatment) 
    reinit();
  else num_treatment++;
}


int GE_Marker::memorize(int index, int value)
{
  assert(value>=0);
  
  int old_value = seen[index];
  seen[index] = -value;

  return old_value;
}

int GE_Marker::increment(int index)
{
  int old_value = seen[index];
  
  if(old_value>=0)
    seen[index] = -1;
  else seen[index]--;
  
  return old_value;
}

int GE_Marker::decrement(int index)
{
  int old_value = seen[index];
  
  if(old_value>=0)
    seen[index] = -1;
  else seen[index]++;
  
  return old_value;
}



int GE_Marker::get_memory(int index) const 
{
  if(seen[index]>=0)
    return 0;
  else return -seen[index];
}


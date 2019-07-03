#include "tools.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/************************
 * a powerful allocator *
 ************************/
bool Allocator::allocate(int*** mem, int height, int width, int col, 
			 const list<int>* l_size) 
{
  //this->desallocate ???

  if(l_size)
    {
      if(l_size->empty())
	return false;
      
      if(height<=0)
	desallocate(mem,l_size->size());
      else desallocate(mem,height);
      
      (*mem) = new int*[l_size->size()];
      
      list<int>::const_iterator i_l = l_size->begin();
      int i_loop = 0;
      
      while(i_l!=l_size->end())
	{
	  if(*i_l<=0)
	    {
	      (*mem)[i_loop] = 0;
	      i_l++;
	      i_loop++;
	      continue;
	    }
	  
	  (*mem)[i_loop] = new int[*i_l];
	  
	  i_l++;
	  i_loop++;
	}
    }
  
  if(col>=0)
    {
      if((*mem)[col]) delete[] (*mem)[col];
      (*mem)[col] = new int[width];
      return true;
    }
  
  if(height<=0)
    return false;
  
  (*mem) = new int*[height];
  
  if(width==0)
    return true;
  
  int w = (width>0) ? width : height;
  for(int i = 0; i<height; i++)
    (*mem)[i] = new int[w];

  return true;
}

/***************************
 * a powerful desallocator *
 ***************************/
bool Allocator::desallocate(int*** mem, int height, int col, bool is_allocated) 
{
  if(not mem) return false;
  if(not (*mem)) return false;


  if(col>0)
    {
      if((*mem)[col]) delete[] (*mem)[col];
      (*mem)[col] = 0;
      return true;
    }
  
  for(int i = 0; i<height; i++)
    {
      if(is_allocated)
	{
	  if((*mem)[i]) 
	    delete[] (*mem)[i];
	  
	  (*mem)[i] = 0;
	}
    }
  
  if(*mem) delete[] (*mem);
  (*mem) = 0;

  return true;
}


bool Allocator::init(int*** mem, int height, int width, int val,
		     const list<int>* l_size) 
{
  if(not allocate(mem,height,width,-1,l_size))
    return false;
  
  if(l_size)
    {
      if(l_size->empty())
	return false;
      
      list<int>::const_iterator i_l = l_size->begin();
      
      for(unsigned int i = 0; i<l_size->size(); i++)
	{
	  for(int j = 0; j<*i_l; j++)
	    (*mem)[i][j] = val;
	  
	  i_l++;
	}
      
      return true;
    }
  
  int w = (width>0) ? width : height;
  for(int i = 0; i<height; i++)
    for(int j = 0; j<w; j++)
      (*mem)[i][j] = val;
  
  return true;
}


bool Allocator::allocate(float**** mem, int depth, int height, int width) 
{
  (*mem) = new float**[depth];
  
  for(int i = 0; i<depth; i++)
    (*mem)[i] = new float*[height];
  
  for(int i = 0; i<depth; i++)
    for(int j = 0; j<height; j++)
      (*mem)[i][j] = new float[width];
  
  return true;
}


bool Allocator::desallocate(float**** mem, int depth, int height) 
{
  for(int i = 0; i<depth; i++)
    for(int j = 0; j<height; j++)
      if((*mem)[i][j])
	{
	  delete[] (*mem)[i][j];
	  (*mem)[i][j] = 0;
	}
  
  
  for(int i = 0; i<depth; i++)
    if((*mem)[i])
      {
	delete[] (*mem)[i];
	(*mem)[i] = 0;
      }
  
  if((*mem))
    {	
      delete[] (*mem); 
      (*mem) = 0;
    }
  
  return true;
}

bool Allocator::allocate(float*** mem, int height, int width) 
{
  (*mem) = new float*[height];
  
  for(int i = 0; i<height; i++)
    (*mem)[i] = new float[width];
  
  return true;
}


bool Allocator::desallocate(float*** mem, int height) 
{
  for(int i = 0; i<height; i++)
    if((*mem)[i])
      {
	delete[] (*mem)[i];
	(*mem)[i] = 0;
      }
  
  if((*mem))
    {	
      delete[] (*mem); 
      (*mem) = 0;
    }
  
  return true;
}








bool ListOperators::is_in(const list<int>& l, int x)
{
  list<int>::const_iterator i_l = l.begin();

  while(i_l!=l.end())
    {
      if(*i_l==x)
	return true;

      i_l++;
    }

  return false;
}

bool ListOperators::push_if_not(list<int>& l, int x) 
{
  if(not is_in(l,x))
    {
      l.push_back(x);
      return true;
    }

  return false;
}

bool ListOperators::unique(list<int>& l)
{
  list<int> copy = l;
  l.clear();
  
  while(not copy.empty())
    {
      int temp = copy.front();
      copy.remove(temp);
      l.push_back(temp);
    }
  
  return true;
}

void ListOperators::erase(list<int>& l, int x) 
{
  list<int>::iterator i_l = l.begin();
  
  while(i_l!=l.end())
    {
      if(*i_l==x)
	i_l = l.erase(i_l);
      else i_l++;
    }
}

bool ListOperators::intersection(const list<int>& l1, const list<int>& l2, 
				 list<int>& inter) 
{
  bool not_empty = false;
  
  list<int>::const_iterator i_l1 = l1.begin();
  while(i_l1!=l1.end())
    {
      if(is_in(l2,*i_l1))
	{
	  inter.push_back(*i_l1);
	  not_empty = true;
	}
      i_l1++;
    }

  return not_empty;
}


void ListOperators::sort(list<int>& l)
{
  l.sort();
}

int ListOperators::random(const list<int>& l) 
{
  if(l.empty())
    {
      cout<<"*** ERROR *** ListOperators::random(list<int>&)"<<endl;
      cout<<"    an empty list is not permitted for this operation"<<endl;
      exit(-1);
    }
  
  int n = (int)l.size();
  int choice = rand()%n;

  list<int>::const_iterator i_l = l.begin();
  
  while(choice>0)
    {
      i_l++;
      choice--;
    }
  
  return *i_l; 
}

int ListOperators::random(list<int>& l, list<int>::iterator& ii) 
{
  if(l.empty())
    {
      cout<<"*** ERROR *** ListOperators::random(list<int>&, list<int>::iterator&)";
      cout<<endl;
      cout<<"    an empty list is not allowed for this operation"<<endl;
      exit(-1);
    }
  
  int n = (int)l.size();
  int choice = rand()%n;

  list<int>::iterator i_l = l.begin();
  
  while(choice>0)
    {
      i_l++;
      choice--;
    }
  
  ii = i_l;
  
  return *i_l; 
}


int ListOperators::select(list<int>& l, 
			  int (&fct) (list<int>&, list<int>::iterator&))
{
  list<int>::iterator ii;
  
  int choice = fct(l,ii);
  if(ii!=l.end())
    ii = l.erase(ii);
  
  return choice;
}

void ListOperators::print(const list<int>& l) 
{
  list<int>::const_iterator i_l = l.begin();
  
  while(i_l!=l.end())
    {
      cout<<(*i_l)<<" ";
      i_l++;
    }
  
  cout<<endl;
}

void ListOperators::print(const list<int>& l, const string (&fct)(const int)) 
{
  list<int>::const_iterator i_l = l.begin();
  
  while(i_l!=l.end())
    {
      cout<<fct(*i_l)<<" ";
      i_l++;
    }
  
  cout<<endl;
}

void ListOperators::print(const list<int>& l, const string (&fct)(const int, const int), 
			  const int arg) 
{
  list<int>::const_iterator i_l = l.begin();
  
  while(i_l!=l.end())
    {
      cout<<fct(arg,*i_l)<<" ";
      i_l++;
    }
  
  cout<<endl;
}



list<int> ListOperators::get_one_coordinate(const list<pair<int,int> >& lp, int mode)
{
  list<int> res;
  
  list<pair<int,int> >::const_iterator i_p = lp.begin();
  while(i_p!=lp.end())
    {
      res.push_back((mode==2) ? i_p->second : i_p->first);
      i_p++;
    }
  
  
  return res;
}

#include "tools_ge.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/************************
 * a powerful allocator *
 ************************/
bool GE_Allocator::allocate(int*** mem, int height, int width, int col, 
			 const list<int>* l_size) 
{
  //this->desallocate ???

  if(l_size)
    {
      if(l_size->empty())
	return false;
      
      if(height<=0)
	desallocate(mem, l_size->size());
      else desallocate(mem, height);
      
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
bool GE_Allocator::desallocate(int*** mem, int height, int col, bool is_allocated) 
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


bool GE_Allocator::init(int*** mem, int height, int width, int val, 
		     const list<int>* l_size) 
{
  if(not allocate(mem, height, width, -1, l_size))
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


bool GE_Allocator::allocate(float**** mem, int depth, int height, int width) 
{
  (*mem) = new float**[depth];
  
  for(int i = 0; i<depth; i++)
    (*mem)[i] = new float*[height];
  
  for(int i = 0; i<depth; i++)
    for(int j = 0; j<height; j++)
      (*mem)[i][j] = new float[width];
  
  return true;
}


bool GE_Allocator::desallocate(float**** mem, int depth, int height) 
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

bool GE_Allocator::allocate(float*** mem, int height, int width) 
{
  (*mem) = new float*[height];
  
  for(int i = 0; i<height; i++)
    (*mem)[i] = new float[width];
  
  return true;
}


bool GE_Allocator::desallocate(float*** mem, int height) 
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








bool GE_ListOperators::is_in(const list<int>& l, int x)
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

bool GE_ListOperators::push_if_not(list<int>& l, int x) 
{
  if(not is_in(l, x))
    {
      l.push_back(x);
      return true;
    }

  return false;
}

bool GE_ListOperators::unique(list<int>& l)
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

bool GE_ListOperators::merge(const list<int>& l, list<int>& l_merge, 
			  bool no_repetition)
{
  if(no_repetition)
    {
      list<int>::const_iterator i_l = l.begin();
      while(i_l!=l.end())
	{
	  GE_ListOperators::push_if_not(l_merge, *i_l);
	  i_l++;
	}
    }
  else
    {
      list<int>::const_iterator i_l = l.begin();
      while(i_l!=l.end())
	{
	  l_merge.push_back(*i_l);
	  i_l++;
	}
    }
  
  return no_repetition;
}


void GE_ListOperators::erase(list<int>& l, int x) 
{
  list<int>::iterator i_l = l.begin();
  
  while(i_l!=l.end())
    {
      if(*i_l==x)
	i_l = l.erase(i_l);
      else i_l++;
    }
}

bool GE_ListOperators::intersection(const list<int>& l1, const list<int>& l2, 
				 list<int>& inter) 
{
  bool not_empty = false;
  
  list<int>::const_iterator i_l1 = l1.begin();
  while(i_l1!=l1.end())
    {
      if(is_in(l2, *i_l1))
	{
	  inter.push_back(*i_l1);
	  not_empty = true;
	}
      i_l1++;
    }

  return not_empty;
}


void GE_ListOperators::sort(list<int>& l)
{
  l.sort();
}

int GE_ListOperators::random(const list<int>& l) 
{
  if(l.empty())
    {
      cout<<"*** ERROR *** GE_ListOperators::random(list<int>&)"<<endl;
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

int GE_ListOperators::random(list<int>& l, list<int>::iterator& ii) 
{
  if(l.empty())
    {
      cout<<"*** ERROR *** GE_ListOperators::random(list<int>&, list<int>::iterator&)";
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


int GE_ListOperators::select(list<int>& l, 
			  int (&fct) (list<int>&, list<int>::iterator&), 
			  bool to_delete)
{
  list<int>::iterator ii;
  
  int choice = fct(l, ii);
  
  if(to_delete)
    if(ii!=l.end())
      ii = l.erase(ii);
  
  return choice;
}

void GE_ListOperators::print(const list<int>& l) 
{
  list<int>::const_iterator i_l = l.begin();
  
  while(i_l!=l.end())
    {
      cout<<(*i_l)<<" ";
      i_l++;
    }
  
  cout<<endl;
}

void GE_ListOperators::print(const list<int>& l, const string (&fct)(const int)) 
{
  list<int>::const_iterator i_l = l.begin();
  
  while(i_l!=l.end())
    {
      cout<<fct(*i_l)<<" ";
      i_l++;
    }
  
  cout<<endl;
}

void GE_ListOperators::print(const list<int>& l, const string (&fct)(const int, const int), 
			  const int arg) 
{
  list<int>::const_iterator i_l = l.begin();
  
  while(i_l!=l.end())
    {
      cout<<fct(arg, *i_l)<<" ";
      i_l++;
    }
  
  cout<<endl;
}


void GE_ListOperators::print_error(const list<int>& l) 
{
  list<int>::const_iterator i_l = l.begin();
  
  while(i_l!=l.end())
    {
      cerr<<(*i_l)<<" ";
      i_l++;
    }
  
  cerr<<endl;
}

void GE_ListOperators::print_error(const list<int>& l, const string (&fct)(const int)) 
{
  list<int>::const_iterator i_l = l.begin();
  
  while(i_l!=l.end())
    {
      cerr<<fct(*i_l)<<" ";
      i_l++;
    }
  
  cerr<<endl;
}

void GE_ListOperators::print_error(const list<int>& l, const string (&fct)(const int, const int), 
			  const int arg) 
{
  list<int>::const_iterator i_l = l.begin();
  
  while(i_l!=l.end())
    {
      cerr<<fct(arg, *i_l)<<" ";
      i_l++;
    }
  
  cerr<<endl;
}




list<int> GE_ListOperators::get_one_coordinate(const list<pair<int, int> >& lp, int mode)
{
  list<int> res;
  
  list<pair<int, int> >::const_iterator i_p = lp.begin();
  while(i_p!=lp.end())
    {
      res.push_back((mode==2) ? i_p->second : i_p->first);
      i_p++;
    }
  
  
  return res;
}









bool GE_VectorOperators::is_in(const vector<int>& v, int x)
{
  for(int index = 0; index<(int)v.size(); index++)
    if(v[index]==x)
      return true;
  
  return false;
}

bool GE_VectorOperators::push_if_not(vector<int>& v, int x) 
{
  if(not is_in(v, x))
    {
      v.push_back(x);
      return true;
    }
  
  return false;
}

void GE_VectorOperators::intersection(const vector<int>& v1, 
				   const vector<int>& v2, 
				   vector<int>& v)
{
  for(int i1 = 0; i1<(int)v1.size(); i1++)
    {
      if(is_in(v2, v1[i1]))
	v.push_back(v1[i1]);
    }
}


int GE_VectorOperators::random(const vector<int>& v) 
{
  if(v.empty())
    {
      cout<<"*** ERROR *** GE_VectorOperators::random(vector<int>&)"<<endl;
      cout<<"    an empty vector is not permitted for this operation"<<endl;
      exit(-1);
    }
  
  int n = (int)v.size();
  int choice = rand()%n;
  return v[choice]; 
}

int GE_VectorOperators::random(vector<int>& v, int& index) 
{
  if(v.empty())
    {
      cout<<"*** ERROR *** GE_VectorOperators::random(vector<int>&, vector<int>::iterator&)";
      cout<<endl;
      cout<<"    an empty vector is not allowed for this operation"<<endl;
      exit(-1);
    }
  
  int n = (int)v.size();
  int choice = rand()%n;
  index = choice;
  
  return v[choice]; 
}


int GE_VectorOperators::select(vector<int>& v, 
			    int (&fct) (vector<int>&, int&), bool to_delete)
{
  int index = 0;
  
  int choice = fct(v, index);
  
  if(to_delete)
    {
      v[index] = v[v.size()-1];
      v.pop_back();
    }
  return choice;
}




void GE_VectorOperators::print_error(const vector<int>& v) 
{
  for(int i = 0; i<(int)(v.size()); i++)
    cerr<<v[i]<<" ";
  
  cerr<<endl;
}

void GE_VectorOperators::print_error(const vector<int>& v, 
				  const string (&fct)(const int, const int), 
				  const int arg) 
{

  for(int i = 0; i<(int)(v.size()); i++)
    cerr<<fct(arg, v[i])<<" ";
  
  cerr<<endl;
}

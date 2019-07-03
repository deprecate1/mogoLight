#include "evaluation_score.h"
#include <stdlib.h>
#include <stdio.h>

float GE_EvaluationScore::evaluate(const GE_Game& g, const float komi)
{
  int score = 0;
  int& height = (g.goban)->height;
  int& width = (g.goban)->width; 
  for(int i = 0; i<height; i++)
    for(int j = 0; j<width; j++)
      {
	switch((g.goban)->board[i][j])
	  {
	  case GE_WHITE_STONE : 
	    score--;
	    break;
	    
	  case GE_BLACK_STONE : 
	    score++;
	    break;
	    
	  case GE_WITHOUT_STONE :
	    {
	      int color_stone = GE_WITHOUT_STONE;
	      if(GE_IS_IN_GOBAN(i-1, j, height, width))
		color_stone = (g.goban)->board[i-1][j];
	      else color_stone = (g.goban)->board[i+1][j];

	      if(color_stone==GE_WITHOUT_STONE)
		break;
	      
	      if((not (GE_IS_IN_GOBAN(i+1, j, height, width))
		  ||((g.goban)->board[i+1][j]==color_stone))
		 &&(not (GE_IS_IN_GOBAN(i, j+1, height, width))
		    ||((g.goban)->board[i][j+1]==color_stone))
		 &&(not (GE_IS_IN_GOBAN(i, j-1, height, width))
		    ||((g.goban)->board[i][j-1]==color_stone)))
		{
		  if(color_stone==GE_WHITE_STONE)
		    score--;
		  else score++;
		}
	    }
	    break;
	    
	  default:;
	  }
      }
  
  return score - komi; //the komi is in favour of white
}


int GE_EvaluationScore::evaluate_territory(const GE_Goban& goban, int* black_points, 
					int* white_points)
{
  bool black_allocated = false;
  bool white_allocated = false;
  if(not black_points)
    {
      black_points = new int;
      black_allocated = true;
    }
  
  if(not white_points)
    {
      white_points = new int;
      white_allocated = true;
    }

  *white_points = 0;
  *black_points = 0;

  GE_Goban without_stones(goban.height, goban.width);
  
  goban.get_empty_intersections(without_stones);
  GE_Chains chs(without_stones);

  list<GE_Chain>::const_iterator i_ch = chs.sets.begin();
  while(i_ch!=chs.sets.end())
    {
      if((i_ch->free_intersections).empty())
	{
	  //cout<<"*** ERROR *** int GE_EvaluationScore::evaluate_territory"<<endl;
	  //exit(-1);
	  //if the goban is empty
	  i_ch++;
	  continue;
	}
      
      int color_stone = goban.get_stone((i_ch->free_intersections).front());
      bool is_colored = true;

      list<int>::const_iterator ii = (i_ch->free_intersections).begin();
      while(ii!=(i_ch->free_intersections).end())
	{
	  if(goban.get_stone(*ii)!=color_stone)
	    {
	      is_colored = false;
	      break;
	    }
	  
	  ii++;
	}
      
      if(is_colored)
	{
	  switch(color_stone)
	    {
	    case GE_BLACK_STONE : (*black_points)+=i_ch->size(); break;
	    case GE_WHITE_STONE : (*white_points)+=i_ch->size(); break;
	    default : 
	      cout<<"*** ERROR *** int GE_EvaluationScore::evaluate_territory"<<endl;
	      exit(-1);
	    }
	}
      

      i_ch++;
    }

  int points = (*black_points) - (*white_points);

  if(black_allocated)
    {
      delete black_points;
      black_points = 0;
    }

  if(white_allocated)
    {
      delete white_points;
      white_points = 0;
    }

  return points;
}


int GE_EvaluationScore::evaluate_area(const GE_Goban& goban, int* black_points, 
				   int* white_points)
{
  bool black_allocated = false;
  bool white_allocated = false;
  if(not black_points)
    {
      black_points = new int;
      black_allocated = true;
    }
  
  if(not white_points)
    {
      white_points = new int;
      white_allocated = true;
    }

  (*black_points) = goban.count_stones(GE_BLACK);
  (*white_points) = goban.count_stones(GE_WHITE);

  int points = (*black_points) - (*white_points);

  if(black_allocated)
    {
      delete black_points;
      black_points = 0;
    }

  if(white_allocated)
    {
      delete white_points;
      white_points = 0;
    }

  return points;
}


float GE_EvaluationScore::evaluate_score2(const GE_Goban& goban, float komi, 
				       int* black_territories, 
				       int* white_territories, 
				       int* black_areas, 
				       int* white_areas)
{
  int area = GE_EvaluationScore::evaluate_area(goban, black_areas, white_areas);
  int territory = GE_EvaluationScore::evaluate_territory(goban, black_territories, 
						      white_territories);
  
  //cout<<area<<" "<<territory<<endl;

  return area + territory - komi;
}

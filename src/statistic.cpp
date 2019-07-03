#include "statistic.h"

void GE_Statistic::stat_moves(const GE_DbGamesGo& dgg, float**** stats, 
			   const bool mode_file, const int height, const int width)
{
  bool is_delete = false;
  int max_nb_moves = dgg.get_max_length_game(height, width);

  if(not stats)
    {
      is_delete = true;

      stats = new float***;
      (*stats) = new float**[max_nb_moves];
      
      for(int i = 0; i<max_nb_moves; i++)
	(*stats)[i] = new float*[height];
      
      for(int i = 0; i<max_nb_moves; i++)
	for(int j = 0; j<height; j++)
	  (*stats)[i][j] = new float[width];
    }
  
  //number of games with this numero of move.
  int* nb_games_with_num = new int[max_nb_moves];
  for(int i = 0; i<max_nb_moves; i++)
    nb_games_with_num[i] = 0;
  
  for(int i = 0; i<max_nb_moves; i++)
    for(int j = 0; j<height; j++)
      for(int k = 0; k<width; k++)
	(*stats)[i][j][k] = 0;
 
  list<GE_FileSGF>::const_iterator i_games = dgg.games.begin();
  
  while(i_games!=dgg.games.end())
    {
      if((i_games->size_goban!=height)
	 ||(i_games->size_goban!=width))
	{
	  i_games++;
	  continue;
	}

      int num_move = 0;

      list<int>::const_iterator j_game = (i_games->game).begin();
      while(j_game!=(i_games->game).end())
	{
	  if(*j_game==GE_PASS_MOVE)
	    { //this move is not considered
	      j_game++;
	      continue;
	    }

	  
	  pair<int, int> mv2 = int_to_pair(i_games->size_goban, *j_game);
	  
	  (*stats)[num_move][mv2.first][mv2.second]++; 
	  nb_games_with_num[num_move]++;
	  
	  num_move++;
	  j_game++;
	}
      
      i_games++;
    }
  

  for(int i = 0; i<max_nb_moves; i++)
    for(int j = 0; j<height; j++)
      for(int k = 0; k<width; k++)
	if(nb_games_with_num[i]!=0)
	  (*stats)[i][j][k] /= (float)nb_games_with_num[i];
  
  //save in file
  if(mode_file)
    {
      ofstream f_stats("GE_Statistic/stat_moves.txt");
      if (f_stats.is_open())
	{
	  for(int i = 0; i<max_nb_moves; i++)
	    {
	      f_stats<<"move n°"<<1+i<<endl;
	      
	      for(int j = 0; j<height; j++)
		{
		  for(int k = 0; k<width; k++)
		    f_stats<<truncf(10000*(*stats)[i][j][k])/100.f<<" ";
		  
		  f_stats<<endl;
		}
	      
	      f_stats<<endl;
	    }
	  
	  f_stats.close();
	}
    }

  //desallocation
  if(is_delete)
    {
      for(int i = 0; i<max_nb_moves; i++)
	for(int j = 0; j<height; j++)
	  if((*stats)[i][j])
	    {
	      delete[] (*stats)[i][j];
	      (*stats)[i][j] = 0;
	    }
		
      for(int i = 0; i<max_nb_moves; i++)
	if((*stats)[i])
	  {
	    delete[] (*stats)[i];
	    (*stats)[i] = 0;
	  }
      
      if(*stats)
	{	
	  delete[] (*stats); 
	  (*stats) = 0;
	}

      if(stats)
	{
	  delete stats;
	  stats = 0;
	} 
    }

  if(nb_games_with_num)
    {  
      delete[] nb_games_with_num;
      nb_games_with_num = 0; 
    }
}


void GE_Statistic::study_stat_moves(const GE_DbGamesGo& dgg, const int height, const int width)
{
  int max_nb_moves = dgg.get_max_length_game(height, width);
  float*** stats = new float**[max_nb_moves];
  
  for(int i = 0; i<max_nb_moves; i++)
    stats[i] = new float*[height];
  
  for(int i = 0; i<max_nb_moves; i++)
    for(int j = 0; j<height; j++)
      stats[i][j] = new float[width];

  GE_Statistic::stat_moves(dgg, &stats, false, height, width);

  for(int num_mov = 0; num_mov<max_nb_moves; num_mov++)
    {
      float max_stat = -1.f;
      bool is_alone = true;
      int mv = GE_PASS_MOVE;

      for(int i = 0; i<height; i++)
	for(int j = 0; j<width; j++)
	  {
	    if(stats[num_mov][i][j]==max_stat)
	      is_alone = false;
	    
	    if(stats[num_mov][i][j]>max_stat)
	      {
		is_alone = true;
		max_stat = stats[num_mov][i][j];
		mv = pair_to_int(height, make_pair(i, j));
	      }
	  }

      cout<<"move n°"<<1+num_mov<<". ";
      cout<<move_to_string(height, mv)<<" ("<<100*max_stat<<"%) --> ";
      if(is_alone) 
	cout<<"the only move"<<endl;
      else cout<<"an exemple among others"<<endl;
    }
  
  //desallocation
  for(int i = 0; i<max_nb_moves; i++)
    for(int j = 0; j<height; j++)
      if(stats[i][j])
	{
	  delete[] stats[i][j];
	  stats[i][j] = 0;
	}
  
  
  for(int i = 0; i<max_nb_moves; i++)
    if(stats[i])
      {
	delete[] stats[i];
	stats[i] = 0;
      }
  
  if(stats)
    {	
      delete[] stats; 
      stats = 0;
    }
}


void GE_Statistic::stat_distances(const GE_DbGamesGo& dgg, float*** stats, 
			       const bool mode_file, const int height, const int width)
{

  bool is_delete = false;
 
  if(not stats)
    {
      is_delete = true;
      
      stats = new float**;
      (*stats) = new float*[height];
      
      for(int i = 0; i<height; i++)
	(*stats)[i] = new float[width];  
    }

  int nb_dist = 0;
  
  for(int i = 0; i<height; i++)
    for(int j = 0; j<width; j++)
      (*stats)[i][j] = 0;
 
  list<GE_FileSGF>::const_iterator i_games = dgg.games.begin();

  while(i_games!=dgg.games.end())
    {
      if((i_games->size_goban!=height)
	 ||(i_games->size_goban!=width))
	{
	  i_games++;
	  continue;
	}
      
      list<int>::const_iterator j_game = (i_games->game).begin();
      while(j_game!=(i_games->game).end())
	{
	  if(*j_game==GE_PASS_MOVE)
	    { //this move is not considered
	      j_game++;
	      continue;
	    }

	  int mv1 = *j_game;
	  j_game++;
	  if(j_game==(i_games->game).end()) break;

	  int mv2 = *j_game;
	  if(mv2==GE_PASS_MOVE)
	    {
	      j_game++; // not an error 
	      continue; 
	    }

	  pair<int, int> dist = distance_goban(mv1, mv2, height);
	  (*stats)[dist.first][dist.second]++;
	  if(dist.first!=dist.second)
	    (*stats)[dist.second][dist.first]++;
	  nb_dist++;
	}
      
      i_games++;
    }
  
  for(int i = 0; i<height; i++)
    for(int j = 0; j<width; j++)
      (*stats)[i][j] /= (float)nb_dist;

    //save in file
  if(mode_file)
    {
      ofstream f_stats("GE_Statistic/stat_distance.txt");
      if (f_stats.is_open())
	{
	  for(int i = 0; i<height; i++)
		{
		  for(int j = 0; j<width; j++)
		    f_stats<<truncf(10000*(*stats)[i][j])/100.f<<" ";
		  
		  f_stats<<endl;
		}
	  
	  f_stats<<endl;
	}
      
      f_stats.close();
    }
  
    //desallocation
  if(is_delete)
    {	
      for(int i = 0; i<height; i++)
	if((*stats)[i])
	  {
	    delete[] (*stats)[i];
	    (*stats)[i] = 0;
	  }
      
      if(*stats)
	{	
	  delete[] (*stats); 
	  (*stats) = 0;
	}
      
      if(stats)
	{
	  delete stats;
	  stats = 0;
	} 
    } 
}

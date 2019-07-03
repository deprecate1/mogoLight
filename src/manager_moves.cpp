#include "manager_moves.h"

GE_ManagerMoves::GE_ManagerMoves(int h, int w)
{
  height = h;
  width = w;
  
  GE_Allocator::init(&eval_moves, height, width, 0);
}

GE_ManagerMoves::~GE_ManagerMoves()
{
  GE_Allocator::desallocate(&eval_moves, height);
}


void GE_ManagerMoves::illegal_moves(GE_Game& g)
{
  for(int mv = 0; mv<(g.goban)->size(); mv++)
    {
      if(not g.is_allowed(mv))
	{
	  pair<int, int> mv2 = int_to_pair(height, mv);  
	  eval_moves[mv2.first][mv2.second] = GE_ILLEGAL_MOVE;
	}
    }
}

void GE_ManagerMoves::avoid_border()
{
  for(int i = 0; i<height; i++)
    for(int j = 0; j<width; j++)
      {
	if(eval_moves[i][j]==GE_ILLEGAL_MOVE) continue;

	if(not GE_IS_IN_BORDER_GOBAN(i, j, height, width))
	  eval_moves[i][j] += GE_BONUS_NO_BORDER;
      }
}

void GE_ManagerMoves::avoid_corner()
{
  for(int i = 0; i<height; i++)
    for(int j = 0; j<width; j++)
      {
	if(eval_moves[i][j]==GE_ILLEGAL_MOVE) continue;

	if(not GE_IS_IN_CORNER_GOBAN(i, j, height, width))
	  eval_moves[i][j] += GE_BONUS_NO_CORNER;
      }
}

void GE_ManagerMoves::bonus_stat_moves(const GE_Game& g, const GE_DbGamesGo& dgg)
{
  assert((g.goban)->height==height);
  assert((g.goban)->width==width);

  int max_num_moves = dgg.get_max_length_game((g.goban)->height, (g.goban)->width);
  int num_move = (int)(g.game).size();

  //cout<<max_num_moves<<" "<<num_move<<endl;

  if(num_move>=max_num_moves) return;

  float*** stats;
  GE_Allocator::allocate(&stats, max_num_moves, height, width);   
  GE_Statistic::stat_moves(dgg, &stats, false, (g.goban)->height, (g.goban)->width);
  
  for(int i = 0; i<height; i++)
    for(int j = 0; j<width; j++)
      {
	if(eval_moves[i][j]==GE_ILLEGAL_MOVE) continue;
	
	eval_moves[i][j]+=(int)(GE_BONUS_STAT_MOVES*stats[num_move][i][j]);
      }
  
  GE_Allocator::desallocate(&stats, max_num_moves, height);
}

void GE_ManagerMoves::bonus_stat_distance(const GE_Game& g, const GE_DbGamesGo& dgg)
{
  assert((g.goban)->height==height);
  assert((g.goban)->width==width);

  if(g.game.empty()) return;
  
  const int last_move = g.game.back();
  if(last_move == GE_PASS_MOVE) return;
  
  pair<int, int> mv2 = int_to_pair(height, last_move);
  
  float** stats;
  GE_Allocator::allocate(&stats, height, width);   
  GE_Statistic::stat_distances(dgg, &stats, false, (g.goban)->height, (g.goban)->width);
  
  for(int i = 0; i<height; i++)
    for(int j = 0; j<width; j++)
      {
	pair<int, int> dist = distance_goban(mv2, make_pair(i, j));
	
	if(eval_moves[i][j]==GE_ILLEGAL_MOVE) continue;
	
	eval_moves[i][j]+=(int)(GE_BONUS_STAT_DISTANCE*stats[dist.first][dist.second]);
      }
  
  GE_Allocator::desallocate(&stats, height);
}

void GE_ManagerMoves::bonus_territories(GE_Game& g)
{
  for(int mv = 0; mv<(g.goban)->size(); mv++)
    {
      pair<int, int> mv2 = int_to_pair(height, mv);
      const int& i = mv2.first;
      const int& j = mv2.second;
      
    
      if(eval_moves[i][j]==GE_ILLEGAL_MOVE) continue;
      
      int col = g.tc;
      int ab = 0;
      int aw = 0;
      int diff = 0;
      GE_Strategy strat;
      g.play(mv);
      int area = strat.get_area(*(g.goban), col, &ab, &aw, &diff);
      g.backward();

      eval_moves[i][j] += (int)(GE_BONUS_TERRITORIES*area); 
    }
}


void GE_ManagerMoves::eval(GE_Game& g, const GE_DbGamesGo& dgg)
{
  this->illegal_moves(g);
  this->avoid_border();
  this->avoid_corner();
  this->bonus_stat_moves(g, dgg);
  this->bonus_stat_distance(g, dgg);
  this->bonus_territories(g);
}



void GE_ManagerMoves::print() const 
{
  cout<<"GE_ManagerMoves"<<endl;
  cout<<"height = "<<height;
  cout<<" && width = "<<width<<endl;
  cout<<"eval_moves:"<<endl;
  for(int i = 0; i<height; i++)
    {
      for(int j = 0; j<width; j++)
	{
	  cout<<eval_moves[i][j]<<" ";
	}
      
      cout<<endl;
    }
}

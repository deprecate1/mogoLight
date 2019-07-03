#include "block.h"

GE_Block::GE_Block()
{
  this->init();
}

GE_Block::GE_Block(const GE_Goban& gob, int i, int j, int block_mode, int block_size)
{
  this->init(&gob, i, j, block_mode, block_size);
}

GE_Block::GE_Block(const GE_Block& bl)
{
  this->copy(bl);
}

GE_Block& GE_Block::operator=(const GE_Block& bl)
{
  this->copy(bl);
  return *this;
}

void GE_Block::init(const GE_Goban* gob, int i, int j, int block_mode, int block_size)
{
  nb_w = 0;
  nb_b = 0;

  if(not gob)
    {
      nb_w = -1;
      nb_b = -1;
      color_stone = GE_WITHOUT_STONE;
      return;
    }

  pair<int, int> coord;

  if(j<0)
    {
      coord = int_to_pair(gob->height, i);
      territory.push_back(j);
    }
  else 
    {
      coord = make_pair(i, j);
      territory.push_back(pair_to_int(gob->height, coord));
    }
  
  switch(block_mode)
    {
    case GE_SQUARE_BLOCK :
      {
	int side_width = int(sqrt(block_size));
	for(int ii = coord.first; ii<coord.first+side_width; ii++)
	  for(int jj = coord.second; jj<coord.second+side_width; jj++)
	    {
	      switch((gob->board)[ii][jj])
		{
		case GE_WHITE_STONE : nb_w++; break;
		case GE_BLACK_STONE : nb_b++; break;
		default : ;
		}
	    }
      }
      break;
      
    default : ;
    }

  // TO REFINE
  /*
  if(nb_w+nb_b<side_width-1)
    {
      color_stone = GE_WITHOUT_STONE;
      return;
    }
  */

  if(nb_w>nb_b) color_stone = GE_WHITE_STONE;
  if(nb_w<nb_b) color_stone = GE_BLACK_STONE;
  if(nb_w==nb_b) color_stone = GE_WITHOUT_STONE;

}

void GE_Block::copy(const GE_Block& bl)
{
  territory = bl.territory;
  nb_w = bl.nb_w;
  nb_b = bl.nb_b;
  color_stone = bl.color_stone;
}



GE_Blocks::GE_Blocks()
{
  block_mode = GE_UNKNOWN;
  block_size = GE_UNKNOWN;
  level = GE_UNKNOWN;

  height = GE_UNKNOWN;
  width = GE_UNKNOWN;
}

GE_Blocks::GE_Blocks(const GE_Goban& gob, int block_mode, int block_size)
{
  this->init(gob, block_mode, block_size);
}

GE_Blocks::GE_Blocks(const GE_Blocks* bls)
{
  GE_Goban gob(false);
  blocksToGoban(*bls, bls->height, bls->width, gob);
  this->init(gob, bls->block_mode, bls->block_size);
  level=bls->level+1;
}

GE_Blocks::GE_Blocks(const GE_Blocks& bls)
{
  this->copy(bls);
}

GE_Blocks& GE_Blocks::operator=(const GE_Blocks& bls)
{
  this->copy(bls);
  return *this;
}


void GE_Blocks::copy(const GE_Blocks& bls)
{
  block_mode = bls.block_mode;
  block_size = bls.block_size;
  level = bls.level;
  blocks = bls.blocks;
  
  height = bls.height;
  width = bls.width;
}

bool GE_Blocks::init(const GE_Goban& gob, int block_mode, int block_size)
{
  if((gob.height==1)||(gob.width==1))
    return false;

  this->block_mode = block_mode;
  this->block_size = block_size;
  level = 1;
 
  //TO IMPLEMENT
  height = 0;
  width = 0;

  switch(block_mode)
    {
    case GE_SQUARE_BLOCK :
      {
	 int side_width = int(sqrt(block_size));
	 int step = side_width-1;
	 
	 for(int i = 0; i<gob.height; i+=step)
	   for(int j = 0; j<gob.width; j+=step)
	     {
	       if((i+step<gob.height)&&(j+step<gob.width))
		 {
		   blocks.push_back(GE_Block(gob, i, j, block_mode, block_size));
		   width++;
		 }
	     }

	 if(block_size==GE_DEFAULT_SQUARE_BLOCK_SIZE)
	   {
	     height = gob.height-1;
	     width = gob.width-1;
	   }
      }
      break;
    }


  if((gob.height<=0)||(gob.width<=0))
    {
      cout<<"*** ERROR GE_Blocks::init *** to implement"<<endl;
      cout<<"   ---> a method to calculate the attibutes height and width"<<endl;
      return false;
    }

  return true;
}



bool blocksToGoban(const GE_Blocks& bls, int height, int width, GE_Goban& gob)
{
  if((height<=0)||(width<=0))
    {
      cout<<"*** ERROR blocksToGoban *** height or width not defined"<<endl;
      return false;
    }
  
  gob.reallocate_goban(height, width);

  switch(bls.block_mode)
    {
    case GE_SQUARE_BLOCK:
      {
	list<GE_Block>::const_iterator i_b = bls.blocks.begin();
	int ii = 0;
	int jj = 0;
	while(i_b!=bls.blocks.end())
	  {
	    gob.board[ii][jj] = i_b->color_stone;
	    
	    i_b++;
	    jj++;

	    if(jj==width)
	      {
		jj = 0;
		ii++;
	      }
	  }
      }
      break;
      
    default : ;
    }
  
  return true;
}


bool blocksToGoban(const GE_Blocks& bls, GE_Goban& gob)
{
  if((bls.height<=0)||(bls.width<=0))
    {
      cout<<"*** ERROR blocksToGoban *** height or width not defined"<<endl;
      return false;
    }
  

  gob.reallocate_goban(bls.height, bls.width);

  switch(bls.block_mode)
    {
    case GE_SQUARE_BLOCK:
      {
	list<GE_Block>::const_iterator i_b = bls.blocks.begin();
	int ii = 0;
	int jj = 0;
	while(i_b!=bls.blocks.end())
	  {
	    gob.board[ii][jj] = i_b->color_stone;
	    
	    i_b++;
	    jj++;

	    if(jj==bls.width)
	      {
		jj = 0;
		ii++;
	      }
	  }
      }
      break;
      
    default : ;
    }
  
  return true;
}

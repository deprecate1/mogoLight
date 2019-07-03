#ifndef GE_BLOCK_H
#define GE_BLOCK_H

#include "goban.h"
#include "const_geography.h"
#include "const_block.h"
#include "param_block.h"

#include <cmath>

struct GE_Block
{
  list<int> territory;
  int nb_w;
  int nb_b;
  int color_stone;

  GE_Block();
  GE_Block(const GE_Goban&, int, int = -1, int = GE_SQUARE_BLOCK, int = GE_DEFAULT_SQUARE_BLOCK_SIZE);
  GE_Block(const GE_Block&);
  ~GE_Block() {}

  GE_Block& operator=(const GE_Block&);

  void init(const GE_Goban* = 0, int = -1, int = -1, int = GE_SQUARE_BLOCK, 
	    int = GE_DEFAULT_SQUARE_BLOCK_SIZE);
  void copy(const GE_Block&);
  
};

struct GE_Blocks
{
  int block_mode;
  int block_size;
  int level;
  list<GE_Block> blocks;

  int height;
  int width;

  GE_Blocks();
  GE_Blocks(const GE_Goban&, int = GE_SQUARE_BLOCK, int = GE_DEFAULT_SQUARE_BLOCK_SIZE);
  GE_Blocks(const GE_Blocks*);
  GE_Blocks(const GE_Blocks&);
  ~GE_Blocks() {}
  
  GE_Blocks& operator=(const GE_Blocks&);

  void copy(const GE_Blocks&);
  bool init(const GE_Goban&, int = GE_SQUARE_BLOCK, int = GE_DEFAULT_SQUARE_BLOCK_SIZE);
};

bool blocksToGoban(const GE_Blocks&, int, int, GE_Goban&);
bool blocksToGoban(const GE_Blocks&, GE_Goban&);



#endif

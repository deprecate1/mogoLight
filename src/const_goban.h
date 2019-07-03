#ifndef GE_CONST_GOBAN_H
#define GE_CONST_GOBAN_H

#define GE_BLACK_STONE -1
#define GE_WHITE_STONE 1
#define GE_WITHOUT_STONE 0
#define GE_EDGE 100000


#define GE_BLACK_STONE_POTENTIAL -2
#define GE_STRONG_BLACK_STONE_POTENTIAL -3
#define GE_WHITE_STONE_POTENTIAL 2
#define GE_STRONG_WHITE_STONE_POTENTIAL 3
#define GE_NEUTRAL_STONE 100000


#define GE_DEFAULT_SIZE_GOBAN 19
#define GE_WIDTH_GOBAN_NOT_DEFINED -1
#define GE_MAX_SIZE_GOBAN 19



#define GE_NEUTRAL_COLOR 0
#define GE_BLACK 1
#define GE_WHITE 2

#define GE_IS_COLOR(stone, col)					\
  (((col==GE_WHITE)&&((stone==GE_WHITE_STONE)				\
		   ||(stone==GE_WHITE_STONE_POTENTIAL)		\
		   ||(stone==GE_STRONG_WHITE_STONE_POTENTIAL)))	\
    ||((col==GE_BLACK)&&((stone==GE_BLACK_STONE)			\
		      ||(stone==GE_BLACK_STONE_POTENTIAL)		\
		      ||(stone==GE_STRONG_BLACK_STONE_POTENTIAL))))	
       

#define GE_COLOR_STONE_ENEMY(stone) (stone==GE_WHITE_STONE) ? GE_BLACK_STONE : GE_WHITE_STONE
  

typedef int team_color;

#define GE_IS_IN_GOBAN(i, j, height, width)		\
  ((i>=0)&&(i<height)&&(j>=0)&&(j<width))

#define GE_IS_IN_BORDER_GOBAN(i, j, height, width)	\
  ((i==0)||(j==0)||(i==height-1)||(j==width-1))

#define GE_IS_IN_CORNER_GOBAN(i, j, height, width)			\
  (((i==0)&&(j==0))||((i==0)&&(j==width-1))			\
   ||((i==height-1)&&(j==0))||((i==height-1)&&(j==width-1)))

#endif


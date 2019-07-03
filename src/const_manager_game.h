#ifndef GE_CONST_MANAGER_GAME_H
#define GE_CONST_MANAGER_GAME_H

#define GE_NEXT_PLAYER(col) \
  (col==GE_WHITE) ? col = GE_BLACK : col = GE_WHITE;

#define GE_NEXT_COLOR(col) \
  (col==GE_WHITE) ? GE_BLACK : GE_WHITE

#endif

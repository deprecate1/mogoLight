#ifndef GE_EVALUATION_SCORE_H
#define GE_EVALUATION_SCORE_H

#include "game_go.h"
#include "const_rule.h"
#include "chain.h"

struct GE_EvaluationScore
{

  GE_EvaluationScore() {}
  ~GE_EvaluationScore() {}
  static float evaluate(const GE_Game&, const float = GE_KOMI);
  static int evaluate_territory(const GE_Goban&, int* = 0, int* = 0);
  static int evaluate_area(const GE_Goban&, int* = 0, int* = 0);
  static float evaluate_score2(const GE_Goban& goban, const float = GE_KOMI, 
			       int* = 0, int* = 0, int* = 0, int* = 0);
};



#endif

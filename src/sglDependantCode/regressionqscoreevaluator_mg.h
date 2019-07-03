//
// C++ Interface: regressionqscoreevaluator_mg
//
// Description:
//
//
// Author: Yizao Wang and Sylvain Gelly <yizao.wang@polytechnique.edu and sylvain.gelly@m4x.org>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MOGOREGRESSIONQSCOREEVALUATOR_MG_H
#define MOGOREGRESSIONQSCOREEVALUATOR_MG_H

#include <qscoreevaluator_mg.h>
#include <vectorgelly.h>
#include <implicitlysharedpointersgl.h>
namespace sgL {
  class Regression;
}

namespace MoGo {

  /**
  Evaluate, using a regression, the score of a goban and a location (see QScoreEvaluator)
   
  @author Yizao Wang and Sylvain Gelly
  */
  class RegressionQScoreEvaluator : public QScoreEvaluator {
  public:
    RegressionQScoreEvaluator();

    virtual ~RegressionQScoreEvaluator();

    /** Evaluate a position with a location to play in a game.
      The evaluation is for the position and playing at "location" and positive iff the next player is expected to win. The evaluation is between -infinity and +infinity */
    virtual double evaluate(const Goban &goban, const Location location,double time=0.);
    /** Evaluate a position with a set of locations to play in a game.
      The evaluation is for the positions and playing at "location[i]" and positive iff the next player is expected to win. The evaluation is between -infinity and +infinity */
    virtual Vector<double> evaluate(const Goban &goban, const Vector<Location> &locations,double time=0.);

    static Vector<double> toVector(const Vector<double> &goban, int iOnGoban, int jOnGoban, int sizeGoban, int patternSize);
    static bool test(int argc, char **argv);

    static bool generateLearningValues(int argc, char **argv);

  protected:
    ISP<Regression> regression;
    int patternSize;
    bool loadRegression();
    static int getIndex(int i, int j, int sizeGoban);
    static void saveLearningResults(const char *name, const Goban &goban, const Location moveLocation, double score, int patternSize, bool append);

  };

}

#endif

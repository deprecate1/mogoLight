//
// C++ Interface: regressionscoreevaluator_mg
//
// Description:
//
//
// Author: Yizao Wang and Sylvain Gelly <yizao.wang@polytechnique.edu and sylvain.gelly@m4x.org>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MOGOREGRESSIONSCOREEVALUATOR_MG_H
#define MOGOREGRESSIONSCOREEVALUATOR_MG_H

#include <vectorgelly.h>
#include <implicitlysharedpointersgl.h>
#include <scoreevaluator_mg.h>

namespace sgL {
  class Regression;
}
namespace MoGo {

  /**
  evaluate the score of a goban using a regression (needs the sgLibrary)
   
  @author Yizao Wang and Sylvain Gelly
  */
  class RegressionScoreEvaluator : public ScoreEvaluator {
  public:
    RegressionScoreEvaluator();

    virtual ~RegressionScoreEvaluator();

    /** Evaluate a position in a game(thus goban must be given). Return an evaluation with a certainty.*/
    virtual double evaluate(const Goban &goban,double* certainty=0,double time=0.);

    static Vector<double> toVector(const Goban &goban);
    static bool test(int argc, char **argv);
    static bool loadRegression(ISP<Regression> &regression, const QString &name);
  protected:
    ISP<Regression> regression;
    bool loadRegression();

  };
}

#endif

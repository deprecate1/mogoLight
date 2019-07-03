/*
 *  Open BEAGLE
 *  Copyright (C) 2001-2005 by Christian Gagne and Marc Parizeau
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Contact:
 *  Laboratoire de Vision et Systemes Numeriques
 *  Departement de genie electrique et de genie informatique
 *  Universite Laval, Quebec, Canada, G1K 7P4
 *  http://vision.gel.ulaval.ca
 *
 */

/*!
 *  \file   SymbRegEvalOp.cpp
 *  \brief  Implementation of the class SymbRegEvalOp.
 *  \author Christian Gagne
 *  \author Marc Parizeau
 *  $Revision: 1.1.1.1 $
 *  $Date: 2007-10-15 12:36:50 $
 */

#include "beagle/GP.hpp"
#include "beagle/Beagle.hpp"
#include "SymbRegEvalOp.h"
#include "../geneticprogramming_mg.h"
#include "../cputimechronometer_mg.h"
#include "../gotools_mg.h"
#include <cmath>


#include <algorithm>
#include <string>


using namespace Beagle;
using namespace MoGo;

/*!
 *  \brief Construct a new symbolic regression evaluation operator.
 */
SymbRegEvalOp::SymbRegEvalOp(bool useTournament, int baseRandomMode, int nbRunsForHumanRandom, int nbRunsForTournament, double coefForHuman, int gobanSize) :
GP::EvaluationOp("SymbRegEvalOp") {
  this->useTournament=useTournament;
  this->baseRandomMode=baseRandomMode;
  this->nbRunsForHumanRandom=nbRunsForHumanRandom;
  this->nbRunsForTournament=nbRunsForTournament;
  this->coefForHuman=coefForHuman;
  this->gobanSize=gobanSize;
}


/*!
 *  \brief Evaluate the individual fitness for the symbolic regression problem.
 *  \param inIndividual Individual to evaluate.
 *  \param ioContext Evolutionary context.
 *  \return Handle to the fitness measure,
 */
Fitness::Handle SymbRegEvalOp::evaluate(GP::Individual& inIndividual, GP::Context& ioContext) {
  return new FitnessSimple(evaluate(inIndividual,ioContext,nbRunsForHumanRandom));
}
double SymbRegEvalOp::evaluate(GP::Individual& inIndividual, GP::Context& ioContext, int nbRunsForHumanRandom) {
  GeneticProgramming gp(baseRandomMode, nbRunsForHumanRandom, gobanSize,0);
  InterestingMoveFinderBeagle *finder=new InterestingMoveFinderBeagle(this, &inIndividual, &ioContext);
  static double evaluateTime=0.;CPUTimeChronometer c;
  static int iterations=0;iterations++;
  if ((iterations%1)==0) GoTools::print("iterations %i, time %f, time/iteration %f\n", iterations, evaluateTime, evaluateTime/double(iterations));
  double v=gp.evaluate(finder);
  evaluateTime+=c.getTime();
  delete finder;
  return v;
}

double SymbRegEvalOp::evaluate(Beagle::GP::Individual& inIndividual1, Beagle::GP::Individual& inIndividual2,
                               Beagle::GP::Context& ioContext, int nbMoy) {
  GeneticProgramming gp(baseRandomMode, nbRunsForHumanRandom, gobanSize,0);
  InterestingMoveFinderBeagle *finder1=new InterestingMoveFinderBeagle(this, &inIndividual1, &ioContext);
  InterestingMoveFinderBeagle *finder2=new InterestingMoveFinderBeagle(this, &inIndividual2, &ioContext);
  //   static double evaluateTime=0.;CPUTimeChronometer c;
  //   static int iterations=0;iterations++;
  //   if ((iterations%10)==0) GoTools::print("\niterations %i, time %f, time/iteration %f\n", iterations, evaluateTime, evaluateTime/double(iterations));
  double v=gp.evaluate(finder1, finder2, nbMoy);
  //   evaluateTime+=c.getTime();
  delete finder1;delete finder2;
  return v;
}


/*!
 *  \brief Apply the evaluation process on the invalid individuals of the deme.
 *  \param ioDeme Deme to process.
 *  \param ioContext Context of the evolution.
 */
void SymbRegEvalOp::operate(Deme& ioDeme, Context& ioContext) {
  //   Beagle::GP::EvaluationOp::operate(ioDeme, ioContext);
#ifndef NEVER_DEFINED
  Beagle_StackTraceBeginM();
  Beagle_LogTraceM(
    ioContext.getSystem().getLogger(),
    "evaluation", "Beagle::EvaluationOp",
    std::string("Evaluating the fitness of the individuals in the ")+
    uint2ordinal(ioContext.getDemeIndex()+1)+" deme"
  );

  Individual::Handle lOldIndividualHandle = ioContext.getIndividualHandle();
  unsigned int lOldIndividualIndex = ioContext.getIndividualIndex();

  prepareStats(ioDeme,ioContext);
  History::Handle lHistory = castHandleT<History>(ioContext.getSystem().haveComponent("History"));

  bool lNoEvaluations = true; // Used to flag whether this operator has done anything
  Vector<Vector<double> > resultsTournament(ioDeme.size());
  for(unsigned int i=0; i<ioDeme.size(); i++)
    resultsTournament[i].resize(ioDeme.size());
  for(unsigned int i=0; i<ioDeme.size(); i++) {
    double v=0.;
    ioContext.setIndividualIndex(i);
    ioContext.setIndividualHandle(ioDeme[i]);
    Beagle_LogVerboseM(
      ioContext.getSystem().getLogger(),
      "evaluation", "Beagle::EvaluationOp",
      std::string("Evaluating the fitness of the ")+uint2ordinal(i+1)+
      " individual"
    );
    static double evaluateTime=0.;CPUTimeChronometer c;
    static int iterations=0;iterations++;
        if ((iterations%1)==0) GoTools::print("iterationsForTournament %i, time %f, time/iteration %f\n", iterations, evaluateTime, evaluateTime/double(iterations));
    // GoTools::print("%i\n",nbRunsForTournament);
    if (nbRunsForTournament>0) {
      for(unsigned int j=i+1; j<ioDeme.size(); j++) {
        double tmp=evaluate(castObjectT<GP::Individual&>(*ioDeme[i]), castObjectT<GP::Individual&>(*ioDeme[j]), castObjectT<GP::Context&>(ioContext), nbRunsForTournament);
        v += tmp;
        // GoTools::print("tmp=%f\n", tmp);
        resultsTournament[i][j]=tmp;
        assert(tmp>=0.);assert(tmp<=1.);
      }
      for(unsigned int j=0; j<i; j++)
        v+=1.-resultsTournament[j][i];
      if (ioDeme.size()>1)
        v/= double(ioDeme.size()-1);
      else
        coefForHuman=1.;
      evaluateTime+=c.getTime();
    }
    //   GoTools::print("v before=%f\n", v);
    double human=evaluate(castObjectT<GP::Individual&>(*ioDeme[i]),
                          castObjectT<GP::Context&>(ioContext),
                          nbRunsForHumanRandom);
    double r=coefForHuman*human
             +(1.-coefForHuman)*v;
std::cout<<ioDeme[i]->serialize()<<std::endl;

 /*Beagle_LogDetailledM(
 ioContext.getSystem().getLogger(),
 "evaluation", "SymbRegEvalOp",
 std::string("The individual is: ")+
 ioDeme[i]->serialize()
 );*/
    GoTools::print("human=%f tournament=%f (fitness %f)\n", human, v, r);
    ioDeme[i]->setFitness(new FitnessSimple(r));
    ioDeme[i]->getFitness()->setValid();
    updateStats(1,ioContext);

    Beagle_LogVerboseM(
      ioContext.getSystem().getLogger(),
      "evaluation", "Beagle::EvaluationOp",
      std::string("The individual\'s fitness is: ")+
      ioDeme[i]->getFitness()->serialize()
    );
  }
  ioContext.setIndividualIndex(lOldIndividualIndex);
  ioContext.setIndividualHandle(lOldIndividualHandle);

  updateHallOfFameWithDeme(ioDeme,ioContext);

  Beagle_StackTraceEndM("void EvaluationOp::operate(Deme&,Context&)");
#else
  Beagle_StackTraceBeginM();
  Beagle_LogTraceM(
    ioContext.getSystem().getLogger(),
    "evaluation", "Beagle::EvaluationOp",
    std::string("Evaluating the fitness of the individuals in the ")+
    uint2ordinal(ioContext.getDemeIndex()+1)+" deme"
  );

  Individual::Handle lOldIndividualHandle = ioContext.getIndividualHandle();
  unsigned int lOldIndividualIndex = ioContext.getIndividualIndex();

  prepareStats(ioDeme,ioContext);
  History::Handle lHistory = castHandleT<History>(ioContext.getSystem().haveComponent("History"));

  bool lNoEvaluations = true; // Used to flag whether this operator has done anything

  for(unsigned int i=0; i<ioDeme.size(); i++) {
    if((ioDeme[i]->getFitness() == NULL) ||
        (ioDeme[i]->getFitness()->isValid() == false)) {

      Beagle_LogVerboseM(
        ioContext.getSystem().getLogger(),
        "evaluation", "Beagle::EvaluationOp",
        std::string("Evaluating the fitness of the ")+uint2ordinal(i+1)+
        " individual"
      );

      ioContext.setIndividualIndex(i);
      ioContext.setIndividualHandle(ioDeme[i]);

      ioDeme[i]->setFitness(GP::EvaluationOp::evaluate(*ioDeme[i], ioContext));
      ioDeme[i]->getFitness()->setValid();

      if(lHistory != NULL) {
        lHistory->trace(ioContext,
                        NULL,
                        ioDeme[i],
                        ioDeme[i]->getFitness(),
                        getName(),
                        "evaluation");
      }

      updateStats(1,ioContext);
      lNoEvaluations = false;

      Beagle_LogVerboseM(
        ioContext.getSystem().getLogger(),
        "evaluation", "Beagle::EvaluationOp",
        std::string("The individual\'s fitness is: ")+
        ioDeme[i]->getFitness()->serialize()
      );
    }
  }

  ioContext.setIndividualIndex(lOldIndividualIndex);
  ioContext.setIndividualHandle(lOldIndividualHandle);

  updateHallOfFameWithDeme(ioDeme,ioContext);

  // Warn if the operator has not evaluated anything
  if (lNoEvaluations) {
    Beagle_LogBasicM(
      ioContext.getSystem().getLogger(),
      "evaluation", "Beagle::EvaluationOp",
      std::string("Warning!  The '")+getName()+"' operator did not evaluate any individuals.  "+
      "Consider using the 'InvalidateFitnessOp' operator to force evaluation of every individual."
    );
  }

  Beagle_StackTraceEndM("void EvaluationOp::operate(Deme&,Context&)");
#endif
}
























/*!
 * \brief Post-initialize the operator by sampling the function to regress.
 * \param ioSystem System to use to sample.
 */
void SymbRegEvalOp::postInit(System& ioSystem) {
  GP::EvaluationOp::postInit(ioSystem);

  /*  for(unsigned int i=0; i<20; i++) {
      mX.push_back(ioSystem.getRandomizer().rollUniform(-1.0,1.0));
      mY.push_back(mX[i]*(mX[i]*(mX[i]*(mX[i]+1.0)+1.0)+1.0));
    }*/
}
bool SymbRegEvalOp::setAllValuesAndGetResult(const Vector<int> &neighborhood, Beagle::GP::Individual *inIndividual, GP::Context *ioContext) {
  assert(neighborhood.size()==24);
  setValue("V0", Int(neighborhood[0]), *ioContext);
  setValue("V1", Int(neighborhood[1]), *ioContext);
  setValue("V2", Int(neighborhood[2]), *ioContext);
  setValue("V3", Int(neighborhood[3]), *ioContext);
  setValue("V4", Int(neighborhood[4]), *ioContext);
  setValue("V5", Int(neighborhood[5]), *ioContext);
  setValue("V6", Int(neighborhood[6]), *ioContext);
  setValue("V7", Int(neighborhood[7]), *ioContext);
  setValue("V8", Int(neighborhood[8]), *ioContext);
  setValue("V9", Int(neighborhood[9]), *ioContext);
  setValue("V10", Int(neighborhood[10]), *ioContext);
  setValue("V11", Int(neighborhood[11]), *ioContext);
  setValue("V12", Int(neighborhood[12]), *ioContext);
  setValue("V13", Int(neighborhood[13]), *ioContext);
  setValue("V14", Int(neighborhood[14]), *ioContext);
  setValue("V15", Int(neighborhood[15]), *ioContext);
  setValue("V16", Int(neighborhood[16]), *ioContext);
  setValue("V17", Int(neighborhood[17]), *ioContext);
  setValue("V18", Int(neighborhood[18]), *ioContext);
  setValue("V19", Int(neighborhood[19]), *ioContext);
  setValue("V20", Int(neighborhood[20]), *ioContext);
  setValue("V21", Int(neighborhood[21]), *ioContext);
  setValue("V22", Int(neighborhood[22]), *ioContext);
  setValue("V23", Int(neighborhood[23]), *ioContext);
  Bool lResult;
  ioContext->setIndividualHandle(inIndividual);
  inIndividual->run(lResult, *ioContext);
  if (lResult)
    return true;
  else
    return false;
}
















// #include "beagle/GP.hpp"
// #include "EqualTo.hpp"

#include <cmath>
#include <fstream>

using namespace Beagle;


/*!
 *  \brief Construct a new EqualTo primitive.
 */
EqualTo::EqualTo() :
Beagle::GP::Primitive(2, "EQ") { }


#ifdef BEAGLE_HAVE_RTTI

/*!
 *  \brief Return the Double tag needed as input for the primitive EqualTo.
 *  \param inN Index of the argument to get the tag.
 *  \param ioContext Evolutionary context.
 *  \return RTTI type_info of the data type Double.
 *  \throw AssertException If the index inN given is > to 1.
 */
const std::type_info* EqualTo::getArgType(unsigned int inN, Beagle::GP::Context& ioContext) const {
  Beagle_AssertM(inN < 2);
  return &typeid(Int);
}


/*!
 *  \brief Return the string tagging the Bool type returned but the primitive EqualTo.
 *  \param ioContext Evolutionary context.
 *  \return RTTI type_info of the Bool type returned.
 */
const std::type_info* EqualTo::getReturnType(Beagle::GP::Context& ioContext) const {
  return &typeid(Bool);
}

#endif // BEAGLE_HAVE_RTTI


/*!
 *  \brief Test the equality between two Double, returning a Bool.
 *  \param outDatum Result of the test.
 *  \param ioContext Evolutionary context.
 */
void EqualTo::execute(GP::Datum& outDatum, GP::Context& ioContext) {
  Bool& lResult = castObjectT<Bool&>(outDatum);
  Int lArgs[2];
  getArguments(lArgs, sizeof(Int), ioContext);
  lResult = lArgs[0].getWrappedValue() == lArgs[1].getWrappedValue();
}














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
 *  \file   SymbRegEvalOp.hpp
 *  \brief  Definition of the type SymbRegEvalOp.
 *  \author Christian Gagne
 *  \author Marc Parizeau
 *  $Revision: 1.1.1.1 $
 *  $Date: 2007-10-15 12:36:50 $
 */

/*!
 *  \defgroup SymbReg Symbolic Regression Example
 *  \brief Symbolic regression (symbreg): A simple GP example with Open BEAGLE.
 *
 *  \par Objective
 *  Find a function of one independent variable and one dependent variable, in
 *  symbolic form, that fits a given sample of 20 \f$(x_i,y_i)\f$ data points,
 *  where the target function is the quadratic polynomial \f$x^4 + x^3 + x^2 + x\f$.
 *
 *  \par Terminal set
 *  - X (the independent variable)
 *  - PI
 *  - Ephemeral constants randomly generated in [-1,1]
 *
 *  \par Function set
 *  - +
 *  - -
 *  - *
 *  - /     (protected division)
 *  - SIN
 *  - COS
 *  - EXP
 *  - LOG   (protected logarithm)
 *
 *  \par Fitness cases
 *  The given sample of 20 data points \f$(x_i,y_i)\f$, randomly chosen within
 *  interval [-1,1].
 *
 *  \par Fitness
 *  \f$\frac{1.}{1.+RMSE}\f$ where RMSE is the Root Mean Square Error on the
 *  fitness cases.
 *
 *  \par Stopping criteria
 *  When the evolution reaches the maximum number of generations.
 *
 *  \par Reference
 *  John R. Koza, "Genetic Programming: On the Programming of Computers by Means
 *  of Natural Selection", MIT Press, 1992, pages 162-169.
 *
 */

#ifndef SymbRegEvalOp_hpp
#define SymbRegEvalOp_hpp

#include "beagle/GP.hpp"
#include <string>
#include <vector>
#include "../typedefs_mg.h"

/*!
 *  \class SymbRegEvalOp SymbRegEvalOp.hpp "SymbRegEvalOp.hpp"
 *  \brief The individual evaluation class operator for the problem of symbolic regression.
 *  \ingroup SymbReg
 */
class SymbRegEvalOp : public Beagle::GP::EvaluationOp {

public:

  //! SymbRegEvalOp allocator type.
  typedef Beagle::AllocatorT<SymbRegEvalOp,Beagle::GP::EvaluationOp::Alloc>
  Alloc;
  //!< SymbRegEvalOp handle type.
  typedef Beagle::PointerT<SymbRegEvalOp,Beagle::GP::EvaluationOp::Handle>
  Handle;
  //!< SymbRegEvalOp bag type.
  typedef Beagle::ContainerT<SymbRegEvalOp,Beagle::GP::EvaluationOp::Bag>
  Bag;

  explicit SymbRegEvalOp(bool useTournament, int baseRandomMode, int nbRunsForHumanRandom, int nbRunsForTournament, double coefForHuman, int gobanSize);

  virtual Beagle::Fitness::Handle evaluate(Beagle::GP::Individual& inIndividual,
      Beagle::GP::Context& ioContext);
  virtual double evaluate(Beagle::GP::Individual& inIndividual,
      Beagle::GP::Context& ioContext, int nbRunsForHumanRandom);
  virtual double evaluate(Beagle::GP::Individual& inIndividual1, Beagle::GP::Individual& inIndividual2,
      Beagle::GP::Context& ioContext, int nbMoy);
  virtual void operate(Beagle::Deme& ioDeme, Beagle::Context& ioContext);
  virtual void postInit(Beagle::System& ioSystem);

  bool setAllValuesAndGetResult(const Vector<int> &neighborhood, Beagle::GP::Individual *inIndividual, Beagle::GP::Context *ioContext);
protected:
private:
  bool useTournament;
  int baseRandomMode;
  int nbRunsForHumanRandom;
  int nbRunsForTournament;
  double coefForHuman;
  int gobanSize;

  //   std::vector<Beagle::Int> mX;

};











#include "beagle/GP.hpp"
#include <string>

#ifdef BEAGLE_HAVE_RTTI
#include <typeinfo>
#endif // BEAGLE_HAVE_RTTI


/*!
 *  \class EqualTo EqualTo.hpp "EqualTo.hpp"
 *  \brief EqualTo primitive that compare equality of two Double and return a Bool.
 *  \ingroup Spambase
 */
class EqualTo : public Beagle::GP::Primitive {

public:

  //! EqualTo allocator type.
  typedef Beagle::AllocatorT<EqualTo,Beagle::GP::Primitive::Alloc>
  Alloc;
  //!< EqualTo handle type.
  typedef Beagle::PointerT<EqualTo,Beagle::GP::Primitive::Handle>
  Handle;
  //!< EqualTo bag type.
  typedef Beagle::ContainerT<EqualTo,Beagle::GP::Primitive::Bag>
  Bag;

  explicit EqualTo();
  virtual ~EqualTo() { }

#ifdef BEAGLE_HAVE_RTTI
  virtual const std::type_info* getArgType(unsigned int inN, Beagle::GP::Context& ioContext) const;
  virtual const std::type_info* getReturnType(Beagle::GP::Context& ioContext) const;
#endif // BEAGLE_HAVE_RTTI

  virtual void                  execute(Beagle::GP::Datum& outDatum, Beagle::GP::Context& ioContext);

};




























#endif // SymbRegEvalOp_hpp

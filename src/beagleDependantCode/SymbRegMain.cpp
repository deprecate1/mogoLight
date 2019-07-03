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
 *  \file   SymbRegMain.cpp
 *  \brief  Implementation of the main routine for the symbolic regression problem.
 *  \author Christian Gagne
 *  \author Marc Parizeau
 *  $Revision: 1.1.1.1 $
 *  $Date: 2007-10-15 12:36:50 $
 */

#include "beagle/GP.hpp"
#include "SymbRegEvalOp.h"

#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <numeric>

using namespace std;
using namespace Beagle;


/*!
 *  \brief Main routine for the function symbolic regression problem.
 *  \param argc Number of arguments on the command-line.
 *  \param argv Arguments on the command-line.
 *  \return Return value of the program.
 *  \ingroup SymbReg
 */
/*int main(int argc, char *argv[]) {
  try {
    // 1: Build primitives.
    GP::PrimitiveSet::Handle lSet = new GP::PrimitiveSet(&typeid(Bool));
//     lSet->insert(new GP::Add);
//     lSet->insert(new GP::Subtract);
//     lSet->insert(new GP::Multiply);
//     lSet->insert(new GP::Divide);
     lSet->insert(new GP::Not);
     lSet->insert(new GP::Or);
     lSet->insert(new GP::And);
      lSet->insert(new EqualTo);
//      lSet->insert(new GP::Xor);
    lSet->insert(new GP::TokenT<Int>("V0"));
    let->insert(new GP::TokenT<Int>("V1"));
    lSet->insert(new GP::TokenT<Int>("V2"));
    lSet->insert(new GP::TokenT<Int>("V3"));
    lSet->insert(new GP::TokenT<Int>("V4"));
    lSet->insert(new GP::TokenT<Int>("V5"));
    lSet->insert(new GP::TokenT<Int>("V6"));
    lSet->insert(new GP::TokenT<Int>("V7"));
    lSet->insert(new GP::TokenT<Int>("Empty", Int(0)));
    lSet->insert(new GP::TokenT<Int>("SameColor", Int(1)));
    lSet->insert(new GP::TokenT<Int>("OtherColor", Int(2)));
    lSet->insert(new GP::TokenT<Int>("Outside", Int(3)));


    //lSet->insert(new GP::EphemeralDouble);
    //lSet->insert(new GP::TokenT<Double>("Pi", Double(M_PI)));
    // 2: Build a system.
    GP::System::Handle lSystem = new GP::System(lSet);
    // 3: Build evaluation operator.
    SymbRegEvalOp::Handle lEvalOp = new SymbRegEvalOp;
    // 4: Build an evolver and a vivarium.
    GP::Evolver::Handle lEvolver = new GP::Evolver(lEvalOp);
    GP::Vivarium::Handle lVivarium = new GP::Vivarium;
    // 5: Initialize and evolve the vivarium.
    lEvolver->initialize(lSystem, argc, argv);
    lEvolver->evolve(lVivarium);
  }
  catch(Exception& inException) {
    inException.terminate();
  }
  catch(exception& inException) {
    cerr << "Standard exception caught:" << endl;
    cerr << inException.what() << endl << flush;
    return 1;
  }
  catch(...) {
    cerr << "Unknown exception caught!" << endl << flush;
    return 1;
  }
  return 0;
}
*/

//
// C++ Implementation: main
//
// Description:
//
//
// Author: Yizao Wang <yizao.wang@polytechnique.edu> and Sylvain Gelly <gelly@lri.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//




#include "typedefs_mg.h"
#ifdef MPIMOGO

extern int MPI_NUMERO;
extern int MPI_NUMBER;

#include "mpi.h"
#endif
#include "goban_mg.h"
#include "gogame_mg.h"
#include "gobannode_mg.h"
#include "fastsmallvector_mg.h"
#include "randomgoplayer_mg.h"
#include <stdio.h>
#include "gogametree_mg.h"
#include "gtpengine_mg.h"
#include "cputimechronometer_mg.h"
#include "gotools_mg.h"
#include <ext/hash_map>
#include "goplayerfactory_mg.h"
#include "goexperiments_mg.h"
#include "fastboard_mg.h"
#ifdef USE_SGL
#include <chronometer.h>
#include <time.h>
#endif

#include <istream>
#include <sys/types.h>
#include <sys/timeb.h>
#include <time.h>

using namespace std;
using namespace MoGo;
#include <stack>


// TEMPORARY COMMENT: THIS FUNCTION IS USED
int main(int argc, char *argv[]) {
#ifdef MPIMOGO

MPI_Init(&argc, &argv);

MPI_Comm_size(MPI_COMM_WORLD, &MPI_NUMBER);

MPI_Comm_rank(MPI_COMM_WORLD, &MPI_NUMERO);
#endif

  if (argc==1) {/**assert(0);**/
    char **argvTmp=new char *[2]; argc=2;
    argvTmp[0]=argv[0];
    argvTmp[1]=new char[4];strcpy(argvTmp[1], "--19");
    argv=argvTmp; // Yet a memory leak, but few bytes only.
  }
#ifdef RELEASE
  if (argc>1) {/**assert(0);**/
    if ((strcmp(argv[1],"--9"))
        && (strcmp(argv[1],"--19")
            && (strcmp(argv[1],"--13")))) {/**assert(0);**/
        GoTools::print("\nThe first argument has to be either --9, --13 or --19. Please see http://www.lri.fr/~gelly/MoGo_Download.htm for instructions.\n\n");
          exit(1);
        }
      }
#endif

  
#ifdef WITH_LIBEGO_TEST
      MoGo::AllLibEgoCode::initStaticVariables();
#endif
  MoGo::FastBoard::initStaticVariables();

#ifdef PARALLELIZED
#ifdef UseQuickMemoryManagementForFastVector
  ThreadsManagement::clearNumThreads();
#endif
#endif

  GoExperiments::expandCommandLine(argc, argv); // usefull for having meta command line options.
  Goban::precomputePatterns();
  GoExperiments::initiateEnviroment(argc,argv);

  MoGo::GoTools::setOutputMode(STDOUT_MODE);

  // Lance l'interface GTP, tout se passe ici
  MoGo::GoExperiments::GTPEngineMode(argc,argv);

#ifdef MPIMOGO
MPI_Finalize();
#endif

  return EXIT_SUCCESS ;
}

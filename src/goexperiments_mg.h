//
// C++ Interface: goexperiments_mg
//
// Description:
//
//
// Author: Yizao Wang <yizao.wang@polytechnique.edu> and Sylvain Gelly <gelly@lri.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MOGOGOEXPERIMENTS_MG_H
#define MOGOGOEXPERIMENTS_MG_H

#include "typedefs_mg.h"
#ifdef MOGOMPI
#include <mpi.h>
#endif

#include <string> 
#include "goban_mg.h"


namespace MoGo {
  class GoPlayerFactory;
  class Goban;
  class Database3;
  class ZonesScoreEvaluator;
  class MoGoServer;
  class MoGoClient;
  class SmallSquarePatternsDB;
  /**
  Class containing some experiments for comparing the performance of several methods
   
  dispatchTests and createPlayerFactory are VERY useful.

  most of the parameters are loaded (from command line) in createPlayerFactory and initiateEnviroment.
   
  @author Yizao Wang
  */
  class GoExperiments {
  public:
    GoExperiments();

    ~GoExperiments();

    static bool playAgainstHumanInTextMode();
    static bool playAgainstGnugo(int argc, char **argv);
    static bool playTournament(int argc, char **argv, bool saveGames=true, std::string suffix="");
    static bool playAgainstGnugoOld(int argc, char **argv);
    static bool dispatchTests(int argc, char **argv);
    /** Look at the command line arguments and search for the string "argument" in the form --argument.
      If the string is found then a double value is search after and v takes this value. Return if success or not.
      v is modified only on a success.
    */
    static bool findArgValue(int argc, char **argv, const std::string &argument, double &v);
    /** Look at the command line arguments and search for the string "argument" in the form --argument.
      If the string is found then a int value is search after and v takes this value. Return if success or not.
      v is modified only on a success.
    */
    static bool findArgValue(int argc, char **argv, const std::string &argument, int &v);
    /** Look at the command line arguments and search for the string "argument" in the form --argument.
      If the string is found then a string value is search after and v takes this value. Return if success or not.
      s is modified only on a success.
    */
    static bool findArgValue(int argc, char **argv, const std::string &argument, std::string &s);
    /** Look at the command line arguments and search for the string "argument" in the form --argument.
      If the string is found then a string value is search after and we replace this value. Return if success or not.
      argv is modified only on a success.
    */
    static bool changeArgValue(int argc, char **argv, const std::string &argument, const std::string &s);
    static bool changeArgValue(std::string lookFor, int argc, char **argv, const std::string &argument, const std::string &s);
    static bool generateLearningValues(int argc, char **argv);
    static bool generatePositionValues(int argc, char **argv);
    static bool generateAllPositionValues(int argc, char **argv);
    static bool GTPEngineMode(int argc, char**argv);
    static bool testInner(int argc, char**argv);
    static bool generateOpening(int argc, char **argv);
    static bool generateOpening2(int argc, char **argv);
    static bool generateOpening3(int argc, char **argv);
    static bool generateOpening4(int argc, char **argv);
    static bool playMultipleTournaments(int argc, char **argv);
    static bool rotateDatabase(int argc, char **argv);
    static bool mergeDatabases(int argc, char **argv);
    static bool client(int argc, char **argv);
    static bool initiateEnviroment(int argc, char**argv);
    static bool debugMode;
    static bool rlTest(int argc, char**argv);
    static bool gpOptimization(int argc, char**argv);
    static bool probasOptimization(int argc, char**argv);
    static bool generateOpeningByLongTraining(int argc, char** argv);
    static bool generateOpeningByLongTraining2(int argc, char** argv);
    static bool compareRandomModes(int argc, char** argv);
    /*
      verify the input options
    */
    static bool verifyCommandLine( int & argc, char **& argv );
    static void expandCommandLine(int &argc, char **&argv);
    static MoGo::GoPlayerFactory *createPlayerFactory(int argc, char **argv,const std::string &name, std::string &infos);
    static void bigBoards(int argc, char **argv);
  protected:
    static void saveLearningResults(const char *name, const Goban &goban, double score, bool append);
    static Goban rotateGoban(const Goban &goban);
    static ZonesScoreEvaluator *createZonesScoreEvaluator(int argc, char **argv, int randomMode, int zoneMode);
    static void handleServerAndClientForDatabaseGeneration(bool isServer, const Vector<Goban> &positions, const Vector<Location> &moves, double value, MoGoServer *server, MoGoClient *client, Database3 &bootstrapDatabase, Database3 &database, int gobanSize);
    static void codeOpening(const Vector<Goban> &positions, const Vector<Location> &moves, double value, char *&msg, int &msgSize);
    static void decodeOpening(Vector<Goban> &positions, Vector<Location> &moves, double &value, const char *msg, int msgSize, int gobanSize);
    static void treatDatabaseResult(bool isServer, const Vector<Goban> &positions, const Vector<Location> &moves, double value, Database3 &bootstrapDatabase, Database3 &database);
    static void generateLocalDatabase(int &argc, char **&argv);
    static void statisticsOnGames(int argc, char **argv);
    static bool expand(int argc, char ** argv, const char *command, Vector<std::string> &expandedTmp);
    static std::vector<std::pair<std::string, std::string> > rlGoParameters(int argc, char **argv);
    static Vector<int> getIntTab(const std::string &prefix, int argc, char **argv, int maxSize);
    static void initAllPatternWeights(int argc, char **argv);
    static void treatSquarePatterns(const Goban &goban, Location location, SmallSquarePatternsDB &database);
  };

}

#endif

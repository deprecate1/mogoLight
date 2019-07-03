//
// C++ Interface: gtpengine_mg
//
// Description:
//
//
// Author: Yizao Wang <yizao.wang@polytechnique.edu> and Sylvain Gelly <gelly@lri.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef MOGOGTPENGINE_MG_H
#define MOGOGTPENGINE_MG_H

//#define MANUALREPORT


#include "goban_mg.h"
#include "goplayer_mg.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <string>
#include "gogame_mg.h"
namespace MoGo {

  /**
  GTP engine, in order to connect to other programs, and also GoGUI.

  some commands are only for the Gogui debug. they are really helpful.
   
  @author Yizao Wang
  */
  class GTPEngine {
  public:
    GTPEngine(GoPlayer* goPlayer, int playsAgainstHuman, double timeMultiplier, double timeDivider, double timeShift, double komi, double forgetKomiValue, int defaultParameterSize);
    ~GTPEngine();

#ifdef MANUALREPORT
    static double manualReport;
#endif
    bool start();
    void setEachMoveTime(const double time, int timeManagementMode, double maxBeginning, int numMoveBeginning);
    void setAllGameTime(const int time);
    void setSaveMode(bool saveMode);
    std::string createCommandList() const;
    void setThinkWhileOtherThinks(bool thinkOverOtherTime);
    static int undoMode;
    /** specialGTPMode used for generating opening database by GoGui. every time the mode is enabled by pass move and disabled by clear_goard/pass move.

    mode 0, normal GTP mode.
    mode 1, from the current situation, every move 'played' on the goban is saved as interesting move. we ignore the color of the played move.
    mode 2, from the current situation, every move "played' on the goban is saved as bad move. the color of the move ignored.
    mode 3, from the current situation, the sequence of moves played are saved as the good sequence.

    */
    static int specialGTPMode;
  private:
    GoPlayer* goPlayer;
    Goban playerGoban;
    int timeLeft;
    bool readCommand(char * command) const;
    bool executeCommand(char * command);
    void failureResponse() const;

    void initiateCommandList();
    void addCommand( const char *commandName);
    void deleteCommandList();
    bool isCommandKnown(char *commandName);
    Vector < char * > commandList;
    char delimiters[5];
    void setDelimiters( ) ;
    void playMoveOnGoban();
    /** In the GTP, genmove refers to generateMoveAndPlay in MoGo's code. Here the function is named according to GTP's convention. */
    void generateMove(int considerPassBeforeEnd);
    /** This refers to reg_genmove in GTP.*/
    void generateMoveOnly();
    Location changeToLocation( const char * token ) ;
    void changeFromLocation( const Location location, char * token) ;
    void readGobanSize();
    void readKomi();
    void readTimeLeft();
    static void send(const char *msg, bool isSuccessful=true);
    double timeAllowed;
    double timeEstimated(const Goban & goban, double timeAllowed);
    void returnFinalStatus();
    void readTimeSettings();
    void saveGame();
    void createNewGameSaver();
    bool saveMode;
    GoGame *gameSaver;
    double komi;
    bool isTheOtherPlaying;
    void continueTraining(double time);
    void continueTrainingNOMPI(double time);
    bool thinkOverOtherTime;
    bool keepTreeIfPossible;
    bool temporaryStopThinking;
    void undoOneMove();
    std::string dateOfGameStart;
    Vector <Goban> historyGoban;
    void showPreferedSequence();
    void showGoban();
    void showHistory();
    void showFreeLocation();
    void updateLocationList();
    void isAtariSavingMove();
    void isStillEatingMove();
    void setRandomMode();
    void showTerritory();
    void setGTPMode(int mode);
    void setOpeningGeneratingMode();

    bool openingGeneratingModeOn;
    void openingGeneratingModePassMoveRecieved();
    void openingGeneratingModeOneMoveRecieved(const Location location);
    void openingGeneratingMode1();
    void openingGeneratingMode2();
    void openingGeneratingMode3();
    void show_evaluation();
    void place_free_handicap();
    Vector<std::string> getHandicaps(int gobanSize, int stoneNumber);
    void set_free_handicap();
    Vector < Location > tmpMoves;

    Database3 tmpDatabase;
    std::string databaseName;
    int playsAgainstHuman;
    void setDatabaseName();
    double timeMultiplier;
    double timeDivider;

    void playWaitingMove(char *color);
    double originalResignThreshold;
    bool handicapAgainst;
    int currentHandicapCompensation;
    double timeShift;
    double forgetKomiValue;
    void handleVT();
    static void sendRaw(const char* msg, ...);
    int defaultParameterSize;
    int timeManagementMode;
    double maxBeginning;
    int numMoveBeginning;
  };

}

#endif

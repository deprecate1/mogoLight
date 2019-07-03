//
// C++ Interface: goplayer_mg
//
// Description:
//
//
// Author: Yizao Wang <yizao.wang@polytechnique.edu> and Sylvain Gelly <gelly@lri.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MOGOGOPLAYER_MG_H
#define MOGOGOPLAYER_MG_H

#include "typedefs_mg.h"
#include "goban_mg.h"
#include "openingdatabase_mg.h"

namespace MoGo {
  /**
  The abstract class for the go player. A go player may be a random one, a MC (UCT) one, or a gtp (gnugo) one. Several functions for the opening case are in this class, since these functions are general for any player (except gnugo).
   
  @author Yizao Wang, Sylvain Gelly
  */
  class GoPlayer {
    friend class GoExperiments;
  public:
    GoPlayer();
    virtual ~GoPlayer()=0;

    /** Given a situation of goban, generate a move within time limited.*/
    virtual Location generateOneMoveNOMPI(const Goban &/*goban*/, double /*time*/=60) {assert(0);return PASSMOVE;}
    virtual Location generateOneMove(const Goban &goban, double time=60)=0;
    /** Generate a move in a game.
     The player is assumed to play always the same color and the other move is given by "lastOtherPlayerMove".
     The goban is here only for convenience as it could be deduced from the past moves.
     The move given by the player is immediately played (please return only legal moves!!!).
    */
    virtual Location generateOneMove(const Location lastOtherPlayerMove, const Goban &goban, double time=60);
    /** Generate a move and play it on the goban.
     The player is assumed to play always the same color and the other move is given by "lastOtherPlayerMove".
     The goban is here only for convenience as it could be deduced from the past moves.
     The move given by the player is immediately played (please return only legal moves!!!).
    */
    virtual Location generateAndPlay(const Location lastOtherPlayerMove, Goban & goban, double time=60);
    /** restart a absolutely new game. for the opening case.*/
    virtual void restart();
    /** Start a game from goban */
    virtual void start(const Goban &goban, const Location lastOtherPlayerMove=-1);
    /** Tells to the player that the game is finished and gives the score */
    virtual void finish(double score);
    /** Return the status of the player */
    virtual void reset();
    /** Clone itself */
    virtual GoPlayer *clone() const=0;
    virtual void showName() const;
    virtual std::string getName() const=0;
    virtual double getScore() ;
    virtual void setGobanSize(int size);
    virtual void setKomi(const double komi);
    virtual bool hasResigned()const;
    virtual int getRandomMode() const;

    /** these two are for the superko. */
    virtual void addHistoryGoban( const Goban & goban );
    virtual void clearHistoryGoban();
    //     virtual void deleteFromHistoryGoban( const Goban & goban);
    void undoForHistory(const Goban & goban);
    bool openingMove(const Goban &goban, Location &location, double *prediction=0);
    /** assume that the database is in "opening" file or "opening2" */
    bool loadOpeningDatabase(/*bool newDatabase*/);
    bool loadOpeningDatabase(const std::string &fileName);
    bool addToOpeningDatabase(const std::string &fileName);

    static bool loadOpeningDatabase(const std::string &fileName, MoGoHashMap(GobanKey, int, hashGobanKey, equalGobanKey) &openingDatabase);
    static bool addToOpeningDatabase(const std::string &fileName, MoGoHashMap(GobanKey, int, hashGobanKey, equalGobanKey) &openingDatabase);

    virtual void showPreferedMoves( const Goban & goban, const int number );
    virtual void showPreferedSequence( const Goban & goban, const Location firstMove = -1, const int number = 3);
    virtual void setRandomModeByGTPEngine( const int mode, const Goban & goban);
    virtual bool firstMove(int mode, const Goban & goban, Location & location) const;
    /** this is just a test function which seems not to be very good.*/
    bool openingMoveForcedMode1(const Goban & goban, Location & location) const;



  protected:
    //     MoGoHashMap(GobanKey, int, hashGobanKey, equalGobanKey) openingDatabase;
    MoGoHashMap(GobanKey, int, hashGobanKey, equalGobanKey) historyGoban;
    Database3 openingDatabase;
    /** lastMoves.back() is the last move. */
    Vector < Location > lastMoves;
    Vector < Location > openingHoshiStone;
    static std::string databaseName;
  };




}

#endif

//
// C++ Interface: game_mg
//
// Description:
//
//
// Author: Yizao Wang <yizao.wang@polytechnique.edu> and Sylvain Gelly <gelly@lri.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MOGOGAME_MG_H
#define MOGOGAME_MG_H

#include "typedefs_mg.h"
#include "goban_mg.h"
#include <fstream>
#include <iostream>

namespace MoGo {

  /**
  The class for the game, who saves the whole move sequence and the result of one the game.
   
  @author Yizao Wang
  */

  /** GoGame class represents a game record/or registrates a game record. It has a goban to save the currentMove situation. */
  class GoGame {
  public:
    /** To create a empty game(record), the moves are to be added.*/
    GoGame(int size = 9);
    GoGame(const Goban &goban);
    ~GoGame();

    /** Create a game from a sgf file(name in *gameName).*/
    GoGame(const char* gameName);

    /** To create a game with the given move sequence and the game information*/
    GoGame(const Vector<Move> &moveList, const GoGameInfo &gameInformation);
    /** To return the state of goban after the moveNumber move is played.(When moveNumber is 0 then the return is an empty goban.*/
    Goban gobanState(const int moveNumber) const;
    /** To return the information of the game.*/
    GoGameInfo information() const;

    /** Go to the next goban situation and return it.FIXME*/
    Goban nextGobanState();
    /** Go to the moveNumber situation and return it.FIXME*/
    Goban goToGobanStateAtMove(const int moveNumber);
    /** Return the goban size.*/
    int gameSize() const;
    /** Return the move of moveNumber.*/
    Location move(const int moveNumber) const;
    /** Return the length of the game.*/
    int gameMoveNumber() const;
    /** For the PASSMOVE test only while read a SGF file.*/
    bool isNotOnGoban(int x, int y) const;
    /***/
    void showMoveSequence(int beginMove=0) const;

    bool saveGame( const std::string &gameName,const std::string &gameDiscription = std::string("") );

    void setScore(const double score) {this->score=score;}

    void addNextMove(const Location location);
    void undoMove();
    void setKomi(double komi);
    double getScore()const;
    int nextMoveNumber() const;
    void fromGoban(const Goban &goban);
    static bool saveGobanPosition(const Goban &goban, const std::string &gameName,const std::string &gameDiscription = std::string(""));
    void clear();
  private:
    /** To save the move sequence.*/
    Vector<Location> moveList;
    double komi;
    int currentMove;
    int size;
    double score;
    Goban goban;
    /** To save the information, like the score, handicap, etc*/
    GoGameInfo gameInformation;

    void readSGFFileHeadInformation( FILE *pFile );
    bool readMoveSequence( FILE *pFile);

  };


}

#endif
